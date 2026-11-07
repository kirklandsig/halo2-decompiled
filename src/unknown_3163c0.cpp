// @flags /O2 /GL- /GX
// Havok (third-party physics library): searching for the time at which two
// moving bodies first come within a tolerance. Both bodies are moved to a
// common time and their contacts collected again; the time is then halved
// back and forth until the step falls below the world's smallest step. When
// no time leaves them clear, the pair is marked and its contacts are
// collected once more with a very low tolerance. (Not in
// src/unknown_3169c0.cpp beside it.) With it, the smallest distance among
// collected contacts, and the narrow phase that runs the search for each pair
// (src/unknown_317c00.cpp has the other).
#include <math.h>
#include "reference_counted_2d88a0.h"
#include "timer_mark_2ebc80.h"

// a body's motion: its time, and the slot that moves it to another time
class motion_316460
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void slot_4() {}
	virtual void slot_5() {}
	virtual void slot_6() {}
	virtual void slot_7() {}
	virtual void slot_8() {}
	virtual void slot_9() {}
	virtual void slot_10() {}
	virtual void slot_11() {}
	virtual void slot_12() {}
	virtual void slot_13() {}
	virtual void slot_14() {}
	virtual void slot_15() {}
	virtual void slot_16() {}
	virtual void slot_17() {}
	virtual void slot_18() {}
	virtual void slot_19() {}
	virtual void slot_20() {}
	virtual void slot_21() {}
	virtual void slot_22() {}
	virtual void slot_23() {}
	virtual void slot_24() {}
	virtual void slot_25() {}
	virtual void slot_26() {}
	virtual void slot_27() {}
	virtual void slot_28() {}
	virtual void move_to_time(float time, long context) {}

	char unknown04[0x78];
	float time;
};

struct body_316460
{
	char unknown00[0x3c];
	motion_316460 *motion;
};

// a pair's member: its body, 0x10 bytes into the object the collector takes
struct member_316460
{
	char unknown00[0x10];
	body_316460 *body;
};

struct input_316460
{
	char unknown00[0x10];
	long context;
};

struct contact_316460
{
	char unknown00[0xc];
	float distance;
	char unknown10[0x20];
};

struct contacts_316460
{
	contact_316460 *data;
	long size;
};

struct output_316460
{
	contacts_316460 *contacts;
};

// takes the contacts collected for a pair
class handler_317950
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void handle(void *first, void *second, input_316460 *input, output_316460 *output) {}
};

// collects a pair's contacts
class collector_316460
{
public:
	virtual void slot_0() {}
	virtual void slot_1() {}
	virtual void slot_2() {}
	virtual void slot_3() {}
	virtual void collect(void *first, void *second, input_316460 *input, output_316460 *output) {}

	long field_4;
	handler_317950 *handler;
};

struct pair_316460
{
	member_316460 *first;
	member_316460 *second;
	collector_316460 *collector;
	long result;
};

struct search_state_316460
{
	char unknown00[0xc];
	long mode;
	float tolerance;
};

struct world_316460
{
	char unknown00[0x244];
	// the time of a pair's first step, and the length of one step
	float first_step_time;
	float step_length;
	float smallest_step;
	char unknown250[4];
	search_state_316460 *state;
};

class time_search_316460
{
public:
	void function_316460(input_316460 *input, pair_316460 *pair, output_316460 *output, float tolerance);

	// 0: a contact is within the tolerance (or the search was stopped);
	// 1: all contacts are clear; 2: there are none
	__forceinline long classify(output_316460 const *output, float tolerance)
	{
		if (world->state->mode == 0)
		{
			return 0;
		}
		long result = 2;
		for (long index = 0; index < output->contacts->size; index++)
		{
			if (output->contacts->data[index].distance < tolerance)
			{
				return 0;
			}
			result = 1;
		}
		return result;
	}

	char unknown00[0x20];
	world_316460 *world;
};

// the smallest distance among the collected contacts (the largest float when
// there are none)
// @retail 0x3163c0
static float __cdecl function_3163c0(output_316460 const *output)
{
	float smallest = 3.40282347e+38f;
	for (long index = 0; index < output->contacts->size; index++)
	{
		if (output->contacts->data[index].distance < smallest)
		{
			smallest = output->contacts->data[index].distance;
		}
	}
	return smallest;
}

