// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_080F30.CPP: the requests that send and receive messages through
   the pending messages (lane D) */

#include "unknown_11c920.h"
#include "pending_messages.h"
#include "crc.h"
#include "physical_memory.h"
#include <xtl.h>
#include <d3d8.h>

bool function_0b49a0(long index, real *result);

// @retail 0x812a0
dword pending_message_payload_crc(const void *data, long size)
{
	dword crc = 0xffffffff;
	function_163ba0(&crc, (const byte *)data + 4, size - 4);
	return crc;
}

struct s_pending_payload_header
{
	dword crc;
	dword version;
	dword size;
	dword unknown0c;
};

struct s_pending_definition
{
	dword unknown00;
	dword unknown04;
	dword version;
	dword unknown0c;
	bool (__stdcall *retry)(s_pending_message_header *header);
	bool (__stdcall *sent)(s_pending_message_header *header);
	bool (__stdcall *received)(s_pending_message_header *header);
};

// @retail 0x81230
long pending_message_request_validate(s_pending_message_header *request, const s_pending_payload_header *payload, dword size)
{
	long result = 1;
	s_pending_definition *definition;
	if (size < sizeof(*payload))
	{
		result = 10;
		goto done;
	}
	if (payload->size != size)
	{
		result = 10;
		goto done;
	}
	if (payload->size != (dword)request->size && request->kind != 4)
	{
		result = 9;
		goto done;
	}
	if (payload->size > (dword)request->size && request->kind == 4)
	{
		result = 9;
		goto done;
	}
	definition = (s_pending_definition *)request->unknown00;
	if (payload->version < definition->version)
	{
		result = 11;
		goto done;
	}
	if (payload->version > definition->version)
	{
		result = 12;
		goto done;
	}
	if (pending_message_payload_crc(payload, size) != payload->crc)
		result = 8;
done:
	return result;
}

/* how far the request's transfer has got */
// @retail 0x80f30
bool pending_message_request_get_progress(s_pending_message_header *header, real *progress)
{
	bool result = false;
	long index = header->pending_index;
	if (index != NONE)
	{
		s_pending_message *message = &g_4d8c28[index];
		if (message->task_index != NONE)
			return function_0b49a0(message->task_index, progress);
		*progress = 0.0f;
		result = true;
	}
	return result;
}

/* queues a request to receive a message */
// @retail 0x80f70
bool pending_message_request_receive(s_pending_message_header *header, long value10, s_pending_message_values values, long value06)
{
	bool result = false;
	if (header->state == 0)
	{
		header->unknown10 = value10;
		header->values = values;
		header->unknown06 = (word)value06;
		if (header->kind != 0 && (header->flags & 2) && !(header->flags & 8))
		{
			result = true;
		}
		else
		{
			header->pending_index = pending_message_add_received(header, header->data, header->size);
			if (header->pending_index != NONE)
			{
				header->state = 1;
				if (header->kind != 4)
					header->flags &= ~2;
				result = true;
			}
		}
	}
	return result;
}

/* queues a request to send a message */
// @retail 0x80ff0
bool pending_message_request_send(s_pending_message_header *header, long value10, s_pending_message_values values, long value06)
{
	bool result = false;
	if (header->state == 0)
	{
		header->unknown10 = value10;
		header->values = values;
		header->unknown06 = (word)value06;
		header->pending_index = pending_message_add(header->size, header, header->data);
		if (header->pending_index != NONE)
		{
			header->state = 2;
			result = true;
		}
	}
	return result;
}

long g_55e700;

static __forceinline void function_81051(word *arg_0, word arg_1, bool arg_2)
{
	if (arg_2)
		*arg_0 |= arg_1;
	else
		*arg_0 &= ~arg_1;
}

// @retail 0x81050
void function_81050(s_pending_message_header *header, long result)
{
	*(short *)&header->unknown0c = header->state;
	bool completed = false;
	((short *)&header->unknown0c)[1] = (short)result;
	header->state = 0;
	header->pending_index = NONE;
	dword old_flags = header->flags;
	if (result == 1)
	{
		completed = true;
		if (*(short *)&header->unknown0c == 2)
			header->flags &= ~20;
		else if (*(short *)&header->unknown0c == 1)
			header->flags = (header->flags & ~28) | 2;
	}
	else if (*(short *)&header->unknown0c == 1 && result == 6)
		header->flags |= 4;
	else if (result == 7 && !(header->unknown06 & 1))
	{
		if (*(short *)&header->unknown0c == 1)
			pending_message_request_receive(header, header->unknown10, header->values, header->unknown06);
		else if (*(short *)&header->unknown0c == 2)
			pending_message_request_send(header, header->unknown10, header->values, header->unknown06);
	}
	s_pending_definition *definition = (s_pending_definition *)header->unknown00;
	if (definition)
	{
		short local_0 = *(short *)&header->unknown0c;
		old_flags |= header->flags & 4;
		bool rejected = false;
		if (local_0 == 2)
		{
			if (definition->sent && !definition->sent(header))
				rejected = true;
		}
		else if (local_0 == 1)
		{
			if (definition->received && !definition->received(header))
				rejected = true;
		}
		if (rejected)
		{
			header->flags = old_flags;
			goto retry;
		}
	}
	if (completed)
		return;
retry:
	if (!(header->unknown06 & 2))
	{
		definition = (s_pending_definition *)header->unknown00;
		bool handled = false;
		if (definition && definition->retry)
		{
			handled = definition->retry(header);
			function_81051(&header->flags, 2, handled);
			function_81051(&header->flags, 8, handled);
		}
		if (!handled)
		{
			if (*(short *)&header->unknown0c == 1)
			{
				g_55e700++;
				pending_message_request_receive(header, header->unknown10, header->values, header->unknown06);
			}
			else if (*(short *)&header->unknown0c == 2)
			{
				g_55e700++;
				pending_message_request_send(header, header->unknown10, header->values, header->unknown06);
			}
			else
				g_55e700++;
		}
	}
}

