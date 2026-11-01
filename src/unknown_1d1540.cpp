#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "unknown_1efac0.h"
#include "havok_reference.h"
#include "constraint_helpers_314090.h"
#include <new>
// @flags /O2 /arch:SSE /Gr

class c_1d19d0 : public c_314710
{
public:
	long field_60;
	long field_64;
	c_1d19d0(hkEntity *arg_0, long arg_1);
	virtual s_1d1870 function_1d1870(void *arg_0);
	virtual s_1d1870 function_1d18c0(void *arg_0);
	static void operator delete(void *arg_0)
	{
		g_480118->allocate((long)arg_0, ((c_1d19d0 *)arg_0)->flags, 0x26);
	}
};

class c_1d7390
{
public:
	virtual void slot0() = 0;
	virtual void slot1() = 0;
	virtual void slot2() = 0;
	virtual void slot3() = 0;
	virtual void *function_1d7390(long arg_0, long arg_1) = 0;
};


struct c_contact_query_world { void *add(void *arg_0); };
struct s_47f048_object { void function_30be40(long arg_0); };
struct s_1d1542 { byte field_0 : 1; byte field_1 : 7; };
struct s_1d1540 { void function_30be00(void *arg_0); };
struct s_1d1541 { void *function_30d2b0(void *arg_0); };
extern hkWorld *g_51e9a4;
byte *g_51eca8;
void __stdcall function_1c39c0(void *arg_0, long arg_1);
void havok_component_rigid_body_linear_velocity_set(long arg_0, s_havok_component *arg_1, vector3f const *arg_2);
void havok_component_rigid_body_angular_velocity_set(long arg_0, s_havok_component *arg_1, vector3f const *arg_2);

// @retail 0x1d1540
void function_1d1540(s_havok_component *arg_0)
{
 if (arg_0->unknown9c) ((c_contact_query_world *)g_51e9a4)->add((void *)arg_0->unknown9c);
 if (arg_0->rigid_body) ((c_contact_query_world *)g_51e9a4)->add(arg_0->rigid_body);
 byte *local_0 = (byte *)havok_object_get(arg_0->object_index);
 for (long local_1 = 0; local_1 < arg_0->rigid_bodies.size; ++local_1)
 {
  __declspec(align(16)) hkRigidBody *local_2 = arg_0->rigid_bodies.data[local_1].rigid_body;
  ((s_1d1540 *)local_2)->function_30be00(g_51eca8);
  if ((arg_0->unknown04 & 1) && !local_2->m_fixed && local_2->m_motion->kind_code() != 6)
  {
   ((s_47f048_object *)local_2)->function_30be40((long)(g_51eca8 ? g_51eca8 + 4 : NULL));
   *(word *)((byte *)local_2 + 0x5c) = 0;
  }
  function_1c39c0(local_2, (bool)((s_1d1542 *)(local_0 + 0xc1))->field_0);
  havok_component_rigid_body_linear_velocity_set(local_1, arg_0, (vector3f *)(local_0 + 0x88));
  havok_component_rigid_body_angular_velocity_set(local_1, arg_0, (vector3f *)(local_0 + 0x94));
  c_1d19d0 *local_3 = (c_1d19d0 *)((c_1d7390 *)g_480118)->function_1d7390(0x70, 0x26);
  local_3->flags = 0x70;
  local_3 = new(local_3) c_1d19d0((hkEntity *)local_2, arg_0->object_index);
  if ((bool)((arg_0->unknown04 >> 11) & 1))
  {
   byte *local_4 = (byte *)havok_object_get(arg_0->object_index);
   byte *local_5 = g_4e3b44[*(long *)local_4 & 0xffff].bytes;
   byte *local_6 = g_4e3b44[*(long *)(local_5 + 0x38) & 0xffff].bytes;
   byte *local_7 = g_4e3b44[*(long *)(local_6 + 0x24) & 0xffff].bytes;
   if (*(real *)(local_7 + 8) != 1.0f || *(real *)(local_7 + 0xc) != 1.0f)
   {
    s_314320 local_8;
    local_3->export_description(&local_8);
    local_8.field_4 *= *(real *)(local_7 + 8);
    local_8.field_8 *= *(real *)(local_7 + 8);
    local_8.field_c *= *(real *)(local_7 + 0xc);
    local_8.field_10 *= *(real *)(local_7 + 0xc);
    local_3->import_description(&local_8);
   }
  }
  c_havok_reference_counted *local_9 = (c_havok_reference_counted *)local_3;
  if (local_9) ++local_9->reference_count;
  c_havok_reference_counted *local_10 = *(c_havok_reference_counted **)((byte *)local_2 + 0x54);
  if (local_10) havok_reference_remove(local_10);
  *(c_havok_reference_counted **)((byte *)local_2 + 0x54) = local_9;
  havok_reference_remove(local_9);
 }
 for (long local_11 = 0; local_11 < arg_0->unknown7c.size; ++local_11)
  ((s_1d1541 *)g_51e9a4)->function_30d2b0(arg_0->unknown7c.data[local_11].contact);
 arg_0->unknown04 = (arg_0->unknown04 & ~8) | 0x20;
}
