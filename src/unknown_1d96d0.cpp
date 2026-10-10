#include "unknown_11c920.h"
#include "unknown_1cec30.h"
#include "unknown_1da540.h"
#include <float.h>
#include <math.h>
#include <new>
// @flags /O2 /Ob1 /arch:SSE /Gr

struct s_1d96d7
{
 byte field_0[2];
 word field_2 : 5;
 word field_3 : 1;
};
struct s_1d96d8
{
 byte field_0[0x10a];
 word field_10a : 2;
 word field_10b : 1;
};
struct s_type_1e6529;
struct s_damage_owner
{
 long player_index;
 long object_index;
 short team;
};
struct s_1d96d1
{
 point3f field_0;
 long field_c;
 real field_10;
 vector3f field_14;
 long field_20;
 real field_24;
 real field_28;
};
bool havok_component_any_rigid_body_active(s_havok_component *arg_0);
bool function_1c58a0();
bool function_e58e0(long arg_0);
bool havok_component_unknown10_recent(s_havok_component const *arg_0);
bool function_1da550(long arg_0, long arg_1, s_collision_damage_entry *arg_2);
void __stdcall object_get_damage_owner(long arg_0, s_damage_owner *arg_1);
bool function_1da6d0(long arg_0, long arg_1, real arg_2, real arg_3, real arg_4);
bool function_1da9e0(long arg_0, point3f const *arg_1, real arg_2, real arg_3,
 real arg_4, bool arg_5, long arg_6);
void function_184060(long arg_0, byte arg_1, s_type_1e6529 *arg_2, long arg_3);

typedef void *(__fastcall *constructor_proc)(void *);
void __stdcall vector_constructor_iterator(void *arg_0, unsigned arg_1, int arg_2, constructor_proc arg_3);
PRIVATE void *__fastcall function_1d96d5(void *arg_0)
{
 return new (arg_0) s_collision_damage_entry;
}

PRIVATE __forceinline real function_1d96d2(hkRigidBody *arg_0)
{
 return 1.0f > arg_0->m_motion->getMass() ? 1.0f : arg_0->m_motion->getMass();
}
PRIVATE __forceinline real function_1d96d3(vector3f const *arg_0)
{
 return (real)sqrt(arg_0->i * arg_0->i + arg_0->j * arg_0->j + arg_0->k * arg_0->k);
}
PRIVATE __forceinline real function_1d96d6(vector3f const *arg_0)
{
 return (real)sqrt(arg_0->i * arg_0->i + (arg_0->j * arg_0->j + arg_0->k * arg_0->k));
}
PRIVATE __forceinline long function_1d96d4()
{
 real local_0 = g_510c54->field_2_3 * 0.35f;
 long local_1;
 __asm
 {
  fld local_0
  fistp local_1
 }
 return local_1;
}

