// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_209520.CPP: the script runtime's threads (script_thread_runner): starting a
   script's thread, evaluating an expression node into a frame, and reading
   and casting global values (outside functions the command scripts of lane I
   call) */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"
#include "hs.h"
#include <stddef.h>
#include <string.h>

/* a frame of a thread's stack: the expression it evaluates and where its
   value goes; the frame's own data follows it */
struct s_hs_frame
{
	s_hs_frame *next;
	long expression_index;
	long *result;
	short size;
	byte unknown0e[2];
};

/* a script thread (g_4f9384, 0x418 bytes) */
struct s_hs_thread
{
	short salt;
	byte type;
	byte flags;
	long script_index;
	long sleep_until;
	byte unknown0c[4];
	s_hs_frame *frame;
	long result;
	s_hs_frame stack;
	byte unknown28[0x418 - 0x28];
};

/* an expression node (g_4f9394, 20 bytes) */
struct s_hs_expression
{
	short salt;
	short value_type;
	short type;
	byte flags;
	byte unknown07[0x10 - 0x7];
	long value;
};

/* a global's runtime value (g_4f9380, 8 bytes) */
struct s_hs_global_value
{
	byte unknown0[4];
	script_value value;
};

/* an external global's definition (g_473468 points at one per global) */
struct s_hs_external_global
{
	short type;
	byte unknown2[2];
	void *address;
};

/* the scenario's scripts and globals (0x28 bytes each) */
struct s_hs_script
{
	byte unknown00[0x20];
	short type;
	short return_type;
	long root_expression_index;
};

struct s_hs_global
{
	byte unknown00[0x20];
	short type;
	byte unknown22[0x28 - 0x22];
};

struct s_hs_scenario_view
{
	byte unknown000[0x1bc];
	s_hs_script *scripts;
	byte unknown1c0[4];
	s_hs_global *globals;
};

typedef long (__stdcall *t_hs_cast_proc)(long value);

extern s_record_pool *g_4f9384;
extern s_record_pool *g_4f9394;
extern t_hs_cast_proc g_4f5770[0x3e * 0x3e];
extern s_record_pool *g_4f9380;
s_hs_external_global *g_473468[1];
char const *g_470010 = "";

long function_bb760(short index);

long function_2097c0(long script_index, byte type);
void function_2099f0(long thread_index, long *result, long expression_index);
long function_209bc0(short global_index);
long function_20a290(long thread_index, short value_type, short type, long value);
void function_20a2e0(short global_index);

inline s_hs_thread *hs_thread_get(long thread_index)
{
	return (s_hs_thread *)(g_4f9384->data + (thread_index & 0xffff) * sizeof(s_hs_thread));
}

inline s_hs_expression *hs_expression_get(long expression_index)
{
	return (s_hs_expression *)(g_4f9394->data + (expression_index & 0xffff) * sizeof(s_hs_expression));
}

inline s_hs_script *hs_script_get(long script_index)
{
	return &((s_hs_scenario_view *)g_4e0350)->scripts[script_index];
}

long hs_thread_for_script(short script_index);
void __stdcall hs_thread_wake(long thread_index);
void hs_thread_return_value(long thread_index, long value);

// @retail 0x208c40
void __stdcall function_208c40(short function_index, long thread_index, bool initialize)
{
	s_hs_expression *expression = hs_expression_get(hs_thread_get(thread_index)->frame->expression_index);
	s_hs_expression *name = hs_expression_get(expression->value);
	s_hs_expression *argument = hs_expression_get(*(long *)((byte *)name + 8));
	long target = hs_thread_for_script((short)argument->value);
	if (target != NONE)
		hs_thread_wake(target);
	hs_thread_return_value(thread_index, 0);
}

s_type_f4462a const g_44b1f8 = { _hs_type_void, 2, function_208c40, "<script name>", 0, { 0 } };

// @retail 0x208750
void __stdcall function_208750(short function_index, long thread_index, bool initialize)
{
	s_hs_expression *expression = hs_expression_get(hs_thread_get(thread_index)->frame->expression_index);
	s_hs_expression *name = hs_expression_get(expression->value);
	long argument_index = *(long *)((byte *)name + 8);
	long target = thread_index;
	if (argument_index != NONE)
	{
		long script_index;
		function_2099f0(thread_index, &script_index, argument_index);
		target = hs_thread_for_script((short)script_index);
	}
	if (target != NONE)
		hs_thread_get(target)->sleep_until = -2;
	hs_thread_return_value(thread_index, 0);
}

s_type_f4462a const g_44b1d8 = { _hs_type_void, 2, function_208750, "[<script>]", 0, { 0 } };

long *__stdcall function_209d50(long thread_index, short parameter_count, short const *parameter_types, bool initialize);

extern short const g_445710[62] =
{
	0, 0, 0, 0, 0, 1, 4, 2, 4, 4, 2, 4, 4, 2, 2, 2,
	2, 2, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 2, 4,
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2,
	2, 2, 4, 4, 4, 4, 4, 4, 2, 2, 2, 2, 2, 2
};

