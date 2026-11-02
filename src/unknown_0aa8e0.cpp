// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0AA8E0.CPP: the writing of a scenario object name into a simulation
   event as its index in the scenario's sorted block (outside lane J's region;
   the event encodings of src/unknown_09a5e0.cpp call it, and inline the
   reading back) */

#include "unknown_11c920.h"
#include "globals.h"
#include "bitstream.h"

typedef long (__stdcall *t_bsearch_compare_function)(const void *, const void *, const void *);
long function_13ddd0(const void *key, const void *base, long count, long element_size, t_bsearch_compare_function compare, const void *context);
/* 0x122cf0, the comparison of two longs (unknown_122870.cpp) */
long __stdcall cache_tag_group_compare(void const *a, void const *b, void const *context);

struct s_object_name_scenario_view
{
	byte unknown000[0x3d8];
	long object_name_count;
	long *object_names;
};

// @retail 0xaa8e0
void scenario_object_name_encode(long object_name, s_bitstream *stream)
{
	long index = NONE;
	if (object_name != NONE)
	{
		long key = object_name;
		s_object_name_scenario_view *scenario = (s_object_name_scenario_view *)g_4e0350;
		index = NONE;
		if (scenario && scenario->object_name_count > 0)
			index = function_13ddd0(&key, scenario->object_names, scenario->object_name_count, sizeof(long), cache_tag_group_compare, 0);
	}
	stream_write_checked(stream, index + 1, 9);
}

#include "unknown_1946f0.h"
struct s_z_transform_state
{
 long identifier;
 vector3f position;
 vector3f up;
 vector3f forward;
 vector3f linear_velocity;
 vector3f angular_velocity;
};

__declspec(noinline) bool function_ab9f0(s_z_transform_state const *state);
void simulation_read_position(s_bitstream *stream, real *position, long bits);
void function_195070(s_bitstream *stream, vector3f *out, real lo, real hi, long bits);

// @retail 0xab960
bool function_ab960(s_bitstream *stream, s_z_transform_state *state)
{
 long index = function_1959c0(stream, 10);
 byte salt = (byte)function_1959c0(stream, 4);
 state->identifier = index | ((dword)salt << 28);
 simulation_read_position(stream, (real *)&state->position, 16);
 function_195240(stream, &state->up, &state->forward);
 function_195070(stream, &state->linear_velocity, 0.03f, 350.0f, 10);
 function_195070(stream, &state->angular_velocity, 0.03f, 30.0f, 8);
 bool result;
 if (function_ab9f0(state)) result = true;
 else result = false;
 return result;
}

#include <string.h>
struct s_z_blend_state
{
 real first;
 real second;
 long identifier;
 long slot;
};

PRIVATE inline real z_read_blend_fraction(s_bitstream *stream)
{
 long value = function_1959c0(stream, 4);
 real result;
 if (value == 0) result = 0.0f;
 else if (value >= 15) result = 1.0f;
 else result = ((15 - value) * 0.0f + value * 1.0f) * (1.0f / 15.0f);
 return result;
}

// @retail 0xab700
bool function_ab700(s_bitstream *stream, s_z_blend_state *state)
{
 memset(state, 0, sizeof(*state));
 bool result = true;
 state->identifier = NONE;
 state->slot = NONE;
 if (function_1957d0(stream))
 {
  state->first = z_read_blend_fraction(stream);
  state->second = z_read_blend_fraction(stream);
  long index = function_1959c0(stream, 10);
  byte salt = (byte)function_1959c0(stream, 4);
  state->identifier = index | ((dword)salt << 28);
  result = state->identifier != NONE;
  if (function_1957d0(stream))
   state->slot = function_1959c0(stream, 5);
 }
 return result;
}

PRIVATE inline void z_write_blend_fraction(s_bitstream *stream, real value)
{
 real scaled = value * 15.0f;
 long quantized;
 __asm
 {
  fld scaled
  fistp quantized
 }
 function_195720(stream, quantized, 4);
}

void simulation_write_entity_identifier(long identifier, s_bitstream *stream);
void function_194830(s_bitstream *stream, bool value);

