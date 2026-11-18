#include "unknown_11c920.h"
#include "unknown_1cec30.h"
// @flags /O2 /arch:SSE /Gr

extern long g_47f050;
extern hkWorld *g_51e9a4;
/* function_278f00 stays declared ahead of function_1c51c0: link-time code generation walks callees in this
   order, and 0x1c4040, 0x146bf0 and 0x1c51c0 match only with it (see 0x1c4040's comment) */
void function_278f00();
void function_1c51c0(long arg_0);

// @retail 0x1c39c0
void __stdcall function_1c39c0(void *arg_0, long arg_1)
{
 void *const *local_0 = &arg_0;
 if (g_47f050 >= 0x352)
  function_1c51c0(havok_entity_property_get((hkEntity *)*local_0, 0x2001));
 ++g_47f050;
 g_51e9a4->entity_world_insert((hkEntity *)*local_0);
 if ((byte)arg_1 && ((c_physics_body_island_view *)*local_0)->m_motion->m_type == 1)
  ((c_physics_body_island_view *)*local_0)->activate();
 function_278f00();
}
