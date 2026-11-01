#include "unknown_4cf790.h"
// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_07A9A0.CPP: the transport layer's local address cache, address
   resolution against the XNet key registry, and the QoS handle pool */

#include "unknown_11c920.h"
#include "data_array.h"
#include "globals.h"
#include "bitstream.h"
#include "unknown_07aec0.h"
#include "unknown_1946f0.h"
#include "unknown_07f720.h"
#include <xtl.h>
#include <string.h>

struct s_xnet_registry_entry
{
	bool valid;
	byte unknown1[7];
	XNKID kid;
	XNKEY key;
};

/* 0x4cf791: the cached title address and the flag for its state */







/* 0x46725c: points at the eight zero bytes of g_440070 (globals.h) */
const byte *g_46725c = g_440070;

void qos_release(long handle);
void transport_get_random_bytes(long count, byte *buffer);

/* 0x468758: the allocator the QoS pool and the online tasks
   (online_tasks.cpp) are built through. Retail's .data holds 0x479860, an
   allocator object not modelled yet, so it starts zeroed here like g_46875c. */
c_data_allocator *g_468758;

static __inline dword byte_swap_long(dword v)
{
	return (((v & 0xff0000) | (v >> 16)) >> 8) | (((v << 16) | (v & 0xff00)) << 8);
}

// @retail 0x7a9b0
bool function_07a9b0(void)
{
	if (!g_4cf791)
	{
		XNADDR xna;
		dword flags;

		memset(&g_4cf793, 0, sizeof(g_4cf793));
		memset(&g_4cf7b8, 0, sizeof(g_4cf7b8));

		flags = XNetGetTitleXnAddr(&xna);
		if (flags != XNET_GET_XNADDR_PENDING)
		{
			if (flags != XNET_GET_XNADDR_NONE)
			{
				g_4cf792 = true;
				g_4cf793 = xna;
				if (flags & (XNET_GET_XNADDR_STATIC | XNET_GET_XNADDR_DHCP | XNET_GET_XNADDR_PPPOE))
				{
					g_4cf7b8.address_length = k_ipv4_address_length;
					g_4cf7b8.ipv4_address = (((xna.ina.s_addr & 0xff0000) | (xna.ina.s_addr >> 16)) >> 8) | (((xna.ina.s_addr << 16) | (xna.ina.s_addr & 0xff00)) << 8);
					g_4cf7b8.port = 0;
				}
				g_4cf791 = true;
			}
			else
				g_4cf792 = false;
		}
	}
	return g_4cf791;
}

// @retail 0x7ab60
bool function_07ab60(const s_type_99af70 *address, long local, long *index_out, XNKID *kid_out, XNKEY *key_out, XNADDR *xnaddr_out)
{
	bool result = false;
	long index = NONE;
	dword ina;
	XNKID kid;
	XNKEY key;

	memset(&kid, 0, sizeof(kid));
	memset(&key, 0, sizeof(key));

	if (address->address_length == k_ipv4_address_length && address->ipv4_address)
	{
		if (local)
		{
			ina = byte_swap_long(address->ipv4_address);
			memset(xnaddr_out, 0, sizeof(*xnaddr_out));
			xnaddr_out->ina.s_addr = ina;
			result = true;
		}
		else if (function_07aec0(address, &ina))
		{
			IN_ADDR in_addr;

			in_addr.s_addr = ina;
			if (XNetInAddrToXnAddr(in_addr, xnaddr_out, &kid) == 0)
			{
				long i;
				s_xnet_registry_entry *entry;

				result = true;
				for (i = 0; i < 8; i++)
				{
					entry = &g_4cf7d4[i];
					if (entry->valid && memcmp(&entry->kid, &kid, sizeof(kid)) == 0)
					{
						index = i;
						key = entry->key;
						break;
					}
				}
			}
		}
	}
	if (index_out)
	{
		*index_out = index;
	}
	if (kid_out)
	{
		*kid_out = kid;
	}
	if (key_out)
	{
		*key_out = key;
	}

	return result;
}

__declspec(noinline) bool function_07acc0(const s_type_99af70 *address);

// @retail 0x7acc0
bool function_07acc0(const s_type_99af70 *address)
{
	bool result = false;
	dword ina;

	if (function_07aec0(address, &ina))
	{
		IN_ADDR in_addr;

		in_addr.s_addr = ina;
		if (XNetConnect(in_addr) == 0)
			result = true;
	}

	return result;
}

// @retail 0x7acf0
long function_07acf0(const s_type_99af70 *address)
{
	dword ina;

	if (function_07aec0(address, &ina))
	{
		IN_ADDR in_addr;

		in_addr.s_addr = ina;
		switch (XNetGetConnectStatus(in_addr))
		{
		case 0:
			return 0;
		case 1:
			return 1;
		case 2:
			return 2;
		case 3:
			return 3;
		}
	}

	return 4;
}

// @retail 0x7ad50
void function_07ad50(byte *buffer)
{
	do
	{
		transport_get_random_bytes(8, buffer);
	}
	while (memcmp(g_46725c, buffer, 8) == 0);
}

// @retail 0x7ae70
void function_07ae70(void)
{
	XNADDR xna;

	memset(&xna, 0, sizeof(xna));
	XNetGetTitleXnAddr(&xna);
	memcpy(g_4cf7cc, xna.abEnet, 6);
}

// @retail 0x7b3e0
void function_07b3e0(void)
{
	memset(&g_4cf8d4, 0, 8);
	g_4cf8d8 = data_new_inlined("transport qos attempts", 0x20, 8, 0, g_468758);
}

