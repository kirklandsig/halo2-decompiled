#include "unknown_11c920.h"
#include "unknown_1d5120.h"
#include <float.h>
#include <new>
// @flags /O2 /arch:SSE /Gr

extern hkWorld *g_51e9a4;


long __stdcall function_1d5940(s_havok_component *component, bool first, bool second, bool third);
__declspec(noinline) void function_1d4360(s_havok_component *component, transform4x3f *volatile result);
struct s_havok_transform;
void function_181f80(s_havok_transform *result, transform4x3f const *matrix);
bool function_f1320(long object_index);
bool function_205510(s_havok_component *component, void const *definition_physics, void *buffer);

class c_1d56f0_shape
{
public:
    virtual void slot0() = 0;
    virtual void slot1() = 0;
    virtual void slot2() = 0;
    virtual void slot3() = 0;
    virtual void slot4() = 0;
    virtual void slot5() = 0;
    virtual void slot6() = 0;
    virtual void slot7() = 0;
    virtual void slot8() = 0;
    virtual void slot9() = 0;
    virtual long slot10() = 0;
};

__declspec(noinline) void function_1d56f0(s_havok_component *component);

// @retail 0x1d56f0
void function_1d56f0(s_havok_component *component)
{
    byte *object = *(byte **)((byte *)g_4e0300->data + (component->object_index & 0xffff) * 12 + 8);
    byte *definition = g_4e3b44[*(long *)object & 0xffff].bytes;
    bool enabled = (bool)((object[0x348] >> 2) & 1);
    component->unknown1c = (byte)function_1d5940(component, enabled, false, true);
    char index = component->unknown18;
    if (index >= 0 && index < component->rigid_bodies.size)
    {
        short const &converted_index = (short)index;
        if (converted_index != NONE)
        {
            byte *physics = definition + 0x2ac;
            bool skip = false;
            if (*(long *)(definition + 0x2f8))
            {
                byte *shape = *(byte **)(physics + 0x50);
                if (*(long *)(shape + 0x38) > 0)
                {
                    if (((c_1d56f0_shape *)shape)->slot10() <= 1)
                        shape += 0x60;
                    if (shape)
                    {
                        c_contact_query_transform_info info;
                        transform4x3f matrix;
                        function_1d4360(component, &matrix);
                        info.shape = shape;
                        info.filter = ((component->object_index + 1) << 16) | 0xc;
                        function_181f80((s_havok_transform *)&info.transform, &matrix);
                        component->rigid_body = (hkRigidBody *)::new (contact_query_allocate(sizeof(c_contact_query_transform_volume))) c_contact_query_transform_volume(&info);
                    }
                }
                if (*(long *)(physics + 0x4c))
                {
                    skip = (bool)((*(dword *)(*(byte **)(physics + 0x50) + 0x3c) >> 1) & 1);
                }
            }
            if (skip || !function_f1320(component->object_index))
            {
                real bounds[6];
                if (function_205510(component, physics, bounds))
                {
                    c_contact_query_bounds_info info;
                    info.lower.set(bounds[0], bounds[2], bounds[4]);
                    info.upper.set(bounds[1], bounds[3], bounds[5]);
                    info.filter = ((component->object_index + 1) << 16) | 0xc;
                    component->unknown9c = (long)new c_contact_query_bounds_volume(&info);
                }
            }
        }
    }
}

/* A member of the component: with `this` the rigid-body lookup reads retail's `[edx + eax + 0x40]` (as the matched
   member 0x1d08e0). The tail sets the body's frame through the transform setter's inline member spelling (see
   0x1d0cf0 in src/unknown_1cec30.cpp). */
// @retail 0x1d5120
bool s_havok_component::function_1d5120(long arg_1, long arg_2, long arg_3,
 real arg_4, long *arg_5, long arg_6)
{
 bool local_0 = false;
 _control87(0x9001f, 0x8001f);
 _mm_setcsr(_mm_getcsr() | 0x1f80);
 hkTransform local_1;
 c_contact_query_transform_volume *local_2;
 {
  c_contact_query_transform_info local_3;
  local_1.set(havok_component_rigid_body_get(arg_1, this)->m_motion->m_transform);
  local_3.transform.set(local_1);
  local_3.shape = (void *)arg_3;
  ((real *)&local_1.m_translation.m_quad)[2] += (real)arg_6 * arg_4;
  local_3.filter = ((object_index + 1) << 16) | arg_2;
  local_2 = new c_contact_query_transform_volume(&local_3);
 }
 ((c_contact_query_world *)g_51e9a4)->add(local_2);
 c_contact_presence local_6;
 bool local_4 = arg_6 > 0;
 long local_5 = arg_6 - (local_4 ? 1 : -1);
 while (local_4 ? local_5 >= 0 : local_5 <= 0)
 {
  local_6.found = false;
  ((hkRigidBody *)local_2)->pose_write(local_1);
  ((c_contact_query_dispatch *)local_2)->query(&local_6);
  if (!local_6.found)
  {
   *arg_5 = arg_6 - local_5;
   hkTransform local_7;
   local_7.m_rotation.m_col0.set(local_1.m_rotation.m_col0(0), local_1.m_rotation.m_col0(1), local_1.m_rotation.m_col0(2));
   local_7.m_rotation.m_col1.set(local_1.m_rotation.m_col1(0), local_1.m_rotation.m_col1(1), local_1.m_rotation.m_col1(2));
   local_7.m_rotation.m_col2.set(local_1.m_rotation.m_col2(0), local_1.m_rotation.m_col2(1), local_1.m_rotation.m_col2(2));
   hkVector4 local_8;
   local_8.set(local_1.m_translation(0), local_1.m_translation(1), local_1.m_translation(2));
   local_7.m_translation = local_8;
   rigid_body_transform_set(arg_1, &local_7);
   local_0 = true;
   break;
  }
  ((real *)&local_1.m_translation.m_quad)[2] += (local_4 ? -1.0f : 1.0f) * arg_4;
  if (local_4)
   local_5--;
  else
   local_5++;
 }
 ((c_contact_query_world *)g_51e9a4)->remove(local_2);
 havok_reference_remove((c_havok_reference_counted *)local_2);
 _mm_setcsr(_mm_getcsr() & 0xffffffc0);
 _clearfp();
 _control87(0x9001f, 0xfffff);
 return local_0;
}
