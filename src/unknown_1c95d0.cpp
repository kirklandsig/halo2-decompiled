#include "unknown_11c920.h"
#include "slot_handler.h"
#include "unknown_1fb7e0.h"
#include "globals.h"
#include <string.h>
// @flags /O2 /arch:SSE /Gr

struct s_1c95d0
{
 byte field_0[0x12c];
 long field_12c;
 byte field_130[8];
 short field_138;
 byte field_13a[2];
 long field_13c;
};
struct s_1c95d1
{
 byte field_0[0x26];
 short field_26;
 byte field_28[0x50 - 0x28];
};
struct s_1c95d2
{
 byte field_0[0x2c];
 real field_2c;
 byte field_30[0xc4 - 0x30];
};
long function_1c9580(long object_index, bool a);
bool function_1df5d0(short team_a, short team_b);
void function_20ff50(long object_index);
void __stdcall function_210180(long value);
void __stdcall function_1fbac0(long unknown, long unit_index, bool unknown2, long unknown3, s_1fbac0_event *event);
long function_26b230(long clump_index, long prop_index);
extern s_record_pool *g_502420;
extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// score 3 (was 3): what is left is that retail stores the lookup's result
// Round 108 fast trials: swapped post-call tests 147; nested field-first tests 3;
// complemented disjunction 3; two-use actor-index snapshot 3; lookup result declared early
// uninitialized/initialized 3/3; const lookup result 3; actor/target pointer declarations before
// the condition 3. Original restored; lookup-result store still precedes retail's field reload/compare.
// score 3 (was 62): what is left is that retail stores the lookup's result
// to its stack home after it reloads the object's field and compares it, ours
// before (the barrier keeps the store above the reload). Retail's LTCG treats
// the 0x1c9580 call as writing memory: with a global store added to that
// callee (diagnostic) the reload and the late store come out as in retail;
// BLOCKED-BY 0xbadc0, which ours sees as read-only. No barrier 165; callee
// bool read through its address, no barrier 165; result kept in the second
// parameter 3; result flag declared before the lookup 10.
// Round 183 (lane 3, fast): the barrier before the lookup call 5; right after the call 4; the result
// flag declared before the call (barrier after it) 10; the event record declared before the call 3; the
// record declared before the barrier 3. Retail keeps g_4e0300 in ESI across the call but reloads the
// object's field, so its 0x1c9580 is seen as writing through pointers only.
// Round 37 (fast): result flag cleared after the barrier 4; result declared, flag cleared, then the
// lookup 10; the `arg_2 != 9` test named first 3; event record before the lookup with the flag after
// the barrier 4; flag after the event record 4; a read-only barrier (`_ReadBarrier`) is not an
// intrinsic in this compiler (becomes a call, 172).
// @retail 0x1c95d0
void function_1c95d0(long arg_0, long arg_1, short arg_2, real arg_3)
{
 if (g_4f55d0->active)
 {
  s_1c95d0 *local_0 = (s_1c95d0 *)object_get(arg_0);
  if (local_0->field_12c != NONE || local_0->field_13c != NONE)
  {
   long local_1 = function_1c9580(arg_1, arg_2 != 9);
   bool local_2 = false;
   /* retail reads the object's fields again after the lookup call */
   _ReadWriteBarrier();
   s_1fb7e0_data local_3;
   if (local_0->field_12c != NONE && local_1 != NONE)
   {
    s_actor_view *local_4 = actor_get(local_0->field_12c);
    s_1c95d0 *local_5 = (s_1c95d0 *)object_get(local_1);
    if (local_0->field_138 != local_5->field_138 &&
        function_1df5d0(local_0->field_138, local_5->field_138))
    {
     bool local_6 = false;
     bool local_7 = true;
     if (local_4->unknown07c != NONE)
      local_6 = ((s_1c95d1 *)g_502420->data)[local_4->unknown07c & 0xffff].field_26 < 3;
     switch (arg_2)
     {
     case 3:
      if (!local_6)
       local_7 = false;
      local_6 = false;
      break;
     case 4:
     case 9:
      if (!local_6)
       local_7 = false;
      break;
     }
     if (local_7)
     {
      local_3.unknown00 = 1;
      local_3.unknown04 = NONE;
      ((short *)&local_3.unknown08)[0] = local_5->field_138;
      ((short *)&local_3.unknown08)[1] = local_0->field_138;
      *(short *)&local_3.unknown0c = local_6 != 0;
      local_2 = true;
     }
    }
   }
   function_20ff50(arg_0);
   if (local_0->field_13c != NONE)
    function_210180(arg_0);
   function_20ba60(0, arg_0, local_1, NONE, arg_2, local_2 ? &local_3 : NULL);
   if (local_0->field_12c != NONE)
   {
    long actor_index = local_0->field_12c;
    s_1fb7e0_data local_12 = { 6, NONE };
    s_actor_view *local_4 = actor_get(actor_index);
    s_1fbac0_event local_8;
    local_8.unknown00 = 0;
    local_8.unknown02 = 1;
    local_8.data = local_12;
    function_1fbac0(actor_index, arg_0, true, 1, &local_8);
    long local_9 = local_4->unknown07c;
    if (local_9 != NONE)
    {
     long local_10 = function_26b230(local_9, local_1);
     if (local_10 != NONE)
     {
      s_1c95d2 *local_11 = &((s_1c95d2 *)g_50241c->data)[local_10 & 0xffff];
      local_11->field_2c += arg_3 > 1.0f ? 1.0f : arg_3;
     }
    }
   }
  }
 }
}
#pragma function(_ReadWriteBarrier)
