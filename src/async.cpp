// @flags /O2 /Ob1 /arch:SSE /Gr
/* ASYNC.CPP: the asynchronous task queue: 150 task nodes on a free list and
   a work list sorted by priority, each list guarded by a mutex, and a worker
   thread that runs the tasks' callbacks. Moved here from unknown_11fc80.cpp
   (0x1208b0, 0x120a30, 0x120a90, 0x120bf0) and unknown_120ce0.cpp (0x120ce0). */

#include "unknown_11c920.h"
#include "async.h"
#include <xtl.h>
#include <string.h>

void function_125d60(void);

s_async_globals async_globals;
s_job_node *g_510810;

// @retail 0x1208b0
s_job_node *function_1208b0(void)
{
	s_job_node *node = NULL;

	do
	{
		WaitForSingleObject(async_globals.free_list_mutex, INFINITE);
		if (async_globals.free_list)
		{
			node = async_globals.free_list;
			async_globals.free_list = node->next;
			node->next = NULL;
		}
		ReleaseMutex(async_globals.free_list_mutex);
		if (!node)
			SwitchToThread();
	}
	while (!node);
	return node;
}

/* inserts a task into the work list: before the first task it should run
   ahead of, at the head when its priority is higher than the head's */
// @retail 0x120900
void function_120900(s_job_node *node)
{
	long priority;
	s_async_insert_state state;
	memset(&state, 0, sizeof(state));
	s_job_node *previous = async_globals.work_list;

	if (!previous)
	{
		async_globals.work_list = node;
		node->next = previous;
		return;
	}
	priority = node->priority;
	if (priority > previous->priority)
	{
		node->next = previous;
		async_globals.work_list = node;
		return;
	}
	for (; previous->next; previous = previous->next)
	{
		s_job_node *next = previous->next;

		if (async_task_should_run_before(&state, node->callback, &node->task, priority, next->callback, &next->task, next->priority))
		{
			node->next = previous->next;
			previous->next = node;
			return;
		}
	}
	previous->next = node;
	node->next = NULL;
}

// @retail 0x1209c0
long async_task_queue(s_job_node *node)
{
	long task_id;

	WaitForSingleObject(async_globals.work_list_mutex, INFINITE);
	function_120900(node);
	node->state = node - async_globals.nodes;
	node->state = (async_globals.task_id_counter << 8) | node->state;
	async_globals.task_id_counter++;
	task_id = node->state;
	ReleaseMutex(async_globals.work_list_mutex);
	{
		LONG previous_count = 0;
		ReleaseSemaphore(async_globals.work_semaphore, 1, &previous_count);
	}
	return task_id;
}

// @retail 0x120a30
void function_120a30(s_job_node *node)
{
	WaitForSingleObject(async_globals.work_list_mutex, INFINITE);
	if (async_globals.work_list == node)
	{
		async_globals.work_list = node->next;
		node->state = NONE;
	}
	else
	{
		s_job_node *previous = async_globals.work_list;
		while (previous->next != node)
			previous = previous->next;
		previous->next = node->next;
		node->state = NONE;
	}
	ReleaseMutex(async_globals.work_list_mutex);
}

// @retail 0x120a90
void function_120a90(void)
{
	memset(async_globals.nodes, 0, sizeof(async_globals.nodes));
	for (long i = 0; i < 150; i++)
	{
		async_globals.nodes[i].next = &async_globals.nodes[i + 1];
		async_globals.nodes[i].state = NONE;
	}
	async_globals.nodes[149].next = NULL;
	async_globals.free_list = async_globals.nodes;
	async_globals.work_list = NULL;
	async_globals.free_list_mutex = CreateMutexA(NULL, FALSE, NULL);
	async_globals.work_list_mutex = CreateMutexA(NULL, FALSE, NULL);
	async_globals.work_semaphore = CreateSemaphoreA(NULL, 0, 150, NULL);
	g_4fa0c8.owner = NULL;
	g_4fa0c8.buffer = g_4fa0c8.storage;
	g_4fa0c8.size = sizeof(g_4fa0c8.storage);
	async_globals.thread = CreateThread(NULL, 0x4000, async_thread_proc, NULL, 0, NULL);
	SetThreadPriority(async_globals.thread, 1);
}

// @retail 0x120b50
bool function_120b50(long category)
{
	bool result = false;
	s_job_node *node;

	WaitForSingleObject(async_globals.work_list_mutex, INFINITE);
	for (node = async_globals.work_list; node; node = node->next)
	{
		if (node->category == category)
		{
			result = true;
			break;
		}
	}
	ReleaseMutex(async_globals.work_list_mutex);
	return result;
}

// @retail 0x120ba0
inline long function_120ba0(long priority, s_async_task *task, long category, async_work_callback callback, bool volatile *done)
{
	s_job_node *node;

	async_globals.tasks_added++;
	*done = false;
	node = function_1208b0();
	node->task = *task;
	node->done = done;
	node->priority = priority;
	node->callback = callback;
	node->category = category;
	return async_task_queue(node);
}

// @retail 0x120bf0
inline long function_120bf0(void)
{
	long count = 0;

	WaitForSingleObject(async_globals.work_list_mutex, INFINITE);
	for (s_job_node *node = async_globals.work_list; node; node = node->next)
		count++;
	ReleaseMutex(async_globals.work_list_mutex);
	return count;
}

__declspec(noreturn) void async_work_loop(void);

// @retail 0x120c40
void async_work_loop(void)
{
	for (;;)
	{
		s_job_node *node;
		long result;

		WaitForSingleObject(async_globals.work_semaphore, INFINITE);
		WaitForSingleObject(async_globals.work_list_mutex, INFINITE);
		node = async_globals.work_list;
		ReleaseMutex(async_globals.work_list_mutex);
		g_510810 = node;
		result = node->callback(&node->task);
		if (result != 0)
		{
			if (result == 1 && node->done)
				*node->done = true;
			function_120a30(node);
			WaitForSingleObject(async_globals.free_list_mutex, INFINITE);
			node->next = async_globals.free_list;
			async_globals.free_list = node;
			ReleaseMutex(async_globals.free_list_mutex);
		}
		else
		{
			LONG previous_count = 0;
			ReleaseSemaphore(async_globals.work_semaphore, 1, &previous_count);
		}
	}
}

// @retail 0x120c30
unsigned long __stdcall async_thread_proc(void *parameter)
{
	async_work_loop();
	return 0;
}

// @retail 0x120ce0
bool function_120ce0(long job, long priority)
{
	bool result = false;
	s_job_node *node;

	WaitForSingleObject(async_globals.work_list_mutex, INFINITE);
	node = &async_globals.nodes[job & 0xff];
	if (node->state == job)
	{
		if (async_globals.work_list == node)
		{
			async_globals.work_list = node->next;
		}
		else
		{
			s_job_node *previous = async_globals.work_list;
			while (previous->next != node)
				previous = previous->next;
			previous->next = node->next;
		}
		node->priority = priority;
		function_120900(node);
		result = true;
	}
	ReleaseMutex(async_globals.work_list_mutex);
	return result;
}

static __forceinline bool function_120d51(bool volatile *arg_0)
{
	return *arg_0;
}

// @retail 0x120d50
inline void function_120d50(bool volatile *done, bool idle)
{
	if (!function_120d51(done))
	{
		while (!*done)
		{
			SwitchToThread();
			if (idle)
				function_125d60();
		}
	}
}
