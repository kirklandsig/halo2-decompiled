// @flags /O2 /Gr
/* UNKNOWN_0ADEF0.CPP: the session membership message codecs
   (membership-update, peer-properties, delegate-leader and boot-machine,
   player-add, player-refuse, player-remove, player-properties) and the
   registration of their message types. A decoder returns whether the
   message it read is valid */

#include "unknown_11c920.h"
#include "bitstream.h"
#include "unknown_1946f0.h"
#include "network_message_types.h"

/* every message starts with a 64 bit header */
struct s_message_header
{
	byte data[8];
};

/* one of the players in the membership-update message, 0x104 bytes */
struct s_message_player
{
	short index1;
	short index2;
	byte data[0x24];
	byte flag0;
	byte flag1;
	byte unknown2a[2];
	byte flag2;
	word words1[16];
	word words2[32];
	byte flag3;
	long value1;
	long value2;
	byte flag4;
	dword bits1;
	dword bits2;
	long value3;
	long value4;
	long value5;
	long value6;
	long value7;
	byte flag5;
	long values[4][4];
	byte flag6;
	long cell;
};

/* one of the entries in the membership-update message, 0x140 bytes */
struct s_message_entry
{
	short index1;
	short kind;
	byte point[0xc];
	short index2;
	short index3;
	byte flag;
	byte unknown15[3];
	long index4;
	byte unknown1c[0x90];
	byte unknownac[0x90];
	dword value;
};

/* 0xadef0, 0xae7f0 */
struct s_message_membership_update
{
	s_message_header header;
	long value1;
	long value2;
	short player_count;
	short entry_count;
	s_message_player players[32];
	s_message_entry entries[32];
	byte flag;
	long value3;
};

/* 0xaedb0, 0xaf050 */
struct s_message_peer_properties
{
	s_message_header header;
	word words1[16];
	word words2[32];
	long value1;
	long value2;
	dword bits1;
	dword bits2;
	long value3;
	long value4;
	long value5;
	long value6;
	long value7;
	long values[4][4];
	long cell;
};

/* 0xaf1f0, 0xaf220: delegate-leader and boot-machine */
struct s_message_delegate_leader
{
	s_message_header header;
	byte data[0x24];
};

/* 0xaf270, 0xaf320 */
struct s_message_player_add
{
	s_message_header header;
	long index;
	byte point[0xc];
	long index2;
	byte unknown1c[0x90];
	dword value;
};

/* 0xaf3c0, 0xaf430 */
struct s_message_player_refuse
{
	s_message_header header;
	long index;
	byte point[0xc];
};

/* 0xaf490, 0xaf4f0 */
struct s_message_player_remove
{
	s_message_header header;
	long index;
};

/* 0xaf540, 0xaf5f0 */
struct s_message_player_properties
{
	s_message_header header;
	long index;
	long index2;
	byte unknown10[0x90];
	dword value;
};

// @retail 0xadef0
void __stdcall function_0adef0(s_bitstream *stream, long unused, s_message_membership_update *message)
{
	long i;
	function_1955d0(stream, message, 0x40);
	function_195720(stream, message->value1, 0x20);
	if (message->value2 == NONE)
		stream_write_bit(stream, true);
	else
	{
		stream_write_bit(stream, false);
		function_195720(stream, message->value2, 0x20);
	}
	stream_write_checked(stream, message->player_count, 5);
	stream_write_checked(stream, message->entry_count, 5);
	for (i = 0; i < message->player_count; i++)
	{
		s_message_player *player = &message->players[i];
		if (player->index1 == NONE)
			stream_write_bit(stream, false);
		else
		{
			stream_write_bit(stream, true);
			stream_write_checked(stream, player->index1, 4);
		}
		if (player->index2 == NONE)
			stream_write_bit(stream, false);
		else
		{
			stream_write_bit(stream, true);
			stream_write_checked(stream, player->index2, 4);
		}
		function_1955d0(stream, player->data, 0x120);
		stream_write_bit(stream, TEST_FIELD_BIT(player->flag0));
		if (player->flag0)
		{
			stream_write_bit(stream, TEST_FIELD_BIT(player->flag1));
			stream_write_bit(stream, TEST_FIELD_BIT(player->flag2));
			if (player->flag2)
			{
				long j;
				word *words;
				for (j = 0, words = player->words1; j < 16; j++, words++)
				{
					word value = *words;
					function_195720(stream, value, 16);
					if (value == 0)
						break;
				}
				for (j = 0, words = player->words2; j < 32; j++, words++)
				{
					word value = *words;
					function_195720(stream, value, 16);
					if (value == 0)
						break;
				}
			}
			stream_write_bit(stream, TEST_FIELD_BIT(player->flag3));
			if (player->flag3)
			{
				stream_write_checked(stream, player->value1, 3);
				stream_write_checked(stream, player->value2, 7);
			}
			stream_write_bit(stream, TEST_FIELD_BIT(player->flag4));
			if (player->flag4)
			{
				function_1955d0(stream, &player->bits1, 0x20);
				function_1955d0(stream, &player->bits2, 0x20);
				stream_write_checked(stream, player->value3, 2);
				stream_write_checked(stream, player->value4, 16);
				stream_write_checked(stream, player->value5, 11);
				stream_write_checked(stream, player->value6, 11);
				stream_write_checked(stream, player->value7, 11);
			}
			stream_write_bit(stream, TEST_FIELD_BIT(player->flag5));
			if (player->flag5)
			{
				long j;
				for (j = 0; j < 16; j++)
					stream_write_checked(stream, player->values[0][j], 2);
			}
			stream_write_bit(stream, TEST_FIELD_BIT(player->flag6));
			if (player->flag6)
				stream_write_checked(stream, player->cell, 4);
		}
	}
	for (i = 0; i < message->entry_count; i++)
	{
		s_message_entry *entry = &message->entries[i];
		stream_write_checked(stream, entry->index1, 4);
		stream_write_checked(stream, entry->kind, 2);
		if (entry->kind == 1)
		{
			function_1955d0(stream, entry->point, 0x60);
			stream_write_checked(stream, entry->index2, 4);
			stream_write_checked(stream, entry->index3, 2);
		}
		stream_write_bit(stream, TEST_FIELD_BIT(entry->flag));
		if (entry->flag)
		{
			stream_write_checked(stream, entry->index4, 2);
			function_07c5a0(stream, entry->unknown1c);
			function_07c5a0(stream, entry->unknownac);
			function_195720(stream, entry->value, 0x20);
		}
	}
	stream_write_bit(stream, TEST_FIELD_BIT(message->flag));
	if (message->flag)
		stream_write_checked(stream, message->value3, 4);
}

