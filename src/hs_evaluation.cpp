// @flags /O2 /Gr
/* HS_EVALUATION.CPP: the script runtime's evaluation side (the /O2 /Gr
   half; hs_scheduler.cpp has the rest): formatting a value of each type as
   text, returning a value to the frame below, popping a frame, and waking a
   sleeping thread */

#include "unknown_11c920.h"
#include "globals.h"
#include "hs.h"
#include "hs_scheduler.h"
#include <string.h>
#include "unknown_0a58d0.h"

/* ---- value formatters: each writes a value of one type as text ---- */

/* the names of an enum type's values */
struct s_hs_enum_names
{
	const char **names;
	long count;
};

/* the enum types' name tables, indexed by script value type (retail's
   entries for _hs_type_game_difficulty onwards are its first used ones):
   retail's data, read as [8 * type + 0x4456fc], so the declaration names
   that base address */
s_hs_enum_names g_4456fc[1];

// @retail 0x208cc0
void __stdcall hs_boolean_format(long type, bool value, char *buffer, long maximum_count)
{
	if (value)
	{
		strncpy(buffer, "true", maximum_count);
	}
	else
	{
		strncpy(buffer, "false", maximum_count);
	}
	buffer[maximum_count - 1] = 0;
}

// @retail 0x208d10
void __stdcall hs_real_format(long type, real value, char *buffer, long maximum_count)
{
	function_11c9c0(buffer, maximum_count, "%f", (double)value);
}

// @retail 0x208d40
void __stdcall hs_short_integer_format(long type, short value, char *buffer, long maximum_count)
{
	function_11c9c0(buffer, maximum_count, "%d", (long)value);
}

// @retail 0x208d70
void __stdcall hs_long_integer_format(long type, long value, char *buffer, long maximum_count)
{
	function_11c9c0(buffer, maximum_count, "%ld", value);
}

// @retail 0x208da0
void __stdcall hs_string_format(long type, const char *value, char *buffer, long maximum_count)
{
	strncpy(buffer, value, maximum_count);
	buffer[maximum_count - 1] = 0;
}

/* writes the name of an enum type's value */
// @retail 0x208dd0
void __stdcall hs_enum_format(short type, short value, char *buffer, long maximum_count)
{
	strncpy(buffer, g_4456fc[type].names[value], maximum_count);
	buffer[maximum_count - 1] = 0;
}

typedef void (__stdcall *t_hs_value_formatter)(long, long, char *, long);

/* retail's value formatter table (indexed by type) at the entries of
   _hs_type_boolean to _hs_type_string, and of _hs_type_game_difficulty to
   _hs_type_network_event */
t_hs_value_formatter g_hs_primitive_value_formatters[5] =
{
	(t_hs_value_formatter)hs_boolean_format,
	(t_hs_value_formatter)hs_real_format,
	(t_hs_value_formatter)hs_short_integer_format,
	(t_hs_value_formatter)hs_long_integer_format,
	(t_hs_value_formatter)hs_string_format,
};

t_hs_value_formatter g_hs_enum_value_formatters[6] =
{
	(t_hs_value_formatter)hs_enum_format,
	(t_hs_value_formatter)hs_enum_format,
	(t_hs_value_formatter)hs_enum_format,
	(t_hs_value_formatter)hs_enum_format,
	(t_hs_value_formatter)hs_enum_format,
	(t_hs_value_formatter)hs_enum_format,
};

/* ---- returning values ---- */

/* converts a value of one type to another; indexed
   [wanted type * k_hs_type_count + returned type] */
typedef long (__stdcall *t_hs_type_cast_proc)(long value);

s_record_pool *g_4f9384;
s_type_f4462a *g_4744e0[1];
t_hs_type_cast_proc g_4f5770[k_hs_type_count * k_hs_type_count];

long function_bb760(short index);

/* stores a value, cast to the type the parent wants, in the frame below the
   thread's top frame and pops the top frame */
// @retail 0x209ae0
void hs_thread_return_value(long thread_index, long value)
{
	long absolute_index = thread_index & 0xffff;
	s_hs_thread *thread = &((s_hs_thread *)g_4f9384->data)[absolute_index];
	short returned_type;
	s_hs_expression_node *node = &((s_hs_expression_node *)g_4f9394->data)[thread->frame->expression_node_index & 0xffff];
	short wanted_type;

	if (!(node->flags & _hs_expression_node_flag_script_call))
		returned_type = g_4744e0[node->function_index]->return_type;
	else
		returned_type = ((s_hs_scenario *)g_4e0350)->scripts[node->script_index].return_type;
	wanted_type = node->type;

	if (returned_type != wanted_type && returned_type != _hs_type_passthrough)
	{
		if (wanted_type < _hs_type_object_name)
		{
			if (wanted_type >= _hs_type_object && wanted_type <= _hs_type_scenery)
			{
				if (returned_type >= _hs_type_object_name && returned_type <= _hs_type_scenery_name)
					value = function_bb760((short)value);
			}
			else
			{
				value = g_4f5770[wanted_type * k_hs_type_count + returned_type](value);
			}
		}
		else if (wanted_type > _hs_type_scenery_name)
		{
			if (wanted_type >= _hs_type_object && wanted_type <= _hs_type_scenery)
			{
				if (returned_type >= _hs_type_object_name && returned_type <= _hs_type_scenery_name)
					value = function_bb760((short)value);
			}
			else
			{
				value = g_4f5770[wanted_type * k_hs_type_count + returned_type](value);
			}
		}
	}

	*thread->frame->next->result = value;
	thread = &((s_hs_thread *)g_4f9384->data)[absolute_index];
	thread->frame = thread->frame->next;
}

/* ---- popping frames and waking threads ---- */

/* pops the thread's top frame */
// @retail 0x209c60
void hs_thread_frame_pop(long thread_index)
{
	s_hs_thread *thread = &((s_hs_thread *)g_4f9384->data)[thread_index & 0xffff];
	thread->frame = thread->frame->next;
}

/* wakes a thread that has not finished: restores the sleep another script
   overrode, or ends a sleep until a condition by popping its frames */
// @retail 0x209c80
void __stdcall hs_thread_wake(long thread_index)
{
	s_hs_thread *thread = &((s_hs_thread *)g_4f9384->data)[thread_index & 0xffff];
	if (thread->sleep_until != k_hs_thread_finished)
	{
		thread->sleep_until = 0;
		if (thread->flags & _hs_thread_flag_sleep_overridden)
		{
			thread->sleep_until = thread->saved_sleep_until;
			thread->flags &= ~_hs_thread_flag_sleep_overridden;
		}
		else
		{
			s_hs_frame *frame = thread->frame;
			if (frame->expression_node_index != NONE &&
				((s_hs_expression_node *)g_4f9394->data)[frame->expression_node_index & 0xffff].function_index == k_hs_function_sleep_until_condition)
			{
				hs_thread_frame_pop(thread_index);
			}
			else
			{
				frame = frame->next;
				if (frame && frame->expression_node_index != NONE &&
					((s_hs_expression_node *)g_4f9394->data)[frame->expression_node_index & 0xffff].function_index == k_hs_function_sleep_until_condition)
				{
					hs_thread_frame_pop(thread_index);
					hs_thread_frame_pop(thread_index);
					thread->flags &= ~_hs_thread_flag_initialize;
				}
			}
		}
	}
}
