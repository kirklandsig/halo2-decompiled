#include "unknown_11c920.h"

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
