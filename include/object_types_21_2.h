#pragma once

/* OBJECT_TYPES_21_2.H: the item, projectile, weapon and device object types
   (vtables 0x451f68, 0x4521b4, 0x4524d8 and 0x452848), whose retail tables
   also hold small event definition classes (event_definitions.h). The object
   types derive from the shared base in unknown_0a58d0.h.
   Slots this batch does not decompile are placeholders. */

#include "unknown_11c920.h"
#include "unknown_0a58d0.h"
#include "event_definitions.h"

/* the item type (vtable 0x451f68) */
class c_item_type : public c_object_type_definition
{
public:
	virtual const char *v1();
	virtual long v2();
	virtual long v3();
	virtual long v5();
	virtual void v9(long a, long b, long *size);
	virtual void v26(long index, long b, s_entity_state *state);
};

/* the projectile type (vtable part of 0x4521b4, slots 23..58) */
class c_projectile_type : public c_object_type_definition
{
public:
	virtual long v0();
	virtual const char *v1();
	virtual long v3();
	virtual void v9(long a, long b, long *size);
	virtual void v21(s_entity *entity);
	virtual void v26(long index, long b, s_entity_state *state);
	virtual bool v30(long index);
};

/* the weapon type (vtable part of 0x4524d8, slots 36..71) */
class c_weapon_type : public c_object_type_definition
{
public:
	virtual const char *v1();
	virtual long v2();
	virtual long v5();
	virtual void v26(long index, long b, s_entity_state *state);
};

/* the device type (vtable 0x452848) */
class c_device_type : public c_object_type_definition
{
public:
	virtual const char *v1();
	virtual long v2();
	virtual long v5();
	virtual bool v34(long a, s_entity_data *source, long *block);
};
