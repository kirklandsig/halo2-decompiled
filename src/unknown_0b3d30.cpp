// @flags /O2 /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_053310.h"
#include "online_tasks.h"
#include <xtl.h>
#include <xonline.h>
#include <wchar.h>

dword g_547610;
long g_547614;
long g_547618;
byte g_54761c;
long g_547620;
byte g_546a8c[1];

bool __stdcall function_b5e40(void *block);
void online_result_registration_clear(void);
void function_6b640(long task_index);
extern bool g_510580;
extern long g_510584;
extern dword g_5107d8;

// @retail 0xb3d30
void __stdcall function_b3d30(long stage)
{
	long size;

	if (stage > 1)
	{
		if (stage > 3)
		{
			return;
		}
		size = 0x400;
	}
	else
	{
		size = 0xa000;
	}

	g_547610 = (dword)physical_memory_malloc_fixed(size, PAGE_READWRITE);
	g_547614 = size;
}

// @retail 0xb3db0
void function_b3db0(void)
{
	if (g_547610)
	{
		function_b5e40(g_546a8c);
		g_547610 = 0;
		g_547614 = 0;
		g_547618 = 0;
		g_54761c = 0;
	}
}

// @retail 0xb4020
void function_b4020(void)
{
	g_54761c = 0;
	online_result_registration_clear();
	long task_index = g_510584;
	if (task_index != NONE)
	{
		function_6b640(task_index);
		g_510584 = NONE;
	}
	g_510580 = false;
}

// @retail 0xb4050
void function_b4050(void)
{
	long iteration = g_547620 + 1;
	g_547620 = iteration;
	long count = 0;
	if (g_510580) count = g_5107d8;
	if (iteration >= 3 * count)
	{
		online_result_registration_clear();
		long task_index = g_510584;
		if (task_index != NONE)
		{
			function_6b640(task_index);
			g_510584 = NONE;
		}
		g_510580 = false;
		g_54761c = 0;
	}
}


// @retail 0xb45f0
long function_b45f0(long task_index, dword *total_size, dword *received_size)
{
    s_type_9df9da *task = online_task_try_get(task_index);
    BYTE *received = 0;
    ULONGLONG owner;
    FILETIME creation;
    if (XOnlineStorageDownloadToMemoryGetResults((XONLINETASK_HANDLE)task->handle, &received, received_size, total_size, &owner, &creation) < 0)
        task->flags |= 0x20;
    return 0;
}


extern bool g_50944e;
wchar_t const *g_4672dc = L"test";
void unicode_string_snprintf(word *buffer, long maximum_count, word const *format, ...);

// @retail 0xb4120
bool function_b4120(long kind, struct _XUID owner, wchar_t const *filename, wchar_t *arg_d4faef, dword *path_size)
{
    long facility;
    unsigned long length = 0;
    switch (kind)
    {
    case 0: facility = 4; break;
    case 1: facility = 5; break;
    default: facility = 3; break;
    }
    unsigned __int64 user_id = 0;
    unsigned __int64 team_id = 0;
    wchar_t prefix[0x100];
    wchar_t path[0x100];
    if (facility == 4)
    {
        if (!g_50944e)
            unicode_string_snprintf((word *)prefix, 0x100, (word const *)L"%d", 0x2651);
        else
        {
            wcsncpy(prefix, g_4672dc, 0xff);
            prefix[0xff] = 0;
        }
    }
    else
    {
        wcsncpy(prefix, L"", 0xff);
        prefix[0xff] = 0;
    }
    for (; length < 0x100; length++)
        if (!prefix[length]) break;
    if (length > 0)
        unicode_string_snprintf((word *)path, 0x100, (word const *)L"%s/%s", prefix, filename);
    else
    {
        wcsncpy(path, filename, 0xff);
        path[0xff] = 0;
    }
    if (facility == 3) team_id = owner.qwUserID;
    else user_id = owner.qwUserID;
    if (XOnlineStorageCreateServerPath(facility, user_id, team_id, path, arg_d4faef, path_size) < 0)
        return false;
    return true;
}

#include "unknown_0b4da0.h"

struct s_http_connection_ab
{
    s_type_99af70 address;
    char hostname[16];
    s_transport_endpoint internal_endpoint;
    s_transport_endpoint *endpoint;
    byte unknown30[4];
    char path[0x80];
    byte unknownb4[0x634 - 0xb4];
    long upload_header_length;
    long upload_part_length;
    long upload_ending_length;
    byte unknown640[0x96c - 0x640];
    long upload_file_length;
    long upload_position;
    char response[0x200];
    long response_length;
    long attempt_count;
    long state;
    bool connect(long address, word port, char const *path, bool flag);
    bool receive(bool *complete);
    bool send();
};


