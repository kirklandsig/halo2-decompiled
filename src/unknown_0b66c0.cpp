// @flags /O2 /Ob1 /Gr
#include "unknown_11c920.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <xtl.h>
#include <stdlib.h>
#include "unknown_0b4da0.h"

struct s_index_pair
{
	long index;
	byte unknown04[2];
	short index2;
};

struct s_index_triple
{
	long index;
	byte unknown04[2];
	short index2;
	byte unknown08[0x60];
	long index3;
};

struct s_small_index
{
	byte unknown00[0x18];
	signed char index;
	byte unknown19[0x5b];
	long count;
};

// @retail 0x000b66c0
char *csprintf_1024(char *buffer, const char *format, ...)
{
	va_list arguments;
	va_start(arguments, format);
	_vsnprintf(buffer, 0x3ff, format, arguments);
	buffer[0x3ff] = 0;
	return buffer;
}

// @retail 0x000b66f0
char *csprintf_256(char *buffer, const char *format, ...)
{
	va_list arguments;
	va_start(arguments, format);
	_vsnprintf(buffer, 0xff, format, arguments);
	buffer[0xff] = 0;
	return buffer;
}

// @retail 0x000b6720
unsigned long function_0b6720(const char *string)
{
	unsigned long length = 0;
	for (; length < 0xff; length++)
	{
		if (!*string++)
			break;
	}
	return length;
}

// @retail 0x000b6760
bool function_0b6760(const s_index_pair *pair)
{
	return pair->index != NONE && pair->index2 != NONE;
}

// @retail 0x000b6780
bool function_0b6780(const s_index_triple *triple)
{
	return triple->index3 != NONE && triple->index != NONE && triple->index2 != NONE;
}

// @retail 0x000b67a0
short function_0b67a0(const s_small_index *data)
{
	signed char index = data->index;
	if (index >= 0 && index < data->count)
		return index;
	return NONE;
}

// @retail 0xb6190
byte function_b6190(char const *a, char const *b)
{
	return strcmp(a, b) == 0;
}

static __forceinline unsigned long string_length_512_ab(char const *string)
{
	unsigned long length = 0;
	for (; length < 0x1ff; length++)
	{
		if (!*string++)
			break;
	}
	return length;
}

struct s_text512_ab
{
    char bytes[0x200];
    long find(long start, char const *substring) const;
};

// @retail 0xb61d0
long s_text512_ab::find(long start, char const *substring) const
{
    char const *string = bytes;
	long result = NONE;
	if (start < (long)string_length_512_ab(string))
	{
		start += (long)string;
		char const *found = strstr((char const *)start, substring);
		if (found)
			result = found - string;
	}
	return result;
}

// @retail 0xb6220
bool function_b6220(char const *volatile string, long start, long count, char *destination)
{
	bool result = false;
	if (start >= 0 && count > 0 && start + count <= (long)string_length_512_ab(string))
	{
		count++;
		if (count > 0x200)
			count = 0x200;
		start += (long)string;
		strncpy(destination, (char const *)start, count);
		destination[count - 1] = 0;
		result = true;
	}
	return result;
}

// @retail 0xb6290
char *function_b6290(char *buffer, char const *format, ...)
{
	va_list arguments;
	va_start(arguments, format);
	_vsnprintf(buffer, 0xf, format, arguments);
	buffer[0xf] = 0;
	return buffer;
}


struct s_upload_file_ab
{
    byte unknown00[0x108];
    HANDLE handle;
    dword position;
};

struct s_upload_ab
{
    long kind;
    char path[0x80];
    char header[0x400];
    char part[0x100];
    char ending[0x80];
    long header_length;
    long part_length;
    long ending_length;
    char content_type[0x20];
    char filename[0x104];
    char extra_headers[0x200];
    byte const *memory;
    s_upload_file_ab *file;
    long file_length;
    long position;
};

static __forceinline char *upload_copy_ab(char *destination, char const *source, long size)
{
    strncpy(destination, source, size);
    destination[size - 1] = 0;
    return destination;
}

// @retail 0xb62b0
s_upload_ab *function_b62b0(s_upload_ab *upload)
{
    char *content_type = upload->content_type;
    char *filename = upload->filename;
    *(volatile char *)upload->path = 0;
    *(volatile char *)upload->header = 0;
    *(volatile char *)upload->part = 0;
    *(volatile char *)upload->ending = 0;
    *(volatile char *)content_type = 0;
    *(volatile char *)filename = 0;
    *(volatile char *)upload->extra_headers = 0;
    upload->kind = 0;
    upload_copy_ab(content_type, "text/plain", sizeof(upload->content_type));
    upload_copy_ab(filename, "blob", sizeof(upload->filename));
    return upload;
}

static __forceinline bool upload_seek_ab(s_upload_file_ab *file)
{
    file->position = SetFilePointer(file->handle, 0, 0, FILE_BEGIN);
    bool result = file->position != 0xffffffff;
    if (!result)
    {
        GetLastError();
        SetLastError(0);
    }
    return result;
}