// score 39 (was 39): retail's second pool register is the only thing missing from the iterator form,
// which reproduces retail's found block (compare, dead datum add, immediate tail stores) and block
// layout byte for byte but keeps one pool register, 58 at 120 bytes. A file-local step that scans
// through the iterator's own pool field and reads the datum through a copy of it gives the same 58
// (the copy folds into the iterator's field), the same when the step is left for LTCG to inline, and
// 64 when the iterator's pool is kept live across the loop; the plain form with two pool locals
// spills the copy to the stack (65).
// Not kept (MiniMax H, 10-31): 11 with the pool read out of the global into two locals (a second,
// one-use `scan` copy for the search; 60 without it) and the index only assigned when a record is found:
// a one-use duplicate local steering the second pool register, so kept only with a match. Its notes:
// the pool is read out of the global into two locals, the index is only
// assigned when a record is found and the datum's index is taken from the other local; retail's
// two pool registers, the found path after the exit, the immediate tail stores and the scan
// loop's padding now match byte for byte. Still differs inside the found block: retail keeps the
// record size in ecx and the record data in esi and leaves a dead `add ecx, esi` behind the
// load, ours keeps the size in edx and the data in ecx and pads the block with two int3 bytes.
// Earlier: the scan uses the pool's own index search and datum index, so it keeps
// retail's do-while shape and the hoisted count and bitmap; retail keeps the pool in two registers
// (ebx for the scan, a hoisted `mov ebp, ebx` copy for the datum), lays the found path out after
// the exit and pads the scan loop; ours holds the pool in ebp only and lays the found path first.
// (ebx for the high-water/bitmap scan, ebp, copied from ebx before the loop, for the datum's size
// and data), lays the found path out after the exit with a dead datum `add`, stores immediate zeros
// at the end and pads the scan loop; ours holds the pool in ebp only, NONE in edx and lays the found
// path first. The iterator form (data_iterator_next_inlined, as ai.cpp's player walk) gives 58 at
// 120 bytes: retail's block layout, found-path compare, dead datum add and immediate final stores
// exactly; only the second pool register (push ebx, mov ebp, ebx) and the loop padding are missing.
// retail keeps the pool in two registers (ebx for the high-water/bitmap scan, a
// `mov ebp, ebx` copy made after the index init for the datum's size and data), puts the found path
// after the exit with a dead datum `add`, stores immediate zeros at the end and pads the scan loop;
// ours holds the pool in ebp only, keeps NONE in edx and lays the found path first. The iterator
// form (s_record_pool_iterator + data_iterator_next_inlined, as ai.cpp's player walk) gives all of
// retail's code except the second pool register (11 bytes short: push/pop ebx, the copy and the
// 7-byte pad that the copy brings), 58 positional.
// Tried: earlier iterator form 58. Round 415: direct scan/identifier/both forced declarations
// 39/39/39; outlined declarations 74/44/91; live pool 45, captured tail 42. Kimi lane 2 round 37:
// absolute-index scan with the salted-index helper 39; direct expression 39; copied/independent/
// const scan pools 39/64/39; counted for scan 121; refreshed pool 39; absolute-datum getter 71;
// split pools 39. Round 17 / lane 11: the iterator form 58; without the datum_index init 58; the
// pool through a local 58; data_iterator_next_calling 94 (0x16bc00 stays a call);
// data_next_absolute_index_inlined scan 39. Lane 14 round 10: data_datum_iterator_next_inlined 58;
// the scan reading the global with the datum from the local 64, the reverse 65; the iterator's own
// fields with data_find_index/data_datum_index 60. Round 20 (iterator form): the datum through
// data_datum_index on the iterator's pool 57, on the global 69 (both pool registers and the pad
// appear, the datum pointer spills), on a pool local 57; index set before the pool 58; this form
// with the scan on the global 64. Lane 13 round 18: the iterator form moved into the /Ob1 QoS file
// beside 0x7b4c0 58 (same code), this body there 39 (same code); the out-of-line 0x16bb70 step 86
// (stays a call); one pool local for the iterator and the final store 64 (copies coalesce); the
// out-of-line 0x16bb50 datum index 44 (stays a call); diagnostics: a step copy reading the pool
// separately for the scan and the datum 58, the index stored before that second read 59 (one pool
// register each time).
// split pools 39.
// Round 17: s_record_pool_iterator with data_iterator_next_inlined (as ai.cpp's player walk) 58:
// 120 bytes, retail's block layout, found-path compare, dead datum add and immediate final
// stores exactly; only the second pool register (push ebx, mov ebp, ebx) and the loop padding
// are missing; without the datum_index init 58; the pool through a local 58;
// data_iterator_next_calling 94 (0x16bc00 stays a call); data_next_absolute_index_inlined scan 39.
// Round 20 (iterator form unless noted): the datum through data_datum_index on the iterator's
// pool 57, on the global 69 (both pool registers and the pad appear, the datum pointer spills),
// on a pool local 57; index set before the pool 58; this form with the scan on the global 64.
// retail holds the pool in two registers (ebx for the high-water/bitmap scan, ebp, copied
// from ebx before the loop, for the datum's size and data), puts the found path after the exit with a dead
// datum `add`, stores immediate zeros at the end and pads the scan loop; ours keeps one pool register, NONE in
// edx and the found path first. Earlier iterator form 58. Round 415: direct scan/identifier/both forced
// declarations 39/39/39; outlined declarations 74/44/91; live pool 45, captured tail 42. Kimi lane 2 round 37
// also reached 39 by scanning by absolute index with the salted-index helper (pool in EBP rather than EBX/EBP).
// Its other forms: direct expression 39; copied/independent/const scan pools 39/64/39; counted for scan 121;
// refreshed pool 39; absolute-datum getter 71; split pools 39. Lane 11 r17: data_iterator_next_inlined 58
// (retail's structure, aligned 9, 11 bytes short: one pool register, no loop padding), without the datum_index
// init 58, data_iterator_next_calling 94, pool local assigned to the iterator 58, absolute-index scan 39. Round
// 10 (lane 14): data_datum_iterator_next_inlined 58 (aligned 9); the scan reading the global with the datum from
// the local 64, the reverse 65; the iterator's own fields with data_find_index/data_datum_index 60; the
// iterator without the datum_index init 58. Original helper declarations, pool snapshot, loop and final global
// stores restored.
// Round 20 (iterator form unless noted): the datum through data_datum_index on the iterator's
// pool 57, on the global 69 (both pool registers and the pad appear, the datum pointer spills),
// on a pool local 57; index set before the pool 58; this form with the scan on the global 64.
// split pools 39. Round 17 / lane 11 r17 (iterator form): without the datum_index init 58; the pool
// through a local 58; data_iterator_next_calling 94 (0x16bc00 stays a call); pool local assigned to
// the iterator 58; data_next_absolute_index_inlined scan 39. Round 20 (iterator form): the datum
// through data_datum_index on the iterator's pool 57, on the global 69 (both pool registers and the
// pad appear, the global is reloaded after the call and the datum pointer spills), on a pool local
// 57; index set before the pool 58; the scan on the global 64. Lane 14 r10: data_datum_iterator_
// next_inlined 58; the scan reading the global with the datum from the local 64, the reverse 65; the
// iterator's own fields with data_find_index/data_datum_index 60. Lane 13 r18: iterator form in
// network_qos.cpp 58, kept body there 39 (file flags change nothing); out-of-line 0x16bb70 86 and
// 0x16bb50 44 (stay calls); one pool local also for the final store 64. Lane 18 r1 (iterator form):
// a step reading the iterator's pool at each use 58, scan on the iterator's field with the datum on
// the step's local copy 58, the loop in a forceinline helper taking the pool as a parameter 58 (all
// three: the copies coalesce into one pool register); iterator at function scope with the pool
// stored last 58; the step called before the loop and at its end 53 (135 bytes; the first step
// becomes a constant-start scan). The copy looks like an uncoalesced parameter copy from LTCG
// inlining the real step/scan (0x16bb70/0x16bc00), which /Ob1 on unknown_16b570.cpp keeps out of
// line here.
// Round 198: Native unsigned absolute cursor, with immutable receiver and/or chained terminal
// zero stores, 54 each; signed long/int cursor with chained zero stores 39. Original body restored.
// Round 45 (lane 9): iterator form re-measured 58 (aligned 9); iterator set up through an inline (iterator, pool)
// helper 58; datum index through data_datum_index on a pool local 57; iterator at function scope 58; a `for` over the
// step's returned datum 53 (135 bytes); scan on the global with the datum from a pool local 64 (the local spills).
// Lane 14 r19 (fast `0x7b430 0x7b650` reproduces 39): index declared before the pool 39; a `for` loop
// from data_find_index(pool, 0) 125 (188 bytes); the final valid store through the pool local 42;
// iterator fields stored index first with the global read into the data field 58; iterator
// initialised from the global 58 (both: retail's code but one pool register). The matched 0x18c060's
// EBX/EBP pair is the hoisted global plus the iterator copy; this global is aliasable (reloaded at the
// end in retail too), so retail's EBX here is a local copy.
// Round 12 (lane 18), the two-local form (one-use `scan` copy for the search, `found` assigned to the index): 13
// at 131 bytes (the pool copy made before the index init); copy-then-assign order swapped 57 (116 bytes); scan
// declared before the pool 13; the index declared before both pools 11, between them 11 (retail's copy order;
// left: the found block keeps size/data in edx/ecx with no dead datum `add`). Not kept (one-use duplicate local).
// Lane 14 r36 (fast): the second pool register is a second read of the global, not a copy: `scan = g_4cf8d8`
// and `pool = g_4cf8d8` read separately give retail's `mov ebx, [g]` ... `mov ebp, ebx` and loop pad, 11 at
// 131 bytes (a copy of the first local instead, 60, coalesces); with the iterator's pool for the datum and the
// separate read for the scan 13; the iterator form's scan with a second read for data_datum_index 70 (147
// bytes); the iterator form aggregate-initialised 58, with the datum named in the while condition 58, with an
// early return 58. Retail's form needs the iterator step's dead datum `add` together with the two reads,
// which only the step inlined at link time from its own file gives (unknown_16b570.cpp is /Ob1).
// @retail 0x7b430
void function_07b430(void)
{
	if (g_4cf8d4)
	{
		s_record_pool *pool = g_4cf8d8;
		long index = NONE;

		while ((index = data_find_index(pool, index + 1)) != NONE)
		{
			qos_release(data_datum_index(pool, index));
		}
		g_4cf8d8->valid = 0;
		g_4cf8d4 = false;
	}
}


