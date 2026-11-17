#include "unknown_11c920.h"
#include "unknown_1cec30.h"
#include <math.h>
#include "vector_products.h"
// @flags /O2 /Ob1 /arch:SSE /Gr

void function_1d1260(s_havok_component *arg_0);
void __stdcall function_1d01c0(s_havok_component *arg_0);
void __stdcall function_bd020(long arg_0);
void function_1cf120(long arg_0);
void function_1d1540(s_havok_component *arg_0);
transform4x3f *function_ba160(long arg_0, transform4x3f *arg_1);
bool __stdcall function_e0d70(long arg_0, real *arg_1);
void havok_component_rigid_body_matrix_set(long arg_0, s_havok_component *arg_1, transform4x3f const *arg_2);
void function_1d4360(s_havok_component *arg_0, transform4x3f *volatile arg_1);
void function_141590(transform4x3f const *arg_0, transform4x3f *arg_1);
int __fastcall function_142a60(transform4x3f const *arg_0, transform4x3f const *arg_1, transform4x3f *arg_2);

PRIVATE __forceinline vector3f *function_1d3fc1(vector3f *arg_0)
{
 real squared = length_sq3f(arg_0);
 real local_0 = (real)sqrt(squared);
 if (!(fabs(local_0) < 0.0001f))
 {
  real local_1 = 1.0f / local_0;
  arg_0->i = local_1 * arg_0->i;
  arg_0->j = arg_0->j * local_1;
  arg_0->k = arg_0->k * local_1;
 }
 return arg_0;
}

/* the same normalization for a transform's forward axis, reached through the transform */
PRIVATE __forceinline vector3f *transform_forward_normalize_1d3fc0(transform4x3f *matrix)
{
 real length = (real)sqrt(length_sq3f(&matrix->forward));
 if (!(fabs(length) < 0.0001f))
 {
  real inverse = 1.0f / length;
  matrix->forward.i = inverse * matrix->forward.i;
  matrix->forward.j = matrix->forward.j * inverse;
  matrix->forward.k = matrix->forward.k * inverse;
 }
 return &matrix->forward;
}

/* Matches retail byte for byte (plain check, m3 lane 10 round 115). Three forms below look odd and are what the match
   needs; VC 7.1 puts the younger of two fields first in every float product and sum here, and a field's age is set by
   how the code first reaches it (a field named directly is oldest, then the first component of a vector whose address
   goes straight to an inline helper, then the other components in the order the code reads them):
   - the forward axis is normalized through the transform (transform_forward_normalize_1d3fc0), so forward's i is
     reached at a non-zero offset and ranks after up's i: retail multiplies `forward.i * up.i` in the dot;
   - the projection's k row reads the matrix field, which makes forward's k the oldest field and stops its load before
     the short-vector test (retail scales it as `inverse * [k]` and reloads it on the short path);
   - the dot is written `i + (j + k)`: retail sums its terms j, k, i. */
// @retail 0x1d3fc0
void __stdcall function_1d3fc0(long arg_0, s_havok_component *arg_1)
{
 real local_0;
 transform4x3f local_1[6];
 if (TEST_FIELD_BIT(arg_1->flag10))
 {
  bool local_7 = TEST_FIELD_BIT(arg_1->flag5);
  if (local_7)
   function_1d1260(arg_1);
  function_1d01c0(arg_1);
  function_bd020(arg_1->object_index);
  function_1cf120(arg_0);
  if (local_7)
   function_1d1540(arg_1);
 }
 else if (TEST_FIELD_BIT(arg_1->flag9))
 {
  function_ba160(arg_1->object_index, &local_1[0]);
  byte *local_8 = (byte *)g_4e0300->data;
  if (((1 << local_8[(arg_1->object_index & 0xffff) * 12 + 3]) & 1) &&
      function_e0d70(arg_1->object_index, &local_0))
   local_1[0].position.z -= local_0;
  havok_component_rigid_body_matrix_set(0, arg_1, &local_1[0]);
 }
 else
 {
  function_1d4360(arg_1, &local_1[4]);
  function_ba160(arg_1->object_index, &local_1[2]);
  function_141590(&local_1[4], &local_1[1]);
  function_142a60(&local_1[2], &local_1[1], &local_1[5]);
  for (long local_9 = 0; local_9 < arg_1->rigid_bodies.size; ++local_9)
  {
   arg_1->rigid_body_matrix_get(local_9, &local_1[3]);
   function_142a60(&local_1[5], &local_1[3], &local_1[0]);
   if (arg_1->unknown04 & 0x10000)
   {
    vector3f *forward = transform_forward_normalize_1d3fc0(&local_1[0]);
    vector3f *up = function_1d3fc1(&local_1[0].up);
    real local_10 = forward->i * up->i + (forward->j * up->j + forward->k * up->k);
    up->i -= forward->i * local_10;
    up->j -= forward->j * local_10;
    up->k -= local_1[0].forward.k * local_10;
    function_1d3fc1(up);
    cross3f(up, forward, &local_1[0].left);
    function_1d3fc1(&local_1[0].left);
   }
   havok_component_rigid_body_matrix_set(local_9, arg_1, &local_1[0]);
  }
  arg_1->unknown04 |= 0x10000;
 }
}
