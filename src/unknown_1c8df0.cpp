#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
// @flags /O2 /arch:SSE /Gr

struct s_collision_result_1697c0
{
 long field_0;
 real field_4;
 point3f field_8;
 byte field_14[0x24 - 0x14];
 short field_24;
 byte field_26[0x40 - 0x26];
 long field_40;
 byte field_44[0x5c - 0x44];
};
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
 long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *collision);
bool function_16a040(long flags, point3f const *point0, point3f const *point1, long ignore_object_index,
 long ignore_unit_index, s_collision_result_1697c0 *result);
bool function_11c010(short row, short column);
real function_30bf0(vector3f *v);
real magnitude3d(vector3f const *v);
extern vector3f *g_4687bc;



// Rates the probe from the source point to the target: 0 clear, 1 clear beside the line, 2 to 4 graded by where
// the blocking hit lies. Parameter order (source cell, source, target, target cell, ...): with the two points ahead of the
// target cell this body adds the centre as a memory operand in both x rows as retail does, and every caller's
// argument setup stays retail's.
// @retail 0x1c8df0
short __stdcall function_1c8df0(short arg_2, point3f const *arg_1, point3f const *arg_4, short arg_3,
 long arg_0, long arg_5, bool arg_6, bool arg_7, bool arg_8, long *arg_9)
{
 short category;
 real local_1;
 long local_2 = NONE;
 if (arg_9)
  *arg_9 = NONE;
 if (arg_2 != NONE && arg_3 != NONE && !(char)function_11c010(arg_2, arg_3))
 {
  category = 4;
  local_1 = 0.0f;
 }
 else
 {
  s_collision_result_1697c0 local_6;
  local_6.field_24 = NONE;
  long local_4 = arg_8 ? 0x15808c2f : 0x5808c2f;
  if (arg_6)
   local_4 |= 0x8000000;
  if (arg_7)
   local_4 &= ~0x20;
  bool local_7;
  {
   vector3f local_5;
   vector3d_from_points3d(arg_1, arg_4, &local_5);
   if (!function_1697c0(local_4, arg_1, &local_5, arg_0, NONE, &local_6))
    local_7 = true;
   else
   {
    local_7 = false;
    local_1 = local_6.field_4;
    local_2 = local_6.field_40;
   }
  }
  if ((short)arg_5 != 0)
  {
   vector3f local_8;
   local_8.i = arg_1->y - arg_4->y;
   local_8.j = arg_4->x - arg_1->x;
   local_8.k = 0.0f;
   if (function_30bf0(&local_8) == 0.0f)
    local_8 = *g_4687a8;
   local_4 |= 0x10000000;
   bool hit;
   if ((short)arg_5 == 1)
   {
    point3f local_9;
    point3f local_10;
    real const spacing = 0.25f;
    local_9.x = arg_1->x + local_8.i * spacing;
    local_9.y = arg_1->y + local_8.j * spacing;
    local_9.z = arg_1->z + local_8.k * spacing;
    local_10.x = arg_1->x - local_8.i * spacing;
    local_10.y = arg_1->y - local_8.j * spacing;
    local_10.z = arg_1->z - local_8.k * spacing;
    if (local_7)
    {
     hit = function_16a040(local_4, &local_9, arg_4, arg_0, NONE, &local_6);
     if (!hit)
      hit = function_16a040(local_4, &local_10, arg_4, arg_0, NONE, &local_6);
     if (hit)
     {
      local_2 = local_6.field_40;
      local_1 = local_6.field_4;
     }
    }
    else
    {
     hit = !function_16a040(local_4, &local_9, arg_4, arg_0, NONE, &local_6);
     if (!hit)
      hit = !function_16a040(local_4, &local_10, arg_4, arg_0, NONE, &local_6);
    }
   }
   else if (local_7)
   {
    point3f local_9;
    point3f local_10;
    point3f local_11;
    local_9.x = arg_4->x + local_8.i * 0.1f;
    local_9.y = arg_4->y + local_8.j * 0.1f;
    local_9.z = arg_4->z + local_8.k * 0.1f;
    local_10.x = arg_4->x - local_8.i * 0.1f;
    local_10.y = arg_4->y - local_8.j * 0.1f;
    local_10.z = arg_4->z - local_8.k * 0.1f;
    local_11.x = arg_4->x + g_4687bc->i * 0.1f;
    local_11.y = arg_4->y + g_4687bc->j * 0.1f;
    local_11.z = arg_4->z + g_4687bc->k * 0.1f;
    hit = function_16a040(local_4, &local_9, arg_1, arg_0, NONE, &local_6);
    if (!hit)
     hit = function_16a040(local_4, &local_10, arg_1, arg_0, NONE, &local_6);
    if (!hit)
     hit = function_16a040(local_4, &local_11, arg_1, arg_0, NONE, &local_6);
    if (hit)
    {
     local_2 = local_6.field_40;
     local_1 = local_6.field_4;
    }
   }
   else
    hit = false;
   if (hit)
   {
    category = 1;
    goto local_14;
   }
  }
  if (local_7)
   category = 0;
  else
  {
   vector3f local_13;
   vector3d_from_points3d(arg_1, arg_4, &local_13);
   real local_12 = magnitude3d(&local_13);
   if (local_12 < 1.0f)
    category = 4;
   else if (local_12 * local_1 < 1.0f)
    category = 2;
   else if (!arg_8 && (1.0f - local_1) * local_12 < 4.0f)
    category = 3;
   else
    category = 4;
  }
 }
local_14:
 if (arg_9 && !arg_8 && category >= 1)
 {
  vector3f local_15;
  vector3d_from_points3d(arg_1, arg_4, &local_15);
  if ((1.0f - local_1) * magnitude3d(&local_15) < 4.0f)
   *arg_9 = local_2;
 }
 return category;
}
