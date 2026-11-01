#include "unknown_11c920.h"
#include "unknown_1cec30.h"
#include "unknown_1c3b20.h"
#include "unknown_1c3b70.h"
#include <new>
// @flags /O2 /Gr

class c_1d7390
{
public:
 virtual void slot0() = 0;
 virtual void slot1() = 0;
 virtual void slot2() = 0;
 virtual void slot3() = 0;
 virtual void *function_1d7390(long arg_0, long arg_1) = 0;
};
struct s_1d70d0;
class c_component_rotation
{
public:
 hkVector4 value;
 void set(hkRotation const &rotation);
};
struct s_1d8d60
{
 __m128 field_0;
 void function_1d8d60();
};
struct c_transformed_point
{
 __m128 value;
 void transform(const void *matrix, const __m128 *point);
};
struct s_311340
{
 long field_0;
 s_1d70d0 *field_4;
 long field_8;
 void *field_c;
 long field_10;
 dword field_14;
 byte field_18[8];
 hkVector4 field_20;
 c_component_rotation field_30;
 byte field_40[0x20];
 hkRotation field_60;
 hkVector4 field_90;
 real field_a0, field_a4, field_a8, field_ac, field_b0;
 byte field_b4;
 s_311340();
 ~s_311340()
 {
  if (!(field_14 & 0x80000000))
   g_480118->allocate((long)field_c, (field_14 & 0x7fffffff) * 8, 0x12);
 }
};
class c_311ba0
{
public:
 long field_0;
 word field_4;
 byte field_6[0x9a];
 c_311ba0(const s_311340 *arg_0);
};


struct s_47f048_object;
extern s_47f048_object *g_47f048;
volatile byte g_47f06d = 1;
c_shape_owner *function_1c3b20(c_havok_reference_counted *arg_0);
void havok_entity_component_index_set(hkEntity *entity, long component_index);

// @retail 0x1c3b70
void function_1c3b70()
{
 c_shape_global_owner *local_0 = (c_shape_global_owner *)((c_1d7390 *)g_480118)->function_1d7390(0x30, 0x22);
 local_0->flags = 0x30;
 local_0 = new (local_0) c_shape_global_owner;
 c_shape_owner *local_1 = function_1c3b20((c_havok_reference_counted *)local_0);
 *(volatile dword *)local_1->field_8 = 0xcabcabb0;
 c_1c5910 *local_2 = NULL;
 if (g_47f06d)
 {
  c_1c5910 *local_6 = (c_1c5910 *)((c_1d7390 *)g_480118)->function_1d7390(0x10, 0x22);
  local_6->field_8 = 0;
  local_6->flags = 0x10;
  local_6->unknown06 = 1;
  local_6 = new (local_6) c_1c5910;
  local_6->field_c = (c_havok_reference_counted *)local_1;
  ++local_1->references;
  local_2 = local_6;
  local_6->field_8 = 0xcabcabb0;
 }
 s_311340 local_3;
 local_3.field_4 = (s_1d70d0 *)(g_47f06d ? (void *)local_2 : (void *)local_1);
 local_3.field_0 = 1;
 local_3.field_b4 = 7;
 c_311ba0 *local_4 = (c_311ba0 *)((c_1d7390 *)g_480118)->function_1d7390(0xa0, 0x28);
 local_4->field_4 = 0xa0;
 local_4 = new (local_4) c_311ba0(&local_3);
 g_47f048 = (s_47f048_object *)local_4;
 havok_entity_component_index_set((hkEntity *)local_4, NONE);
 havok_reference_remove((c_havok_reference_counted *)local_1);
 havok_reference_remove((c_havok_reference_counted *)local_0);
 if (g_47f06d) havok_reference_remove((c_havok_reference_counted *)local_2);
}
