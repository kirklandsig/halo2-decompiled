// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_07AEC0.CPP: network addresses */

#include "unknown_11c920.h"
#include <string.h>

/* Bungie's macro, as in Halo CE's cseries.h */
#ifndef MIN
#define MIN(a,b) ((a)>(b)?(b):(a))
#endif

enum
{
	k_ipv4_address_length = 4,
	k_ipv6_address_length = 16
};

struct s_type_99af70
{
	union
	{
		dword ipv4_address;
		word ipv6_address[8];
	};
	word port;
	short address_length;
};

bool function_7af40(s_type_99af70 const *address);

/* an IPv4 address in host byte order whose last octet is zero */
// score 26 (was 55): retail ends with "if (!(byte)output) result = true;" (test cl, cl; jne;
// mov al, 1), but that form moves the address parameter from ecx to edx (score 14 here, and
// callers 0x784a0 and 0x7ab60 stop matching), so the sete form keeps the callers' convention.
// Tried (with the if-form tail): an outer if (address) with a short length local (inlines
// into 0x7acc0), "address &&" before the call (calls 0x7af40, 86), byte_swap_long() helper
// (16), a value local (16), nested ifs / sizeof(dword) / (long) cast / reference before
// result (all 14), early return true (48), "result = true; if ((byte)*ipv4_address) result = false;" (23, but the
// tail is mov al, 1 / je / xor al, al), (output & 0xff) == 0 (28), swapped parameter order (14).
// b13O (11-17) walk against retail, fast build on r846: from 0x7aeff on the if-form is byte-identical to retail (the
// byte-swap tree, "mov cl, dl / test cl, cl / jne / mov al, 1" at 0x7af25-0x7af2d); only the incoming address register
// differs (edx here, ecx at 0x7aec2), so it is an allocation difference, not a logic one. With the if-form the callers
// 0x784a0 (score 1) and 0x7ab60 (3) stop matching (0x7acc0 and 0x7acf0 still match); the sete form leaves all four
// matching. More if-form spellings, all unchanged or worse: result as byte (58) or long (61); (short) length cast,
// result declared first, a copy of address, a byte local for the low byte, empty-if/else, "!(word)(output << 8)"
// (all 14); "!(output << 24)" (28); the value swapped in a local then stored (29); "address &&" first (86).
// GB r16 (fast with all 10 callers/peers MATCH; control 26; 28 real trials): moving 0x7af40 above 0x7aec0, dropping
// `dword &output`, 4 byte-swap commutative orders, `!(char)output`, `!(bool)(byte)output`, 2-var swap splits, `low_zero`
// bool, local inline 0x7af40 copies (single-exit, early-return, case4-first, while-loop) all hit 14 (113/113 B, breaking
// 0x783d0/0x784a0/0x7ab60); 4-term swap 45; `v` local / `bswap` inline helper 16 (and 25 on sete-form, wrong and/shl
// order, not kept); test-before-store 44; if-chain inline 0x7af40 44; short loop index 88; early-return if-form 49;
// `(output & 0xff) == 0` 28 (keeps ECX/EDX in prefix but spills `ipv4_address` to stack, breaking 4 callers); sete
// ternary / no-ref / if-else true-false all tie 26. Restored at 26.
// @retail 0x7aec0
bool function_07aec0(s_type_99af70 const *address, dword *ipv4_address)
{
	dword &output = *ipv4_address;
	bool result = false;

	if (function_7af40(address) && address->address_length == k_ipv4_address_length)
	{
		output = (((address->ipv4_address & 0xff0000) | (address->ipv4_address >> 16)) >> 8) | (((address->ipv4_address << 16) | (address->ipv4_address & 0xff00)) << 8);
		result = !(byte)output;
	}
	return result;
}

// @retail 0x7af40
bool function_7af40(s_type_99af70 const *address)
{
	bool result = false;

	if (address)
	{
		switch (address->address_length)
		{
		case NONE:
		case k_ipv4_address_length:
			result = address->ipv4_address != 0;
			break;
		case k_ipv6_address_length:
			for (long i = 0; i < 8; i++)
			{
				if (address->ipv6_address[i])
					return true;
			}
			break;
		}
	}
	return result;
}

// @retail 0x7af80
bool function_7af80(s_type_99af70 const *a, s_type_99af70 const *b, bool compare_ports)
{
	short length = MIN(a->address_length, b->address_length);

	if (a->address_length > 0 && a->address_length == b->address_length && memcmp(a, b, length) == 0 &&
		(!compare_ports || a->port == b->port))
		return true;
	return false;
}
