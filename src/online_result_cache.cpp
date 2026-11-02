// @flags /O2 /Ob1 /Gr
/* Asynchronous online results, their address registration, and diagnostic text. */
#include "unknown_11c920.h"
#include "globals.h"
#include "online_tasks.h"
#include "online_result_cache.h"
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

/* A diagnostic text buffer, filled by the format and append helpers. */
struct s_online_result_text
{
    char buffer[4096];

    __forceinline void clear(void) { buffer[0] = 0; }
    __forceinline char *text_start(void) { return buffer; }
};

bool g_510580;
long g_510584;
s_online_result_list g_510588;

long online_result_query_start(void);
void online_result_registration_clear(void);
void online_result_query_finish(s_type_9df9da *task);
bool online_result_register(s_online_cached_result *record, IN_ADDR *address);
void online_result_shuffle(dword count, s_online_cached_result *records);
void online_result_describe(s_online_result_text *text, const s_online_cached_result *record);
long sound_random_range(short lower, short upper);
char *online_result_format(char *text, const char *format, ...);
char *online_result_append(char *text, const char *format, ...);
dword online_result_text_length(const char *text);

/* the logon test is written out: calling the inline online_logon_connected()
   compiles to the same code, but the extra inline expansion moves LTCG's
   whole-program inlining enough that 0x73360 stops matching */
// @retail 0xb36c0
bool online_result_service_connected(void)
{
    bool logon_succeeded = false;
    bool connected;
    if (g_467214 != NONE)
    {
        switch (online_task_get_logon_status(g_467214))
        {
        case 1:
            logon_succeeded = true;
            break;
        }
    }
    if (logon_succeeded)
    {
        connected = false;
        for (long i = 0; i < 13; i++)
            if (g_467178[i].service_id == 0x4d530064)
                connected = g_467178[i].connect;
    }
    return logon_succeeded && connected;
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
    if (g_510588.registered_index == index)
    {
        *address = g_510588.registered_address;
        result = true;
    }
    else
    {
        IN_ADDR converted = { 0 };
        online_result_registration_clear();
        if (online_result_register(&g_510588.results[index], &converted))
        {
            *address = converted;
            g_510588.registered_index = index;
            g_510588.registered_address = *address;
            result = true;
        }
    }
    return result;
}

// @retail 0xb37f0
long online_result_query_start(void)
{
    long index = online_task_new_if_logged_on();
    if (index != NONE)
    {
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
                index = NONE;
            }
        }
    }
    return index;
}

// @retail 0xb38d0
void online_result_shuffle(dword count, s_online_cached_result *records)
{
    for (dword i = 0; i < count - 1; i++)
    {
        long chosen = sound_random_range((short)i, (short)count);
        s_online_cached_result temporary = records[i];
        records[i] = records[chosen];
        records[chosen] = temporary;
    }
}

// @retail 0xb3970
void online_result_query_finish(s_type_9df9da *task)
{
    dword returned_size = 0;
    dword buffer_size = sizeof(g_510588.results);
    s_online_result_text text;
    long result;
    if (online_result_service_connected())
        result = XOnlineQuerySearchGetResults(task->handle, &returned_size,
            &g_510588.count, &buffer_size, g_510588.results);
    else
        result = (long)0x80004005;
    function_6b640(g_510584);
    g_510584 = NONE;
    if (result >= 0 && g_510588.count > 0)
    {
        text.clear();
        online_result_format(text.text_start(), "online:stats: XOnlineQuerySearchGetResults succeeded with %d servers: ", g_510588.count);
        for (dword i = 0; i < g_510588.count; i++)
        {
            online_result_append(text.text_start(), "|Server %d)  ", i);
            online_result_describe(&text, &g_510588.results[i]);
            online_result_append(text.text_start(), "|  ");
        }
        g_510580 = true;
        online_result_shuffle(g_510588.count, g_510588.results);
    }
}

// @retail 0xb3a70
bool online_result_register(s_online_cached_result *record, IN_ADDR *address)
{
    bool result = false;
    s_online_result_text text;
    if (XNetRegisterKey(&record->key_id, &record->key) == 0)
    {
        g_510588.key_count++;
        if (XNetTsAddrToInAddr((TSADDR *)&record->address, 0x4d530064,
            &record->key_id, address) == 0)
        {
            result = true;
        }
        else
        {
            text.clear();
            if (g_510588.registered_address.s_addr)
            {
                XNetConnect(g_510588.registered_address);
                g_510588.registered_address.s_addr = 0;
            }
            XNetUnregisterKey(&record->key_id);
            g_510588.key_count--;
            online_result_format(text.text_start(), "XNetTsAddrToInAddr() failed in register_ip_and_key() with error code %s.  ", 0);
            online_result_describe(&text, record);
        }
    }
    return result;
}

// @retail 0xb3b10
void online_result_registration_clear(void)
{
    if (g_510588.registered_index != NONE)
    {
        s_online_cached_result *record = &g_510588.results[g_510588.registered_index];
        if (g_510588.registered_address.s_addr)
        {
            XNetConnect(g_510588.registered_address);
            g_510588.registered_address.s_addr = 0;
        }
        XNetUnregisterKey(&record->key_id);
        g_510588.key_count--;
        g_510588.registered_index = NONE;
        g_510588.registered_address.s_addr = 0;
    }
}

// @retail 0xb3b70
void online_result_describe(s_online_result_text *text, const s_online_cached_result *record)
{
    online_result_append(text->text_start(), "SessionID: ");
    for (dword i = 0; i < 8; i++)
        online_result_append(text->text_start(), "%02X", record->key_id.ab[i]);
    online_result_append(text->text_start(), ".  TsAddr: ina %08X, inaOnline %08X, wPortOnline %08X, abEnet ",
        record->address.ina.s_addr, record->address.inaOnline.s_addr, record->address.wPortOnline);
    for (dword j = 0; j < 6; j++)
        online_result_append(text->text_start(), "%02X", record->address.abEnet[j]);
    online_result_append(text->text_start(), ", abOnline ");
    for (dword k = 0; k < 20; k++)
        online_result_append(text->text_start(), "%02X", record->address.abOnline[k]);
    online_result_append(text->text_start(), ".  KEK: ");
    for (dword l = 0; l < 16; l++)
        online_result_append(text->text_start(), "%02X", record->key.ab[l]);
    online_result_append(text->text_start(), ".  EntitiyID: ");
    for (dword m = 0; m < 8; m++)
        online_result_append(text->text_start(), "%02X", ((const byte *)&record->entity)[m]);
    online_result_append(text->text_start(), ".");
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
