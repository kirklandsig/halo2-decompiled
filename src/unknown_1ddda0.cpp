// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include <xmmintrin.h>

struct s_bsp3d;

struct s_1ddda1
{
	word field_0;
	word field_2;
	long field_4;
};

struct s_1ddda2
{
	word field_0[2];
	word field_4[2];
	short field_8;
	short field_a;
};

struct s_1ddda3
{
	point3f field_0;
	long field_c;
};

struct s_1ddda0
{
	byte field_0[0x2c];
	s_1ddda1 *field_2c;
	long field_30;
	s_1ddda2 *field_34;
	long field_38;
	s_1ddda3 *field_3c;
};

// @retail 0x1ddda0
short function_1ddda0(s_bsp3d const *arg_0, long arg_1, point3f *arg_2)
{
	(void)&arg_1;
	(void)&arg_2;
	short local_3 = 0;
	s_1ddda0 const *local_0 = (s_1ddda0 const *)arg_0;
	long local_1 = local_0->field_2c[arg_1].field_2;
	long local_2 = local_1;
	do
	{
		s_1ddda2 const *local_4 = &local_0->field_34[local_2];
		bool local_5 = local_4->field_a == arg_1;
		arg_2[local_3++] = local_0->field_3c[local_4->field_0[local_5]].field_0;
		local_2 = local_4->field_4[local_5];
	} while (local_2 != local_1);
	return local_3;
}

struct s_1debd0
{
 word field_0;
 word field_2;
 byte field_4;
 byte field_5;
 short field_6;
};

// @retail 0x1debd0
bool function_1debd0(dword const *arg_1, s_bsp3d const *arg_0, short arg_2, long arg_3,
 short arg_4, bool arg_5, point2f const *arg_6)
{
 (void)&arg_2; (void)&arg_3; (void)&arg_4; (void)&arg_5; (void)&arg_6;
 point2f const *const *local_18 = &arg_6;
 s_1ddda0 const *local_0 = (s_1ddda0 const *)arg_0;
 s_1debd0 const *local_1 = &((s_1debd0 const *)local_0->field_2c)[arg_3];
 if (arg_1 && (local_1->field_4 & 8) && (short)local_1->field_5 < arg_2 &&
  !(arg_1[local_1->field_5 >> 5] & (1 << (local_1->field_5 & 31))))
  return false;
 s_1ddda2 const *local_2 = local_0->field_34;
 s_1ddda3 const *local_3 = local_0->field_3c;
 long local_4 = local_1->field_2;
 long local_5 = local_4;
 short const *local_6 = g_440b94[arg_4 * 2 + arg_5];
 long local_7 = local_6[0] * sizeof(real), local_8 = local_6[1] * sizeof(real);
 do
 {
  s_1ddda2 const *local_9 = &local_2[local_5];
  bool local_10 = local_9->field_a == arg_3;
  real const *local_11 = (real const *)&local_3[local_9->field_0[local_10]].field_0;
  point2f local_12 = { *(real const *)((byte const *)local_11 + local_7), *(real const *)((byte const *)local_11 + local_8) };
  point2f local_14 = { (*local_18)->x - local_12.x, (*local_18)->y - local_12.y };
  real const *local_16 = (real const *)&local_3[local_9->field_0[!local_10]].field_0;
  point2f local_17 = { *(real const *)((byte const *)local_16 + local_7) - local_12.x, *(real const *)((byte const *)local_16 + local_8) - local_12.y };
  if (local_17.y * local_14.x - local_14.y * local_17.x > 0.0f) return false;
  local_5 = local_9->field_4[local_10];
 } while (local_5 != local_4);
 return true;
}

struct s_1dea01
{
 byte field_0;
 byte field_1;
 word field_2;
};
struct s_1dea02
{
 short field_0;
 short field_2;
};
struct s_1dea00
{
 byte field_0[0xc];
 plane3f *field_c;
 long field_10;
 s_1dea01 *field_14;
 long field_18;
 s_1dea02 *field_1c;
};
struct s_296520_tree;
long function_296520(s_296520_tree *tree, long index, point2f const *point);

