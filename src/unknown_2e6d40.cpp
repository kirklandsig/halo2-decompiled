// @flags /O2 /GL- /GX
// Havok (third-party physics library): the agent for a shape inside a
// bounding volume. The bounding volume is queried first; only while it is
// found overlapping is the shape inside queried, its agent made when first
// needed and dropped when the volume stops overlapping. The kind used with
// the two bodies the other way round turns what it finds back.
// Child-agent lifetime follows the bounding query: create on entry, reuse
// while overlapping, and release when the bounding query stops reporting hits.
#include "lane_b_bv_agents.h"

// These query bodies establish the parent before taking its placement.
// Keep this construction local: other wrapped-body callers use the opposite order.
struct bounded_body_2e6d40 : body_2ea940
{
	bounded_body_2e6d40(body_2ea940 const *p, void const *where, wrapper_2f9950 const *wrapper)
	{
		parent = p;
		placement = where;
		void const *inner = wrapper->child;
		key = parent->key;
		shape = inner;
	}

	void set_shape(void const *inner)
	{
		key = parent->key;
		shape = inner;
	}
};

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

// @retail 0x2e6d40
void lane_b_bv_agent::cleanup()
{
	bounding_agent->slot_5();
	if (child_agent)
	{
		child_agent->slot_5();
		child_agent = 0;
	}
	delete this;
}

// @retail 0x2e6d70
lane_b_bv_agent::lane_b_bv_agent(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *manager) : lane_a_agent_base(manager)
{
	bounded_body_2e6d40 child(a, a->placement, (wrapper_2f9950 const *)a->shape);
	lane_a_dispatch *table = *input;
	long first = ((typed_2ed170 *)child.shape)->type();
	long second = ((typed_2ed170 *)b->shape)->type();
	lane_a_create_handler handler = table->create[first][second];
	bounding_agent = handler(&child, b, input, manager);
	child_agent = 0;
}

// @retail 0x2e6e30
lane_b_bv_agent *__cdecl function_2e6e30(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *manager)
{
	return new lane_b_bv_agent(a, b, input, manager);
}

// @retail 0x2e6e90
void lane_b_bv_agent::process_collision(body_2ea940 const *a, void const *b, void const *input, void *collector)
{
	timer_add_2ebc80("LthkBvAgent", g_480154 < g_480158);
	lane_b_bv_shape const *shape = (lane_b_bv_shape const *)a->shape;
	bounded_body_2e6d40 child(a, a->placement, (wrapper_2f9950 const *)shape);
	lane_b_flag_body_pair_collector overlap;
	bounding_agent->slot_1(&child, b, input, &overlap);
	if (overlap.early_out)
	{
		timer_add_2ebc80("Stchild");
		child.set_shape(shape->child);
		if (!child_agent)
		{
			void *manager = contact_manager;
			lane_a_dispatch *table = *(lane_a_dispatch *const *)input;
			long first = ((typed_2ed170 *)child.shape)->type();
			long second = ((typed_2ed170 *)((body_2ea940 const *)b)->shape)->type();
			lane_a_create_handler handler = table->create[first][second];
			child_agent = handler(&child, (body_2ea940 const *)b, (lane_a_dispatch *const *)input, manager);
		}
		child_agent->slot_4(&child, b, input, collector);
	}
	else if (child_agent)
	{
		child_agent->slot_5();
		child_agent = 0;
	}
	timer_add_2ebc80("lt");
}

// @retail 0x2e7030
void lane_b_bv_agent::linear_cast(body_2ea940 const *a, void const *b, void const *input, void *collector, void *start_collector)
{
	timer_add_2ebc80("LthkBvAgent", g_480154 < g_480158);
	lane_b_bv_shape const *shape = (lane_b_bv_shape const *)a->shape;
	bounded_body_2e6d40 child(a, a->placement, (wrapper_2f9950 const *)shape);
	lane_b_closest_point_collector closest;
	bounding_agent->slot_3(&child, b, input, &closest, &closest);
	if (closest.has_hit)
	{
		timer_add_2ebc80("Stchild");
		child.set_shape(shape->child);
		if (!child_agent)
		{
			void *manager = contact_manager;
			lane_a_dispatch *table = *(lane_a_dispatch *const *)input;
			long first = ((typed_2ed170 *)child.shape)->type();
			long second = ((typed_2ed170 *)((body_2ea940 const *)b)->shape)->type();
			lane_a_create_handler handler = table->create[first][second];
			child_agent = handler(&child, (body_2ea940 const *)b, (lane_a_dispatch *const *)input, manager);
		}
		child_agent->slot_3(&child, b, input, collector, start_collector);
	}
	else if (child_agent)
	{
		child_agent->slot_5();
		child_agent = 0;
	}
	timer_add_2ebc80("lt");
}

