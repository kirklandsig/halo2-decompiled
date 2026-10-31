// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_075870.h"
#include "globals.h"

extern short g_485ac0;
extern s_connection_counter g_4e6398;
real g_4cf5d0;

bool network_observer_channel_has_host(s_network_observer *arg_0, long arg_1);
bool function_76520(long arg_0, bool arg_1, long *arg_2);
long __stdcall function_765c0(long arg_0, long arg_1, byte *arg_2);
long link_connection_get_reliable_window(s_link_connection *arg_0);
long link_connection_get_reliable_bytes(s_link_connection *arg_0);
long link_connection_get_reliable_pending(s_link_connection *arg_0);
long link_connection_owner_active(s_link_connection *arg_0);

struct s_75fc0
{
	byte field_0[0x470];
	long field_470;
	byte field_474[4];
	__int64 field_478;
	long field_480;
	real field_484;
	long field_488;
	bool field_48c;
	bool field_48d;
	bool field_48e;
	byte field_48f[5];
	long field_494;
	long field_498;
	real field_49c;
	bool field_4a0;
	bool field_4a1;
	bool field_4a2;
	byte field_4a3[0x4dc - 0x4a3];
	long field_4dc;
};

union s_75fc2
{
	s_connection_counter field_0;
	__int64 field_8;
};

static __forceinline long function_75fc1(real arg_0)
{
	long local_0;
	__asm
	{
		fld arg_0
		fistp local_0
	}
	return local_0;
}

static __forceinline long function_75fc3(s_network_observer *arg_0, long arg_1)
{
    long local_0 = NONE;
    for (long local_1 = 0; local_1 < MAXIMUM_OBSERVER_CHANNELS; local_1++)
    {
        s_network_observer_channel *local_2 = &arg_0->channels[local_1];
        if (local_2->state && local_2->connection_index == arg_1)
        {
            local_0 = local_1;
            break;
        }
    }
    return local_0;
}