// @retail 0x1dea00
long function_1dea00(s_bsp3d const *arg_0, long arg_1, point3f const *arg_6,
 vector3f const *arg_7, short arg_2, dword const *arg_3, long arg_4, real arg_5, bool arg_8)
{
 (void)&arg_2; (void)&arg_3; (void)&arg_4; (void)&arg_5; (void)&arg_8;
 s_1dea00 const *local_0 = (s_1dea00 const *)arg_0;
 s_1dea01 const *local_1 = &local_0->field_14[arg_1];
 long local_2 = local_1->field_2;
 long local_3 = local_2 + local_1->field_1;
 for (; local_2 < local_3; ++local_2)
 {
  s_1dea02 const *local_4 = &local_0->field_1c[local_2];
  long local_15 = local_4->field_0;
  if ((local_15 & 0x7fff) == arg_4)
  {
   plane3f const *local_5 = &local_0->field_c[arg_4];
   real local_6 = (real)fabs(local_5->i);
   real local_7 = (real)fabs(local_5->j);
   real local_8 = (real)fabs(local_5->k);
   short local_9;
   if (local_8 >= local_7 && local_8 >= local_6) local_9 = 2;
   else if (local_7 >= local_6) local_9 = 1;
   else local_9 = 0;
   bool local_10 = (((real const *)local_5)[local_9] > 0.0f) != (bool)((local_15 >> 15) & 1);
   point3f local_11;
   local_11.x = arg_7->i * arg_5 + arg_6->x;
   local_11.y = arg_7->j * arg_5 + arg_6->y;
   local_11.z = arg_7->k * arg_5 + arg_6->z;
   short const *local_12 = g_440b94[local_9 * 2 + local_10];
   point2f local_13 = { ((real *)&local_11)[local_12[0]], ((real *)&local_11)[local_12[1]] };
   long local_14 = function_296520((s_296520_tree *)((byte *)arg_0 + 0x20), local_4->field_2, &local_13);
   if (!arg_8 || function_1debd0(arg_3, arg_0, arg_2, local_14, local_9, local_10, &local_13))
    return local_14;
  }
 }
 return NONE;
}

struct s_1de6d1
{
 short field_0;
 byte field_2[6];
};
struct s_1de6d2
{
 long field_0;
 s_1de6d1 *field_4;
 long field_8;
 plane3f *field_c;
 long field_10;
 s_1dea01 *field_14;
 long field_18;
 s_1dea02 *field_1c;
 byte field_20[0xc];
 s_1debd0 *field_2c;
};
struct s_1de6d3
{
 real field_0;
 plane3f const *field_4;
 long field_8;
 long field_c;
 long field_10;
 byte field_14;
 byte field_15;
 short field_16;
 long field_18;
 long field_1c[256];
};
struct s_1de6d0
{
 dword field_0;
 s_1de6d2 const *field_4;
 short field_8;
 dword const *field_c;
 point3f const *field_10;
 vector3f const *field_14;
 s_1de6d3 *field_18;
 long field_1c;
 byte field_20;
 long field_24;
};

PRIVATE __forceinline real function_1de6d4(vector3f const *arg_0, vector3f const *arg_1)
{
 return arg_0->k * arg_1->k + arg_0->j * arg_1->j + arg_0->i * arg_1->i;
}

