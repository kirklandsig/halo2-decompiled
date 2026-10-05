#include "unknown_11c920.h"
#include "data_array.h"

// @flags /O2 /Gr

struct s_script_stack_link
{
	s_script_stack_link *next;
};

struct s_script_stack_view
{
	byte unknown00[0x10];
	s_script_stack_link *frame;
	byte unknown14[0x418 - 0x14];
};

extern s_record_pool *g_4f9384;

// @retail 0x209c60
void function_209c60(long thread_index)
{
	s_script_stack_view *thread = &((s_script_stack_view *)g_4f9384->data)[thread_index & 0xffff];
	thread->frame = thread->frame->next;
}
