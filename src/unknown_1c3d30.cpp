#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "havok_reference.h"
#include "unknown_184250.h"
#include <float.h>
// @flags /O2 /arch:SSE /Gr

void __cdecl function_2d8910(long arg_0);
void __cdecl function_2d8890();
class c_2d8ab0
{
public:
 void function_2d8ab0();
};
void function_146bf0();
void function_278f00();
void function_1c4260();
void function_1c4a20(hkEntity const *entity);
struct s_311100
{
 real field_0;
 real field_4;
};
class c_311100
{
public:
 void function_311100(s_311100 const *arg_0);
};
struct s_278300_references;
void function_278300(s_278300_references *references);
struct s_2797a0_group
{
 byte unknown00[0x3c];
 hkEntity **entities;
 long count;
};
struct s_2797a0_groups
{
 byte unknown00[8];
 s_2797a0_group **first;
 long first_count;
 long unknown10;
 s_2797a0_group **second;
 long second_count;
};
struct s_2797a0_iterator
{
 byte unknown00;
 bool second;
 byte unknown02[2];
 long group;
 long index;
 s_2797a0_groups *groups;
 long mode;
};
bool function_2797a0(s_2797a0_iterator *iterator);
struct s_1c3d30
{
 byte field_0[8];
 c_havok_reference_counted **field_8;
 long field_c;
 long field_10;
 long field_14;
 byte field_18;
};
extern hkWorld *g_51e9a4;
extern byte *g_51eca8;
extern short g_51ecb0;
c_2d8ab0 *g_47f044;

/* steps the Havok world for this tick, then runs the post-step pass over every entity the
   island iterator yields and releases the bodies queued during the step */
// @retail 0x1c3d30
void function_1c3d30()
{
 if (g_51e9a4)
 {
  real local_0 = g_510c54->rate;
  function_2e90a0(s_184251(false));
  ((s_1c3d30 *)g_51eca8)->field_18 = true;
  /* read through a pointer: retail stores field_18 before this load */
  if (*(c_2d8ab0 *const *)&g_47f044)
  {
   function_2d8910(2000000);
   function_2d8890();
  }
  s_311100 local_1;
  local_1.field_0 = local_0;
  local_1.field_4 = 1.0f / local_0;
  function_146bf0();
  _control87(0x9001f, 0x8001f);
  _mm_setcsr(_mm_getcsr() | 0x1f80);
  ((c_311100 *)g_51e9a4)->function_311100(&local_1);
  _mm_setcsr(_mm_getcsr() & 0xffffffc0);
  _clearfp();
  _control87(0x9001f, 0xfffff);
  function_278f00();
  function_1c4260();
  function_146bf0();
  if (g_47f044)
   g_47f044->function_2d8ab0();
  s_1c3d30 *local_2 = (s_1c3d30 *)g_51eca8;
  local_2->field_18 = false;
  s_2797a0_iterator local_3;
  local_3.groups = (s_2797a0_groups *)g_51e9a4;
  local_3.mode = 0;
  local_3.second = false;
  local_3.group = NONE;
  local_3.index = NONE;
  local_3.unknown00 = true;
  ++g_51ecb0;
  while (function_2797a0(&local_3))
  {
   function_1c4a20(!local_3.second ? local_3.groups->first[local_3.group]->entities[local_3.index]
    : local_3.groups->second[local_3.group]->entities[local_3.index]);
  }
  --g_51ecb0;
  while (local_2->field_14 + 1 < local_2->field_c)
  {
   ++local_2->field_14;
   c_havok_reference_counted *local_6 = local_2->field_8[local_2->field_14];
   havok_reference_remove(local_6);
   if (!((hkRigidBody *)local_6)->body_is_simulating().m_bool)
    function_1c4a20((hkEntity *)local_6);
   local_2 = (s_1c3d30 *)g_51eca8;
  }
  function_278300((s_278300_references *)local_2);
 }
}
