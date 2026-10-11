// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_08E1E0.CPP: a table of 32 pending messages with two queues of 16
   indices into it (lane D) */

#include "unknown_11c920.h"
#include <string.h>
#include "crc.h"
#include "pending_messages.h"
#include "online_tasks.h"


/* function_x86aaf2, which retail inlines here (this file is /Ob1 so that 0x8e1e0 stays
   a call) */
static inline void crc_new_inlined(dword *crc_reference)
{
	*crc_reference = 0xffffffff;
}

long g_4d8ba8[2][16];
s_pending_message g_4d8c28[32];

// @retail 0x8e1e0
long pending_message_find_free(void)
{
	long index = NONE;
	long i = 0;

	do
	{
		if (i >= 32)
			break;
		index = !g_4d8c28[i].header ? i : index;
		i++;
	} while (index == NONE);
	return index;
}

static __forceinline void function_8e215(long arg_0)
{
 s_pending_message *local_0 = g_4d8c28 + arg_0;
 local_0->header = 0;
 local_0->task_index = NONE;
 local_0->size = 0;
 local_0->data = 0;
}

// @retail 0x8e210
void pending_messages_reset(void)
{
	memset(g_4d8ba8, NONE, sizeof(g_4d8ba8));
	long i = 0;
	do
	{
		function_8e215(i);
		i++;
	} while (i < 32);
}

// @retail 0x8e500
long pending_message_add(long size, s_pending_message_header *header, void *data)
{
	long index = pending_message_find_free();
	long slot = NONE;

	long i = 0;
	do
	{
		if (i >= 16)
			break;
		slot = g_4d8ba8[0][i] == NONE ? i : slot;
		i++;
	} while (slot == NONE);
	if (header->kind == 1 || header->kind == 2)
	{
		crc_new_inlined((dword *)data);
		function_163ba0((dword *)data, data, size);
	}
	g_4d8c28[index].size = size;
	g_4d8c28[index].data = data;
	g_4d8c28[index].header = header;
	g_4d8c28[index].task_index = NONE;
	g_4d8ba8[0][slot] = index;
	return index;
}

// @retail 0x8e580
long pending_message_add_received(s_pending_message_header *header, void *data, long size)
{
	long index = pending_message_find_free();
	long slot = NONE;

	long i = 0;
	do
	{
		if (i >= 16)
			break;
		slot = g_4d8ba8[1][i] == NONE ? i : slot;
		i++;
	} while (slot == NONE);
	s_pending_message_header *local_0 = *(s_pending_message_header *volatile *)&header;
	g_4d8c28[index].task_index = NONE;
	g_4d8c28[index].size = size;
	g_4d8c28[index].header = local_0;
	g_4d8c28[index].data = data;
	g_4d8ba8[1][slot] = index;
	return index;
}

void function_6b640(long task_index);
void function_81050(long result, s_pending_message_header *header);
void function_12d520(long memory);
void __stdcall function_812d0(void *allocation, s_pending_message_header *header);

void __stdcall function_8e0f0(long index, long result);
#pragma optimize("g", off)
PRIVATE __forceinline void function_8e0f1(s_pending_message_header *arg_0, long arg_1)
{
 if (arg_0->kind && arg_1 != NONE)
  function_8e0f0(arg_1, 13);
 void *local_0 = arg_0->data;
 if (local_0)
 {
  function_12d520((long)local_0);
  arg_0->data = 0;
  arg_0->size = 0;
 }
}
#pragma optimize("", on)
// @retail 0x8e0f0
void __stdcall function_8e0f0(long index, long result)
{
 if (g_4d8c28[index].header)
 {
  if (g_4d8c28[index].task_index != NONE)
   function_6b640(g_4d8c28[index].task_index);
  function_81050(result, g_4d8c28[index].header);
  s_pending_message_header *header = g_4d8c28[index].header;
  if (header->kind == 4)
  {
   long local_0 = header->pending_index;
   if (local_0 == NONE)
    function_8e0f1(header, local_0);
  }
  g_4d8c28[index].header = 0;
  g_4d8c28[index].task_index = NONE;
  g_4d8c28[index].size = 0;
  g_4d8c28[index].data = 0;
  for (long queue = 0; queue < 2; queue++)
  {
   for (long slot = 0; slot < 16; slot++)
   {
    if (g_4d8ba8[queue][slot] == index)
     g_4d8ba8[queue][slot] = NONE;
    if (slot > 0 && g_4d8ba8[queue][slot - 1] == NONE)
    {
     g_4d8ba8[queue][slot - 1] = g_4d8ba8[queue][slot];
     g_4d8ba8[queue][slot] = NONE;
    }
   }
  }
 }
}

