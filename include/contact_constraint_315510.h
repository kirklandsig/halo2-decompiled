#ifndef CONTACT_CONSTRAINT_315510_H
#define CONTACT_CONSTRAINT_315510_H
#include "lane_b_constraint_leaves.h"

// Owned contact arrays use allocator tag 0x12; the capacity's sign bit means
// that storage belongs to someone else and must not be released here.
// Concrete array types keep their destructors inline with this compiler.
#define CONTACT_ARRAY_315510(name, type) \
struct name { \
	type *data; \
	long size, capacity_and_flags; \
	__forceinline ~name() { \
		if (!(capacity_and_flags & 0x80000000)) \
		{ \
			type *block = data; \
			g_memory_2d88a0->release_block(block, \
				(capacity_and_flags & 0x7fffffff) * sizeof(type), 0x12); \
		} \
	} \
}
CONTACT_ARRAY_315510(contact_ids_315510, unsigned char);
CONTACT_ARRAY_315510(contact_points_315510, lane_b_contact_point);
CONTACT_ARRAY_315510(contact_properties_315510, lane_b_contact_property);
#undef CONTACT_ARRAY_315510

class __declspec(novtable) contact_constraint_315510 : public lane_a_constraint
{
public:
	unsigned long unknown1c;
	contact_ids_315510 ids;
	unsigned char unknown2c[8];
	contact_points_315510 points;
	union { float scale_a; void const *velocity; } modifier;
	float scale_b;
	unsigned char unknown48[8];
	__declspec(align(16)) lane_b_contact_solver_data solver_data;
	contact_properties_315510 properties;
	lane_b_contact_property inline_properties[4];
	__forceinline contact_constraint_315510() {}
	virtual ~contact_constraint_315510();
	virtual void *v3() { return 0; }
	virtual lane_a_solver_result *runtime() { return 0; }
	virtual long result_count() const { return 0; }
	virtual bool_result_2da7f0 valid() const { return true; }
	virtual long kind() const { return 11; }
	virtual void build(lane_a_solver_input const *, lane_a_solver_output *) = 0;
	virtual void info(lane_a_constraint_info *) const = 0;
};

// Concrete contact storage shares the existing out-of-line array teardown.
class contact_default_31c000 : public contact_constraint_315510
{
public:
	contact_default_31c000();
	virtual void build(lane_a_solver_input const *input, lane_a_solver_output *output)
	{ ((lane_b_contact_constraint_prefix *)this)->build(input, output); }
	virtual void info(lane_a_constraint_info *output) const
	{ ((lane_b_contact_constraint_prefix *)this)->get_sizes((lane_b_constraint_sizes *)output); }
};

// Both concrete modifiers share this out-of-line teardown wrapper.
class __declspec(novtable) contact_runtime_3156c0 : public contact_default_31c000
{
public:
	virtual ~contact_runtime_3156c0();
	virtual void build(lane_a_solver_input const *, lane_a_solver_output *) = 0;
};
class contact_sizes_3155f0 : public contact_runtime_3156c0
{
public:
	contact_sizes_3155f0();
	virtual void build(lane_a_solver_input const *, lane_a_solver_output *);
	virtual void info(lane_a_constraint_info *) const;
};
class contact_sizes_315720 : public contact_runtime_3156c0
{
public:
	contact_sizes_315720();
	virtual void build(lane_a_solver_input const *, lane_a_solver_output *);
	virtual void info(lane_a_constraint_info *) const;
};
#endif
