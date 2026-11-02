#include "unknown_11c920.h"
#include "unknown_1cec30.h"
#include "bit_vector.h"
#include <string.h>
// @flags /O2 /arch:SSE /Gr

struct s_type_1a7926;
struct s_section_lists;
struct s_physics_model_owner;
struct s_physics_body_groups;
struct s_1d7300;
struct s_1d5bf0
{
 s_1d7300 *field_0;
 char field_4[64];
 long field_44;
 bool field_48;
};
struct s_1d5941
{
 byte field_0[0x48];
 byte *field_48;
 byte *field_4c;
 transform4x3f *field_50;
};
struct s_1d5942
{
 short field_0;
 byte field_2[0x1a - 2];
 short field_1a;
 byte field_1c[0x90 - 0x1c];
};
struct s_1d5943
{
 byte field_0[0x38];
 long field_38;
 s_1d5942 *field_3c;
 byte field_40[0xc8 - 0x40];
 long field_c8;
 byte *field_cc;
};
struct s_1d5944
{
 long field_0;
 struct { char field_0[64]; long field_40; } field_4[257];
};

bool function_20a9a0(s_type_1a7926 *arg_1, long arg_0);
s_section_lists *function_181a80(s_section_lists *arg_0, long arg_1, bool arg_2);
bool function_1d5bf0(s_havok_component *arg_0, s_physics_model_owner *arg_1, char const *arg_2,
 bool arg_3, long arg_4, long *arg_5, dword *arg_6, s_physics_body_groups *arg_7, s_1d5bf0 *arg_8);
void __cdecl function_2d9160(void *arg_0, long arg_1, long arg_2);
void function_1d0080(hkRigidBody *arg_0, s_havok_component *arg_1, byte const *arg_2, long arg_3, bool arg_4);
void function_1d6ad0(s_havok_component *arg_0, s_physics_model_owner *arg_1);

