// @flags /O2 /Gr
/* UNKNOWN_209D50.CPP: evaluates the arguments of a script function, one per
   call, into the thread's current frame (script_thread_runner; the front end of lane
   A's script functions) */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "hs.h"

/* a frame of a thread's stack (unknown_209520.cpp): its data follows the
   size at +0xe */
struct s_hs_macro_frame
{
	s_hs_macro_frame *next;
	long expression_index;
	long *result;
	short size;
	byte data[2];
};

struct s_hs_macro_thread
{
	byte unknown00[0x10];
	s_hs_macro_frame *frame;
	byte unknown14[0x418 - 0x14];
};

/* an expression node (g_4f9394, 20 bytes) */
struct s_hs_macro_expression
{
	short salt;
	short value_type;
	short type;
	byte flags;
	byte unknown07;
	long next_index;
	byte unknown0c[4];
	long value;
};

extern s_record_pool *g_4f9384;
extern s_record_pool *g_4f9394;

void function_2099f0(long thread_index, long *result, long expression_index); /* unknown_209520.cpp */

static inline s_hs_macro_thread *hs_macro_thread_get(long thread_index)
{
	return (s_hs_macro_thread *)(g_4f9384->data + (thread_index & 0xffff) * sizeof(s_hs_macro_thread));
}

static inline s_hs_macro_expression *hs_macro_expression_get(long expression_index)
{
	return (s_hs_macro_expression *)(g_4f9394->data + (expression_index & 0xffff) * sizeof(s_hs_macro_expression));
}

/* room in the thread's current frame */
static inline void *function_xc5914d(long thread_index, short size)
{
	s_hs_macro_frame *frame = hs_macro_thread_get(thread_index)->frame;
	short *frame_size = &frame->size;
	void *result = frame->data + *frame_size;

	*frame_size += size;
	return result;
}

// score 12 (was 12): retail copies the argument pointer into EAX and returns it in a
// register, ours reloads the result from the stack. Tried: no volatile 95 (one register for
// both, no spill); result NULL first and set on the else path 90; result declared first
// and assigned after the allocation 95; the slot address passed straight to the call 21;
// the result cleared before the call 31. Retail stores the pointer to its slot and copies
// the live register, as for an address-taken local.
// Round 11 (lane 18), all plain checks: the result taken from the allocation and the volatile pointer
// copied from it 71 (277 bytes; the pointer is computed straight into EAX, retail computes it in EBX
// and copies it); no volatile with the result cleared first in the success block 83 (277 bytes: no
// spill and the expression pool is no longer held in EBX); no volatile with the allocation helper
// reading the frame size without its size pointer 95 (266 bytes); no volatile, the result copied after
// all three allocations and cleared before the call 83; no volatile and no frame-reference local 78
// (278 bytes: the spill goes to the index pointer instead); no volatile with the argument index read
// into a local for the test and the element 95 (266 bytes); no volatile with the result kept as void *
// 95 (266 bytes).
// @retail 0x209d50
long *__stdcall function_209d50(long thread_index, short parameter_count, short const *parameter_types, bool initialize)
{
	s_hs_macro_frame **frame = &hs_macro_thread_get(thread_index)->frame;
	long *volatile arguments = (long *)function_xc5914d(thread_index, parameter_count * sizeof(long));
	long *result = arguments;
	short &argument_index = *(short *)function_xc5914d(thread_index, sizeof(short));
	long &expression_index = *(long *)function_xc5914d(thread_index, sizeof(long));

	if (initialize)
	{
		argument_index = 0;
		expression_index = hs_macro_expression_get(hs_macro_expression_get((*frame)->expression_index)->value)->next_index;
	}
	if (argument_index < parameter_count &&
		hs_macro_expression_get(expression_index)->type == parameter_types[argument_index])
	{
		long *argument = &arguments[argument_index];
		function_2099f0(thread_index, argument, expression_index);
		expression_index = hs_macro_expression_get(expression_index)->next_index;
		(argument_index)++;
		result = NULL;
	}
	return result;
}