word g_546a80;
char const *g_4672d8 = "/upload_server/stats.ashx";
bool online_result_get_address(long index, IN_ADDR *address);

// @retail 0xb40b0
bool function_b40b0()
{
    long index = (dword)g_547620 % g_5107d8;
    volatile bool result = false;
    volatile bool failure = false;
    IN_ADDR address;
    if (online_result_get_address(index, &address))
    {
        if (((s_http_connection_ab *)g_546a8c)->connect(address.s_addr, g_546a80, g_4672d8, false))
            return true;
        else return result;
    }
    return result;
}

dword g_547624;
bool function_b5e90(s_http_connection_ab *connection, bool *complete);

// @retail 0xb3fc0
void function_b3fc0()
{
    if (!((s_http_connection_ab *)g_546a8c)->state)
    {
        if (!g_510580 || !function_b40b0())
            function_b4050();
    }
    else
    {
        bool complete = false;
        if (!function_b5e90((s_http_connection_ab *)g_546a8c, &complete) || GetTickCount() >= g_547624)
            function_b4050();
        else if (complete)
            function_b4020();
    }
}

bool g_546a89;
bool online_result_service_connected();
void online_result_cache_start();
void online_result_cache_update();

// @retail 0xb3de0
void function_b3de0()
{
    bool connected = online_result_service_connected();
    if (g_546a89 != connected)
    {
        g_546a89 = connected;
        online_result_registration_clear();
        if (g_510584 != NONE)
        {
            function_6b640(g_510584);
            g_510584 = NONE;
        }
        g_510580 = false;
        function_b5e40(g_546a8c);
        g_547620 = 0;
    }
    else if (connected && g_510584 != NONE)
    {
        if (GetTickCount() < g_547624)
        {
            online_result_cache_update();
            if (g_510584 != NONE || g_510580)
                return;
        }
        function_b4050();
    }
    else if (g_54761c)
    {
        if (!connected)
            function_b4050();
        else if (!g_510580)
            online_result_cache_start();
        else
            function_b3fc0();
    }
}

static inline long storage_facility_ab(long kind)
{
    switch (kind)
    {
    case 0: return 4;
    case 1: return 5;
    default: return 3;
    }
}

static __forceinline long storage_error_ab(HRESULT error)
{
    switch (error)
    {
    case 0x8007000e:
    case 0x80150002:
    case 0x80150003:
    case 0x80150005:
    case 0x80150006:
    case 0x80150008:
        return 2;
    case 0x80150004:
    case 0x8015c002:
        return 3;
    case 0x8015c006:
    case 0x8015c008:
    case 0x8015c009:
        return 4;
    case 0x8015c004:
    case 0x8015c007:
        return 5;
    default:
        return 1;
    }
}

// @retail 0xb4810
long function_b4810(long kind, long controller, struct _XUID owner, wchar_t const *filename, char const *directory, long *task_out)
{
    (void)&controller;
    long result = 1;
    *task_out = NONE;
    long facility = storage_facility_ab(kind);
    wchar_t path[0x100];
    dword path_size = 0x100;
    if (function_b4120(kind, owner, filename, path, &path_size))
    {
        long task_index = online_task_new_if_logged_on();
        s_type_9df9da *task = function_6b910(task_index);
        if (task)
        {
            HRESULT status = XOnlineStorageDownload(facility, controller, path, directory, 0, 0, (XONLINETASK_HANDLE *)&task->handle);
            if (status >= 0)
            {
                *task_out = task_index;
                task->flags = 1;
                task->type = 0x2c;
                task->controller_index = controller;
                return 0;
            }
            result = storage_error_ab(status);
            function_6b640(task_index);
        }
    }
    return result;
}

// @retail 0xb4450
long function_b4450(long kind, long controller, struct _XUID owner, wchar_t const *filename, byte *buffer, dword size, long *task_out)
{
    (void)&controller;
    long *const volatile *output_reference = &task_out;
    task_out = *output_reference;
    long result = 1;
    *task_out = NONE;
    long facility = storage_facility_ab(kind);
    wchar_t path[0x100];
    dword path_size = 0x100;
    if (function_b4120(kind, owner, filename, path, &path_size))
    {
        long task_index = online_task_new_if_logged_on();
        s_type_9df9da *task = function_6b910(task_index);
        if (task)
        {
            HRESULT status = XOnlineStorageDownloadToMemory(facility, controller, path, buffer, size, 0, 0, (XONLINETASK_HANDLE *)&task->handle);
            if (status >= 0)
            {
                *task_out = task_index;
                task->flags = 1;
                task->type = 0x2a;
                task->controller_index = controller;
                return 0;
            }
            result = storage_error_ab(status);
            function_6b640(task_index);
        }
    }
    return result;
}