// @retail 0x2e71f0
void __cdecl function_2e71f0(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *collector, void *start_collector)
{
	timer_add_2ebc80("LthkBvAgent", g_480154 < g_480158);
	lane_b_bv_shape const *shape = (lane_b_bv_shape const *)a->shape;
	bounded_body_2e6d40 child(a, a->placement, (wrapper_2f9950 const *)shape);
	long first = ((typed_2ed170 *)child.shape)->type();
	long second = ((typed_2ed170 *)b->shape)->type();
	lane_a_cast_handler handler = (*input)->linear_cast[first][second];
	lane_b_closest_point_collector closest;
	handler(&child, b, input, &closest, &closest);
	if (closest.has_hit)
	{
		timer_add_2ebc80("Stchild");
		child.set_shape(shape->child);
		long inner = ((typed_2ed170 *)child.shape)->type();
		(*input)->linear_cast[inner][second](&child, b, input, collector, start_collector);
	}
	timer_add_2ebc80("lt");
}

// @retail 0x2e7370
void lane_b_bv_agent::get_closest_points(body_2ea940 const *a, void const *b, void const *input, void *collector)
{
	timer_add_2ebc80("LthkBvAgent", g_480154 < g_480158);
	{
		lane_b_bv_shape const *shape = (lane_b_bv_shape const *)a->shape;
		bounded_body_2e6d40 child(a, a->placement, (wrapper_2f9950 const *)shape);
		lane_b_flag_body_pair_collector overlap;
		bounding_agent->slot_1(&child, b, input, &overlap);
		if (overlap.early_out)
		{
			timer_add_2ebc80("Stchild");
			child.set_shape(shape->child);
			if (!child_agent)
			{
				void *manager = contact_manager;
				lane_a_dispatch *table = *(lane_a_dispatch *const *)input;
				long first = ((typed_2ed170 *)child.shape)->type();
				long second = ((typed_2ed170 *)((body_2ea940 const *)b)->shape)->type();
				lane_a_create_handler handler = table->create[first][second];
				child_agent = handler(&child, (body_2ea940 const *)b, (lane_a_dispatch *const *)input, manager);
			}
			child_agent->slot_2(&child, b, input, collector);
		}
		else if (child_agent)
		{
			child_agent->slot_5();
			child_agent = 0;
		}
		timer_add_2ebc80("lt");
	}
}

// @retail 0x2e7510
void __cdecl function_2e7510(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *collector)
{
	timer_add_2ebc80("LthkBvAgent", g_480154 < g_480158);
	{
		lane_b_bv_shape const *shape = (lane_b_bv_shape const *)a->shape;
		bounded_body_2e6d40 child(a, a->placement, (wrapper_2f9950 const *)shape);
		long first = ((typed_2ed170 *)child.shape)->type();
		long second = ((typed_2ed170 *)b->shape)->type();
		lane_a_query_handler handler = (*input)->penetration[first][second];
		lane_b_flag_body_pair_collector overlap;
		handler(&child, b, input, &overlap);
		if (overlap.early_out)
		{
			timer_add_2ebc80("Stchild");
			child.set_shape(shape->child);
			long inner = ((typed_2ed170 *)child.shape)->type();
			(*input)->closest[inner][second](&child, b, input, collector);
		}
		timer_add_2ebc80("lt");
	}
}

// @retail 0x2e7670
void lane_b_bv_agent::get_penetrations(body_2ea940 const *a, void const *b, void const *input, void *collector)
{
	timer_add_2ebc80("TthkBvAgent");
	body_2ea940 child(a, a->placement, (wrapper_2f9950 const *)a->shape);
	bounding_agent->slot_1(&child, b, input, collector);
	timer_add_2ebc80("Et");
}

// @retail 0x2e7710
void __cdecl function_2e7710(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *collector)
{
	timer_add_2ebc80("TthkBvAgent");
	bounded_body_2e6d40 child(a, a->placement, (wrapper_2f9950 const *)a->shape);
	long first = ((typed_2ed170 *)child.shape)->type();
	long second = ((typed_2ed170 *)b->shape)->type();
	(*input)->penetration[first][second](&child, b, input, collector);
	timer_add_2ebc80("Et");
}

// @retail 0x2e77d0
void lane_b_bv_agent::update_filter(body_2ea940 const *a, void const *b, void const *input)
{
	lane_b_bv_shape const *shape = (lane_b_bv_shape const *)a->shape;
	body_2ea940 child(a, a->placement, (wrapper_2f9950 const *)shape);
	bounding_agent->slot_7(&child, b, input);
	if (child_agent)
	{
		long key = child.parent->key;
		void const *inner = shape->child;
		child.key = key;
		child.shape = inner;
		child_agent->slot_7(&child, b, input);
	}
}