// score 9 (was 9): retail zeroes the eight words of the mask with eax and
// ours with ecx, so the zeroing and the eight stores differ (retail picks ecx
// only where eax holds a live value; nothing visible is live in eax here).
// Tried: field read through the owner at each use 9; mask declared first 9;
// existing bit_vector_fill 9 (188 under the old inline_depth(0) pair, now removed).
// Round 55 (plain checks; --fast moves the component into EBX here): memset before the node
// list local 9; `dword local_2[8] = { 0 };` at the memset 20 (the rest still ECX); loop counter
// declared before its for 9; memset through a forceinline wrapper 9; node list local read before
// the section call 22. Diagnostics: the first loop from 1 keeps ECX; the memset ahead of the
// section call gives EAX (shared with its `false` push).
// Round 63 (plain): section-list result used at both queries 65; per-call result before the
// clear 49; body count before the clear 156 (696 bytes); clear in the loop initializer 9;
// model pointer declared at function scope 9; whole-array destination 9; element-size count 9;
// literal 0x20 count 9.
// Round 49 (plain): mask declared at the clear 9; loop bound as `field_38 > index` 11; header test
// `(...) == 0` 9; header index product `12 * index` 9; switch result declared at function scope 9.
// @retail 0x1d5940
long __stdcall function_1d5940(s_havok_component *arg_0, bool arg_1, bool arg_2, bool arg_3)
{
 long local_0 = 0;
 long local_1;
 dword local_2[8];
 s_1d5941 local_3;
 char local_4[64];
 s_1d5bf0 local_5[32];
 s_1d5944 local_6;
 byte *local_7 = (byte *)g_4e0300->data + (arg_0->object_index & 0xffff) * 12;
 if (!(local_7[2] & 1) && !((1 << local_7[3]) & 0x80))
  arg_1 = true;
 if (function_20a9a0((s_type_1a7926 *)&local_3, arg_0->object_index))
 {
  function_181a80((s_section_lists *)&local_6, arg_0->object_index, false);
  s_1d5943 *local_8 = (s_1d5943 *)local_3.field_48;
  memset(local_2, 0, sizeof(local_2));
  for (long local_9 = 0; local_9 < local_8->field_38; ++local_9)
  {
   s_1d5942 *local_10 = &local_8->field_3c[local_9];
   long local_11;
   switch (local_10->field_1a)
   {
   case 0: local_11 = 2; break;
   case 1: local_11 = 3; break;
   case 2: local_11 = 4; break;
   case 3: local_11 = 5; break;
   case 4: local_11 = 6; break;
   case 5: local_11 = 7; break;
   default: __assume(0);
   }
   if (arg_1)
    local_11 = 7;
   if (arg_2 && local_11 != 7 &&
       (local_10->field_0 == NONE || !(local_8->field_cc[local_10->field_0 * 12 + 4] & 1)))
    local_11 = 6;
   local_4[local_9] = (char)local_11;
  }
  local_1 = 0;
  long local_12 = 0;
  if (function_1d5bf0(arg_0, (s_physics_model_owner *)&local_3, local_4, arg_3,
       NONE, &local_1, local_2, (s_physics_body_groups *)&local_6, &local_5[local_12]))
   local_12 = 1;
  if (local_1 >= 0)
   local_0 = local_1;
  for (long local_13 = 0; local_13 < ((s_1d5943 *)local_3.field_48)->field_c8; ++local_13)
  {
   if (!bit_vector_test(local_2, local_13))
   {
    local_1 = 0;
    if (function_1d5bf0(arg_0, (s_physics_model_owner *)&local_3, local_4, arg_3,
         local_13, &local_1, local_2, (s_physics_body_groups *)&local_6, &local_5[local_12]))
     ++local_12;
    if (local_0 <= local_1)
     local_0 = local_1;
   }
  }
  if (local_12 > 0)
  {
   s_havok_array60 *local_15 = &arg_0->rigid_bodies;
   if ((local_15->capacity_and_flags & 0x7fffffff) < local_12)
    function_2d9160(local_15, local_12, 0x60);
   for (long local_14 = 0; local_14 < local_12; ++local_14)
    function_1d0080((hkRigidBody *)local_5[local_14].field_0, arg_0,
     (byte const *)local_5[local_14].field_4, local_5[local_14].field_44, local_5[local_14].field_48);
  }
  function_1d6ad0(arg_0, (s_physics_model_owner *)&local_3);
  arg_0->unknown04 |= 0x800;
 }
 return local_0;
}

void __stdcall function_1d77d0(s_havok_component *arg_0, s_physics_model_owner *arg_1);

/* picks the rigid body that drives the lowest node of the model (unknown18)
   and that node (unknown19); a lone body without a node sets flag 0x200 */
// @retail 0x1d6ad0
void function_1d6ad0(s_havok_component *arg_0, s_physics_model_owner *arg_1)
{
 s_1d5941 *owner = (s_1d5941 *)arg_1;
 long count = arg_0->rigid_bodies.size;
 arg_0->unknown18 = NONE;
 arg_0->unknown19 = NONE;
 long lowest = 0x7fffffff;
 for (long i = 0; i < arg_0->rigid_bodies.size; i++)
 {
  s_havok_component_rigid_body const *body = &arg_0->rigid_bodies.data[i];
  if (body->node_count > 0)
  {
   s_1d5942 *node = &((s_1d5943 *)owner->field_48)->field_3c[body->nodes[0]];
   if (node->field_0 == NONE)
   {
    arg_0->unknown18 = (char)i;
    arg_0->unknown19 = (char)node->field_0;
    break;
   }
   if (node->field_0 < lowest)
   {
    arg_0->unknown18 = (char)i;
    lowest = node->field_0;
    arg_0->unknown19 = (char)lowest;
   }
  }
 }
 if (count == 1 && arg_0->unknown19 == NONE)
  arg_0->unknown04 |= 0x200;
 else
  arg_0->unknown04 &= ~0x200;
 function_1d77d0(arg_0, arg_1);
}

