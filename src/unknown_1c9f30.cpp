#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_1e46c0.h"
#include "globals.h"
// @flags /O2 /Gr

struct s_1c9f30
{
 byte field_0[0xaa];
 char field_aa;
 byte field_ab[7];
 byte field_b2;
 byte field_b3[0x12c - 0xb3];
 long field_12c;
 long field_130;
 byte field_134[0x3a0 - 0x134];
 long field_3a0;
};
struct s_1c9f31
{
 byte field_0[0x342];
 short field_342;
 long field_344[1];
};
struct s_1c9f32
{
 byte field_0[0x40];
 long field_40;
 byte field_44[0x69 - 0x44];
 bool field_69;
 bool field_6a;
};
void function_26ae30(long object_index);
void function_a7bc0(long index);
void __stdcall function_1e1a00(long actor_index, bool value);
void function_201a50(long squad_index, long object_index);
void __stdcall function_205320(long group_index);
void function_211640(long arg_0);
void function_210020(long object_index);
void function_2103d0(long object_index);
void joint_clear_references(long joint_index);
void function_1e17d0(long actor_index, long object_index);
void swarm_spawn_slots_clear_object(long key);
void ai_players_unit_deleted(long unit_index);
extern s_record_pool *g_502414;

// @retail 0x1c9f30
void __stdcall function_1c9f30(long arg_0)
{
 if (g_4f55d0->active)
 {
  s_1c9f30 *local_0 = (s_1c9f30 *)object_get(arg_0);
  short local_1 = local_0->field_aa;
  long local_2 = 1 << local_1;
  if ((local_2 & 0x1003) || (local_0->field_b2 & 2))
  {
   function_26ae30(arg_0);
   if (local_2 & 3)
   {
    s_1c9f30 *local_3 = (s_1c9f30 *)object_get(arg_0);
    if (local_3->field_130 != NONE)
     function_a7bc0(arg_0);
    if (local_3->field_12c != NONE)
     function_1e1a00(local_3->field_12c, 0);
    if (local_3->field_aa == 1 && local_3->field_3a0 != NONE)
     function_201a50(local_3->field_3a0, arg_0);
    s_data_datum_iterator local_4;
    local_4.data = g_502414;
    local_4.index = NONE;
    local_4.datum_index = NONE;
    while (data_datum_iterator_next(&local_4))
    {
     s_1c9f32 *local_5 = (s_1c9f32 *)local_4.datum;
     if (local_5->field_40 == arg_0)
     {
      local_5->field_40 = NONE;
      local_5->field_6a = false;
      local_5->field_69 = false;
     }
    }
    s_1c9f31 *local_6 = (s_1c9f31 *)g_4f55d0;
    for (short local_7 = 0; local_7 < local_6->field_342; ++local_7)
    {
     long *local_8 = &local_6->field_344[local_7];
     if (*local_8 == arg_0)
     {
      --local_6->field_342;
      if (local_6->field_342 > 0)
       *local_8 = local_6->field_344[local_6->field_342];
     }
    }
    function_205320(arg_0);
   }
   else if (local_1 == 12)
    function_211640(arg_0);
   function_210020(arg_0);
  }
  if (local_2 & 0x8c3)
  {
   function_2103d0(arg_0);
   joint_clear_references(arg_0);
  }
  if (local_1 != 5 || (local_0->field_b2 & 2))
  {
   s_actor_iterator local_9;
   function_x66da2b(&local_9, false);
   while (function_1e46c0(&local_9))
    function_1e17d0(local_9.actor_index, arg_0);
   swarm_spawn_slots_clear_object(arg_0);
  }
  ai_players_unit_deleted(arg_0);
 }
}
