// @flags /O2 /Ob1 /Gr
/* Asynchronous online results, their address registration, and diagnostic text. */
#include "unknown_11c920.h"
#include "globals.h"
#include "online_tasks.h"
#include <xtl.h>
#include <stdarg.h>
#include <stdio.h>

/* API identities verified against retail library signatures; argument layouts
   follow the retail calls. These declarations require no private SDK types. */
extern "C" long __stdcall XOnlineQuerySearch(dword procedure, dword query,
    dword flags, dword maximum_results, dword attribute_count,
    const void *attributes, dword parameter_count, const void *parameters,
    void *event, void **task);
extern "C" long __stdcall XOnlineQuerySearchGetResults(void *task,
    dword *returned_size, dword *count, dword *buffer_size, void *buffer);

/* Identical to the existing declaration in unknown_06b590.cpp. */
struct s_online_service
{
    DWORD service_id;
    long unknown04;
    bool connect;
};
extern s_online_service g_467178[13];

#pragma pack(push, 2)
struct s_online_cached_result
{
    unsigned __int64 entity;
    word unknown08;
    XNADDR address;
    word unknown2e;
    XNKID key_id;
    word unknown38;
    XNKEY key;
};
#pragma pack(pop)
typedef char online_cached_result_size[sizeof(s_online_cached_result) == 0x4a ? 1 : -1];

bool g_510580;
long g_510584;
s_online_cached_result g_510588[8];
dword g_5107d8;
long g_5107dc;
IN_ADDR g_5107e0;
long g_5107e4;

long online_result_query_start(void);
void online_result_registration_clear(void);
void online_result_query_finish(s_type_9df9da *task);
bool online_result_register(s_online_cached_result *record, IN_ADDR *address);
void online_result_shuffle(dword count, s_online_cached_result *records);
void online_result_describe(char *text, const s_online_cached_result *record);
char *online_result_format(char *text, const char *format, ...);
char *online_result_append(char *text, const char *format, ...);
dword online_result_text_length(const char *text);

// @retail 0xb36c0
bool online_result_service_connected(void)
{
    if (g_467214 != NONE && online_task_get_logon_status(g_467214) == 1)
    {
        bool connected = false;
        for (long i = 0; i < 13; i++)
            if (g_467178[i].service_id == 0x4d530064)
                connected = g_467178[i].connect;
        if (connected)
            return true;
    }
    return false;
}

// @retail 0xb3700
void online_result_cache_start(void)
{
    g_510584 = online_result_query_start();
    online_result_registration_clear();
    g_510580 = false;
}

// @retail 0xb3720
void online_result_cache_update(void)
{
    s_type_9df9da *task = online_task_try_get(g_510584);
    if (task && (task->flags & 4))
        online_result_query_finish(task);
}

// @retail 0xb3770
bool online_result_get_address(long index, IN_ADDR *address)
{
    bool result = false;
    if (g_5107dc == index)
    {
        *address = g_5107e0;
        return true;
    }
    IN_ADDR converted = { 0 };
    online_result_registration_clear();
    if (online_result_register(&g_510588[index], &converted))
    {
        *address = converted;
        g_5107dc = index;
        g_5107e0 = *address;
        result = true;
    }
    return result;
}

// @retail 0xb37f0
long online_result_query_start(void)
{
    long index = online_task_new_if_logged_on();
    s_type_9df9da *task = online_task_try_get(index);
    if (task)
    {
        dword attributes[8] = { 0, 0, 0x80200001, 0x24,
            0x80200002, 8, 0x80200003, 0x10 };
        if (XOnlineQuerySearch(0xaaaa, 1, 0, 8, 4, attributes,
            0, NULL, NULL, &task->handle) >= 0)
        {
            task->flags = 1;
            task->type = 0x28;
            task->controller_index = NONE;
        }
        else
        {
            function_6b640(index);
            return NONE;
        }
    }
    return index;
}

// @retail 0xb38d0
void online_result_shuffle(dword count, s_online_cached_result *records)
{
    for (dword i = 0; i < count - 1; i++)
    {
        dword seed = g_4e7408->seed * 1664525 + 1013904223;
        g_4e7408->seed = seed;
        short chosen = (short)i + (short)(((seed >> 16) *
            ((short)count - (short)i)) >> 16);
        s_online_cached_result temporary = records[i];
        records[i] = records[chosen];
        records[chosen] = temporary;
    }
}