// @retail 0x1de6d0
bool __stdcall function_1de6d0(s_1de6d0 *arg_0, long arg_1, real arg_2, real arg_3)
{
 (void)&arg_0; (void)&arg_1; (void)&arg_2; (void)&arg_3;
 while (!(arg_1 & 0x800000))
 {
  s_1de6d1 const *local_0 = &arg_0->field_4->field_4[arg_1 & 0x7fffff];
  plane3f const *local_1 = &arg_0->field_4->field_c[local_0->field_0];
  real local_2 = local_1->k * arg_0->field_10->z + local_1->j * arg_0->field_10->y + local_1->i * arg_0->field_10->x - local_1->d;
  real local_3 = function_1de6d4(arg_0->field_14, &local_1->n);
  real local_15 = local_3 * arg_2 + local_2;
  real local_16 = local_3 * arg_3 + local_2;
  bool local_4 = local_15 >= 0.0f;
  bool local_5 = local_16 >= 0.0f;
  if (local_4 != local_5)
  {
   bool local_6 = local_3 > 0.0f;
   bool local_17 = !local_6;
   real local_7 = -(local_2 / local_3);
   if (function_1de6d0(arg_0, *(long const *)&local_0->field_2[local_17 * 3 - 1] >> 8, arg_2, local_7)) return true;
   if (local_7 >= arg_0->field_18->field_0) return false;
   arg_0->field_24 = local_0->field_0;
   arg_2 = local_7;
   arg_1 = *(long const *)&local_0->field_2[local_6 * 3 - 1] >> 8;
  }
  else
   arg_1 = *(long const *)&local_0->field_2[local_4 * 3 - 1] >> 8;
 }
 long local_8 = NONE;
 byte local_9 = 3;
 bool local_10 = false;
 if (arg_1 != NONE)
 {
  local_8 = arg_1 & 0x7fffff;
  byte local_18 = *(byte volatile const *)&arg_0->field_4->field_14[local_8].field_0;
  local_9 = (local_18 & 1) ? 2 : 1;
 }
 dword local_11 = arg_0->field_0;
 long local_12 = NONE;
 if ((local_11 & 1) && (arg_0->field_20 == 1 || arg_0->field_20 == 2) && local_9 == 3)
  local_12 = arg_0->field_1c;
 else if ((local_11 & 2) && arg_0->field_20 == 3 && (local_9 == 1 || local_9 == 2))
  local_12 = local_8;
 else if (!(local_11 & 4) && arg_0->field_20 == 2 && local_9 == 2)
 {
  local_12 = (local_11 & 1) ? arg_0->field_1c : local_8;
  local_10 = true;
 }
 if (local_12 != NONE)
 {
  long local_13 = function_1dea00((s_bsp3d const *)arg_0->field_4, local_12, arg_0->field_10,
   arg_0->field_14, arg_0->field_8, arg_0->field_c, arg_0->field_24, arg_2, local_10);
  if (local_13 != NONE)
  {
   s_1debd0 const *local_14 = &arg_0->field_4->field_2c[local_13];
   if (!((local_14->field_4 & 2) && (local_11 & 8)) && !((local_14->field_4 & 8) && (local_11 & 0x10)))
   {
    arg_0->field_18->field_0 = arg_2;
    arg_0->field_18->field_4 = &arg_0->field_4->field_c[arg_0->field_24];
    arg_0->field_18->field_8 = local_12;
    arg_0->field_18->field_c = local_13;
    arg_0->field_18->field_10 = (short)local_14->field_0;
    arg_0->field_18->field_14 = local_14->field_4;
    arg_0->field_18->field_15 = local_14->field_5;
    arg_0->field_18->field_16 = local_14->field_6;
    return true;
   }
  }
 }
 if (local_8 != NONE)
 {
  if (arg_0->field_18->field_18 < 256)
   arg_0->field_18->field_1c[arg_0->field_18->field_18++] = local_8;
  else
   arg_0->field_18->field_1c[255] = local_8;
 }
 arg_0->field_1c = local_8;
 arg_0->field_20 = local_9;
 return false;
}

struct s_1de2c1
{
 long field_0;
 long field_4[256];
};
struct s_1de2c2
{
 s_1de2c1 field_0;
 s_1de2c1 field_404;
 s_1de2c1 field_808;
 s_1de2c1 field_c0c;
};
struct s_1de2c0
{
 s_1ddda0 const *field_0;
 short field_4;
 dword const *field_8;
 point3f const *field_c;
 real field_10;
 s_1de2c2 *field_14;
 long field_18;
 long field_1c[128];
 short field_21c;
 bool field_21e;
 bool field_21f;
 point2f field_220;
};
bool function_11e5e0(point3f const *, point3f const *, vector3f const *, real);

PRIVATE __forceinline void function_1de2c1(s_1de2c2 *arg_0, long arg_1)
{
 for (short local_0 = 0; local_0 < arg_0->field_808.field_0; ++local_0)
  if (arg_0->field_808.field_4[local_0] == arg_1) return;
 if (arg_0->field_808.field_0 < 256)
  arg_0->field_808.field_4[arg_0->field_808.field_0++] = arg_1;
}

PRIVATE __forceinline void function_1de2c4(s_1de2c2 *arg_0, long arg_1)
{
 for (short local_0 = 0; local_0 < arg_0->field_404.field_0; ++local_0)
  if (arg_0->field_404.field_4[local_0] == arg_1) return;
 if (arg_0->field_404.field_0 < 256)
  arg_0->field_404.field_4[arg_0->field_404.field_0++] = arg_1;
}