struct s_pending_payload_header;
long pending_message_request_validate(s_pending_message_header *request, const s_pending_payload_header *payload, dword size);
long function_b45f0(long task_index, dword *size, dword *transferred);

// @retail 0x8e5e0
void function_8e5e0(long status, long task_index)
{
 long index = NONE;
 for (long i = 0; i < 32; i++)
 {
  if (g_4d8c28[i].header && g_4d8c28[i].task_index == task_index)
  {
   index = i;
   break;
  }
 }
 s_pending_message *message = &g_4d8c28[index];
 long type = online_task_get_unchecked(message->task_index)->type;
 dword size = 0;
 dword transferred = 0;
 if (!status && type == 0x2a)
  status = function_b45f0(task_index, &size, &transferred);
 long result;
 switch (status)
 {
 case 0:
  result = 1;
  if (type == 0x2a)
  {
   if (size > (dword)message->size)
    result = 9;
   else if (size != transferred)
    result = 7;
   else
    result = pending_message_request_validate(message->header,
     (const s_pending_payload_header *)message->data, size);
  }
  break;
 case 1: result = 2; break;
 case 2: result = 3; break;
 case 3: result = 4; break;
 case 4: result = 5; break;
 case 5: result = 6; break;
 case 6: result = 7; break;
 default: result = 2; break;
 }
 function_8e0f0(index, result);
}

#include <xtl.h>
#include <xonline.h>

struct s_pending_message_storage;
bool function_80e10(s_pending_message_storage *request, void **output, long *size);
long function_b4810(long kind, long controller, XUID owner, const wchar_t *filename, const char *directory, long *task_out);
long function_b4450(long kind, long controller, XUID owner, const wchar_t *filename, byte *buffer, dword size, long *task_out);
long function_b4670(long kind, long controller, XUID owner, const wchar_t *filename, const char *directory, long *task_out);
long function_b42b0(long kind, long controller, XUID owner, const wchar_t *filename, byte *buffer, dword size, long *task_out);

struct s_pending_transfer_definition
{
 long kind;
 const wchar_t *filename;
 dword version;
 const char *directory;
};
const long g_4402a4[2] = { 1, 1 };

// @retail 0x8e240
void function_8e240(void)
{
 for (long queue = 0; queue < 2; queue++)
 {
  long *indices = g_4d8ba8[queue];
  long maximum = g_4402a4[queue];
  long active = 0;
  for (long i = 0; i < 16; i++)
   if (indices[i] != NONE && g_4d8c28[indices[i]].task_index != NONE)
    active++;
  long slot = 0;
  bool failed = false;
  for (; slot < 16 && active < maximum && !failed; slot++)
  {
   long index = indices[slot];
   if (index != NONE)
   {
    s_pending_message *message = &g_4d8c28[index];
    if (message->task_index == NONE)
    {
     s_pending_message_header *header = message->header;
     s_pending_transfer_definition *definition = (s_pending_transfer_definition *)header->unknown00;
     long controller = header->unknown10;
     XUID owner = *(const XUID *)&header->values;
     long kind = definition->kind;
     const wchar_t *filename = definition->filename;
     long task = NONE;
     long status = 0;
     switch (queue)
     {
     case 0:
      {
      // Reload the request's definition before testing its transfer kind.
      s_pending_transfer_definition *current_definition =
       (s_pending_transfer_definition *)*(volatile dword *)&header->unknown00;
      if (header->kind == 3)
       status = function_b4670(kind, controller, owner, filename, current_definition->directory, &task);
      else
       status = function_b42b0(kind, controller, owner, filename, (byte *)message->data, message->size, &task);
      }
      break;
     case 1:
      {
      s_pending_transfer_definition *current_definition =
       (s_pending_transfer_definition *)*(volatile dword *)&header->unknown00;
      if (header->kind == 3)
       status = function_b4810(kind, controller, owner, filename, current_definition->directory, &task);
      else
      {
       if (header->kind == 4 && !function_80e10((s_pending_message_storage *)header, &message->data, &message->size))
       {
        failed = true;
        continue;
       }
       status = function_b4450(kind, controller, owner, filename, (byte *)message->data, message->size, &task);
      }
      }
      break;
     }
     if (!status)
     {
      active++;
      message->task_index = task;
     }
     else
     {
      long result;
      switch (status)
      {
      case 2: result = 3; break;
      case 3: result = 4; break;
      case 4: result = 5; break;
      case 5: result = 6; break;
      default: result = 2; break;
      }
      function_8e0f0(index, result);
      if (slot < 15)
       slot--;
      failed = true;
     }
    }
   }
  }
 }
}
