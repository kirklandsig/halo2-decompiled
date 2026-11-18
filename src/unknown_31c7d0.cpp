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

static __forceinline char const *find_dash_31c7d0(char const *text)
{
	while (*text != '-')
	{
		if (!*text) return 0;
		++text;
	}
	return text;
}

static __forceinline unsigned long decode_31c7d0(char const *last)
{
	return ((unsigned long)(*last - 'A') << 28)
		| ((unsigned long)(last[-1] - 'A') << 24)
		| ((unsigned long)(last[-2] - 'A') << 20)
		| ((unsigned long)(last[-3] - 'A') << 16)
		| ((unsigned long)(last[-4] - 'A') << 12)
		| ((unsigned long)(last[-5] - 'A') << 8)
		| ((unsigned long)(last[-6] - 'A') << 4)
		| (unsigned long)(last[-7] - 'A');
}

// Matched: both dash scans go through one inline finder, the label is compared by
// index up to the separator, and the eight digits are read by an inline decoder
// indexing back from the last one (the order the byte loads come out in follows the
// base the offsets are taken from, not the order the terms are written in).
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
	char const *separator = find_dash_31c7d0(g_46fbf0);
	if (!separator) { function_2d97d0(g_415460); return; }
	char const *label = separator + 1;
	separator = find_dash_31c7d0(label);
	if (!separator) { function_2d97d0(g_415460); return; }
	for (long i = 0; label + i != separator; i++)
	{
		if (!label[i] || !g_415444[i] || label[i] != g_415444[i])
		{
			function_2d97d0(g_415460);
			return;
		}
	}
	unsigned long encoded = decode_31c7d0(separator + 8);
	long expires = (long)(encoded ^ g_415440);
	expires -= expires % 86400;
	if (expires > now && (long)((unsigned long)expires - (unsigned long)now) < 15552000L)
		g_480c60 = 1;
	else function_2d97d0(g_415460);
}