// @retail 0x208570
void __stdcall function_208570(short function_index, long thread_index, bool initialize)
{
	s_hs_expression *expression = hs_expression_get(hs_thread_get(thread_index)->frame->expression_index);
	s_hs_expression *name = hs_expression_get(expression->value);
	s_hs_expression *argument = hs_expression_get(*(long *)((byte *)name + 8));
	short type = argument->type;
	short types[2];
	types[1] = type;
	types[0] = type;
	long *values = function_209d50(thread_index, 2, types, initialize);
	if (values)
	{
		script_value result;
		bool comparison;
		switch (type)
		{
		case _hs_type_real:
		{
			real left = (real)*(real *)&values[0];
			real right = (real)*(real *)&values[1];
			switch (function_index)
			{
			case 15: comparison = left > right; break;
			case 16: comparison = left < right; break;
			case 17: comparison = left >= right; break;
			case 18: comparison = left <= right; break;
			default: comparison = false; break;
			}
			break;
		}
		case _hs_type_long_integer:
		{
			real left = (real)*(long *)&values[0];
			real right = (real)*(long *)&values[1];
			switch (function_index)
			{
			case 15: comparison = left > right; break;
			case 16: comparison = left < right; break;
			case 17: comparison = left >= right; break;
			case 18: comparison = left <= right; break;
			default: comparison = false; break;
			}
			break;
		}
		default:
		{
			real left = (real)*(short *)&values[0];
			real right = (real)*(short *)&values[1];
			switch (function_index)
			{
			case 15: comparison = left > right; break;
			case 16: comparison = left < right; break;
			case 17: comparison = left >= right; break;
			case 18: comparison = left <= right; break;
			default: comparison = false; break;
			}
			break;
		}
		}
		result.b = comparison;
		hs_thread_return_value(thread_index, result.d);
	}
}

s_type_f4462a const g_44b188 = { _hs_type_boolean, 2, function_208570, "<number> <number>", 0, { 0 } };
s_type_f4462a const g_44b198 = { _hs_type_boolean, 2, function_208570, "<number> <number>", 0, { 0 } };
s_type_f4462a const g_44b1a8 = { _hs_type_boolean, 2, function_208570, "<number> <number>", 0, { 0 } };
s_type_f4462a const g_44b1b8 = { _hs_type_boolean, 2, function_208570, "<number> <number>", 0, { 0 } };

// @retail 0x2084c0
void __stdcall function_2084c0(short function_index, long thread_index, bool initialize)
{
	script_value result;
	s_hs_expression *expression = hs_expression_get(hs_thread_get(thread_index)->frame->expression_index);
	s_hs_expression *name = hs_expression_get(expression->value);
	s_hs_expression *argument = hs_expression_get(*(long *)((byte *)name + 8));
	short types[2];
	types[0] = types[1] = argument->type;
	long *arguments = function_209d50(thread_index, 2, types, initialize);
	if (arguments)
	{
		bool equal = memcmp(arguments, arguments + 1, g_445710[types[0]]) == 0;
		if (function_index == 14)
			equal = !equal;
		result.b = equal;
		hs_thread_return_value(thread_index, result.d);
	}
}

s_type_f4462a const g_44b168 = { _hs_type_boolean, 2, function_2084c0, "<expression> <expression>", 0, { 0 } };
s_type_f4462a const g_44b178 = { _hs_type_boolean, 2, function_2084c0, "<expression> <expression>", 0, { 0 } };

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)

// @retail 0x209520
long function_209520(short script_index)
{
	s_hs_script *script = hs_script_get(script_index);
	long thread_index = NONE;

	if (script->type == 5)
	{
		thread_index = function_2097c0(script_index, 4);
		if (thread_index != NONE)
		{
			long *result = &hs_thread_get(thread_index)->result;
			_ReadWriteBarrier();
			function_2099f0(thread_index, result, script->root_expression_index);
		}
	}
	return thread_index;
}
#pragma function(_ReadWriteBarrier)

// @retail 0x2097c0
long function_2097c0(long script_index, byte type)
{
	long thread_index = record_pool_allocate(g_4f9384);

	if (thread_index != NONE)
	{
		s_hs_thread *thread = hs_thread_get(thread_index);

		thread->frame = &thread->stack;
		thread->frame->next = NULL;
		thread->frame->size = 0;
		thread->frame->expression_index = NONE;
		thread->type = type;
		thread->script_index = script_index;
		thread->flags = 0;
		if (script_index != NONE && hs_script_get(script_index)->type == 1)
		{
			thread->sleep_until = -2;
		}
		else
		{
			thread->sleep_until = 0;
		}
	}
	return thread_index;
}

static inline void hs_thread_stack_adjust(long thread_index)
{
	s_hs_thread *thread = hs_thread_get(thread_index);
	s_hs_frame *frame = thread->frame;
	s_hs_frame *pushed = (s_hs_frame *)((byte *)frame + sizeof(s_hs_frame) + frame->size);

	pushed->next = frame;
	thread->frame = pushed;
	pushed->size = 0;
}

// @retail 0x2099f0
void function_2099f0(long thread_index, long *result, long expression_index)
{
	s_hs_thread *thread = hs_thread_get(thread_index);
	s_hs_expression *expression = hs_expression_get(expression_index);
	__assume(result != NULL);

	if (hs_expression_get(expression_index)->flags & 1)
	{
		if (expression->flags & 4)
		{
			word global_index = (word)expression->value;
			short type;

			if (global_index & 0x8000)
			{
				type = g_473468[global_index & 0x7fff]->type;
			}
			else
			{
				type = ((s_hs_scenario_view *)g_4e0350)->globals[global_index & 0x7fff].type;
			}
			*result = function_20a290(thread_index, type, expression->type, function_209bc0(global_index));
		}
		else
		{
			*result = function_20a290(thread_index, expression->value_type, expression->type, expression->value);
		}
	}
	else
	{
		thread->frame->result = result;
		hs_thread_stack_adjust(thread_index);
		thread->flags |= 1;
		thread->frame->expression_index = expression_index;
	}
}