// @retail 0xb4670
long function_b4670(long kind, long controller, struct _XUID owner, wchar_t const *filename, char const *directory, long *task_out)
{
    (void)&controller;
    *task_out = NONE;
    long result = 1;
    long facility = storage_facility_ab(kind);
    wchar_t path[0x100];
    dword path_size = 0x100;
    if (function_b4120(kind, owner, filename, path, &path_size))
    {
        long task_index = online_task_new_if_logged_on();
        s_type_9df9da *task = function_6b910(task_index);
        if (task)
        {
            FILETIME expiration = { 0, 0 };
            HRESULT status = XOnlineStorageUploadByServerPath(facility, controller, path, expiration, directory, 0, 0, (XONLINETASK_HANDLE *)&task->handle);
            if (status >= 0)
            {
                *task_out = task_index;
                task->flags = 1;
                task->type = 0x2c;
                task->controller_index = controller;
                return 0;
            }
            result = storage_error_ab(status);
            function_6b640(task_index);
        }
        else return 1;
    }
    return result;
}

// @retail 0xb42b0
long function_b42b0(long kind, long controller, struct _XUID owner, wchar_t const *filename, byte *buffer, dword size, long *task_out)
{
    (void)&controller;
    long result = 1;
    *task_out = NONE;
    long facility = storage_facility_ab(kind);
    wchar_t path[0x100];
    dword path_size = 0x100;
    if (function_b4120(kind, owner, filename, path, &path_size))
    {
        long task_index = online_task_new_if_logged_on();
        s_type_9df9da *task = function_6b910(task_index);
        if (task)
        {
            FILETIME expiration = { 0, 0 };
            HRESULT status = XOnlineStorageUploadFromMemory(facility, controller, path, expiration, buffer, size, 0, 0, (XONLINETASK_HANDLE *)&task->handle);
            if (status >= 0)
            {
                *task_out = task_index;
                task->flags = 1;
                task->type = 0x29;
                task->controller_index = controller;
                return 0;
            }
            switch (status)
            {
            case 0x8007000e:
            case 0x80150002:
            case 0x80150003:
            case 0x80150005:
            case 0x80150006:
            case 0x80150008: result = 2; break;
            case 0x80150004:
            case 0x8015c002: result = 3; break;
            case 0x8015c006:
            case 0x8015c008:
            case 0x8015c009: result = 4; break;
            default: result = 1; break;
            }
            function_6b640(task_index);
        }
        else return 1;
    }
    return result;
}


// Disabled: retail passes shared upload-state addresses to the serializer and publisher.
#if 0
extern byte g_546a88;
extern long g_546abc;
extern dword g_5473f0;
extern long g_5473f8, g_547624;
extern char g_5470cc[32];
bool __stdcall function_1995a0(void const *data, void *buffer, long *size, long capacity, long mode, long kind);
void function_b6570(long *state);

// Retail 0xb3ed0
bool __stdcall function_b3ed0(s_packed_clc const *data, long kind, char const *name)
{
    bool result = false;
    if (g_547610 && !g_54761c && !g_546a88)
    {
        if (function_1995a0(data, (void *)(g_547610 + 8), &g_547618, g_547614 - 8, 3, kind))
        {
            *(long *)g_547610 = g_547618 + 8;
            *(long *)(g_547610 + 4) = 1;
            g_547618 += 8;
            g_546abc = 1;
            g_5473f0 = g_547610;
            g_5473f8 = g_547618;
            function_b6570(&g_546abc);
            strncpy(g_5470cc, name, 32);
            g_5470cc[31] = 0;
            g_547620 = 0;
            g_547624 = GetTickCount() + 120000;
            g_54761c = true;
            result = true;
        }
    }
    return result;
}
#endif



struct s_grs_source;
struct s_packed_grs;
struct s_packed_clc;
void packed_grs_write(s_grs_source const *source, s_packed_grs *packed);
void __stdcall function_b3ed0(s_packed_clc const *data, long kind, char const *name);

// @retail 0xb3e90
void __stdcall function_b3e90(byte *results)
{
    byte packed[0x150bc];
    packed_grs_write((s_grs_source const *)results, (s_packed_grs *)packed);
    function_b3ed0((s_packed_clc const *)packed, sizeof(packed), "application/x-halo2pcr");
}
