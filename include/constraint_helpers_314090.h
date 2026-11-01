#ifndef CONSTRAINT_HELPERS_314090_H
#define CONSTRAINT_HELPERS_314090_H
#include "lane_b_constraint_dependencies.h"

// Description layouts follow the retail constructors and import/export copies.
// The base occupies twelve bytes, followed by aligned vector fields.
struct c_component_joint_snapshot_3 : lane_b_rigid_constraint_description
{
	__m128 pivot_a, pivot_b;
	float length;
	c_component_joint_snapshot_3();
};
struct c_component_joint_snapshot_4 : lane_b_rigid_constraint_description
{
	__m128 pivot_a, pivot_b;
	c_component_joint_snapshot_4();
};
struct c_component_joint_snapshot_5 : lane_b_rigid_constraint_description
{
	__m128 pivot_a, pivot_b, axis_a, axis_b, perpendicular_b;
	__m128 frame_b[3];
	float minimum, maximum, friction;
	c_component_joint_snapshot_5();
};

// The constraints those descriptions build. Retail's vtables (ball and socket
// 0x41498c, spring 0x414b4c, prismatic 0x414ca4) give each one's result count
// and kind; the ball and socket's validity slot is the shared 0x31bc80, which
// always passes.
class lane_a_ball_socket : public lane_b_rigid_constraint
{
public:
	__m128 pivot_a, pivot_b;
	lane_a_solver_result results[3];
	virtual lane_a_solver_result *runtime();
	virtual void build(lane_a_solver_input const *input, lane_a_solver_output *output);
	virtual long result_count() const { return 3; }
	virtual bool_result_2da7f0 valid() const { return true; }
	virtual long kind() const { return 0; }
	void export_description(c_component_joint_snapshot_4 *description) const;
	void import_description(c_component_joint_snapshot_4 const *description);
	virtual void info(lane_a_constraint_info *output) const;
};
class lane_a_spring : public lane_b_rigid_constraint
{
public:
	__m128 pivot_a, pivot_b;
	float length;
	lane_a_solver_result results[1];
	virtual lane_a_solver_result *runtime();
	virtual void build(lane_a_solver_input const *input, lane_a_solver_output *output);
	virtual long result_count() const { return 1; }
	virtual bool_result_2da7f0 valid() const;
	virtual long kind() const { return 8; }
	void export_description(c_component_joint_snapshot_3 *description) const;
	void import_description(c_component_joint_snapshot_3 const *description);
	virtual void info(lane_a_constraint_info *output) const;
};
class lane_a_prismatic : public lane_b_rigid_constraint
{
public:
	__m128 pivot_a, pivot_b, axis_a, axis_b, perpendicular_b;
	__m128 frame_b[3];
	float minimum, maximum, friction;
	lane_a_solver_result results[7];
	virtual lane_a_solver_result *runtime();
	virtual void build(lane_a_solver_input const *input, lane_a_solver_output *output);
	virtual long result_count() const { return 7; }
	virtual bool_result_2da7f0 valid() const;
	virtual long kind() const { return 6; }
	void import_fields(c_component_joint_snapshot_5 const *description);
	void export_description(c_component_joint_snapshot_5 *description) const;
	void import_description(c_component_joint_snapshot_5 const *description);
	virtual void info(lane_a_constraint_info *output) const;
};

struct s_314320 : lane_b_constraint_aggregate
{
	long field_0;
	real field_4, field_8, field_c, field_10;
	s_314320();
};

struct s_1d1870
{
	char field_0;
	s_1d1870() {}
	s_1d1870(bool value) : field_0(value) {}
};
class hkEntity;
class shape_bounds_314650;
// Its vector fields make it 16-byte aligned, which is why the game's 0x1d1540
// allocates 0x70 bytes for the eight-byte subclass c_1d19d0.
class c_314710 : public c_a
{
public:
	float radius_squared;
	byte unknown0c[4];
	__m128 position_a, rotation_a, position_b, rotation_b;
	float distance_a, angle_a, distance_b, angle_b;
	c_314710(hkEntity *entity);
	s_1d1870 function_3144b0(void *body);
	s_1d1870 function_314580(void *body);
	float update_radius_squared(shape_bounds_314650 const *shape);
	void export_description(s_314320 *description) const;
	void import_description(s_314320 const *description);
};
struct contact_sizes_3155f0 : lane_b_contact_constraint_prefix
{
	void get_sizes(lane_b_constraint_sizes *sizes);
};
struct contact_sizes_315720 : lane_b_contact_constraint_prefix
{
	void get_sizes(lane_b_constraint_sizes *sizes);
};
#endif
