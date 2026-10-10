#include "unknown_11c920.h"
#include "unknown_1cec30.h"
#include "unknown_182d90.h"
#include <new>
// @flags /O2 /arch:SSE /Gr

class c_1d7390
{
public:
 virtual void slot0() = 0;
 virtual void slot1() = 0;
 virtual void slot2() = 0;
 virtual void slot3() = 0;
 virtual void *function_1d7390(long arg_0, long arg_1) = 0;
};

class c_1d70d0
{
public:
 virtual void function_1d70d0() = 0;
 virtual void function_1d70d1() = 0;
 virtual void function_1d70d2() = 0;
 virtual void function_1d70d3() = 0;
 virtual void function_1d70d4() = 0;
 virtual long function_1d70d5() = 0;
};

struct s_1d70d0 : c_1d70d0
{
 byte field_4[8];
 s_1d70d0 *field_c;
 hkTransform field_10;
};

class c_component_rotation
{
public:
 hkVector4 value;
 void set(hkRotation const &rotation);
};
struct s_1d8d60
{
 __m128 field_0;
 void function_1d8d60();
};
struct c_transformed_point
{
 __m128 value;
 void transform(const void *matrix, const __m128 *point);
};
struct s_311340
{
 long field_0;
 s_1d70d0 *field_4;
 long field_8;
 void *field_c;
 long field_10;
 dword field_14;
 byte field_18[8];
 hkVector4 field_20;
 c_component_rotation field_30;
 byte field_40[0x20];
 hkRotation field_60;
 hkVector4 field_90;
 real field_a0, field_a4, field_a8, field_ac, field_b0;
 byte field_b4;
 s_311340();
 ~s_311340()
 {
  if (!(field_14 & 0x80000000))
   g_480118->allocate((long)field_c, (field_14 & 0x7fffffff) * 8, 0x12);
 }
};
class c_311ba0
{
public:
 long field_0;
 word field_4;
 byte field_6[0x9a];
 c_311ba0(const s_311340 *arg_0);
};

struct s_1d70d1
{
 byte field_0[0xc0];
 word field_c0 : 5;
 word field_c1 : 1;
 word field_c2 : 10;
};

// @retail 0x1d70d0
c_311ba0 *function_1d70d0(s_havok_component *arg_0, s_1d70d0 *arg_1, bool arg_2, bool arg_3,
 const hkVector4 *arg_4, real arg_5, real arg_6, const hkRotation *arg_7, real arg_8, long arg_9,
 const hkTransform *arg_10, long arg_11, bool arg_12, real arg_13, real arg_14)
{
 (void)&arg_1; (void)&arg_2; (void)&arg_3; (void)&arg_4; (void)&arg_5; (void)&arg_6;
 (void)&arg_7; (void)&arg_8; (void)&arg_9; (void)&arg_10; (void)&arg_11; (void)&arg_12;
 (void)&arg_13; (void)&arg_14;
 const hkRotation *const *local_8 = &arg_7;
 volatile long *local_9 = &arg_9;
 long local_0 = (arg_0->object_index & 0xffff) + 1;
 s_311340 local_1;
 c_component_rotation local_2;
 local_2.set(arg_10->m_rotation);
 byte *local_3 = *(byte **)(g_4e0300->data + (arg_0->object_index & 0xffff) * 12 + 8);
 if (TEST_FIELD_BIT(((s_1d70d1 *)local_3)->field_c1)) *local_9 = 7;
 if (arg_11 == 10) local_0 = 0x802;
 ((s_1d8d60 *)&local_2)->function_1d8d60();
 local_1.field_90 = *arg_4;
 local_1.field_ac = arg_5;
 local_1.field_b0 = arg_6;
 local_1.field_60 = **local_8;
 local_1.field_a0 = arg_8;
 local_1.field_20 = arg_10->m_translation;
 local_1.field_4 = arg_1;
 local_1.field_b4 = (byte)*local_9;
 local_1.field_30 = local_2;
 local_1.field_0 = (local_0 << 16) | (arg_12 ? 2 : arg_11);
 local_1.field_a4 = arg_13;
 local_1.field_a8 = arg_14;
 if (!arg_3 && local_1.field_4->function_1d70d5() == 0x13)
  local_1.field_4 = local_1.field_4->field_c;
 if (arg_2 && local_1.field_4->function_1d70d5() == 0x15)
 {
  s_1d70d0 *local_4 = local_1.field_4;
  local_1.field_4 = local_4->field_c;
  s_extent_transform local_5;
  local_5.compose((const s_extent_transform *)arg_10, (const s_extent_transform *)&local_4->field_10);
  local_1.field_20.m_quad = local_5.rows[3];
  local_1.field_30.set(*(hkRotation *)&local_5);
  ((s_1d8d60 *)&local_1.field_30)->function_1d8d60();
  arg_0->unknown04 |= 0x40;
  arg_0->transform.setInverse(local_4->field_10);
  c_transformed_point local_6;
  local_6.transform(&arg_0->transform, &local_1.field_90.m_quad);
  local_1.field_90.m_quad = local_6.value;
 }
 c_311ba0 *local_7 = (c_311ba0 *)((c_1d7390 *)g_480118)->function_1d7390(0xa0, 0x28);
 local_7->field_4 = 0xa0;
 return new (local_7) c_311ba0(&local_1);
}