// @retail 0xab5e0
void function_ab5e0(s_z_blend_state const *state, s_bitstream *stream)
{
 if ((state->first > 0.0f || state->second > 0.0f) && state->identifier != NONE)
 {
  stream_write_bit(stream, true);
  real first = state->first > 0.0f ? state->first : 0.0f;
  real second = state->second > 0.0f ? state->second : 0.0f;
  z_write_blend_fraction(stream, first);
  z_write_blend_fraction(stream, second);
  simulation_write_entity_identifier(state->identifier, stream);
  function_194830(stream, state->slot != NONE);
  if (state->slot != NONE)
   stream_write_checked(stream, state->slot, 5);
 }
 else
  stream_write_bit(stream, false);
}

void simulation_write_position(s_bitstream *stream, real const *position, long bits, bool keep_inside);
void function_194d30(s_bitstream *stream, vector3f const *up, vector3f const *forward);
void function_194c10(s_bitstream *stream, vector3f const *vector, real lo, real hi, long bits);
bool __stdcall function_a75d0(vector3f *vector, real maximum);

PRIVATE __forceinline void transform_angular_cap(vector3f *vector, real maximum)
{
 real squared = length_sq3f(vector);
 if (squared > maximum * maximum)
 {
  double scale = maximum / sqrt(squared);
  vector->i = (real)(scale * vector->i);
  vector->j = (real)(scale * vector->j);
  vector->k = (real)(scale * vector->k);
 }
}

// @retail 0xab7f0
void function_ab7f0(s_bitstream *stream, s_z_transform_state const *state)
{
 simulation_write_entity_identifier(state->identifier, stream);
 simulation_write_position(stream, (real const *)&state->position, 16, false);
 function_194d30(stream, &state->up, &state->forward);
 vector3f velocity = state->linear_velocity;
 {
  real first_squared = velocity.i * velocity.i;
  real second_squared = velocity.j * velocity.j;
  real length_squared = (first_squared + second_squared) + velocity.k * velocity.k;
  if (length_squared > 122500.0f)
  {
   real inverse = 350.0f / (real)sqrt(length_squared);
   velocity.i *= inverse;
   velocity.j *= inverse;
   velocity.k *= inverse;
  }
 }
 function_194c10(stream, &velocity, 0.03f, 350.0f, 10);
 velocity = state->angular_velocity;
 transform_angular_cap(&velocity, 30.0f);
 function_194c10(stream, &velocity, 0.03f, 30.0f, 8);
}


struct s_z_input_state
{
 real angles[2];
 real movement[2];
 word flags;
 byte unknown12[2];
 char first;
 char second;
 short third;
};
struct s_z_input_packet
{
 s_z_input_state input;
 bool fields18[5];
 s_z_blend_state blend;
 bool fields30[3];
};
bool function_ab510(s_z_input_state const *state);

PRIVATE inline real z_read_input_real(s_bitstream *stream, long bits, long maximum, real lo, real hi)
{
 long value = function_1959c0(stream, bits);
 real result;
 if (value == 0) result = lo;
 else if (value >= maximum) result = hi;
 else result = ((maximum - value) * lo + value * hi) * (1.0f / maximum);
 return result;
}

// @retail 0xab2f0
bool function_ab2f0(s_bitstream *stream, s_z_input_packet *state)
{
 memset(state, 0, sizeof(*state));
 bool result = true;
 state->blend.first = 0.0f;
 state->blend.second = 0.0f;
 state->blend.identifier = NONE;
 state->blend.slot = NONE;
 state->input.angles[0] = z_read_input_real(stream, 13, 8191, 0.0f, 6.2831854820251465f);
 state->input.angles[1] = z_read_input_real(stream, 12, 4095, -3.1415927410125732f, 3.1415927410125732f);
 state->input.movement[0] = z_read_input_real(stream, 5, 30, -1.0f, 1.0f);
 state->input.movement[1] = z_read_input_real(stream, 5, 30, -1.0f, 1.0f);
 if (function_1957d0(stream)) state->input.flags = (word)function_1959c0(stream, 4);
 *(short *)state->input.unknown12 = (short)(function_1959c0(stream, 5) - 1);
 state->input.first = (char)(function_1959c0(stream, 3) - 1);
 state->input.second = (char)(function_1959c0(stream, 3) - 1);
 state->input.third = (short)(function_1959c0(stream, 2) - 1);
 state->fields18[0] = function_1957d0(stream);
 state->fields18[1] = function_1957d0(stream);
 state->fields18[2] = function_1957d0(stream);
 state->fields18[3] = function_1957d0(stream);
 state->fields18[4] = function_1957d0(stream);
 if (state->fields18[4]) result = function_ab700(stream, &state->blend) != false;
 state->fields30[0] = function_1957d0(stream);
 state->fields30[1] = function_1957d0(stream);
 state->fields30[2] = function_1957d0(stream);
 if (result && function_ab510(&state->input)) return true;
 return false;
}


