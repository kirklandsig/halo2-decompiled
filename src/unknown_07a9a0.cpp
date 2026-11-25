#include "unknown_4cf790.h"
// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_07A9A0.CPP: the transport layer's local address cache, address
   resolution against the XNet key registry, and the QoS handle pool */

#include "unknown_11c920.h"
#include "data_array.h"
#include "data_iterator.h"
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

// @retail 0x7b430
void function_07b430(void)
{
	if (g_4cf8d4)
	{
		s_data_datum_iterator iterator;
		iterator.data = g_4cf8d8;
		iterator.index = NONE;
		s_record_pool *pool = g_4cf8d8;
		long index;

		while ((index = data_find_index(iterator.data, iterator.index + 1)) != NONE)
		{
			data_datum_iterator_set_current(&iterator, pool, index);
			qos_release(iterator.datum_index);
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
long function_193300(s_surface_description *variant);
long network_session_get_maximum_players(c_class_58d20 *session);
bool balance_teams_by_count(long player_count, const long *parties, long team_count, long maximum_team_size,
	bool flag_parties, long minimum, long maximum, bool use_parties, long *imbalance);
bool balance_teams_can_add(long player_count, const long *parties, long extra_count, long team_count,
	long maximum_team_size, bool flag_parties, long minimum, long maximum, bool use_parties, long maximum_imbalance);

struct s_session_summary;
long session_summary_get_machine_highest_value(s_session_summary *summary, long machine_index);

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
// score 871 (was 871): The frame is 0xd0 versus 0xb4. The opening variant lookup expands and the upper-capacity
// selector remains a call. Earlier score 964.
// Frame 0xd0 versus 0xb4. The opening lookup expands and the upper-capacity selector remains a call.
// Earlier score 871 (was 871; earlier 964): The frame is 0xd0 versus 0xb4. The opening variant lookup expands and the upper-capacity
// The 0xd0 frame exceeds retail by 0x1c, and the opening variant lookup expands.
// Previously noted: Earlier 964. The frame is 0xd0 versus 0xb4. The opening variant lookup expands and the upper-capacity
// selector remains a call (two stacked first lines merged in r108).
// Earlier score 871 (was 871): The frame is 0xd0 versus 0xb4. The opening variant lookup expands and the upper-capacity
// The frame remains 0xd0 versus 0xb4, the opening variant lookup expands and the capacity selector stays a call.
// the frame is 0xd0 versus 0xb4, the opening variant lookup expands,
// and the upper-capacity selector stays a call. An earlier body scored 964.
// Earlier score 871 (was 964): The frame is 0xd0 versus 0xb4. The opening variant lookup expands and the upper-capacity
// selector remains a call.
// M3 lane 14 r114 (fast with 0x73530, which matches throughout; control 871): the convention already agrees (session in
// EAX, `ret 4` in both). Retail calls 0x192e60 at the top, 0x1932c0, 0x1931a0 and 0x193250, and expands 0x193300, the
// player loop's variant lookup and 0x631f0; ours expands the top lookup and calls 0x193300 and 0x631f0. Tried: the loop's
// written-out lookup as a call to 0x192e60 1007 (2620 B: the loop's call stays out of line, the top stays expanded); the
// two early exits as `result = false; goto done;` with one return at the end 903 (2748 B); both together 1016; the first
// exit as `{ result = false; return result; }` 871 (same code); `bool result = true;` declared first 871 (same code).
// Diagnostics (one-object link screen, restored): the top call under one test becomes a call; /d2inlT40-130 on this
// file alone changes no decision; with the first exit not returning (a behaviour change) the loop's lookup, 0x631f0,
// 0x1931a0 and 0x193250 all expand, so the exit's frequency halving is what keeps the loop cold here.
// MiniMax I (11-17): no new form, no lower score (964): re-measured the baseline; retail's session arrives in EAX
// with a 0xb4 frame against our 0xd4, and that ingress is fixed by the callers in the other files, and the opening
// 0x192e60 and the capacity test are inlined or called the other way round here (both callees in other files), so
// the body stands.
// MiniMax E (11-16): no new form, no lower score (964): retail's session arrives in eax with a 0xb4 frame against our 0xd4, and that convention is fixed by the caller in a file this lane may not edit.
// MiniMax C (11-16): no new form, no lower score (766): retail takes the session in EAX and the output pointer on the
// stack (frame 0xb4), where ours takes the session from the stack (frame 0xd4), and retail calls the matched 0x192e60
// out of line where this build expands the lookup inline; the session's EAX home belongs to the callers in the other
// files.
// Earlier: 0xd4 frame versus retail's 0xb4. The opening lookup expands here and the capacity check remains a call.
// Earlier score 871 (was 964): 0xd0 frame versus retail's 0xb4. The opening lookup expands here and the capacity check remains a call.
// Earlier at 871 (was 964): 0xd0 frame versus retail's 0xb4. The opening lookup expands here and the capacity check remains a call.
// Frame 0xd0 versus retail 0xb4, with an expanded opening variant lookup
// and a capacity selector call. The dword histogram trials reduce the frame to 0xb0.
// Independent dword histograms reduce the frame to 0xb0.
// Earlier score 942 (was 964): 0xd4 frame versus retail's 0xb4. The opening lookup expands here and the capacity check remains a call.
// Earlier: frame 0xd4 against retail's 0xb4; the session now arrives in EAX as retail, but the top 0x192e60 is
// frame 0xd0 against retail's 0xb4; the session now arrives in EAX as retail, but the top 0x192e60 is
// inlined and the capacity test and 0x631f0 are calls (retail the reverse). Earlier first line 766 (was 766): the session on the stack
// coordinator 11-16: the player loop's group-highest scan through the existing 0x631f0 (retail inlines
// it here; network_session_membership.cpp calls it the same way), kept because it matches 0x73530 (the session reaches it
// in EAX); 2684 bytes against retail's 2828, the top 0x192e60 still inlined and 0x631f0 still a call (retail the reverse).
// Earlier 766 (was 766): the frame is 0xd4 where retail's is 0xb4, the session comes in on the stack
// where retail passes it in EAX and the output pointer on the stack, and the opening lookup is
// expanded here while retail calls the matched 0x192e60 out of line.
// MiniMax H (11-12): no new form, no lower score (766): the frame is still 0xd4 against retail's 0xb4, the session still comes in on the stack where retail passes it in eax with the output pointer on the stack, and the opening lookup is still expanded here while retail calls the matched 0x192e60 out of line.
// MiniMax S (11-11): no new form, no lower score (766): the frame is 0xd4 against retail's 0xb4, the session
// comes in on the stack where retail passes it in EAX with the output pointer on the stack, and the opening
// lookup is expanded here while retail calls the matched 0x192e60 out of line; the session's EAX home belongs
// to the callers in other files.
// MiniMax E (11-05): no new form, no lower score; the session's EAX home belongs to the caller's
// declaration, which this lane may not change.
// w14C (11-06): walked against retail block by block; no behaviour difference. The histogram cells are dwords in retail
// (0x10 dwords cleared at 0x7e66c/0x7e6a5 with rep stosd, 4-byte inc at 0x7e682-0x7e68f and 0x7e740-0x7e74d); `long
// histogram[16]` with a memset for each clear recovers retail's frame 0xb4 (machine_ids and the histogram share slots)
// but scores 844 (2772 bytes). The histogram is never read, so it is not kept without a match. Tried: the add-loop minimum as a
// three-way type 5/4/other (no effect).
// Earlier note (766, was 766; round 103 unchanged): the frame is 0xd4 where retail's is 0xb4, the session comes in on the stack
// Earlier 766 (was 766; round 103 unchanged): the frame is 0xd4 where retail's is 0xb4, the session comes in on the stack
// where retail passes it in EAX and the output pointer on the stack, and the opening lookup is
// expanded here while retail calls the matched 0x192e60 out of line.
// Earlier: Frame 0xd4/0xb4, stacked session and protected opening lookup expansion remain.
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
// Round 40 (lane 13, fast): the opening through the matched accessors (get_value_49c8, get_data_4a00,
// network_session_get_maximum_players) 766 (same code, kept: their bodies were written out here). Ours
// calls the capacity switch (this file's helper, or 0x193300) out of line where retail inlines it (jump
// table), and inlines whichever 0x192e60 call comes first: the getter called in the player loop 857
// (top inlined, loop called); the capacity helper `__forceinline` 906 (2872 bytes); both 859 at 2828 bytes
// with retail's seven out-of-line calls but top/loop reversed; that with a long histogram 869 (frame 0xb4,
// 2792 bytes); the matched accessors 0x193440/0x1933d0/0x1933a0/0x193400/0x1934d0/0x1934b0/0x193470 for
// this file's helpers 904 (0x193300 too) / 840 with the capacity helper forceinline (2828 bytes, aligned
// 447 against 515) / 869 with the loop getter; every helper `__forceinline` 906, with the loop getter 859.
// Round 285: three multi-use pointer-home names independently and five joint lexical
// orders 766 each; original pointer names, workspaces and body restored.
// Round 333: Uniform identity memcpy/memmove with word cells 766/855, dword cells 844/876; scoped
// ID counterparts 766/855/844/875. Frame/session/lookup remain; original transfers restored.
// M287: Existing matched minimum/maximum/team selectors in seven subsets,
// all with the existing limited selector:
// 808/814/906/816/921/899/910/910. Original private selectors and body restored;
// frame/session/initial lookup expansion remain.
// Round 310: Native variant-byte flag/spread and identity-byte layout subsets 1..7 766 each; all
// with dword histogram 844. Original actual layouts, histogram, selectors, protected lookup and
// complete body restored; frame/session/lookup remain.
// M299: Native short identity/player/group induction subsets
// [871, 863, 865, 873, 881, 886, 885], all word 885. Original long counters restored.
// Round 136 (m3 lane 6, fast): the loop's variant check through the existing 0x192db0 876 (2768 B; that call stays
// out of line); through 0x192e60 857; 0x192e60's own body calling 0x192db0 766 (same code everywhere); 0x192e60 as
// a ternary or as if/else with no default 908 (60 B, and 0x63220/0x73ca0 then inline it, 112/140). LTCG's inline
// choice follows 0x192e60's compiled size, so retail's out-of-line top call has to come from this function's side.
// Also round 136: `bool result = true;` declared first 766 (same code).
// M302: Independent all/eligible phase cells byte/word, word/byte, byte/byte,
// byte/dword, each aggregate/library first clear: 823/830/816/826/831/839/835/850.
// Original word workspace and both populations restored; frame/session/lookup remain.
// M312: Actual capacity/team selector interior case priorities, five capacity/two team/one joint:
// 766/766/766/766/766/766/766/766. Original body restored.
// Lane 14 r53 (fast): the variant lookup after the summary read 803; after the capacity 942 (2732 B, aligned 695 from
// 750: session then arrives in ECX and 0x73530 reads 1); the variant named on its own line or tested at once 766.
// The memset and result ahead of the opening lookups 926 (2736 B, aligned 779).
// Diagnostic, not kept: 0x192e60 pinned out of line gives retail's head, session in EAX and 0x73530 byte for byte,
// but this function 942 (2700 B) since the capacity switch then stays a call where retail has it inline. So retail's
// inline choices here are call out of line, capacity inline; ours are the reverse pair.
// M334: Current Boolean-balance/word-workspace graph with existing team/minimum/maximum/spread/
// limited/imbalance selectors forced inline, then all party bounds and all except capacity:
// 766/766/766/766/766/766/766/766. Original inline contracts restored; frame/session/lookup remain.
// Lane 14 r55 (fast): retail's player loop is the matched 0x63220 (session_summary_get_player_value) expanded: its
// MAX evaluates 0x1931a0 twice (the inline loop, then the call), with 0x631f0 and 0x192e60 inline inside it; the body
// here is that function written out. The call to 0x63220 1097 (2524 B: our link keeps it out of line; the function
// then returns with ret 4 as retail); with a __forceinline declaration here, or `inline` on 0x63220's definition, the
// same; diagnostic only, /arch:SSE on 0x63220's file the same. The clamp as a max of 0x1931a0 908 (2748 B, both calls
// out of line); the highest rating through 0x631f0 964 (2684 B); the loop variant through 0x192e60 plus that max 987;
// through 0x192e60 plus 0x631f0 1011. BLOCKED by our link not inlining 0x63220 here.
// Lane 18 r41 (fast = plain at the base): ours calls 0x193300's twin for the capacity and inlines the top 0x192e60
// (two 0x1934f0 calls), retail the reverse. The player loop through 0x63220 with the capacity switch forceinline 1033
// (2592 bytes: capacity inline, 0x63220 still called, top 0x192e60 still inlined, ret 8); adding_group declared
// beside result 833; the loop getter through 0x192e60 with the top lookup after the summary read 887 (2756), plus the
// capacity switch forceinline 858 (2828); adjusted seeded from the rating local 811. Diagnostic: `#pragma
// auto_inline(off)` around this function 766 (no effect on the link's inline choices).
// Round 146 (m3 lane 5, fast; body unchanged, 0x73530 is the target): the session takes EBP (so the stack) because the
// output pointer has EBX, the state ESI and the inlined getter's pointer EDI. The output pointer's address taken 766, read
// through its address 766; none of the loop-getter, capacity and long-histogram combinations gives the session EAX. Diagnostic,
// not kept: the top getter call inside `if (1)` or a constant-true local test stays out of line with retail's head, session
// EAX and 0x73530 byte for byte, 942 (2700 bytes), with the capacity helper force-inline 911 (aligned 471), plus the
// loop getter 956, plus a long histogram 915 (frame 0xb4); `do {} while (0)` or a bare block around the call 766; the call in
// both arms of the state test 915, with the capacity helper force-inline 856 (2828 bytes; the NONE arm folds, so not
// retail's shape). A natural enclosing test that emits no code is the lead.
// Y561: Existing capacity/imbalance/party-mode provider subsets 1..7,
// then all plus limited provider: 766/903/903/766/766/903/903/903.
// Original selectors/workspaces restored; frame/session/lookup remain.
// DL255: Direct descending group/initial-histogram/validation ordinals, subsets 1..7,
// then separate reverse native ID transfer: 831/794/831/917/852/824/852/845. NOT kept: all regress,
// frames 0xd4/0xd8 versus 0xb4 and stack ingress remain. Original complete body,
// histogram phases, selector contracts, observations and call operands restored.
// M359: Read-only receiver contracts on capacity/teams/minimum/maximum/spread/limited/
// imbalance selectors, individually then all: 766/766/766/766/766/766/766/766.
// NOT kept: frame/session/lookup unchanged; original complete helpers and body restored.
// DH333: Standard player extrema/member maximum/histogram clears, seven subsets
// 824/836/861/828/829/848/825; typed three-word identity transfers 895. Frame 0xd4/0xb4
// persists. NOT kept: regressions; original word table, observations and complete body restored.
// M361: Immediately overwritten maximum/minimum/average defaults elided in seven subsets, then
// success declared at its first test region:
// 771/771/771/776/771/771/851/851. NOT kept: no eligible byte match or supported closer.
// Original initializations and full body restored; frame/session/initial lookup remain.
// M363: Real summary or summary/count admission owns the opening query, early-return
// or positive-body forms: word 954/954/954/954; dword 982/982/982/982. NOT kept:
// frames 0xd8/0xb8 versus 0xb4, late lookup and session ECX; caller reduced 8 -> 1.
// Original complete body restored; frame/session/initial lookup remain.
// M366: Closed ID/initial/eligible histogram phase lifetimes, six new word-table subsets 766;
// full dword scopes 849/2780 bytes, scoped word first CRT clear 820/2812 (retail 2828).
// NOT kept: no eligible ingress/lookup repair; original complete body restored; dword frame alone is insufficient.
// M370: Complete initial-admission truth bool/byte/short/long, rejection then acceptance,
// all 948 at 2956/2828 bytes. NOT kept: no frame or opening lookup recovery; original
// full gate, body and selectors restored. Frame 0xd4/0xb4 and stacked session remain.
// M372: Plain while identity/player/group traversal, seven phase subsets 766;
// all plus initial histogram while 907, 2764/2828 bytes, frame 0xd8/0xb4. Original
// loops/body restored; frame 0xd4, stacked session and initial lookup remain.
// Round 148 (m3 lane 6, fast): `inline` on 0x192e60 766, on 0x1934f0 766, both 766. Player loop through a call to 0x63220:
// forceinline on its definition 1032 (2588), `inline` on its definition 1097 (2524), with `inline` on 0x192e60 1097; each gives
// 0x73530 its match (session in EAX, ret 4), but a score rise is not kept: held lead. Result first and capacity before the summary 785.
// M377: Whole native ID/output-identity library transfers before/after the
// call-free group phase, singles and joint placements: 937/937/838/831/986/957/986/957.
// NOT kept: all regress and shorten; ID transfers shrink frame to 0x50 rather
// than native 0xb4. Original element copies, arrays and complete body restored.
// M392: Native initial reject scopes after gate boundaries 1/3/1+3/1+2/1+4/2+3/1+2+3/all: 766/766/766/766/766/766/766/766.
// NOT kept: no native ingress/frame/lookup recovery; original short-circuit guard restored. Original complete body restored.
// Round 148 (m3 lane 6, fast): `inline` on 0x192e60 766, on 0x1934f0 766, both 766. Player loop through a call to 0x63220:
// forceinline on its definition 1032 (2588), `inline` on its definition 1097 (2524), with `inline` on 0x192e60 1097; each gives
// 0x73530 its match (session in EAX, ret 4), but a score rise is not kept: held lead. Result first and capacity before the summary 785.
// M3 lane 12 r93 (fast; 0x73530 8 throughout): the capacity switch with grouped case labels (1-2 height, 3-5 product)
// 876 (2852 bytes: the switch now inlines as retail's jump table, but the top 0x192e60 call still inlines); that with
// the loop's variant through 0x192e60 864 (2804 bytes: the loop call stays out of line, the top one inlines, the
// reverse of retail); that with the player value through 0x63220 1067 (2556 bytes, 0x63220 still out of line); the
// team-count switch grouped alone 940 (2704 bytes); the capacity helper with external `inline` linkage 840 (2828
// bytes). Our link inlines the first 0x192e60 call site in source order and keeps 0x63220 out of line.
// M3 lane 4 r167 (fast, 13 s; the one-file link reads 943 because it keeps the top call out of line, so it is no judge here): retail's top
// call stays out of line only when its site's frequency is lower than the entry block's. Written as an always-true ternary (no code, diagnostic)
// 942 (2700 bytes, 0x73530 byte for byte); the whole body in the positive arm of the admission test 815 (2868 bytes), as the else arm of
// `result = false` 815, plus the out-of-line top call 904 (2740 bytes). The player loop through a call to 0x63220 1097, with the out-of-line
// top 1145 (2476 bytes), plus the capacity helper forced inline 1106 (2512 bytes), in the positive arm 1061/1109/1070: 0x63220 stays a call in
// every form, so retail's frequency there comes from something else. The top lookup through a one-line inline wrapper, inline or forced,
// with a result local: 766 (no effect); `__assume(session != NULL)` first 766; the lookup after the capacity 942 (2732 bytes). The written-out
// loop passes `lower` to 0x1931a0 where retail passes `highest` (ECX still holds it at the call); passing `highest` 820 (2836 bytes).
// M3 lane 12 r93 (fast; 0x73530 8 throughout): the capacity switch with grouped case labels (1-2 height, 3-5 product)
// 876 (2852 bytes: the switch now inlines as retail's jump table, but the top 0x192e60 call still inlines); that with
// the loop's variant through 0x192e60 864 (2804 bytes: the loop call stays out of line, the top one inlines, the
// reverse of retail); that with the player value through 0x63220 1067 (2556 bytes, 0x63220 still out of line); the
// team-count switch grouped alone 940 (2704 bytes); the capacity helper with external `inline` linkage 840 (2828
// bytes). Our link inlines the first 0x192e60 call site in source order and keeps 0x63220 out of line.
// Round 218 (m3 lane 3, fast; masked = register/stack-masked aligned line pairs x2, control 422): the top lookup
// guarded by `variant_index != NONE ? ... : NULL` (a test retail does not have; diagnostic) 930 (2716 bytes, masked
// 406): the call stays out of line, the session arrives in EAX with retail's head, and 0x73530 matches byte for byte,
// but the capacity helper stays a call; with the capacity case labels grouped 940 (2732 bytes: the seven out-of-line
// calls are retail's set, the capacity switch inlines but as compares, not retail's table, and the early exit jumps to
// the shared tail where retail has its own `result = false` epilogue); that with a long histogram 928 (2700 bytes, masked
// 376); grouped labels and a long histogram without the guard 850; the early exit as `result = false; return result;` 766
// (same code). Lead: retail's call site is out of line with no guard, so its inline choice comes from elsewhere.
// Round 103 (m3 lane 9, fast): the player value through a call to 0x63220 1097 (2524 bytes: 0x63220 stays a call, the top
// 0x192e60 lookup still inlines, and the session then arrives in EAX with `ret 4`, so 0x73530 matches); with an `inline`
// declaration of 0x63220 in this file 1097; a file-local inline helper of 0x63220's shape 1097 (it stays a call too, and still
// does with the body cut after the player loop);
// a `__forceinline` declaration before 0x63220's definition (diagnostic) 1032 (2588: 0x63220 inlines but its own
// 0x192e60/0x631f0/0x1931a0 calls stay out of line, and 0x6e910 then inlines it, 0 -> 155); the loop's availability test through
// the matched 0x193f50 857 (2804, it stays a call); both early exits as `result = false; return result;` 766 (same code).
// MiniMax I (11-10): no new form, no lower score; retail's party histogram is 16 dwords at frame+0x44, cleared twice with `rep stosd` and indexed as `mov ebx,[esp + eax*4 + 0x40]` with the count-1 folded into the displacement, so our word cells and the 0xd4 frame stay (the dword cells reach retail's 0xb4 frame but score 844), and the session ingress and the inlined lookup remain the only differences.
// Round 103 (m3 lane 16, fast with 0x73530 (8 throughout), 0x63220, 0x192e60, 0x1934f0 and 0x1931a0 matched): the admission
// test with `!variant` first 766, with `!summary || !variant` first 766, with the capacity compares written capacity-last 766, with
// `capacity < summary->player_count` 766 (all four the same code); the type-5 machine scan ending in `break` once a match clears
// the result 893 (2764 bytes). The top 0x192e60 call's inline choice is the cause; it stays a flags lead (this file is /arch:SSE).
// Round 66 (m3 lane 2, fast; 0x6fa60, 0x63220 and 0x192e60 stay matched, 0x71730 426 throughout): retail makes seven calls here
// (0x192e60 at the top, 0x1931a0, 0x193250, 0x1932c0, 0x1934f0, 0x91e30, 0x91f00) and inlines everything else, the
// matched ratings accessors, the capacity switch and 0x63220 included. In retail the matched 0x66050 also keeps 0x192e60 out
// of line at an entry-block site (after another call), and 0x705f0 at a guarded one; the matched 0x6fa60 drops its call to
// this function in both builds (result and output unused); retail's 0x73530 pushes the output before setting EAX to the
// session (no reorder evidence); retail keeps the type-5 machine flag as this body does. On 766: the player value through
// the existing 0x63220 and every other helper through its matched accessor (0x193300, 0x193440, 0x1933d0, 0x1933a0,
// 0x1934d0, 0x193400, 0x1934b0, 0x193470) 1127 (2492 bytes); the same with this file's capacity helper kept 1127; that
// with the capacity labels grouped 1101 (2524 bytes); the accessors without 0x63220 and the capacity helper kept 904 (2744);
// that with grouped labels 851 (2808 bytes). Re-measures: the 0x63220 call alone 1097 (2524 bytes, 0x73530 matches); the
// accessors alone 904. Diagnostic, not kept: `#pragma inline_depth(255)` with the 0x63220 call 1097 (no change), so inline
// depth does not keep 0x63220 out of line. Blockers left: the link's inline choices, and 0x63220 staying a call here.
// Round 226 (m3 lane 3, fast, control 766, 0x73530 8): the player loop written as 0x63220's body with its calls (0x192e60 for
// the variant, 0x631f0 for the group's highest rating, a two-evaluation max of 0x1931a0) 1032 at 2588 B: 0x73530 matches (the
// top 0x192e60 call stays out of line), but none of the four loop calls inline (retail inlines 0x192e60, 0x631f0 and the first
// 0x1931a0 there); with a forceinline redeclaration of 0x631f0 here 1032 (no effect); with the capacity helper forceinline 998
// (2656 B, the top call inlines again, 0x73530 8); with forceinline redeclarations of 0x192e60 and 0x1931a0 as well 998.
// Round 69 (m3 lane 15, fast): the capacity helper with grouped case labels and external `inline` linkage together 876
// (2852 B, 0x73530 8: the switch inlines as a table, the top call still inlines). Our link inlines only the first direct
// 0x192e60 site here, and retail's one inlined copy is inside the inlined 0x63220, which fits one inlined copy of a callee
// per caller; 0x63220 never inlines in this link, so that cannot be tested from this function. No natural no-code test
// fits the top call: retail calls 0x192e60(-1) on the invalid-state path with no compare. Retail's out-of-line 0x63220
// itself calls 0x192e60, and so do matched 0x741f0, 0x73ca0, 0x596d0 and 0x19a0c5, all from files built without /arch:SSE
// (or /O1, /Ob1), so retail inlines 0x192e60 only inside the copy of 0x63220 it inlines here. This function's retail
// code has no float instruction, so its build's /arch cannot be read from it. Lead for the flags owner (not tried): this
// file is /arch:SSE like 0x192e60's, 0x63220's file is not; a non-SSE build of this body may keep the top call out of line
// as 0x741f0's does and let 0x63220 inline.
// Round 91 (m3 lane 1, fast with 0x72db0, 0x71730 and 0x192e60): the top lookup through a file-local inline wrapper of
// 0x192e60 766 (the wrapper and 0x192e60 both inline: call depth is not what keeps retail's call out of line); with the
// capacity helper's case labels grouped (re-measured 876, 2852 bytes, the switch then inlines as retail's table), the top
// lookup after the capacity read 850 (2836), after the summary read 869 (2844), that after-capacity form with the loop's
// variant through 0x192e60 844 (2788: the top call inlines and the loop call stays out of line, retail's reverse), and
// (diagnostic, not keepable) an `__assume` on the index ahead of the lookup 876 (no effect on the inline choice); this
// function with its helpers and records moved up to just after the identity records, ahead of 0x7ed20, 766 (same code,
// the file's 21 other functions unchanged). Body restored.
// m3 lane 2 r70: no new form run. The register-masked aligned diff (142 blocks, masked 348) is the session's stack ingress, the
// capacity switch kept as a call (retail inlines it as a jump table) and the register homes that follow; every natural spelling
// of the top lookup, the player loop, 0x63220 and the capacity helper is listed above. Retail inlines 0x192e60 here only inside
// its inlined copy of 0x63220, whose file (network_session_membership.cpp) is not /arch:SSE, while this file is; every file that
// keeps a 0x192e60 call out of line in retail (0x741f0, 0x66050, 0x705f0) is non-SSE too. BLOCKED on build flags (the user's
// decision; not tried, no flags diagnostic run).
// Then five local forms (fast, control 766, aligned 750, masked 348): this file's party minimum/maximum/spread copies written as
// the matched accessors 0x1933d0/0x1933a0 are (`long result = 0;` then if/else-if, one return) 820 at 2792 bytes (aligned 601,
// masked 343: every inlined copy now has retail's `xor` first and `cmp byte [ebp+0x604], 0`); with the teams and imbalance
// copies as the accessors' switch-with-result form too 908 (2744, masked 494); that with `long histogram[16]` 903 (2712); the
// switch forms alone 903 (2764); the party forms with `long histogram[16]` 830 (2760 bytes, masked 313, frame 0xb4 as retail;
// first difference +0x7). 0x73530 stays 8 and every matched caller and callee stays matched in all five.
// M3 lane 12 r114 (fast with the file's 22 functions, 0x73530 (8 throughout), 0x63220, 0x192e60, 0x1934f0 and 0x1931a0, control
// 766 [masked aligned 486, register-masked 267]): `excess` as a zero default raised by an if 778 (2812 B); the rated-count tail
// with the division arm first 774 [491, 272]; the add loop's minimum as a 1 default replaced by an if 774 (2816 B); the imbalance
// zeroed before the team block instead of in its else 779 (2812 B) [555, 351]; the inner add-mask test without its `type == 3 ||`
// (never true inside `type == 2 || type == 4`) 902 (2764 B: the smaller body moves the link's inline choices, so the test stays).
// Re-measured: the type-5 machine scan returning false from inside the loop 893 (2764 B, the listed `break` form's numbers).
// m3 lane 3 r233 (fast, control 766 = full; register-masked aligned 343). Diagnostic (compiler setting, reverted at once):
// this file without /arch:SSE plus the player loop through a call to 0x63220 1097 at 2524 bytes, the same as with /arch:SSE:
// 0x63220 still stays a call, so the SSE mismatch between this file and 0x63220's is not what keeps it out of line (lane 14
// r55 already found /arch:SSE on 0x63220's file changes nothing). Forms: the balance call's party flag passed as the
// expression, no `use_parties` local, 858 (2812 B); the two identical team-type tests merged into one block 909 (2756 B);
// the rated extrema as `if (rating > max) max = rating;` statements 818 (2812 B); the party spread and team tests joined
// into one `||` test 907 (2764 B); `available` computed just ahead of its first use 821. Body unchanged.
// m3 lane 16 r109 (fast, 0x73530 8 throughout unless noted): the write-only machine-id copy dropped 937 (2700 B); the capacity
// test through the existing 0x193300 766 (same code), with a forceinline redeclaration 766; that with the top lookup after
// the capacity read 942 (2732 B, 0x73530 1); 0x6fa60's ratings buffer without `__declspec(align(8))` 766. Body unchanged.
// Round 76 (m3 lane 2, fast, control 766 / 0x73530 8; all restored): output cleared first 815; the top lookup inside the
// `||` validity test 954 (2700 B; 0x73530 1, session in ECX); the loop's lookup through 0x192e60 at the loop body's top level
// 930 (2716 B; 0x73530 matches: the inliner gives its one copy of 0x192e60 to the hotter loop site, so the top call stays out
// of line as in retail, whose copy sits inside the inlined 0x63220); the variant index by an explicit state test 766; the
// validity test split in two 766. Diagnostic (flags, not kept): this file without /arch:SSE 836, the top call still inlines.
// m3 lane 14 r89 (fast with 0x73530, 8 throughout; control 766, aligned 750, masked 348): the party loop's `last = count;`
// ahead of the `first == NONE` test 766 (one row differs; masked 350); `out->player_count` stored ahead of the three NONE
// defaults 851 (2804 B); the player loop's second-rating copy ahead of the first 766 (masked 353); the group loop's
// identity copy ahead of the three counter clears 766 (aligned 747, masked 340); the add loop leaving with `if (count >
// available) break;` 766 (same code). The inline choices above stay the cause.
// MiniMax A (11-13): no new form, no lower score (766): the first difference is still the stack ingress and the capacity switch, which retail expands as a jump table and this link keeps as a call, and the player loop, which retail inlines (0x63220 with its 0x192e60 call) and this link does not; the register homes follow from that choice.
// Round 81 (m3 lane 15, fast with 0x73530, 0x63220, 0x192e60, 0x1934f0, 0x1931a0, 0x631f0, 0x6e910 and the accessors, all unchanged;
// control 766, aligned 515). New cause: this link's inline choices follow each call site's static frequency, and every early exit
// ahead of a site halves it. Measured on this body: a small same-file helper called from the player loop inlines only when nothing
// in it would inline in turn (a trivial body, or one call to 0x1931d0 or 0x193250: 1109/1096/1098); with a four-test body, or one
// call to 0x1931a0, 0x1932c0, 0x193300, 0x1933d0, 0x193440 or 0x1934d0, it stays a call (1097 each), and so does the real 0x63220
// (1097; plain or `inline` same-file copy of its shape 1079, forced 1033 with its four calls left out of line). That helper called
// ahead of the opening test inlines (948), after the player loop it does not (868). The opening test cut to n cheap early exits
// (diagnostic): n = 0 or 1 inlines the real 0x63220 in the loop together with its 0x192e60, 0x631f0 and both 0x1931a0 (889/918),
// n = 2 or 3 does not (1027/976); in the written-out body n <= 3 inlines 0x1931a0 and 0x193250 (958), n = 4, 5, 6 do not
// (932/778/830). So retail's body cannot sit behind early returns, and retail's two failure exits do store false to the result
// byte and reload it (0x7e2ea, 0x7ea9a): `result = false;` joined to the one `return result;`. Written so (the opening test and the
// excess test set result = false, the type-5 scan under `result &&`, the rest under `if (result)`): 984 at 2948 bytes, 0x1931a0
// and 0x193250 inline; with the player value through the real 0x63220 947 (2920): it inlines with all four nested callees; with
// this file's copies replaced by the real 0x193300, 0x193440, 0x1933d0, 0x1933a0, 0x1934d0, 0x193400, 0x1934b0 and 0x193470, which
// then inline too, 900 (2896 bytes, aligned 566). That form still inlines the top 0x192e60, the second 0x1931a0 and 0x193250 where
// retail calls them, and calls 0x193300. The opening test split into `if (!summary || ... || !variant) result = false;` and
// `if (result && (capacity > 0x193300 || capacity < 0x1932c0)) result = false;` inlines both lookups (1034, 2984 bytes); as an
// else-if 900; with `!variant` moved to the second test 986. Diagnostic: the whole body under a constant-true test (every site
// halved once more) with that split gives retail's call set but for 0x631f0 (called here, inlined in retail) and 0x193250 (inlined
// here, called in retail): 843 at 2816 bytes, 0x73530 MATCH. Here 0x631f0 and the second 0x1931a0 always flip together, so one of
// them weighs differently in retail. No budget effect: 16 extra accessor expansions change no other choice. On the early-return
// body the loop lookup without its 0x1934f0 call 862, through 0x192e60 857, with no validity test 892; positive nesting or the
// failure arm first for the opening test alone 815 each (the base's inline choices). Compiler-setting probe (reverted): the 900 form
// without /arch:SSE 912, the same inline choices. Every form raises the score, so the body stays at 766;
// build/r81/lead_7e210_rejsplit1.patch and build/r81/lead_7e210_w1split1.patch hold the 1034 and 843 forms.
// P474: Standard player/machine/group identity copies forward 851/821/892;
// backward 851/868/892; full-byte equality/mismatch 889/888. NOT kept:
// regressions; frame/session/initial lookup remain. Original body restored.
// P481: Direct failure completion to the sole result return, bool/byte/word/long,
// original/entry true default: 785/785/808/808/801/825/803/826 (control 766).
// NOT kept: frame/ingress/lookup still differ and extents regress; original
// early returns, result, helpers and complete executable body restored.
// P484: Actual player-summary maximum/minimum/total/count scalar reductions
// 824/808/878/890; paired extrema 867, total/count 890, first three 865, all four 858.
// NOT kept: native per-player publications require equality; frames/extent/ingress
// still differ. Original field reductions, selectors, observations and full body restored.
// M3 lane 13 r91 (fast with 0x73530 (8 throughout), 0x63220, 0x192e60 and 0x6e910, all unchanged; control 766): the capacity
// helper's labels grouped with the loop's lookup through 0x192e60 at the loop body's top level 880 (2780 B: the capacity
// switch inlines, but the top call inlines again and the loop call stays out of line); round 81's single-exit form (the
// opening test and the excess test set `result = false`, the type-5 scan under `result &&`, the rest under `if (result)`)
// with the player value through 0x63220 and grouped capacity labels 998 (2952 B: 0x63220 inlines with all four nested
// callees, the top 0x192e60, both 0x1931a0 and 0x193250 inline too); the single-exit form with grouped labels and the
// written-out loop 1016 (2984 B); the 998 form with the matched accessors 0x193440/0x1933d0/0x1933a0/0x1934d0/0x193400/
// 0x1934b0/0x193470 for this file's copies (capacity kept as the grouped file helper) 956 (2928 B); that with the scan and
// the body nested as `if (result) { scan; if (result) { body } }` 920 (2928 B: the second 0x1931a0 now stays a call as in
// retail; the top 0x192e60 and 0x193250 still inline). Body unchanged.
// M3 lane 3 r242 (fast with 0x73530, 8 throughout; base 766): the player loop bounded by the summary's player count 845 (2812
// B); `result` declared after the validity exit 766 (same code); the group loop bounded by the summary's machine count 783;
// `first` declared before `last` 766 (same code); `available` computed from out's player count after the out stores 879 (2764
// B). Body unchanged.
// M3 lane 1 r104 (fast with 0x73530, 8 throughout): the output memset below the admission test 815 (2812 B); the type-5
// scan's exit as `return false;` 766, the machine-user compare as `memcmp(...) == 0` 766, the average as an explicit
// division 766 (same code each); the player loop's variant lookup hoisted above the loop 846 (2796 B). Re-measured: a
// `long` histogram 844 (frame 0xb4; the session still arrives on the stack in EBP).
// P501: Loop-owned rejection in machine validation, group maximum, rated-player reduction, party count, group mask
// and final admission, then complete graphs: 766/766/766/766/766/766/766/844. NOT kept: Seven graphs tie 766 and
// leave the ingress/frame/lookup unchanged; complete native-width table regresses to 844 and shortens away from
// retail. Every loop and table restored. Original complete executable body restored.
// M3 lane 3 r245 (fast with 0x73530, 8 throughout; base 766, ours `ret 8`/frame 0xd4, retail session in EAX/`ret 4`/0xb4):
// `result` declared first 766 (same code); the first histogram zeroing through memset 820 (2812 B); `spread`/`parties`
// ahead of `last`/`first` 766 (same code); the excess exit as `result = false;` 789 (2844 B); `adjusted` started from
// `rating` 811. Body unchanged.
// M3 lane 14 r97 (fast with 0x73530, 0x192e60, 0x63220; control 766): the loop's variant through 0x192e60 after the rating
// reads plus `long histogram[16]` 918 (2736 B; frame 0xb4 and the session in EAX as retail, 0x73530 matches, but the top call
// still inlines and the loop call and capacity switch stay out of line); with the capacity switch forceinline 868 (2788 B,
// 0x73530 8); with grouped capacity labels 888 (2768 B, 0x73530 8); the player value through 0x63220 plus the long histogram
// 1141 (2476 B); the 0x192e60 call first in the loop body plus the long histogram 937 (2704 B, frame 0xb0). Body unchanged.
// M3 lane 2 r86 (fast with 0x73530, 0x63220, 0x192e60, 0x1934f0, 0x1931a0; control 766, 0x73530 8), all with `long
// histogram[16]` (frame 0xb4 as retail): the top lookup after the capacity read 874 (2772 B, 0x73530 1); that with the
// loop's variant through 0x192e60 922 (2724 B, 0x73530 1); the top lookup after the summary read with the loop's variant
// through 0x192e60 908 (2724 B); the validity test split in two with the loop's variant through 0x192e60 901 (2740 B);
// the type-5 scan's early return dropped and the rest of the body under `if (result)` 862 (2788 B).
// Round 123 (m3 lane 16, fast with 0x73530 (8 throughout), 0x63220, 0x192e60, 0x1934f0, 0x1931a0 and 0x6fa60, all unchanged):
// the balance result as `if (!balance_teams_by_count(...)) result = false;` (retail's test/jne/store-al shape) 777 (2812 B);
// round 81's single-exit form rebuilt as control 984 (2948 B: retail's two failure exits store false to the result's home
// and reload it, a shared `return result;` duplicated into each exit); on it that conditional clear 981; plus the player value
// through 0x63220 924 (2916 B: 0x63220 inlines here, the top 0x192e60 and 0x193250 still inline, the capacity switch stays a
// call); plus grouped capacity labels 1001 (2952 B); the 924 form with `long histogram[16]` 901 (2888 B, frame 0xb4 as
// retail). Re-measured: the opening test as one inline admission predicate 948 (2956 B, M370's form). Body unchanged.
// MiniMax E (11-15): no new form, no lower score (766): retail's session arrives in EAX with a 0xb4 frame against our
// 0xd4, and that convention is fixed by the caller in a file I may not edit.
// m3 lane 5 r188 (fast with 0x73530, 0x192e60, 0x63220, 0x631f0, 0x6fa60, 0x1931a0, 0x1934f0, all matched throughout but
// 0x73530; control 766, 0x73530 8): the write-only machine-id copy declared inside the group loop 766 (same code, frame
// still 0xd4); the player loop's group-highest scan through the existing 0x631f0 (retail inlines it there) 964 at 2684
// bytes, 0x73530 MATCH (the session arrives in EAX, but the top 0x192e60 still inlines and 0x631f0 stays a call; not kept:
// a rise here; diff saved as build/r188/lead-0x73530-631f0-call.patch; applied by the coordinator 11-16, see the 964 line); that with `long histogram[16]` 984 (2640 bytes,
// frame 0xb4, 0x73530 MATCH); that with the loop's variant through 0x192e60 1011 (2620 bytes, 0x73530 MATCH, the top call
// still inlined); the player's group named before the written-out scan 766 (same code). Body unchanged.
// m3 lane 8 r176 (fast with 0x73530, 0x192e60, 0x631f0, 0x193300, 0x1932c0; control 964): the capacity copy forceinline 866
// (2800 B) but 0x73530 8 (the session on the stack, `ret 8`: the inlined top 0x192e60 takes EDI, so the session is reloaded
// from its slot); with `long histogram[16]` 894; the single exit 984 (2948 B); forceinline with `result` first 866; with
// the loop's variant through 0x192e60 957; all with 0x73530 at 8. Kept: the capacity test through the matched 0x193300 in
// place of this file's copy of its body, 964 (same code). Lead: the top 0x192e60 out of line with the capacity switch inline.
// M3 lane 7 r172 (fast with 0x71730/0x73530/0x6fa60/0x192e60/0x193300/0x631f0/0x63220; control 964, aligned 862; ours
// already returns with `ret 4`, the session in EAX). Behaviour: retail's second 0x1931a0 call in the player loop takes the
// group's highest rating (ECX, 0x7e4a9; it is 0x63220's MAX evaluating 0x1931a0(highest) twice), where this body passes
// `lower`, the first evaluation's result; the two differ when lower <= field_51c[lower - 1] < highest. Passing `highest`
// 966 (the extra `mov ecx, eax`: our called 0x631f0 returns the highest in EAX, retail's inline copy keeps it in ECX); the
// clamp as `adjusted > f(highest) ? adjusted : f(highest)` 990 (2652 B, both calls out of line). Re-measured: `long
// histogram[16]` 984 (2640 B, frame 0xb4). New: the top 0x192e60 after the summary and capacity reads 966 (0x73530 1).
// LEAD (loses 0x73530, so not kept; build/r172/lead-0x7e210-t7.patch in the m3-lane7 worktree): the single exit (the
// opening and excess tests set `result = false`, the rest under `if (result)`), the player value through the real 0x63220
// and `long histogram[16]` 924 (2892 B, frame 0xb4; 0x63220 inlines with its nested 0x192e60/0x631f0/0x1931a0 as retail);
// with the opening test split into an else-if 924 (same code); with the type-5 scan inside `if (result)` and the body under
// a second `if (result)` 914 (2900 B, aligned 763). In all three the top 0x192e60 still inlines (its pointer takes EDI) and
// `out` takes EBX, so the session goes to EBP and stays a stack argument: 0x73530 8, 0x71730 387 -> 402.
// M3 lane 13 r104 (fast with 0x73530, 0x192e60, 0x631f0, 0x63220, 0x193300, 0x1932c0, 0x1931a0, all matched throughout;
// control 964, both `ret 4`): the clamp as retail's two-evaluation max of 0x1931a0(highest) in place of the hand loop 990
// (2652 B); that with the loop's variant through 0x192e60 1032 (2588 B); the clamp's call passing `highest` 966 (re-measured;
// retail's ECX holds the group's highest rating at that call, so `lower` is a behaviour difference left for the reviewer);
// that with the loop's variant through 0x192e60 1011 (2620 B: the loop's lookup stays a call and the top one still
// inlines, the reverse of retail); the clamp as `adjusted > lower ? adjusted : 0x1931a0(highest)` 968.
// M3 lane 11 r112 (fast with 0x73530, 0x71730, 0x6fa60, 0x192e60, 0x631f0, 0x63220, 0x193300, 0x1932c0; plain confirmed
// 942; 0x73530 MATCH and 0x71730 387 throughout). Kept: the excess exit as `result = false;` (the existing `if (result
// && ...)` guards the rest; retail's exit at 0x7ea98 stores false into the result's stack byte and reloads it, which this
// form reproduces) 964 -> 942 (2716 B); its direct form `result &= excess <= available;` 954 (2700 B). On 964: round 81's
// single exit (opening test too, rest under `if (result)`) 984 (2948 B, 0x73530 8); that with `long histogram[16]` and
// memset clears 935 (2908 B, 0x73530 8); `long histogram[16]` with memset clears 964 -> 995 (2636 B). On 942: that long
// histogram 975 (2656 B); the opening exit as `{ result = false; return result; }` 942 (same code); the rest under
// `if (result)` after the type-5 scan (opening return kept) 951 (2704 B); the type-5 exit as `return false;` 942 (same
// code); `adding_group` declared beside `result` 889 (2764 B; a moved declaration, so match-only); the balance result as
// `if (!balance_teams_by_count(...)) result = false;` 902 (2764 B: retail's exact `test al, al; jne; mov byte ptr [esp +
// 0x13], al` after 0x91e30, but its direct form is main's `result &=`, so match-only); both 890 (2768 B). The same
// conditional clear with the excess exit left as `return false` 917 (2736 B).
// Deep round 410: existing inline upper-capacity selector 852; selector after summary admission
// 852; validated count through remaining capacity 942; selector and validated count 900;
// separate capacity rejection branches 942; const index and variant pointer 942; result
// initialized after admission 901; late result and inline selector 846. NOT kept: inlining the
// existing upper-capacity selector reaches 852, and with the later result 846. Both forms stack
// the session and lose the 0x73530 caller in preview. The later result alone reaches 901 with
// the caller still exact, but initializes the result after validation where retail stores it
// before the output clear. The 0xb4 workspace and out-of-line opening lookup still differ.
// Round 455: overlaid native workspace, both memset clears 975, overlaid native workspace, first range
// clear 960, overlaid native workspace, second range clear 970, overlaid native workspace, both range
// clears 959, overlaid native workspace, first counted clear 971, overlaid native workspace, second counted
// clear 969, overlaid native workspace, both counted clears 959, Independent native workspace, both range
// clears 959. The native-width forms recover frame 0xb4 and shorten the body to 2656–2688 bytes against
// retail's 2828. The opening lookup still expands. All scores rise. Restored the original machine-ID and
// histogram storage and call sites.
// Both histogram passes with a group-size pointer and signed remaining count 871 kept (plain). Identity
// remaining count/end pointer 942/941; all-group/eligible-party word passes 947/937; all-group/eligible-
// party/both dword passes 969/997/936. NOT kept: 941, 937 and 936 each gain fewer than eight.
// Round 418: Outer source-ID/source-identity/destination-identity cursors and their pairs
// 877/902/871/900/870/873, all three 873, all with dword histogram 918. NOT kept: the one-point lead leaves the
// entry unchanged, and the native-width table allocates 0xb0 and shortens the body.
// Round 605. The 0xd0 frame still exceeds 0xb4 and the entry variant lookup still expands. Per-group total,
// rated-count and average pointers 871/871/878. All three 852 (NOT kept: only the combination gains and retail
// advances the counters at a shared stride). Player group, rating and identity cursors 916/931/902. Machine ID
// destination cursor 969.
// shared histogram group count 871; shared group count from summary phase 879; count snapshot with
// native histogram 936; group sizes and count for histograms 871; native histogram with shared size
// source 936; scoped ids with native width and shared count 936; native histogram bucket addresses
// 936; native buckets with shared group count 936. NOT kept because the equal-width controls tie and
// the dword forms shorten the body and raise its score.
// Paired machine ID cursors, paired group identity cursors, both, and machine ID destination ordinal:
// 869/873/881/935. The same forms with dword histogram cells: 931/917/921/996. NOT kept: the two-point
// gain is below threshold, and the 0xb4-frame cursor ordinal shortens the body to 2636 bytes.
// Separate machine success byte 948. Separate invalid-machine byte 947.
// Grouped summary and capacity guards 871. Result from initial admission 1014.
// Admission failure through result 871. Chained group-mask clear 871.
// Group decision from admission tests 899. Group decision inside party mode 896.
// Separate machine-validation bytes shorten the body to 2704 bytes. Initializing the result from admission
// grows it to 2952 bytes and changes caller 0x73530 from matched to 8 in preview. The other forms tie or
// regress. The opening call and the ID/histogram frame layout still differ.
// The separate histogram keeps the frame at 0xd0, and the opening variant lookup still expands.
// Group range ends for all/eligible passes 886/886; skip other groups 871;
// per-group player-count snapshot 957; spread from count before last update 907;
// conditional first accepted size 871; counters cleared before ID copy 871;
// two-field session ID copy 867; admitted rating before flags 910; one team setup gate 908.
// NOT kept: the two-field ID copy gains four and shortens the body to 2748 bytes.
// Admitted player count for availability or also output count 947/908; mode before the minimum or both capacity tests 883/883.
// Early mode with carried count 927/899; named capacity bounds with carried count 908/927.
// NOT kept: frame and opening lookup still differ, and every form scores higher.
// native network id indexed copy 871; native network id cursor 969; opaque 64-bit id indexed copy 871; opaque
// 64-bit id cursor 969; native network id indexed copy with dword histogram 911; native network id cursor
// with dword histogram 1013; opaque 64-bit id indexed copy with dword histogram 911; opaque 64-bit id cursor
// with dword histogram 1013. The indexed copies with word buckets tie. Indexed copies with dword buckets
// reserve 0xb0 and score 911 at 2748 bytes. The dword cursor forms recover 0xb4 but shorten to 2612 bytes.
// All four cursor forms lose the matched 0x73530 caller in reduced checks. Opening executable source
// restored.
// Tried rating-loop identity cursors 946; first-rating cursors 906; identity and first-rating cursors 910; group
// and adjusted output cursors 907; identity and group outputs 909; first ratings and group outputs 929; all
// rating-pass cursors 916; all rating-pass cursors with dword histogram 947.
// NOT kept: The mixed cursor forms grow the frame and shorten the body. Dword histogram with all cursors reaches
// 0xc0 but still expands the opening lookup and raises the score.
// One indexed signed/unsigned dword ID and histogram workspace 959/959; advancing workspace
// 948; entry-sized library copy 936; paired ID cursors 968; ID snapshot 963; ID copy after
// counter clears 950; workspace scoped through party counts 959. NOT kept: every form moves
// the body farther from retail's 2828 bytes and scores higher. The opening lookup still expands.
// M3 lane 11 r119 (fast with 0x73530 (MATCH throughout unless noted) and 0x71730 (387); masked = aligned count with
// registers and stack offsets masked, base 416). Retail's long histogram shares the machine-id copy's storage (both at
// +0x34 of the frame, cleared with `rep stosd` of 16 dwords), so retail's frame has no separate histogram. Diagnostic: the
// top lookup guarded (`variant_index != NONE ? function_192e60(variant_index) : NULL`) 886 (2732 B, masked 370): one more
// test ahead of the call keeps 0x192e60 out of line as retail (round 146's finding again). Tried: the first histogram
// clear through memset 892 (2764 B); `long player_count` named for `available` and the output count 908 (0x73530 8).
// Lead: retail's inline party-bound code zeroes the result first and compares the flag byte in memory (`xor edx, edx;
// cmp ecx, 5; ...; cmp byte ptr [ebp + 0x604], 0`), the shape of the matched 0x1933d0/0x1933a0; the file's minimum,
// maximum and spread selectors rewritten that way (`long result = 0; if (...) result = ...; else if (...) result =
// ...; return result;`) give that code exactly, 894 (2728 B, masked 357); with the team-count and imbalance selectors
// in 0x193440/0x193400's default-first form too 916 (2724 B, masked 348). Not kept (the body shrinks, so the score
// rises), but it is retail's code: keep it once the inline choices (top lookup a call, capacity switch and 0x631f0
// inline) give back the length.
// Machine ID dwords split around group clears, after identity, or between the clears
// 867/929/867/867. Whole-ID block copy before group statistics 978. Separate ID loop before
// group statistics or before histogram 914/914. Split store with dword histogram 933.
// NOT kept: the four-point leads shorten the body to 2748 bytes, and the other forms regress.
// The opening variant lookup still expands and the frame still differs.
// Typed shared machine-ID and histogram storage (long/word cells): one shared histogram 936/892,
// separate overlapping histograms 936/892, first histogram overlapping 931/894, and second
// histogram overlapping 911/871. NOT kept: the full overlap reserves 0xb0 instead of 0xb4,
// the first variant lookup still expands, and no form matches.
// M3 lane 2 r108 (fast with 0x73530; control 871): `long histogram[16];` plus a memset at its declaration (retail's
// dword cells: 16-dword rep stosd at 0x7e66c/0x7e6a5, 4-byte increments) 936 (2716 B, frame 0xb0); `long histogram[16]
// = {0};` 911 (2748 B); the machine-ID copy loop in its own block 871 (same code); that block with the memset dword
// histogram 936; the top variant lookup moved after the summary and capacity 870 (2784 B; 0x73530 MATCH -> 1, not kept).
// Reading: retail calls the top 0x192e60 at frequency one (unconditional) and expands the colder 0x193300 and the loop's
// lookup, so the difference is the inliner's site choice, not a statement form here.
// M3 lane 7 r185 (fast with 0x73530, 0x71730, the team-balance callees and 0x192e60's/0x193300's file-mates; control 871,
// aligned 1016; both builds return with `ret 4`): re-measured on today's body, the loop's written-out lookup as a call
// to 0x192e60 1007 (2620 B, aligned 986), with a `__forceinline` declaration of 0x193300 here 1007 (the same code: the
// declaration does not expand it); the type-5 block's `if (!result) return result;` moved after the block 871 (same
// code); `inline` on 0x193300's definition 872 (2796 B, aligned 921: the capacity switch is then expanded here as in
// retail, but 0x705f0 and 0x72330 expand it too and lose their matches, 3 and 49), with the loop lookup through
// 0x192e60 997 (2636 B, aligned 941). Retail expands 0x193300 only at this frequency-1 site and calls it under the
// deeper tests of 0x705f0/0x72330, so the split is the link's inline threshold, not a source form.
// Inner lookup candidate held for validation, selected from validation, or cleared on failure
// 876/885/885. With the shared availability test 875/905/903, direct pointer publication with the
// shared test 885, complete nullable expression 871. NOT kept: the expression ties, and the other
// forms regress. The opening lookup still expands and no trial reaches the retail frame.
// Pointer/reference from the output clear 926/926, after machine admission 871/871.
// With dword buckets, clear-result forms 938/938 and admitted receivers 911/911.
// NOT kept: the clear-result dword forms recover frame 0xb4 but shorten the body
// and lose the caller in preview. The opening lookup still expands.
// M3 lane 12 r152 (fast with 0x73530, MATCH each time; control 871, both builds `ret 4`): the admission test as two `if`s
// (summary, count and variant, then the two capacity calls) 871; `bool result = true;` declared first 871; the variant
// declared, then assigned 871; 0x193300's result named for the second test 871 (all the same code); diagnostic, the output
// memset after the admission test 934 (2716 B).
// common countdown 871; common group size cursor 871; common countdown and cursor 871; dword cells with common
// countdown 911; dword cells with common cursor 911; dword cells with common countdown and cursor 911; dword
// cells with common traversal and two memset clears 936; scoped dword cells with common traversal and two
// memset clears 987.
// NOT kept: The shared word-cell traversal ties. Dword cells reserve 0xb0 and shorten the body. The scoped
// dword form reaches 0xb4, but changes the session ingress to the stack and shortens to 2652 bytes. All nine
// checked peers retain their opening measurements.
// Variant/summary names, index/capacity names and two complete entry name orders,
// each with word buckets 871/871/871/871 and dword buckets 911/911/911/911.
// NOT kept: word forms tie. Dword forms reserve 0xb0, shorten to 2748 bytes and
// raise the score. The opening lookup still expands.
// First population count before clear 871, population count shared across scans 871, indexed scans with shared
// count 885, dword cells with shared count 911, dword memset clears with shared count 936, machine IDs scoped
// before dword cells 911, separate dword histogram scopes with shared count 917, shared group-size base with
// dword cells 911. NOT kept: Changes to count lifetime tie or shorten the body. Dword cells bring the frame to
// 0xb0 and the scores to 911 or higher. The opening lookup still expands.
// M3 lane 16 r152 (fast with this file's functions, 0x193300's file, 0x73530, 0x71730, 0x6fa60, 0x63220, 0x631f0,
// 0x72330, 0x705f0 and 0x73360; control 871 with 63 others matched; plain confirmed). Kept: the clamp's call passing the
// group's highest rating, `function_1931a0(highest, rating_variant)`, 871 at 2780 bytes (one `mov ecx, eax` more, the
// same count): retail's call at 0x7e4a9 has that rating in ECX and the matched 0x63220, whose body this loop is, passes
// it too, so the earlier `lower` argument was a behaviour difference (lane 7 r172's reading), not a form. Tried: the
// balance result as `if (!balance_teams_by_count(...)) result = false;` 905 (2752 B); grouped case labels in 0x193300's
// own selector 925 (2896 B: the selector then expands here as retail's table, but 0x193300 itself shrinks to 26 bytes
// against retail's 52 and 0x72330, 0x73530, 0x1931d0 and 0x193250 change with it, so retail's selector has its five
// separate cases); 0x193250 with the upper-place selector written in both of its cases instead of the shared exit 868
// (2848 B: 0x193250 shrinks to 72 bytes against retail's 100 and this function then expands it, so not kept); the add
// loop's minimum as retail's three-way (`type <= 3 ? 1 : type == 4 ? field_5fc : height`, the arms behind retail's `cmp
// edx, 3` at 0x7eaea and `cmp edx, edx` at 0x7eaef) 872 (2784 B, register-masked aligned 269 against the body's 267).
// Complete positive admission, nested summary/capacity scopes, summary rejection with positive
// capacity, positive summary with capacity failure, or missing-summary rejection 871 each.
// Common admission result 886, positive machine-validation region 894, positive admission with
// dword histogram 911. NOT kept: five forms tie. The dword form reserves 0xb0 and is shorter.
// The opening lookup still expands and the upper-capacity selector remains a call.
// M3 lane 13 r126 (fast with 0x73530, 0x63220, 0x192e60, 0x193300, 0x1932c0, 0x631f0; control 871 at 2780 B, session in EAX
// and `ret 4` as retail, frame 0xd0 for 0xb4, 0x73530 MATCH): the player loop's value through the matched 0x63220 1012
// (2608 B, still called); the opening test as four separate early returns 871 (same code); the output cleared as
// `*out = s_matchmaking_ratings();` 862 (2800 B, but through a 0x364-byte temporary copied with `rep movsd`, frame 0x434,
// aligned 1020 against 1016: not kept); the player loop bounded by the summary's count 883 (0x73530 8); adding_group
// declared beside result 881.
// M3 lane 12 r159 (fast with 0x73530; control 871 at 2780 B, 0x73530 MATCH): the player value through the matched 0x63220
// re-measured 1012 (2608 B, 0x63220 stays a call); the single exit (opening test sets `result = false`, the type-5 scan and
// the rest under `if (result)`) with the player value through 0x63220 and `long histogram[16]` 844 (2852 B, frame 0xb4;
// 0x63220 inlines with its nested lookup, 0x631f0 and first 0x1931a0, but the top 0x192e60 inlines too and 0x73530 drops to
// 8); that with the top lookup after the summary and capacity reads 851 at retail's 2828 B (0x73530 1: the session then
// arrives in ECX, since no call sits at the entry); that with the capacity bounds in an `else if` 851 (same code); the
// original body with the loop's variant through 0x192e60 at the loop body's top level 949 (2700 B: the loop call stays out
// of line, the top inlines, the reverse of lane 2 r76's result on the older body). Diagnostics (restored at once): this
// file without /arch:SSE plus the 0x63220 call 1013 (no change in the inline set); the disabled 0x6d4c0 draft linked in
// place of its stub (with 0x6e910 listed so its file is in the image) 871 (no change). Retail's EAX session follows from
// the top call staying out of line (EAX carries its argument); every form above that expands it moves the session.
// Shared upper selector 865; split capacity gates 865, individual admission gates 865,
// named upper result 865. Dword cells 876, with both library clears 903, split capacity
// gates 876 and type retained across the lower check 876.
// NOT kept: the selector expands but the opening lookup still expands, and all eight
// move the session onto the stack and disturb the matched caller. The dword frame is 0xb0.
// Player-loop lookup nested at the range, population or enablement checks, at each pair, at all three, or at
// separate signed range bounds 871 each. The frame and both lookup boundaries remain unchanged.
// Early signed dword histogram declaration 936; adjacent ID and signed/unsigned histogram scopes
// 987/987; adjacent signed scopes with range fill for the first, second, or both clears 966/994/971,
// and counted fills 993; two independently cleared dword histogram scopes 936. NOT kept: adjacent
// scopes recover 0xb4 with the session still in EAX and shorten the body to 2652–2668 bytes. The
// opening lookup still expands.
// Dword bucket updates by explicit first/second/both assignments 911/911/911. Named first/second/both
// bucket indices 928/911/928. First or both bucket addresses 911/911. NOT kept: all shorten the
// body and increase the score. The frame reaches 0xb0 and the first variant lookup still expands.
// Histogram pointer endpoints for all groups, eligible parties, both with separate counts,
// or both with a shared count, word buckets 886/886/845/927, dword buckets 919/915/916/929.
// NOT kept: the lower word score replaces two retail signed countdowns with pointer tests.
// The opening lookup still expands and the upper-capacity selector still calls.
// M3 lane 2 r122 (fast with 0x73530, 0x192e60, 0x192db0, 0x63220, 0x66050 and 0x1934f0; control 871; body unchanged). The top
// 0x192e60 call is not this body's choice: (a) cut down to three lines (the index accessor, the call, a store and a return) this
// file still expands the lookup at the top, as do cuts after the type-5 scan (274 B), the player loop, the group loop and the
// party block; (b) the matched 0x73530 keeps the same depth-0 call out of line only inside its `#pragma inline_depth(0)` region:
// the same three-line body placed in that file outside the region expands it too; (c) seven spellings of 0x192e60 itself (calling
// 0x192db0, the tests written out, nested ifs, a named element pointer, early returns, a named bool, the current one) all leave
// the top expanded, three of them losing 0x192e60's own match; (d) one caller of this function removed (0x6fa60's dropped call)
// changes nothing; (e) flag diagnostic (reverted): the callee's file built without its SSE flag changes nothing here, in
// 0x73530, 0x63220 or 0x66050. So this link expands 0x192e60 at every depth-0 call site, where retail keeps it out of line at
// depth 0 in three functions (here, 0x73530, 0x66050) and expands it only inside this function's player loop: a link-wide
// inline choice, not a source form of this body or of the callee.
// M3 lane 11 r138 (fast with 0x73530; control 871 at 2780 B, session in EAX and `ret 4` as retail): the capacity
// test through an inline copy of 0x193300's five-case switch (retail expands that switch here: table at 0x7ec90) 865
// (2836 B, aligned 813, masked 465; the session then takes a stack home and 0x73530 drops to 8); that with `long
// histogram[16]` and a memset ahead of the first count 903 (frame 0xb0, 0x73530 8); the dword histogram alone 936;
// the opening test setting `result = false` with the rest under `if (result)` 1024 (2968 B: the loop then expands
// 0x631f0, both 0x1931a0 and 0x193250, 0x73530 8), with the switch 1117; the switch plus the loop lookup through
// 0x192e60 903; the loop lookup through 0x192e60 alone 1013. Diagnostics (restored at once): the switch with the top
// 0x192e60 kept out of line by a constant test 903 (aligned 798, masked 438, 0x73530 MATCH): the session keeps EAX
// once the top lookup is a call, and the call set then differs from retail only in 0x631f0 (called here, expanded in
// retail). Loop frequency decides the loop's set: the player value through 0x63220 stays a call with the opening
// test as returns (also with two of its five conditions) and expands with everything else when the test is a value
// or sets `result`; a test at 0x71730's call changes nothing.
// MiniMax B (11-20): no new form, no lower score (871): the frame is 0xd0 against retail's 0xb4 and retail calls 0x192e60 out of line where ours expands it; the histogram as a long array reads 911 and as a memset read 936, both worse than the kept body.
// M3 lane 16 r166 (fast with 0x73530, 0x63220 and 0x192e60, all MATCH unless noted; control 871 at 2780 B, aligned 1016).
// Retail's own 0x63220 calls 0x192e60 under its one test and retail's loop here has the lookup expanded (0x63220's body
// brought in), so retail expands that lookup only at loop depth. The top lookup through a file-local inline wrapper
// 871, forced inline 871 (same code: the wrapper folds and the lookup is still expanded); through an inline helper that
// reads the index into an out pointer 1009 (2624 B: the helper itself stays a call); the lookup's argument read from
// the session again 875 (2796 B); the lookup after the summary read only 858 (2796 B, 0x73530 lost, 8). Body unchanged.
// all-group read advance 871, all-group read advance with dword buckets 911, eligible-party read advance
// 870, eligible-party read advance with dword buckets 911, both reads advance 870, both reads advance
// with dword buckets 911, eligible-party reject after advance 870, eligible-party reject after advance
// with dword buckets 911.
// NOT kept: The 870 forms gain one point. Dword cells cut the frame to 0xb0 but shorten the body to 2748 bytes and raise the score.
// The 0xd0 frame exceeds 0xb4. The opening variant lookup still expands and the upper-capacity selector remains a call.
// M3 lane 2 r129 (h2m fast with 0x73530, 0x192e60, 0x192db0, 0x63220, 0x66050, 0x1934f0, 0x193300 and its three callers;
// control 871 at 2780 B; the session already arrives in EAX with `ret 4` as retail): a folded second test nested inside
// 0x192e60's valid arm (more written depth in the callee) 871 (the top lookup still expands; 0x192e60 still MATCH); the
// forceinline redeclaration of 0x193300 re-measured 871 (the capacity call stays a call); `inline` on 0x193300's definition
// 872 (2796 B, and the matched 0x705f0 and 0x72330 stop matching); diagnostic, an inline_depth(0) region around the top
// lookup statement 994 (2640 B: the top call stays out of line but the region turns off the body's other expansions);
// `result` declared ahead of the getter 871 (same code). BLOCKED on the link-wide inline choice for the top 0x192e60 call.
// M3 lane 13 r139 (fast with 0x63220, 0x73530 and 0x192e60, all MATCH; control 871): the type-5 scan returning false at
// the first match 887 (2768 B); the histogram cleared by memset instead of its initialiser 892 (2764 B); `available`
// computed before the player loop 878; the adjusted value started from `rating` 885 (2768 B); the averages' zero test on
// the element 855 (2796 B, a behaviour change: retail tests the array's address, not kept).
// M3 lane 8 r217 (h2m fast with 0x73530 (MATCH), 0x192e60, 0x631f0, 0x193300, 0x1932c0, 0x63220, 0x1931a0, 0x66050; control 871).
// Diagnostics in 0x192e60's file, restored: 2, 8 and 24 redundant `result = NULL;` statements in 0x192e60 871 each (the same code
// everywhere: the inline choice does not follow the callee's source length); 0x192e60 as nested ifs with a named element pointer
// 853 (2796 B), but 0x192e60 itself 0 -> 20 and 0x73530 MATCH -> 8. Body unchanged; BLOCKED on the link's inline choice.
// Complete machine-ID capture, with both words before clears, split around clears, second after identity,
// or first after total clear 871/867/916/899. Dword histogram with the same publication forms
// 911/933/946/946. NOT kept: the 867 form gains four and shortens to 2748 bytes. Dword cells
// reserve 0xb0 and regress. The opening variant lookup still expands.
// M3 lane 12 r172 (H2M_FAST with 0x73530 and 0x192e60, both MATCH unless noted; control 871): the output memset ahead
// of the session reads 870 (2796 B; 0x73530 drops to 8); the opening test split in two 871; the opening exit as
// `{ result = false; return result; }` 871; the imbalance excess test returning at once (retail's exit at 0x7ea98) 895
// (2752 B); the type-5 scan's exit as `return false` 871. Still BLOCKED on the link's inline choices.
// M3 lane 5 r231 (fast with 0x73530, control 871): the party histogram as `long histogram[16] = {0}` 911 (2748 B), as
// `long histogram[16]` cleared by memset 936 (2716 B); an `inline` declaration of 0x192e60 in this file 871, of 0x193300
// 871; the capacity compares written callee first 871 (same code each). All restored.
// Machine validity via Boolean AND, logical conjunction, conditional preservation, or an
// independent accumulator 878/858/880/915. With dword cells 915/891/916/954.
// NOT kept: conjunction moves the matched caller's session argument to the stack. The
// independent dword accumulator reaches 0xb4 but shortens the body to 2700 bytes.
// Round 463. Captured group count for group construction only, summary field throughout, summary count
// throughout, or published count throughout 884/901/879/879 with word cells. Dword versions
// 921/929/943/943. Dword cells shrink the frame but shorten the body. The lookup and capacity call
// boundaries still differ.
// Rejection byte 890, declared at entry 890 or after admission 904. Accumulated machine,
// party or capacity rejection 885/895/893. Single exit with rejection byte 874, or with
// result initialized after the clear 903. The frame stays above 0xb4 and the opening
// lookup still inlines.
// M3 lane 16 r177 (h2m fast with 0x73530, 0x192e60 and 0x63220, all MATCH unless said; control 871 at 2780 B): the loop's
// variant lookup as a call to the existing 0x192e60 (the top call still expands) 1013 (2620 B: several capacity and rating
// accessors then stay calls); the machine-id array declared at the function top 871 (same code); the histogram declared
// at the top 874; `available` computed after the averages, ahead of the party masks, 862 (2800 B, but 0x73530 loses its
// match, 8); `adding_group` declared with the party locals 882. Still BLOCKED on the link's inline choice for the top
// 0x192e60 call.
// M3 lane 1 r153 (H2M_FAST with 0x73530 and 0x192e60, both MATCH; control 871): the type-5 exit as `return false` 871;
// the opening test as two early returns (summary, count and variant, then the capacity pair) 871; the rated test on
// `rating` for `adjusted` 875; the loop lookup's validity tests and its 0x1934f0 test as nested ifs 871;
// `adding_group` declared beside `result` 881. Retail re-reads the variant type after each call and threads the loop
// lookup's negative-index branch past the `if (variant_index)` test.
// M3 lane 6 r229 (H2M_FAST with 0x73530 and the callees, control 871): the existing noinline declaration on 0x1934f0
// dropped 871 (0x192e60, 0x1934f0, 0x192db0, 0x66050 and 0x73530 MATCH; the top lookup still expands); `inline` on
// 0x631f0's definition 871 and a `__forceinline` redeclaration of it here 871 (same size: the loop's call stays a call);
// the loop's hand-written 0x1931a0 scan as a call to 0x1931a0 986 (2648 B, 0x73530 148); that with `inline` on
// 0x1931a0's definition 962 (0x63220 55, 0x73530 164). Survey: every matched function that keeps a depth-0 0x192e60 call
// out of line does so under a test (0x73ca0) or inside an existing inline_depth(0) region (0x6dcc0, 0x73530, 0x66050,
// 0x90880), so the top call stays the link's choice.
// M3 lane 1 r158 (h2m fast with 0x73530, 0x63220 and 0x192e60, all MATCH unless noted; control 871; re-measures on today's base,
// which rose from the 766 of the earlier rounds): the single-exit form (the opening test sets `result = false`, the type-5
// scan and the rest in the else arm, the rest under `if (result)`) 903 (2748 B; the call set is unchanged: the top lookup
// still expands and 0x193300 and 0x631f0 stay calls); the loop's variant through 0x192e60 at the loop body's top level 949
// (2700 B: the top lookup still expands, so the loop site no longer takes the one inlined copy); `long histogram[16]` 911
// (2748 B, frame 0xb0: the histogram now shares the machine-ID copy's storage, retail's 0xb4 has four bytes more); the party
// minimum/maximum/spread copies in the matched accessors' result-first form 894 (2728 B), with `long histogram[16]` 942
// (2696 B, frame 0xb0); the result flag and the output clear ahead of the session reads 859 (2796 B) but 0x73530 8 (lost,
// and retail clears the output after the reads).
// m3 lane 4 r251 (H2M_FAST with 0x73530 and 0x192e60, both MATCH; control 871): the party histogram's cells `short` 871;
// `use_parties` computed ahead of the team-count block 875; the rated count stepped ahead of the average sum 871;
// `bool result = true;` after the output clear 871; the player loop's group copy ahead of the identity copy 877 (2784 B).
// M3 lane 15 r139 (h2m fast with 0x73530, 0x71730 (376), 0x6fa60, 0x63220, 0x192e60, 0x193300, 0x1932c0, 0x193250,
// 0x1931a0, 0x1931d0 and 0x72330; control 871, aligned 1016): round 81 forms re-measured on this body. Both failure exits
// as `result = false;` joined to the one `return result;` (type-5 scan under `result &&`, the rest under `if (result)`) 988
// (2952 B, aligned 846; 0x73530 8); that with the player value through the existing 0x63220 957 (2920 B, aligned 893);
// that with the file-local ratings accessors replaced by the matched 0x1934d0/0x1933d0/0x1933a0/0x193440/0x193400 and the
// type tests by 0x1934b0/0x193470, 902 (2892 B, aligned 786; 0x73530 8; the top 0x192e60, 0x1931a0 and 0x193250 still
// expand and 0x193300 stays a call). On the early-return body: the player value through 0x63220 1012 (2608 B); with the
// accessors 1103 (2516 B); the accessors alone 917 (2724 B, aligned 785). Body unchanged.
// Guarded opening lookup with lower/upper/full/nested index bounds and word buckets
// 888/888/885/885. The same guards with long buckets 937/937/942/942.
// NOT kept: the guarded call adds a rejection path where retail calls unconditionally.
// Long buckets reduce the frame to 0xb0, short of 0xb4, and all eight scores rise.
// M3 lane 3 r304 (h2m fast with 0x73530, 0x63220 and 0x192e60; control 871, 2780 B, 0x73530 MATCH; body unchanged).
// Reading: the link-time expansions here follow the estimated frequency of each call site, and the estimate is what
// separates retail from ours. Retail's seven calls are the opening 0x192e60, 0x1932c0, 0x1934f0 (inside the lookup
// expanded in the player loop), 0x1931a0 (the maximum's second evaluation; the first is expanded), 0x91e30, 0x193250 and
// 0x91f00. With the player loop written as a call to the existing 0x63220 (declared here): early returns as on main
// 1012 (2608 B: the helper stays a call, because the five-term validity test in front of an early return leaves the
// rest of the function at a thirty-second); without the second early exit (diagnostic) 1087; with no early exit at all
// (diagnostic) 971 (2920 B: the helper and everything in it expand, 0x73530 lost at 8); the validity test clearing the
// result with `if (!result) return result;` after it 977 (2920 B, the same expansions, 0x73530 8), on main's hand-written
// loop 1013 (2952 B); that exit test written two or three times (diagnostic) 948 (2696 B: the helper a call again),
// four times 1028 (2592 B, 0x193250 a call again). So retail's loop sits between one and two halvings. Exit structures in
// that band: the clearing test, then the type-5 scan in its else arm, then one exit test: 964 (2908 B); the clearing
// test, an exit test, the type-5 scan, an exit test outside the scan's block: 964 (2908 B); the scan guarded by the
// result instead 901 with the guard below. In all of them the opening lookup still expands (retail calls it) until it
// sits behind one more two-way test: with the diagnostic `if (result) variant = function_192e60(variant_index);` (always
// true, no code) the two 964 forms give 858 at retail's 2828 B, 0x73530 MATCH, with six of retail's seven calls in place
// (ours still calls 0x193300 where retail expands it and expands 0x193250 where retail calls it); lead patch
// build/r304/lead-7e210-858.patch in the m3-lane3 worktree, not kept: the guard is a test that is always true. Natural
// spellings of the opening on the second 964 form: the result declared ahead of the lookup, or first of all, 945 (2900
// B); the variant declared NULL and then assigned 964; the accessor called in the lookup's argument 951 (2924 B); the
// lookup only when there is a summary 916 on the 977 form (0x73530 1: the call then follows the summary read); the
// 0x5b060 accessor with two returns 966 (and 0x5b060 itself lost, 27 B for 26). None makes the opening lookup a call.
// M3 lane 15 r143 (fast screens with 0x73530, 0x63220, 0x192e60, 0x192db0, 0x193f50 and 0x5b060 listed; control 871 at
// 2780 B, 0x73530 MATCH; body unchanged). Asked whether the three expansion choices that differ (the opening 0x192e60
// and 0x193250 expand here where retail calls them, 0x193300 is called where retail expands it) come from how the
// callees themselves are written, since each goes the other way for a callee of about the same size. They do not. In
// the callees' file: 0x192e60 and its siblings calling the real 0x192db0 and 0x193f50 in place of the file-local
// inline copies of those two (the file's 34 functions, 0x63220 and 0x73530 all MATCH) leaves this function at 871, lane
// 3's guarded lead at 858 (2828 B) and its unguarded form at 945 (0x73530 8), the same code each; 0x193300 with the
// capacity selection written in its own body and 0x193250 returning it at its shared label (no forced-inline copy; all
// 36 MATCH) the same three values; both together the same; 0x193250 returning 0x193300 from each of its two cases
// instead 868 (2848 B) with 0x193250 lost (56 B for 100). So the choices do not follow the callees' spelling, and the
// opening lookup still needs one more two-way test in front of it that emits no code.
// M3 lane GA r1 (fast with 0x73530, 0x63220, 0x192e60, 0x193300, 0x1932c0, 0x1931a0, 0x193250, 0x631f0; control 871 at
// 2780 B, aligned 1016 / masked 616, 0x73530 MATCH; body unchanged). Tested 11 untried combinations of the single-exit /
// 0x63220 / long-histogram / result-first helpers: all 5 file-local helpers in result-first form on base 919 (2724 B,
// aligned 795, masked 399, 0x73530 MATCH); that + `if (!balance_teams_by_count) result = false;` + 3-way add_mask minimum
// 924 (2724 B, aligned 792, masked 399, 0x73530 MATCH); single-exit + 0x63220 + `long histogram[16]` memset 913 (2868 B,
// aligned 887, 0x73530 8); same with `word histogram[16]` 957 (2920 B, 0x73530 8); same + all 5 result-first + balance
// `if` 891 (2868 B, aligned 791, masked 397, 0x73530 8) and with word histogram 897 (0x73530 8); that + excess early
// `{ result = false; return result; }` + 3-way add_mask minimum 935 (2892 B, aligned **788**, masked **390**, 0x73530 8);
// single early return after `else if (type == 5)` + 0x63220 + long histogram 880 (2836 B, aligned 819, 0x73530 8); opening
// `{ result = false; return result; }` + type-5 + `if (result)` + 0x63220 + long histogram 1141 (2480 B, 0x63220 stays a
// call, 0x73530 MATCH); positive nested 5-term `&&` admission + 0x63220 + long histogram 1157 (2464 B, 0x63220 a call,
// 0x73530 MATCH); single-exit + 0x63220 + long histogram with summary read before variant_index/variant 914 (0x73530 8).
// Confirms every form that inlines both the opening 0x192e60 and 0x63220 spills session off EAX and breaks 0x73530 (8).
// Round 469: cached machine-scan count with conditional false store/Boolean AND/logical
// update 871/878/858. Pointer range with the same updates 883/879/863. Cursor advanced
// at the comparison with conditional false store or Boolean AND 883/880. NOT kept: 858
// loses 0x73530. The 863 range and repeated result stores differ from retail's countdown
// and conditional false store. Frame 0xd0 versus 0xb4 and lookup expansion remain.
// M3 lane 16 r187 (h2m fast with 0x73530, 0x63220 and 0x192e60; control 871 at 2780 B; lane 3 r304's guarded lead rebuilt
// 858 at 2828 B; body unchanged). Every matched caller of 0x192e60 on main (0x73530, 0x741f0, 0x90880 and the others)
// keeps its top-level lookup out of line only inside an inline-depth 0 region, so retail never expands the lookup at
// an entry-level site and expands it only inside the hotter player loop; this build expands it at every site that
// runs once. On the lead's exit structure (the chain clearing the result, one exit test after it, one after the type-5
// scan, the loop through 0x63220): the plain opening 945 (2900 B, 0x73530 8, re-measured); the index computed by a
// written-out `live ? value49c8 : NONE` 947; as a NONE default overridden under the liveness test (0x73530's own
// spelling) 945; the lookup written in both arms of the liveness test (`if (live) { index = value49c8; variant =
// lookup(index); } else { index = NONE; variant = lookup(index); }`) 850 at retail's 2828 B with 0x73530 MATCH: each
// arm's call then runs at half the entry's rate, neither is expanded and the two calls merge back into retail's single
// call after the join. Not kept: two arms that differ only in the index are a steering form (patch
// build/r187/lead-7e210-850-two-arm.patch in the m3-lane16 worktree). Callee side, with the plain opening: 0x1934f0
// declared without its noinline 945 (same code everywhere); 0x192e60 with two returns 1043 (0x192e60 itself 60 B for
// 68, 0x63220 150 B for 94); 0x192e60 with the validity chain written in its own body 945 (0x192e60 14).
// M3 lane GB r2 (13 trials, fast control 871 at 2780 B, aligned 1306 / masked 1026, 0x73530 and 7 callees MATCH throughout; body restored):
// Result-first switch helpers (0x193440/0x193400 shape) 905 (2752 B, aligned 1159); all 5 result-first helpers + ratings_party_limited(variant) in add_mask + 3-way add minimum 899 (2744 B, aligned 1141, masked 917); 3-way add minimum + ratings_party_limited alone 872 (2784 B); inverted add_mask branches 872 (2784 B); switch add_mask 894 (2748 B); merged team-type if 908 (2752 B); merged team if + if (!balance_teams_by_count) result = false 910 (2748 B); adding_group before out->maximum + ternary first = first == NONE ? count : first 877 (2780 B); single rating read + pre-loaded group first_rating 890 (2764 B); { result = false; return result; } at opening and excess exits + if (!balance) 917 (2732 B); local group_mask accumulator + early adding_group 888 (2768 B); combined retail-shaped lower half 885 (2724 B, aligned 1129, masked 935); combined + scoped machine_ids + memset long histogram[16] 955 (2676 B, aligned 1109, masked 916). All retail-shaped lower-half forms cut aligned/masked diffs while shortening the already-short body (top 0x192e60 still expands and 0x193300/0x631f0 stay calls).
// GB9 (fast with 0x73530, 0x91f00 (34 throughout), 0x63220, 0x192e60, 0x193300, 0x1932c0, 0x1931a0, 0x193250, 0x631f0, 0x91e30; control 871 at 2780/2828 B, aligned 1205, masked 1024; 11 trials, all 0x73530 MATCH, body unchanged):
// 5-case switch helper for add_mask minimum 858 (2804 B, aligned 1213, masked 1032: gains only from an extra 20-byte jump table retail does not emit; not kept); add_mask maximum via ratings_party_limited + matched 0x1933d0 877 (2780 B); matched bool predicates 0x193470/0x1934b0/0x1934d0 only 884 (2780 B, masked 978); matched switch accessors 0x193440/0x193400 only 905 (2752 B); matched party bounds 0x1933d0/0x1933a0 only 897 (2744 B, aligned 1114); negated add_mask ifs 871 (same code); `!function_193490(variant)` for add_mask 883 (2764 B); party `first` ternary alone 871 (same code); type-5 scan without early return (`if (result)` around rest) 894 (2764 B); two-stage single-exit admission keeping 0x631f0 903 (2748 B); combined 5-case add minimum + matched predicates + 0x1933d0 max 857 (2816 B, masked 996, same extra jump table; not kept). Opening 0x192e60 expansion and out-of-line 0x193300/0x631f0 remain; body stays at 871.
// The output pointer is loaded early and the opening variant lookup still expands. Using the clear result
// in the player, group, both participant, or party phases scores 872/902/891/926 with word cells and
// 905/925/917/954 with dword cells. Player, group and combined participant dword forms reach the 0xb4 frame
// but shorten the body. Player and combined forms change the matched caller.
// Initialize the first party's span or both endpoints 893/893, extend only
// an existing span 871, first-party count on either side of its update 933/895,
// conditional span 911, initialize from the last-party sentinel 890/910.
// NOT kept: all tie or regress and leave the opening call and frame different.
// M3 lane GA r6 (19 source trials + 1 diagnostic, fast with 0x73530/0x63220/0x192e60/0x193300/0x1932c0/0x1931a0/0x193250/0x631f0/0x1934f0, control 871 at 2780 B, 0x73530 MATCH; body unchanged): single-exit + 0x63220 + long/word histogram 906/957 (0x73530 8); with else-if type-5 or positive if 844/844 (2852 B, frame 0xb4, 0x73530 8); + all 7 matched 0x1933a0..0x1934d0 accessors + if-balance + excess early return 977 (aligned 540, 0x73530 8), + memset/scoped/cmove 969 (aligned 538, 0x73530 8), + result-first spread + switch minimum + excess clear (`else if` type-5) 833 (2896 B, aligned 458, masked 412, frame 0xb4, tail-duplicated false epilogues, 0x73530 8); diagnostic inline_depth(1) 957 (0x73530 8); single-exit on main's hand-written loop 988 (0x73530 8, proving single-exit alone loses session-in-EAX while top 0x192e60 expands). Early-return forms keeping 0x73530 MATCH: if-balance alone 905; result-first spread + switch minimum + if-balance 897; all 7 matched accessors + if-balance + excess clear/return 893/891 (masked 427/453), + scoped long histogram memset 932/940 (masked 402/426, frame 0xb0); opening return-false + type-5 `if (result)` with hand-written loop 893, with 0x63220 + real accessors 1040 (2580 B, masked 337, 0x63220 stays a call), with 0x63220 on main accessors 1002; early-return + 0x63220 + real accessors 1053; main base + scoped long histogram memset 936.
// @retail 0x7e210
bool function_7e210(c_class_58d20 *session, s_matchmaking_ratings *out)
{
	long variant_index = session->get_value_49c8();
	s_surface_description *variant = function_192e60(variant_index);
	const s_ratings_summary *summary = (const s_ratings_summary *)session->get_data_4a00();
	long capacity = network_session_get_maximum_players(session);
	bool result = true;
	memset(out, 0, sizeof(*out));
	if (!summary || summary->player_count > capacity || !variant ||
		capacity > function_193300(variant) || capacity < function_1932c0(variant))
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
				long highest = session_summary_get_machine_highest_value((s_session_summary *)summary, summary->groups[i]);
				if (highest != NONE)
				{
					long lower = highest;
					while (lower > 0 && rating_variant->field_51c[lower - 1] >= highest) lower--;
					if (adjusted <= lower) adjusted = function_1931a0(highest, rating_variant);
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
		// retail tests the array's address here (0x7e5ff-0x7e607: lea, test), so the division below always runs
		if (!out->rated_counts) out->averages[i] = NONE;
		else out->averages[i] /= out->rated_counts[i];
	}
	word histogram[16] = {0};
	{
		const long *group_size = out->total_counts;
		for (long remaining = out->group_count; remaining > 0; --remaining)
		{
			++histogram[*group_size - 1];
			++group_size;
		}
	}
	long last = NONE;
	long first = NONE;
	memset(histogram, 0, sizeof(histogram));
	long spread = 0;
	long parties = 0;
	if (ratings_party_limited(variant))
	{
		const long *group_size = out->total_counts;
		for (long remaining = out->group_count; remaining > 0; --remaining)
		{
			long count = *group_size;
			if (count >= ratings_party_minimum(variant) && count <= ratings_party_maximum(variant))
			{
				histogram[count - 1]++;
				parties++;
				if (first == NONE) first = count;
				last = count;
				spread = last - first;
			}
			++group_size;
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
		if (excess > available) result = false;
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