// @retail 0x20a290
long function_20a290(long thread_index, short value_type, short type, long value)
{
	if (value_type != type && value_type != _hs_type_passthrough)
	{
		if (type < _hs_type_object_name || type > _hs_type_scenery_name)
		{
			if (type >= _hs_type_object && type <= _hs_type_scenery)
			{
				if (value_type >= _hs_type_object_name && value_type <= _hs_type_scenery_name)
				{
					return function_bb760((short)value);
				}
			}
			else
			{
				return g_4f5770[type * 0x3e + value_type](value);
			}
		}
	}
	return value;
}

/* the thread being run (NONE between runs), and whether scripts run */
long g_4f938c = NONE;
bool g_4f9388;

void hs_thread_return_value(long thread_index, long value);

/* room for a value in the thread's current frame */
static inline long *hs_frame_allocate_value(long thread_index)
{
	s_hs_frame *frame = hs_thread_get(thread_index)->frame;
	long *value = (long *)((byte *)frame + offsetof(s_hs_frame, unknown0e) + frame->size);

	frame->size += sizeof(long);
	return value;
}

// @retail 0x207ef0
void __stdcall function_207ef0(short function_index, long thread_index, bool initialize)
{
	s_hs_frame *const *frame_reference = &hs_thread_get(thread_index)->frame;
	long *condition = hs_frame_allocate_value(thread_index);
	long *branch = hs_frame_allocate_value(thread_index);
	long *value = hs_frame_allocate_value(thread_index);
	if (initialize)
	{
		*condition = 0;
		*branch = NONE;
		s_hs_expression *expression = hs_expression_get((*frame_reference)->expression_index);
		s_hs_expression *name = hs_expression_get(expression->value);
		function_2099f0(thread_index, condition, *(long *)((byte *)name + 8));
	}
	else if (*branch == NONE)
	{
		if (*(bool *)condition)
		{
			s_hs_expression *expression = hs_expression_get((*frame_reference)->expression_index);
			s_hs_expression *name = hs_expression_get(expression->value);
			s_hs_expression *test = hs_expression_get(*(long *)((byte *)name + 8));
			*branch = *(long *)((byte *)test + 8);
		}
		else
		{
			s_hs_expression *expression = hs_expression_get((*frame_reference)->expression_index);
			s_hs_expression *name = hs_expression_get(expression->value);
			s_hs_expression *test = hs_expression_get(*(long *)((byte *)name + 8));
			s_hs_expression *then_expression = hs_expression_get(*(long *)((byte *)test + 8));
			*branch = *(long *)((byte *)then_expression + 8);
			if (*branch == NONE)
			{
				hs_thread_return_value(thread_index, 0);
				return;
			}
		}
		function_2099f0(thread_index, value, *branch);
	}
	else
		hs_thread_return_value(thread_index, *value);
}

s_type_f4462a const g_44b0b8 = { _hs_type_passthrough, 2, function_207ef0, "<boolean> <then> [<else>]", 0, { 0 } };

// @retail 0x208310
void __stdcall function_208310(short function_index, long thread_index, bool initialize)
{
	s_hs_frame *const *frame_reference = &hs_thread_get(thread_index)->frame;
	s_hs_frame *frame = *frame_reference;
	short *count = (short *)((byte *)frame + offsetof(s_hs_frame, unknown0e) + frame->size);
	frame->size += sizeof(short);
	long *next = hs_frame_allocate_value(thread_index);
	real *result;
	real *value = (real *)hs_frame_allocate_value(thread_index);
	result = (real *)hs_frame_allocate_value(thread_index);
	if (initialize)
	{
		*count = 0;
		s_hs_expression *expression = hs_expression_get((*frame_reference)->expression_index);
		s_hs_expression *name = hs_expression_get(expression->value);
		*next = *(long *)((byte *)name + 8);
	}
	else
	{
		real operand = *value;
		if (*count == 0)
			*result = operand;
		else switch (function_index)
		{
		case 7: *result += operand; break;
		case 8: *result -= operand; break;
		case 9: *result *= operand; break;
		case 10: *result /= operand; break;
		case 11: *result = *result > operand ? operand : *result; break;
		case 12: *result = *result > operand ? *result : operand; break;
		}
		(*count)++;
	}
	if (*next != NONE)
	{
		function_2099f0(thread_index, (long *)value, *next);
		*next = *(long *)((byte *)hs_expression_get(*next) + 8);
	}
	else
	{
		script_value returned;
		returned.r = *result;
		hs_thread_return_value(thread_index, returned.d);
	}
}

