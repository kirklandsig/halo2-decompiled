#pragma once

/* UNKNOWN_0A58D0.H: the object type definition class hierarchy.

   Retail has one 36-slot vtable per object type (turret entity definition at
   0x452788, vehicle at 0x4520f0, and others at 0x452030 and 0x451f68 that are
   not decompiled). A slot that several vtables share is a method of the base
   class c_object_type_definition; a slot only one vtable holds is an
   override in that type's class. The methods are defined in
   unknown_09a9f0.cpp (turret and the shared methods it holds),
   unknown_09fe30.cpp (vehicle and the shared methods it holds) and
   unknown_0a45d0.cpp (slot 32), each exactly once. */

#include "unknown_11c920.h"
#include "bitstream.h"


/* the object, as seen by this code */
struct s_object_view
{
	long definition_index;
	byte unknown04[8];
	long next_sibling;
	long first_child;
	long field14;
	byte unknown18[2];
	short field1a;
	byte unknown1c[8];
	long field24;
	byte unknown28[0xaa - 0x28];
	signed char type;
	byte field_ab;
	byte unknownac[3];
	byte field_af;
	byte unknownb0[0xd4 - 0xb0];
	long field_d4;
	byte field_d8;
	byte unknownd9[0x10a - 0xd9];
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word flag3 : 1;
	word flag4 : 1;
	word flag5 : 1;
	word flag6 : 1;
	word flag7 : 1;
	word flag8 : 1;
	word flag9 : 1;
	word flag10 : 1;
	word flag11 : 1;
	word flag12 : 1;
	word flag13 : 1;
	word flag14 : 1;
	word flag15 : 1;
};

struct s_object_header
{
	byte unknown00[2];
	byte flags;
	byte unknown03[5];
	s_object_view *object;
};

/* the per-entity creation info: the definition index, and an identifier at
   +0x10 (10 bits of index and 4 bits of salt). The vehicle type keeps 0x20
   bytes of data there instead. */
struct s_entity_info
{
	long field0;
	long definition_index;
	union
	{
		long field8;
		byte byte8;
		byte unknown08[8];
	};
	union
	{
		long identifier;
		byte vehicle_data[0x20];
	};
};

/* the entity: field 8 is the object index once created */
struct s_entity
{
	long field0;
	byte unknown04[2];
	bool field_6;
	byte unknown07;
	long object_index;
	byte unknown0c[8];
	s_entity_info *info;
};

struct s_entity_state
{
	long field0;
	long field4;
	union
	{
		byte field8;
		long field8_long;
	};
	long fieldc;
	long field10;
};

struct s_entity_data
{
	byte unknown00[0x68];
	long block[10];
};

struct s_creation_request
{
	long entity_index;
	short definition_index;
	byte unknown06[2];
	long object_index;
};

class c_object_type_definition
{
public:
	virtual long v0() { return 0; }
	virtual const char *v1() { return 0; }
	virtual long v2();
	virtual long v3();
	virtual long v4();
	virtual long v5();
	virtual bool v6(s_entity *entity);
	virtual bool v7(s_entity *entity);
	virtual bool v8(long a, long b);
	virtual void v9(long a, long b, long *size);
	virtual void v10(s_creation_request *request, long parameter, long size, char *buffer) {}
	virtual void v11(long a, long b, long c);
	virtual void v12(long a, s_entity_info *info, long c, s_bitstream *stream);
	virtual bool v13(long a, s_entity_info *info, s_bitstream *stream);
	virtual bool v14(long a1, long a2, long a3, long a4, long a5, long a6, long a7, long a8);
	virtual bool v15(long a, long b, long c, long d, s_bitstream *stream);
	virtual void v16(long a, long b, long c);
	virtual bool v17(long a, long b, long c);
	virtual void v18(s_entity *entity, long b, s_entity_state *state);
	virtual bool v19(long a, long b, long c, s_entity_data *data);
	virtual bool v20(s_entity *entity, long *index, long c, long d);
	virtual void v21(s_entity *entity);
	virtual bool v22(s_entity *entity, long b, s_entity_info *info, long d, long e, long f);
	virtual bool v23(s_entity *entity, long b, long c, long d);
	virtual bool v24(s_entity *entity);
	virtual bool v25(s_entity *entity);
	virtual void v26(long index, long b, s_entity_state *state);
	virtual long v27(long a, long b, long c, long d);
	virtual bool v28(long index);
	virtual long v29(long a, s_entity_info *info, long *c, long d, long e);
	virtual bool v30(long index);
	virtual void v31(long a, long b, long c, long d);
	virtual bool v32(long a);
	virtual bool v33();
	virtual bool v34(long a, s_entity_data *source, long *block);
	virtual void v35(long a, s_entity_data *data, long *block);
};

/* the turret simulation entity definition (vtable 0x452788) */
class c_turret_entity_definition : public c_object_type_definition
{
public:
	virtual long v0();
	virtual const char *v1();
	virtual void v9(long a, long b, long *size);
	virtual void v10(s_creation_request *request, long parameter, long size, char *buffer);
	virtual void v12(long a, s_entity_info *info, long c, s_bitstream *stream);
	virtual bool v13(long a, s_entity_info *info, s_bitstream *stream);
	virtual void v21(s_entity *entity);
	virtual void v26(long index, long b, s_entity_state *state);
	virtual long v29(long a, s_entity_info *info, long *c, long d, long e);
	virtual bool v30(long index);
};

/* the vehicle object type (vtable 0x4520f0) */
class c_vehicle_type : public c_object_type_definition
{
public:
	virtual long v0();
	virtual const char *v1();
	virtual void v10(s_creation_request *request, long parameter, long size, char *buffer);
};

/* the library routines the entity code calls; none are decompiled yet */
bool function_a5bd0(long a);
bool function_a6d50(long a, long b, s_bitstream *stream);
/* 0xa6900: the base of the update sizes of the object types, from their
   update flags; the definition asking is passed but unused */
class c_object_type_definition;
struct s_flags_a6900
{
	dword flags;
	void function_a6900(long *result_pointer, c_object_type_definition const *definition) const;
};
long function_a5930(long a);
long function_a5e70(long a, long b, long c);
long function_a58d0(long a);
void function_a6430(long a, long b, long c);
struct s_relevance_observers;
/* 0xaa4d0 (src/unknown_0aa4d0.cpp): how relevant the entities are to the
   observers, from their distance and whether an observer faces them */
real function_aa4d0(long count, long const *entity_indices, real maximum_distance,
	s_relevance_observers const *observers, bool *exact);
char *function_11c9c0(char *buffer, long size, const char *format, ...);
void function_a6660(s_entity_info *info);
void function_b5650(long identifier, s_bitstream *stream);
bool function_a6810(s_bitstream *stream);
bool function_a69a0(long a, long b, long c, long d, long e, bool f, long g);
void function_a7180(long a, long b);
void __stdcall function_b8540(long a);
void function_a5d90(void *data, s_entity_info *info, long *c, long e);
long function_a73b0(s_entity_info *info);
long function_b7b40(void *creation);
void function_b9b90(long object_index, bool disable);
