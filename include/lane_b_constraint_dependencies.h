#ifndef LANE_B_CONSTRAINT_DEPENDENCIES_H
#define LANE_B_CONSTRAINT_DEPENDENCIES_H
#include "lane_b_constraint_leaves.h"
// Contact properties and solver state are updated while building the rows.
void __cdecl lane_b_contact_build(const void *points, long count,
	void *properties, void *solver_data, const void *input, void *output);
#endif
