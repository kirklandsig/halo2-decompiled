// @flags /O2 /Ob1 /GL-
#include "lane_b_constraint_dependencies.h"
#include "contact_constraint_315510.h"

// @retail 0x31bdf0
void contact_default_31c000::build(lane_a_solver_input const *input, lane_a_solver_output *output)
{
	lane_b_contact_build(points.data, points.size, properties.data, &solver_data, input, output);
}