/* the session description as the network codecs pack it into a bit stream */
struct s_session_packet
{
	word flag0;
	byte unknown2;
	char field3;
	word name[32];
	long type;
	dword field48;
	dword field4c;
	dword field50;
	dword field54;
	dword field58;
	byte unknown5c[0x18];
	dword field74;
	dword field78;
	dword field7c;
	dword field80;
	dword field84;
	dword field88;
	byte unknown8c[0x18];
	dword fielda4;
	dword fielda8;
	dword fieldac;
	byte unknownb0[4];
	dword fieldb4;
	byte unknownb8[0x14];
	char fieldcc[12];
	byte unknownd8[0x18];
	union
	{
		dword d[6];
		short w[14];
	} u;
	byte unknown10c[0x130 - 0x10c];
};

/* a value in 0..maximum, written in the fewest bits that hold the maximum */
#define STREAM_WRITE_RANGE(stream, value, maximum) \
	{ \
		long bits = 0; \
		do \
		{ \
			bits++; \
		} \
		while (((1 << bits) - 1) < (maximum)); \
		function_1947e0((stream), (value), bits); \
	}

// @retail 0x7cc50
void __stdcall function_07cc50(s_bitstream *stream, void *part)
{
	const s_session_packet *packet = (const s_session_packet *)part;
	long i;

	stream_write_checked(stream, packet->type, 4);
	if (packet->type)
	{
		stream_write_checked(stream, packet->flag0, 1);
		i = 0;
		for (; i < 32; i++)
		{
			word c = packet->name[i];
			function_195720(stream, c, 16);
			if (c == 0)
			{
				break;
			}
		}
		stream_write_checked(stream, packet->field3 + 1, 7);
		stream_write_checked(stream, packet->field48, 15);
		STREAM_WRITE_RANGE(stream, packet->field4c, 6);
		stream_write_checked(stream, packet->field50, 16);
		stream_write_checked(stream, packet->field54, 16);
		STREAM_WRITE_RANGE(stream, packet->field58, 3);
		STREAM_WRITE_RANGE(stream, packet->field74, 0x10);
		STREAM_WRITE_RANGE(stream, packet->field78, 0x10);
		stream_write_checked(stream, packet->field7c, 16);
		stream_write_checked(stream, packet->field80, 16);
		stream_write_checked(stream, packet->field84, 16);
		STREAM_WRITE_RANGE(stream, packet->field88, 2);
		STREAM_WRITE_RANGE(stream, packet->fielda4, 2);
		STREAM_WRITE_RANGE(stream, packet->fielda8, 2);
		stream_write_checked(stream, packet->fieldac, 16);
		STREAM_WRITE_RANGE(stream, packet->fieldb4, 8);
		STREAM_WRITE_RANGE(stream, packet->fieldcc[0], 3);
		STREAM_WRITE_RANGE(stream, packet->fieldcc[1], 7);
		STREAM_WRITE_RANGE(stream, packet->fieldcc[2], 7);
		STREAM_WRITE_RANGE(stream, packet->fieldcc[3], 4);
		STREAM_WRITE_RANGE(stream, packet->fieldcc[4], 4);
		STREAM_WRITE_RANGE(stream, packet->fieldcc[5], 4);
		STREAM_WRITE_RANGE(stream, packet->fieldcc[6], 6);
		STREAM_WRITE_RANGE(stream, packet->fieldcc[7], 6);
		STREAM_WRITE_RANGE(stream, packet->fieldcc[8], 0x13);
		STREAM_WRITE_RANGE(stream, packet->fieldcc[9], 3);
		STREAM_WRITE_RANGE(stream, packet->fieldcc[10], 0x14);
		STREAM_WRITE_RANGE(stream, packet->fieldcc[11], 0x14);

		switch (packet->type)
		{
		case 9:
			stream_write_checked(stream, packet->u.w[12], 16);
			stream_write_checked(stream, packet->u.w[13], 16);
		case 1:
			stream_write_checked(stream, packet->u.d[0], 8);
			stream_write_checked(stream, packet->u.d[1], 16);
			STREAM_WRITE_RANGE(stream, packet->u.d[2], 2);
			STREAM_WRITE_RANGE(stream, packet->u.d[3], 1);
			STREAM_WRITE_RANGE(stream, packet->u.d[4], 3);
			STREAM_WRITE_RANGE(stream, packet->u.d[5], 2);
			return;
		case 2:
			stream_write_checked(stream, packet->u.d[0], 3);
			break;
		case 3:
			stream_write_checked(stream, packet->u.d[0], 3);
			STREAM_WRITE_RANGE(stream, packet->u.w[2], 3);
			STREAM_WRITE_RANGE(stream, packet->u.w[3], 1);
			STREAM_WRITE_RANGE(stream, packet->u.w[4], 2);
			STREAM_WRITE_RANGE(stream, packet->u.w[5], 3);
			break;
		case 4:
			stream_write_checked(stream, packet->u.d[0], 5);
			stream_write_checked(stream, packet->u.w[2], 16);
			return;
		case 7:
			stream_write_checked(stream, packet->u.d[0], 7);
			STREAM_WRITE_RANGE(stream, packet->u.w[2], 2);
			break;
		case 8:
			STREAM_WRITE_RANGE(stream, packet->u.w[0], 8);
			stream_write_checked(stream, packet->u.w[1], 16);
			stream_write_checked(stream, packet->u.w[2], 16);
			break;
		default:
			__assume(0);
		}
	}
}
bool function_153750(s_player_appearance *appearance);

