// @flags /O2 /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "havok_reference.h"
#include "unknown_1cec30.h"
#include "unknown_1efac0.h"
#include "constraint_helpers_314090.h"

struct s_1d8dd0
{
	long field_0;
	word field_4;
};

// @retail 0x1d8dd0
void __cdecl function_1d8dd0(void *arg_0)
{
	g_480118->allocate((long)arg_0, ((s_1d8dd0 *)arg_0)->field_4, 0x26);
}

struct s_1d8df0
{
	byte field_0[0x54];
	c_havok_reference_counted *field_54;
	void function_1d8df0(c_havok_reference_counted *arg_0);
};

// @retail 0x1d8df0
void s_1d8df0::function_1d8df0(c_havok_reference_counted *arg_0)
{
	if (arg_0)
		++arg_0->reference_count;
	if (field_54)
		havok_reference_remove(field_54);
	field_54 = arg_0;
}

struct s_1d8e40 : hkRigidBody
{
	void function_1d8e40(hkVector4 const &arg_0);
	void function_1d8ec0(hkVector4 const &arg_0, hkVector4 const &arg_1);
};

// @retail 0x1d8e40
void s_1d8e40::function_1d8e40(hkVector4 const &arg_0)
{
	if (!body_is_simulating().m_bool && m_simulation_island)
		activate();
	m_motion->setLinearVelocity(arg_0);
}

// @retail 0x1d8ec0
void s_1d8e40::function_1d8ec0(hkVector4 const &arg_0, hkVector4 const &arg_1)
{
	if (!body_is_simulating().m_bool && m_simulation_island)
		activate();
	m_motion->applyPointImpulse(arg_0, arg_1);
}

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

// @retail 0x1d19d0 deleting c_1d19d0

// @retail 0x1d1820
c_1d19d0::c_1d19d0(hkEntity *arg_0, long arg_1)
	: c_314710(arg_0), field_60(arg_1)
{
	(void)&arg_1;
	field_64 = havok_entity_property_get(arg_0, 0x2002);
}

struct s_1d1910
{
 short field_0;
 byte field_2;
 byte field_3;
 long field_4;
 byte *field_8;
};

bool function_efb60(long arg_0);
bool function_e5240(long arg_0);

// @retail 0x1d1910
char function_1d1910(c_1d19d0 *arg_0)
{
 long local_0 = arg_0->field_60;
 s_1d1910 *local_1 = &((s_1d1910 *)g_4e0300->data)[local_0 & 0xffff];
 byte *local_2 = local_1->field_8;
 s_havok_component *local_3 = havok_component_get(*(long *)(local_2 + 0xb4));
 bool local_4 = *((byte *)local_3->rigid_bodies.data + arg_0->field_64 * 0x60 + 0x44) == 1;
 char result = false;
 switch (local_1->field_3)
 {
 case 0:
  if (local_2[0x34b] == 1) break;
  result = local_4 || function_e5240(local_0);
  break;
 case 1:
  result = local_4 || function_efb60(local_0);
  break;
 default:
  result = local_4;
  break;
 }
 return result;
}

// @retail 0x1d1870
s_1d1870 c_1d19d0::function_1d1870(void *arg_0)
{
 return function_1d1910(this) ? s_1d1870(false) : function_3144b0(arg_0);
}

// @retail 0x1d18c0
s_1d1870 c_1d19d0::function_1d18c0(void *arg_0)
{
 return function_1d1910(this) ? s_1d1870(false) : function_314580(arg_0);
}
