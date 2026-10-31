#include "unknown_440dd8.h"
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0820f0.h"
// @flags /O2 /Gr
#include "counters_storage.h"

struct s_structure_bsp_reference;
struct s_bsp3d;
struct s_slot_entry_list;
extern s_bsp3d *g_4e033c;
extern s_slot_entry_list *g_4e0340;
extern s_connection_counter g_4e6398;

void function_d4e60(short arg_0);
void function_137da0();
void function_137d40();
bool cache_files_load_structure_bsp(s_structure_bsp_reference *arg_0);
void cache_files_unload_structure_bsp(s_structure_bsp_reference *arg_0);
void function_16e2c0(long arg_0);

// score 50 (was 50): initial index DI/zero EBX differ from retail BX/EDI, and the saved-index
// store follows the range test instead of preceding it; frame and both result lifetimes agree.
// Round 78: callbacks through the real table field (g_440dd8[i].field_18, made-up view dropped)
// 50, kept; saved copy re-read from the global 50; range test as a guard with goto to the single
// exit 50; success folded into result 130; reload condition on the saved copy 50.
// On the separate-result form: signed ordinal 125; ordinal != 68 51; row cursor 125;
// counted row cursor 95; callback snapshot 50; signed snapshot 125; row snapshot 125;
// counted row snapshot 95. Original dispatch, two index values and returns retained.
// Earlier: retail keeps local_0 in ebx (xor ebx, ebx; mov bx) up to the
// function_137da0 call and stores it at once into arg_0's slot, which then serves as
// local_7; it holds the early false in al (so the limit goes in esi and every push
// comes first) and only later the result in bl, with edi as its zero register. Ours
// keeps local_1 in bl from the start, local_0 in di and zero in edx. Tried: local_0
// and local_7 as one variable 116 (edi becomes the zero register); local_7 = local_0
// at its declaration 143; with one variable, return false for the range test (local_1
// with or without its initializer, declared first or second) 131 (ebx gets retail's
// xor/mov, but the early return gets its own epilogue); return local_1 there 114
// (draft) / 131 (one variable); the body inside if (arg_0 == NONE || in range) 99
// (draft) / 116 (one variable).
// Earlier: result false from entry (al) and the success in bl now match
// retail, and local_7 shares arg_0's slot, but the zero goes in ebx and local_0 in edi
// (retail: local_0 ebx, zero edi), and local_7's store comes after the range test
// (retail: at entry). One variable with result + local_1 128 (retail's registers, but
// its own slot: push ecx); local_7 = local_0 after the loop 87; local_0 read after the
// range test 58; result declared first 120 (100 one variable); local_1 declared at the
// block top 50; local_7 after the true test 50; local_7 in the NONE test 50; local_1 &&
// local_7 != arg_0 for the goto 50 (same code); a 600 s permuter run, none below 50.
// Late byte/short/long success carriers 80/110/109; word/byte callback counters 95/95;
// word counter plus byte success 107; equality-produced success 94;
// equality result plus word counter 128.
// Requested value before/after old index 50/50; early saved index after request
// 128, request-first 128, false-before-save 130, false-before-request 130;
// requested input with wide saved/current carriers 125/122. Separate indices/results retained.
// One index variable 128 (every instruction as retail except its own slot: a variable takes the
// argument's home only when defined after the argument's first read); validity bool first, one
// or two variables 130/130, with the false result first 132; local copy of the requested index
// read first 128.
// Round 16 (lane 10): on the one-variable form, arg_0 copied first 128 and the variable renamed 128
// (still its own slot), the body in an inner block 100. Byte-for-byte match (plain check, all seven
// callers unchanged) only when the saved index lives in the parameter variable itself: copy arg_0
// into a requested-index local, assign arg_0 = local_0 at entry and arg_0 = g_4e6948->index after
// function_137da0, and test the requested local everywhere else. Not kept: a reused parameter.
// Round 406: Initial index/zero registers and saved-index publication remain. Consistent unsigned-
// word current/saved/request equality with 0xffff sentinel, parameter-read ordering, saved entry
// lifetime and byte late-success combinations. Scores 75/75/133/133/91/91/137/137. All restored.
// Round 18 (lane 10): an inline helper for the four global clears (both sites) 50; the saved index
// declared at the top and first assigned in the block 50 (still the argument's home: the def, not the
// scope, decides); the copy used in the early true test 50, in both tests 50; the copy re-read from
// the global 50 (all identical code).
// Round 93: the saved index already shares arg_0's slot; left are local_0/zero in di/ebx (retail
// bx with xor ebx, ebx / edi) and the slot store after the range test (retail: before it, as only
// a reassigned parameter gives). Both NULL-clear blocks as one inline helper 50; early return false
// for the range test 122; saved index declared at function scope 50; early true test on the saved
// copy 50; unload argument written in the call 50.
// Round 38 (lane 9): a named `arg_0 == NONE` part with the saved index defined after it at the top
// 130 (own slot, push ecx); early true test operands swapped 50; `0 <= arg_0` 50; the saved index
// declared at the top and assigned in the block 50. The reused-parameter form (saved index in arg_0,
// a requested copy tested everywhere) 30 here, not kept (a reused parameter).
// Round 100: one index variable assigned after a range guard that jumps to the single exit 142 (the
// load follows the test and the success flag takes a stack byte); saved index declared after the
// callback loop 87; success flag declared before the dispose test 127; the early true through the
// single exit 50 (same code); the global read at each early test with no current-index local 108.
// Round 10 (lane 13): retail's `xor ebx, ebx; mov bx` then a dword store into the slot before the
// range test is the shape of an assignment to the short parameter itself. Without that: one result
// carrier (no separate success flag) 130; the success flag defined before the true test 127; the
// unload pointer in the call 50; the true test's operands swapped 50; a named callback pointer in the
// loop 50.
// Round 99 (lane 6): requested-index copy first with the saved index defined at the top, either order
// of the copy and the current index, 128/128 (retail's BX/EDI and early store, but the saved index
// gets its own slot: the copy is propagated, so the argument's first read follows the definition);
// the copy first with the saved index still in the block 50; saved index defined after `result` 130;
// a named validity bool, then the saved index, then `if (valid)` 130.
// Round 233: phase-local tag table 73; loaded-index snapshot 126; published BSP value 50;
// entry-list value 50; tag/index 122; tag/BSP 50; index/entries 126; all four 126.
// Original index/result lifetimes and global read-backs retained; entry slot and BX/EDI remain.
// Round 30 (lane 11, fast): the saved index read from the global at the top 130 (own slot, registers
// still swapped); the saved index set in an if/else after the callback loop 120; the current index read
// inside the block after the range test, the copy right after it, 58 (457 bytes: BX/EDI and the zero
// extension become retail's and the copy takes the argument's home, but the global read follows the
// range test); the copy at the top in either direction 128/128 (retail's registers and early store,
// own slot). Leads: the registers follow whether the current index is live across the range test
// without an early store; only the argument's home is still missing from the copy-at-top form.
// Round 92 (--fast, reproduces 50): the requested index read once through the argument's address
// (`short const *ref = &arg_0; short requested = *ref;`) with the saved index defined at entry 128
// (own slot: the argument's home is no longer shareable); the address read with the saved index still
// in the block 50; the address read after the current index, before/after the saved index 128/128; the
// saved index as its own read of the global at entry 130. Restored.
// Round 35 (lane 10, fast): saved index as long 83 (462 bytes), current index as word 123 (464) / long 101
// (457); the BSP-globals store as a ternary 50 (same code); the unload argument written in the call 50 (same
// code, kept: it drops a one-use element pointer).
// Round 110 (fast): the range test through a forceinline validity helper taking the index 50; the BSP
// count read into a local at entry 50; the range test before the NONE test in the disjunction 50 (the
// same code); the structure-globals pointer as a ternary 50; the loaded BSP's list index read once into a
// named local for the call and the table 89 (461 bytes).
// Round 37 (lane 10, fast): one index variable at the top 128 (own slot, push ecx); one variable defined as
// the block's first statement 58 (457 bytes: retail's BX/EDI, the zero-extended dword store into the
// argument's home and every later instruction agree; only the global read and that store follow the range
// test, where retail has them at entry, the store between `cmp bp, -1` and its je); that with the early
// true test reading the index first 59; one variable after a named `arg_0 == NONE` bool 100 (459 bytes: the
// store lands between the compare and its jump as in retail, but in its own slot); the body in an inner
// block with the variable at its top 100 (own slot). The argument's home is taken only by a variable
// defined after the range test.
// @retail 0x11c1b0 standard
bool __stdcall function_11c1b0(short arg_0, bool arg_1)
{
    short local_0 = g_4e6948->index;
    bool result = false;
    if (arg_0 == NONE || (arg_0 >= 0 && arg_0 < *(long *)((byte *)g_4e0350 + 0x210)))
    {
        short local_7 = local_0;
        if (g_4686c4 == arg_0 && local_0 == arg_0)
            return true;
        for (dword local_2 = 0; local_2 < 68; local_2++)
        {
            if (g_440dd8[local_2].field_18)
                g_440dd8[local_2].field_18(arg_0);
        }
        if (local_0 != NONE && local_0 != arg_0)
        {
            function_137da0();
            local_7 = g_4e6948->index;
        }
        if (g_4686c4 != NONE && g_4686c4 != arg_0)
        {
            if (arg_1)
            {
                cache_files_unload_structure_bsp((s_structure_bsp_reference *)(*(byte **)((byte *)g_4e0350 + 0x214) + g_4686c4 * 0x44));
            }
            g_4686c4 = NONE;
            g_4e0348 = NULL;
            g_4e0344 = NULL;
            g_4e0340 = NULL;
            g_4e033c = NULL;
        }
        bool local_1 = true;
        if (g_4686c4 != arg_0)
        {
            byte *local_4 = *(byte **)((byte *)g_4e0350 + 0x214) + arg_0 * 0x44;
            g_4e0348 = NULL;
            g_4e0344 = NULL;
            g_4e0340 = NULL;
            g_4e033c = NULL;
            g_4686c4 = arg_0;
            if (cache_files_load_structure_bsp((s_structure_bsp_reference *)local_4))
            {
                function_16e2c0(*(long *)(local_4 + 0x14));
                long local_6 = *(long *)(local_4 + 0x1c);
                if (local_6 != NONE)
                    g_4e0344 = (s_structure_bsp_globals *)g_4e3b44[local_6 & 0xffff].bytes;
                else
                    g_4e0344 = NULL;
                g_4e0348 = (s_match_globals *)g_4e3b44[*(long *)(local_4 + 0x14) & 0xffff].bytes;
                g_4e0340 = *(s_slot_entry_list **)((byte *)g_4e0348 + 0x18);
                g_4e033c = (s_bsp3d *)g_4e0340;
            }
            else
            {
                g_4686c4 = NONE;
                local_1 = false;
                goto local_5;
            }
        }
        if (local_7 != arg_0)
            function_137d40();
    local_5:
        g_4e6398 = g_485ab0;
        result = local_1;
    }
    return result;
}
