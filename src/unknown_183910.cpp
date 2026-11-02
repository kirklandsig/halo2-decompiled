// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1d5120.h"
#include "unknown_1efac0.h"
#include <float.h>
#include <xmmintrin.h>

struct s_batch_contact;
class c_query_collector
{
public:
 virtual ~c_query_collector() {}
 virtual void collect(s_batch_contact const *) {}
 static void operator delete(void *block)
 {
  g_480118->allocate((long)block, 8, 0x1a);
 }
 real maximum;
};
struct s_query_collector_entries
{
 void *data;
 long count;
 dword capacity_and_flags;
 ~s_query_collector_entries()
 {
  if (!(capacity_and_flags & 0x80000000))
   g_480118->allocate((long)data, (capacity_and_flags & 0x7fffffff) * 0x30, 0x12);
 }
};
struct source_2df020;
class class_2df020
{
public:
 void function_2df020(source_2df020 const *source);
};

class c_query_array_collector : public c_query_collector
{
public:
 virtual ~c_query_array_collector();
 /* retail table 0x453b90 slot 1 (0x2df020): the contact appended to
    the array at +0x10; empty, every query here found nothing (0x183910
    gave explosions no closest point, so their damage fell off as if
    from far away) */
 virtual void collect(s_batch_contact const *contact)
 {
  ((class_2df020 *)this)->function_2df020((source_2df020 const *)contact);
 }
 byte unknown08[8];
 s_query_collector_entries entries;
};
class c_2def90 : public c_a
{
public:
 c_2def90(real arg_0);
 long field_8;
 real field_c;
};
class c_183911
{
public:
 virtual void function_0() {}
 virtual void function_1() {}
 virtual void function_2() {}
 virtual void function_3() {}
 virtual void function_4() {}
 virtual long function_5() { return 0; }
};
struct s_183912
{
 byte field_0[0xc];
 c_183911 *field_c;
 byte field_10[0x30];
 byte field_40;
};
struct s_183913
{
 byte field_0[0x40];
 s_183912 *field_40;
 byte field_44[0x1c];
};
struct s_183914
{
 long field_0;
 struct { dword field_0 : 5; dword field_5 : 1; } field_4;
 byte field_8[0x68];
 s_183913 *field_70;
 long field_74;
 byte field_78[0x28];
};
struct s_183915;
typedef void (__cdecl *function_183916)(void const *, void const *, s_183915 *, c_query_collector *);
struct s_183917
{
 byte field_0[0x218c];
 function_183916 field_218c[32][32];
};
struct __declspec(align(16)) s_183915
{
 s_183917 *field_0;
 long field_4;
 real field_8;
 long field_c;
};
struct s_183918
{
 byte field_0[0xcc];
 s_183915 *field_cc;
};
struct s_183919
{
 __m128 field_0;
 __m128 field_10;
 __m128 field_20;
};
class hkWorld;
extern hkWorld *g_51e9a4;

struct s_18391a
{
 c_query_array_collector field_0;
 s_183919 field_20[8];
};

// @retail 0x183910
bool function_183910(long arg_0, point3f const *arg_1, point3f *arg_2, vector3f *arg_3)
{
 s_183915 *p = ((s_183918 *)g_51e9a4)->field_cc;
 s_183915 local_0 = *p;
 s_183914 *local_1 = &((s_183914 *)g_51e9b8->data)[arg_0 & 0xffff];
 bool local_2 = false;
 if (TEST_FIELD_BIT(local_1->field_4.field_5))
 {
  _control87(0x9001f, 0x8001f);
  _mm_setcsr(_mm_getcsr() | 0x1f80);
  c_2def90 local_3(0.05f);
  c_contact_query_transform_info local_4;
  local_4.transform.m_rotation.m_col0.m_quad = _mm_setzero_ps();
  local_4.transform.m_rotation.m_col1.m_quad = _mm_setzero_ps();
  local_4.transform.m_rotation.m_col2.m_quad = _mm_setzero_ps();
  local_4.transform.m_rotation.m_col0.m_quad.m128_f32[0] = 1.0f;
  local_4.transform.m_rotation.m_col1.m_quad.m128_f32[1] = 1.0f;
  local_4.transform.m_rotation.m_col2.m_quad.m128_f32[2] = 1.0f;
  local_4.transform.m_translation.m_quad.m128_f32[0] = arg_1->x;
  local_4.transform.m_translation.m_quad.m128_f32[1] = arg_1->y;
  local_4.transform.m_translation.m_quad.m128_f32[2] = arg_1->z;
  local_4.transform.m_translation.m_quad.m128_f32[3] = 0;
  local_4.shape = &local_3;
  c_contact_query_transform_volume local_5(&local_4);
  c_183911 *local_6 = *(c_183911 **)((byte *)&local_5 + 0xc);
  local_0.field_8 = FLT_MAX;
  real local_7 = FLT_MAX;
  for (long local_8 = 0; local_8 < local_1->field_74; ++local_8)
  {
   if (!local_1->field_70[local_8].field_40->field_40)
   {
    s_18391a local_23;
    c_query_array_collector &local_10 = local_23.field_0;
    local_10.entries.data = local_23.field_20;
    local_10.entries.capacity_and_flags = 0x80000008;
    local_10.entries.count = 0;
    local_10.maximum = FLT_MAX;
    c_183911 **local_12 = &local_1->field_70[local_8].field_40->field_c;
    long local_13 = (*local_12)->function_5();
    long local_14 = local_6->function_5();
    local_0.field_0->field_218c[local_14][local_13]((byte *)&local_5 + 0xc, local_12, &local_0, &local_10);
    s_183919 *local_15 = (s_183919 *)local_10.entries.data;
    for (long local_16 = 0; local_16 < local_10.entries.count; ++local_16)
    {
     if (local_7 > local_15[local_16].field_0.m128_f32[3])
     {
      local_7 = local_15[local_16].field_0.m128_f32[3];
      real local_17 = local_15[local_16].field_0.m128_f32[0];
      real local_18 = local_15[local_16].field_0.m128_f32[2];
      real local_19 = local_15[local_16].field_0.m128_f32[1];
      arg_2->x = local_17;
      arg_2->y = local_19;
      arg_2->z = local_18;
      real local_20 = local_15[local_16].field_10.m128_f32[0];
      real local_21 = local_15[local_16].field_10.m128_f32[2];
      real local_22 = local_15[local_16].field_10.m128_f32[1];
      arg_3->i = local_20;
      arg_3->j = local_22;
      arg_3->k = local_21;
      local_2 = true;
     }
    }
   }
  }
  _mm_setcsr(_mm_getcsr() & ~0x3f);
  _clearfp();
  _control87(0x9001f, 0xfffff);
 }
 return local_2;
}
