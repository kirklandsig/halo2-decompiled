/* JOB_QUEUE.H: the asynchronous task queue's nodes (a mutex-guarded free list
   and a work list of 150 nodes of 0x40 bytes; async.cpp) and the tasks the
   file helpers queue (unknown_1a08d0.cpp) */
#ifndef JOB_QUEUE_H
#define JOB_QUEUE_H

#include "unknown_11c920.h"

/* a file, passed by value */
struct s_file_handle
{
	void *handle;
};

struct s_create_file_task
{
	char const *path;
	dword access;
	dword share_mode;
	dword creation_disposition;
	dword flags;
	s_file_handle *file;
	bool create_path;
};

struct s_copy_file_task
{
	long state;
	s_file_handle source;
	s_file_handle destination;
	dword size;
	dword bytes_read;
	dword bytes_written;
	void *buffer;
	bool *success;
};

struct s_read_position_task
{
	s_file_handle file;
	void *buffer;
	dword size;
	dword offset;
	dword *bytes_read_out;
	dword bytes_read;
};

struct s_write_position_task
{
	s_file_handle file;
	void const *buffer;
	dword size;
	dword offset;
	dword *bytes_written_out;
	dword bytes_written;
	dword flags;
};

struct s_copy_position_task
{
	s_file_handle source;
	s_file_handle destination;
	dword source_offset;
	dword destination_offset;
	void *buffer;
	dword size;
	dword *bytes_copied_out;
	dword bytes_copied;
	long writing;
};

struct s_set_file_size_task
{
	s_file_handle file;
	dword size;
	bool *success;
};

struct s_read_entire_file_task
{
	char const *path;
	void *buffer;
	dword buffer_size;
	dword volatile *size_out;
	bool *success;
	s_file_handle file;
	dword size;
};

struct s_file_task
{
	s_file_handle file;
	dword *size_out;
};

struct s_font_load_task
{
	long font_index;
	dword unknown04;
	char name[32];
};

/* a work task: a work function and a copy of its parameters
   (unknown_1a17b0.cpp) */
union s_async_task;
typedef long (__stdcall *async_task_work_function)(s_async_task *task, void *parameters, long parameters_size);

struct s_work_task
{
	async_task_work_function callback;
	byte parameters[0x23];
	char parameters_size;
};

/* a task's own data; its meaning depends on the callback */
union s_async_task
{
	dword data[10];
	s_create_file_task create_file;
	s_copy_file_task copy_file;
	s_read_position_task read_position;
	s_write_position_task write_position;
	s_copy_position_task copy_position;
	s_set_file_size_task set_file_size;
	s_read_entire_file_task read_entire_file;
	s_file_task file;
	s_font_load_task function_1223a0;
	s_work_task work;
};

typedef long (__stdcall *async_work_callback)(s_async_task *task);

struct s_job_node
{
	long priority;
	long state;
	s_async_task task;
	long category;
	async_work_callback callback;
	bool volatile *done;
	s_job_node *next;
};

long async_task_add_work(async_task_work_function callback, long parameters_size, void *parameters, long priority, bool *done);

#endif
