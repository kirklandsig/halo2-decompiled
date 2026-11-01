#include "unknown_11c920.h"
#include "unknown_1cec30.h"
#include "havok_reference.h"
#include <string.h>
// @flags /O2 /arch:SSE /Gr

struct s_physics_model_owner;
struct s_physics_body_groups;
struct s_1d70d0;
class c_311ba0;
struct s_1d7300;
struct s_1d7400;
c_havok_reference_counted *__stdcall function_1d5ec0(s_havok_component *, s_physics_model_owner *, char const *, char *, long *, s_physics_body_groups *, long, dword *, real *, hkVector4 *, hkRotation *, bool *, long *, bool *, bool *);
long function_1d69d0(s_havok_component *, long, s_physics_model_owner *, long, long);
c_311ba0 *function_1d70d0(s_havok_component *, s_1d70d0 *, bool, bool, hkVector4 const *, real, real, hkRotation const *, real, long, hkTransform const *, long, bool, real, real);
s_1d7300 *function_1d7300(s_1d7300 *, s_1d7400 *, real);
struct s_1d5bf0
{
 s_1d7300 *field_0;
 char field_4[64];
 long field_44;
 bool field_48;
};

// @retail 0x1d5bf0
bool function_1d5bf0(s_havok_component *arg_0, s_physics_model_owner *arg_1, char const *arg_2,
 bool arg_3, long arg_4, long *arg_5, dword *arg_6, s_physics_body_groups *arg_7, s_1d5bf0 *arg_8)
{
 bool local_0 = false;
 char local_1[64];
 long local_2 = 0;
 bool local_3 = false, local_4 = true, local_5 = false;
 real local_6;
 hkVector4 local_7;
 hkRotation local_8;
 c_havok_reference_counted *local_9 = function_1d5ec0(arg_0, arg_1, arg_2, local_1, &local_2,
  arg_7, arg_4, arg_6, &local_6, &local_7, &local_8, &local_3, arg_5, &local_4, &local_5);
 if (local_9)
 {
  byte *local_10 = *(byte **)((byte *)arg_1 + 0x48);
  long local_21 = local_1[0];
  byte *local_11 = *(byte **)(local_10 + 0x3c) + local_21 * 0x90;
  long local_12 = local_3 ? 7 : arg_2[local_21];
  long local_13 = function_1d69d0(arg_0, local_12, arg_1, *arg_5, local_21);
  long local_14 = *(short *)local_11;
  transform4x3f *local_15 = local_14 == NONE ? (transform4x3f *)((byte *)arg_1 + 8) :
   *(transform4x3f **)((byte *)arg_1 + 0x50) + local_14;
  hkTransform local_16;
  local_16.m_rotation.m_col0.set(local_15->forward.i, local_15->forward.j, local_15->forward.k);
  local_16.m_rotation.m_col1.set(local_15->left.i, local_15->left.j, local_15->left.k);
  local_16.m_rotation.m_col2.set(local_15->up.i, local_15->up.j, local_15->up.k);
  hkVector4 local_17;
  local_17.set(local_15->position.x, local_15->position.y, local_15->position.z);
  local_16.m_translation = local_17;
  word local_18 = *(word *)(local_11 + 0x18);
  volatile bool local_19;
  if ((local_18 & 1) || ((1 << g_4e0300->data[(arg_0->object_index & 0xffff) * 12 + 3]) & 2)) local_19 = true;
  else local_19 = false;
  s_1d7300 *local_20 = (s_1d7300 *)function_1d70d0(arg_0, (s_1d70d0 *)local_9,
   *(long *)(local_10 + 0x38) == 1, local_12 == 7 || local_12 == 6, &local_7, 1.0f, 0.0f,
   &local_8, local_6, local_12, &local_16, local_13, (bool)((local_18 >> 1) & 1),
   *(real *)(local_11 + 0x24), *(real *)(local_11 + 0x28));
  havok_reference_remove(local_9);
  if (arg_3) local_20 = function_1d7300(local_20, (s_1d7400 *)arg_0, 0.85f);
  arg_8->field_0 = local_20;
  memcpy(arg_8->field_4, local_1, local_2);
  arg_8->field_48 = local_4;
  arg_8->field_44 = local_2;
  local_0 = true;
  if (local_5) arg_0->unknown04 |= 0x4000;
 }
 return local_0;
}

extern transform4x3f *g_4687d0;
point3f *function_b9dd0(long, point3f *);
bool __stdcall function_e0d70(long, real *);
void function_1d0080(hkRigidBody *, s_havok_component *, byte const *, long, bool);

class c_1d6d01
{
public:
 virtual void function_0() = 0;
 virtual void function_1() = 0;
 virtual void function_2() = 0;
 virtual void function_3() = 0;
 virtual void function_4() = 0;
 virtual long function_5() = 0;
};

class c_1d6d02
{
public:
 virtual void function_0() = 0;
 virtual void function_1() = 0;
 virtual void function_2() = 0;
 virtual void function_3() = 0;
 virtual void function_4() = 0;
 virtual void function_5() = 0;
 virtual void function_6() = 0;
 virtual void function_7() = 0;
 virtual void function_8() = 0;
 virtual void function_9() = 0;
 virtual void function_a() = 0;
 virtual void function_b() = 0;
 virtual void function_c(hkRotation *) = 0;
};

struct s_1d6d03
{
 c_1d6d01 *field_0;
};

