#ifndef CONTACT_MANAGER_318C10_H
#define CONTACT_MANAGER_318C10_H
#include "lane_a_contact_manager.h"
#include "lane_b_constraint_leaves.h"
#include "contact_constraint_315510.h"
#include "contact_storage_318f10.h"

class hkBool
{
public:
	hkBool() {}
	char m_bool;
};
class c_1d1261
{
public:
	char unknown0[0x12c];
	char field_12c;
	hkBool function_30ca30(void *entry);
};
class class_318e60 : public referenced_2d88a0
{
public:
	c_1d1261 *world;
	long unknownc;
	contact_default_31c000 constraint;
	REFERENCED_NEW_2D88A0(0x1d)
	class_318e60(c_1d1261 *, hkEntity *, hkEntity *);
	virtual ~class_318e60();
	__forceinline lane_b_contact_constraint_prefix &contacts()
	{ return *(lane_b_contact_constraint_prefix *)&constraint; }
	virtual unsigned short add(lane_a_contact_body const *, lane_a_contact_body const *,
		void const *, lane_b_contact_point const *);
	virtual void remove(unsigned short id);
	virtual void processed(lane_a_contact_body const *, lane_a_contact_body const *, void const *, void const *);
	virtual void cleanup() { delete this; }
	virtual s_contact_property_318f30 *property(unsigned short id)
	{ return ((s_contact_storage_318f10 *)this)->property(id); }
	virtual s_contact_point_318f10 *point(unsigned short id)
	{ return ((s_contact_storage_318f10 *)this)->point(id); }
	virtual void collect(array_2d91d0 *out) const
	{ ((s_contact_storage_318f10 const *)this)->collect(out); }
};
#endif