// @retail 0x1d96d0
void function_1d96d0(long arg_0)
{
 long local_7 = 0;
 s_havok_component *local_0 = havok_component_get(arg_0);
 if (local_0->unknown88.size == 0 || !havok_component_any_rigid_body_active(local_0) || function_1c58a0())
  return;
 long local_1 = local_0->object_index;
 if (local_1 != NONE)
 {
  byte *local_2 = (byte *)g_4e0300->data;
  if (((1 << local_2[(local_1 & 0xffff) * 12 + 3]) & 1) && function_e58e0(local_1))
   return;
 }
 byte local_48[16 * sizeof(s_collision_damage_entry)];
 s_collision_damage_entry *local_3 = (s_collision_damage_entry *)local_48;
 vector_constructor_iterator(local_3, sizeof(s_collision_damage_entry), 16, function_1d96d5);
 byte *local_4 = (byte *)havok_object_get(local_1);
 byte *local_5 = (byte *)g_4e3b44[*(long *)local_4 & 0xffff].data;
 bool local_6 = !TEST_FIELD_BIT(((s_1d96d7 *)local_5)->field_3);
 long local_8 = 0;
 s_1d96d1 local_9[16];
 real local_10 = function_1d96d3(&local_0->rigid_bodies.data[0].linear_velocity);
 if (havok_component_unknown10_recent(local_0))
  local_10 += local_0->unknown14;
 real local_11 = 0.0f;
 for (long local_12 = 0; local_12 < local_0->rigid_bodies.size; ++local_12)
  local_11 += function_1d96d2(local_0->rigid_bodies.data[local_12].rigid_body);
 real local_13 = 0.0f;
 real local_14 = 0.0f;
 long local_15 = NONE;
 for (long local_16 = 0; local_16 < local_0->unknown88.size; ++local_16)
 {
  s_havok_component_element48 *local_17 = &local_0->unknown88.data[local_16];
  long local_18 = *(long *)&local_17->unknown18;
  s_havok_component *local_19 = local_17->component_b != NONE ? havok_component_get(local_17->component_b) : NULL;
  real local_20 = function_1d96d2(local_0->rigid_bodies.data[local_17->rigid_body_index_a].rigid_body) *
   (1.0f / local_11) * local_17->impulse;
  bool local_21 = local_19 == NULL;
  if (local_19)
  {
   hkRigidBody *local_22 = local_19->rigid_bodies.data[local_17->rigid_body_index_b].rigid_body;
   if (local_22->m_fixed || local_22->m_motion->getType() == 6)
    local_21 = true;
  }
  if (local_21 && !(local_13 > local_20))
   local_13 = local_20;
  bool local_23 = true;
  if (local_18 != NONE)
  {
   byte *local_24 = (byte *)havok_object_get(local_18);
   byte *local_25 = (byte *)g_4e3b44[*(long *)local_24 & 0xffff].data;
   local_23 = !TEST_FIELD_BIT(((s_1d96d7 *)local_25)->field_3);
  }
  if (!local_21 && local_23 && (local_15 == NONE || local_20 > local_14))
  {
   local_14 = local_20;
   local_15 = local_18;
  }
  if ((((byte *)local_17)[0x44] & 8) && local_7 < 16)
  {
   if (function_1da550(local_0->object_index, local_16, &local_3[local_7]))
    ++local_7;
  }
  if (local_21 && local_6 && local_18 != NONE && local_20 >= 0.001f && local_8 < 16)
  {
   real local_26 = local_13;
   real local_27 = -FLT_MAX;
   long local_28 = NONE;
   if (havok_component_unknown10_recent(local_0))
   {
    local_26 += local_0->unknown14;
    local_20 += local_0->unknown14;
   }
   for (long local_29 = 0; local_29 < local_0->unknown88.size; ++local_29)
   {
    s_havok_component_element48 *local_30 = &local_0->unknown88.data[local_29];
    real local_31 = function_1d96d2(local_0->rigid_bodies.data[local_30->rigid_body_index_a].rigid_body) *
     (1.0f / local_11) * local_30->impulse;
    if (*(long *)&local_30->unknown18 == local_18 && local_31 > local_27)
    {
     local_28 = local_29;
     local_27 = local_31;
    }
   }
   local_9[local_8].field_0 = local_0->unknown88.data[local_28].position;
   local_9[local_8].field_c = local_18;
   local_9[local_8].field_14 = *g_4687a4;
   local_9[local_8].field_10 = 0.0f;
   local_9[local_8].field_20 = local_1;
   local_9[local_8].field_24 = local_20;
   local_9[local_8].field_28 = local_26;
   ++local_8;
  }
 }
 if (local_15 != NONE)
 {
  long local_32 = havok_object_get(local_15)->havok_component_index;
  if (local_32 != NONE)
  {
   s_havok_component *local_33 = havok_component_get(local_32);
   real local_34 = function_1d96d6(&local_33->rigid_bodies.data[0].linear_velocity);
   if (g_510c54->game_time - local_33->unknown10 < function_1d96d4())
    local_34 += local_33->unknown14;
   if (local_34 > local_10)
   {
    s_damage_owner local_35;
    object_get_damage_owner(local_33->object_index, &local_35);
    *(long *)(local_4 + 0xc4) = local_35.player_index;
    *(long *)(local_4 + 0xc8) = local_35.object_index;
    *(short *)(local_4 + 0xc2) = local_35.team;
   }
  }
 }
 if ((local_13 >= 0.001f || local_14 >= 0.0f) && !TEST_FIELD_BIT(((s_1d96d8 *)local_4)->field_10b))
 {
  long local_36 = *(long *)(local_5 + 0x38);
  if (local_36 != NONE && *(long *)((byte *)g_4e3b44[local_36 & 0xffff].data + 0x60) > 0)
  {
   long local_37 = NONE;
   long local_38 = NONE;
   real local_39 = -FLT_MAX;
   real local_40 = -FLT_MAX;
   if (g_510c54->game_time - local_0->unknown10 < function_1d96d4())
   {
    local_13 += local_0->unknown14;
    local_14 += local_0->unknown14;
   }
   for (long local_41 = 0; local_41 < local_0->unknown88.size; ++local_41)
   {
    s_havok_component_element48 *local_42 = &local_0->unknown88.data[local_41];
    real local_43 = function_1d96d2(local_0->rigid_bodies.data[local_42->rigid_body_index_a].rigid_body) *
     (1.0f / local_11) * local_42->impulse;
    if (local_43 > local_40)
    {
     local_38 = local_41;
     local_40 = local_43;
    }
    if (*(long *)&local_42->unknown18 == local_15 && local_43 > local_39)
    {
     local_37 = local_41;
     local_39 = local_43;
    }
   }
   if ((local_15 == NONE || local_37 != NONE) && local_38 != NONE)
   {
    point3f local_44 = local_0->unknown88.data[local_38].position;
    bool local_45 = local_15 != NONE && function_1da6d0(local_0->object_index, local_15, local_13, local_14, local_10);
    function_1da9e0(local_0->object_index, &local_44, local_14, local_13, local_10, local_45, local_15);
   }
  }
 }
 for (long local_46 = 0; local_46 < local_7; ++local_46)
  function_184060(local_3[local_46].instance_index, (byte)local_3[local_46].material_index,
   (s_type_1e6529 *)&local_3[local_46], local_3[local_46].surface_index);
 for (long local_47 = 0; local_47 < local_8; ++local_47)
  function_1da6d0(local_9[local_47].field_c, local_9[local_47].field_20,
   local_9[local_47].field_28, local_9[local_47].field_24, local_9[local_47].field_10);
}