s_type_f4462a const g_44b108 = { _hs_type_real, 2, function_208310, "<number(s)>", 0, { 0 } };
s_type_f4462a const g_44b118 = { _hs_type_real, 2, function_208310, "<number> <number>", 0, { 0 } };
s_type_f4462a const g_44b128 = { _hs_type_real, 2, function_208310, "<number(s)>", 0, { 0 } };
s_type_f4462a const g_44b138 = { _hs_type_real, 2, function_208310, "<number> <number>", 0, { 0 } };
s_type_f4462a const g_44b148 = { _hs_type_real, 2, function_208310, "<number(s)>", 0, { 0 } };
s_type_f4462a const g_44b158 = { _hs_type_real, 2, function_208310, "<number(s)>", 0, { 0 } };

// @retail 0x2081e0
void __stdcall function_2081e0(short function_index, long thread_index, bool initialize)
{
	s_hs_thread *thread = hs_thread_get(thread_index);
	long *next = hs_frame_allocate_value(thread_index);
	long *value = hs_frame_allocate_value(thread_index);
	s_hs_frame *frame = hs_thread_get(thread_index)->frame;
	bool *result = (bool *)((byte *)frame + offsetof(s_hs_frame, unknown0e) + frame->size);
	frame->size++;
	bool conjunction = function_index == 5;
	if (initialize)
	{
		s_hs_expression *expression = hs_expression_get(thread->frame->expression_index);
		s_hs_expression *name = hs_expression_get(expression->value);
		*next = *(long *)((byte *)name + 8);
		*result = conjunction;
	}
	else
	{
		bool other = *(bool *)value;
		if (conjunction)
			*result = *result && other;
		else
			*result = *result || other;
	}
	if (*next != NONE && *result == conjunction)
	{
		function_2099f0(thread_index, value, *next);
		*next = *(long *)((byte *)hs_expression_get(*next) + 8);
	}
	else
	{
		script_value returned;
		returned.b = *result;
		hs_thread_return_value(thread_index, returned.d);
	}
}

s_type_f4462a const g_44b0e8 = { _hs_type_boolean, 2, function_2081e0, "<boolean(s)>", 0, { 0 } };
s_type_f4462a const g_44b0f8 = { _hs_type_boolean, 2, function_2081e0, "<boolean(s)>", 0, { 0 } };

// @retail 0x2087e0
void __stdcall function_2087e0(short function_index, long thread_index, bool initialize)
{
	s_hs_frame *const *frame_reference = &hs_thread_get(thread_index)->frame;
	long *delay = hs_frame_allocate_value(thread_index);
	long *script = hs_frame_allocate_value(thread_index);
	s_hs_frame *frame = hs_thread_get(thread_index)->frame;
	short *state = (short *)((byte *)frame + offsetof(s_hs_frame, unknown0e) + frame->size);
	frame->size += sizeof(short);
	long target = thread_index;
	if (initialize)
	{
		s_hs_expression *expression = hs_expression_get((*frame_reference)->expression_index);
		s_hs_expression *name = hs_expression_get(expression->value);
		function_2099f0(thread_index, delay, *(long *)((byte *)name + 8));
		*state = 0;
	}
	else
	{
		if (*state == 0)
		{
			s_hs_expression *expression = hs_expression_get((*frame_reference)->expression_index);
			s_hs_expression *name = hs_expression_get(expression->value);
			s_hs_expression *argument = hs_expression_get(*(long *)((byte *)name + 8));
			long script_expression = *(long *)((byte *)argument + 8);
			*state = 1;
			if (script_expression != NONE)
			{
				function_2099f0(thread_index, script, script_expression);
				return;
			}
			*script = NONE;
		}
		if (*state != 0)
		{
			if (*(short *)script != NONE)
				target = hs_thread_for_script(*(short *)script);
			long delay_value;
			if (target != NONE && (delay_value = *(short *)delay) >= 0)
			{
				real seconds = (real)delay_value * 0.03333333507180214f;
				seconds *= (real)g_510c54->field_2_3;
				long ticks;
				__asm
				{
					fld seconds
					fistp ticks
				}
				if (ticks > 0)
				{
					s_hs_thread *thread = hs_thread_get(target);
					long until = g_510c54->game_time + ticks;
					if (thread->sleep_until != NONE)
					{
						if (target != thread_index && !(thread->flags & 2))
						{
							thread->flags |= 2;
							*(long *)thread->unknown0c = thread->sleep_until;
						}
						hs_thread_get(target)->sleep_until = until;
					}
				}
			}
			hs_thread_return_value(thread_index, 0);
		}
	}
}

s_type_f4462a const g_44b1c8 = { _hs_type_void, 2, function_2087e0, "<short> [<script>]", 0, { 0 } };

PRIVATE inline long hs_delay_ticks(long delay)
{
	real seconds = (real)delay * 0.03333333507180214f;
	long ticks;

	seconds *= (real)g_510c54->field_2_3;
	__asm
	{
		fld seconds
		fistp ticks
	}
	return ticks;
}

