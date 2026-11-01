/* ASYNC.H: the asynchronous task queue (async.cpp, unknown_1a1720.cpp) */
#ifndef ASYNC_H
#define ASYNC_H

#include "unknown_11c920.h"
#include <xtl.h>
#include "job_queue.h"

/* the queue: the task nodes, their free and work lists, the mutexes that
   guard them and the worker thread (async.cpp) */
struct s_async_globals
{
	HANDLE thread;
	HANDLE free_list_mutex;
	HANDLE work_list_mutex;
	HANDLE work_semaphore;
	long task_id_counter;
	s_job_node nodes[150];
	s_job_node *free_list;
	s_job_node *work_list;
	long tasks_added;
};

/* the worker thread's scratch buffer: its own storage, the task that last
   claimed it, the buffer in use (the storage unless one was supplied), its
   size, and whether it is a supplied buffer */
struct s_thread_stack
{
	byte storage[0x8000];
	s_async_task *owner;
	byte *buffer;
	dword size;
	bool external;
};

extern s_async_globals async_globals;
extern s_thread_stack g_4fa0c8;

/* where the last queued file read was, so reads are ordered by position */
struct s_async_insert_state
{
	dword file;
	dword offset;
	bool valid;
};

bool async_task_should_run_before(
	s_async_insert_state *state,
	async_work_callback callback,
	s_async_task const *task,
	long priority,
	async_work_callback other_callback,
	s_async_task const *other_task,
	long other_priority);

void function_120900(s_job_node *node);
long async_task_queue(s_job_node *node);
bool function_120b50(long category);
long function_120ba0(long priority, s_async_task *task, long category, async_work_callback callback, bool volatile *done);
void function_120d50(bool volatile *done, bool idle);
unsigned long __stdcall async_thread_proc(void *parameter);

/* unknown_1a08d0.cpp */
long function_1a0b40(char const *path, dword access_flags, long disposition, dword file_flags, long category, long priority, s_file_handle *file, bool volatile *done);
bool async_copy_file(s_file_handle source, s_file_handle destination, long category);
long function_1a0f10(s_file_handle file, void *buffer, dword size, dword offset, long category, long priority, dword *bytes_read, bool volatile *done);
/* the options of a positioned write (bit 0: flush the file once the data is
   written). 0x1a1050 takes them by value as one object, so every retail caller
   builds them in a register before pushing them (`xor ecx, ecx; push ecx`,
   `mov eax, 1; push eax`) where a plain dword would be pushed as a constant */
struct s_file_write_flags
{
	dword bits;

	s_file_write_flags(dword value) { bits = value; }
};

long function_1a1050(s_file_handle file, void const *buffer, dword size, dword offset, s_file_write_flags flags, long category, long priority, dword *bytes_written, bool volatile *done);

long async_copy_position(s_file_handle source, s_file_handle destination, void *buffer, dword size, dword source_offset, dword destination_offset, long category, long priority, dword *bytes_copied, bool volatile *done);
long function_1a1310(s_file_handle file, dword size, long category, long priority, bool *success, bool volatile *done);
long function_1a1480(char const *path, void *buffer, dword buffer_size, long category, long priority, bool *success, dword *size, bool volatile *done);
long function_1a1550(s_file_handle file, long category, long priority, bool volatile *done);
long function_1a15f0(s_file_handle file, long category, long priority, dword *size, bool volatile *done);
long async_flush_file(s_file_handle file, long category, long priority, bool volatile *done);
void async_flush_file_blocking(s_file_handle file, long category);

#endif
