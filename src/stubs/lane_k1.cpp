// stubs for key lane 1, Havok round 6: Havok functions not decompiled yet
// whose addresses matched code takes

#include "agent_table_2e5d50.h"
#include "tagged_objects_2e4ff0.h"
#include "sweep_batches_2e50d0.h"
#include "motion_description_3115a0.h"
#include "pair_agents_2eab70.h"
#include "derived_311920.h"
#include "stream_holder_2d8e70.h"
#include "convex_search_3048a0.h"
#include "point_triangle_308d90.h"
#include "string_helpers_2d8c90.h"
#include "placed_shape_312800.h"
#include "joined_groups_3720a0.h"
#include "slot_table_372a40.h"



// the three-axis sweep's pair report for many boxes at once (claimed, #638)
// @stub 0x2e2f70
void sweep_batches_2e50d0::function_2e2f70(endpoint_2e0f80 *endpoints, long count, pairs_2e1090 *pairs, pair_change_2e2f70 change)
{
}

// handlers of the pair table (0x2e5d50) not decompiled yet




struct walk_3028c0;
struct segment_3028c0;

class class_301a60
{
public:
	void function_301a60(walk_3028c0 *walk, char const *cells, segment_3028c0 const *segment);
};


// @stub 0x3048a0
int class_3048a0::function_3048a0(convex_2fefd0 const *first, convex_2fefd0 const *second, c_query_transform const *transform, pair_state_2fefd0 *state, closest_3048a0 *closest)
{
	return 0;
}












// Havok round 17: callees of matched functions not decompiled yet












struct s_surface_key_array;
struct c_query_transform;
struct s_query_bounds;

struct cell_box_300290;

class class_3008d0
{
public:
	void function_300290(cell_box_300290 const *box, unsigned char const *node);
};

struct s_244980_ray;
struct s_244980_output;
class c_244c10;

struct result_2df6e0
{
	result_2df6e0() {}

	bool hit;
};

struct pending_301800;
struct fixed_ray_301800;

struct class_2df670
{
	result_2df6e0 function_301720(void const *shape, void const *tree, void const *ray, void *output);
	void function_301800(void const *shape, void const *tree, s_244980_ray const *ray, s_244980_output *output, c_244c10 *callback);
	void function_3009e0(pending_301800 *pending, void const *root, fixed_ray_301800 *const ray);
};



// (class_2e1090 is declared in axis_sweep_2e1090.h, through sweep_batches_2e50d0.h)
// @stub 0x2e0020
void class_2e1090::function_2e0020(element_2e1090 const *element, bounds_2e0020 *bounds)
{
}




struct s_311100;

class c_311100
{
public:
	void function_30fcb0(s_311100 const *description);
	void function_30fef0(s_311100 const *description);
	void function_30e720();
};

class narrow_phase_317c00
{
public:
	void function_3175e0(void const *step, void const *input);
	void function_317950(void const *input);
};

// Havok round havok18: callees of the queries passed on with their sides changed











class class_308180;

struct contact_307f90;

// whether a contact was found
class answer_307f90
{
public:
	answer_307f90() {}

	char value;
};

class query_2ef260
{
public:
	void function_3080a0(class_308180 *manifold);
	void function_307f90(answer_307f90 *found, contact_307f90 *contact);
};

// whether two boxes overlap, returned through a hidden pointer
class answer_306580
{
public:
	answer_306580() {}

	char value;
};

class class_306580
{
public:
	answer_306580 function_306580();
};



struct pending_2ddbd0;

class class_2dce60
{
public:
	void function_2dce60(pending_2ddbd0 *pending, void const *root, __m128 const *offset);
};

class class_309b50
{
public:
	long function_309b50(long arg_0);
};

class entity_316000;

class class_30e140
{
public:
	void function_30e140(entity_316000 *entity);
};


class class_30f710
{
public:
	void function_30f710();
};


// motion objects 0x311940 builds
class class_31ad80 { public: class_31ad80(void const *position, void const *orientation); };


// @stub 0x31ad80
class_31ad80::class_31ad80(void const *position, void const *orientation)
{
}



// Havok round 23: callees of the functions this round writes

// the islands' step handed on (925 bytes, not decompiled)
// @stub 0x31c100
void __cdecl function_31c100(void *step_10, void *step_20, void *step_50, void *world_info,
	void *simulation, void *islands, long island_count, long field_58, long field_5c)
{
}

// frees a block too large for Havok's pool (library code, called through g_461ddc)
// @stub 0x2d6fb0
void __cdecl function_2d6fb0(void *address)
{
}

// the game's test for slowing a body on a contact (src/unknown_279410.cpp,
// disabled there; called from Havok code, so standard cdecl)
struct s_component_contact_pair;
class hkEntity;
// @stub 0x279410
bool __cdecl function_279410(s_component_contact_pair const *pair, hkEntity *moving,
	hkEntity *opposing, float limit, float reduction, float approach)
{
	return false;
}

// the walk that tests a tree's nodes against planes (3124 bytes, not decompiled)
struct pending_2ddbd0;
struct fixed_point_2dc220;
class class_2dc220
{
public:
	void function_2dc220(pending_2ddbd0 *pending, void const *root, fixed_point_2dc220 const *fixed_point);
};
