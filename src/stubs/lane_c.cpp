// stubs for lane C (0x1c0000..0x1cffff): callees outside the region that are
// not decompiled yet, and the library (Havok) functions the region calls
#include "unknown_11c920.h"
#include "unknown_1cec30.h"
#include "constraint_helpers_314090.h"
#include "joint_descriptions_312920.h"


class c_1d1261
{
public:
 hkBool function_30ca30(void *arg_0);
 void function_30bd10(void *arg_0);
};

class c_2df9d0
{
public:
 virtual hkBool function_2df9d0(void const *arg_0, void const *arg_1);
};

class c_2df8f0
{
public:
 virtual hkBool function_2df8f0(void const *arg_0, void const *arg_1);
};

class c_2dfae0
{
public:
 virtual hkBool function_2dfae0(void const *arg_0, void const *arg_1);
};

class c_2dfbf0;

struct s_3111d0
{
 s_3111d0();
};

struct s_30cb70
{
 byte field_0;
};
class c_interface_278b40;
class c_3101c0
{
public:
 c_3101c0(s_3111d0 const &arg_0, long arg_1);
 void function_30c1a0(byte *arg_0);
 void function_30cb70(s_30cb70 arg_0);
 void function_30c500();
 void function_310900(c_2dfbf0 *arg_0, s_30cb70 arg_1);
 void function_30cab0(c_interface_278b40 *arg_0);
};



struct s_311100;
class c_311100
{
public:
 void function_311100(s_311100 const *arg_0);
};

// @stub 0x2d8780
void __cdecl function_2d8780() {}

class c_278370
{
public:
 virtual void function_a9ef0(void *arg_0);
};
void c_278370::function_a9ef0(void *arg_0) {}



class c_library_30c470
{
public:
	void detach(void *callback);
};



class c_278b60_filter;
struct s_278b60_dispatch;
struct s_278b60_filter;
class c_library_30c190
{
public:
	c_278b60_filter *filter();
	byte unknown00[0xcc];
	s_278b60_dispatch *dispatch;
	s_278b60_filter *filter_object;
};

#include "slot_owner.h"
#include "unknown_1c62f0.h"
#include "unknown_0259a0.h"
#include "unknown_1765e0.h"




/* slot handler callbacks of the region not decompiled yet (their handler
   structs hold their addresses) */

/* game functions outside the region called by the ai lifecycle callbacks */





/* the animation graph lookups (0x1d9000..0x1de000) */


/* the physics callees of the havok components */
struct s_havok_component;


/* callees of the slot handler callbacks (unknown_0259a0.h) */






/* callees of the physics code (unknown_1c25a0.cpp, unknown_1cec30.cpp) */









/* in the region, not decompiled yet */







/* the rigid body accessors of the havok components (unknown_1cec30.cpp) */




/* callees of ai.cpp's 0x1caa40 */



struct real_quaternion_transform;










class c_component_rotation
{
public:
	hkVector4 value;
	void set(hkRotation const &rotation);
};



#include "pair_filter_class.h"


#include <xmmintrin.h>
class c_world_callback_registration
{
public:
    void remove(void *callback);
};


struct c_contact_query_bounds_info { c_contact_query_bounds_info(); };
struct c_contact_query_transform_info { c_contact_query_transform_info(); };
class c_contact_query_bounds_volume
{
public:
    c_contact_query_bounds_volume(c_contact_query_bounds_info const *info);
};
#include "contact_volume_30bb50.h"
class c_contact_query_world
{
public:
    void *add(void *volume);
    void remove(void *volume);
};


class c_havok_abstract_allocator;
struct s_fixed_memory_statistics_16;
struct s_physics_pool_statistics;




struct s_physics_mass_array;
struct s_physics_mass;

class c_havok_reference_counted;


struct s_2dc030
{
    byte field_0[8];
    long field_8;
    real field_c;
    real field_10;
    s_2dc030();
};

class c_2dbfc0
{
public:
    byte field_0[4];
    word field_4;
    byte field_6[0x3a];
    c_2dbfc0(long arg_0, void *arg_1, real arg_2);
    void function_2dbcf0(s_2dc030 const *arg_0);
};

// @stub 0x314710
c_314710::c_314710(hkEntity *arg_0) {}

struct c_shape_library_base_a
{
 c_shape_library_base_a(c_havok_reference_counted *arg_0, long arg_1);
};

#include "agent_table_2e5d50.h"

// @stub 0x2e5e70
void s_1c3a40::function_2e5e70(long arg_0, long arg_1) {}










struct s_311340 { s_311340(); };
class c_311ba0 { public: c_311ba0(const s_311340 *arg_0); };

struct s_2da8c0
{
 void function_2da8c0(hkRotation const *arg_0, hkRotation const *arg_1);
};



struct s_1d1540 { void function_30be00(void *arg_0); };
struct s_1d1541 { void *function_30d2b0(void *arg_0); };
/* Unwritten constructors for the three remaining joint kinds. */
class c_component_joint_3 { public: c_component_joint_3(c_component_joint_snapshot_3 const *info); };
class c_component_joint_4 { public: c_component_joint_4(c_component_joint_snapshot_4 const *info); };
class c_component_joint_5 { public: c_component_joint_5(c_component_joint_snapshot_5 const *info); };

// @stub 0x314b00
c_component_joint_3::c_component_joint_3(c_component_joint_snapshot_3 const *info) {}

// @stub 0x314270
c_component_joint_4::c_component_joint_4(c_component_joint_snapshot_4 const *info) {}

// @stub 0x314ef0
c_component_joint_5::c_component_joint_5(c_component_joint_snapshot_5 const *info) {}