PRIVATE __forceinline void function_1de2c5(s_1de2c2 *arg_0, long arg_1)
{
 for (short local_0 = 0; local_0 < arg_0->field_0.field_0; ++local_0)
  if (arg_0->field_0.field_4[local_0] == arg_1) return;
 if (arg_0->field_0.field_0 < 256)
  arg_0->field_0.field_4[arg_0->field_0.field_0++] = arg_1;
}

PRIVATE __forceinline real function_1de2c2(point3f const *arg_0, point3f const *arg_1)
{
 real local_0;
 real *local_1 = &local_0;
 __asm
 {
  mov eax, local_1
  mov ecx, arg_0
  movss xmm0, dword ptr [ecx]
  movhps xmm0, qword ptr [ecx + 4]
  mov ecx, arg_1
  movss xmm1, dword ptr [ecx]
  movhps xmm1, qword ptr [ecx + 4]
  subps xmm0, xmm1
  mulps xmm0, xmm0
  movss xmm2, xmm0
  shufps xmm0, xmm0, 0x0e
  addss xmm2, xmm0
  shufps xmm0, xmm0, 0x39
  addss xmm2, xmm0
  movss dword ptr [eax], xmm2
 }
 return local_0;
}

// @retail 0x1de2c0
void function_1de2c0(s_1de2c0 *arg_0, long arg_1)
{
 (void)&arg_1;
 s_1debd0 const *local_0 = &((s_1debd0 const *)arg_0->field_0->field_2c)[arg_1];
 if ((local_0->field_4 & 8) && (short)local_0->field_5 < arg_0->field_4 &&
  !(arg_0->field_8[local_0->field_5 >> 5] & (1 << (local_0->field_5 & 31)))) return;
 bool local_1 = false;
 real local_2 = arg_0->field_10 * arg_0->field_10;
 long local_3 = local_0->field_2;
 do
 {
  s_1ddda2 const *local_4 = &arg_0->field_0->field_34[local_3];
  bool local_5 = local_4->field_a == arg_1;
  long local_6 = local_4->field_0[local_5];
  point3f const *local_26 = &arg_0->field_0->field_3c[local_6].field_0;
  if (function_1de2c2(local_26, arg_0->field_c) <= local_2)
  {
   function_1de2c1(arg_0->field_14, local_6);
   local_1 = true;
  }
  local_3 = local_4->field_4[local_5];
 } while (local_3 != local_0->field_2);
 local_3 = local_0->field_2;
 do
 {
  s_1ddda2 const *local_7 = &arg_0->field_0->field_34[local_3];
  bool local_8 = local_7->field_a == arg_1;
  point3f const *local_9 = &arg_0->field_0->field_3c[local_7->field_0[local_8]].field_0;
  point3f const *local_10 = &arg_0->field_0->field_3c[local_7->field_0[!local_8]].field_0;
  vector3f local_11;
  local_11.i = local_10->x - local_9->x;
  local_11.j = local_10->y - local_9->y;
  local_11.k = local_10->z - local_9->z;
  if (function_11e5e0(local_9, arg_0->field_c, &local_11, arg_0->field_10))
  {
   function_1de2c4(arg_0->field_14, local_3);
   local_1 = true;
  }
  local_3 = local_7->field_4[local_8];
 } while (local_3 != local_0->field_2);
 if (!local_1)
 {
  long local_14 = local_0->field_2;
  s_1ddda2 const *local_12 = arg_0->field_0->field_34;
  s_1ddda3 const *local_13 = arg_0->field_0->field_3c;
  long local_15 = local_14;
  bool local_28 = *(bool const volatile *)&arg_0->field_21e;
  long local_16 = local_28 + arg_0->field_21c * 2;
  long local_17 = g_440b94[local_16][0] * sizeof(real), local_18 = g_440b94[local_16][1] * sizeof(real);
  do
  {
   s_1ddda2 const *local_19 = &local_12[local_15];
   bool local_20 = local_19->field_a == arg_1;
   volatile long local_26 = local_20;
   real const *local_21 = (real const *)&local_13[local_19->field_0[local_20]].field_0;
   real const *local_24 = (real const *)&local_13[local_19->field_0[!local_20]].field_0;
   long local_22 = *(short const volatile *)&arg_0->field_21c * 2 + *(bool const volatile *)&arg_0->field_21e;
   point2f local_23 = { local_21[g_440b94[local_22][0]] - arg_0->field_220.x, local_21[g_440b94[local_22][1]] - arg_0->field_220.y };
   point2f local_25 = { *(real const *)((byte const *)local_24 + local_17) - arg_0->field_220.x,
    *(real const *)((byte const *)local_24 + local_18) - arg_0->field_220.y };
   if (local_25.y * local_23.x - local_23.y * local_25.x < 0.0f) return;
   local_15 = local_19->field_4[local_26];
  } while (local_15 != local_14);
 }
 function_1de2c5(arg_0->field_14, arg_1);
}