// @retail 0x2089b0
void __stdcall function_2089b0(short function_index, long thread_index, bool initialize)
{
	s_hs_thread *thread = hs_thread_get(thread_index);
	long *condition = hs_frame_allocate_value(thread_index);
	long *interval = hs_frame_allocate_value(thread_index);
	long *timeout = hs_frame_allocate_value(thread_index);
	long *start = hs_frame_allocate_value(thread_index);
	s_hs_frame *frame = hs_thread_get(thread_index)->frame;
	short *state = (short *)((byte *)frame + offsetof(s_hs_frame, unknown0e) + frame->size);
	frame->size += sizeof(short);
	s_hs_expression *expression = hs_expression_get(thread->frame->expression_index);
	s_hs_expression *name = hs_expression_get(expression->value);
	s_hs_expression *test = hs_expression_get(*(long *)((byte *)name + 8));
	long optional = *(long *)((byte *)test + 8);
	if (initialize)
	{
		*(bool *)condition = false;
		*start = g_510c54->game_time;
		*state = 0;
		*(short *)interval = 30;
		*timeout = NONE;
		if (optional != NONE)
		{
			function_2099f0(thread_index, interval, optional);
			return;
		}
	}
	if (*state == 0)
	{
		*state = 1;
		if (optional != NONE)
		{
			long next = *(long *)((byte *)hs_expression_get(optional) + 8);
			if (next != NONE)
			{
				function_2099f0(thread_index, timeout, next);
				return;
			}
		}
	}
	else if (*state != 1)
		return;
	long duration = NONE;
	if (*timeout != NONE)
		duration = hs_delay_ticks(*timeout);
	if (*(bool *)condition || (duration != NONE && g_510c54->game_time >= *start + duration))
		hs_thread_return_value(thread_index, 0);
	else
	{
		expression = hs_expression_get(thread->frame->expression_index);
		name = hs_expression_get(expression->value);
		function_2099f0(thread_index, condition, *(long *)((byte *)name + 8));
		long ticks = hs_delay_ticks(*(short *)interval);
		thread->sleep_until = g_510c54->game_time + (ticks < 1 ? 1 : ticks);
		if (duration != NONE)
			thread->sleep_until = *start + duration > thread->sleep_until ? thread->sleep_until : *start + duration;
	}
}

s_type_f4462a const g_44b1e8 = { _hs_type_void, 2, function_2089b0, "<boolean> [<short>]", 0, { 0 } };

// @retail 0x207cf0
void __stdcall function_207cf0(short function_index, long thread_index, bool initialize)
{
	s_hs_frame *const *frame_reference = &hs_thread_get(thread_index)->frame;
	s_hs_frame *frame = *frame_reference;
	short size = frame->size;
	short *count = (short *)((byte *)frame + offsetof(s_hs_frame, unknown0e) + size);
	frame->size = size + sizeof(short);
	dword *used = (dword *)hs_frame_allocate_value(thread_index);
	long *result = hs_frame_allocate_value(thread_index);
	if (initialize)
	{
		s_hs_expression *expression = hs_expression_get((*frame_reference)->expression_index);
		s_hs_expression *name = hs_expression_get(expression->value);
		long next = *(long *)((byte *)name + 8);
		*count = 0;
		while (next != NONE)
		{
			next = *(long *)((byte *)hs_expression_get(next) + 8);
			(*count)++;
		}
		memset(used, 0, ((*count + 31) >> 5) * sizeof(dword));
	}
	short choice_count = *count;
	g_4e7408->unknown0 = g_4e7408->unknown0 * 0x19660d + 0x3c6ef35f;
	short first = (short)(((g_4e7408->unknown0 >> 16) * choice_count) >> 16);
	short offset = 0;
	for (; offset < *count; offset++)
	{
		short selected = (first + offset) % *count;
		if (!(used[selected >> 5] & (1 << (selected & 31))))
		{
			s_hs_expression *expression = hs_expression_get((*frame_reference)->expression_index);
			s_hs_expression *name = hs_expression_get(expression->value);
			long next = *(long *)((byte *)name + 8);
			for (short i = 0; i < selected; i++)
				next = *(long *)((byte *)hs_expression_get(next) + 8);
			function_2099f0(thread_index, result, next);
			used[selected >> 5] |= 1 << (selected & 31);
			break;
		}
	}
	if (offset == *count)
		hs_thread_return_value(thread_index, *result);
}

s_type_f4462a const g_44b0a8 = { _hs_type_passthrough, 2, function_207cf0, "<expression(s)>", 0, { 0 } };

// @retail 0x207c20
void __stdcall function_207c20(short function_index, long thread_index, bool initialize)
{
	s_hs_thread *thread = hs_thread_get(thread_index);
	long *next = hs_frame_allocate_value(thread_index);
	long *result = hs_frame_allocate_value(thread_index);
	if (initialize)
	{
		s_hs_expression *expression = hs_expression_get(thread->frame->expression_index);
		s_hs_expression *name = hs_expression_get(expression->value);
		*next = *(long *)((byte *)name + 8);
		*result = 0;
	}
	if (*next != NONE)
	{
		function_2099f0(thread_index, result, *next);
		*next = *(long *)((byte *)hs_expression_get(*next) + 8);
	}
	else
		hs_thread_return_value(thread_index, *result);
}

s_type_f4462a const g_44b098 = { _hs_type_passthrough, 2, function_207c20, "<expression(s)>", 0, { 0 } };

extern short const g_4456d4[6] = { -1, 3, 2, 4, 0x380, 0x40 };

