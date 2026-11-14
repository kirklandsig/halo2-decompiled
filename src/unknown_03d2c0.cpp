// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_03D2C0.CPP: the first-person view's object, and the lighting cache
   of an object (built with SSE, apart from its neighbours in
   unknown_02b5a0.cpp) */

#include "target_cache_storage.h"
#include "view_context_storage.h"
#include "unknown_11c920.h"
#include "data_array.h"
#include "globals.h"
#include <string.h>

extern s_record_pool *g_509434;
extern dword g_4ba034;

struct s_3dd10_object_header
{
	byte unknown00[8];
	byte *object;
};

void function_3dd10(long object_index, bool force);
struct s_interface_function_context;
void function_2c3b0(s_interface_function_context *context);
void function_ba690(long object_index, long *state_count, byte **states, long *a, long *b);
real __stdcall function_be8b0(long object_index, long name);
struct s_object_list;
extern s_object_list *g_4de2f4;
long function_baf80(long object_index);

struct s_effect_color_query
{
    real unknown00, unknown04, unknown08;
    dword color_a, color_b;
};
struct s_lighting_record
{
    color3f color0;
    vector3f direction0;
    real length, value1c;
    color3f color1;
    vector3f direction1;
    color3f color2;
    vector3f direction2;
    dword unknown50;
};
struct s_3dfe0_cache
{
    short identifier;
    bool valid, initialized;
    long parent;
    dword sample_time, update_time;
    point3f position;
    s_effect_color_query current, target;
    byte unknown44[0x10];
    s_lighting_record lighting;
    byte unknowna8[0x58];
};
long function_d1850(long object_index, bool value, s_effect_color_query *query);
void function_d4080(long type, s_effect_color_query const *query, long flag, s_lighting_record *record);
struct s_object;
s_object *function_badc0(long object_index, dword mask);
void function_3dfe0(long cache_index, long object_index, real priority);

extern s_lighting_record g_55ee60;

struct s_first_person_model;
struct s_unknown_78;
extern s_unknown_78 *g_510c6c;
long function_155760(long player_index);
long __stdcall first_person_weapons_get_models(long user_index, long unit_index, long maximum, s_first_person_model *models);
extern long g_4b72c0;

long __stdcall function_3ddd0(long object_index);

bool function_14ddc0(long local_player_index);
long function_16c2b0(void);

