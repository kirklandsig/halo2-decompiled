// @flags /O2 /Gr
/* ASYNC_WORK.CPP: asynchronous tasks that run a work function with a copy
   of its parameters on the job thread (async.cpp). Moved here from
   unknown_1a17b0.cpp. */

#include "unknown_11c920.h"
#include <xtl.h>
#include <string.h>
#include "async.h"

s_job_node *function_1208b0(void);

/* a work task's callback: runs the work function on its parameters */
// @retail 0x1a17b0
PRIVATE long __stdcall async_task_execute_work(s_async_task *task)
{
	return task->work.callback(task, task->work.parameters, task->work.parameters_size);
}

/* queues a work function with a copy of its parameters (at most the
   task's room for them); *done turns true when it has run */
// @retail 0x1a17d0
long async_task_add_work(async_task_work_function callback, long parameters_size, void *parameters, long priority, bool volatile *done)
{
	struct { s_async_task task; long unused; } frame;
	s_async_task &task = frame.task;
	long result;

	memset(&task, 0, sizeof(task));
	if (parameters_size <= sizeof(task.work.parameters))
	{
		task.work.callback = callback;
		memcpy(task.work.parameters, parameters, parameters_size);
		task.work.parameters_size = (char)parameters_size;

		async_globals.tasks_added++;
		*done = false;
		s_job_node *node = function_1208b0();
		node->task = task;
		node->done = done;
		node->priority = priority;
		node->callback = async_task_execute_work;
		node->category = 0;
		result = async_task_queue(node);
	}
	return result;
}