// @retail 0x7efa0
bool function_7efa0(s_bitstream *stream, s_player_appearance *appearance)
{
	memset(appearance, 0, sizeof(*appearance));
	appearance->colors[0] = (char)(function_1959c0(stream, 5) - 1);
	appearance->colors[1] = (char)(function_1959c0(stream, 5) - 1);
	appearance->colors[2] = (char)(function_1959c0(stream, 5) - 1);
	appearance->colors[3] = (char)(function_1959c0(stream, 5) - 1);
	appearance->unknown04[0] = (byte)(function_1959c0(stream, 3) - 1);
	appearance->unknown04[1] = (byte)function_1959c0(stream, 6);
	appearance->unknown04[2] = (byte)function_1959c0(stream, 6);
	appearance->unknown04[3] = (byte)function_1959c0(stream, 4);
	return function_153750(appearance);
}

struct s_player_message_properties
{
	word name[32];
	s_player_appearance appearance;
	word group_name[16];
	byte identity[12];
	char index;
	byte flags;
	char value7e;
	char value7f;
	bool flag80;
	char value81;
	byte unknown82[2];
	long value84;
	short value88;
	short value8a;
	long value8c;
};

// @retail 0x7ca70
byte function_07ca70(s_bitstream *stream, void *destination)
{
	s_player_message_properties *properties = (s_player_message_properties *)destination;
	read_word_string(stream, properties->name, 32);
	bool appearance_valid = function_7efa0(stream, &properties->appearance);
	if (function_1957d0(stream))
	{
		function_195820(stream, properties->identity, 96);
		function_194fa0(stream, properties->group_name, 16);
	}
	else
	{
		memset(properties->identity, 0, sizeof(properties->identity));
		memset(properties->group_name, 0, sizeof(properties->group_name));
	}
	if (function_1957d0(stream))
		properties->index = (char)function_1959c0(stream, 4);
	else
		properties->index = NONE;
	if (function_1957d0(stream))
		properties->value7e = (char)function_1959c0(stream, 7);
	else
		properties->value7e = NONE;
	if (function_1957d0(stream))
		properties->value7f = (char)function_1959c0(stream, 7);
	else
		properties->value7f = NONE;
	if (function_1957d0(stream))
	{
		properties->value84 = function_1959c0(stream, 4);
		if (function_1957d0(stream))
			properties->value88 = (short)function_1959c0(stream, 7);
		else
			properties->value88 = NONE;
		if (function_1957d0(stream))
			properties->value8a = (short)function_1959c0(stream, 7);
		else
			properties->value8a = NONE;
		if (function_1957d0(stream))
			properties->value8c = function_1959c0(stream, 30);
		else
			properties->value8c = NONE;
	}
	else
	{
		properties->value84 = NONE;
		properties->value8a = NONE;
		properties->value88 = NONE;
		properties->value8c = NONE;
	}
	properties->flags = (byte)function_1959c0(stream, 2);
	properties->flag80 = function_1957d0(stream) != 0;
	properties->value81 = (char)function_1959c0(stream, 3);
	if (wcslen((const wchar_t *)properties->name) < 32 && appearance_valid &&
		(properties->index == NONE || (properties->index >= 0 && properties->index < 16)) &&
		properties->value81 >= 0 && properties->value81 < 8)
		return true;
	return false;
}

struct s_session_message_values
{
	short unknown00;
	short type;
	long mode;
	long value08;
	long value0c;
	long value10;
	byte data14[8];
	byte data1c[16];
	byte data2c[36];
	long value50;
	long value54;
	long count58;
	long count5c;
	long count60;
	long count64;
	dword value68;
	dword value6c;
	long index70;
	long value74;
	long value78;
	long value7c;
	long value80;
	long value84;
	long count88;
	dword identities[16][3];
};

// @retail 0x7da60
bool function_7da60(const s_session_message_values *values)
{
	if (values && values->type >= 0 && values->type < 2 &&
		values->mode >= 0 && values->mode < 5 &&
		(values->value08 == NONE || (values->value08 > 0 && values->value08 <= 0xfffe)) &&
		(values->value0c == NONE || (values->value0c > 0 && values->value0c <= 0xfffe)) &&
		values->count58 >= 0 && values->count58 <= 16 &&
		values->count5c >= 0 && values->count5c <= 16 &&
		values->count60 >= 0 && values->count60 <= 16 &&
		values->count64 >= 0 && values->count64 <= 16 &&
		values->value68 < 0x10000 && values->value6c < 0x10000 &&
		values->index70 >= 0 && values->index70 < 16)
		return true;
	return false;
}

// @retail 0x7df00
bool function_7df00(s_bitstream *stream, s_session_message_values *values)
{
	values->unknown00 = (short)function_1959c0(stream, 8);
	values->type = (short)function_1959c0(stream, 2);
	values->mode = function_1959c0(stream, 3);
	values->value08 = function_1959c0(stream, 16) - 1;
	values->value0c = function_1959c0(stream, 16) - 1;
	values->value10 = function_1959c0(stream, 4);
	function_195820(stream, values->data14, 64);
	function_195820(stream, values->data1c, 128);
	function_195820(stream, values->data2c, 288);
	function_195820(stream, &values->value50, 32);
	function_195820(stream, &values->value54, 32);
	values->count58 = function_1959c0(stream, 5);
	values->count5c = function_1959c0(stream, 5);
	values->count60 = function_1959c0(stream, 5);
	values->count64 = function_1959c0(stream, 5);
	values->value68 = function_1959c0(stream, 16);
	values->value6c = function_1959c0(stream, 16);
	values->index70 = function_1959c0(stream, 4);
	function_195820(stream, &values->value74, 32);
	values->value78 = function_1959c0(stream, 7);
	values->value7c = function_1959c0(stream, 7);
	values->value80 = function_1959c0(stream, 7);
	values->value84 = function_1959c0(stream, 7);
	values->count88 = function_1959c0(stream, 4);
	for (long i = 0; i < 16; i++)
	{
		if (i < values->count88)
			function_195820(stream, values->identities[i], 96);
		else
			memset(values->identities[i], 0, sizeof(values->identities[i]));
	}
	if (!stream_overflowed(stream) && function_7da60(values))
		return true;
	return false;
}

// @retail 0x7e080
bool function_7e080(const byte *data, long size, s_session_message_values *values)
{
	bool result = false;
	if (size > 0 && data)
	{
		s_bitstream stream;
		stream.unknown08 = 1;
		stream_set_buffer(&stream, data, size);
		function_1947a0(&stream);
		result = function_7df00(&stream, values) && stream.bit_position <= (stream.size_in_bytes << 3) && !stream.error;
	}
	return result;
}

struct s_message_identity
{
	dword words[3];
};

struct s_message_player_identity
{
	s_message_identity identity;
	byte unknown0c[0x13c - 0xc];
};

struct s_message_identities
{
	byte unknown00[0x10d0];
	dword player_mask;
	byte unknown10d4[0x11ec - 0x10d4];
	s_message_player_identity players[16];
};

// @retail 0x7ed20
bool function_7ed20(const s_message_identities *message, s_message_identity *common, bool *missing_out, bool *different_out)
{
	bool found = false;
	bool missing = false;
	bool different = false;
	s_message_identity first;
	dword mask = message->player_mask;
	for (long i = 0; i < 16; i++)
	{
		if (mask & (1 << i))
		{
			if (memcmp(&message->players[i].identity, g_440070, sizeof(first)) != 0)
			{
				if (found)
				{
					if (memcmp(&first, &message->players[i].identity, sizeof(first)) != 0)
						different = true;
				}
				else
				{
					first = message->players[i].identity;
					found = true;
				}
			}
			else
				missing = true;
		}
	}
	bool result = found && !missing && !different;
	if (missing_out)
		*missing_out = missing;
	if (different_out)
		*different_out = different;
	if (result && common)
		*common = first;
	return result;
}