// @retail 0x3d2c0
void function_3d2c0()
{
    g_4c1bd0 = NONE;
    g_4c1bd4 = 0;
    g_4c1bd0 = function_16c2b0();
    if (g_4c1bd0 == NONE)
    {
        g_4c1bd0 = g_4b72c0;
        if (g_4c1bd0 == NONE && g_4b9ed8 != NONE && !function_155760(g_4b9ed8))
        {
            long unit = function_14ddc0(g_4b9ed8) ? *(long *)(g_4e8c24->data + (g_4e8c20->entries[g_4b9ed8] & 0xffff) * 0x21c + 0x2c) : NONE;
            g_4c1bd0 = unit;
            g_4c1bd4 = first_person_weapons_get_models(g_4b9ed8, unit, 4, (s_first_person_model *)g_4c1bd8);
        }
    }
}
// @retail 0x3db00
s_lighting_record *function_3db00(long object_index, real priority)
{
    byte *object = ((s_3dd10_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    if (object[7] & 1) return (s_lighting_record *)((byte *)g_4de2f4 + 0x1c);
    short cluster = *(short *)(object + 0xd0);
    if (cluster != NONE)
        return (s_lighting_record *)(*(byte **)((byte *)g_4e0348 + 0x230) + cluster * 0x5c + 8);
    long parent_index = function_baf80(object_index);
    long cache_index = function_3ddd0(parent_index);
    if (cache_index != NONE)
    {
        s_3dfe0_cache *cache = (s_3dfe0_cache *)(g_509434->data + (cache_index & 0xffff) * 0x100);
        if (parent_index == object_index) function_3dfe0(cache_index, object_index, priority);
        else
        {
            long child_cache_index = function_3ddd0(object_index);
            if (child_cache_index != NONE)
            {
                s_3dfe0_cache *child = (s_3dfe0_cache *)(g_509434->data + (child_cache_index & 0xffff) * 0x100);
                child->lighting = cache->lighting;
                child->target = cache->current;
                child->current = cache->current;
            }
        }
        return &cache->lighting;
    }
    byte *parent = ((s_3dd10_object_header *)g_4e0300->data)[parent_index & 0xffff].object;
    byte *definition = g_4e3b44[*(long *)parent & 0xffff].bytes;
    long special = 0;
    if ((bool)(((*(dword *)(parent + 4)) >> 13) & 1)) special = 1;
    if ((bool)((*(word *)(definition + 2) >> 1) & 1)) special |= 1;
    s_effect_color_query query;
    function_d1850(parent_index, 1, &query);
    function_d4080(*(short *)definition, &query, special, &g_55ee60);
    return &g_55ee60;
}
// MiniMax G (11-13): no lower score (77); the flag set in both arms of the allocation test reaches 9 again, but that is the
// form the coordinator reverted on 11-11 (a statement copied into both arms, kept only with a byte match), so the body
// stays as it is; the walk's NONE exit spelled as a then-arm return 83, the allocation test inverted with the flag in the
// else arm 83, the flag hoisted above the allocation 116, and the permuter over the 9 form found nothing better.
// score 77 (was 77): m3 lane 4 r196 note at the end; round 116 (m3 lane 9, fast with 0x3db00 and 0x3dd10): lane 14 r84's `long result = NONE;` and
// `if (result == NONE) { allocation; walk; }` skeleton with round 99's `if (result != NONE) initialize = true;` after the
// walk (no walk return) 99 (491 B: the walk's NONE exit jumps to the end as retail, but the allocation and walk paths are
// threaded over the merge test retail keeps); re-measured the 9 form (9, 519 B). Every spelling that keeps the merge test
// sends the found path through it; every spelling that threads the found path threads or folds the other paths too.
// MiniMax H (11-12): no new form, no lower score (77); this round tried the cached and the allocated index in one variable (151), the allocation test inverted (151), the flag starting true and cleared on the hit (130), the initialization nested in the allocation arm (176), the hit returning early (186), one guard for the two tests (132) and the early return moved to the tail (152), and the permuter over the function managed 129 tries in 900.4 s without a confirming build (best 71, baseline 71), so the merge's register choice stands.
// Earlier at 77 (was 77): retail keeps the result in esi and tests it before the
// retail keeps the result in esi and tests it before the
// reload, ours copies it into eax at the merge point and puts the cold "no
// result" return block ahead of the initialization instead of after it
// (retail returns it with mov eax, esi; ours folds it to the constant).
// MiniMax H (11-12): no new form, no lower score (77); this round tried the cached and the allocated index in one variable (151), the allocation test inverted (151), the flag starting true and cleared on the hit (130), the initialization nested in the allocation arm (176), the hit returning early (186), one guard for the two tests (132) and the early return moved to the tail (152), and the permuter over the function managed 129 tries in 900.4 s without a confirming build (best 71, baseline 71), so the merge's register choice stands.
// MiniMax H (11-12): no new form, no lower score (77); this round added the flag set before the walk, the flag in
// both arms of the allocation test, the allocation stored straight into the result and the allocation stored into the
// result with the flag after the walk, and they score 115, 83, 116 and 141. What is left is that retail tests the
// result in ESI at the merge and sends the failure to a trailing return where ours reloads the result into EAX before
// the merge test, and the walk's NONE exit returns the constant -1 where retail returns the reloaded value.
// MiniMax A (11-11, run 58): score 77 (was 77): no lower score; retail's cache-hit path stores the index and
// jumps past the merge test straight to the record lookup, ours jumps into the merge test and reloads the index
// into EAX there; the copy of the cached index into the result between the two tests hides the known condition
// from the compiler, and every spelling that keeps one variable for the cached and the allocated index or moves
// the merge test onto the cached index scores worse (155 and 36).
// Tried: return NONE (77); the NONE test after the inner if (77); no early return (161);
// "if (result != NONE) initialize = true; else return" (83); one variable for the
// allocation and the result (141); result = cache_index at its declaration (148).
// Round 99 (lane 16): the region lookup through the existing 0xba690 (count and states): 77 (same code; kept).
// After the walk: `cache_index = result;` and both later tests on cache_index 79; only the early return on
// cache_index 77; the walk's best index in its own local 162 (524 bytes), the same copied back through
// cache_index 128 (522); no early return, `if (result != NONE) initialize = true;` 85 (504 bytes, aligned
// 15: retail's block structure, the walk's NONE exit jumps to the end and the alloc success joins at the
// second test; left: the join reloads the result into EAX where retail tests ESI, and ours keeps the result
// in ESI across 0x3dd10 so the two epilogues merge); the same with cache_index = result and the final test
// on cache_index 141 (518 bytes, aligned 24), with only the initialize test on it 85; `initialize = result
// != NONE;` 116 (495); the walk as a forceinline helper writing the best index through a pointer 85 (same
// code as the 85 form), returning it 108.
// Round 101 (lane 16, fast with 0x3dd10): ours returns the constant on the walk's NONE path (`or eax, esi`, ESI known -1); retail
// returns the variable (`mov eax, esi`) from a second epilogue after the main one. The early return returning cache_index 77,
// `long result = NONE;` 77, `result = cache_index = record_pool_allocate(...)` 77 (all same code); one early return after the
// if/else in place of the `if (result != NONE)` block 152 (497 bytes); the early return as `goto done` to the only return 85
// (504 bytes, the two NONE tests merge).
// m3 lane 15 r72 (fast with 0x3db00/0x3dd10): re-measured round 99's single-return form (`if (result != NONE) initialize = true;`
// after the walk, no early return) 85 (504 bytes): its walk exit reaches an unfolded copy of the final return as retail's does.
// New: on that form, `long result = cache_index;` with one inverted test 168 (491 bytes); one variable for lookup, allocation,
// walk and return 129 (504) (on main it re-measures the listed one-variable form, 141 then, 151 now, 508); `initialize = true;` then `if (result == NONE) return result;` at the end of the
// else arm without the outer test 145 (508), with the walk's early return kept too 138 (518 bytes: the else arm moves to the end
// and both returns fold to `or eax, -1`); the found test inverted with the found arm as the else 77 (same code), on round 99's
// form 157; the outer `if (result != NONE)` dropped (it is redundant after main's else arm) 86 (506), with round 99's inner
// test 98. Finding: with no test between the found arm and the record lookup (the last two forms), the found path jumps
// straight to the lookup and the lookup, the 0x3dd10 call setup and the reload after it are retail's exactly; retail keeps
// its NONE test after `initialize = true` on the allocation and walk paths only. VC threads a join only from a test of the
// same variable just before it (diagnostic: the found test written NONE-last on `result` threads the found arm, 172; written
// NONE-last on cache_index it does not, 96): a copy of the tested index (`result = cache_index`) does not carry the knowledge.
// Goto forms to the one final `return result;` (match-only): the walk's early return kept, `initialize = true;` then
// `if (result == NONE) goto done;` closing the else arm, outer test dropped 83 (508 bytes: from the walk's exit on, the C
// test, the record lookup, the 0x3dd10 call and the main epilogue are retail's; left: the walk's NONE return, inline and
// folded to `or eax, esi` where retail jumps to an unfolded second epilogue after the main one, `mov eax, esi`); both NONE
// exits as that goto 85 (504: the epilogues merge and the join loads result into EAX); the same with result declared first 85;
// the second NONE test ahead of `initialize = true;` 99 (491), with no walk test 99; round 99's inner test then the goto 99;
// the inner test in the walk arm with an else 100; re-measured round 101's early return as `goto done` 85. Matched 0x23600 shows
// the second epilogue is VC's copy of a single return for a path that holds the value in a register.
// M3 lane 8 r151 (fast; control 77): retail's found path skips the merge test on a constant, not on the index: with the
// merge test spelled `!initialize || result != NONE` (diagnostic, an implied test) the found path jumps straight to the
// record block and the merge block becomes retail's `cmp esi, -1; mov cl, 1; je` (83, 508 bytes; only the walk's NONE exit
// still differs: ours inline returning the constant, retail a second epilogue at the end returning ESI). Our build folds
// `return result` after any direct NONE test of result to the constant (the walk exit returning cache_index, a copy of
// result tested, `result == cache_index`, the result typed dword, the walk writing through a pointer: 83/83/83/83/83; on the
// plain body dword 77 and the pointer 77, same code). New forms: the found test as the else arm of an alloc-first
// `cache_index == NONE || owner != object_index` 77 (same code); `if (initialize && result == NONE) return result;` ahead of an
// unconditional record block 138 (518 bytes, merge block as retail, the else arm laid out after the main return), with
// `return NONE` 138; the NONE test with its return at the end of the else arm, record block unconditional, 138 (518 bytes);
// that without the walk's own return 145. Re-measured: the listed `if (result != NONE) initialize = true; else return
// result;` 72 now (519 bytes: the walk's NONE return moves to the end as retail, but returns `or eax, -1`), with the merge
// test spelled as above 87; one variable for the allocation and the result 141.
// m3 lane 14 r84 (fast with 0x3dd10 and 0x13ddd0, both matched; aligned rows in brackets, base 11): re-measured the
// `initialize = true` in both arms of the allocation test 9 [6] (match-only: a flag set inside two tests). New: the
// allocation arm first (`cache_index == NONE || owner != object_index`) closed by `initialize = true; if (result ==
// NONE) return result;` with the record block unconditional 138 (518 B [125]: the record block becomes the found arm's
// fall-through and the allocation arm moves after the return); `long result = NONE;`, the found test only setting
// `result = cache_index`, then `if (result == NONE) { allocation and walk; initialize = true; }` and main's outer test 83
// (508 B [16]: the found path now skips the merge test and enters the record lookup as retail; left only the walk's NONE
// return, inline and folded to `or eax, esi` where retail has an unfolded `mov eax, esi` epilogue after the main one, and
// one alignment pad); on that the 9 form's arms 86 (506 B), no walk return 101 (491 B, the two NONE tests merge), the
// found test as a ternary initialiser 147 (524 B).
// Round 171 (m3 lane 5, fast; control 77). Lead 9, match-only (plain 9, 519 B, aligned 3): `initialize = true` set in
// both arms of the allocation test, `if (cache_index != NONE) initialize = true; else { walk; if (result != NONE)
// initialize = true; else return result; }`, the outer test unchanged. Not kept: the flag is set inside two tests and
// tested later, and main's direct form (one `initialize = true;` after the early return) is this 77 body. Left on 9:
// the found path jumps to the result reload ahead of the merge test where retail's jumps past the test to the record
// lookup (so retail compares ESI before reloading it), and the walk's NONE return at the end folds to `or eax, -1`
// (retail `mov eax, esi`). On 9: `long result = NONE;`, `return NONE;`, `return cache_index;`, the merge test as `if
// (result == NONE) return result;` before an unconditional block, the else-first found test, a dword cache index, a
// dword result, both: 9 each (same code); one variable for the cached and allocated index 155 (the found path then
// skips the test as retail, but the index is stored at its read and the allocation arm is laid out first); no walk
// return 85 (round 99's code); `!initialize || result != NONE` (implied test) 87; the allocation stored into result
// 138; the walk test as `if (result == NONE) return result; initialize = true;` 83; `return index` 117; result
// assigned only in the allocation success arm 131; `long result = cache_index;` with no found arm 153; the merge test
// on cache_index with `cache_index = result` in the walk arm 36 (the found path still enters the test); `cache_index =
// result = index` in the walk with that test 159; one variable with initialize assigned in each arm 155. The NONE test
// closing the else arm with an unconditional record block: with no walk return 86 (the walk's NONE return is then `mov
// eax, esi` as retail, but inline, and the merge test goes for every path), with the walk return 87; main with that
// test 138 (518 B, aligned 67, else arm laid out after the main return); the 72 form with it 86. Other forms: the
// early return after the inner if 77 (same code as main); initialize assigned in every arm on the 72 form 142, on main
// 143; the 72 form with the allocation stored into result 131; the single-return form with `!initialize || result !=
// NONE` 98; the 72 form returning cache_index 72 (same code); the found arm's store through a pointer to result 72
// (same code). Reading: VC drops the merge test only when every incoming path knows the result; a copy into result
// between the cached-index test and the merge test hides it on the found path; a NONE return reached only through a
// threaded jump stays unfolded.
// MiniMax R (11-11): reached 9 from 77: the flag is now set in both arms of the allocation test (the allocation
// succeeded, or the walk found a record) instead of after the walk, which is the same behaviour and gives retail's
// merge block; left: the found path reloads the result from its stack home before the merge test where retail tests
// the register value first and reloads it after, the walk's NONE exit still returns the constant -1 where retail
// returns the reloaded value, and the function is one byte longer than retail's.
// MiniMax H (11-12): no new form tried, no lower score (77); retail's body was read again instruction by instruction.
// Its merge block (after the allocation and after the walk) is `cmp esi,-1; mov cl,1; je`, so the flag is set at the
// merge and the cached path jumps past the merge straight to the record lookup, while ours reloads the result into EAX
// before the test and keeps the flag in a byte of its own. The walk's exit jumps over the return where retail jumps to
// it, and retail's init block copies the four state blocks itself (a 16-byte copy loop over an array in the object,
// with the counts at 0x118 and 0x11a) where ours calls function_ba690 and copies with memcpy, so the score is not only
// the merge block. The plain forms that set the flag on each path keep the score where it is.
// Earlier at 77 (was 77): the flag is set in both arms of the allocation test now, which gives retail's merge block;
// left: the found path reloads the result from its stack home before the merge test where retail tests the register
// value first and reloads it after, the walk's NONE exit still returns the constant -1 where retail returns the
// reloaded value, and the function is one byte longer than retail's.
// MiniMax R (11-11), NOT kept (coordinator 11-11): 77 -> 9 with the allocation test inverted (cache_index != NONE first) and `initialize = true` written in both arms, the oldest-record search's arm setting it when a record was found and returning otherwise (a statement copied into both arms: kept only with a byte match); body at 77. Lead: retail sets the flag on each path; find the plain form that does that.
// Round 116 (m3 lane 12, fast with 0x3db00 and 0x3dd10; control 77): the walk as a `for` loop with the next-index call as
// its increment 77 (same code). Correction to MiniMax H (11-12) above: ours inlines 0xba690 here as retail does (the same
// `movsx [eax+0x11a]`, `movsx [eax+0x118]`, divide by ten and `rep movsd`/`rep movsb` copy loop); all 77 differences are
// the block layout from +0x54 (the found path, the merge test and the walk's NONE epilogue). No untried natural form found
// that changes this code.
// m3 lane 14 r89 (fast with 0x3db00 and 0x3dd10, both matched; control 77): the found test held in a named bool 77
// (same code); the merge and initialize tests joined as `result != NONE && initialize` 132 (506 B: the record pointer
// then comes after the initialize test, retail computes it before); the walk's best index stored before its age 79;
// the negative-age reset as a ternary 77 (same code); the found test written `!(cache_index == NONE || owner !=
// object_index)` 77 (same code). Re-measured: the walk's NONE exit returning cache_index 77. As round 116 found, no
// untried natural form changes the layout from +0x54.
// M3 lane 13 r89 (fast with 0x3db00 and 0x3dd10, both matched; control 77): the flag computed from the found test, `bool
// initialize = cache_index == NONE || owner != object_index;`, then `if (initialize) { allocation } else result = cache_index;`
// 157 (498 bytes), with the arms the other way round 148 (495 bytes); the cache index stored into the object before the four
// state copies 107; `state_count`/`states` declared at the top 77 (the code changes); the tail as `if (initialize && result !=
// NONE)` 176 (488 bytes). Re-measured: the record pointer inside `if (initialize)` 132 (506 bytes, the listed joined-test 132).
// Rebuilt for reading: 9, 83, 85, 86, 138, 145. VC folds `return result` after any NONE test of result (`or eax, esi` / `or
// eax, -1`); retail's walk exit returns the register unfolded from a second epilogue, which no listed spelling gives.
// M3 lane 4 r196 (fast with 0x3db00/0x3dd10, both stay matched). A two-function LTCG link harness (build/r196/h/h3dd.py and
// gen3dd.py of m3-lane4, about 1 s a form; it reproduces this body's code and the listed 9 form's) screened 120 forms: found
// arm (if/else, inverted, `result = NONE` then `if (result == NONE)`), walk exit (return, none, goto to the only return,
// `if (result != NONE) initialize = true; else ...`), merge (guarded, `!initialize ||`, closed arm). Every goto form merges
// the two epilogues on ESI and every explicit walk return folds to `or eax, esi`. Real: `result = NONE`, the found test only
// setting result, `if (result == NONE) { allocation; walk; if (result == NONE) goto done; initialize = true; }` and the only
// return at `done` 85 (504 B); main with the walk's return as that goto 85 (504 B); the first with the walk closing `if (result
// != NONE) initialize = true; else goto done;` 100 (491 B); the first with no walk exit test 101 (491 B); main with that close
// 85 (504 B).
// P472: Byte/char/word/short initialization decision, integer negative-age normalization, promoted age/oldest/both: 77/77/74/74/140/160/136/164.
// NOT kept: word/short gain three, below the threshold; other scalar-domain forms tie or regress. Original merge, scan and full body restored; cached-edge and trailing-return layout remain.
// @retail 0x3ddd0
long __stdcall function_3ddd0(long object_index)
{
    byte *object = ((s_3dd10_object_header *)g_4e0300->data)[object_index & 0xffff].object;
    long cache_index = *(long *)(object + 0xcc);
    long result;
    bool initialize = false;
    if (cache_index != NONE && *(long *)(g_509434->data + (cache_index & 0xffff) * 0x100 + 4) == object_index)
    {
        result = cache_index;
    }
    else
    {
        cache_index = record_pool_allocate(g_509434);
        result = cache_index;
        if (cache_index == NONE)
        {
            real oldest = -3.402823466e+38f;
            long index = data_datum_index(g_509434, function_16bc00(g_509434, 0));
            while (index != NONE)
            {
                byte *record = g_509434->data + (index & 0xffff) * 0x100;
                real age = (real)((long)g_4ba034 - *(long *)(record + 0xc));
                if (age < 0.0f) age = 1000.0f;
                if (age > oldest)
                {
                    oldest = age;
                    result = index;
                }
                index = data_datum_index(g_509434, function_16bc00(g_509434, index == NONE ? 0 : (index & 0xffff) + 1));
            }
            if (result == NONE) return result;
        }
        initialize = true;
    }
    if (result != NONE)
    {
        byte *record = g_509434->data + (result & 0xffff) * 0x100;
        if (initialize)
        {
            *(long *)(record + 4) = object_index;
            *(long *)(record + 8) = NONE;
            *(long *)(record + 0xc) = NONE;
            record[2] = 0;
            record[3] = 0;
            function_2c3b0((s_interface_function_context *)(record + 0x44));
            *(long *)(record + 0x44) = object_index;
            *(long *)(record + 0x48) = (long)function_be8b0;
            *(long *)(record + 0xbc) = NONE;
            long state_count;
            byte *states;
            function_ba690(object_index, &state_count, &states, NULL, NULL);
            for (long i = 0; i < 4; ++i)
                memcpy(record + 0xc0 + i * 16, states, state_count);
            *(long *)(object + 0xcc) = result;
            function_3dd10(object_index, true);
        }
    }
    return result;
}