struct s_1de220
{
 real field_0;
 real field_4;
 real field_8;
 short field_c;
 short field_e;
};

// @retail 0x1de220
void __stdcall function_1de220(s_1de2c0 *arg_0, long arg_1)
{
 (void)&arg_0; (void)&arg_1;
 while (!(arg_1 & 0x8000))
 {
  s_1de220 const *local_0 = &(*(s_1de220 const **)((byte const *)arg_0->field_0 + 0x24))[arg_1 & 0x7fff];
  real local_1 = local_0->field_4 * arg_0->field_220.y + local_0->field_0 * *(real const volatile *)&arg_0->field_220.x - local_0->field_8;
  bool local_2 = local_1 <= arg_0->field_10;
  bool local_3 = local_1 >= 0.0f - arg_0->field_10;
  if (local_2) function_1de220(arg_0, local_0->field_c);
  if (!local_3) return;
  arg_1 = local_0->field_e;
 }
 function_1de2c0(arg_0, arg_1 & 0x7fff);
}

struct s_slot_entry_list;
struct s_collision_bsp_test_vector_result;

// @retail 0x1de630
bool function_1de630(dword arg_0, s_slot_entry_list *arg_1, s_collision_bsp_test_vector_result *arg_2,
 real arg_3, long arg_4, byte const *arg_5, point3f const *arg_6, vector3f const *arg_7)
{
 (void)&arg_4; (void)&arg_5; (void)&arg_6; (void)&arg_7;
 s_1de6d0 local_0;
 local_0.field_0 = arg_0;
 local_0.field_4 = (s_1de6d2 const *)arg_1;
 local_0.field_8 = (short)arg_4;
 local_0.field_c = (dword const *)arg_5;
 local_0.field_10 = arg_6;
 local_0.field_14 = arg_7;
 local_0.field_18 = (s_1de6d3 *)arg_2;
 ((s_1de6d3 *)arg_2)->field_0 = arg_3 < 0.0f ? 0.0f : arg_3;
 ((s_1de6d3 *)arg_2)->field_18 = 0;
 local_0.field_1c = NONE;
 local_0.field_20 = 0;
 local_0.field_24 = NONE;
 real local_1 = arg_3 < 0.0f ? 0.0f : (arg_3 > 1.0f ? 1.0f : arg_3);
 return function_1de6d0(&local_0, 0, 0.0f, local_1);
}

PRIVATE __forceinline bool function_1ddea1(s_1de2c0 const *arg_0, long arg_1)
{
 for (short local_0 = 0; local_0 < arg_0->field_18; ++local_0)
  if (arg_0->field_1c[local_0] == arg_1) return true;
 return false;
}