// @retail 0x208f10
void __stdcall function_208f10(short function_index, long thread_index, bool initialize)
{
	s_hs_thread *thread = hs_thread_get(thread_index);
	s_hs_frame **frame_field = &thread->frame;
	s_hs_frame *frame = *frame_field;
	long *value = (long *)((byte *)frame + offsetof(s_hs_frame, unknown0e) + frame->size);
	frame->size += sizeof(long);
	if (initialize)
	{
		s_hs_expression *expression = hs_expression_get((*frame_field)->expression_index);
		s_hs_expression *name = hs_expression_get(expression->value);
		function_2099f0(thread_index, value, *(long *)((byte *)name + 8));
	}
	else
	{
		long object_index = *value;
		if (object_index != NONE)
		{
			byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
			long type_mask = 1 << object[0xaa];
			if (g_4456d4[(short)(function_index - 23)] & type_mask)
				hs_thread_return_value(thread_index, object_index);
			else
				hs_thread_return_value(thread_index, NONE);
		}
		else
			hs_thread_return_value(thread_index, NONE);
	}
}

s_type_f4462a const g_44b218 = { _hs_type_unit, 2, function_208f10, "<object>", 0, { 0 } };

/* calls a script from an expression: evaluates its root into the frame, or
   returns the value computed */
// @retail 0x209bf0
void function_209bf0(long thread_index, short script_index, bool initialize)
{
	s_hs_script *script = hs_script_get(script_index);
	long *value = hs_frame_allocate_value(thread_index);

	if (initialize)
	{
		function_2099f0(thread_index, value, script->root_expression_index);
	}
	else
	{
		hs_thread_return_value(thread_index, *value);
	}
}

/* runs the thread until it sleeps, waits or finishes */
// @retail 0x209850
void function_209850(long thread_index)
{
	s_hs_thread *thread = hs_thread_get(thread_index);
	s_hs_script *script = NULL;

	g_4f938c = thread_index;
	if (thread->type == 0 || thread->type == 4)
	{
		script = hs_script_get(thread->script_index);
	}
	thread->sleep_until = 0;
	if (thread->frame == &thread->stack)
	{
		thread->frame->size = 0;
		function_2099f0(thread_index, hs_frame_allocate_value(thread_index), script->root_expression_index);
	}
	while (thread->frame != &thread->stack && thread->sleep_until >= 0 &&
		(!g_4e6948 || !g_4e6948->flag1120 || thread->sleep_until <= g_510c54->game_time) && g_4f9388)
	{
		s_hs_frame *frame = thread->frame;
		s_hs_expression *expression = hs_expression_get(frame->expression_index);
		bool initialize = (thread->flags & 1) != 0;

		frame->size = 0;
		thread->flags &= ~1;
		if (!(expression->flags & 2))
		{
			short function_index = expression->value_type;

			g_4744e0[function_index]->evaluate(function_index, thread_index, initialize);
		}
		else
		{
			function_209bf0(thread_index, expression->value_type, initialize);
		}
	}
	if (thread->frame == &thread->stack)
	{
		if (thread->type == 0)
		{
			if (script->type == 0 || script->type == 1)
			{
				thread->sleep_until = NONE;
				g_4f938c = NONE;
				return;
			}
		}
		else if (thread->type == 4)
		{
			thread->sleep_until = NONE;
			g_4f938c = NONE;
			return;
		}
		else if (thread->type == 2)
		{
			record_pool_release(g_4f9384, thread_index);
		}
	}
	g_4f938c = NONE;
}

// @retail 0x209490
long function_209490(long expression_index)
{
	long const *expression_reference = &expression_index;
	long result = NONE;
	if (g_4f9388 && *expression_reference != NONE)
	{
		long thread_index = function_2097c0(NONE, 3);
		if (thread_index != NONE)
		{
			s_hs_thread *thread = hs_thread_get(thread_index);
			long *value = &thread->result;
			function_2099f0(thread_index, value, *expression_reference);
			if (thread->flags & 1)
				function_209850(thread_index);
			result = *value;
			record_pool_release(g_4f9384, thread_index);
		}
	}
	return result;
}


extern s_record_pool *g_4f55d8;
void function_20a490(short global_index);

/* an object list header (12 bytes) as 0x208070 counts its references */
struct s_object_list_208070
{
    long unknown0;
    short reference_count;
    short unknown6;
    long unknown8;
};