PRIVATE inline void z_write_input_real(s_bitstream *stream, real value, real lo, real multiplier, long bits)
{
 real scaled = (value - lo) * multiplier;
 long quantized;
 __asm
 {
  fld scaled
  fistp quantized
 }
 function_195720(stream, quantized, bits);
}

// @retail 0xaaef0
void function_aaef0(s_bitstream *stream, s_z_input_packet const *state)
{
 z_write_input_real(stream, state->input.angles[0], 0.0f, 1303.6380615234375f, 13);
 z_write_input_real(stream, state->input.angles[1], -3.1415927410125732f, 651.739501953125f, 12);
 z_write_input_real(stream, state->input.movement[0], -1.0f, 15.0f, 5);
 z_write_input_real(stream, state->input.movement[1], -1.0f, 15.0f, 5);
 stream_write_bit(stream, state->input.flags != 0);
 if (state->input.flags) stream_write_checked(stream, state->input.flags, 4);
 stream_write_checked(stream, *(short const *)state->input.unknown12 + 1, 5);
 stream_write_checked(stream, state->input.first + 1, 3);
 stream_write_checked(stream, state->input.second + 1, 3);
 stream_write_checked(stream, state->input.third + 1, 2);
 stream_write_bit(stream, state->fields18[0]);
 stream_write_bit(stream, state->fields18[1]);
 stream_write_bit(stream, state->fields18[2]);
 stream_write_bit(stream, state->fields18[3]);
 stream_write_bit(stream, state->fields18[4]);
 if (state->fields18[4]) function_ab5e0(&state->blend, stream);
 stream_write_bit(stream, state->fields30[0]);
 stream_write_bit(stream, state->fields30[1]);
 stream_write_bit(stream, state->fields30[2]);
}


#include <math.h>
#include "vector_products.h"
transform4x3f *function_ba160(long object_index, transform4x3f *matrix);
void function_1420f0(transform4x3f *out, point3f const *position, vector3f const *forward, vector3f const *up);
void function_141590(transform4x3f const *in, transform4x3f *out);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
quaternionf *function_141f60(matrix3x3 const *matrix, quaternionf *out);
void function_11d790(quaternionf const *q, real *angle, vector3f *axis);

PRIVATE inline void z_limit_correction(vector3f *value, real maximum)
{
 real squared = value->k * value->k + value->j * value->j + value->i * value->i;
 if (squared > maximum * maximum)
 {
  scale_vector_by_double(value, maximum / sqrt(squared), value);
 }
}

// @retail 0xaaca0
void function_aaca0(long index, point3f const *position, vector3f const *forward, vector3f const *up,
 vector3f const *linear_velocity, vector3f const *angular_velocity,
 vector3f *linear_result, vector3f *angular_result)
{
 transform4x3f current, desired, inverse, difference;
 quaternionf rotation;
 vector3f correction;
 real angle;
 function_ba160(index, &current);
 function_1420f0(&desired, position, forward, up);
 function_141590(&current, &inverse);
 function_142a60(&desired, &inverse, &difference);
 function_141f60(&difference.rotation, &rotation);
 function_11d790(&rotation, &angle, &correction);
 real angular_scale = angle * 5.0f;
 correction.i *= angular_scale;
 correction.j *= angular_scale;
 correction.k *= angular_scale;
 z_limit_correction(&correction, 1.0f);
 vector3f angular;
 angular.i = correction.i;
 angular.j = correction.j;
 angular.k = correction.k;
 vector3f translation;
 translation.i = difference.position.x * 0.9f;
 translation.j = difference.position.y * 0.9f;
 translation.k = difference.position.z * 0.9f;
 z_limit_correction(&translation, 0.05f);
 linear_result->i = linear_velocity->i + translation.i;
 linear_result->j = linear_velocity->j + translation.j;
 linear_result->k = linear_velocity->k + translation.k;
 angular_result->i = angular_velocity->i + angular.i;
 angular_result->j = angular_velocity->j + angular.j;
 angular_result->k = angular_velocity->k + angular.k;
}