// @retail 0x1ddea0
void __stdcall function_1ddea0(s_1de2c0 *arg_0, long arg_1)
{
 (void)&arg_0; (void)&arg_1;
 s_1de6d2 const *local_0 = (s_1de6d2 const *)arg_0->field_0;
 if (!(arg_1 & 0x800000))
 {
  s_1de6d1 const *local_1 = &local_0->field_4[arg_1];
  long local_2 = *(long const *)&local_1->field_2[-1] >> 8;
  long local_3 = *(long const *)&local_1->field_2[2] >> 8;
  if (!(local_2 & 0x800000)) _mm_prefetch((char const *)&local_0->field_4[local_2], _MM_HINT_NTA);
  else if (local_2 != NONE) _mm_prefetch((char const *)&local_0->field_14[local_2 & 0x7fffff], _MM_HINT_NTA);
  if (!(local_3 & 0x800000)) _mm_prefetch((char const *)&local_0->field_4[local_3], _MM_HINT_NTA);
  else if (local_3 != NONE) _mm_prefetch((char const *)&local_0->field_14[local_3 & 0x7fffff], _MM_HINT_NTA);
  long local_4 = local_1->field_0;
  plane3f const *local_5 = &local_0->field_c[local_4];
  real local_6 = local_5->k * arg_0->field_c->z + local_5->j * arg_0->field_c->y + local_5->i * arg_0->field_c->x - local_5->d;
  bool local_7 = local_6 < arg_0->field_10;
  bool local_8 = local_6 > 0.0f - arg_0->field_10;
  if (local_8 && local_7)
  {
   arg_0->field_1c[arg_0->field_18++] = local_4 | 0xffff8000;
   function_1ddea0(arg_0, local_2);
   if (arg_0->field_21f && arg_0->field_14->field_c0c.field_0 > 0) return;
   --arg_0->field_18;
   arg_0->field_1c[arg_0->field_18++] = (word)local_1->field_0 & 0x7fff;
   function_1ddea0(arg_0, local_3);
   if (arg_0->field_21f && arg_0->field_14->field_c0c.field_0 > 0) return;
   --arg_0->field_18;
  }
  else
   function_1ddea0(*(s_1de2c0 *volatile *)&arg_0, *(long const *)&local_1->field_2[local_8 * 3 - 1] >> 8);
 }
 else if (arg_1 != NONE)
 {
  long local_9 = arg_1 & 0x7fffff;
  s_1dea01 const *local_10 = &local_0->field_14[local_9];
  if (arg_0->field_14->field_c0c.field_0 < 256)
   arg_0->field_14->field_c0c.field_4[arg_0->field_14->field_c0c.field_0++] = local_9;
  if (arg_0->field_21f) return;
  for (long local_11 = local_10->field_2; local_11 < local_10->field_2 + local_10->field_1; ++local_11)
  {
   s_1de6d2 const *local_12 = (s_1de6d2 const *)arg_0->field_0;
   s_1dea02 const *local_13 = &local_12->field_1c[local_11];
   if (function_1ddea1(arg_0, local_13->field_0))
   {
    plane3f const *local_15 = &local_12->field_c[local_13->field_0 & 0x7fff];
    real local_16 = local_15->k * arg_0->field_c->z + local_15->j * arg_0->field_c->y + local_15->i * arg_0->field_c->x - local_15->d;
    real local_17 = 0.0f - local_16;
    point3f local_18;
    local_18.x = local_17 * local_15->i + arg_0->field_c->x;
    local_18.y = local_17 * local_15->j + arg_0->field_c->y;
    local_18.z = local_15->k * local_17 + arg_0->field_c->z;
    real local_19 = (real)fabs(local_15->i);
    real local_20 = (real)fabs(local_15->j);
    real local_21 = (real)fabs(local_15->k);
    short local_22;
    if (local_21 >= local_20 && local_21 >= local_19) local_22 = 2;
    else if (local_20 >= local_19) local_22 = 1;
    else local_22 = 0;
    arg_0->field_21c = local_22;
    arg_0->field_21e = (((real const *)local_15)[local_22] > 0.0f) != (bool)(((word)local_13->field_0 >> 15) & 1);
    short const *local_23 = g_440b94[local_22 * 2 + arg_0->field_21e];
    arg_0->field_220.x = ((real const *)&local_18)[local_23[0]];
    arg_0->field_220.y = ((real const *)&local_18)[local_23[1]];
    function_1de220(arg_0, local_13->field_2);
   }
  }
 }
}

// @retail 0x1dde10
bool function_1dde10(s_bsp3d const *arg_0, short arg_1, dword const *arg_2,
 point3f const *arg_3, real arg_4, s_1de2c2 *arg_5)
{
 (void)&arg_3; (void)&arg_4;
 s_1de2c0 local_0;
 local_0.field_0 = (s_1ddda0 const *)arg_0;
 local_0.field_4 = arg_1;
 local_0.field_8 = arg_2;
 local_0.field_c = arg_3;
 local_0.field_10 = arg_4;
 local_0.field_14 = arg_5;
 local_0.field_18 = 0;
 local_0.field_21f = false;
 arg_5->field_c0c.field_0 = 0;
 arg_5->field_0.field_0 = 0;
 arg_5->field_404.field_0 = 0;
 arg_5->field_808.field_0 = 0;
 function_1ddea0(&local_0, 0);
 return arg_5->field_0.field_0 > 0 || arg_5->field_404.field_0 > 0 || arg_5->field_808.field_0 > 0;
}
