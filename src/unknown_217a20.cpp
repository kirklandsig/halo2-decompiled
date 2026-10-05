#include "unknown_11c920.h"
#include <string.h>

// @flags /O2 /Gr

// @retail 0x217a20
const word *function_217a20(long language)
{
	switch (language)
	{
	case 1: return L"Player";
	case 2: return L"Standard";
	case 3: return L"Par d\x00e9" L"faut";
	case 4: return L"Predeterminada";
	case 5: return L"Predefinito";
	case 7: return L"\x9810\x8a2d";
	case 6: return L"Default";
	case 8: return L"Default";
	default: return L"Default";
	}
}

// @retail 0x217b70
dword function_217b70(word const *text)
{
	dword length;
	for (length = 0; length < 255; length++)
	{
		if (!text[length])
			break;
	}
	return length;
}

const word *function_217620(int language);
const word *function_217680(int language);
const word *function_2176e0(int language);
const word *function_217740(int language);
const word *function_2177a0(int language);
const word *function_2177f0(int language);
const word *function_217840(int language);
const word *function_2178a0(int language);
const word *function_217900(int language);
const word *function_217960(int language);
const word *function_2179c0(int language);

// @retail 0x217a80
void function_217a80(long language, long string_handle_2, word *buffer)
{
	const word *text = L"";
	switch (string_handle_2)
	{
	case 0x1b0006e8: text = function_217a20(language); break;
	case 0x1b0006e9: text = function_217620(language); break;
	case 0x130006ea: text = function_217680(language); break;
	case 0x110006eb: text = function_2176e0(language); break;
	case 0x110006ec: text = function_217740(language); break;
	case 0x140006ed: text = function_2177a0(language); break;
	case 0x170006ee: text = function_2177f0(language); break;
	case 0x170006ef: text = function_217840(language); break;
	case 0x100006f0: text = function_2178a0(language); break;
	case 0x140006f1: text = function_217900(language); break;
	case 0x180006f2: text = function_217960(language); break;
	case 0x150006f3: text = function_2179c0(language); break;
	}
	wcsncpy(buffer, text, 0xff);
	buffer[0xff] = 0;
}
