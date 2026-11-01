#ifndef LANE_A_CONSTRAINTS_H
#define LANE_A_CONSTRAINTS_H
#include "unknown_1efac0.h"
#include "reference_counted_2d88a0.h"
#include <xmmintrin.h>
#include "matrix_compare_2da7f0.h"
struct lane_a_solver_input;
struct lane_a_solver_output;

// Modelled prefixes only: retail names and vector layouts are established by
// the serialization tables at 414538/414728/414890 and their copy routines.
struct lane_a_constraint_description
{
	lane_a_constraint_description();
	void *body_a;
	void *body_b;
	long priority;
};
// The constraint-specific description base adds no storage, but its retail
// constructor remains out of line between the concrete and common defaults.
struct lane_b_rigid_constraint_description : lane_a_constraint_description
{
	lane_b_rigid_constraint_description();
};
struct lane_a_constraint_info
{
	long maximum;
	long op_bytes;
	long result_bytes;
	long result_count;
};
struct lane_a_solver_result
{
	float impulse;
	float internal;
	lane_a_solver_result() : impulse(0.0f), internal(0.0f) {}
};
// The secondary listener interface occupies +8. In the primary table,
// build() is at +0x24 and info() follows it at +0x28.
class hkEntity;
struct lane_a_constraint_interface
{
	virtual ~lane_a_constraint_interface() {}
	virtual void event_1(hkEntity *);
	virtual void event_2(hkEntity *);
	virtual void event_3(hkEntity *);
};
class lane_a_constraint : public c_a, public lane_a_constraint_interface
{
public:
	void *body_a;
	void *body_b;
	void *owner;
	long priority;
	static void *operator new(unsigned int size)
	{
		lane_a_constraint *object = (lane_a_constraint *)g_memory_2d88a0->allocate_block(size, 0x27);
		object->flags = (unsigned short)size;
		return object;
	}
	static void __cdecl operator delete(void *object)
	{
		g_memory_2d88a0->release_block(object,
			((lane_a_constraint *)object)->flags, 0x27);
	}
	void export_base(lane_a_constraint_description *description) const;
	void import_base(lane_a_constraint_description const *description);
	lane_a_constraint(lane_a_constraint_description const *description);
	virtual ~lane_a_constraint();
	virtual void *v3();
	virtual lane_a_solver_result *runtime() = 0;
	virtual long result_count() const = 0;
	virtual bool_result_2da7f0 valid() const = 0;
	virtual long kind() const = 0;
	virtual void event_2(hkEntity *entity);
	virtual void build(lane_a_solver_input const *input, lane_a_solver_output *output) = 0;
	virtual void info(lane_a_constraint_info *output) const = 0;
	void set_entity_a(hkEntity *entity);
	void set_entity_b(hkEntity *entity);
};
// The empty runtime layer has its own out-of-line destructor. Constructor
// unwind records target that wrapper, which tail-calls lane_a_constraint.
class lane_b_rigid_constraint : public lane_a_constraint
{
public:
	lane_b_rigid_constraint(lane_a_constraint_description const *description)
		: lane_a_constraint(description) {}
};
struct lane_a_ragdoll_description : lane_b_rigid_constraint_description
{
	__m128 pivot_a, pivot_b, twist_a, plane_a, twist_b, plane_b;
	float cone_limit, plane_low, plane_high, twist_low, twist_high, friction;
};
struct lane_a_limited_hinge_description : lane_b_rigid_constraint_description
{
	__m128 pivot_a, pivot_b, axle_a, axle_b, perp_a, perp_b, reference_b;
	float angle_min, angle_max, friction;
};
struct lane_a_hinge_description : lane_b_rigid_constraint_description
{
	__m128 pivot_a, pivot_b, axle_b, axle_a, perpendicular_a;
};
class lane_a_ragdoll : public lane_b_rigid_constraint
{
public:
	lane_a_ragdoll(lane_a_ragdoll_description const *description);
	__m128 pivot_a, twist_a, plane_a, pivot_b, twist_b, plane_b;
	float cone_limit, plane_low, plane_high, twist_low, twist_high, friction;
	lane_a_solver_result results[9];
	virtual void *v3();
	virtual void build(lane_a_solver_input const *input, lane_a_solver_output *output);
	void export_description(lane_a_ragdoll_description *description) const;
	void set_strength(float strength);
	void import_fields(lane_a_ragdoll_description const *description);
	void build_swing_limits(__m128 const &twist, __m128 const &cone, __m128 const &plane,
		lane_a_solver_input const *input, lane_a_solver_output *output) const;
	void import_description(lane_a_ragdoll_description const *description);
	virtual lane_a_solver_result *runtime();
	virtual long result_count() const { return 9; }
	virtual bool_result_2da7f0 valid() const;
	virtual long kind() const { return 7; }
	virtual void info(lane_a_constraint_info *output) const;
};
class lane_a_limited_hinge : public lane_b_rigid_constraint
{
public:
	lane_a_limited_hinge(lane_a_limited_hinge_description const *description);
	__m128 pivot_a, axle_a, perp_a, perp_b, pivot_b, axle_b, reference_b;
	lane_a_solver_result results[7];
	float angle_min, angle_max, friction, previous_angle;
	virtual void *v3();
	virtual void build(lane_a_solver_input const *input, lane_a_solver_output *output);
	void export_description(lane_a_limited_hinge_description *description) const;
	virtual lane_a_solver_result *runtime();
	virtual long result_count() const { return 7; }
	virtual bool_result_2da7f0 valid() const;
	virtual long kind() const { return 2; }
	void import_fields(lane_a_limited_hinge_description const *description);
	void import_description(lane_a_limited_hinge_description const *description);
	virtual void info(lane_a_constraint_info *output) const;
};
class lane_a_hinge : public lane_b_rigid_constraint
{
public:
	lane_a_hinge(lane_a_hinge_description const *description);
	__m128 pivot_a, axle_a, perpendicular_a, pivot_b, axle_b;
	lane_a_solver_result results[5];
	virtual void *v3();
	virtual void build(lane_a_solver_input const *input, lane_a_solver_output *output);
	void export_description(lane_a_hinge_description *description) const;
	void import_fields(lane_a_hinge_description const *description);
	virtual lane_a_solver_result *runtime();
	virtual long result_count() const { return 5; }
	virtual bool_result_2da7f0 valid() const;
	virtual long kind() const { return 1; }
	void import_description(lane_a_hinge_description const *description);
	virtual void info(lane_a_constraint_info *output) const;
};
#endif
