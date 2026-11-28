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
void function_184060(long arg_0, long arg_1, s_type_1e6529 *arg_2, long arg_3);

typedef void *(__fastcall *constructor_proc)(void *);
void __stdcall vector_constructor_iterator(void *arg_0, unsigned arg_1, int arg_2, constructor_proc arg_3);

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

// score 1088 (was 1088): BLOCKED-BY 0x20670: the full build expands the compiler's array-constructor helper for
// the 16 damage entries into an in-line loop where retail calls 0x20670 (`push 0x1da540; push 0x10; push 0x94`);
// the reduced image keeps the call (985, 3660 bytes). Beyond it: retail's frame is 4 bytes smaller, its zero register
// is EBX (ours EBP) and the entry count's zero is stored after the array is built (ours at entry). Round 158 (m3 lane
// 14, fast, with 0x20670 0xd66d0 0xe58e0 0x184060 0x1c58a0 0x1cfdf0 0x1d1c10 0x1da550 0x1da6d0 0x1da9e0 listed):
// `long local_7 = 0;` declared after the entry array 951 (lead: a declaration moved later); with local_8 there too
// 951; the mass total assigned after its declaration 985; the object flag as `!(local_5[2] & 0x20)` 985; the inverse
// mass computed once 1063.
// M3 lane 6 r248 (H2M_FAST with the same ten callees, control 985 at 3660 B; plain control 1088): the entry count's
// zero declared beside the second count, before it 951, after it 951 (plain 1072 at 3677 B: a declaration moved later,
// so match-only, and the function cannot match while the iterator expands), both counts after the entry array 951,
// after the object flag 951, just before the contact loop 1066. On the 951 forms the reduced image still differs
// from the first instruction: frame 0xc60 against 0xc5c, the zero in EBP (retail EBX) and the component pointer's
// home at the frame's bottom (retail 0xc above it). Body unchanged.
// M3 lane 5 r253 (fast screen, control 985 at 3660 B; plain 1088). Read from the register allocator: the zero takes EBP,
// its own first-pass preference, because EBX carries 4600 of last-register penalties from four locals; retail's zero is
// in EBX. Forms: the final damage loop through a named entry pointer 1004 (3657 B); the final link loop through a named
// link pointer 985 (same code); the link record filled through a named pointer 999; all three 992; the object flag
// computed after the speed magnitude 983 (a statement order, two points); the flag tested at its use with no local
// 1054 (3676 B). Still BLOCKED-BY 0x20670. Body unchanged.
// M3 lane 3 r321 (H2M_FAST with the same ten callees, control 985 at 3660 B, aligned 633 rows; plain 1088). Read on
// the reduced image, where the iterator call stays: retail tests both entry counts unsigned (`cmp [count], 0x10; jae`
// at 0x1d9b80 and 0x1d9bda) yet bounds its two final loops signed (`test edi, edi; jle`), so the counts are signed and
// the bound is an unsigned one; and retail keeps the contact index and both counts in memory with nothing strength
// reduced (`imul esi, esi, 0x94` + `lea esi, [esp+esi+0x32c]` at the damage call, `imul eax, eax, 0x2c` for the link
// record, `lea ecx, [eax+eax*8]` for the contact, `inc dword ptr [count]`), where ours carries a stepped pointer for
// each of the three; ours also keeps the fixed-body flag in BL where retail has it in a stack byte and the per-contact
// flag in BL. Forms: both tests against the arrays' sizeof-based element counts 985 (the two `jae` appear, aligned
// 630; plain 1088, no gain, not kept); the counts declared unsigned with the final loops cast back 985 (same code); a
// dead pointer to each count 985 and the damage count stepped through a pointer in an inline step 985 (both same code:
// an address that does not escape changes nothing); the damage count stepped by the call's result (`count += f()`) 1009
// (3644 B): the entry pointer is then retail's multiply and the frame is retail's 0xc5c, but the step is `movzx; add`
// where retail has `test al, al; je; inc`; the contact index declared ahead of its loop 985 (same code); diagnostic,
// the contact index stepped by a value the compiler cannot fold: its offset is no longer carried and the frame is
// 0xc5c again, while both entry pointers stay stepped (1000), so each of the three is reduced on its own and retail
// has a cause for each or one that makes all three memory variables. Still BLOCKED-BY 0x20670. Body unchanged.
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
 s_collision_damage_entry local_3[16];
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
   if (local_22->m_fixed || local_22->m_motion->kind_code() == 6)
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
