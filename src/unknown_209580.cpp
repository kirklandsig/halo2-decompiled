// @flags /O2 /Ob1 /Gr
/* UNKNOWN_209580.CPP: runs a script thread if it is due (script_thread_runner; lane
   I's outside function, kept out of line as in retail, where the command
   scripts' 0x258880 calls it) */

#include "unknown_11c920.h"
#include "globals.h"
#include "data_array.h"

/* a script thread (g_4f9384, 0x418 bytes; unknown_209520.cpp) */
struct s_hs_due_thread
{
	byte unknown00[8];
	long sleep_until;
	byte unknown0c[0x418 - 0xc];
};

extern s_record_pool *g_4f9384;
extern s_record_pool *g_4f9380;
extern bool g_4f9388;

// @retail 0x209290
void function_209290(void)
{
	g_4f9384->valid = false;
	s_record_pool_iterator iterator;
	iterator.data = g_4f9380;
	iterator.index = NONE;
	while (data_iterator_next_inlined(&iterator))
	{
		long datum = iterator.datum_index;
		if ((datum & 0xffff) >= 0x41d)
			record_pool_release(iterator.data, datum);
	}
	g_4f9388 = false;
}

void function_209850(long thread_index); /* unknown_209520.cpp */

/* runs the thread if it is due: 0 when it finished, 1 when it sleeps, 2 when
   it waits */
// @retail 0x209580
short function_209580(long thread_index)
{
	s_hs_due_thread *thread = (s_hs_due_thread *)(g_4f9384->data + (thread_index & 0xffff) * sizeof(s_hs_due_thread));
	short result = 2;

	if (thread->sleep_until == -3)
	{
		thread->sleep_until = 0;
	}
	if (thread->sleep_until >= 0 && thread->sleep_until <= g_510c54->game_time)
	{
		function_209850(thread_index);
		result = 0;
		if (thread->sleep_until != NONE)
		{
			result = (thread->sleep_until != -3) + 1;
		}
	}
	return result;
}

PRIVATE inline long next_script_record(s_record_pool *records, long index)
{
	long next = index == NONE ? 0 : (index & 0xffff) + 1;
	return data_datum_index(records, function_16bc00(records, next));
}

// @retail 0x209e70
long function_209e70(short script_index)
{
	s_record_pool *records = g_4f9384;
	for (long index = next_script_record(records, NONE); index != NONE; index = next_script_record(records, index))
	{
		s_hs_due_thread *thread = (s_hs_due_thread *)(records->data + (index & 0xffff) * sizeof(s_hs_due_thread));
		if (*(long *)&thread->unknown00[4] == script_index)
			return index;
	}
	return NONE;
}