// score 408 (was 408): frame 0x3c versus 0x34; observer/channel/result and counter homes remain.
// Earlier: the 388 form below (four call-result and state locals as word, exact
// 1369-byte length) was not kept: retail keeps these in 32-bit homes with no 16-bit operations,
// so the word type is score-picked. Frame 0x3c versus retail 0x34; homes differ.
// Earlier at 408: retail's frame is 0x34 and it holds `this` in edi, forming the channel
// pointer as observer + index*0x528 with the 0xa8 folded into every field displacement; ours has a
// 0x3c frame, spills `this` and adds 0xa8 to the channel base, and composes the counter's halves.
// Earlier: frame 0x3c/0x34; channel/result homes and the counter copy remain.
// Native channel 434; primary/both counter products 448/419; native products 446/423;
// native live flag/callback/both 441/439/432; direct counter product 419;
// both/primary word concatenation 423/431; native both/primary concatenation 415/441.
// Native both concatenation plain 415 at 1350/1369 bytes; products add a call and are restored.
// Output name 415; counter name 415; paired output names 415; later channel 430;
// flag test order 416; output declaration first 415 (fast).
// Search direct returns/earlier result 415; indexed search fields and combinations 419; identity-first test 415. Return byte/short/long 421/409/414; common exit on current channel/counter graph 426. Six-point short gain excluded; original helper, result and exits restored.
// Lane 9 r23: result as a volatile stack byte 419 (search, its jmp/found block and the result's
// argument home follow retail); with `if (local_6 < 48) return` 406; store-only-where-retail
// for all three result updates 435 (449 without volatile); `local_23 < local_13` test 407, both
// tests 416, `!(local_23 >= local_13)` 416; fields through channels[index] 410 at 1369 bytes (retail's
// [esi+0x520] displacements), 423 without volatile; window factor as if 413, with the field form 417. Not kept: the volatile result
// (a volatile local steers the allocator; volatiles are kept only with a byte match). Body unchanged.
// Lane 9 r26: the search helper's `#pragma optimize("g", off)` removed 415 (identical code, kept);
// every `return result` through one exit 408 (kept: retail's entry pushes all four registers before the
// search and keeps the result in the first parameter's slot); counter snapshot through the existing
// union 441, with the reset from it 432; channel fields through channels[index] 415; host flag set in
// the host test's arm 431; the window factor as if/else 408 at 1369 bytes (retail's byte test and
// branch; no gain, not kept); with the union snapshot 439 (frame 0x34 as retail), plus the host-flag
// arm 433.
// Round 148 (counts, then difflib-aligned differences with the baseline at 427): snapshot as the existing
// union copy 445/425; plus the reset as a struct copy of the counter through a union member of field_478
// 459/417; window factor if/else 430/432; channel fields through channels[index] 415/257; index + union
// snapshot + struct reset 440/241; that plus the window if/else 425/230 (frame 0x34, `this` in EDI and the
// search's own channel pointer as retail; left: NONE/index/zero registers, the has-host flag set in its
// test (retail stores false first, 436/226), the 0xe4/0xe8 pick, the 0.0 early exit, the reliable-bytes
// tail); on that, window as `= 0; if` 440/226; 0xe4/0xe8 as if/else 438/237; the 0.0 exit as its own
// return 444/334. Body unchanged.
// Round 222: Persistent configuration/updated counter/both 428/415/432; callback-scale arms 415; configuration-arms/counter-arms/all 424/422/415; all with named window terms 415. All restored; frame 0x3c versus 0x34, channel/result homes and counter composition remain.
// MiniMax D run 2026-10-29: the two flag stores in the other order and the reliable-pending call's
// declaration split from its assignment 407 (gain 1, under the keep bar; not kept).
// Round 270 M: sequential snapshot OR/add low word 408/418; scratch declared first
// 408/418; packet output initialized first 424/420; both entry lifetimes 424/420.
// Same counter bits throughout; original composition, lifetimes and complete body restored.
// Round 314: Persistent cursor with pointer/indexed fields and ternary/branch window: state-first
// 407/430/413/419; identity-first 407/430/413/419. One-count best is rejected; original finder,
// channel/counter/result graph and body restored at 408; frame 0x3c versus 0x34 remains.
// Round 207: canonical host-result byte/word/long 408/404/407; packet-result
// byte/word/long 408/410/425; both word/long 397/424. Both word carriers 397
// plain, kept; independent four-point host gain excluded. Provider definitions,
// call tuples, counter bits, returns and every output are unchanged.
// On paired word results: derived callback/channel-active/forced-send word domains,
// seven nonempty subsets [426, 411, 405, 408, 388, 395, 389]; signed short call results 397.
// The paired call results and callback/secondary-output states as words gave
// 388 plain (not kept, see above); channel-active remained Boolean.
// Round 247: single remaining-budget admission with maximum/available block-quotient
// rounding subsets 427/428/430/429; separate raw/aligned capacity counterparts
// 427/428/430/429. Original tail, counters, result graph and entire body restored.
// @retail 0x75fc0
bool s_network_observer::connection_v05(long arg_1,
	bool arg_2, bool arg_3, bool arg_4, bool *arg_5, bool *arg_6,
	long *arg_7, long *arg_8, long arg_9, byte *arg_10)
{
	s_network_observer *arg_0 = this;
	long local_0 = function_75fc3(arg_0, arg_1);
	bool local_2 = false;
	if (local_0 == NONE)
		goto done;
	s_link_connection *local_3 = link_connection_get(arg_1);
	s_75fc0 *local_4 = (s_75fc0 *)&arg_0->channels[local_0];
	long local_5 = NONE;
	long local_6 = NONE;
	bool local_7 = false;
	bool local_8 = network_observer_channel_has_host(arg_0, local_0);
	if (local_8)
	{
		arg_3 = true;
		arg_4 = true;
	}
	long local_9;
	bool local_10 = function_76520(local_0, false, &local_9);
	long local_11 = g_510548 ? g_51054c : GetTickCount();
	__int64 counter_snapshot = (__int64)(((unsigned __int64)g_4e6398.high << 32) | (dword)g_4e6398.low);
	long local_13 = NONE;
	bool local_14 = (local_3->callback || local_10) && !local_8;
	bool local_15 = local_4->field_48c;
	bool local_16 = false;
	real local_17 = -1.0f;
	long local_18 = NONE;
	if (local_15 && !local_8)
	{
		if (local_4->field_48d && local_4->field_4dc == 2)
		{
			local_16 = true;
			local_14 = true;
		}
		else if (local_4->field_48e)
			local_16 = true;
	}
	if (!local_8)
	{
		if (local_14)
		{
			if (local_15)
				local_17 = local_4->field_49c;
			else if (local_3->callback)
				local_17 = local_4->field_484;
			else
				local_17 = g_4cf5d0;
		}
		else if (!arg_2)
			local_17 = arg_3 ? 4.0f : 0.0f;
	}
	if (local_3->state == 5 && !local_8)
	{
		long local_19 = (local_3->flags & 8) ? 128 : 0;
		long local_20 = link_connection_get_reliable_window(local_3);
		real local_21 = (real)local_20 /
			((real)local_19 * *(real *)((byte *)arg_0->configuration + 0x90));
		if (local_21 < 1.0f)
		{
			real local_22 = *(real *)((byte *)arg_0->configuration + 0x94) +
				(1.0f - local_21) * (*(real *)((byte *)arg_0->configuration + 0x98) -
				*(real *)((byte *)arg_0->configuration + 0x94));
			if (local_17 <= 0.0f || 1.0f / local_22 <= local_17)
				local_17 = 1.0f / local_22;
		}
	}
	if (local_14)
	{
		local_18 = local_15 ? local_4->field_494 : local_4->field_480;
		local_5 = local_15 ? local_4->field_498 : local_4->field_488;
		arg_4 = true;
	}
	long local_23 = g_4e6398.low - (long)local_4->field_478;
	long local_24 = g_485ac0;
	if (local_24 <= 0)
		local_24 = 60;
	if (local_23 < 0)
	{
		local_4->field_478 = (__int64)(((unsigned __int64)g_4e6398.high << 32) | (dword)g_4e6398.low);
		local_4->field_470 = local_11;
		local_23 = 0;
	}
	if (local_17 > 0.0f)
	{
		local_13 = function_75fc1((real)local_24 / local_17);
		local_2 = local_23 >= local_13;
		if (!local_2)
			goto done;
	}
	else if (local_17 == 0.0f)
		goto done;
	long local_26 = NONE;
	if (local_18 >= 0)
	{
		local_26 = (local_23 * local_18 / local_24) / 8;
		local_6 = local_26 - 45;
		local_2 = local_6 >= 48;
		if (!local_2)
			goto done;
	}
	if (local_13 >= 0)
	{
		local_4->field_470 = local_11;
		local_4->field_478 += local_13;
		if (local_4->field_478 + local_13 <= counter_snapshot)
			local_4->field_478 = counter_snapshot;
	}
	local_2 = true;
	if (local_16)
		local_7 = true;
	if (local_5 >= 0 && local_26 >= 0)
	{
		long local_27 = link_connection_get_reliable_bytes(local_3);
		long local_28 = link_connection_get_reliable_pending(local_3);
		if (local_28 > 0 && local_27 + local_26 > local_5)
		{
			long local_29 = local_5 - local_27;
			local_2 = local_29 >= local_27 / local_28 && local_29 - 45 >= 48;
			if (local_2)
				local_6 = local_29 - 45;
			if (local_4->field_48c)
				local_4->field_4a2 = true;
			if (!local_2)
				goto done;
		}
	}
	long local_30;
	if (local_10)
	{
		real local_31 = 1.0f;
		if (arg_2)
		{
			if (local_3->callback)
				local_31 = *(real *)((byte *)arg_0->configuration +
					(link_connection_owner_active(local_3) ? 0xe4 : 0xe8));
			else
				local_31 = *(real *)((byte *)arg_0->configuration + 0xec);
		}
		long local_32 = local_6 >= 0 ? function_75fc1((real)local_6 * local_31) : 512;
		if (local_32 > arg_9)
			local_32 = arg_9;
		if (local_4->field_48c && local_32 < local_9)
			local_4->field_4a1 = true;
		local_30 = function_765c0(local_0, local_32, arg_10);
	}
	else
		local_30 = 0;
	long local_33 = local_6 >= 0 ? local_6 - local_30 : 0x600;
	if (local_6 >= 0 && local_33 <= 48)
		local_33 = 48;
	long local_34 = 0x516 - local_30;
	local_34 -= local_34 % 8;
	if (local_34 < 0)
		return false;
	if (local_33 > 8)
		local_33 -= local_33 % 8;
	if (local_33 > local_34)
		local_33 = local_34;
	*arg_7 = local_33;
	*arg_8 = local_30;
	*arg_5 = arg_4;
	*arg_6 = local_7;
done:
	return local_2;
}
