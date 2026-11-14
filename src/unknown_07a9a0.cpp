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
long network_session_get_maximum_players(c_class_58d20 *session);
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
// MiniMax H (11-12): no new form, no lower score (766): the frame is still 0xd4 against retail's 0xb4, the session still comes in on the stack where retail passes it in eax with the output pointer on the stack, and the opening lookup is still expanded here while retail calls the matched 0x192e60 out of line.
// MiniMax S (11-11): no new form, no lower score (766): the frame is 0xd4 against retail's 0xb4, the session
// comes in on the stack where retail passes it in EAX with the output pointer on the stack, and the opening
// lookup is expanded here while retail calls the matched 0x192e60 out of line; the session's EAX home belongs
// to the callers in other files.
// MiniMax E (11-05): no new form, no lower score; the session's EAX home belongs to the caller's
// declaration, which this lane may not change.
// w14C (11-06): walked against retail block by block; no behaviour difference. The histogram cells are dwords in retail
// (0x10 dwords cleared at 0x7e66c/0x7e6a5 with rep stosd, 4-byte inc at 0x7e682-0x7e68f and 0x7e740-0x7e74d); `long
// histogram[16]` with a memset for each clear recovers retail's frame 0xb4 (machine_ids and the histogram share slots) but
// scores 844 (2772 bytes). The histogram is never read, so it is not kept without a match. Tried: the add-loop minimum as a
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
		// retail tests the array's address here (0x7e5ff-0x7e607: lea, test), so the division below always runs
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
