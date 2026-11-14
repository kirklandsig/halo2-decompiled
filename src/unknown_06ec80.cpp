// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_059ad0.h"
#include "unknown_058dd0.h"
#include "globals.h"
#include <string.h>
#include <xtl.h>

struct s_game_options;
void function_138110(s_game_options *options);
bool __stdcall function_138180(s_session_options const *options);
bool __stdcall function_18e8b0(s_session_options const *arg_0);

union s_6ec80
{
    s_session_options field_0;
    byte field_1[0x1118];
    __int64 field_2;
};

// score 5 (was 5): only the mask's zero differs: ours sits before the guard, retail's inside it.
// MiniMax G (11-06): no new form, no lower score; five more forms that put the mask's zero inside the
// guard — the two tests split into nested ifs, the mask declared outside and zeroed inside, the mask as
// a signed int, the mask as an unsigned long, and the mask declared at the top of the function and
// zeroed in the guard — all score 225. Retail has two `xor esi,esi`: one at the entry, where esi holds
// the zero constant and ecx holds NONE, and one at the guard, which re-zeroes esi for the mask after esi
// has been scratch in the setup block. Our baseline has both as well, but the guard's zero sits on the
// block's dominator. Every form that moves the zero into the block also moves the constant registers
// (NONE to edi, the zero to ecx) and takes the machines pointer out of ebx, so the placement and the
// allocation cannot be won together from this source; the baseline stands.
// Before round 75 (13, first written at 53): the two stack slots were swapped: retail keeps the short at esp+0x14 and
// the pointer at esp+0x10, ours the other way round; the mask is zeroed before the guard.
// The mask scoped inside the guard gives both retail's slots and its zeroing, but then NONE
// and zero take edi and ecx (retail ecx and esi) and the machines pointer is not kept in ebx:
// 223, with the declarations moved 223/223/223/223, with a for loop 348, the counter declared
// first 223, the mask declared first 273. Machines pointer and short as settings fields 171
// (slots right, the short's first store not widened); the existing pair getter for the 4da0
// values 72 (227 scoped); out-parameter getters 13; renamed locals 13.
// Round 32: pointer/short declarations swapped 13, both declared after the settings block 13 (local_9
// for the pointer or local_10 for the short: 13 each, so the slot order is not by name); the short's
// NONE as its declaration's initialiser 42, the pointer's 0 262 (both, harness, no gain); mask scoped
// in an inverted guard 223; with the mask scoped: a players pointer local, the counter declared first,
// the mask declared at the top and zeroed in the guard (harness, 217-219 positional, no gain).
// Round 72 (lane 9, fast): names a_/z_ either way round and an upper-case pointer name 13; the short as
// a ternary or an if/else 245 (981 bytes); the mask declared outside the guard (block or function top)
// and zeroed inside it 223/223 (the same code as scoped); with the mask scoped: a players pointer local
// 223, the counter declared outside 223, explicit NULL tests 223, explicit zero tests 223. Diagnostics:
// a repeated pointer test 13 (folded); the memcpy under `if (local_2)` 68 (977 bytes, slots unchanged),
// so the slots do not follow reference counts here.
// Round 77: a shared short NONE predicate 273 (981 bytes); with the mask scoped: a signed mask
// 223; readiness from the mask/data tests 227; the short initialized at entry 237; the pointer
// initialized at entry 261; both entry initializers 260; a register mask 223; a const machines
// pointer 223.
// Round 31 (lane 17, fast): mask zeroed at entry and again in the guard 223; inverted guard with the
// mask outside it 13; the pointer renamed `machines` 13; the counter zeroed beside the mask before the
// guard 33; the two flags declared first 13. A 600 s permuter run: 4 tries, none below 13.
// Round 163: both slots hold 4 references, and the more referenced home sits lower (word short, compile-only:
// the pointer gets 5 and goes below it), so retail breaks a tie or has one more pointer reference. An inline
// readiness predicate 308 (928 bytes); an inline short getter 13, early return 245, scoped 223/244; the short as
// a long written through a short pointer or as a long/short union 25 (974 bytes, slots unchanged), scoped 222/222;
// pointer and short as the settings' first fields 18 (974 bytes, slots right), scoped 222; a word short 175
// (982 bytes; its NONE is not the shared register), scoped 182.
// Round 75 (lane 1, fast): the settings as plain locals instead of a struct put every slot in retail's relative order
// (59, all 4 too high); retail's hole at esp+0x40 is the 8-byte alignment of the 0x4da0 pair, so the existing pair getter
// with an `__int64` local gave retail's frame: 5 (0x5b120 still matches). On that body: the mask scoped 225; the pair
// zeroed the other way round, chained or declared at their zeroing 225 each; the players test first 225; a for loop 348
// (1059 bytes); the counter zeroed before the guard 88; a ready flag 226; zeroed in both arms 225; mask tested after the
// join 156 (978 bytes); mask default in the else or pre-guard, zero again inside, joined test 226/226, else flag 225;
// cleared on no data 150; the two tests split 156; the mask initialised first in the function 272 (984), beside the
// first zero 272, beside the data pointer's zero 264, before or after the pair 210/51, before or after the arg_1 block
// 209/243, before the name test 246, before the data test 8 (the zero stays where it is written). A forced-inline
// active-player mask helper named 223 on the struct body, in the test 213 (976, data test ahead of the loop), plain
// inline 287 (940, not inlined). `__assume(local_2 != NULL)` at four places 13 each on the struct body (slots unmoved);
// out-param helper for the pair, inline short getter, nested live/flag and range/flag 13 each on the struct body.
// Diagnostics (struct body, scoped, reverted): up to three volatile reads of the pointer or NONE stores leave NONE in
// EDI; `field_c = 0` written after `field_8 = 0` gives NONE ECX at 57. `local_1 = false` replaces the old goto, same code.
// With the mask scoped on this body: the flags set after the opening stores 228, the name zeroed first or last 225/225,
// the opening values initialised at declaration 228, in an inline with out parameters 225. A permuter run on this body
// tried nothing: its baseline (1256 s) and an 818-source reduced image used the whole 2400 s.
// Round 69 (m3 lane 14, fast): a `while (local_15 < 16)` loop with the mask zeroed before the guard 348 (1059 bytes),
// the same with the mask scoped 348; with the mask scoped: the players walked through a `player++` pointer 228,
// `(dword)1 << local_15` 225; the mask declared with the top declarations and zeroed where it is 5 (same code).
// Round 43 (m3 lane 18, fast): on this body a forced-inline active-mask helper called inside the guard 225 (plain
// inline 287, 940 bytes, not inlined, as listed); an inline out-pointer getter for the 0x4f24/0x4f90 blocks (live
// and flag test inside, as 0x6e720's lead) 5 (same code), with the mask scoped 225; the guard as two nested ifs
// with the mask scoped 225; a forced-inline populated-blocks predicate (null tests, mask loop, data test) 226.
// The scoped forms all give NONE EDI for its whole range and drop the machines pointer's EBX range.
// Round 173 (m3 lane 4, fast), all with the mask scoped in the guard: `while (++local_15 < 16)` 225; a word mask 224;
// the bit or-ed through a ternary 240 (974 bytes); the bit named in the loop body 109; the data and mask tests as two
// ifs 228 (977 bytes).
// Round 59 (m3 lane 2, fast), all with the mask scoped in the guard (alone 225: NONE EDI and zero ECX from the entry,
// local_2 in memory, players in EBX): players declared first 225, players declared last 225, local_2 declared last
// 225, the counter declared at the top and zeroed in the guard 225, counter then mask at the guard top 227, the two
// flags declared true flag first 227, a short counter 247 (981 bytes).
// Round 83 (m3 lane 10, fast), all with the mask scoped in the guard (alone 225: NONE takes EDI for the whole
// setup and the frame's second_id/name and short/data slots swap): the block pointer, mask and counter named
// machines/player_mask/player_index 225; the block pointers set in an if/else (no zero ahead of the test) 227;
// the guard tested players first 225, with the if/else pointers 227; a typed player-record pointer for the loop 225.
// Round 75 (m3 lane 15, fast; masked = register-masked aligned count): the scoped mask re-measured 225 (masked 19). On it the
// 0x4da8 value as a ternary 269 (981 bytes, masked 7: NONE goes back to ECX, but the ternary adds a jmp and the two block
// pointers swap EBX/EDI); the data pointer as a ternary 256 (masked 26); the 0x4dac value as a ternary 242 (974 bytes, masked
// 21); all three 254 (masked 13), the same unscoped 254; first/second id NONE chained 225, unscoped 7. Same code as the plain
// reads: inline result-default getters for the 0x4da8/0x4dac/data/short values (225 scoped, 5 unscoped).
// Round 96 (m3 lane 10, fast): the scoped mask's NONE-to-EDI shift comes from the 0x49c8 getter's result-default
// body: with the mask scoped and that read written by hand (`if (live) value_49c8 = arg_0->value49c8;`) NONE and the
// zero stay in ECX/ESI and the first difference moves from +0x17 to +0x101, 195 (left: the hand read compares against
// the NONE register where retail's getter loads into ECX, compares with -1 and re-forms NONE, and the two block
// pointers swap EBX/EDI); the same hand read unscoped 195, scoped with the 0x4d08 getter also by hand 195; scoped with
// only the 0x4d08 getter by hand 225; scoped with the result-default 0x49c8 body as a file-local forceinline helper
// 225. A compile-only harness (dummy first parameter so the session arrives in EDX, getters inline) does not show
// the shift: there the scoped mask keeps NONE in ECX. The 0x49c8 read as `live ? value : NONE` 177 unscoped, 229
// scoped (990 bytes each).
// Round 97 (m3 lane 1, fast), all with the mask zeroed inside the guard (alone 225): the machines block's first
// word read into a named local ahead of the loop 229 (980 bytes: NONE stays in ECX, but the word is then loaded
// into EDI before the loop and the block pointers move to ESI/EBX); an out-pointer getter for the two blocks with
// the null tests, mask loop and data test in an inline predicate taking both pointers 308 (928 bytes, not
// inlined), both force-inlined 226; the mask zeroed between the two pointer tests with a populated flag 227
// (diagnostic: the zero has to be live at the first pointer test for retail's allocation); a non-zero start value
// (diagnostic) 227, so the shift does not come from sharing the zero register; the machines pointer renamed to
// sort after the players pointer 225 (names move nothing).
// A bool for "both blocks present" computed where the two blocks are fetched and tested at the guard, mask zeroed
// inside 251 (985 bytes; it is materialised as a byte, not carried as the two pointer registers).
// Round 115 (m3 lane 16, fast with 0x5b120; control 5), all with the mask scoped in the guard (alone 225: NONE in EDI,
// the zero in ECX, the machines pointer spilled): 0-7 dead initialised locals at the top 225 each (no phase effect on
// this allocation); the two block copies as aggregate assignments in place of memcpy 225; the members-equal bool
// computed where the mode is chosen 219; the member count read at its test instead of a local 115 (the later code
// recovers, the entry roles stay NONE EDI / zero ECX); the local machine read at its store instead of the copy 236.
// Also: with the mask scoped, the member count at its test plus the members-equal bool at the mode choice 164, plus
// the local machine read at its store 238 (960 bytes); unscoped, the member count read at its test 150.
// @retail 0x6ec80
bool function_06ec80(c_class_58d20 *arg_0, bool arg_1)
{
    s_unknown_108 *local_2;
    short local_3;
    byte *stored_data;
    byte *name;
    long session_kind;
    long second_id;
    s_unknown_3648 *players;
    long first_id;
    long value_49c8;
    long value_4da8;
    __int64 value_4da0;
    s_session_machine machine;
    long value_18;
    bool local_0 = false;
    bool local_1 = true;
    first_id = NONE;
    second_id = NONE;
    name = 0;
    value_4da8 = NONE;
    if (session_state_is_live(arg_0))
        value_4da8 = arg_0->value4da8;
    value_4da0 = arg_0->get_values_4da0();
    stored_data = 0;
    if (session_state_is_live(arg_0))
        stored_data = arg_0->data4db0;
    local_3 = NONE;
    if (session_state_is_live(arg_0))
        local_3 = arg_0->value5dd0;
    session_kind = NONE;
    if (session_state_is_live(arg_0))
        session_kind = arg_0->value4dac;
    value_49c8 = NONE;
    value_18 = arg_0->value18;
    arg_0->get_values_4d08(&first_id, &second_id, &name);
    local_2 = 0;
    players = 0;
    if (session_state_is_live(arg_0) && arg_0->flag4f20)
    {
        local_2 = &arg_0->data4f24;
        players = &arg_0->data4f90;
    }
    if (arg_1)
    {
        value_49c8 = arg_0->get_value_49c8();
        if (value_49c8 == NONE)
            local_1 = false;
    }
    if (second_id == NONE)
        local_1 = false;
    if (!name || !*name)
        local_1 = false;
    if (!stored_data)
        local_1 = false;
    dword local_14 = 0;
    if (local_2 && players)
    {
        long local_15 = 0;
        do
        {
            if (((s_session_player *)players)[local_15].active)
                local_14 |= 1 << local_15;
            local_15++;
        } while (local_15 < 16);
        if (!local_2->data[0] || !local_14)
            local_1 = false;
    }
    else
        local_1 = false;
    if (session_kind >= 0 && session_kind < 3 && local_1)
    {
        bool local_16 = arg_0->current_member == arg_0->member_index;
        long local_17 = arg_0->player_count;
        const s_session_member *member = &arg_0->members[arg_0->current_member];
        long local_18 = arg_0->member_count;
        machine = *(const s_session_machine *)((const byte *)member + 0xa);
        if (local_3 != NONE)
        {
            if (local_17 > 2)
                local_1 = false;
            if (local_18 > 1)
                local_1 = false;
        }
        if (local_1)
        {
            long mode;
            switch (session_kind)
            {
            case 0: mode = 1; break;
            case 1: mode = local_16 ? 3 : 2; break;
            case 2: mode = local_16 ? 5 : 4; break;
            default: __assume(0);
            }
            s_6ec80 local_21;
            function_138110((s_game_options *)&local_21);
            s_session_options *local_22 = &local_21.field_0;
            *(long *)(local_21.field_1 + 0x14) = first_id;
            *(long *)(local_21.field_1 + 0x18) = second_id;
            strncpy(local_22->name, (char *)name, sizeof(local_22->name));
            local_22->name[sizeof(local_22->name) - 1] = 0;
            local_22->unknown4 = (char)mode;
            switch (value_18)
            {
            case 0: local_22->unknown5 = 1; break;
            case 1: local_22->unknown5 = 2; break;
            case 2: local_22->unknown5 = 3; break;
            }
            *(__int64 *)(local_21.field_1 + 8) = value_4da0;
            *(long *)(local_21.field_1 + 0x10) = value_4da8;
            if (local_3 != NONE)
            {
                local_22->type = 1;
                local_22->unknown12a = local_3;
                local_22->unknown12c = local_17 > 1;
                local_22->unknown12d[0] = 0;
            }
            else
            {
                local_22->type = 2;
                memcpy(local_22->unknown134, stored_data, sizeof(local_22->unknown134));
                *(long *)(local_21.field_1 + 0x130) = value_49c8;
            }
            memcpy(&local_22->machine_mask, local_2, sizeof(*local_2));
            memcpy(local_22->players, players, sizeof(*players));
            local_22->local_machine_valid = true;
            local_22->local_machine = machine;
            g_510548 = false;
            g_51054c = GetTickCount();
            if (function_138180(local_22) && function_18e8b0(local_22))
                local_0 = true;
            g_510548 = true;
            g_51054c = GetTickCount();
        }
    }
    return local_0;
}

struct s_6ec10
{
    byte field_0[0x10];
    long field_10;
    long field_14;
};

// @retail 0x6ec10
void c_session_state::function_06ec10(c_class_58d20 *arg_0)
{
    if (function_06ec80(arg_0, false))
    {
        s_6ec10 *local_0 = (s_6ec10 *)this;
        local_0->field_10 = g_4e6948->id_a;
        local_0->field_14 = g_4e6948->id_b;
    }
    else
    {
        long local_0 = arg_0->state;
        if (local_0 == 5 || local_0 == 6 || local_0 == 7 || local_0 == 8)
            network_session_set_mode(arg_0, 1);
        else
        {
            volatile long local_1 = local_0;
            network_session_leave(arg_0, false);
        }
    }
}
