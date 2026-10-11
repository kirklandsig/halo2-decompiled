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
	virtual long v29(long a, s_entity_info *info, long *flags, long size, long state);
	virtual void v31(long a, long b, long c, long d);
	virtual bool v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8);
	virtual bool v15(long a, long b, long c, long d, s_bitstream *stream);
	virtual long v27(long a, long b, long c, long d);
	virtual const char *v1();
	virtual long v2();
	virtual long v3();
	virtual long v5();
	virtual void v9(long a, long b, long *size);
	virtual void v26(long index, long b, s_entity_state *state);
	virtual void v10(s_creation_request *request, long parameter, long size, char *buffer);
	virtual void v11(long a, long b, long c);
};

/* the projectile type (vtable part of 0x4521b4, slots 23..58) */
class c_projectile_type : public c_object_type_definition
{
public:
	virtual long v29(long a, s_entity_info *info, long *flags, long size, long state);
	virtual void v31(long a, long b, long c, long d);
	virtual bool v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8);
	virtual bool v15(long a, long b, long c, long d, s_bitstream *stream);
	virtual bool v8(long a, long b);
	virtual long v27(long a, long b, long c, long d);
	virtual long v0();
	virtual const char *v1();
	virtual long v3();
	virtual void v9(long a, long b, long *size);
	virtual void v21(s_entity *entity);
	virtual void v26(long index, long b, s_entity_state *state);
	virtual bool v30(long index);
	virtual void v10(s_creation_request *request, long parameter, long size, char *buffer);
    virtual bool v13(long a, s_entity_info *info, s_bitstream *stream);
};

/* the weapon type (vtable part of 0x4524d8, slots 36..71) */
class c_weapon_type : public c_object_type_definition
{
public:
	virtual long v29(long a, s_entity_info *info, long *flags, long size, long state);
	virtual bool v13(long a, s_entity_info *info, s_bitstream *stream);
	virtual bool v16(long a, long b, long c);
	virtual void v31(long a, long b, long c, long d);
	virtual bool v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8);
	virtual bool v15(long a, long b, long c, long d, s_bitstream *stream);
	virtual long v27(long a, long b, long c, long d);
	virtual const char *v1();
	virtual long v2();
	virtual long v5();
	virtual void v26(long index, long b, s_entity_state *state);
	virtual void v10(s_creation_request *request, long parameter, long size, char *buffer);
	virtual void v11(long a, long b, long c);
};

/* the device type (vtable 0x452848) */
class c_device_type : public c_object_type_definition
{
public:
	virtual long v29(long a, s_entity_info *info, long *flags, long size, long state);
	virtual bool v13(long a, s_entity_info *info, s_bitstream *stream);
	virtual bool v16(long a, long b, long c);
	virtual void v31(long a, long b, long c, long d);
	virtual bool v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8);
	virtual bool v15(long a, long b, long c, long d, s_bitstream *stream);
	virtual long v27(long a, long b, long c, long d);
	virtual const char *v1();
	virtual long v2();
	virtual long v5();
	virtual bool v34(long a, s_entity_data *source, long *block);
	virtual void v10(s_creation_request *request, long parameter, long size, char *buffer);
	virtual void v11(long a, long b, long c);
};
