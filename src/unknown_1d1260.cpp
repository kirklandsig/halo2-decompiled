#include "unknown_11c920.h"
#include "unknown_1cec30.h"
#include "globals.h"
// @flags /O2 /Gr

class c_1d1261
{
public:
 hkBool function_30ca30(void *arg_0);
 void function_30bd10(void *arg_0);
};
class c_contact_query_world
{
public:
 void *add(void *volume);
 void remove(void *volume);
};
struct s_1d1261
{
 short field_0;
 byte field_2;
 byte field_3;
 long field_4;
 byte *field_8;
};
struct s_1d1262
{
 char field_0;
 byte field_1[3];
 long field_4;
};
struct s_1d1263
{
 byte field_0[2];
 short field_2;
 long field_4;
 byte field_8[0x40 - 8];
};
extern hkWorld *g_51e9a4;
extern byte *g_51eca8;
extern long g_47f050;
extern s_record_pool *g_51ec00;
/* function_2266a0 stays declared ahead of function_278f00: link-time code generation walks callees in this
   order, and 0x1c4040, 0x146bf0 and 0x1c51c0 match only with it (see 0x1c4040's comment) */
void function_2266a0(long impact_index);
void function_278f00();
void __stdcall function_b9b90(long object_index, bool disable);

PRIVATE __forceinline byte *function_1d1261(long arg_0)
{
 byte *local_0 = NULL;
 if (arg_0 != NONE)
 {
  long local_1 = arg_0 & 0xffff;
  if (local_1 < g_4e0300->high_water_index)
  {
   s_1d1261 *local_2 = (s_1d1261 *)(g_4e0300->data + local_1 * g_4e0300->size);
   if (local_2->field_0 && local_2->field_0 == arg_0 >> 16 && (1 << local_2->field_3))
    local_0 = local_2->field_8;
  }
 }
 return local_0;
}

// @retail 0x1d1260
void function_1d1260(s_havok_component *arg_0)
{
 for (long local_0 = 0; local_0 < arg_0->unknown7c.size; ++local_0)
  ((c_1d1261 *)g_51e9a4)->function_30ca30(arg_0->unknown7c.data[local_0].contact);
 for (long local_1 = 0; local_1 < arg_0->rigid_bodies.size; ++local_1)
 {
  hkRigidBody *local_2 = arg_0->rigid_bodies.data[local_1].rigid_body;
  ((c_1d1261 *)local_2)->function_30bd10(g_51eca8);
  if ((arg_0->unknown04 & 1) && !local_2->m_fixed && local_2->m_motion->kind_code() != 6)
   ((c_physics_body_island_view *)local_2)->entity_observer_detach((c_havok_entity_observer *)(g_51eca8 ? g_51eca8 + 4 : NULL));
  --g_47f050;
  g_51e9a4->entity_world_remove((hkEntity *)local_2);
  function_278f00();
 }
 if (arg_0->unknown94)
 {
  byte *local_3 = ((s_1d1261 *)g_4e0300->data)[arg_0->object_index & 0xffff].field_8;
  byte *local_4 = (byte *)g_4e3b44[*(long *)local_3 & 0xffff].data;
  local_4 = (byte *)g_4e3b44[*(long *)(local_4 + 0x38) & 0xffff].data;
  local_4 = (byte *)g_4e3b44[*(long *)(local_4 + 0x24) & 0xffff].data;
  for (long local_5 = arg_0->unknown94->size - 1; local_5 >= 0; --local_5)
  {
   s_1d1262 *local_6 = &((s_1d1262 *)arg_0->unknown94->data)[local_5];
   byte *local_7 = *(byte **)(local_4 + 0x44);
   short local_8 = *(short *)(local_7 + local_6->field_0 * 0xc + 8);
   byte *local_9 = *(byte **)(local_4 + 0x2c) + local_8 * 0x68;
   if (function_1d1261(local_6->field_4))
   {
    function_b9b90(local_6->field_4, false);
    if (*(dword *)local_9 & 0x1000000)
    {
     s_1d1261 *local_10 = &((s_1d1261 *)g_4e0300->data)[local_6->field_4 & 0xffff];
     if ((1 << local_10->field_3) & 1)
     {
      byte *local_11 = local_10->field_8;
      if (*(byte *)(local_11 + 0x3dc) == 1 && *(long *)(local_11 + 0x3e4) == arg_0->object_index)
       *(long *)(local_11 + 0x3e4) = NONE;
     }
    }
   }
  }
  arg_0->unknown94->size = 0;
 }
 if (arg_0->unknown04 & 1)
 {
  while ((arg_0->unknown20 == NONE ? 0 : ((s_1d1263 *)g_51ec00->data)[arg_0->unknown20 & 0xffff].field_2) != 0)
   function_2266a0(((s_1d1263 *)g_51ec00->data)[arg_0->unknown20 & 0xffff].field_4);
  if (!(arg_0->unknown88.capacity_and_flags & 0x80000000))
   g_480118->allocate((long)arg_0->unknown88.data, (arg_0->unknown88.capacity_and_flags & 0x7fffffff) * 0x48, 0x12);
  arg_0->unknown88.data = NULL;
  arg_0->unknown88.size = 0;
  arg_0->unknown88.capacity_and_flags = 0x80000000;
 }
 if (arg_0->rigid_body)
  ((c_contact_query_world *)g_51e9a4)->remove(arg_0->rigid_body);
 if (arg_0->unknown9c)
  ((c_contact_query_world *)g_51e9a4)->remove((void *)arg_0->unknown9c);
 arg_0->flag5 = false;
}