// @retail 0xae7f0
bool __stdcall function_0ae7f0(s_bitstream *stream, long unused, s_message_membership_update *message)
{
	long i;
	bool valid = true;
	function_195820(stream, message, 0x40);
	message->value1 = function_1959c0(stream, 0x20);
	if (function_1957d0(stream))
		message->value2 = NONE;
	else
		message->value2 = function_1959c0(stream, 0x20);
	message->player_count = (short)function_1959c0(stream, 5);
	message->entry_count = (short)function_1959c0(stream, 5);
	if (message->player_count < 0 || message->player_count > 32 || message->entry_count < 0 || message->entry_count > 32)
		valid = false;
	else
	{
		for (i = 0; i < message->player_count; i++)
		{
			s_message_player *player = &message->players[i];
			if (function_1957d0(stream))
			{
				player->index1 = (short)function_1959c0(stream, 4);
				if (valid && player->index1 >= 0 && player->index1 < 16)
					valid = true;
				else
					valid = false;
			}
			else
				player->index1 = -1;
			if (function_1957d0(stream))
			{
				player->index2 = (short)function_1959c0(stream, 4);
				if (valid && player->index2 >= 0 && player->index2 < 16)
					valid = true;
				else
					valid = false;
			}
			else
				player->index2 = -1;
			if (valid && (player->index1 != -1 || player->index2 != -1))
				valid = true;
			else
				valid = false;
			function_195820(stream, player->data, 0x120);
			player->flag0 = function_1957d0(stream);
			if (player->flag0)
			{
				player->flag1 = function_1957d0(stream);
				player->flag2 = function_1957d0(stream);
				if (player->flag2)
				{
					function_194fa0(stream, player->words1, 16);
					function_194fa0(stream, player->words2, 32);
				}
				player->flag3 = function_1957d0(stream);
				if (player->flag3)
				{
					player->value1 = function_1959c0(stream, 3);
					player->value2 = function_1959c0(stream, 7);
					if (valid && player->value1 >= 0 && player->value1 < 5 && player->value2 >= 0 && player->value2 <= 100)
						valid = true;
					else
						valid = false;
				}
				player->flag4 = function_1957d0(stream);
				if (player->flag4)
				{
					function_195820(stream, &player->bits1, 0x20);
					function_195820(stream, &player->bits2, 0x20);
					player->value3 = function_1959c0(stream, 2);
					player->value4 = function_1959c0(stream, 16);
					player->value5 = function_1959c0(stream, 11);
					player->value6 = function_1959c0(stream, 11);
					player->value7 = function_1959c0(stream, 11);
					if (valid && player->value5 >= 0 && player->value5 <= 2000 && player->value6 >= 0 && player->value6 <= 2000 && player->value7 >= 0 && player->value7 <= 2000)
						valid = true;
					else
						valid = false;
				}
				player->flag5 = function_1957d0(stream);
				if (player->flag5)
				{
					long j;
					for (j = 0; j < 16; j++)
						player->values[0][j] = function_1959c0(stream, 2);
					for (j = 0; j < 4; j++)
					{
						long k;
						for (k = 0; k < 4; k++)
						{
							if (valid && player->values[j][k] >= 0 && player->values[j][k] < 4)
								valid = true;
							else
								valid = false;
						}
					}
				}
				player->flag6 = stream_read_bit(stream);
				if (player->flag6)
				{
					player->cell = function_1959c0(stream, 4);
					if (valid && player->cell >= 0 && player->cell < 9)
						valid = true;
					else
						valid = false;
				}
			}
		}
		for (i = 0; i < message->entry_count; i++)
		{
			s_message_entry *entry = &message->entries[i];
			entry->index1 = (short)function_1959c0(stream, 4);
			entry->kind = (short)function_1959c0(stream, 2);
			if (valid && entry->index1 >= 0 && entry->index1 < 16 && entry->kind >= 0 && entry->kind < 3)
				valid = true;
			else
				valid = false;
			if (entry->kind == 1)
			{
				function_195820(stream, entry->point, 0x60);
				entry->index2 = (short)function_1959c0(stream, 4);
				entry->index3 = (short)function_1959c0(stream, 2);
				if (valid && entry->index2 >= 0 && entry->index2 < 16 && entry->index3 >= 0 && entry->index3 < 4)
					valid = true;
				else
					valid = false;
			}
			entry->flag = stream_read_bit(stream);
			if (entry->flag)
			{
				entry->index4 = function_1959c0(stream, 2);
				bool ok = valid && entry->index4 >= 0 && entry->index4 < 4;
				byte first = function_07ca70(stream, entry->unknown1c);
				ok = ok && first;
				byte second = function_07ca70(stream, entry->unknownac);
				valid = ok && second;
				entry->value = function_1959c0(stream, 0x20);
			}
		}
	}
	bool tail_flag = stream_read_bit(stream);
	message = *(s_message_membership_update *volatile *)&message;
	message->flag = tail_flag;
	if (message->flag)
		message->value3 = function_1959c0(stream, 4);
	if (valid && !stream_overflowed(stream) && (message->value2 == NONE || message->value2 < message->value1))
		return true;
	return false;
}