// @retail 0x316460
void time_search_316460::function_316460(input_316460 *input, pair_316460 *pair, output_316460 *output, float tolerance)
{
	motion_316460 *first = pair->first->body->motion;
	motion_316460 *second = pair->second->body->motion;
	float const reached_a = first->time;
	float const reached_b = second->time;
	float const earliest = reached_a < reached_b ? reached_a : reached_b;
	if (reached_a != reached_b)
	{
		motion_316460 *later = reached_a > reached_b ? first : second;
		later->move_to_time(earliest, input->context);
		output->contacts->size = 0;
		world->state->mode = 2;
		world->state->tolerance = tolerance;
		pair->collector->collect((char *)pair->first - 0x10, (char *)pair->second - 0x10, input, output);
		switch (classify(output, tolerance))
		{
		case 1:
			return;
		case 2:
			{
				float step = (float)fabs(reached_a - reached_b) * 0.5f;
				float time = earliest + step;
				while (step > world->smallest_step)
				{
					later->move_to_time(time, input->context);
					output->contacts->size = 0;
					world->state->mode = 2;
					world->state->tolerance = tolerance;
					pair->collector->collect((char *)pair->first - 0x10, (char *)pair->second - 0x10, input, output);
					switch (classify(output, tolerance))
					{
					case 0:
						step *= 0.5f;
						time -= step;
						break;
					case 1:
						return;
					case 2:
						step *= 0.5f;
						time += step;
						break;
					}
				}
				if (classify(output, tolerance) == 0)
				{
					later->move_to_time(earliest, input->context);
					pair->result = -3;
					output->contacts->size = 0;
					world->state->mode = 2;
					world->state->tolerance = -1000.0f;
					pair->collector->collect((char *)pair->first - 0x10, (char *)pair->second - 0x10, input, output);
					return;
				}
			}
			break;
		}
	}
	float time = (first->time < second->time ? first->time : second->time) * 0.5f;
	float step = time;
	while (step > world->smallest_step)
	{
		first->move_to_time(time, input->context);
		second->move_to_time(time, input->context);
		world->state->mode = 2;
		world->state->tolerance = tolerance;
		output->contacts->size = 0;
		pair->collector->collect((char *)pair->first - 0x10, (char *)pair->second - 0x10, input, output);
		switch (classify(output, tolerance))
		{
		case 0:
			step *= 0.5f;
			time -= step;
			break;
		case 1:
			return;
		case 2:
			step *= 0.5f;
			time = step + time;
			break;
		}
	}
	if (step <= world->smallest_step)
	{
		if (classify(output, tolerance) == 0)
		{
			pair->result = -3;
			first->move_to_time(reached_a, input->context);
			second->move_to_time(reached_b, input->context);
			world->state->tolerance = -1000.0f;
			output->contacts->size = 0;
			pair->collector->collect((char *)pair->first - 0x10, (char *)pair->second - 0x10, input, output);
		}
	}
}

struct element_317060
{
	char bytes[0x30];
};

// an array of 48-byte elements whose storage comes from the allocator's
// stack area (src/unknown_317060.cpp)
struct array_317060
{
	element_317060 *data;
	long size;
	unsigned long capacity_and_flags;

	array_317060() : data(0), size(0), capacity_and_flags(0x80000000) {}
	~array_317060()
	{
		if ((capacity_and_flags & 0x80000000) == 0)
		{
			g_memory_2d88a0->release_block(data, (capacity_and_flags & 0x7fffffff) * sizeof(element_317060), 0x12);
		}
	}
};

struct stack_array_317060 : array_317060
{
	element_317060 *stack_block;
	long stack_capacity;

	stack_array_317060(long capacity);
	~stack_array_317060();
};

// the narrow phase (src/unknown_317c00.cpp), seen here as the time search it
// also is: its pairs follow at +0x68
class narrow_phase_317c00 : public time_search_316460
{
public:
	char unknown24[0x68 - 0x24];
	pair_316460 *pairs;
	long pair_count;

	void function_317950(void const *input);
};

// the narrow phase that searches for each pair's time of contact: a pair's
// result counts the steps it has advanced (negative once it has given up);
// its contacts are collected at that step's time, and the search moves on a
// step while one is too close, or back a step when every contact is clear of
// the earlier step's time too; the contacts then go to the agent's handler
// Capture callback arguments in retail order, and snapshot the pair count only
// after the temporary array has been allocated. Allocation may invoke a callback.
// @retail 0x317950
void narrow_phase_317c00::function_317950(void const *input)
{
	timer_mark_2ebc80 *mark = (timer_mark_2ebc80 *)g_480154;
	bool const room = (char *)mark < g_480158;
	if (room)
	{
		mark->label = "TtNarrowPhase";
		unsigned long time;
		__asm
		{
			rdtsc
			mov time, eax
		}
		mark->time = time;
		g_480154 = (char *)(mark + 1);
	}
	{
		output_316460 output;
		stack_array_317060 results(0x40);
		int const count = pair_count;
		pair_316460 *pair = pairs;
		output.contacts = (contacts_316460 *)&results;
		for (int index = count - 1; index >= 0; index--, pair++)
		{
			if (pair->collector != 0)
			{
				output.contacts->size = 0;
				long steps = pair->result;
				if (steps >= 0)
				{
					float step_time = steps * world->step_length + world->first_step_time;
					world->state->mode = 2;
					world->state->tolerance = step_time;
					{
						void *body_b = (char *)pair->second - 0x10;
						void *body_a = (char *)pair->first - 0x10;
						input_316460 *query = (input_316460 *)input;
						output_316460 *out = &output;
						pair->collector->collect(body_a, body_b, query, out);
					}
					if (classify(&output, step_time) != 0)
					{
						if (pair->result > 0)
						{
							long earlier = pair->result - 1;
							float earlier_time = earlier * world->step_length + world->first_step_time;
							if (function_3163c0(&output) > earlier_time)
							{
								pair->result = earlier;
							}
						}
					}
					else
					{
						pair->result++;
						step_time += world->step_length;
						function_316460((input_316460 *)input, pair, &output, step_time);
					}
				}
				else
				{
					world->state->mode = 2;
					world->state->tolerance = -1000.0f;
					{
						void *body_b = (char *)pair->second - 0x10;
						void *body_a = (char *)pair->first - 0x10;
						input_316460 *query = (input_316460 *)input;
						output_316460 *out = &output;
						pair->collector->collect(body_a, body_b, query, out);
					}
					if (pair->result == -2 && classify(&output, 0.0f) != 0)
					{
						pair->result = 0;
					}
				}
				{
					handler_317950 *handler = pair->collector->handler;
					void *body_b = (char *)pair->second - 0x10;
					void *body_a = (char *)pair->first - 0x10;
					input_316460 *query = (input_316460 *)input;
					output_316460 *out = &output;
					handler->handle(body_a, body_b, query, out);
				}
			}
		}
	}
	timer_add_2ebc80("Et");
}
