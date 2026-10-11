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
	void *result = frame->data + frame->size;

	frame->size += size;
	return result;
}

// @retail 0x209d50
long *__stdcall function_209d50(long thread_index, short parameter_count, short const *parameter_types, bool initialize)
{
	s_hs_macro_thread *thread;
	long *arguments = (long *)function_xc5914d(thread_index, parameter_count * sizeof(long));
	short &argument_index = *(short *)function_xc5914d(thread_index, sizeof(short));
	long &expression_index = *(long *)function_xc5914d(thread_index, sizeof(long));
	long *volatile result = arguments;

	if (initialize)
	{
		thread = hs_macro_thread_get(thread_index);
		argument_index = 0;
		expression_index = hs_macro_expression_get(hs_macro_expression_get(thread->frame->expression_index)->value)->next_index;
	}
	if (argument_index < parameter_count &&
		hs_macro_expression_get(expression_index)->type == parameter_types[argument_index])
	{
		function_2099f0(thread_index, &arguments[argument_index], expression_index);
		expression_index = hs_macro_expression_get(expression_index)->next_index;
		(argument_index)++;
		result = NULL;
	}
	return result;
}
