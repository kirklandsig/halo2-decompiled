// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_08CA00.CPP: the message of the day, kept in
   n:\message_of_the_day.dat (lane D) */

#include "unknown_11c920.h"
#include "files.h"
#include <string.h>
#include "pending_messages.h"

/* the request that receives the message of the day: its flags (bit 1: there
   is a message) and its kind */
s_pending_message_header g_479780;
char const *g_4672b8 = "message_of_the_day.dat";

static inline void function_x454397(s_type_acf665 *reference)
{
	memset(reference, 0, sizeof(*reference));
	reference->signature = FILE_REFERENCE_SIGNATURE;
	reference->location = NONE;
}

static inline void function_x73bce5(s_type_acf665 *reference, char const *name)
{
	if (reference->flags & 1)
		function_1373c0(reference->path);
	function_137320(reference->path, name);
	reference->flags |= 1;
}

// @retail 0x8ca00
void message_of_the_day_get_file(s_type_acf665 *file)
{
	function_x454397(file);
	function_137320(file->path, "n:\\");
	function_x73bce5(file, g_4672b8);
}

// @retail 0x8ca50
bool message_of_the_day_available(void)
{
	if (g_479780.kind != 0 && (g_479780.flags & 2))
		return true;
	return false;
}

#include "language.h"

// @retail 0x8c900
bool __stdcall function_8c900(const byte *messages)
{
	bool result = false;
	s_type_acf665 file;
	message_of_the_day_get_file(&file);
	if (function_1367d0(&file))
	{
		dword error;
		if (function_136970(&file, 2, &error))
		{
			if (g_47ff38 == NONE)
			{
				g_47ff38 = function_11ca80(XGetLanguage());
			}
			const byte *local_0 = messages + g_47ff38 * 0x200;
			long language = g_47ff38;
			if (function_136d00(&file, 0x200, local_0) &&
				function_136d00(&file, 0x4004, messages + 0x1200 + language * 0x4004))
				result = true;
			if (CloseHandle(file.handle))
			{
				file.handle = 0;
				file.position = 0;
			}
			else
			{
				GetLastError();
				SetLastError(0);
			}
		}
		if (!result)
			file_ref_delete(&file);
	}
	return result;
}

#include "globals.h"

struct s_name_buffer
{
	wchar_t name[256];
};

struct s_name_list
{
	long count;
	s_name_buffer names[32];
};

s_name_list *name_list_clear(s_name_list *list);
void function_08cc20(s_name_buffer *buffer, const wchar_t *name);
bool function_1368f0(s_type_acf665 *file);

#pragma inline_depth(0)
static __forceinline bool function_8ca71(s_type_acf665 *arg_0, dword arg_1, bool arg_2)
{
 return file_ref_seek(arg_0, arg_1, arg_2);
}
#pragma inline_depth(8)
// @retail 0x8ca70
bool __stdcall function_8ca70(long mode, s_name_buffer *output)
{
	bool result = false;
	s_type_acf665 file;
	message_of_the_day_get_file(&file);
	if (function_1368f0(&file) && message_of_the_day_available())
	{
		dword error;
		if (function_136970(&file, 1, &error))
		{
			bool local_c793c4 = true;
			if (file.position != 0)
			{
				file.position = SetFilePointer(file.handle, 0, 0, FILE_BEGIN);
				local_c793c4 = file.position != INVALID_SET_FILE_POINTER;
			}
			if (local_c793c4)
			{
				if (mode == 1)
				{
					s_name_list list;
					name_list_clear(&list);
					function_8ca71(&file, 0x200, true);
					if (file_ref_read(&file, sizeof(list), &list, true))
					{
						if (list.count <= 32)
						{
							short choice_count = (short)list.count;
							g_4e7408->seed = g_4e7408->seed * 0x19660d + 0x3c6ef35f;
							short index = (short)(((g_4e7408->seed >> 16) * choice_count) >> 16);
							function_08cc20(output, list.names[index].name);
							result = true;
						}
					}
				}
				else
				{
					wchar_t message[256] = { 0 };
					if (file_ref_read(&file, sizeof(message), message, true))
					{
						function_08cc20(output, message);
						result = true;
					}
				}
			}
			function_136bb0(&file);
		}
	}
	return result;
}


#include "unknown_080e10.h"
extern s_physical_object *g_4e6464;
long __stdcall function_12d2f0(long size, long user_data, long update, long release);
void function_12c600(void);
void function_12d520(long memory);
double timing_ticks_to_seconds(__int64 ticks);
dword function_199560(void *data, dword size);
bool __stdcall function_199740(byte *buffer, long size, byte *destination, long *decompressed_size);

struct s_4672bc
{
 long unknown00;
 const wchar_t *filename;
 long version;
 struct
 {
  long unknown00;
  bool (__stdcall *handler)(const byte *request);
 } handlers[2];
};

/* folded with 0xa45d0 in retail */
static bool __stdcall function_a45d0(const byte *request)
{
 return true;
}

bool __stdcall function_8c790(const byte *request);

/* the message of the day's handlers, laid out like g_46729c (src/unknown_08c5b0.cpp) */
s_4672bc g_4672bc =
{
 0, L"message_of_the_day.dat", 1,
 { {0, function_a45d0}, {0, function_8c790} }
};

// @retail 0x8c790
bool __stdcall function_8c790(const byte *request)
{
 volatile bool result = false;
 if (*(short *)(request + 8) && (request[4] & 2))
 {
  short kind = *(short *)(request + 0xe);
  if (kind == 1)
  {
   byte *source = 0;
   if (*(byte **)(request + 0x24) && *(dword *)(request + 0x28) > 16)
    source = *(byte **)(request + 0x24) + 16;
   dword size = *(dword *)(request + 0x28) - 16;
   if (source && size > 0 && function_199560(source, size) == 0x25224)
   {
    byte *allocation = (byte *)function_80e14(0x25224, 0, 0, 0, 90, 1.0f);
    if (allocation)
    {
     long decompressed = 0;
     if (function_199740(source, size, allocation, &decompressed) && function_8c900(allocation))
      result = true;
     function_12d520((long)allocation);
    }
   }
  }
 }
 return result;
}