// @retail 0xaedb0
void __stdcall function_0aedb0(s_bitstream *stream, long unused, s_message_peer_properties *message)
{
	long i;
	function_1955d0(stream, message, 0x40);
	word *words;
	for (i = 0, words = message->words1; i < 16; i++, words++)
	{
		word wide_char_unit = *words;
		function_195720(stream, wide_char_unit, 16);
		if (wide_char_unit == 0)
			break;
	}
	for (i = 0, words = message->words2; i < 32; i++, words++)
	{
		word wide_char_unit = *words;
		function_195720(stream, wide_char_unit, 16);
		if (wide_char_unit == 0)
			break;
	}
	stream_write_checked(stream, message->value1, 3);
	stream_write_checked(stream, message->value2, 7);
	function_1955d0(stream, &message->bits1, 0x20);
	function_1955d0(stream, &message->bits2, 0x20);
	stream_write_checked(stream, message->value3, 2);
	stream_write_checked(stream, message->value4, 16);
	stream_write_checked(stream, message->value5, 11);
	stream_write_checked(stream, message->value6, 11);
	stream_write_checked(stream, message->value7, 11);
	for (i = 0; i < 16; i++)
		stream_write_checked(stream, message->values[0][i], 2);
	stream_write_checked(stream, message->cell, 4);
}

// @retail 0xaf050
bool __stdcall function_0af050(s_bitstream *stream, long unused, s_message_peer_properties *message)
{
	long i;
	function_195820(stream, message, 0x40);
	read_word_string(stream, message->words1, 16);
	read_word_string(stream, message->words2, 32);
	message->value1 = function_1959c0(stream, 3);
	message->value2 = function_1959c0(stream, 7);
	function_195820(stream, &message->bits1, 0x20);
	function_195820(stream, &message->bits2, 0x20);
	message->value3 = function_1959c0(stream, 2);
	message->value4 = function_1959c0(stream, 16);
	message->value5 = function_1959c0(stream, 11);
	message->value6 = function_1959c0(stream, 11);
	message->value7 = function_1959c0(stream, 11);
	for (i = 0; i < 16; i++)
		message->values[0][i] = function_1959c0(stream, 2);
	message->cell = function_1959c0(stream, 4);
	bool valid = !stream_overflowed(stream);
	if (valid && message->value1 >= 0 && message->value1 < 5 && message->value2 >= 0 && message->value2 <= 100)
		valid = true;
	else
		valid = false;
	for (i = 0; i < 4; i++)
	{
		long j;
		for (j = 0; j < 4; j++)
		{
			if (valid && message->values[i][j] >= 0 && message->values[i][j] < 4)
				valid = true;
			else
				valid = false;
		}
	}
	if (valid && message->cell >= 0 && message->cell < 9)
		return true;
	return false;
}

