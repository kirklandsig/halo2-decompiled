#include "unknown_11c920.h"
#include "ai_actor.h"
#include "props.h"
#include "unknown_0259a0.h"
#include "unknown_2626b0.h"

// @flags /O2 /arch:SSE /Gr

#include "unknown_1af160.h"

struct s_1f4a20_entry;
struct s_1f4a20_source;
void *function_1e4be0(long arg_0);
bool __stdcall function_1ac610(long arg_0, point3f const *arg_1, s_object_marker *arg_2, short *arg_3);
bool function_1f4a20(s_1f4a20_entry const *arg_2, long arg_0, s_reference arg_1, s_1f4a20_source const *arg_8,
    bool arg_3, s_type_c3b527 *arg_4, vector3f *arg_5, signed char *arg_6, bool *arg_7);
bool function_1af530(long arg_0, long *arg_1, long *arg_2);
void function_265c30(long arg_0, long arg_1, bool arg_2);

// score 12 (was 12): the three dword homes still rotate against retail's (ours: flag,
// temp, actor, result; retail: actor, temp, result, flag); this round's source
// permuter (19 tries, 670 s) found no lower form of that order.
// Earlier: the frame homes for the actor, the result and the completion flag
// still rotate against retail's (ours: flag, temp, actor, result; retail: actor,
// temp, result, flag) although each of the four locals is read the same number of
// times; this round's trials in this file and the source permuter found no lower
// form of that order.
// Earlier: actor/source/completion homes rotate +0x2c/+0x30/+0x24 versus
// +0x24/+0x2c/+0x30. The frame and native conversion agree.
// Earlier layout/conversion observations and trials: the locals sat eight bytes lower, so the
// retail rounds the delay with its own fld/fistp straight into the long that
// the locals sit at stack addresses eight bytes lower than retail's, so the
// values handed to the calls are taken from the wrong slots, and retail converts the scaled delay
// inline (fld, fistp into local_13's dead slot, mov dx) where ours calls real_to_long.
// `static __forceinline long real_to_long(real value);` declared here gives retail's code
// instruction for instruction (44, all frame offsets: ours pairs the fistp temp with the actor
// offset's slot, so local_16 needs its own and the lower slots move by 4; retail also has local_13
// below local_4 and local_2 below local_14). With it: local_16 before local_17 46; local_13
// declared at the top 44; the product passed straight to the conversion 44; names swapped 44
// (names do not order this frame); a separate local for 0x1af530's unused output 44 (67 without).
// the 0x163c frame agrees, but scalar stack homes and the conversion
// helper boundary differ from retail.
// Round 197 fast trials (unchanged reduced preview 207; full 36): Native reference member
// comparisons, short-circuit/bitwise tests x aggregate/member copy x original/forced-inline
// conversion 279, 275, 348, 350, 346, 353, 402, 405.
// Round 77 (plain checks): forceinline conversion declaration 44 (retail's inline fld/fistp; only
// frame slots then differ: retail pairs the offset temporary with local_16 and local_13 with the
// conversion result, and orders local_1, local_14, local_2 at the top); with 0x1af530's second
// output in its own block local 44; local_14 renamed 44 (names move nothing); with the +0x14
// result field read once 46; the separate output local alone 67.
// Round 16 (plain, all with the forceinline declaration): the frame is one dword larger than
// retail's: retail packs the fistp temporary, 0x261280's long output and 0x1af530's output into
// one slot and the 0x262b40 result into the actor offset's; ours packs the fistp temporary and a
// separate 0x1af530 output into the actor offset's slot and gives the 0x262b40 result its own.
// The conversion result in a named long 44; the separate output declared before the real 44;
// the bool for 0x1f4a20 declared at the top 44; the long and bool outputs of 0x261280 declared
// the other way round 44; 0x261280's outputs scoped to a block ending after 0x2626b0 (the empty
// result branch returning early) 206.
// retail rounds the delay with its own fld/fistp straight into the long that
// also takes 0x261280's count output and 0x1af530's second output (the matched 0x1bd230 writes the
// same random-delay rounding in its body); the real_to_long helper's separate result cannot share
// that home. Left: three dword homes rotate (the actor spill, ours 0x2c / retail 0x24; the
// 0x25d690 result spill, 0x30 / 0x2c; the 0x261280 bool output, 0x24 / 0x30): retail sorts the
// bool output last, ours first, probably because it reaches the stubbed 0x260670 (BLOCKED-BY
// 0x260670, whose real body is disabled in the held src/unknown_261280.cpp).
// Tried: `static __forceinline long real_to_long(real value);` 44 (retail's fld/fistp, but the
// helper's result shares the actor offset's home, so every small slot moves): with local_16
// before local_17 46; local_13 declared at the top 44; the product passed straight to the
// conversion 44; names swapped 44; a separate local for 0x1af530's unused output 44 (67 without);
// local_14 renamed 44; the +0x14 result field read once 46. Native reference member comparisons,
// short-circuit/bitwise tests x aggregate/member copy x original/forced-inline conversion 279,
// 275, 348, 350, 346, 353, 402, 405 (fast). `local_13 = real_to_long(...)` with the forceinline
// helper 148 (981 bytes), without it 133; the 0x261280 outputs in a block closing after the
// 0x2626b0 call (equal case returning early) 206. Round 41: the in-body fld/fistp into a
// block-scoped long 44 (as the forceinline helper); into local_13 9 (kept). On that form: the bool
// output declared first 9; it renamed 9 (names do not order these homes); the existing actor and
// prop-node getters 9 (identical code, kept); the 0x262b40 result declared before its flag 13;
// a separate long for 0x1af530's second output 45.
// Round 204 (all-image fast): completion initialized 205; chained false outputs 13;
// acceptance producer 174; const source 9; guarded two-use key 9; complete role names 9;
// role names/completion initialized 205; role names/guarded key 9. Original body restored.
// a separate long for 0x1af530's second output 45. Round 43: descriptive names for the actor,
// location and flag 9; the 0x2626b0 call's test nested or as `!(a || b)` 9/9; the flag declared
// after the actor 9; the outputs declared before the scratch buffer 9. A no-/GL /FAs build orders
// these homes by weighted use count (names and declaration places never move them). Diagnostic,
// reverted: 0x2605d0 not passing the flag pointer on to the stubbed 0x260670 gives 36 with the
// flag's home moved, so the homes follow what LTCG knows of that callee chain (BLOCKED-BY 0x260670).
// Round 86 (all-image fast): call-only and two-use Boolean output pointers 9/9; one-element
// Boolean output array 9; count/Boolean declarations swapped 9; derived record declared before
// the node 9; search result declared before the outputs 9; node and derived declarations before
// the actor 9; const actor pointer 9. Every form has identical differences; original restored.
// Round 114 (lane 6): the 0x260670 stub writing its flag output first, as the disabled body does,
// 9 (plain; its callers unchanged), so a written output alone does not move the homes; 0x1f4a20's
// bool output declared at function scope 209 (975 bytes); the actor local for the equal-arm
// unknown040 store 256 (955 bytes); a fresh actor lookup for the unknown4ae store 176 (985 bytes);
// the prop position pointer named once for 0x1ac610 and 0x210ac0 227. Original body restored.
// Round 54 (lane 10, plain; fast is unusable here, control 209): result declared first 187; both
// 0x1f4a20 flags declared before the 0x262b40 result 9 (same code); the five cleared flags as one
// chained assignment 13; the result declared right before the search-result test 58; the 0x1af530
// range output declared at the top of the found branch 9 (same code).
// Round 51 (m3 lane 14, plain; fast control 209): the reference copy declared after the 0x1e4be0 result
// 14 (the field read moves after the call; homes unchanged); confirmations of earlier forms: the bool
// output declared first 9, both 0x261280 outputs declared at the top 9. Retail calls 0x25d690 twice as
// ours does. Still BLOCKED-BY 0x260670: its real body is disabled in src/unknown_261280.cpp (held).
// Round 96 (plain): second source result assigned to the existing pointer 9; second position expression
// direct 9; equal-case actor lookup assigned to the existing actor pointer 9; search result assignment
// rather than copy initialization 9; saved reference assignment 9; Boolean output explicit predicate 169;
// source pointer declaration split 9; actor pointer declaration split 9. Original body restored.
// Round 162 (lane 4, plain; fast control 209): the three homes rotate because the bool output sorts with the
// address-taken locals in ours and with the spilled pointers in retail. The output as long, byte or char with a cast
// at the call 179/169/169; a named copy of it for 0x2626b0 9; a bare scope around the three outputs 57, around the bool
// alone 9. Still BLOCKED-BY 0x260670 (its stub keeps the bool's address escaped).
// Round 148 (m3 lane 5, fast with 0x1b0540's chain in the list; other lists read 210): no longer blocked: 0x260670's real body is in src/unknown_260670.cpp, 12: two home
// pairs swapped against retail (actor pointer with the actor offset, the prop node with the bool output), where the stub gave 9 with three rotated. The 0x1e4be0 result declared
// apart from its call 12; the second 0x25d690 result passed straight 12; the five cleared flags written `false` 12; the outputs bool first 12; a named copy of the bool 12.
// Round 150 (m3 lane 6, plain): the two swapped pairs are ties on reference count (actor pointer 4 against the offset temp's
// shared slot 4; prop node 2 against the bool output 2). Names, re-checked once for this two-pair state: the bool output to sort
// first 12, the prop node to sort last 12, the actor to sort last or first 12/12, all three 12 (same code). Weight forms: the second
// 0x25d690 call on a fresh prop node lookup 87 (1002 bytes), the range test on a fresh lookup 236 (998 bytes), the equal-arm flag
// test on a fresh actor lookup 61, the prop index named for its two uses 256. A one-file harness with the callee files and the
// ballast is not exact (275, 959 bytes). Open lead: pointer-typed outputs on 0x2605d0/0x260670 need a prototype edit in held
// src/unknown_261280.cpp.
// Round 139 (m3 lane 8): `--fast 0x1af160 0x1e1f20` reads the true 12 (without 0x1e1f20 it reads 210). In both swapped
// pairs ours gives the lower home to the one referenced first in code (the offset temp before the actor, the node before
// the bool output) and retail to the other. Names do not move them (descriptive or one-letter, four locals renamed: 12).
// The 0x262b40 result declared at the function's top 12; the equal-arm test inverted 29; the result flag declared first
// 187; the 0x261280 outputs declared before the entry 12; 0x1af530's real output declared at the top 12; the two
// 0x1f4a20 flags declared the other way round 12. Original body restored.
// Round 143 (m3 lane 8, fast with 0x1e1f20, control 12): stack variables moved to function scope, to test whether the tied
// home order follows the frame's variable set: the 0x1af530 real output, the distance vector, the rounding input real,
// the two 0x1f4a20 flags, the 0x1e1f20 result: 12 each (identical code). The bool output's home follows its callee chain
// (round 43's diagnostic), and 0x260670 on that chain is still 700 differences and 71 bytes long, so what LTCG knows of
// it is not retail's: a lead, likely BLOCKED-BY 0x260670 until that body matches (round 148 enabled it, still unmatched).
// Round 182 (m3 lane 4, fast with 0x1e1f20, control 12; masked aligned 12: only the two swapped home pairs, the actor
// against the offset temp's slot shared with the 0x262b40 result, and the prop node against the bool output): the
// 0x265c30 call's prop index through a fresh actor lookup 77 (984 bytes; the frame shrinks), the search record as
// `= {0}` for its memset 231 (978 bytes), the saved reference copy declared const 12, the repeated found test held
// in one named bool 177. No untried natural form left in this body: the levers are 0x260670 (src/unknown_260670.cpp,
// another lane's file this round) and the held src/unknown_261280.cpp prototypes. Still BLOCKED-BY 0x260670.
// Round 116 (m3 lane 12): no trial. Both swapped pairs are use-count ties in retail too (the actor spill and the offset
// slot four references each, the prop node and the bool output two each), and every natural use-count change is listed
// above with its score. The levers left are outside this body: 0x260670 (score 440 at 3097 bytes against 3077) and the
// output prototypes in the held src/unknown_261280.cpp.
// Round 177 (m3 lane 6, `--fast 0x1af160 0x1e1f20`): the 0x261280 bool output initialised false 201 (979 bytes); its long
// output initialised 0 235 (980 bytes); the 0x262b40 result as a const pointer 12; the 0x1e4be0 result kept `void *` 12; the
// prop node as a const pointer 12. A compile-only lab is not faithful here (without LTCG the offset stays in EBP).
// M3 lane 14 r97 (`--fast 0x1af160 0x1e1f20`, control 12): the equal-arm store through the array-index actor getter
// 12 (same code); the prop node through the existing prop_ref_get as `s_prop_datum *` with its two casts dropped 12 (same
// code, not kept); the node getter's body written out 12 (diagnostic: inline return temporaries do not order the tie); the
// top actor through the array-index getter 12 (diagnostic, same code); no asm (a plain cast and the helper call) 135
// (diagnostic: the actor then takes retail's 0x24 but the offset moves to 0x20). The permuter cannot search this
// function: its reduced image scores 209 even with 0x1e1f20 as context.
// m3 lane 14 r99 (`--fast 0x1af160 0x1e1f20`, control 12): the definition range test written `node < limit` 12 (same
// code); the squared-distance test written `limit * limit > length` 30; the 0x1af530 range test written `call < output`
// 12 (same code); the equal-arm store's actor lookup written as the getter's expression 12 (same code: the inline getter's
// temporary does not order the offset's home); the 0x1e4be0 result as `dword const *` 12 (same code). 0x260670 on the
// bool output's chain is still 440 at 3097 bytes against 3077.
// m3 lane 5 r188 (`--fast 0x1af160 0x1e1f20`, control 12; ours homes the actor offset 0x24 / actor 0x28 / node 0x30 /
// bool output 0x34, retail 0x28 / 0x24 / 0x34 / 0x30): the equal arm's result flag cleared before the actor store 12
// (same code); the marker pointer as a NULL default and an if 236 (982 bytes); the two opening tests nested 12 (same
// code); the definition's range field named once for its three uses 65; the 0x265c30 flag declared inside the found
// test 19. Body unchanged.
// m3 lane 15 r93 (`--fast 0x1af160 0x1e1f20`, control 12; the same two swapped home pairs): the equal arm's two clears as one
// chained `local_5 = actor_get(arg_0)->unknown040 = false;` 12 (same code); the saved-reference test with the macro's operands
// swapped 13; the 0x261280 outputs declared bool before long, all three ahead of the scratch buffer 12 (same code); the search's
// +0x59 flag as a ternary on the 0x1e4be0 result 222; the search's +0x698 bool stored before the +0x69a short 14. Still the
// homes tie (0x260670, ours, is 440 at 3097 bytes against 3077, on the bool output's chain). Body unchanged.
// m3 lane 13 r103 (`--fast 0x1af160 0x1e1f20`, control 12; reference bindings as in the contributor's slot forms, each
// match-only): the prop node bound by reference for its two later uses 12; the bool output bound by reference for the
// 0x2626b0 argument 12; the bool output's address in a pointer local for the 0x261280 call 12; the actor bound by reference
// for the node lookup 12 (all the same code); re-measured on the real 0x260670 body, the three 0x261280 outputs declared at
// the function's top 12 (same code). 0x260670 (ours) is still 440 at 3097 bytes against 3077.
// m3 lane 15 r96 (`--fast 0x1af160 0x1e1f20`, control 12; the same two swapped home pairs): the node looked up through a
// fresh actor lookup 79; a block around the 0x261280 outputs and everything that uses them (ending before the scratch release)
// 44; the search record declared first in the function 12 (same code); the scratch buffer fetched before the search record's
// setup 249 (978 bytes); the 0x1ac610 short output array scoped to its call 12 (same code). Re-measured by mistake: the
// equal-arm flag test on a fresh actor lookup 61 (round 150). Body unchanged.
// m3 lane 9 r137 (`--fast 0x1af160 0x1e1f20`, control 12; masked 0: only the two swapped home pairs, the actor 0x28 / offset temp 0x24
// and the node 0x30 / bool output 0x34 against retail's 0x24 / 0x28 and 0x34 / 0x30): the 0x25d690 result as a const pointer 12; the
// search's +0x59 flag as an if/else 12; the 0x261280 result declared ahead of its outputs and assigned at the call 12 (all the same
// code); the scratch buffer fetched before the marker pointer is chosen 257 (978 bytes); the two 0x1f4a20 flags' declarations swapped
// around the 0x262b40 result 15. Each tied pair holds four and two slot references in both builds, so the order still looks like
// the allocator's tie-break; 0x260670 on the bool output's chain is still 440 at 3097 bytes against 3077.
// M3 lane 12 r139 (`--fast 0x1af160 0x1e1f20`, control 12): fresh actor lookups to give the actor fewer uses, none folding
// back (calls sit between): the saved reference copy 295 (990 bytes), the first test's +0x3c flag 297 (994 bytes), its
// unknown227 306 (1002 bytes), the first two together 306; the node looked up again after the first 0x25d690 call, its first
// lookup inline in that call's argument, 311 (1003 bytes). Body unchanged.
// M3 lane 7 r180 (`--fast 0x1af160 0x1e1f20`, control 12): 0x2605d0's and 0x260670's two integer outputs typed as pointers
// (`void *`, `long *`) at both definitions, every declaration and all twelve call lines, the casts dropped, 42 (the pairs stay
// swapped and the low slots 0x12-0x1c move, so retail's frame is the escaped-integer one; the other eleven callers stay
// MATCH; restored); the actor pointer const 12, the actor, node and 0x25d690 result const 12 (same code); the 0x262b40
// result's +0x14 field named for its two tests 14; the three 0x261280 outputs declared ahead of the scratch fetch 12.
// M3 lane 16 r141 (`--fast 0x1af160 0x1e1f20`, control 12): 1..8 dead initialised locals at the top 12 each; diagnostics in
// 0x260670 (reverted): its read of the flag output dropped 12, the flag pointer stored to a global 9 (the bool output then takes
// the lowest home; retail's 0x260670 reads and writes the flag as ours does, so the lever is elsewhere on that chain); the bool
// output passed as `local_14 != false` 172 (978 B); the definition range test with its operands swapped 12 (same code).
// M3 lane 13 r113 (`--fast 0x1af160 0x1e1f20`, control 12): the bool output alone in a block spanning its uses (the
// reference result declared outside) 12; the actor offset `(arg_0 & 0xffff) * size` as a named local at both actor lookups
// 12, renamed to sort last 12 (the CSE temporary's slot does not follow a user name); the three outputs in a block with
// the reference result outside 44; the node looked up through a fresh actor lookup ahead of the actor local 79; the
// equal arm's actor lookup through a named local 12 (same code).
// M3 lane 4 r220 (`--fast 0x1af160 0x1e1f20`, control 12; homes ours offset 0x24 / actor 0x28 / node 0x30 / bool output
// 0x34, retail actor 0x24 / offset 0x28 / bool output 0x30 / node 0x34): the bool output declared at the top with a const
// pointer to it passed to 0x261280 12; the two marker/short arrays declared ahead of the cleared flag 12, the short array
// ahead of the marker array 12; the 0x261280 result declared bare at the top and assigned at the call 12 (same code each);
// the bool output passed to 0x2626b0 as `local_14 != false` 172 (978 bytes). Re-measured: the three outputs declared at
// the top 12, the node pointer const 12. 0x260670 (ours, on the bool output's chain) is still 440 at 3097 bytes against
// 3077 (frame 0x8a8 against 0x8a0).
// M3 lane 13 r122 (`--fast 0x1af160 0x1e1f20`, control 12; merging same-role locals to change the frame's symbol set, the
// lever that fixed 0x12eb00's tied homes, each match-only): the second 0x25d690 result assigned to `local_2` (no `local_28`)
// 12; the 0x265c30 flag carried in `local_18` 12, in `local_17` 33; the 0x1e1f20 result carried in `local_13` 72 (978
// bytes); the rounding input sharing 0x1af530's real output (one real declared at the arm's top) 44. The pairs stay swapped.
// M3 lane 16 r147 (`--fast 0x1af160 0x1e1f20`, control 12): the delay rounding through a forceinline seconds-to-ticks
// helper (the tick-rate product and fld/fistp inside) assigned to the long output 134 (981 bytes), its result stored
// straight to the record's short 25 (the helper's result takes its own home); the 0x1e5280 completion flag declared
// inside its test 19 (its true store moves after the call); the rounding input declared at the top of the non-equal
// branch 12 (same code); the result flag declared after the 0x261280 call 62.
// M3 lane 6 r208 (`--fast 0x1af160 0x1e1f20`, control 12; reference bindings, each match-only): the 0x262b40 result's +0x14
// field bound by const reference for its two tests 12; the actor bound by const reference for the 0x265c30 prop index 12;
// the 0x262b40 result bound by const reference for the 0x210ac0 call 12; the long output bound by const reference for the
// 0x2626b0 call 12; the marker pointer bound by const reference for the 0x1f4a20 call 12. 0x260670 (ours, on the bool
// output's chain, in another lane's file this round) is still 440 at 3097 bytes against 3077.
// M3 lane 12 r154 (`--fast 0x1af160 0x1e1f20`, control 12; `const &` bindings, each match-only): the 0x262b40 result read
// through one for its two +0x14 tests and the 0x210ac0 argument 12; the prop node, the actor, both, and the 0x25d690 result
// each bound straight to its lookup's result (a hidden temporary) 12/12/12/12 (all the same code). 0x260670 is still 440.
// M3 lane 12 r157 (fast with 0x1e1f20, control 12; 0x260670's file was another lane's this round): the actor through a
// `const &` at its two late uses 12; a second actor pointer local for the three flag tests 12; the saved +0x418
// reference copy read ahead of the node lookup 27; the bool output declared at the top with its address named there 12;
// the result flag stored true again at the top of the non-equal arm 124 (978 bytes).
// M3 lane 1 r137 (H2M_FAST with 0x1e1f20, control 12): the rounding through the existing `real_to_long` (slot_handler.h)
// stored straight to the record's short 25 (the helper's result takes its own home; retail's fistp writes the long output's
// home, so the asm form stays); `local_13 = real_to_long(local_20);` 134 (981 bytes); `local_13 = real_to_long(...)` on the
// product with no real local 134 (981 bytes); the rounding product written `field_2_3 * random` 12; the bool output through
// a `bool *const` local for the 0x261280 call and the 0x2626b0 argument 12 (both the same code).
// M3 lane 6 r211 (`--fast 0x1af160 0x1e1f20`, control 12): the rounding through the existing x87 rounding helper of
// slot_handler.h assigned to the long output 134 (981 bytes), stored straight to the record's short 25 (974 bytes, masked 0:
// the helper's own result takes a new home and the small cells rotate further); the asm block writing a fresh block-local
// long 25 (same layout as the helper); the saved reference copy declared const 12 (same code). On 0x260670's chain (with
// that function's five trials of this round: the sort scratch in its own block, the two flags in one declaration, the flags
// declared after the best score, the evaluator-loop flag declared at the top, a long sorted index) this function stays 12.
// @retail 0x1af160
bool function_1af160(long arg_0, s_1af160 *arg_1)
{
    s_actor_view *local_0 = actor_get(arg_0);
    s_prop_node_view *local_1 = prop_node_get(local_0->prop_index);
    s_type_5cfb45 *local_2 = function_25d690((s_prop_datum *)local_1);
    s_reference local_3 = local_0->unknown418;
    dword *local_4 = (dword *)function_1e4be0(arg_0);
    bool local_6 = false;
    arg_1->field_7 = local_6;
    arg_1->field_a = local_6;
    arg_1->field_b = local_6;
    arg_1->field_28 = local_6;
    arg_1->field_8 = local_6;
    bool local_5 = true;
    s_object_marker local_7[32];
    short local_8[32];
    if (!*(bool *)((byte *)local_0 + 0x3c) && local_0->unknown227)
        local_6 = function_1ac610(arg_0, &local_2->position, local_7, local_8);
    s_prop_search local_9;
    memset(&local_9, 0, sizeof(local_9));
    local_9.type = 0;
    *(short *)((byte *)&local_9 + 0x69a) = 2;
    *(bool *)((byte *)&local_9 + 0x698) = true;
    local_9.unknown59 = local_4 && (*local_4 & 1);
    s_object_marker *local_10 = local_6 ? local_7 : NULL;
    *(s_object_marker **)((byte *)&local_9 + 0x64) = local_10;
    byte *local_11 = ai_scratch_buffer_get();
    s_261d20_entry local_12;
    long local_13;
    bool local_14;
    s_reference local_15 = function_261280(&local_9, arg_0, &local_12, &local_13, local_11, &local_14);
    if (REFERENCE_EQUAL(local_15, g_470fa0))
    {
        if (!local_0->unknown227)
        {
            arg_1->field_0 = 0;
            arg_1->field_6 = true;
        }
        else
        {
            actor_get(arg_0)->unknown040 = false;
            local_5 = false;
        }
    }
    else
    {
        bool local_17 = false;
        s_262b40_result *local_16 = function_262b40(local_15);
        bool local_18 = false;
        if (*(short *)((byte *)&local_12 + 0x5c) != 0 &&
            *(long *)((byte *)local_16 + 0x14) != NONE && *(long *)((byte *)local_16 + 0x14) != 0xffff)
        {
            local_18 = function_1f4a20((s_1f4a20_entry const *)&local_12, arg_0, local_15, (s_1f4a20_source const *)local_10,
                true, &arg_1->field_c, &arg_1->field_1c, &arg_1->field_28, &local_17);
            if (local_18)
            {
                arg_1->field_a = true;
                arg_1->field_b = false;
            }
            else if (local_17)
                local_15 = g_470fa0;
        }
        if (!local_17 && !local_18)
            local_15 = function_2626b0(arg_0, local_15, local_13, local_11, local_14, true);
        if (!REFERENCE_EQUAL(local_15, local_3))
        {
            real local_20 = function_259d0(&g_4e7408->unknown0, __FILE__, __LINE__, 5.0f, 5.0f) * g_510c54->field_2_3;

            __asm
            {
                fld local_20
                fistp local_13
            }
            arg_1->field_0 = (short)local_13;
        }
        if (!REFERENCE_EQUAL(local_15, g_470fa0))
        {
            arg_1->field_4 = 0;
            local_0->unknown4ae = true;
            real local_22;
            if (function_1af530(arg_0, (long *)&local_22, &local_13) &&
                local_22 > function_210ac0((s_type_c3b527 const *)local_16, &local_2->position))
                arg_1->field_8 = true;
        }
        long local_23 = function_1e1f20(arg_0);
        if (local_23 != NONE)
        {
            bool local_26 = true;
            byte *local_24 = (byte *)function_1e5280(arg_0, ai_object_get(local_23)->definition_index);
            if (local_24)
            {
                if (*(real *)(local_24 + 0xc) > local_1->unknown28)
                    local_26 = false;
                else if (!REFERENCE_EQUAL(local_15, g_470fa0))
                {
                    point3f *local_28 = &function_25d690((s_prop_datum *)local_1)->position;
                    vector3f local_29;
                    vector3d_from_points3d(&local_12.point, local_28, &local_29);
                    if (length_sq3f(&local_29) < *(real *)(local_24 + 0xc) * *(real *)(local_24 + 0xc))
                        local_26 = false;
                }
                function_265c30(local_0->prop_index, arg_0, local_26);
            }
        }
    }
    ai_scratch_buffer_release(local_11);
    return local_5;
}
