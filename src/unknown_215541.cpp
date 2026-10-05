#include "unknown_11c920.h"
#include "globals.h"

// @flags /O1 /Gr

class c_slots_215541
{
public:
	void function_215513(void);
	void function_215541(void *entry);
	long function_215561(void);
	long unknown00;
	void *entries[8];
};

void function_120d50(bool volatile *done, bool idle);
void __stdcall function_2154b4(void *pointer);

// @retail 0x215513
void c_slots_215541::function_215513(void)
{
	for (long i = 0; i < 8; i++)
	{
		if (entries[i])
		{
			function_120d50((bool volatile *)entries[i], true);
			function_2154b4(entries[i]);
			entries[i] = NULL;
		}
	}
}

// @retail 0x215541
void c_slots_215541::function_215541(void *entry)
{
	for (long i = 0; i < 8; i++)
	{
		if (!entries[i])
		{
			entries[i] = entry;
			break;
		}
	}
}

// @retail 0x215561
long c_slots_215541::function_215561(void)
{
	long result = 1;
	for (long i = 0; i < 8; i++)
	{
		if (entries[i])
		{
			if (*(bool *)entries[i])
			{
				function_2154b4(entries[i]);
				entries[i] = NULL;
			}
			else
			{
				result = 0;
			}
		}
	}
	return result;
}

// @retail 0x215651
long __stdcall function_215651(dword elapsed)
{
	(void)&elapsed;
	long result;
	if (elapsed > 500 && elapsed < 3000)
		result = 0;
	else
		result = 1;
	return result;
}

struct s_storage_request
{
	bool done;
	byte unknown01;
	bool cancelled;
	byte unknown03;
	long result;
	byte unknown008[0x10c - 8];
	char controller;
	bool option;
	byte unknown10e[2];
	dword started;
	byte unknown114[0x2f4 - 0x114];
};

extern dword g_54d5b8;
extern long g_51ea10;
bool function_138800(void);
long function_1910b8(long controller);
void *__stdcall function_1a47fd(unsigned int size);
class c_campaign_options_list;
typedef bool (__stdcall *storage_progress_callback)(c_campaign_options_list *list, long unused, real *fraction, long *error);
void __stdcall function_2acab4(long a, long user_flags, long title, storage_progress_callback progress, long cleanup, c_campaign_options_list *context);

// @retail 0x215593
long __stdcall function_215593(void *context, long *description, real *fraction, long *error)
{
	long result;
	s_storage_request *request = (s_storage_request *)context;
	if (request->done && (byte)function_215651(g_54d5b8 - request->started))
	{
		if (request->cancelled)
			*error = 0;
		else
			*error = request->result;
		result = 1;
	}
	else
	{
		result = 0;
		*description = 0x13000159;
	}
	return result;
}

// @retail 0x2155dc
void __stdcall function_2155dc(void *context)
{
	function_120d50((bool volatile *)context, true);
	function_2154b4(context);
}

// @retail 0x2154cd
void function_2154cd(s_storage_request *request, long controller, long title)
{
	(void)&title;
	request->controller = (char)controller;
	long index;
	if (function_138800() && g_4e6948->state == 3)
		index = 4;
	else
		index = function_1910b8(controller);
	function_2acab4(index, 1 << controller, title, (storage_progress_callback)function_215593,
		(long)function_2155dc, (c_campaign_options_list *)request);
}

// @retail 0x215454
s_storage_request *__stdcall function_215454(c_slots_215541 *slots, bool option)
{
	(void)&slots;
	(void)&option;
	s_storage_request *request = NULL;
	if (function_138800())
	{
		request = (s_storage_request *)function_1a47fd(sizeof(s_storage_request));
		if (request)
		{
			s_storage_request volatile *initializing = request;
			g_51ea10++;
			initializing->done = true;
			initializing->cancelled = false;
			initializing->result = 4;
			initializing->option = option;
			initializing->started = g_54d5b8;
		}
		if (slots && request)
			slots->function_215541(request);
	}
	return request;
}

// @retail 0x2155f4
long __stdcall function_2155f4(c_slots_215541 *slots, long *description, real *fraction, long *error)
{
	(void)&slots;
	(void)&description;
	(void)&fraction;
	(void)&error;
	c_slots_215541 *const *slots_reference = &slots;
	long result = 0;
	bool done = (*slots_reference)->function_215561() == 1;
	dword elapsed = g_54d5b8 - (*slots_reference)->unknown00;
	if (done && (byte)function_215651(elapsed))
		result = 1;
	*description = elapsed > 1000 ? 0x14000718 : 0x15000717;
	return result;
}