PRIVATE __forceinline s_1d6d03 function_1d6d03(void *arg_0, long arg_1)
{
 long local_0 = (byte)arg_1 != 0;
 s_1d6d03 local_1;
 if (*(long *)((byte *)arg_0 + 0x28))
  local_1.field_0 = (c_1d6d01 *)(*(byte **)((byte *)arg_0 + 0x2c) + local_0 * 0x50 + 0x20);
 else local_1.field_0 = (c_1d6d01 *)(*(byte **)((byte *)arg_0 + 0x34) + local_0 * 0x80 + 0x30);
 return local_1;
}

PRIVATE __forceinline bool function_1d6d04(long arg_0)
{
 return arg_0 == 4 || arg_0 == 5;
}

// @retail 0x1d6d00
void __stdcall function_1d6d00(s_havok_component *arg_0, void *arg_1, byte *arg_2,
 long arg_3, long arg_4, bool arg_5, bool arg_6)
{
 if (!arg_6) arg_0->unknown04 |= 0x40000;
 else arg_0->unknown04 &= ~0x40000;
 dword local_0 = *(dword *)arg_1;
 if ((bool)((local_0 >> 6) & 1)) arg_4 = arg_3 = 10;
 if ((bool)((local_0 >> 5) & 1))
 {
  arg_0->unknown18 = (char)0xff;
  return;
 }
 long local_1 = arg_0->object_index;
 long local_2 = (~((long)g_4e0300->data[(local_1 & 0xffff) * 12 + 2] << 1) & 2) | 5;
 transform4x3f local_3 = *g_4687d0;
 byte local_4 = 0xff;
 hkVector4 local_5;
 local_5.set(0.0f, 0.0f, 0.0f);
 c_1d6d01 *local_6;
 if (arg_6) local_6 = (c_1d6d01 *)(*(byte **)((byte *)arg_1 + 0x24) + arg_2[0x23] * 0x80 + 0x30);
 else local_6 = function_1d6d03(arg_1, arg_5).field_0;
 hkRotation local_8;
 local_8.m_col0 = local_5;
 local_8.m_col1 = local_5;
 local_8.m_col2 = local_5;
 if (!arg_6) arg_0->unknown04 |= 4;
 else arg_0->unknown04 &= ~4;
 arg_0->unknown04 |= 0x203;
 if (function_1d6d04(arg_2[0])) arg_0->unknown04 |= 0x100;
 else arg_0->unknown04 &= ~0x100;
 if (function_1d6d04(arg_2[0])) local_3.position = *(point3f *)(arg_2 + 0x10);
 else
 {
  function_b9dd0(local_1, &local_3.position);
  if ((1 << g_4e0300->data[(local_1 & 0xffff) * 12 + 3]) & 1)
  {
   real local_9;
   if (function_e0d70(arg_0->object_index, &local_9)) local_3.position.z -= local_9;
  }
 }
 hkTransform local_10;
 ((real *)&local_10.m_rotation.m_col0)[0] = local_3.forward.i;
 ((real *)&local_10.m_rotation.m_col0)[1] = local_3.forward.j;
 ((real *)&local_10.m_rotation.m_col0)[2] = local_3.forward.k;
 ((real *)&local_10.m_rotation.m_col1)[0] = local_3.left.i;
 ((real *)&local_10.m_rotation.m_col1)[1] = local_3.left.j;
 ((real *)&local_10.m_rotation.m_col1)[2] = local_3.left.k;
 ((real *)&local_10.m_rotation.m_col2)[0] = local_3.up.i;
 ((real *)&local_10.m_rotation.m_col2)[1] = local_3.up.j;
 ((real *)&local_10.m_rotation.m_col2)[2] = local_3.up.k;
 local_5.set(local_3.position.x, local_3.position.y, local_3.position.z);
 ((real *)&local_10.m_rotation.m_col0)[3] = 0.0f;
 ((real *)&local_10.m_rotation.m_col1)[3] = 0.0f;
 ((real *)&local_10.m_rotation.m_col2)[3] = 0.0f;
 local_10.m_translation = local_5;
 if (local_6->function_5() == 7)
 {
  __m128 local_11 = _mm_set_ss(0.5f);
  local_5.m_quad = _mm_mul_ps(_mm_shuffle_ps(local_11, local_11, 0), _mm_add_ps(*(__m128 *)((byte *)local_6 + 0x10), *(__m128 *)((byte *)local_6 + 0x20)));
 }
 else if (local_6->function_5() == 0x15) local_5.m_quad = *(__m128 *)((byte *)local_6 + 0x40);
 else
 {
  ((real *)&local_5)[0] = g_468788->x;
  ((real *)&local_5)[1] = g_468788->y;
  ((real *)&local_5)[2] = g_468788->z;
  ((real *)&local_5)[3] = 0.0f;
 }
 long local_12;
 real local_13;
 if (arg_6) { local_12 = arg_4; local_13 = 1.0f; }
 else { local_12 = arg_3; local_13 = 0.0f; }
 hkRigidBody *local_14 = (hkRigidBody *)function_1d70d0(arg_0, (s_1d70d0 *)local_6, false, false,
  &local_5, local_13, 0.0f, &local_10.rotation_part(), *(real *)((byte *)arg_1 + 0x10),
  local_2, &local_10, local_12, false, 0.0f, 0.0f);
 if (!local_14->m_fixed) ((c_1d6d02 *)local_14->m_motion)->function_c(&local_8);
 if (!(bool)((arg_0->unknown04 >> 8) & 1)) local_14 = (hkRigidBody *)function_1d7300((s_1d7300 *)local_14, (s_1d7400 *)arg_0, 0.85f);
 function_1d0080(local_14, arg_0, &local_4, 1, true);
 arg_0->unknown18 = 0;
}
