// @flags /O2 /GL- /GX
// Havok (third-party physics library): the agent for a shape that wraps a
// child shape. Each query is passed on to the agent made for the wrapped
// child, with the wrapping side replaced by its child at the same key and
// placement; the kind used with the two bodies the other way round turns what
// it finds back.
// Construction resolves the child pair once; later queries reuse that agent.
#include "lane_b_wrapper_agents.h"

// Construction establishes the parent before copying its placement. Query
// forwarding below retains the shared body's placement-first construction.
struct wrapper_creation_body_2e63a0 : body_2ea940
{
	wrapper_creation_body_2e63a0(body_2ea940 const *p, void const *where, wrapper_2f9950 const *wrapper)
	{
		parent = p;
		placement = where;
		void const *inner = wrapper->child;
		key = parent->key;
		shape = inner;
	}
};

static __declspec(align(16)) const unsigned long g_4124a0[4] = {0x80000000, 0x80000000, 0x80000000, 0x80000000};

// @retail 0x2e6380 destructor lane_b_cd_point_collector

// @retail 0x2e6390 destructor lane_b_cd_body_pair_collector

// @retail 0x2e7a50 deleting lane_b_flip_body_pair_collector

// @retail 0x2e7a80 deleting lane_b_flip_point_collector

// @retail 0x2e63a0
lane_b_wrapper_agent::lane_b_wrapper_agent(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *manager) : lane_a_agent_base(manager)
{
	wrapper_creation_body_2e63a0 child(a, a->placement, (wrapper_2f9950 const *)a->shape);
	lane_a_dispatch *table = *input;
	long first = ((typed_2ed170 *)child.shape)->type();
	long second = ((typed_2ed170 *)b->shape)->type();
	lane_a_create_handler handler = table->create[first][second];
	agent = handler(&child, b, input, manager);
}

// @retail 0x2e6450
void lane_b_wrapper_agent::get_penetrations(body_2ea940 const *a, void const *b, void const *input, void *collector)
{
	body_2ea940 child(a, a->placement, (wrapper_2f9950 const *)a->shape);
	agent->slot_1(&child, b, input, collector);
}

// @retail 0x2e64a0
void __cdecl function_2e64a0(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *collector)
{
	body_2ea940 child;
	child.placement = a->placement;
	child.parent = a;
	void const *shape = ((wrapper_2f9950 const *)a->shape)->child;
	child.key = a->key;
	child.shape = shape;
	long second = ((typed_2ed170 *)b->shape)->type();
	long first = ((typed_2ed170 *)shape)->type();
	(*input)->penetration[first][second](&child, b, input, collector);
}

// @retail 0x2e6510
void lane_b_wrapper_agent::get_closest_points(body_2ea940 const *a, void const *b, void const *input, void *collector)
{
	body_2ea940 child(a, a->placement, (wrapper_2f9950 const *)a->shape);
	agent->slot_2(&child, b, input, collector);
}

// @retail 0x2e6560
void __cdecl function_2e6560(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *collector)
{
	body_2ea940 child;
	child.placement = a->placement;
	child.parent = a;
	void const *shape = ((wrapper_2f9950 const *)a->shape)->child;
	child.key = a->key;
	child.shape = shape;
	long second = ((typed_2ed170 *)b->shape)->type();
	long first = ((typed_2ed170 *)shape)->type();
	(*input)->closest[first][second](&child, b, input, collector);
}

// @retail 0x2e65d0
void lane_b_wrapper_agent::linear_cast(body_2ea940 const *a, void const *b, void const *input, void *collector, void *start_collector)
{
	body_2ea940 child(a, a->placement, (wrapper_2f9950 const *)a->shape);
	agent->slot_3(&child, b, input, collector, start_collector);
}

// @retail 0x2e6620
void __cdecl function_2e6620(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *collector, void *start_collector)
{
	body_2ea940 child;
	child.placement = a->placement;
	child.parent = a;
	void const *shape = ((wrapper_2f9950 const *)a->shape)->child;
	child.key = a->key;
	child.shape = shape;
	long second = ((typed_2ed170 *)b->shape)->type();
	long first = ((typed_2ed170 *)shape)->type();
	(*input)->linear_cast[first][second](&child, b, input, collector, start_collector);
}

// @retail 0x2e6690
void lane_b_wrapper_agent::process_collision(body_2ea940 const *a, void const *b, void const *input, void *collector)
{
	body_2ea940 child(a, a->placement, (wrapper_2f9950 const *)a->shape);
	agent->slot_4(&child, b, input, collector);
}

void lane_b_wrapper_agent::cleanup()
{
	agent->slot_5();
	delete this;
}

void lane_b_wrapper_agent::is_symmetric(bool *result)
{
	*result = false;
}

// @retail 0x2e66e0
lane_b_wrapper_agent *__cdecl function_2e66e0(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *manager)
{
	return new lane_b_wrapper_agent(a, b, input, manager);
}

// @retail 0x2e6740
void lane_b_wrapper_agent::update_filter(body_2ea940 const *a, void const *b, void const *input)
{
	body_2ea940 child(a, a->placement, (wrapper_2f9950 const *)a->shape);
	agent->slot_7(&child, b, input);
}

// @retail 0x2e6780
void __cdecl function_2e6780(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, lane_b_cd_body_pair_collector *collector)
{
	lane_b_flip_body_pair_collector flipped(collector);
	function_2e64a0(b, a, input, &flipped);
}

