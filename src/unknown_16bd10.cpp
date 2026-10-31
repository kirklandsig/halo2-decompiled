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
void function_3f660(transform4x3f const *matrix);
void function_1554b0(real elapsed);
void __stdcall function_16f280(real dt);

#include "slot_handler.h"
static __forceinline long real_to_long(real value);

static inline long camera_seconds_to_ticks_round(real seconds)
{
	return real_to_long((real)g_510c54->field_2_3 * seconds);
}

static inline void cross3f(vector3f const *a, vector3f const *b, vector3f *out)
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

// score 49 (was 49): retail keeps only start and deceleration in xmm registers and reads the
// peak (twice) and acceleration from their array homes (frame 0x1c); ours keeps all four in
// registers (frame 0x10). Three-arm first clamp 96; two separate result clamps 49 (same);
// last term as a separate += 55 (retail's add order and single-exit clamp tail appear);
// one 2x3 coefficient array 54 (retail's 0x1c frame and tail, but the reads stay in
// registers); reordered coefficient fills 55; start/deceleration as named locals 59;
// endpoint field reads in the sum 114 (the homes collapse); pointer-taking fill helper 63;
// coefficient fill after the clamps 120.
// retail keeps both arrays in its frame (sub esp, 0x1c)
// and reads rates[1], rates[2] and accelerations[0] back from it; ours keeps
// only rates[2] there, so the sum takes other registers, the b * b term
// moves up (the compiler reorders the sum: the source order of the terms
// does not matter) and the two clamp exits are not merged. Retail also
// tests ticks against the acceleration ticks before choosing ticks (an
// if/else-if/else gives that, 96). Tried: other term orders 49, squares in
// locals 49, b * (b * accelerations[1]) 99, a loop over the three segments
// 59-65 (unrolled, factored), aggregate initializers 49, accelerations
// zero-initialized 101, the arrays filled or declared in another order 49-50.
// Round 16 (lane 10): retail's three tick clamps are PIN(x, 0, bound) with the value and bound
// re-evaluated (the macro this file already defines further down); that form gives retail's integer
// code exactly, but the float preloads stay 2 bytes shorter and shift the listing: 96 (aligned
// 45 -> 37); with one 2x3 coefficient array 95 (0x1c frame); with the last term as a separate +=
// 90 (retail's add order and single-exit tail); with both 88 (aligned 33: only the coefficients'
// registers differ, retail spills both peak copies and the acceleration and keeps start and
// deceleration in xmm6/xmm7, ours spills one); the fills in retail's load order 88 (spills start
// instead); a, b, c declared first 88; aggregate initializers 90.
// Round 181: all coefficient field reads at uses 117; complete native profile snapshot 107; snapshot
// plus field reads 105; profile reference 49, plus field reads 117; reordered profile/ticks 49, plus
// field reads 117, plus native snapshot 107. Coefficient homes and 0x1c frame remain unresolved;
// original arrays and input binding restored.
// Round 93 (PIN clamps make the integer part exact; the coefficient spill choice is what is left):
// PIN + 2x3 array + last term as a separate += 88 (aligned 23); on it the second peak copied from the
// first element 114 (259 bytes); PIN + two arrays + separate += 90 (frame 0x10, only rates[2] homed);
// on it renamed arrays 90 (names do not move the spill), aggregate initializers 90, segment times in a
// float array summed in a loop 106 (unrolled and factored). On the 2x3 form, sum texts: per-segment
// terms 95, coefficient-first products 88, rates then accelerations 95, one statement per segment
// 105, one six-term sum 95, squares grouped t * (t * k) 86; the arrays in one local struct 88;
// peaks filled by a chained assignment 114 (both peaks one register, no frame). The preloads keep
// four coefficients in xmm4-7 in every form; retail spills the peaks and the acceleration.
// Round 85 (compile-only screens reproduce this function's float code; the written-order `+=` chain
// gives retail's add order and shared clamp tail, but all 720 `+=` orders, all 720 fill orders and the
// 2x3/3x2 arrays keep one coefficient spilled, retail three; fills after the clamps spill none):
// `+=` chain cc2, c2, aa0, a0, bb1, b1 50; written-order `+=` chain 55, with the 2x3 array 54, with
// PIN clamps 90, with both 88.
// Round 98 (retail's memory operands for rates[1], rates[2] and accelerations[0] look like array homes
// that were never scalar-replaced): PIN clamps into a long segment-ticks array summed in a three-pass
// loop 88 (aligned 82); the same with the casts at each use 88 (287 bytes); the two peak rates filled
// by a loop 100 (aligned 31; 271 bytes), the loop after the other fills 100; the three segment
// products summed in a loop over the arrays 106; PIN + two arrays + separate += 90 again. A
// compile-only screen of 34 array/fill/sum forms never kept those three in memory.
// Round 98, on the PIN form: tick counts in an integer array 95 (292 bytes); a three-segment loop
// (four product spellings) 88 each, VC unrolls it and factors (a * t + r) * t; const 2x3 aggregate 88;
// const two-array aggregates 90; segment times in a float array with the written-out sum 88.
// Round 80 (plain clamps): the zero acceleration assigned just before the sum 49; the fills in
// retail's load order (peaks, start, deceleration, acceleration, zero) 53, with the late zero 53;
// the sum as six += statements in retail's add order 55; the last two terms as separate += 53
// (272 bytes).
// Round 22 (lane 10): ours homes only rates[2] and keeps the accelerations out of the frame. Both
// arrays at function scope 49; the two peak elements swapped in the sum 52; the tick conversions as
// locals declared at the sum 83 (289 bytes); the peaks filled by a loop over segments 1..2 56; a named
// peak rate for both peak elements 112 (263 bytes).
// Round 42: arrays and tick locals at function scope 49 (identical); short tick locals 119; the
// conversions written in the sum 83; PIN + 2x3 with the segment times in a float array 88, with
// per-segment {rate, acceleration} pairs 88; compile-only, all 720 orders of the six 2x3 fills, a
// time array, a time loop and factored per-segment terms keep the same single spilled peak.
// Round 3 (lane 17): the final clamp as one nested ternary 49 (same code); an early return when not
// valid 100; a, b, c declared at their conversions 49; the sum in a named value, clamped, then
// assigned 49; a named hold offset and hold length 117 (254 bytes); a named deceleration start 106
// (262 bytes).
// DL201: native contiguous start/peak pair copied by memcpy/memmove, second peak from
// its original field/copied home 96/110/97/103; with complete conditional first count
// 95/105/50/100. Original arrays, observations and six-term expression
// restored; extra coefficient promotion and frame 0x10/0x1c remain.
// Round 11 (lane 2), on PIN clamps with the last term as a separate += (one-file LTCG harness: retail's
// integer code and single-exit tail, 92/92 instructions; only the coefficient homes differ, ours
// spilling rates[2] alone): a, b, c declared at their conversions, all six += statements, the
// result clamp as PIN: the same code; real: fills after the hold clamp 104 (268 bytes); fills after
// the first conversion 99 (271 bytes); the sum in three statements 101 (270 bytes); all three
// conversions after the clamps 87 (285 bytes); fills just before the sum 121 (253 bytes).
// Round 26 (lane 15, fast): each segment's two terms accumulated right after its tick conversion 125 (249
// bytes); const aggregate-initialized arrays 49 (same code); the rate terms and the acceleration terms as two
// named partial sums, ramp + distance or distance + ramp, 51/51; every product of the six-term sum
// parenthesised 49 (same code); the sum explicitly grouped left to right in the written order 55.
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
		real a, b, c;

		rates[0] = profile->start_rate;
		rates[1] = profile->peak_rate;
		rates[2] = profile->peak_rate;
		accelerations[0] = profile->acceleration;
		accelerations[1] = 0.0f;
		accelerations[2] = profile->deceleration;

		acceleration_ticks = ticks;
		if (acceleration_ticks < 0)
		{
			acceleration_ticks = 0;
		}
		else if (acceleration_ticks > profile->acceleration_ticks)
		{
			acceleration_ticks = profile->acceleration_ticks;
		}

		a = (real)acceleration_ticks;
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

		b = (real)hold_ticks;
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

		c = (real)deceleration_ticks;
		{
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
				cross3f(forward, up, left);
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

PRIVATE inline real game_ticks_to_seconds(long ticks)
{
	return (real)ticks * g_510c54->rate;
}

/* the camera director of scripted cameras: fills the observer's command */
// score 236 (was 236): frame 72 agrees; the camera mode and the object index are read into
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
						vector3f const *direction;

						command->object_matrix.forward = *g_4687a8;
						command->object_matrix.up = *g_4687b0;
						command->object_matrix.left = *g_4687ac;
						direction = &camera->point.left;
						plane.n = *direction;
						plane.d = dot3f((vector3f *)&command->object_matrix.position, &plane.n);
						length_squared = (plane.n.k * direction->k + plane.n.j * direction->j) + plane.n.i * direction->i;
						if (length_squared != 0.0f)
						{
							t = 0.0f - plane_distance_to_point(&plane, (point3f *)&camera->point.offset) / length_squared;
						}
						else
						{
							t = 0.0f;
						}
						command->object_matrix.position.x = direction->i * t + camera->point.offset.i;
						command->object_matrix.position.y = direction->j * t + camera->point.offset.j;
						command->object_matrix.position.z = direction->k * t + camera->point.offset.k;
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
			dword camera_object_index = camera->object_index;
			if (camera_object_index != NONE)
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
	if (g_510c50 && ((s_camera_field_of_view_flags *)g_510c50)->active)
	{
		real target = camera->field_of_view_target;
		if (target != 0.0f)
		{
			real duration = camera->field_of_view_duration;
			if (camera->field_of_view_time >= duration)
			{
				field_of_view = target;
			}
			else
			{
				real t = camera->field_of_view_time / duration;

				t = PIN(t, 0.0f, 1.0f);
				field_of_view = (target - camera->field_of_view_start) * t + camera->field_of_view_start;
			}
		}
	}
	command->field_of_view = field_of_view;
	command->unknown90 = 3;
	command->unknowna4 = 0.0f;
	function_172520(command);
}