void __stdcall function_8e0f0(long index, long result);
void function_12d520(long memory);

// @retail 0x812d0
void __stdcall function_812d0(void *allocation, s_pending_message_header *header)
{
 if (header->kind && header->pending_index != NONE)
  function_8e0f0(header->pending_index, 13);
 if (header->data)
 {
  function_12d520((long)header->data);
  header->data = 0;
  header->size = 0;
 }
}

extern s_physical_object *g_4e6464;
long __stdcall function_12d2f0(long size, long user_data, long update, long release);
void function_12c600(void);
double timing_ticks_to_seconds(__int64 ticks);

static inline __int64 pending_read_ticks(void)
{
 volatile __int64 value = 0;
 __asm rdtsc
}

#include "unknown_080e10.h"

struct s_pending_message_storage : s_pending_message_header
{
 long capacity;
};

// score 14 (was 14): the frame is 0x1c against retail's 0x18 and the stack-slot offsets
// differ throughout; retail gives the pump counter and the size the two lowest slots and
// packs the time stamp's high half into the tick counter's slot, ours gives it its own.
// unsigned capacity 14; result after both outputs 85; result after pointer output 85; unsigned
// capacity and late result 85; published field output 14; published field test 14; both
// published field consumers 14; allocation-result flow 45. _ReadWriteBarrier before the
// allocation 13 (retail's store order; the frame stays 0x1c: retail packs start into the
// first counter's slot, ours packs the retry count there); after it 14; before the if 17.
// Inner external inline 14; outer and both external inline 287 each (out of line);
// long, byte and const allocation 14 each; nested guards 14; allocation scope 14; role names 14.
// Round 107: retail gives start's home the first time stamp's slot (as in 0x862e0) and the pump
// counter its own; ours places start's home last. Unchanged layout (14) with: helper local names
// (pumps, start, time stamp) renamed; pumps declared at the top, after start, initialised at the top 105;
// non-volatile time stamp 123 (stores dropped); start as a helper parameter, last or middle 14/14;
// start assigned after its declaration, unsigned start, unsigned time stamp, named second reading 14
// each; capacity read from the request at each use 14. The unused &capacity pointer was dropped
// (identical bytes).
// Direct capacity field reads 14; storage-first guard 40; full-width unsigned result 92;
// optimistic result with failure arms 98; request/allocation shared exits 14/14;
// native int capacity 14; direct capacity with full-width result 92.
// Round 89 (frame probe): ours packs the first time stamp with pumps and gives start's spilled high half
// its own slot; retail packs start with the time stamp (pumps alone). Renames, declaration moves, clamp
// spellings and start as a parameter keep this layout; it changes only when start is fully spilled.
// Named now in the helper 14; elapsed -= start 94 (0x8c790 67); result declared first 14.
// Round 60 (header helper, checked on all its users): the helper's result declared before start 15
// (both pushes then precede the time stamp stores as in retail, but `xor esi, esi` moves between
// them; 0x6a8a0 2 -> 12, 0x18f260 4 -> 75); pumps declared at the helper's top 14; the page count
// read without a local 14 (0x1241b0 lost, 0x862e0 148); const start 14; a named second reading 14.
// A one-link harness of this file shows the same 0x1c frame for start passed in as the helper's
// first parameter, with or without pumps at the top, and for joined size/page tests.
// @retail 0x80e10
bool function_80e10(s_pending_message_storage *request, void **output, long *size)
{
 long capacity = request->capacity;
 bool result = false;
 if ((dword)capacity >= 16 && !request->data)
 {
  void *allocation = (void *)function_80e14(capacity, (long)request, 0, (long)function_812d0, 90, 1.0f);
  request->data = allocation;
  if (allocation)
  {
   request->size = request->capacity;
   result = true;
   if (output)
    *output = allocation;
   if (size)
    *size = request->capacity;
  }
 }
 return result;
}