// @retail 0x2e67e0
void __cdecl function_2e67e0(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, lane_b_cd_point_collector *collector)
{
	lane_b_flip_point_collector flipped(collector);
	function_2e6560(b, a, input, &flipped);
}

// @retail 0x2e6840
void lane_b_symmetric_wrapper_agent::get_penetrations(body_2ea940 const *a, void const *b, void const *input, void *collector)
{
	lane_b_flip_body_pair_collector flipped((lane_b_cd_body_pair_collector *)collector);
	body_2ea940 const *side = (body_2ea940 const *)b;
	body_2ea940 child(side, side->placement, (wrapper_2f9950 const *)side->shape);
	agent->slot_1(&child, a, input, &flipped);
}

// @retail 0x2e68c0
void lane_b_symmetric_wrapper_agent::get_closest_points(body_2ea940 const *a, void const *b, void const *input, void *collector)
{
	lane_b_flip_point_collector flipped((lane_b_cd_point_collector *)collector);
	body_2ea940 const *side = (body_2ea940 const *)b;
	body_2ea940 child(side, side->placement, (wrapper_2f9950 const *)side->shape);
	agent->slot_2(&child, a, input, &flipped);
}

// @retail 0x2e6950
void lane_b_symmetric_wrapper_agent::process_collision(body_2ea940 const *a, void const *b, void const *input, void *collector)
{
	lane_b_collision_output *output = (lane_b_collision_output *)collector;
	long i = output->points->count;
	body_2ea940 const *side = (body_2ea940 const *)b;
	body_2ea940 child(side, side->placement, (wrapper_2f9950 const *)side->shape);
	agent->slot_4(&child, a, input, collector);
	for (; i < output->points->count; ++i)
	{
		output->points->data[i].normal = _mm_xor_ps(output->points->data[i].normal, *(const __m128 *)g_4124a0);
	}
}

// @retail 0x2e69e0
void lane_b_symmetric_wrapper_agent::update_filter(body_2ea940 const *a, void const *b, void const *input)
{
	body_2ea940 const *side = (body_2ea940 const *)b;
	body_2ea940 child(side, side->placement, (wrapper_2f9950 const *)side->shape);
	agent->slot_7(&child, a, input);
}

void lane_b_symmetric_wrapper_agent::is_symmetric(bool *result)
{
	*result = true;
}

// @retail 0x2e6a20
void __cdecl function_2e6a20(body_2ea940 const *a, body_2ea940 const *b, lane_b_linear_cast_input const *input, lane_b_cd_point_collector *collector, lane_b_cd_point_collector *start_collector)
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
		function_2e6620(b, a, &flipped_input.collision.dispatcher, &flipped, &flipped_start);
	}
	else
	{
		function_2e6620(b, a, &flipped_input.collision.dispatcher, &flipped, 0);
	}
}

// @retail 0x2e6b20
void lane_b_symmetric_wrapper_agent::linear_cast(body_2ea940 const *a, void const *b, void const *input, void *collector, void *start_collector)
{
	lane_b_linear_cast_input const *cast_input = (lane_b_linear_cast_input const *)input;
	lane_b_linear_cast_input flipped_input;
	flipped_input.collision = cast_input->collision;
	flipped_input.unknown20 = cast_input->unknown20;
	flipped_input.unknown24 = cast_input->unknown24;
	flipped_input.path = _mm_xor_ps(cast_input->path, *(const __m128 *)g_4124a0);
	lane_b_flip_cast_collector flipped(cast_input->path, (lane_b_cd_point_collector *)collector);
	body_2ea940 const *side = (body_2ea940 const *)b;
	if (start_collector)
	{
		lane_b_flip_cast_collector flipped_start(cast_input->path, (lane_b_cd_point_collector *)start_collector);
		body_2ea940 child(side, side->placement, (wrapper_2f9950 const *)side->shape);
		agent->slot_3(&child, a, &flipped_input, &flipped, &flipped_start);
	}
	else
	{
		body_2ea940 child(side, side->placement, (wrapper_2f9950 const *)side->shape);
		agent->slot_3(&child, a, &flipped_input, &flipped, 0);
	}
}

// @retail 0x2e6c60
lane_b_wrapper_agent *__cdecl function_2e6c60(body_2ea940 const *a, body_2ea940 const *b, lane_a_dispatch *const *input, void *manager)
{
	return new lane_b_symmetric_wrapper_agent(a, b, input, manager);
}

// @retail 0x2e6cd0
void __cdecl function_2e6cd0(s_1c3a40 *table)
{
	handlers_2e5d50 handlers;
	handlers.handler[0] = (handler_2e5d50)function_2e6c60;
	handlers.handler[1] = (handler_2e5d50)function_2e6780;
	handlers.handler[2] = (handler_2e5d50)function_2e67e0;
	handlers.handler[3] = (handler_2e5d50)function_2e6a20;
	table->function_2e5d50(&handlers, -1, 0x14);
	handlers.handler[0] = (handler_2e5d50)function_2e66e0;
	handlers.handler[1] = (handler_2e5d50)function_2e64a0;
	handlers.handler[2] = (handler_2e5d50)function_2e6560;
	handlers.handler[3] = (handler_2e5d50)function_2e6620;
	table->function_2e5d50(&handlers, 0x14, -1);
}