// @retail 0xb6570
bool function_b6570(s_upload_ab *upload)
{
    bool result = true;
    if (upload->kind == 2)
    {
        s_upload_file_ab *file = upload->file;
        if (file->position)
        {
            if (!upload_seek_ab(file))
                result = false;
        }
    }
    upload->position = 0;
    return result;
}

// @retail 0xb65f0
void function_b65f0(s_upload_ab *upload)
{
    upload_copy_ab(upload->ending, "\r\n--BUNGIEr0x0rz--\r\n", sizeof(upload->ending));
    unsigned long length = 0;
    char const *string = upload->ending;
    for (; length < 0x7f; length++)
        if (!*string++) break;
    upload->ending_length = length;
    csprintf_256(upload->part, "--BUNGIEr0x0rz\r\nContent-Disposition: form-data; name=\"upload\"; filename=\"%s\"\r\nContent-Type: %s\r\n\r\n", upload->filename, upload->content_type);
    upload->part_length = function_0b6720(upload->part);
    csprintf_1024(upload->header, "POST %s HTTP/1.0\r\nContent-type: multipart/form-data; boundary=BUNGIEr0x0rz\r\nContent-Length: %d\r\n%s\r\n", upload->path, upload->part_length + upload->ending_length + upload->file_length, upload->extra_headers);
    length = 0;
    string = upload->header;
    for (; length < 0x3ff; length++)
        if (!*string++) break;
    upload->header_length = length;
}


static char g_4530e8[] = "Content-Length: ";

// @retail 0xb5f10
bool function_b5f10(char const *response, bool *complete)
{
    char buffer[0x200];
    char length_text[0x200];
    char body[0x200];
    bool result = false;
    upload_copy_ab(buffer, response, sizeof(buffer));
    long begin = ((s_text512_ab const *)buffer)->find(0, g_4530e8);
    *complete = false;
    if (begin != NONE)
    {
        begin += strlen(g_4530e8);
        long end = ((s_text512_ab const *)buffer)->find(begin, "\r\n");
        if (end != NONE)
        {
            length_text[0] = 0;
            if (function_b6220(buffer, begin, end - begin, length_text))
            {
                body[0] = 0;
                long size = atoi(length_text);
                if (function_b6220(buffer, end + 4, size, body))
                {
                    result = true;
                    *(byte *)complete = function_b6190(body, "DONE");
                }
            }
        }
    }
    return result;
}

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

bool function_b51b0(s_transport_endpoint *endpoint, s_type_99af70 const *address);

static __forceinline bool http_address_valid_ab(s_type_99af70 const *address)
{
    long length = address->address_length;
    if (length == NONE || length == 4)
        return address->ipv4_address != 0;
    if (length == 16)
        for (long i = 0; i < 8; i++)
            if (address->ipv6_address[i]) return true;
    return false;
}

// @retail 0xb5d40
bool s_http_connection_ab::connect(long address, word port, char const *path, bool flag)
{
    s_http_connection_ab *connection = this;
    connection->address.address_length = 4;
    connection->address.ipv4_address = htonl(address);
    connection->address.port = htons(port);
    connection->hostname[0] = 0;
    function_b6290(connection->hostname, "%d.%d.%d.%d", (byte)address, (byte)(address >> 8), (byte)(address >> 16), (byte)(address >> 24));
    if (http_address_valid_ab(&connection->address))
    {
        s_transport_endpoint *endpoint = connection->endpoint;
        endpoint->socket = NONE;
        endpoint->flags = 0;
        endpoint->type = 4;
        if (flag) connection->endpoint->flags |= 0x40;
        if (function_b51b0(connection->endpoint, &connection->address))
        {
            connection->attempt_count++;
            connection->state = 1;
            upload_copy_ab(connection->path, path, sizeof(connection->path));
            return true;
        }
        transport_endpoint_close(connection->endpoint);
    }
    return false;
}

// @retail 0xb5e40
bool __stdcall function_b5e40(void *block)
{
    s_http_connection_ab *connection = (s_http_connection_ab *)block;
    bool result = true;
    if (connection->state)
    {
        if (!function_b6570((s_upload_ab *)((byte *)connection + 0x30)))
            result = false;
        transport_endpoint_close(connection->endpoint);
        connection->attempt_count--;
        connection->state = 0;
    }
    return result;
}

// @retail 0xb6180
bool __stdcall function_b6180(void *block)
{
    return function_b5e40(block);
}

struct s_http_callbacks_ab
{
    long count;
    long values[8];
    bool (__stdcall *callbacks[8])(void *);
    long stages[8];
    void *blocks[8];
};

s_http_callbacks_ab g_4d8b1c;

// @retail 0xb5cd0
s_http_connection_ab *function_b5cd0(s_http_connection_ab *connection)
{
    *(volatile char *)connection->hostname = 0;
    function_b62b0((s_upload_ab *)((byte *)connection + 0x30));
    s_transport_endpoint *endpoint = &connection->internal_endpoint;
    memset(endpoint, 0, sizeof(*endpoint));
    connection->endpoint = endpoint;
    connection->state = 0;
    g_4d8b1c.values[g_4d8b1c.count] = 0;
    g_4d8b1c.callbacks[g_4d8b1c.count] = function_b6180;
    g_4d8b1c.stages[g_4d8b1c.count] = 0;
    g_4d8b1c.blocks[g_4d8b1c.count] = connection;
    g_4d8b1c.count++;
    return connection;
}