// @retail 0xb3970
void online_result_query_finish(s_type_9df9da *task)
{
    dword returned_size = 0;
    dword buffer_size = sizeof(g_510588);
    char text[4096];
    long result;
    if (online_result_service_connected())
        result = XOnlineQuerySearchGetResults(task->handle, &returned_size,
            &g_5107d8, &buffer_size, g_510588);
    else
        result = (long)0x80004005;
    function_6b640(g_510584);
    g_510584 = NONE;
    if (result >= 0 && g_5107d8 > 0)
    {
        text[0] = 0;
        online_result_format(text, "online:stats: XOnlineQuerySearchGetResults succeeded with %d servers: ", g_5107d8);
        for (dword i = 0; i < g_5107d8; i++)
        {
            online_result_append(text, "|Server %d)  ", i);
            online_result_describe(text, &g_510588[i]);
            online_result_append(text, "|  ");
        }
        g_510580 = true;
        online_result_shuffle(g_5107d8, g_510588);
    }
}

// @retail 0xb3a70
bool online_result_register(s_online_cached_result *record, IN_ADDR *address)
{
    bool result = false;
    char text[4096];
    if (XNetRegisterKey(&record->key_id, &record->key) == 0)
    {
        g_5107e4++;
        if (XNetTsAddrToInAddr((TSADDR *)&record->address, 0x4d530064,
            &record->key_id, address) == 0)
            return true;
        text[0] = 0;
        if (g_5107e0.s_addr)
        {
            XNetConnect(g_5107e0);
            g_5107e0.s_addr = 0;
        }
        XNetUnregisterKey(&record->key_id);
        g_5107e4--;
        online_result_format(text, "XNetTsAddrToInAddr() failed in register_ip_and_key() with error code %s.  ", 0);
        online_result_describe(text, record);
    }
    return result;
}

// @retail 0xb3b10
void online_result_registration_clear(void)
{
    if (g_5107dc != NONE)
    {
        s_online_cached_result *record = &g_510588[g_5107dc];
        if (g_5107e0.s_addr)
        {
            XNetConnect(g_5107e0);
            g_5107e0.s_addr = 0;
        }
        XNetUnregisterKey(&record->key_id);
        g_5107e4--;
        g_5107dc = NONE;
        g_5107e0.s_addr = 0;
    }
}

// @retail 0xb3b70
void online_result_describe(char *text, const s_online_cached_result *record)
{
    online_result_append(text, "SessionID: ");
    for (dword i = 0; i < 8; i++)
        online_result_append(text, "%02X", record->key_id.ab[i]);
    online_result_append(text, ".  TsAddr: ina %08X, inaOnline %08X, wPortOnline %08X, abEnet ",
        record->address.ina.s_addr, record->address.inaOnline.s_addr, record->address.wPortOnline);
    for (dword j = 0; j < 6; j++)
        online_result_append(text, "%02X", record->address.abEnet[j]);
    online_result_append(text, ", abOnline ");
    for (dword k = 0; k < 20; k++)
        online_result_append(text, "%02X", record->address.abOnline[k]);
    online_result_append(text, ".  KEK: ");
    for (dword l = 0; l < 16; l++)
        online_result_append(text, "%02X", record->key.ab[l]);
    online_result_append(text, ".  EntitiyID: ");
    for (dword m = 0; m < 8; m++)
        online_result_append(text, "%02X", ((const byte *)&record->entity)[m]);
    online_result_append(text, ".");
}

// @retail 0xb3c80
char *online_result_format(char *text, const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    _vsnprintf(text, 4095, format, arguments);
    text[4095] = 0;
    return text;
}

// @retail 0xb3cb0
char *online_result_append(char *text, const char *format, ...)
{
    dword length = online_result_text_length(text);
    dword remaining = 4096 - length;
    char *end = text + length;
    va_list arguments;
    va_start(arguments, format);
    _vsnprintf(end, remaining - 1, format, arguments);
    end[remaining - 1] = 0;
    return text;
}

// @retail 0xb3cf0
dword online_result_text_length(const char *text)
{
    dword length = 0;
    for (; length < 4095; length++)
        if (!*text++)
            break;
    return length;
}
