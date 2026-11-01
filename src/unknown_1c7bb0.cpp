#include "unknown_11c920.h"
#include "slot_handler.h"
#include "globals.h"
#include "squads.h"
// @flags /O2 /Gr

struct s_1c7bb0
{
 byte field_0[2];
 char field_2;
 byte field_3[0x28 - 3];
 short field_28;
 byte field_2a[0x7e - 0x2a];
 short field_7e;
 byte field_80[0x98 - 0x80];
};
bool function_204390(long squad_index);
bool actor_update_bsp_locations(long actor_index);
void function_203ed0(long actor_index, bool keep_count);
void function_1e22d0(long actor_index, bool conditional);
void function_203cc0(long squad_index);
bool function_1e13f0(long actor_index);
void function_203fb0(long actor_index);
void function_203d70(long actor_index, short squad_index, bool keep_team);
void function_295e60(short arg_0);
void function_210280();
void function_203f60(long actor_index);
void function_292f10();

struct s_squad_iterator_1c7bb0
{
 byte *squad;
 s_record_pool_iterator records;
 long squad_index;
};

/* starts the squad walk; it tests the ai's active flag again, as the walk's step does */
PRIVATE inline void squad_walk_begin(s_squad_iterator_1c7bb0 *arg_0)
{
 if (g_4f55d0->active)
 {
  arg_0->records.data = g_51e9d8;
  arg_0->records.index = NONE;
 }
}

PRIVATE __forceinline byte *function_1c7bb1(s_squad_iterator_1c7bb0 *arg_0)
{
 byte *result = NULL;
 if (g_4f55d0->active)
 {
  byte *datum = data_iterator_next_calling(&arg_0->records);
  if (datum)
   result = datum;
  arg_0->squad_index = arg_0->records.datum_index & 0xffff;
 }
 arg_0->squad = result;
 return result;
}

// @retail 0x1c7bb0
void function_1c7bb0()
{
 if (g_4f55d0->active)
 {
  short local_0 = g_4686c4;
  s_squad_iterator_1c7bb0 local_1;
  byte *datum;
  squad_walk_begin(&local_1);
  while ((datum = function_1c7bb1(&local_1)) != NULL)
  {
   long local_2 = local_1.squad_index;
   if (function_204390(local_2))
   {
    ((s_1c7bb0 *)g_51e9d8->data)[local_2 & 0xffff].field_7e = (short)local_0;
    s_squad_actor_iterator local_4;
    squad_actors_begin(&local_4, local_2);
    s_actor_datum *local_6;
    while ((local_6 = squad_actors_next(&local_4)) != NULL)
    {
     if (!actor_update_bsp_locations(local_4.actor_index))
     {
      *(long *)((byte *)local_6 + 0x34) = local_6->squad_index;
      function_203ed0(local_4.actor_index, false);
      function_203f60(local_4.actor_index);
     }
    }
   }
   else
    ((s_1c7bb0 *)g_51e9d8->data)[local_2 & 0xffff].field_7e = NONE;
   if (((s_1c7bb0 *)datum)->field_2 & 0x80)
   {
    ((s_1c7bb0 *)g_51e9d8->data)[local_2 & 0xffff].field_28 = 0;
    function_203cc0(local_2);
   }
  }
  long local_4 = g_4f55d0->unknown14;
  while (local_4 != NONE)
  {
   s_actor_view *local_6 = actor_get(local_4);
   long local_5 = local_4;
   local_4 = local_6->unknown020;
   if (function_1e13f0(local_5) && actor_update_bsp_locations(local_5))
   {
    long local_7 = *(long *)((byte *)local_6 + 0x34);
    if (local_7 != NONE && ((s_1c7bb0 *)g_51e9d8->data)[local_7 & 0xffff].field_7e == (short)local_0)
    {
     function_203fb0(local_5);
     function_203d70(local_5, (short)*(long *)((byte *)local_6 + 0x34), true);
    }
   }
   local_6 = actor_get(local_5);
   if (local_6->unknown009)
   {
    local_6->unknown009 = false;
    *(long *)((byte *)local_6 + 0x10) = g_510c54->game_time;
    --*(short *)((byte *)g_4f55d0 + 0x36a);
   }
  }
  function_295e60(local_0);
  function_210280();
  function_292f10();
 }
}
