// @flags /O2 /GL-
// The fixed-width text hash used by the solver's validation gate.
#include "solver_validation_31c540.h"

// Validation data is supplied separately; no retail key material is embedded.
char g_415448[64];

// @retail 0x31c540
unsigned long __cdecl function_31c540(char const *text)
{
	unsigned long weights[64] = {
		2,3,1,2,3,1,4,3,2,1,3,1,2,2,3,3,
		2,3,1,2,3,1,4,3,2,1,3,1,2,2,3,3,
		2,3,1,2,3,1,4,3,2,1,3,1,2,2,3,3,
		2,3,1,2,3,1,4,3,2,1,3,1,2,2,3,3
	};
	char input[64], salt[64];
	for (long i = 63; i >= 0; --i)
	{
		input[i] = 0;
		salt[i] = 0;
	}
	for (long i = 0; i < 64 && text[i]; ++i) input[i] = text[i];
	for (long i = 0; i < 64 && g_415448[i]; ++i) salt[i] = g_415448[i];
	unsigned long hash = 0;
	unsigned long state = 0xc18f77a2;
	long multiply = 1;
	for (long i = 0; i < 64; ++i)
	{
		unsigned long value = (salt[i] ^ input[i]) * 0x1010101UL;
		hash += multiply ? weights[i] * value : value / weights[i];
		multiply = 1 - multiply;
		for (long j = 0; j < i; ++j)
		{
			state = (state * 2) ^ ((salt[j] + input[j]) * 0x1010101UL);
			// Retail keeps the running sum in memory during this mixing loop.
			*(volatile unsigned long *)&hash += state;
		}
	}
	return hash;
}