// score 21 (was 47): the two record pools are loaded in the opposite order, so the pointer of the
// second pool is read before the first one's. In the initialize arm retail forms the global value
// pointer (ebx) before reading the expression; in the retain arm it masks the list index before
// loading the pool (into eax). Tried: a word list index 21 (retail's 361 bytes, movzx); a pool
// base local 21; pointer arithmetic on the pool data 21; the expression index in a local after
// the value pointer 21; the initial expression node in a local 21. Both are register choices (the
// one extra byte is ours loading the object list pool into ecx, not eax): an inline helper for the
// value pointer 21; the retain arm's increment without the list pointer local 22 (the dead lea then
// gains +4), or through an inline list getter 22; the arms swapped (if (!initialize)) 59; the value
// pointer chosen by if/else 68; the return value as a call to function_209bc0 73 (not inlined).
// @retail 0x208070
void __stdcall function_208070(short function_index, long thread_index, bool initialize)
{
    s_hs_frame *frame = hs_thread_get(thread_index)->frame;
    s_hs_expression *expression = hs_expression_get(frame->expression_index);
    s_hs_expression *name = hs_expression_get(expression->value);
    long variable_index = *(long *)((byte *)name + 8);
    s_hs_expression *nodes = (s_hs_expression *)g_4f9394->data;
    frame->size += 4;
    s_hs_expression *variable = &nodes[variable_index & 0xffff];
    long *global_index = &variable->value;
    word initial_index = (word)*global_index;
    short type = (initial_index & 0x8000) ? g_473468[initial_index & 0x7fff]->type :
        ((s_hs_scenario_view *)g_4e0350)->globals[initial_index & 0x7fff].type;
    if (initialize)
    {
        if (type == _hs_type_object_list)
        {
            long list_index = function_209bc0(*global_index);
            if (list_index != NONE)
            {
                s_object_list_208070 *list = &((s_object_list_208070 *)g_4f55d8->data)[list_index & 0xffff];
                list->reference_count--;
            }
        }
        long index = variable->value;
        long slot = (index & 0x8000) ? (index & 0x7fff) : (index & 0x7fff) + 0x41d;
        long *value = (long *)&((s_hs_global_value *)g_4f9380->data)[slot].value;
        function_2099f0(thread_index, value, *(long *)((byte *)hs_expression_get(variable_index) + 8));
    }
    else
    {
        function_20a490(*global_index);
        if (type == _hs_type_object_list)
        {
            long list_index = function_209bc0(*global_index);
            if (list_index != NONE)
            {
                s_object_list_208070 *list = &((s_object_list_208070 *)g_4f55d8->data)[list_index & 0xffff];
                list->reference_count++;
            }
        }
        short index = *global_index;
        function_20a2e0(index);
        long index_long = index;
        long slot = (index_long & 0x8000) ? (index_long & 0x7fff) : (index_long & 0x7fff) + 0x41d;
        hs_thread_return_value(thread_index, ((s_hs_global_value *)g_4f9380->data)[slot].value.d);
    }
}

s_type_f4462a const g_44b0d8 = { _hs_type_passthrough, 2, function_208070, "<variable name> <expression>", 0, { 0 } };

extern long g_4686c0;
long function_16b990(s_record_pool *data, long index);

// score 5 (was 5): retail forms the value pointer (ebx) before it reads and pushes
// the initialization expression; ours reads the expression first. BLOCKED-BY 0x2099f0:
// with its parameters in the order (thread, expression, result) at the definition and
// every call, and the expression read at the call, this matches (and so does 0x208070).
// Tried here: the expression read at the call 72; a result pointer local after the
// resets 72, before them 72, with the expression in a local after them 72; the value
// as a field pointer 72.
// Expression preload precedes resets/destination setup. Allocated record
// reference 5; native value reference 6; unsigned expression bits 5; thread
// rereads 130; size memset 88; script-index memset 12; unsigned allocated
// index 5; external thread reference 5.
// score 4 (was 5): the script index reset before the expression read (found by the permuter).
// @retail 0x2090a0
void function_2090a0(void)
{
    g_4f9384->valid = true;
    record_pool_release_all(g_4f9384);
    g_4f9388 = true;
    g_4f938c = NONE;
    if (g_4686c0 != NONE && g_4e6948->mode != 4)
    {
        byte *scenario = (byte *)g_4e0350;
        long thread_index = function_2097c0(NONE, 1);
        s_hs_thread *thread = hs_thread_get(thread_index);
        for (long i = 0; i < *(long *)(scenario + 0x1c0); i++)
        {
            byte *global = *(byte **)((byte *)g_4e0350 + 0x1c4) + i * 0x28;
            long slot = (i & 0x8000) ? (i & 0x7fff) : (i & 0x7fff) + 0x41d;
            long value_index = function_16b990(g_4f9380, slot);
            s_hs_global_value *value = &((s_hs_global_value *)g_4f9380->data)[value_index & 0xffff];
            thread->script_index = NONE;
            long expression_index = *(long *)(global + 0x24);
            thread->frame->size = 0;
            function_2099f0(thread_index, (long *)&value->value, expression_index);
            if (thread->flags & 1)
            {
                function_209850(thread_index);
                s_record_pool *values = g_4f9380;
                if (*(short *)(global + 0x20) == _hs_type_object_list)
                {
                    function_20a2e0((short)i);
                    long index = (short)i;
                    long value_slot = (index & 0x8000) ? (index & 0x7fff) : (index & 0x7fff) + 0x41d;
                    long list_index = ((s_hs_global_value *)values->data)[value_slot].value.d;
                    if (list_index != NONE)
                    {
                        s_object_list_208070 *list = &((s_object_list_208070 *)g_4f55d8->data)[list_index & 0xffff];
                        list->reference_count++;
                    }
                }
            }
            function_20a490((short)i);
        }
        record_pool_release(g_4f9384, thread_index);
        for (long i = 0; i < *(long *)(scenario + 0x1b8); i++)
        {
            s_hs_script const *script = &((s_hs_scenario_view *)scenario)->scripts[i];
            if (script->type == 0 || script->type == 1 || script->type == 2)
                function_2097c0(i, 0);
        }
    }
}


void hs_scheduler_map_end(void);

// @retail 0x2093d0
long __stdcall function_2093d0(long expression_index, bool option)
{
    (void)&expression_index;
    (void)&option;
    long result = NONE;
    if (expression_index != NONE)
    {
        bool initialized = false;
        if (!g_4f9388)
        {
            function_2090a0();
            initialized = true;
        }
        if (g_4f9388)
        {
            long thread_index = function_2097c0(NONE, 2);
            if (thread_index != NONE)
            {
                s_hs_thread *thread = hs_thread_get(thread_index);
                if (option)
                    thread->flags |= 4;
                else
                    thread->flags &= ~4;
                function_2099f0(thread_index, &thread->result, expression_index);
                if (thread->flags & 1)
                    function_209850(thread_index);
                else
                    result = thread->result;
            }
        }
        if (initialized)
            hs_scheduler_map_end();
    }
    return result;
}