// @retail 0xaf1f0
void __stdcall function_0af1f0(s_bitstream *stream, long unused, s_message_delegate_leader *message)
{
	function_1955d0(stream, message, 0x40);
	function_1955d0(stream, message->data, 0x120);
}

// @retail 0xaf220
bool __stdcall function_0af220(s_bitstream *stream, long unused, s_message_delegate_leader *message)
{
	function_195820(stream, message, 0x40);
	function_195820(stream, message->data, 0x120);
	if (!stream_overflowed(stream))
		return true;
	return false;
}

// @retail 0xaf270
void __stdcall function_0af270(s_bitstream *stream, long unused, s_message_player_add *message)
{
	function_1955d0(stream, message, 0x40);
	stream_write_checked(stream, message->index, 2);
	function_1955d0(stream, message->point, 0x60);
	stream_write_checked(stream, message->index2, 2);
	function_07c5a0(stream, message->unknown1c);
	function_195720(stream, message->value, 0x20);
}

// @retail 0xaf320
bool __stdcall function_0af320(s_bitstream *stream, long unused, s_message_player_add *message)
{
	function_195820(stream, message, 0x40);
	message->index = function_1959c0(stream, 2);
	function_195820(stream, message->point, 0x60);
	message->index2 = function_1959c0(stream, 2);
	bool valid = function_07ca70(stream, message->unknown1c);
	message->value = function_1959c0(stream, 0x20);
	if (valid && !stream_overflowed(stream) && message->index >= 0 && message->index < 4 && message->index2 >= 0 && message->index2 < 4)
		return true;
	return false;
}

// @retail 0xaf3c0
void __stdcall function_0af3c0(s_bitstream *stream, long unused, s_message_player_refuse *message)
{
	function_1955d0(stream, message, 0x40);
	stream_write_checked(stream, message->index, 2);
	function_1955d0(stream, message->point, 0x60);
}

// @retail 0xaf430
bool __stdcall function_0af430(s_bitstream *stream, long unused, s_message_player_refuse *message)
{
	function_195820(stream, message, 0x40);
	message->index = function_1959c0(stream, 2);
	function_195820(stream, message->point, 0x60);
	if (!stream_overflowed(stream) && message->index >= 0 && message->index < 4)
		return true;
	return false;
}

// @retail 0xaf490
void __stdcall function_0af490(s_bitstream *stream, long unused, s_message_player_remove *message)
{
	function_1955d0(stream, message, 0x40);
	stream_write_checked(stream, message->index, 2);
}

// @retail 0xaf4f0
bool __stdcall function_0af4f0(s_bitstream *stream, long unused, s_message_player_remove *message)
{
	function_195820(stream, message, 0x40);
	message->index = function_1959c0(stream, 2);
	if (!stream_overflowed(stream) && message->index >= 0 && message->index < 4)
		return true;
	return false;
}

// @retail 0xaf540
void __stdcall function_0af540(s_bitstream *stream, long unused, s_message_player_properties *message)
{
	function_1955d0(stream, message, 0x40);
	stream_write_checked(stream, message->index, 2);
	stream_write_checked(stream, message->index2, 2);
	function_07c5a0(stream, message->unknown10);
	function_195720(stream, message->value, 0x20);
}

// @retail 0xaf5f0
bool __stdcall function_0af5f0(s_bitstream *stream, long unused, s_message_player_properties *message)
{
	function_195820(stream, message, 0x40);
	message->index = function_1959c0(stream, 2);
	message->index2 = function_1959c0(stream, 2);
	bool valid = function_07ca70(stream, message->unknown10);
	message->value = function_1959c0(stream, 0x20);
	if (valid && !stream_overflowed(stream) && message->index >= 0 && message->index < 4 && message->index2 >= 0 && message->index2 < 4)
		return true;
	return false;
}

// @retail 0xaf680
void network_message_types_register_session_membership(c_type_659ceb *collection)
{
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_membership_update, "membership-update", 0x489c, function_0adef0, function_0ae7f0);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_peer_properties, "peer-properties", 0xd0, function_0aedb0, function_0af050);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_delegate_leader, "delegate-leadership", 0x2c, function_0af1f0, function_0af220);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_boot_machine, "boot-machine", 0x2c, function_0af1f0, function_0af220);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_player_add, "player-add", 0xb0, function_0af270, function_0af320);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_player_refuse, "player-refuse", 0x18, function_0af3c0, function_0af430);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_player_remove, "player-remove", 0xc, function_0af490, function_0af4f0);
	REGISTER_MESSAGE_TYPE(collection, _network_message_type_player_properties, "player-properties", 0xa4, function_0af540, function_0af5f0);
}