void lane_b_bv_agent::is_symmetric(bool *result)
{
	*result = false;
}

// @retail 0x2e7850
void __cdecl function_2e7850(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, lane_b_cd_body_pair_collector *collector)
{
	lane_b_flip_body_pair_collector flipped(collector);
	function_2e7710(b, a, input, &flipped);
}

// @retail 0x2e78b0
void __cdecl function_2e78b0(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, lane_b_cd_point_collector *collector)
{
	lane_b_flip_point_collector flipped(collector);
	function_2e7510(b, a, input, &flipped);
}

// @retail 0x2e7910
void lane_b_symmetric_bv_agent::get_penetrations(body_2ea940 const *a, void const *b, void const *input, void *collector)
{
	lane_b_flip_body_pair_collector flipped((lane_b_cd_body_pair_collector *)collector);
	lane_b_bv_agent::get_penetrations((body_2ea940 const *)b, a, input, &flipped);
}

// @retail 0x2e7970
void lane_b_symmetric_bv_agent::get_closest_points(body_2ea940 const *a, void const *b, void const *input, void *collector)
{
	lane_b_flip_point_collector flipped((lane_b_cd_point_collector *)collector);
	lane_b_bv_agent::get_closest_points((body_2ea940 const *)b, a, input, &flipped);
}

// @retail 0x2e79d0
void lane_b_symmetric_bv_agent::process_collision(body_2ea940 const *a, void const *b, void const *input, void *collector)
{
	lane_b_collision_output *output = (lane_b_collision_output *)collector;
	long i = output->points->count;
	lane_b_bv_agent::process_collision((body_2ea940 const *)b, a, input, collector);
	for (; i < output->points->count; ++i)
	{
		__m128 &normal = output->points->data[i].normal;
		normal = _mm_xor_ps(normal, *(const __m128 *)g_4124a0);
	}
}

// @retail 0x2e7a30
void lane_b_symmetric_bv_agent::update_filter(body_2ea940 const *a, void const *b, void const *input)
{
	lane_b_bv_agent::update_filter((body_2ea940 const *)b, a, input);
}

void lane_b_symmetric_bv_agent::is_symmetric(bool *result)
{
	*result = true;
}

// @retail 0x2e7ab0
void __cdecl function_2e7ab0(body_2ea940 const *a, body_2ea940 const *b, lane_b_linear_cast_input const *input, lane_b_cd_point_collector *collector, lane_b_cd_point_collector *start_collector)
{
	lane_b_linear_cast_input flipped_input;
	flipped_input.collision = input->collision;
	flipped_input.unknown20 = input->unknown20;
	flipped_input.unknown24 = input->unknown24;
	flipped_input.path = _mm_xor_ps(input->path, *(const __m128 *)g_4124a0);
	lane_b_flip_cast_collector flipped(input->path, collector);
	if (start_collector)
	{
		lane_b_flip_cast_collector flipped_start(input->path, start_collector);
		function_2e71f0(b, a, &flipped_input.collision.dispatcher, &flipped, &flipped_start);
	}
	else
	{
		function_2e71f0(b, a, &flipped_input.collision.dispatcher, &flipped, 0);
	}
}

// @retail 0x2e7bb0
void lane_b_symmetric_bv_agent::linear_cast(body_2ea940 const *a, void const *b, void const *input, void *collector, void *start_collector)
{
	lane_b_linear_cast_input const *cast_input = (lane_b_linear_cast_input const *)input;
	lane_b_linear_cast_input flipped_input;
	flipped_input.collision = cast_input->collision;
	flipped_input.unknown20 = cast_input->unknown20;
	flipped_input.unknown24 = cast_input->unknown24;
	flipped_input.path = _mm_xor_ps(cast_input->path, *(const __m128 *)g_4124a0);
	lane_b_flip_cast_collector flipped(cast_input->path, (lane_b_cd_point_collector *)collector);
	if (start_collector)
	{
		lane_b_flip_cast_collector flipped_start(cast_input->path, (lane_b_cd_point_collector *)start_collector);
		lane_b_bv_agent::linear_cast((body_2ea940 const *)b, a, &flipped_input, &flipped, &flipped_start);
	}
	else
	{
		lane_b_bv_agent::linear_cast((body_2ea940 const *)b, a, &flipped_input, &flipped, 0);
	}
}

// @retail 0x2e7ca0
lane_b_bv_agent *__cdecl function_2e7ca0(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *manager)
{
	return new lane_b_symmetric_bv_agent(a, b, input, manager);
}
