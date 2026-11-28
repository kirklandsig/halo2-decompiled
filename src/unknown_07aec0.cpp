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
// GA14 (40 real + 1 diag, fast with 0x783d0/0x784a0/0x7ab60/0x7acc0/0x7acf0/0x7af40/0x7af80): if-form variants without
// output ref, *(byte *)ipv4_address, ((byte *)&output)[0], commuted swap operands, 0x7af40 defined above 0x7aec0,
// (byte)output == 0, (char)output, renamed params/locals, nested if without ref, embedded assign, goto done, addr ref
// inside, inline/forceinline byte-check or convert wrapper, (word)address_length, (byte)(*ipv4_address & 0xff), shared
// header include, valid local, and 0x7af40 long i / case order all hit 14 (113/113 B, identical tail, ECX/EDX swapped
// in inlined 0x7af40 prefix, breaking 0x783d0/0x784a0/0x7ab60); inline/forceinline bswap 16; ternary / if-else false
// 26; local union 28 (spills out); if-ret-false 42; 4-term bswap 45; switch length 46; early ret false 49; MSB/mask
// check 54; diag #pragma optimize("s") 92. Body restored at 26.
// GB r16 (fast with all 10 callers/peers MATCH; control 26; 28 real trials): moving 0x7af40 above 0x7aec0, dropping
// `dword &output`, 4 byte-swap commutative orders, `!(char)output`, `!(bool)(byte)output`, 2-var swap splits, `low_zero`
// bool, local inline 0x7af40 copies (single-exit, early-return, case4-first, while-loop) all hit 14 (113/113 B, breaking
// 0x783d0/0x784a0/0x7ab60); 4-term swap 45; `v` local / `bswap` inline helper 16 (and 25 on sete-form, wrong and/shl
// order, not kept); test-before-store 44; if-chain inline 0x7af40 44; short loop index 88; early-return if-form 49;
// `(output & 0xff) == 0` 28 (keeps ECX/EDX in prefix but spills `ipv4_address` to stack, breaking 4 callers); sete
// ternary / no-ref / if-else true-false all tie 26. Restored at 26.
// M3 lane 13 r160 (fast with the five callers/peers above; control 26): reading from the allocator's ranking, the
// address takes ECX only while its priority (32 here) beats the inlined 0x7af40 loop counter's 31; the if-form tail
// moves the result's last reference out of the swap block, one candidate fewer there, so the address drops to 30 and
// the counter takes ECX first (14, the address in EDX). Tried: `result = false;` again in the arm ahead of the if-form
// test 20 (the address in ECX and every caller matching, but the swapped value moves to EAX; a dead store, not kept);
// `result` declared bare and set false in both arms 48 (callers lost); `result` initialised from the two tests and
// reused for the low-byte test 48 (114 B); `if ((byte)output == 0) result = true;` 14 (as the if-form); a `dword
// swapped` local stored and tested 28 (four callers lost). All restored.
// Round 171 (if-form tail, fast with the same five callers/peers): a forced-ranking search on the if-form matches
// with one decision, the address up two (30 -> 32) or the inlined loop counter down two (31 -> 29); the swap block
// holds four candidates (the address, the value, the output pointer, zero), and a fifth one there would give the +2,
// but every spelling stays at 14 (address in EDX, 0x783d0, 0x784a0 and 0x7ab60 lost): the field read through `dword
// const &value = address->ipv4_address`, through `s_type_99af70 const &source = *address`, stored and tested through
// `*ipv4_address` with no output reference, a `dword *output` pointer for the store and test, `result` declared
// before the output reference.
// Round 233 (if-form tail): the address's priority is 30 (swap block +8 at four candidates, live but unread -2 in the
// two blocks that compare the shared constant 4, -4 in the loop step) against the counter's 31; on the if-form, the
// length test against sizeof(address->ipv4_address) leaves both (the 4 stays one constant; 14); 0x7af40's ipv6 hit as
// `result = true; break;` 14; the length test written constant-first 14 (both lose 0x783d0, 0x784a0 and 0x7ab60);
// 0x7af40's ipv4 case as an early return 90 (0x7af40 itself lost).
// M3 lane 15 r155 (h2m fast with the ten callers/peers; if-form 14, address EDX). Ledger on the if-form: the address
// 30 (entry, switch head, loop body at weight 2, ipv4 test, swap block) against the counter 31 (its init block shares
// the zero constant, the latch the bound 8); a forced-ranking search finds only counter -2 or address +2. Tried on the
// if-form: a file-local inline byte-swap function 16; a named value local for the swap 16 (coalesced, address still
// EDX); the 0x7af40 result in a named bool 14; the field stored to the output first and swapped in place 14; two early
// false returns 49 (111 B). Diagnostic: the function as a const member of the address record, `this` the address, 14
// (LTCG does not pin `this` to ECX).
// M3 lane 5 r256 (h2m fast with the seven callers/peers; if-form 14, address EDX): ledger again address 30 against the
// inlined counter 31, and a forced-ranking search matches with one decision only (counter -2 or address +2). On the
// if-form: the output as a `dword &` reference parameter (header and the four call lines changed) 14; 0x7af40's switch
// on a short length local 14, on a long length local 14 (0x7af40 still MATCH); nested tests with a short length local
// read after the call 14; the swap split into `output = ...; output |= ...;` 41 (115 B). All restored.
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
