// @flags /O2 /Gr /arch:SSE
#include "unknown_2551c0.h"
#include "globals.h"
#include "swarm.h"

struct s_28dab1
{
	byte field_0[0x18];
	long field_18;
};

struct s_28dab2
{
	byte field_0[0x2e];
	short field_2e;
};

long function_1e4990(long arg_0);
long function_28dbd0(long arg_0, long arg_1, long arg_2, s_28dab2 const *arg_3, bool arg_4);
void __stdcall swarm_delete(long arg_0);

// score 44 (was 44): the tag pointer is in ESI where retail keeps it in EDI and the NONE result in EDI
// where retail keeps it in ESI, so NONE stays live into the record stores (retail makes a fresh
// `or eax, -1`) and the loop's zero takes DI early; and retail keeps the loop limit in EBX with a cmovg
// and spills the creature definition index, where ours keeps the index in EBX and spills the limit.
// Retail's record stores run NONE pair, zero pair, first point, event count (the counter's zero),
// second point, flags. Tried: declaration order, the result split from the allocation, the comparison
// against the variable, the goto, the limit's width 53 or worse; retail's store order with the count
// read once 53; tag pointer declared before the result 44; flag byte in a local 44; nested tag test
// 44; locals at the top 53; the record pointer before the tag-field read (retail's head, not its
// limit): ternary 79, `count = 10; if (field > 0) count = field;` 56 (cmovg into ebp), 65 with an else
// tail, after the stores 81 (retail's cmovg ebx, NONE moves to ebx); requested count 74; nonpositive
// bound replaced 97; explicit bound arms 44; sampled object flags 44; guarded tag-field reference 97;
// immutable record receiver 44; completed count 51; immutable receiver and bound 44; point setters
// first/second/both 83/88/92; tag/placement references 44/44; plain while 44; equality termination 45;
// completed-count field reference 44; inline call-boundary declarations 44; the swarm type and getter
// 44, with retail's store order 53, the swarm pointer first 95 (72 with retail's order); the loop
// counter's zero before the stores 108; requested/completed count snapshots and common-value chains
// 44 (eight forms); coordinated handle parameter priorities 44/45/45/44/45; placement argument
// priorities 44/44/44; tag-query/placement/deletion inline declaration subsets (seven forms) and three
// separate noinline declarations 44; behavioural local names 44 (three sets); the if-form count alone
// 89 (cmovg ebx and the index spilled as retail, but the store block takes ECX/EBX); retail's store
// order alone 53 (aligned 17 against 22); both 92 (aligned, padding aside, only the tag/NONE swap,
// the NONE pair stored from the NONE register and the zero pair from the counter's xor remain); early
// returns 97 (NONE is then made after the first call); the result assigned only on success from a
// separate allocation local 89, also on allocation failure 89; on that both-form: the result declared
// after the tag pointer, nested condition tests, a named flag for the type test, the NONE pair
// chained either way 92 each, early `return local_0` for both tests 88, for each test 104; the tag pointer
// stays in ESI even without the NONE stores, without the tag-field test or without the flag test.
// Round 121: the tag pointer through the existing character definition getter 44 (92 on the
// both-form); the getter at each use (the tag pointer is then made after the flag call) 67 (54 on the
// both-form); the field compared against the result 44 (92 on the both-form).
// the tag pointer is in ESI where retail keeps it in EDI (the NONE result in
// ESI, returned by the failed-test exit), and retail keeps the loop limit in EBX with a cmovg and
// spills the creature definition index, where ours keeps the index in EBX and spills the limit (a
// branch, no cmov). Retail's record stores run NONE pair (a fresh `or eax, -1`), zero pair, first
// point, field_1c (the counter's zero), second point, flags. Our NONE register stays live into
// the record stores, so it cannot share ESI with the swarm pointer as retail's does.
// Tried: declaration order, the result split from the allocation, the comparison against the
// variable, the goto, the limit's width 53 or worse; retail's store order with the count read once
// 53; tag pointer declared before the result 44; flag byte in a local 44; nested tag test 44;
// locals at the top 53; the record pointer before the tag-field read (retail's head, not its
// limit): ternary 79, `count = 10; if (field > 0) count = field;` 56 (cmovg into ebp), 65 with an
// else tail, after the stores 81 (retail's cmovg ebx, NONE moves to ebx); requested count 74;
// nonpositive bound replaced 97; explicit bound arms 44; sampled object flags 44; guarded
// tag-field reference 97; immutable record receiver 44; completed count 51; immutable receiver
// and bound 44; point setters first/second/both 83/88/92; tag/placement references 44/44; plain
// while 44; equality termination 45; completed-count field reference 44; inline call-boundary
// declarations 44; the swarm type and getter 44, with retail's store order 53, the swarm pointer
// first 95 (72 with retail's order); the loop counter's zero before the stores 108;
// requested/completed count snapshots and common-value chains 44 (eight forms); coordinated
// handle parameter priorities 44/45/45/44/45; placement argument priorities 44/44/44;
// tag-query/placement/deletion inline declaration subsets (seven forms) and three separate
// noinline declarations 44; behavioural local names 44 (three sets: names never move the
// registers); `count = 10; if (field > 0) count = field;` alone 89 (cmovg ebx and the index
// spilled as retail, but the store block takes ECX/EBX); retail's store order alone 53 (aligned 17
// against 22); both 92 (aligned, padding aside, only the tag/NONE swap, the NONE pair stored from
// the NONE register and the zero pair from the counter's xor remain); early returns 97 (NONE is
// then made after the first call); the result assigned only on success from a separate allocation
// local 89, also on allocation failure 89; on that shape: the NONE pair chained either way 92,
// early `return local_0` for both tests 88, for each test 104; the result declared after the tag
// pointer, nested condition tests, a named flag for the type test 92 each; the tag pointer stays
// in ESI even without the NONE stores, without the tag-field test or without the flag test.
// Round 23 (lane 10): the existing character getter for the tag 44 (same code); the tag read at
// each use with the index read after the swarm pointer 113/114 (NONE then takes ESI as retail, but
// the tag is read after the first call); the index read after the swarm pointer 95, with retail's
// store order and the if-form limit 81. Compile-only screen of 96 body shapes (result
// init/assignment, tag local/at each use, index read at the top/before/after the swarm pointer/
// after the stores, nested/early exit, store order, limit form): none gives tag EDI with NONE ESI.
// else tail, after the stores 81; requested count 74; nonpositive bound replaced 97; explicit bound
// arms 44; sampled object flags 44; guarded tag-field reference 97; immutable record receiver 44;
// completed count 51; immutable receiver and bound 44; point setters first/second/both 83/88/92;
// tag/placement references 44/44; plain while 44; equality termination 45; completed-count field
// reference 44; inline call-boundary declarations 44; the swarm type and getter 44, with retail's
// store order 53, the swarm pointer first 95 (72 with retail's order); the loop counter's zero
// before the stores 108.
// the tag pointer is in esi where retail keeps it in edi
// (the constant -1 is in the other one), and retail keeps the loop limit in
// ebx with a cmov where ours puts it in a stack slot with a branch. Nine
// source rewrites (the declaration order, the result split from the
// allocation, the comparison against the variable, the goto, the width of the
// limit) all scored 53 or worse.
// the same register split. Retail store order with the
// count read once 53; tag pointer declared before the result 44; flag byte
// in a local 44; nested tag test 44; locals declared at the top 53.
// retail's record stores run NONE pair, zero pair, first point,
// field_1c, second point, flags; that order 53. The record pointer before the
// tag-field read with the limit next gives retail's head (tag edi, NONE esi)
// but not its limit: ternary 79; `count = 10; if (field > 0) count = field;` 56
// (cmovg into ebp), 65 with an else tail. That form after the stores gives
// retail's cmovg ebx but moves NONE to ebx 81.
// Tag/sentinel register roles and spilled loop limit
// remain. Native requested count 74; replace nonpositive bound 97; explicit
// bound arms 44; sampled object flags 44; guarded tag-field reference 97;
// immutable record receiver 44; native completed count 51; immutable receiver
// and bound 44.
// tag/sentinel roles and the spilled loop bound remain.
// Existing point setters first/second/both 83/88/92; tag/placement references 44/44;
// plain while 44; equality termination 45; completed-count field reference 44.
// Earlier store-order trial: the fresh-record field stores matched retail's order, but
// the tag pointer is still in esi where retail keeps it in edi, and retail keeps
// the loop limit in ebx with a cmov where ours keeps it in a stack slot.
// Native swarm receiver with requested/completed count snapshots and common-value chains: all
// eight 44; tag/sentinel roles and EBX bound still differ.
// Coordinated handle parameter priorities 44/45/45/44/45; placement argument priorities 44/44/44.
// Every definition/declaration/call changed together; original complete contract restored.
// Tag-query/placement/deletion ordinary inline declaration subsets (seven forms),
// and all three separate noinline declarations: eight at 44. Original declarations restored.
// Round 8: retail's store order alone 53 positional but aligned 34 -> 25; with the limit as
// `short local_4 = 10; if (arg_1->field_2e > 0) local_4 = arg_1->field_2e;` 92 positional, aligned 24
// and register-masked 30 -> 11: retail's cmovg into EBX, the spilled definition index and the loop
// call setup exactly; only the tag pointer/NONE roles (ours ESI/EDI, retail EDI/ESI) remain, which keep
// NONE live into the record stores (retail rematerializes `or eax, -1`) and the loop's zero in DI
// early. Not kept (the positional score rises). On that form: result declared after the tag pointer,
// nested condition tests, a named flag for the type test, chained NONE stores either way 92 each;
// early returns 88.
// Retail's record stores run NONE pair (a fresh `or eax, -1`), zero pair, first point, field_1c (the
// counter's zero), second point, flags. Tried: declaration order, the result split from the
// allocation, the comparison against the variable, the goto, the limit's width 53 or worse; retail's
// store order with the count read once 53; tag pointer declared before the result 44; flag byte in a
// local 44; nested tag test 44; locals at the top 53; the record pointer before the tag-field read
// (retail's head, not its limit): ternary 79, `count = 10; if (field > 0) count = field;` 56 (cmovg
// into ebp), 65 with an else tail, after the stores 81 (retail's cmovg ebx, NONE moves to ebx);
// requested count 74; nonpositive bound replaced 97; explicit bound arms 44; sampled object flags 44;
// guarded tag-field reference 97; immutable record receiver 44; completed count 51; immutable receiver
// and bound 44; point setters first/second/both 83/88/92; tag/placement references 44/44; plain while
// 44; equality termination 45; completed-count field reference 44; inline call-boundary declarations
// 44; the swarm type and getter 44, with retail's store order 53, the swarm pointer first 95 (72 with
// retail's order); the loop counter's zero before the stores 108; requested/completed count snapshots
// and common-value chains 44 (eight forms); coordinated handle parameter priorities 44/45/45/44/45;
// placement argument priorities 44/44/44; tag-query/placement/deletion inline declaration subsets
// (seven forms) and three separate noinline declarations 44; behavioural local names 44 (three sets:
// names never move the registers); `count = 10; if (field > 0) count = field;` alone 89 (now cmovg
// ebx and the index spilled as retail, but the store block takes ECX/EBX); retail's store order alone
// 53 (aligned 17 against 22); both 92 (aligned, padding aside, only the tag/NONE swap, the NONE pair
// stored from the NONE register and the zero pair from the counter's xor remain); early returns 97
// (NONE is then made after the first call); the result assigned only on success from a separate
// allocation local 89, also on allocation failure 89; on the s2 shape (retail's stores and count):
// the NONE pair chained either way 92, early `return local_0` for both tests 88 (NONE is then made
// after the first call), for each test 104; the tag pointer stays in ESI even without the NONE stores,
// without the tag-field test or without the flag test. Round 33: the tag lookup written at both
// uses 67 (it sinks below the flag call); the lookup inside the flag test 53, with the s2 shape 103;
// s2 with the zero pair chained 92; s2 with a separate allocation local and the result set on success
// 89. Compile-only screens: the tag takes ESI over the NONE result whatever the use counts, store
// order, declaration order, nesting, deletions (loop and delete path removed) or a use of the result
// before the lookup; only a non-constant initial result (computed from the argument) gives it EDI.
// without the tag-field test or without the flag test.
// Round 12 (lane 13): in a compile-only screen (which reproduces this head) the NONE result has
// only its definition and the return as references (the NONE compares reuse its register after
// allocation), so the tag pointer always takes ESI: 32 store-order/limit/nesting/tail/declaration
// combinations and 13 diagnostics (no loop, no delete path, no stores, the allocation alone, the
// tag read after the flag test, an index local, compare against the result, early returns) never
// flip it. Real: the tag field read once before the tests 98 (289 bytes), with the if-form limit 93;
// the delete path as an else 44, with the if-form limit 89; the flag test in a named bool 44.
// Round 19 (lane 13): the tag read through the existing character-definition getter 44 (same code);
// with retail's store order and the if-form limit 92 (registers masked 11: only the tag/NONE swap and
// its NONE/zero stores remain); on that, the allocation in its own local with the result set on success
// and the delete path as an else 89; the limit read first and replaced when nonpositive 94; the result
// declared after the tag with the tag-field test nested 92.
// Round 45 (lane 16): the delete path returning NONE directly 96 (first +0xb, the result then shares
// EBP from the start); the NONE result assigned in the failed-test arm of an if/else 92 (NONE made after
// the first call); the success block returning on its own 95 (`--fast`, callee caveat); retail's store
// order with the if-form limit and the delete path returning NONE 92 (plain, first +0xb); the
// result as an unsigned dword 44 (plain, same code).
// Round 177: the tag field compared against the result with the result/tag names in either sort order
// 44/44 (same code), on the s2 shape 92/92 (same code: names never break this tie); the tests as a
// `do { ... break; ... } while (false)` chain 149 (328 bytes), on s2 132; the tag pointer `const` 44 (same
// code); the tag field read at the loop call instead of a local 106 (the tag then takes EBX and the NONE
// value stays in EDI, copied to ESI for the result: NONE prefers EDI even with ESI free).
// Round 24 (lane 14, fast; the 0x28dbd0 call setup is already retail's, EAX definition and ECX
// placement): on the s2 shape (92), the definition index read before the allocation 98; the origin
// point read once into a named pointer for both copies 92 (same code); the record fields stored in
// declaration order 92. On this body, the delete path as an else arm without the goto, count test
// `> 0` first, 56; the same with the `<= 0` delete arm first 44 (identical code, kept: no goto or label).
// Round 115 (lane 6, fast): with the if-form limit (`local_4 = 10; if (field_2e > 0) local_4 = field_2e;`):
// the tag field compared against the result 89, the `> 0` count test first with the delete in the else 89,
// the allocation test written `NONE != local_0` 89, the tag pointer `const` with the field compared against
// the result 89; on this body, the field compared against the result with the `> 0` count test first 56.
// Round 29 (lane 14): the prologue never reads ESI (no hidden register argument). Compile-only:
// `(dword)NONE` record stores, a dword result, a dword tag compare and an inline construction helper
// all keep tag ESI / NONE EDI. Real (fast): the construction helper 170 (207 bytes, not inlined);
// the flag test through TEST_FIELD_BIT 44 (same code); the limit before the allocation 91; `!local_5`
// as the first-iteration flag 44 (same code); swarm_get at each use 149.
// Round 7 (lane 17, fast): the limit as `field <= 0 ? 10 : field` 74; on s2 (92), the record pointer before
// the definition-index read with the ternary limit 72 (289 bytes; the tag then takes EBX), the limit as an
// explicit if/else 53, the definition index read between the allocation and its NONE test 93; the existing
// character getter with the tag pointer declared before the result 44 (same code).
// Round 54 (lane 16, plain): the swarm getter at each use in place of the record pointer 149 (338 bytes);
// the record stores in struct order (actor, position, counts, first creature, event count, gather point,
// flags) 82; the NONE pair chained 44 (same code); a long loop index 75; the creature count read once into
// a short local for the test and the copy 44 (same code).
// Round 115 (lane 6, fast): with the if-form limit (`local_4 = 10; if (field_2e > 0) local_4 = field_2e;`):
// the tag field compared against the result 89, the `> 0` count test first with the delete in the else 89,
// the allocation test written `NONE != local_0` 89, the tag pointer `const` with the field compared against
// the result 89; on this body, the field compared against the result with the `> 0` count test first 56.
// Round 41 (lane 11, fast; compile-only screens agree with each): the tag pointer read through a named tag
// instance element pointer 44; the first block's flag byte through a named pointer 44; arg_0 as a dword
// 44; the allocation alone under the head test with the swarm setup under a separate `if (local_0 !=
// NONE)` 44, on s2 92; s2 again 92 (all identical heads: tag ESI, NONE EDI). Compile-only diagnostics:
// a zero initial result, the result declared after the tag pointer, the field test ahead of the flag
// call, no use of the result at all, and the argument as the initial value (then held in EBX) all
// still give the tag ESI.
// Round 119 (fast; the 0x28dbd0 callee carries a reduced-image caveat): s2 (retail's store order, if-form limit)
// 92 again; on s2 the tag lookup after the flag call 103 (the lookup sinks below the call). Record pointer
// first, then the definition index, with the if-form limit after the stores: retail's store order 81 (tag EDI
// and the result in ESI as retail, but the NONE constant splits off into a pushed EBX, `mov esi, ebx`, and
// feeds the compares and record stores); the definition index after the stores, before/after the limit 96/96;
// with the original store order 93, the index after the stores 90. A compile-only screen does not reproduce
// the record-first head (tag/NONE swap in all 64 placements: head window 6-10).
// Round 14 (lane 17, fast; all on s2, 92, identical code each): a const tag pointer with `NONE != field`;
// the allocation folded into the head condition; the allocation pool read once into a local for the
// allocation and the record address; the result declared uninitialised and set NONE after the tag
// pointer; the record pointer declared at function scope.
// @retail 0x28dab0
long __stdcall function_28dab0(long arg_0, s_28dab2 const *arg_1)
{
	long local_0 = NONE;
	s_28dab1 *local_1 = (s_28dab1 *)g_4e3b44[arg_0 & 0xffff].bytes;
	if ((*(byte *)function_1e4990(arg_0) & 1) && local_1->field_18 != NONE)
	{
		local_0 = record_pool_allocate(g_5044c8);
		if (local_0 != NONE)
		{
			long local_2 = local_1->field_18;
			s_swarm *local_3 = swarm_get(local_0);
			local_3->gather_position_valid = false;
			local_3->spawn_slot_reference = true;
			local_3->creature_count = 0;
			local_3->initial_creature_count = 0;
			local_3->event_count = 0;
			local_3->actor_index = NONE;
			local_3->first_creature_index = NONE;
			local_3->position = *g_468788;
			local_3->gather_position = *g_468788;
			short local_4 = arg_1->field_2e > 0 ? arg_1->field_2e : 10;
			for (short local_5 = 0; local_5 < local_4; local_5++)
				function_28dbd0(local_0, arg_0, local_2, arg_1, local_5 == 0);
			if (local_3->creature_count <= 0)
			{
				swarm_delete(local_0);
				local_0 = NONE;
			}
			else
			{
				local_3->initial_creature_count = local_3->creature_count;
			}
		}
	}
	return local_0;
}
