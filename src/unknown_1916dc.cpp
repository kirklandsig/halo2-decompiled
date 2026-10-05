#include "unknown_11c920.h"
#include "globals.h"

// @flags /O1 /arch:SSE /Gr

/* UNKNOWN_1916DC.CPP: the messages the HUD shows about the player's weapons */

struct s_type_2d4969
{
	byte unknown00[4];
	real age;
	byte unknown08[0x24 - 0x08];
	short total_rounds;
	byte unknown26[2];
	short loaded_rounds;
	short magazines;
	short charge;
};

// @retail 0x1916dc
bool function_1916dc(s_type_2d4969 const *state)
{
	bool depleted = false;

	if (state->total_rounds > 0 && state->magazines != 0 && state->loaded_rounds == 0 && state->charge == 0)
		depleted = true;

	if (state->age == 1.0f)
		depleted = true;

	return depleted;
}

/* the messages of an item definition (a weapon's pickup messages) */
struct s_item_message_definition
{
	byte unknown00[0xe0];
	long message;
	long singular_message;
	long plural_message;
};

void function_13925f(long string_handle, word *buffer);
void function_24caac(long player_index, word const *text, word const *plural_text, long count);
void function_24cb54(long player_index, word const *text, word const *plural_text);
void function_24cbbf(long player_index, long string_handle);

/* shows the item's message for one of it */
// @retail 0x191e51
void __stdcall function_191e51(long definition_index, long player_index)
{
	if (player_index != NONE)
	{
		s_item_message_definition *definition = (s_item_message_definition *)g_4e3b44[definition_index & 0xffff].bytes;
		word text[0x100];
		word plural_text[0x100];

		text[0] = 0;
		plural_text[0] = 0;
		function_13925f(definition->singular_message, text);
		function_13925f(definition->plural_message, plural_text);
		function_24caac(player_index, text, plural_text, 1);
	}
}

/* shows the item's message for a count of it */
// @retail 0x191ec4
void __stdcall function_191ec4(long definition_index, long player_index, short count)
{
	if (player_index != NONE)
	{
		s_item_message_definition *definition = (s_item_message_definition *)g_4e3b44[definition_index & 0xffff].bytes;
		word text[0x100];
		word plural_text[0x100];

		text[0] = 0;
		plural_text[0] = 0;
		function_13925f(definition->singular_message, text);
		function_13925f(definition->plural_message, plural_text);
		function_24caac(player_index, text, plural_text, count);
	}
}

// @retail 0x191f3a
void __stdcall function_191f3a(long definition_index, long player_index)
{
	if (player_index != NONE)
	{
		s_item_message_definition *definition = (s_item_message_definition *)g_4e3b44[definition_index & 0xffff].bytes;
		word text[0x100];
		word plural_text[0x100];

		text[0] = 0;
		plural_text[0] = 0;
		function_13925f(definition->singular_message, text);
		function_13925f(definition->plural_message, plural_text);
		function_24cb54(player_index, text, plural_text);
	}
}

// @retail 0x191fab
void __stdcall function_191fab(long definition_index, long player_index)
{
	if (player_index != NONE)
	{
		s_item_message_definition *definition = (s_item_message_definition *)g_4e3b44[definition_index & 0xffff].bytes;

		function_24cbbf(player_index, definition->message);
	}
}
