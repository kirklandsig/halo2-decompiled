// @flags /O2 /GL-
// Preserve the solver's two retail validation paths and their failure notices.
#include "solver_validation_31c540.h"

// Storage for externally supplied validation data. Leave the gate disabled
// until the retail checks succeed; no retail key material is embedded.
unsigned long g_445470;
char *g_46fbf0;
long g_415440;
char g_415444[4];
char g_415460[120];
char g_4154d8[64];
unsigned char g_480c60;
long __cdecl function_3724f0();
void __cdecl function_2d97d0(char const *message);

// @retail 0x31c7d0
void __cdecl function_31c7d0()
{
	if (g_445470)
	{
		unsigned long hash = function_31c540(g_46fbf0);
		if (g_445470 == hash) g_480c60 = 1;
		else function_2d97d0(g_4154d8);
		return;
	}
	long now = function_3724f0();
	char const *separator = g_46fbf0;
	while (*separator != '-')
	{
		if (!*separator) { function_2d97d0(g_415460); return; }
		++separator;
	}
	char const *label = ++separator;
	while (*separator != '-')
	{
		if (!*separator) { function_2d97d0(g_415460); return; }
		++separator;
	}
	long expected_offset = (unsigned long)g_415444 - (unsigned long)label;
	while (label != separator)
	{
		if (!*label || !label[expected_offset] || *label != label[expected_offset])
		{
			function_2d97d0(g_415460);
			return;
		}
		++label;
	}
	unsigned long encoded = ((unsigned long)(separator[4] - 'A') << 12)
		| ((unsigned long)(separator[3] - 'A') << 8)
		| ((unsigned long)(separator[7] - 'A') << 24)
		| ((unsigned long)(separator[2] - 'A') << 4)
		| ((unsigned long)(separator[6] - 'A') << 20)
		| ((unsigned long)(separator[5] - 'A') << 16)
		| ((unsigned long)(separator[8] - 'A') << 28)
		| (unsigned long)(separator[1] - 'A');
	long expires = (long)(encoded ^ g_415440);
	expires -= expires % 86400;
	if (expires > now && (long)((unsigned long)expires - (unsigned long)now) < 15552000L)
		g_480c60 = 1;
	else function_2d97d0(g_415460);
}