// @retail 0x7ee10
void function_7ee10(s_bitstream *stream, const s_player_appearance *appearance)
{
	stream_write_checked(stream, appearance->colors[0] + 1, 5);
	stream_write_checked(stream, appearance->colors[1] + 1, 5);
	stream_write_checked(stream, appearance->colors[2] + 1, 5);
	stream_write_checked(stream, appearance->colors[3] + 1, 5);
	stream_write_checked(stream, (char)appearance->unknown04[0] + 1, 3);
	stream_write_checked(stream, appearance->unknown04[1], 6);
	stream_write_checked(stream, appearance->unknown04[2], 6);
	stream_write_checked(stream, appearance->unknown04[3], 4);
}

/* the title's address: the XNet address and the transport address, read
   again while the transport runs; true when the title has one */
// @retail 0x7aaa0
bool function_07aaa0(XNADDR *xnaddr, s_type_99af70 *address)
{
	if (g_transport_globals.initialized && g_transport_globals.started)
		function_07a9b0();
	if (xnaddr)
		*xnaddr = g_4cf793;
	if (address)
		*address = g_4cf7b8;
	return g_4cf792;
}

#include "unknown_0662e0.h"

struct s_member_quality_entry
{
	byte unknown00[0x90];
	long first;
	long second;
	long kind;
	dword mask;
	byte unknowna0[0x10c - 0xa0];
};
struct s_member_quality_collection
{
	byte unknown00[8];
	long count;
	s_member_quality_entry entries[16];
};

// @retail 0x7e100
void function_7e100(long current, const s_member_quality_collection *collection,
	long *selected, long *first, long *second, long *level)
{
	long best = NONE;
	long best_first = 0;
	long best_second = 0;
	if (current != NONE)
	{
		best = current;
		best_first = collection->entries[current].first;
		best_second = collection->entries[current].second;
	}
	long count = collection->count;
	for (long i = 0; i < count; i++)
	{
		const s_member_quality_entry *entry = &collection->entries[i];
		if ((!g_network_configuration.flag1ac || entry->kind != 3) &&
			entry->first >= g_network_configuration.valued0 &&
			entry->second >= g_network_configuration.valuecc &&
			entry->mask == (dword)((1 << count) - 1) &&
			entry->second >= best_second + g_network_configuration.valued4)
		{
			best = i;
			best_first = entry->first;
			best_second = entry->second;
		}
	}
	long quality = 0;
	for (long j = 16; j > 0; j--)
	{
		if (best_second >= g_network_configuration.value40[j])
		{
			quality = j;
			break;
		}
	}
	if (quality <= g_network_configuration.valuec8)
		quality = g_network_configuration.valuec8;
	if (selected)
		*selected = best;
	if (first)
		*first = best_first;
	if (second)
		*second = best_second;
	if (level)
		*level = quality;
}

struct s_session_description_payload
{
	short field0;
	short field2;
	long field4;
	long field8;
	long fieldc;
	long field10;
	byte field14[8];
	byte field1c[16];
	byte field2c[36];
	long field50;
	long field54;
	long field58;
	long field5c;
	long field60;
	long field64;
	long field68;
	long field6c;
	long field70;
	long field74;
	long field78;
	long field7c;
	long field80;
	long field84;
	long count;
	byte identities[16][12];
};

// @retail 0x7db10
void function_7db10(s_bitstream *stream, const s_session_description_payload *message)
{
	stream_write_checked(stream, message->field0, 8);
	stream_write_checked(stream, message->field2, 2);
	stream_write_checked(stream, message->field4, 3);
	stream_write_checked(stream, message->field8 + 1, 16);
	stream_write_checked(stream, message->fieldc + 1, 16);
	stream_write_checked(stream, message->field10, 4);
	function_1955d0(stream, message->field14, 64);
	function_1955d0(stream, message->field1c, 128);
	function_1955d0(stream, message->field2c, 288);
	function_1955d0(stream, &message->field50, 32);
	function_1955d0(stream, &message->field54, 32);
	stream_write_checked(stream, message->field58, 5);
	stream_write_checked(stream, message->field5c, 5);
	stream_write_checked(stream, message->field60, 5);
	stream_write_checked(stream, message->field64, 5);
	stream_write_checked(stream, message->field68, 16);
	stream_write_checked(stream, message->field6c, 16);
	stream_write_checked(stream, message->field70, 4);
	function_1955d0(stream, &message->field74, 32);
	stream_write_checked(stream, message->field78, 7);
	stream_write_checked(stream, message->field7c, 7);
	stream_write_checked(stream, message->field80, 7);
	stream_write_checked(stream, message->field84, 7);
	stream_write_checked(stream, message->count, 4);
	for (long i = 0; i < message->count; i++)
		function_1955d0(stream, message->identities[i], 96);
}

#include "unknown_19d220.h"
bool function_19d650(s_game_variant *variant);

#define SESSION_READ_RANGE(destination, type, maximum) \
	{ \
		long bits = 0; \
		do { bits++; } while (((1 << bits) - 1) < (maximum)); \
		(destination) = (type)function_1959c0(stream, bits); \
	}

