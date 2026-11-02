#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_1cec30.h"
#include "unknown_03d380.h"
#include <new>
#include <string.h>
// @flags /O2 /Gr

class c_interface_277890
{
public:
 c_interface_277890();
 virtual ~c_interface_277890();
 byte field_4[0x28 - 4];
};
struct s_278371;
class c_278370
{
public:
 virtual void function_278370(s_278371 *arg_0);
 virtual void function_1c2691(void *arg_0) {}
 virtual void function_1c2692(void *arg_0) {}
 virtual void function_a9ef0(void *arg_0);
};
void __cdecl function_2d8780();
int __cdecl function_2d7f00();
void (__cdecl *g_461df0)() = function_2d8780;
int (__cdecl *g_461df4)() = function_2d7f00;
byte g_510c58;
extern long g_47f04c;
extern long g_47f050;
extern bool g_47f05b;
extern long g_479890;
extern s_record_pool *g_51ebfc;
extern s_record_pool *g_51ec00;
extern long g_502138;
struct s_impact_component_iterator
{
 s_havok_component *component;
 s_record_pool_iterator iterator;
};
extern s_impact_component_iterator g_5021bc;
extern byte *g_51eca8;
extern void *g_51ecac;
extern s_47f048_object *g_47f048;
extern hkWorld *g_51e9a4;
extern bool g_55e4fc;
void function_146a20();
void function_146ac0();
void function_146b80();
void function_1c2d40();
void function_1c2b10();
void function_1c34a0();
void function_278f00();
void havok_components_dispose();

// @retail 0x1c2690
void function_1c2690()
{
 function_146a20();
 g_461df0();
 g_47f04c = 0;
 g_51eca8 = (byte *)new c_interface_277890;
 g_51ecac = new c_278370;
 g_51ebfc->valid = true;
 record_pool_release_all(g_51ebfc);
 g_51ec00->valid = true;
 record_pool_release_all(g_51ec00);
 g_502138 = NONE;
 g_51e9b8->valid = true;
 record_pool_release_all(g_51e9b8);
 g_47f058 = true;
 function_1c2910();
 function_1c2d40();
 if (!g_510c58)
  g_479890 = 2;
 if (!g_47989c)
  function_146b80();
 function_1c39c0(g_47f048, 0);
 g_47f048->function_30be40((long)g_51ecac);
 function_1c2a10();
 g_5021bc.iterator.data = g_51e9b8;
 g_5021bc.iterator.index = NONE;
 g_5021bc.iterator.datum_index = NONE;
}

// @retail 0x1c27a0
void function_1c27a0()
{
 bool local_0 = g_47f05b;
 if (!local_0)
  g_47f05b = true;
 function_1c2b10();
 if (!g_510c58)
  g_479890 = 1;
 ((c_physics_body_island_view *)g_47f048)->entity_observer_detach((c_havok_entity_observer *)g_51ecac);
 --g_47f050;
 g_51e9a4->entity_world_remove((hkEntity *)g_47f048);
 function_278f00();
 function_1c34a0();
 function_1c2890();
 g_47f058 = false;
 havok_components_dispose();
 g_51ebfc->valid = false;
 g_51ec00->valid = false;
 memset(&g_47f04c, 0xff, sizeof(g_47f04c));
 delete (c_278370 *)g_51ecac;
 g_51ecac = NULL;
 if (g_51eca8)
  delete (c_interface_277890 *)g_51eca8;
 g_51eca8 = NULL;
 g_461df4();
 function_146ac0();
 g_55e4fc = false;
 if (!local_0)
  g_47f05b = false;
}