typedef void (__stdcall *t_script_value_formatter)(long, long, char *, long);
void __stdcall hs_boolean_format(long unused, bool value, char *buffer, long maximum_count);
void __stdcall hs_real_format(long unused, long value, char *buffer, long maximum_count);
void __stdcall hs_short_integer_format(long unused, short value, char *buffer, long maximum_count);
void __stdcall hs_long_integer_format(long unused, long value, char *buffer, long maximum_count);
void __stdcall hs_string_format(long unused, char const *value, char *buffer, long maximum_count);
void __stdcall hs_enum_format(short table_index, short name_index, char *buffer, long maximum_count);

// Retail's complete value formatter table, including the unsupported types.
PRIVATE t_script_value_formatter const g_46fd70[62] =
{
    0, 0, 0, 0, 0,
    (t_script_value_formatter)hs_boolean_format,
    (t_script_value_formatter)hs_real_format,
    (t_script_value_formatter)hs_short_integer_format,
    (t_script_value_formatter)hs_long_integer_format,
    (t_script_value_formatter)hs_string_format,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0,
    (t_script_value_formatter)hs_enum_format,
    (t_script_value_formatter)hs_enum_format,
    (t_script_value_formatter)hs_enum_format,
    (t_script_value_formatter)hs_enum_format,
    (t_script_value_formatter)hs_enum_format,
    (t_script_value_formatter)hs_enum_format,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// @retail 0x208e10
void __stdcall function_208e10(short function_index, long thread_index, bool initialize)
{
    typedef void (__stdcall *t_local_formatter)(short, long, char *, long);
    char buffer[1024];
    s_hs_thread *thread = hs_thread_get(thread_index);
    long *value = hs_frame_allocate_value(thread_index);
    if (initialize)
    {
        s_hs_expression *expression = hs_expression_get(thread->frame->expression_index);
        s_hs_expression *name = hs_expression_get(expression->value);
        function_2099f0(thread_index, value, *(long *)((byte *)name + 8));
    }
    else
    {
        s_hs_expression *expression = hs_expression_get(thread->frame->expression_index);
        s_hs_expression *name = hs_expression_get(expression->value);
        s_hs_expression *argument = hs_expression_get(*(long *)((byte *)name + 8));
        if ((thread->flags & 4) && g_46fd70[argument->type])
            ((t_local_formatter)g_46fd70[argument->type])(argument->type, *value, buffer, sizeof(buffer));
        hs_thread_return_value(thread_index, 0);
    }
}

s_type_f4462a const g_44b208 = { _hs_type_void, 2, function_208e10, "<expression>", 0, { 0 } };


extern s_record_pool *g_4f55d4;
bool function_1dee00(long list_index, long object_index);
void function_2963b0(s_record_pool *pool, long index);

// @retail 0x2095e0
void __stdcall function_2095e0(long object_index)
{
    byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
    if ((bool)((*(dword *)(object + 4) >> 28) & 1))
    {
        struct
        {
            s_hs_global_value *value;
            s_record_pool_iterator iterator;
        } globals;
        globals.iterator.data = g_4f9380;
        globals.iterator.index = NONE;
        while ((globals.value = (s_hs_global_value *)data_iterator_next_inlined(&globals.iterator)) != NULL)
        {
            bool external = (short)globals.iterator.datum_index < 0x41d;
            long index = globals.iterator.datum_index;
            if (!external)
                index -= 0x41d;
            short global_index = (index & 0x7fff) | (external ? 0x8000 : 0);
            short type = (global_index & 0x8000) ? g_473468[global_index & 0x7fff]->type :
                ((s_hs_scenario_view *)g_4e0350)->globals[global_index & 0x7fff].type;
            if (type >= _hs_type_object && type <= _hs_type_scenery)
            {
                function_20a2e0(global_index);
                long index_long = global_index;
                long slot = (index_long & 0x8000) ? (index_long & 0x7fff) : (index_long & 0x7fff) + 0x41d;
                if (((s_hs_global_value *)globals.iterator.data->data)[slot].value.d == (dword)object_index)
                {
                    globals.value->value.d = NONE;
                    function_20a490(global_index);
                }
            }
            else if (type == _hs_type_object_list)
            {
                function_20a2e0(global_index);
                long index_long = global_index;
                long slot = (index_long & 0x8000) ? (index_long & 0x7fff) : (index_long & 0x7fff) + 0x41d;
                long list_index = ((s_hs_global_value *)globals.iterator.data->data)[slot].value.d;
                if (function_1dee00(list_index, object_index))
                {
                    if (list_index != NONE)
                    {
                        byte *list = g_4f55d8->data + (list_index & 0xffff) * 12;
                        if (*(short *)(list + 6) != 0)
                            continue;
                        function_2963b0(g_4f55d4, *(long *)(list + 8));
                        record_pool_release(g_4f55d8, list_index);
                    }
                    globals.value->value.d = NONE;
                    function_20a490(global_index);
                }
            }
        }
    }
}