// @retail 0x7d520
bool function_07d520(s_bitstream *stream, void *part)
{
	bool result = true;
	s_session_packet *packet = (s_session_packet *)part;
	packet->type = function_1959c0(stream, 4);
	if (packet->type)
	{
		packet->flag0 = (word)function_1959c0(stream, 1);
		function_194fa0(stream, packet->name, 32);
		packet->field3 = (char)(function_1959c0(stream, 7) - 1);
		packet->field48 = (dword)function_1959c0(stream, 15);
		SESSION_READ_RANGE(packet->field4c, dword, 6);
		packet->field50 = (dword)function_1959c0(stream, 16);
		packet->field54 = (dword)function_1959c0(stream, 16);
		SESSION_READ_RANGE(packet->field58, dword, 3);
		SESSION_READ_RANGE(packet->field74, dword, 0x10);
		SESSION_READ_RANGE(packet->field78, dword, 0x10);
		packet->field7c = (dword)function_1959c0(stream, 16);
		packet->field80 = (dword)function_1959c0(stream, 16);
		packet->field84 = (dword)function_1959c0(stream, 16);
		SESSION_READ_RANGE(packet->field88, dword, 2);
		SESSION_READ_RANGE(packet->fielda4, dword, 2);
		SESSION_READ_RANGE(packet->fielda8, dword, 2);
		packet->fieldac = (dword)function_1959c0(stream, 16);
		SESSION_READ_RANGE(packet->fieldb4, dword, 8);
		SESSION_READ_RANGE(packet->fieldcc[0], char, 3);
		SESSION_READ_RANGE(packet->fieldcc[1], char, 7);
		SESSION_READ_RANGE(packet->fieldcc[2], char, 7);
		SESSION_READ_RANGE(packet->fieldcc[3], char, 4);
		SESSION_READ_RANGE(packet->fieldcc[4], char, 4);
		SESSION_READ_RANGE(packet->fieldcc[5], char, 4);
		SESSION_READ_RANGE(packet->fieldcc[6], char, 6);
		SESSION_READ_RANGE(packet->fieldcc[7], char, 6);
		SESSION_READ_RANGE(packet->fieldcc[8], char, 0x13);
		SESSION_READ_RANGE(packet->fieldcc[9], char, 3);
		SESSION_READ_RANGE(packet->fieldcc[10], char, 0x14);
		SESSION_READ_RANGE(packet->fieldcc[11], char, 0x14);

		switch (packet->type)
		{
		case 9:
			packet->u.w[12] = (short)function_1959c0(stream, 16);
			packet->u.w[13] = (short)function_1959c0(stream, 16);
		case 1:
			packet->u.d[0] = (dword)function_1959c0(stream, 8);
			packet->u.d[1] = (dword)function_1959c0(stream, 16);
			SESSION_READ_RANGE(packet->u.d[2], dword, 2);
			SESSION_READ_RANGE(packet->u.d[3], dword, 1);
			SESSION_READ_RANGE(packet->u.d[4], dword, 3);
			SESSION_READ_RANGE(packet->u.d[5], dword, 2);
			break;
		case 2:
			packet->u.d[0] = (dword)function_1959c0(stream, 3);
			break;
		case 3:
			packet->u.d[0] = (dword)function_1959c0(stream, 3);
			SESSION_READ_RANGE(packet->u.w[2], short, 3);
			SESSION_READ_RANGE(packet->u.w[3], short, 1);
			SESSION_READ_RANGE(packet->u.w[4], short, 2);
			SESSION_READ_RANGE(packet->u.w[5], short, 3);
			break;
		case 4:
			packet->u.d[0] = (dword)function_1959c0(stream, 5);
			packet->u.w[2] = (short)function_1959c0(stream, 16);
			break;
		case 7:
			packet->u.d[0] = (dword)function_1959c0(stream, 7);
			SESSION_READ_RANGE(packet->u.w[2], short, 2);
			break;
		case 8:
			SESSION_READ_RANGE(packet->u.w[0], short, 8);
			packet->u.w[1] = (short)function_1959c0(stream, 16);
			packet->u.w[2] = (short)function_1959c0(stream, 16);
			break;
		default:
			result = false;
			break;
		}
		if (result)
		{
			s_game_variant checked;
			memcpy(&checked, packet, sizeof(checked));
			result = function_19d650(&checked);
		}
	}
	else
		memset(packet, 0, sizeof(s_game_variant));
	return result;
}

#undef SESSION_READ_RANGE


// @retail 0x7c5a0
void function_07c5a0(s_bitstream *stream, const void *source)
{
	const s_player_message_properties *properties = (const s_player_message_properties *)source;
	for (long i = 0; i < 32; i++)
	{
		word character = properties->name[i];
		function_195720(stream, character, 16);
		if (!character) break;
	}
	function_7ee10(stream, &properties->appearance);
	bool has_identity = memcmp(properties->identity, g_440070, sizeof(properties->identity)) != 0;
	stream_write_bit(stream, has_identity);
	if (has_identity)
	{
		function_1955d0(stream, properties->identity, 96);
		for (long i = 0; i < 16; i++)
		{
			word character = properties->group_name[i];
			function_195720(stream, character, 16);
			if (!character) break;
		}
	}
	stream_write_bit(stream, properties->index != NONE);
	if (properties->index != NONE) stream_write_checked(stream, properties->index, 4);
	stream_write_bit(stream, properties->value7e != NONE);
	if (properties->value7e != NONE) stream_write_checked(stream, properties->value7e, 7);
	stream_write_bit(stream, properties->value7f != NONE);
	if (properties->value7f != NONE) stream_write_checked(stream, properties->value7f, 7);
	stream_write_bit(stream, properties->value84 != NONE);
	if (properties->value84 != NONE)
	{
		stream_write_checked(stream, properties->value84, 4);
		stream_write_bit(stream, properties->value88 != NONE);
		if (properties->value88 != NONE) stream_write_checked(stream, properties->value88, 7);
		stream_write_bit(stream, properties->value8a != NONE);
		if (properties->value8a != NONE) stream_write_checked(stream, properties->value8a, 7);
		stream_write_bit(stream, properties->value8c != NONE);
		if (properties->value8c != NONE) stream_write_checked(stream, properties->value8c, 30);
	}
	stream_write_checked(stream, (char)properties->flags, 2);
	stream_write_bit(stream, *(const char *)&properties->flag80 > 0);
	stream_write_checked(stream, properties->value81, 3);
}

#include "unknown_059ad0.h"

struct s_surface_description
{
	long type;
	long field_4;
	dword field_8;
	char names[9][0x10];
	char descriptions[9][0x80];
	char field_51c[128];
	byte unknown59c[8];
	long points[16];
	long field_5e4;
	bool flag_5e8;
	byte unknown5e9[3];
	long width, height, depth, field_5f8, field_5fc, field_600;
	bool flag_604;
	byte unknown605[3];
	long field_608, field_60c;
	byte unknown610[4];
};
struct s_game_variant_globals
{
	dword unknown0, flags;
	word count, state;
};
extern s_game_variant_globals g_47d8f4;
extern s_surface_description g_551ae8[16];
s_surface_description *function_192e60(long index);
bool function_1934f0(s_surface_description *variant);
long function_1931a0(long count, s_surface_description *variant);
long function_193250(s_surface_description *variant);
long function_1932c0(s_surface_description *variant);
bool balance_teams_by_count(long player_count, const long *parties, long team_count, long maximum_team_size,
	bool flag_parties, long minimum, long maximum, bool use_parties, long *imbalance);
bool balance_teams_can_add(long player_count, const long *parties, long extra_count, long team_count,
	long maximum_team_size, bool flag_parties, long minimum, long maximum, bool use_parties, long maximum_imbalance);

struct s_ratings_summary
{
	long machine_count;
	s_session_id machine_ids[16];
	s_message_identity machine_users[16];
	long machine_times[16];
	long player_count;
	s_message_identity players[16];
	long first[16], second[16], groups[16];
};
struct s_matchmaking_ratings
{
	long average, maximum, minimum, player_count, rated_count;
	s_message_identity identities[16];
	long first[16], second[16], adjusted[16], groups[16];
	long group_count;
	long rated_counts[16], total_counts[16], averages[16];
	s_message_identity group_identities[16];
	long imbalance;
	dword add_mask, group_mask;
};

