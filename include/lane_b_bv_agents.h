#ifndef LANE_B_BV_AGENTS_H
#define LANE_B_BV_AGENTS_H
// Havok (third-party physics library): the agent for a shape with a bounding
// volume (shape type 0x11). It keeps an agent for the bounding volume and,
// while that volume overlaps the other side, one for the shape inside it.
#include "lane_b_wrapper_agents.h"
#include "timer_mark_2ebc80.h"

// a shape inside a bounding volume: the volume at +0xc, the shape at +0x10
struct lane_b_bv_shape
{
	char prefix[0xc];
	void const *bounding_volume;
	void const *child;
};

// records only that some pair was found (retail table 0x4129cc)
class lane_b_flag_body_pair_collector : public lane_b_cd_body_pair_collector
{
public:
	virtual void add(body_2ea940 const *a, body_2ea940 const *b)
	{
		early_out = true;
	}
};

// the position (w = distance) and normal of a point
struct lane_b_point_contact
{
	__m128 position;
	__m128 normal;
};

// keeps the closest point found (retail table 0x4129d4)
class lane_b_closest_point_collector : public lane_b_cd_point_collector
{
public:
	lane_b_closest_point_collector() : has_hit(false)
	{
		hit.position.m128_f32[3] = FLT_MAX;
	}
	virtual void add(lane_b_cd_point const *point)
	{
		if (!has_hit || point->position.m128_f32[3] < hit.position.m128_f32[3])
		{
			has_hit = true;
			hit.position = point->position;
			hit.normal = point->normal;
			early_out = point->position.m128_f32[3];
		}
	}

	bool has_hit;
	lane_b_point_contact hit;
};

// retail table 0x4129dc
class lane_b_bv_agent : public lane_a_agent_base
{
public:
	lane_b_bv_agent(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *manager);
	virtual void get_penetrations(body_2ea940 const *a, void const *b, void const *input, void *collector);
	virtual void get_closest_points(body_2ea940 const *a, void const *b, void const *input, void *collector);
	virtual void linear_cast(body_2ea940 const *a, void const *b, void const *input, void *collector, void *start_collector);
	virtual void process_collision(body_2ea940 const *a, void const *b, void const *input, void *collector);
	// tells both agents, then destroys itself (retail 0x2e6d40)
	virtual void cleanup();
	// retail slot 0x2fe820
	virtual void is_symmetric(bool *result);
	virtual void update_filter(body_2ea940 const *a, void const *b, void const *input);

	lane_a_child_agent *bounding_agent;
	lane_a_child_agent *child_agent;
};

// the same with the bounded shape as the second body (retail table 0x412a30)
class lane_b_symmetric_bv_agent : public lane_b_bv_agent
{
public:
	lane_b_symmetric_bv_agent(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *manager)
		: lane_b_bv_agent(b, a, input, manager)
	{
	}
	virtual void get_penetrations(body_2ea940 const *a, void const *b, void const *input, void *collector);
	virtual void get_closest_points(body_2ea940 const *a, void const *b, void const *input, void *collector);
	virtual void linear_cast(body_2ea940 const *a, void const *b, void const *input, void *collector, void *start_collector);
	virtual void process_collision(body_2ea940 const *a, void const *b, void const *input, void *collector);
	// retail slot 0x31bc80
	virtual void is_symmetric(bool *result);
	virtual void update_filter(body_2ea940 const *a, void const *b, void const *input);
};

lane_b_bv_agent *__cdecl function_2e6e30(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *manager);
void __cdecl function_2e71f0(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *collector, void *start_collector);
void __cdecl function_2e7510(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *collector);
void __cdecl function_2e7710(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *collector);
void __cdecl function_2e7850(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, lane_b_cd_body_pair_collector *collector);
void __cdecl function_2e78b0(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, lane_b_cd_point_collector *collector);
void __cdecl function_2e7ab0(body_2ea940 const *a, body_2ea940 const *b, lane_b_linear_cast_input const *input, lane_b_cd_point_collector *collector, lane_b_cd_point_collector *start_collector);
lane_b_bv_agent *__cdecl function_2e7ca0(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *manager);
#endif
