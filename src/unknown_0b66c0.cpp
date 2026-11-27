// @flags /O2 /Ob1 /Gr
#include "transaction_storage.h"
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

// `inline` (M3 lane 3 r313, lane 14 r134; #1605): retail's callers 0x106790, 0x166d75, 0x16760c, 0x1cd070,
// 0x1cd8a0 and 0x1cdb00 keep nothing in ECX/EDX across its calls and 0x106790 expands it at its channel-0 tests, so
// retail's callers had no register summary for it.
// @retail 0x000b6760
inline bool function_0b6760(const s_index_pair *pair)
{
	return pair->index != NONE && pair->index2 != NONE;
}

// @retail 0x000b6780
bool function_0b6780(const s_index_triple *triple)
{
	return triple->index3 != NONE && triple->index != NONE && triple->index2 != NONE;
}

__declspec(noinline) short function_0b67a0(const s_small_index *data);

// @retail 0x000b67a0
inline short function_0b67a0(const s_small_index *data)
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
    char const *get_chars() const { return bytes; }
    long find(long start, char const *substring) const;
    bool function_b6220(long start, long count, char *destination) const;
};

// @retail 0xb61d0
long s_text512_ab::find(long start, char const *substring) const
{
    char const *string = get_chars();
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

static __forceinline char *upload_copy_ab(char *destination, char const *source, long size)
{
	strncpy(destination, source, size);
	destination[size - 1] = 0;
	return destination;
}

// Matched as a const member of the text record (as find 0xb61d0 is), with 0xb5f10 declaring its buffer
// as that record. The text is a plain `this` (no volatile): it stays a stack argument reloaded at the scan and at the
// sum. The clamp is written as one ternary; as an `if` after `count++` the text stays in a register. The member call
// evaluates the text after start, which is the second call's `lea ecx, [esi + 4]` before `push edx`, while the add still
// lands in start's EDI. As a free function, (start, string, ...) matched here but cost 0xb5f10 2; (string, start, ...)
// summed into EAX.
// @retail 0xb6220
bool s_text512_ab::function_b6220(long start, long count, char *destination) const
{
	char const *string = get_chars();
	bool result = false;
	if (start >= 0 && count > 0)
	{
		long length = (long)string_length_512_ab(string);
		if (start + count <= length)
		{
			count = count + 1 > 0x200 ? 0x200 : count + 1;
			start += (long)string;
			upload_copy_ab(destination, (char const *)start, count);
			result = true;
		}
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



// @retail 0xb62b0
s_upload_ab *function_b62b0(s_upload_ab *upload)
{
    char *content_type = upload->content_type;
    char *filename = upload->filename;
    char volatile &path = upload->path[0];
    path = 0;
    char volatile &header = upload->header[0];
    header = 0;
    char volatile &part = upload->part[0];
    part = 0;
    char volatile &ending = upload->ending[0];
    ending = 0;
    char volatile &type_prefix = content_type[0];
    type_prefix = 0;
    char volatile &filename_prefix = filename[0];
    filename_prefix = 0;
    char volatile &extra_prefix = upload->extra_headers[0];
    extra_prefix = 0;
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
    s_text512_ab buffer;
    char length_text[0x200];
    char body[0x200];
    bool result = false;
    upload_copy_ab(buffer.bytes, response, sizeof(buffer));
    long begin = buffer.find(0, g_4530e8);
    *complete = false;
    if (begin != NONE)
    {
        begin += strlen(g_4530e8);
        long end = buffer.find(begin, "\r\n");
        if (end != NONE)
        {
            length_text[0] = 0;
            if (buffer.function_b6220(begin, end - begin, length_text))
            {
                body[0] = 0;
                long size = atoi(length_text);
                if (buffer.function_b6220(end + 4, size, body))
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
    s_http_connection_ab();
    bool connect(long address, word port, char const *path, bool flag);
    bool receive(bool *complete);
    bool send();
};

bool function_b51b0(s_transport_endpoint *endpoint, s_type_99af70 const *address);

bool function_7af40(s_type_99af70 const *address);

/* points the connection at an IPv4 address and port and opens its endpoint;
   on success counts the attempt and keeps the request path */
// The endpoint local for the three clearing stores beside `connection->endpoint` for the flag and the
// open call, and the open result tested twice, are retail's code shape: kept only with the byte match.
// @retail 0xb5d40
bool s_http_connection_ab::connect(long address, word port, char const *path, bool flag)
{
    s_http_connection_ab *connection = this;
    connection->address.address_length = 4;
    connection->address.ipv4_address = htonl(address);
    connection->address.port = htons(port);
    connection->hostname[0] = 0;
    function_b6290(connection->hostname, "%d.%d.%d.%d", (byte)address, (byte)(address >> 8), (byte)(address >> 16), (byte)(address >> 24));
    if (function_7af40(&connection->address))
    {
        s_transport_endpoint *endpoint = connection->endpoint;
        endpoint->socket = NONE;
        endpoint->flags = 0;
        endpoint->type = 4;
        if (flag) connection->endpoint->flags |= 0x40;
        bool opened = function_b51b0(connection->endpoint, &connection->address);
        if (opened)
            connection->attempt_count++;
        else
            transport_endpoint_close(connection->endpoint);
        if (opened)
        {
            connection->state = 1;
            upload_copy_ab(connection->path, path, sizeof(connection->path));
            return true;
        }
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

void transport_register_transition_functions(transport_transition_function startup, transport_transition_function shutdown, transport_transition_function reset, void *context);

/* an idle connection, with its close (function_b6180) registered as a transport shutdown
   function; retail expands transport_register_transition_functions (src/transport.cpp) here.
   Retail's only caller is the dynamic initializer of the upload connection g_546a8c
   (src/unknown_0b3d30.cpp), 0x376bf0, which passes the connection in edi */
// @retail 0xb5cd0
s_http_connection_ab::s_http_connection_ab()
{
    hostname[0] = 0;
    function_b62b0((s_upload_ab *)((byte *)this + 0x30));
    memset(&internal_endpoint, 0, sizeof(internal_endpoint));
    endpoint = &internal_endpoint;
    state = 0;
    transport_register_transition_functions(0, (transport_transition_function)function_b6180, 0, this);
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

bool function_b6320(s_upload_ab *upload, byte *buffer, long maximum, long *written);

short function_b5000(s_transport_endpoint *endpoint, void const *buffer, short length);

/* sends the upload a buffer at a time until everything is out (then waits for the response) or the
   endpoint would block. A would-block leaves the loop with the result still false: the test after the
   loop tells it from completion and rewinds the position past the buffer that was not sent. */
// @retail 0xb6000
bool s_http_connection_ab::send()
{
    bool result;
    byte buffer[0x518];
    long position;
    do
    {
        position = upload_position;
        long written = 0;
        result = false;
        if (!function_b6320((s_upload_ab *)((byte *)this + 0x30), buffer, sizeof(buffer), &written))
            goto return_exit;
        short sent;
        if (!written)
            sent = 0;
        else
            sent = function_b5000(endpoint, buffer, (short)written);
        if (sent < 0)
        {
            if (sent != -2)
                goto return_exit;
            break;
        }
        result = true;
        if (sent < written)
            upload_position = position + sent;
    } while (upload_position != upload_file_length + upload_ending_length + upload_part_length + upload_header_length);
    if (result)
    {
        response_length = 0;
        state = 3;
    }
    else
    {
        upload_position = position;
        result = true;
    }

return_exit:
    return result;
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