short function_b4fa0(s_transport_endpoint *endpoint, void *buffer, short length);

// @retail 0xb60e0
bool s_http_connection_ab::receive(bool *complete)
{
    bool result = false;
    char *buffer = response + response_length;
    *complete = false;
    long received = function_b4fa0(endpoint, buffer, (short)(0x1ff - response_length));
    if (received > 0)
    {
        response_length += received;
        response[(dword)response_length] = 0;
        bool done = false;
        if (function_b5f10(response, &done))
        {
            if (done)
            {
                result = true;
                *complete = result;
            }
        }
        else if (response_length < 0x1ff)
            result = true;
    }
    else if (received == -2)
        result = true;
    return result;
}

static inline bool upload_read_ab(s_upload_file_ab *file, long position, void *buffer, long count)
{
    if (file->position != position)
    {
        file->position = SetFilePointer(file->handle, position, 0, FILE_BEGIN);
        if (file->position == 0xffffffff)
            return false;
    }
    dword received;
    bool result = false;
    if (ReadFile(file->handle, buffer, count, &received, 0))
    {
        if (received == count)
            result = true;
        else
            SetLastError(0x26);
    }
    file->position += received;
    return result;
}

// @retail 0xb6320
bool function_b6320(s_upload_ab *upload, byte *buffer, long maximum, long *written)
{
    byte *out = buffer;
    long remaining = maximum;
    volatile bool result = false;
    bool valid = upload->kind == 2 || upload->kind == 1;
    unsigned long length = 0;
    char const *path = upload->path;
    for (; length < 0x7f; ++length)
        if (!*path++) break;
    if (length && valid)
    {
        if (!upload->position)
            function_b65f0(upload);
        result = true;
        while (upload->position != upload->ending_length + upload->file_length + upload->part_length + upload->header_length && remaining > 0)
        {
            long position = upload->position;
            long count;
            if (position < upload->header_length)
            {
                count = remaining < upload->header_length - position ? remaining : upload->header_length - position;
                memcpy(out, upload->header + position, count);
            }
            else if ((position -= upload->header_length) < upload->part_length)
            {
                count = remaining < upload->part_length - position ? remaining : upload->part_length - position;
                memcpy(out, upload->part + position, count);
            }
            else if ((position -= upload->part_length) < upload->file_length)
            {
                count = remaining < upload->file_length - position ? remaining : upload->file_length - position;
                if (upload->kind == 1)
                    memcpy(out, upload->memory + position, count);
                else if (!upload_read_ab(upload->file, position, out, count))
                {
                    GetLastError();
                    SetLastError(0);
                    result = false;
                }
            }
            else if ((position -= upload->file_length) < upload->ending_length)
            {
                count = remaining < upload->ending_length - position ? remaining : upload->ending_length - position;
                memcpy(out, upload->ending + position, count);
            }
            else
            {
                result = false;
                break;
            }
            upload->position += count;
            out += count;
            remaining = maximum - (out - buffer);
        }
        *written = out - buffer;
    }
    return result;
}

short function_b5000(s_transport_endpoint *endpoint, void const *buffer, short length);

// @retail 0xb6000
bool s_http_connection_ab::send()
{
    bool result_value = 0;
    byte buffer[0x518];
    for (;;)
    {
        long position = upload_position;
        long written = 0;
        bool result = false;
        if (!function_b6320((s_upload_ab *)((byte *)this + 0x30), buffer, sizeof(buffer), &written))
            { result_value = result; goto return_exit; }
        short sent = written ? function_b5000(endpoint, buffer, (short)written) : 0;
        if (sent < 0)
        {
            if (sent == -2)
            {
                upload_position = position;
                { result_value = true; goto return_exit; }
            }
            { result_value = result; goto return_exit; }
        }
        if (sent < written)
            upload_position = position + sent;
        if (upload_position == upload_file_length + upload_ending_length + upload_part_length + upload_header_length)
            break;
    }
    response_length = 0;
    state = 3;
    { result_value = true; goto return_exit; }

return_exit:
    return result_value;
}

bool transport_endpoint_test_connection(s_transport_endpoint *endpoint, bool *connected);

// @retail 0xb5e90
bool function_b5e90(s_http_connection_ab *connection, bool *complete)
{
    bool result = false;
    *complete = false;
    if (connection->state == 1)
    {
        bool connected = false;
        if (transport_endpoint_test_connection(connection->endpoint, &connected))
        {
            result = true;
            if (connected)
                connection->state = 2;
        }
    }
    else if (connection->state == 2)
    {
        if (connection->send())
            result = true;
    }
    else if (connection->state == 3)
    {
        if (connection->receive(complete))
            result = true;
    }
    if (!result || *complete)
        function_b5e40(connection);
    return result;
}
