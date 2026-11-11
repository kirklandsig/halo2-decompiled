// @flags /O2 /GL-
// The fixed-width text hash used by the solver's validation gate.
#include "solver_validation_31c540.h"

// Validation data is supplied separately; no retail key material is embedded.
char g_415448[64];

// Advance the running mixer in place and expose that same value to the sum.
// The salt reference preserves the retail signed-byte load order.
static __forceinline unsigned long &advance_hash_state(unsigned long &state,
	char const volatile &salted, char const &supplied)
{
	state *= 2;
	state ^= (salted + supplied) * 0x1010101UL;
	return state;
}

// @retail 0x31c540
unsigned long __cdecl function_31c540(char const *text)
{
	// Capture the input before initializing the local weights.
	char const *const volatile &input_argument = text;
	char const *source = input_argument;
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
	for (long i = 0; i < 64 && source[i]; ++i) input[i] = source[i];
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
			// Retail keeps the running sum in memory during this mixing loop.
			*(volatile unsigned long *)&hash += advance_hash_state(state, salt[j], input[j]);
		}
	}
	return hash;
}