static inline long ratings_variant_capacity(s_surface_description *variant)
{
	switch (variant->type)
	{
	case 1: return variant->height;
	case 2: return variant->height;
	case 3: return variant->depth * variant->width;
	case 4: return variant->depth * variant->width;
	case 5: return variant->depth * variant->width;
	default: __assume(0);
	}
}
static inline long ratings_variant_teams(s_surface_description *variant)
{
	switch (variant->type)
	{
	case 1: return NONE;
	case 2: return NONE;
	case 3: return variant->width;
	case 4: return variant->width;
	case 5: return variant->width;
	default: __assume(0);
	}
}
static inline long ratings_party_minimum(s_surface_description *variant)
{
	if (variant->type == 5) return variant->height;
	if (variant->type == 4 && variant->flag_604) return variant->field_608;
	return 0;
}
static inline long ratings_party_maximum(s_surface_description *variant)
{
	if (variant->type == 5) return variant->depth;
	if (variant->type == 4 && variant->flag_604) return variant->field_60c;
	return 0;
}
static inline long ratings_party_spread(s_surface_description *variant)
{
	if (variant->type == 5) return variant->field_5f8;
	if (variant->type == 4 && variant->flag_604) return *(long *)variant->unknown610;
	return 0;
}
static inline bool ratings_party_limited(s_surface_description *variant)
{
	return (variant->type == 4 && variant->flag_604) || variant->type == 5;
}
static inline long ratings_allowed_imbalance(s_surface_description *variant)
{
	switch (variant->type)
	{
	case 1: return 0;
	case 2: return 0;
	case 3: return *(bool *)&variant->field_5f8 ? 1 : 0;
	case 4: return variant->field_5f8;
	case 5: return variant->field_5f8;
	default: __assume(0);
	}
}
// score 766 (was 766): the frame is 0xd4 where retail's is 0xb4, the session comes in on the stack
// where retail passes it in EAX and the output pointer on the stack, and the opening lookup is
// expanded here while retail calls the matched 0x192e60 out of line.
// score 766 (was 766): Frame 0xd4/0xb4, stacked session and protected opening lookup expansion remain.
// Earlier: retail's frame is 0xb4 and it takes the session in EAX and the output pointer
// on the stack, while ours has a 0xd4 frame and takes both on the stack; retail also calls the matched
// 0x192e60 out of line, ours inlines it.
// frame 0xd4 versus 0xb4, opening session home and lookup expansion remain; tried byte
// Frame 0xd4 versus 0xb4, session input and initial lookup expansion remain.
// Earlier: frame 0xd4 versus 0xb4, opening session home and lookup expansion remain; tried byte
// private histogram 852; wide group decision 840; shared group total base 879; native group identity copy
// 918; membership native maximum 834.
// frame 0xd4 versus 0xb4; session entry and lookup differ.
// Reduced preview 864 (754 code); ID phase 864; phased dword histogram 885;
// histogram phase 934; bound snapshots 936; split guards 864; capacity inline 920.
// frame/session/lookup still differ; reduced preview 864.
// Early team scalars 896; early party scalars 922; group observation 865;
// member-rating observation 864; allowed-imbalance observation 956; all restored.
// word histogram initialized at declaration; frame 0xd4 versus 0xb4 remains.
// Fast long result 873; byte result 868; later group decision 884; word initialization 848;
// final spread 873. Code-plus-extent 804/798/806/777/792 versus preview 788.
// Frame 0xd4 versus 0xb4 and session/lookup homes remain; scalar machine ID copy 840; player identity memmove 855; chained initial extrema 777; shared limited predicate 928; shared team-mode predicate 786 (fast).
// Frame 0xd4 versus 0xb4 and session/lookup homes remain. Shared player rating 831;
// descending rating scan 850; group scalar accumulators 868; group mask accumulator 804 (plain); histogram
// count cursor 836; common mask publication 798; shared mask spread 881; complete add-mask accumulator 817
// (fast).
// Frame 0xd4 versus 0xb4 and session/lookup homes remain. Machine-ID library copy 777; entry word histogram 855;
// shared rating observation 831; chained group clears 777; Boolean balance accumulation 766 kept at retail length;
// split lower-bound guard 777 (fast).
// Consistent player/group/type observations 854/832/847; player+group 886; player+type 828;
// group+type 948; all 833; full unsigned type 833. Original body and selectors restored.
// Round 155: retained native output-array bases, then group rated-count base: groups 847; identities 870; groups identities 889; ratings 913; groups ratings 863; identities ratings 912; groups identities ratings 864; group rated array 867. All restored.
// Identity/party/both/excess/identity-excess/party-excess/all test-result accumulation 897/869/856/772/888/861/848; all with logical identity 908 (fast).
// Original complete body restored; frame 0xd4/0xb4, stacked session and opening lookup expansion remain.
// minimum-result 808; maximum-result 814.
// spread-result 820; minimum-maximum-result 816.
// minimum-spread-result 813; maximum-spread-result 835.
// minimum-maximum-spread-result 820; switch-selector-results 766.
// Round 141: Native output/session/both object contracts 766 each; output-first pointer, output object,
// session object and both objects 766 each; readonly session 766. All original interfaces and call
// sites restored; frame/session/lookup remain.
// Round 134: all selector const pointers/const references/native values 766/766/1802;
// switch/field/flag const references 766/766/766;
// all mutable references/switch values 766/1079.
// Round 180 native short party
// extrema/spread/population/extrema-spread/extrema-population/spread-population/all, then word population
// 901/797/766/904/904/796/904/766 (fast); all restored. Frame 0xd4/0xb4 and protected opening lookup remain.
// Native selector case declarations: team cases first in capacity/teams/both/
// imbalance/capacity-imbalance/teams-imbalance/all 766/798/798/765/765/815/815;
// all three reversed 814. One-point forms rejected; original selectors restored.
// Round 173: Independent all/eligible histograms word/word 766, word/dword 826, dword/word 829,
// dword/dword 844; each with machine-ID phase scope 766/826/829/844. Dword forms recover frame 0xb4
// but worsen code-plus-extent 765 to 784/783/797. Frame 0xd4 versus 0xb4, stacked session and
// protected opening lookup expansion remain. Original complete body restored.
// Group zero-store orders, five combinations 766; initial extrema-store orders, three combinations 766.
// Available, first, last, spread, party count, team count, team size and combined
// party-boundary slot names 766 each (fast); original names restored.
// Round 380: Shared capacity height/product/both cases 766/877/876; team NONE/width/both cases 900/937/940; shared imbalance cases 936; all three selectors 963. Original selectors restored.
// Round 245: Current balance graph result byte/short/long/dword 784/841/841/841;
// paired group-decision domains 784/844/847/847. All original Boolean carriers restored;
// frame/session/lookup remain.
// Round 169: Admission/identity/excess result-join subsets 815/778/827/789/859/803/785;
// all joins with entry result 785. Original workspace lifetimes and returns restored;
// frame, session convention and opening lookup expansion remain.
// Round 220: Native dword histogram with forced capacity/teams/both/imbalance/
// capacity-imbalance/teams-imbalance/all switch selectors/all seven selectors
// 863/844/863/844/863/844/863/863. Frame 0xb4 recovered; original table and selectors restored.
// Round 174: coordinated count-balance/addition inline and noinline summaries,
// each boundary and four paired contracts, 766 each; all declarations restored.
// Frame 0xd4/0xb4, stacked session and protected opening lookup remain.
// Round 389: independent player/peer, machine/member and party induction identities,
// all seven subsets, then entry declarations for all five counters: 766 each (fast).
// Original counter names and declaration lifetimes restored; frame/session/lookup remain.
// Round 261: explicit initial histogram memset with word/short/long/dword cells
// 820/820/844/844; each with byte balance argument 871/871/870/870. Original clear and cells restored.
// Round 414: type-four-first conditional minimum/maximum/spread selectors, each pair
// and all three 766 each; all with type-five-first limited predicate 803 at 2832/2828.
// Original selectors, predicates and complete body restored.
// Y423: Balance-call-first logical result with independent all/eligible word/dword
// histograms 768/824/821/831; bitwise result 766/829/826/844 fast (control 766).
// Native dword workspaces recover frame 0xb4 but regress; original body restored.
// DL196: input group/rating/machine-user base subsets
// 853/855/907/856/850/855/856; all bases with admitted member-rating observation 856.
// Original executable body and all supporting contracts restored.
// T47: Uniform session-state observations / membership maximum selection / flattened
// complete type dispatch subsets 766/766/766/888/888/888/888; all with dword histogram
// 931. Original body, selectors and all protected declarations restored.
// @retail 0x7e210
bool function_7e210(c_class_58d20 *session, s_matchmaking_ratings *out)
{
	long variant_index = NONE;
	long local_0 = session->state;
	if (local_0 > 2 && local_0 <= 8) variant_index = session->value49c8;
	s_surface_description *variant = function_192e60(variant_index);
	const s_ratings_summary *summary = NULL;
	if (local_0 > 2 && local_0 <= 8 && session->flag49fd)
		summary = (const s_ratings_summary *)session->data4a00;
	long capacity = 16;
	if (local_0 > 2 && local_0 <= 8) capacity = session->value4994;
	bool result = true;
	memset(out, 0, sizeof(*out));
	if (!summary || summary->player_count > capacity || !variant ||
		capacity > ratings_variant_capacity(variant) || capacity < function_1932c0(variant))
		return false;
	if (variant->type == 5)
	{
		for (long i = 0; i < summary->machine_count; i++)
			if (!memcmp(&summary->machine_users[i], g_440070, 12)) result = false;
		if (!result) return result;
	}
	long available = capacity - summary->player_count;
	out->maximum = NONE;
	out->minimum = NONE;
	out->average = NONE;
	out->player_count = summary->player_count;
	out->rated_count = 0;
	out->maximum = 0;
	out->minimum = 127;
	out->average = 0;
	bool adding_group = false;
	for (long i = 0; i < out->player_count; i++)
	{
		long rating = summary->first[i];
		out->identities[i] = summary->players[i];
		out->groups[i] = summary->groups[i];
		out->first[i] = rating;
		out->second[i] = summary->second[i];
		long adjusted = summary->first[i];
		if (adjusted != NONE)
		{
			s_surface_description *rating_variant = NULL;
			if (variant_index >= 0 && variant_index < 16 && g_47d8f4.count && (g_47d8f4.flags & 2) &&
				function_1934f0(&g_551ae8[variant_index])) rating_variant = &g_551ae8[variant_index];
			if (variant_index)
			{
				long highest = NONE;
				for (long j = 0; j < summary->player_count; j++)
					if (summary->groups[j] == summary->groups[i] && highest <= summary->first[j]) highest = summary->first[j];
				if (highest != NONE)
				{
					long lower = highest;
					while (lower > 0 && rating_variant->field_51c[lower - 1] >= highest) lower--;
					if (adjusted <= lower) adjusted = function_1931a0(lower, rating_variant);
				}
			}
		}
		out->adjusted[i] = adjusted;
		if (!(out->identities[i].words[2] & 3))
		{
			out->maximum = out->maximum > rating ? out->maximum : rating;
			out->minimum = out->minimum > rating ? rating : out->minimum;
			out->average += rating;
			out->rated_count++;
		}
	}
	if (!out->rated_count)
	{
		out->maximum = NONE;
		out->minimum = NONE;
		out->average = NONE;
	}
	else out->average /= out->rated_count;
	out->group_count = summary->machine_count;
	s_session_id machine_ids[16];
	for (long i = 0; i < out->group_count; i++)
	{
		machine_ids[i] = summary->machine_ids[i];
		out->total_counts[i] = 0;
		out->rated_counts[i] = 0;
		out->averages[i] = 0;
		out->group_identities[i] = summary->machine_users[i];
		for (long j = 0; j < out->player_count; j++)
		{
			if (out->groups[j] == i)
			{
				if (!(out->identities[j].words[2] & 3))
				{
					out->averages[i] += out->first[j];
					out->rated_counts[i]++;
				}
				out->total_counts[i]++;
			}
		}
		if (!out->rated_counts) out->averages[i] = NONE;
		else out->averages[i] /= out->rated_counts[i];
	}
	word histogram[16] = {0};
	for (long i = 0; i < out->group_count; i++) histogram[out->total_counts[i] - 1]++;
	long last = NONE;
	long first = NONE;
	memset(histogram, 0, sizeof(histogram));
	long spread = 0;
	long parties = 0;
	if (ratings_party_limited(variant))
	{
		for (long i = 0; i < out->group_count; i++)
		{
			long count = out->total_counts[i];
			if (count >= ratings_party_minimum(variant) && count <= ratings_party_maximum(variant))
			{
				histogram[count - 1]++;
				parties++;
				if (first == NONE) first = count;
				last = count;
				spread = last - first;
			}
		}
		if (spread > ratings_party_spread(variant)) result = false;
		if (parties > ratings_variant_teams(variant)) result = false;
	}
	out->group_mask = 0;
	out->add_mask = 0;
	if (ratings_party_limited(variant) && parties > 0 && parties < ratings_variant_teams(variant))
	{
		adding_group = true;
		for (long count = ratings_party_minimum(variant); count <= ratings_party_maximum(variant); count++)
			if (last - count <= ratings_party_spread(variant) && count - first <= ratings_party_spread(variant))
				out->group_mask |= 1 << (count - 1);
		out->add_mask = out->group_mask;
	}
	long teams = NONE;
	long team_size = NONE;
	if (variant->type == 5 || variant->type == 3 || variant->type == 4)
	{
		teams = ratings_variant_teams(variant);
		team_size = capacity / teams;
	}
	if (variant->type == 5 || variant->type == 3 || variant->type == 4)
	{
		bool use_parties = variant->type == 5 || variant->type == 2 || variant->type == 4;
		result &= balance_teams_by_count(out->player_count, out->groups, teams, team_size,
			ratings_party_limited(variant), ratings_party_minimum(variant), ratings_party_maximum(variant), use_parties, &out->imbalance);
	}
	else out->imbalance = 0;
	if (!adding_group)
	{
		long excess = out->imbalance > ratings_allowed_imbalance(variant) ? out->imbalance - ratings_allowed_imbalance(variant) : 0;
		if (excess > available) return false;
	}
	if (result && !adding_group && variant->type != 5)
	{
		if (variant->type == 2 || variant->type == 4)
		{
			if (variant->type == 3 || variant->type == 4)
			{
				long minimum = variant->type == 4 ? variant->field_5fc : 1;
				long maximum = function_193250(variant);
				if (variant->type == 4 && variant->flag_604) maximum = ratings_party_minimum(variant) - 1;
				for (long count = minimum; count <= maximum; count++)
				{
					if (count <= available && balance_teams_can_add(out->player_count, out->groups, count, teams, team_size,
						ratings_party_limited(variant), ratings_party_minimum(variant), ratings_party_maximum(variant), true, ratings_allowed_imbalance(variant)))
						out->add_mask |= 1 << (count - 1);
				}
			}
			else if (available > 0) out->add_mask = 1;
		}
		else out->add_mask = (1 << available) - 1;
	}
	return result;
}
