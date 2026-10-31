// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_02CBF0.CPP: adding an entry to the visibility list (built with SSE,
   apart from its neighbours in unknown_03f2b0.cpp) */

#include "cluster_mask_storage.h"
#include "material_records_storage.h"
#include "target_cache_storage.h"
#include "shader_storage.h"
#include "projection_storage.h"
#include "view_context_storage.h"
#include "unknown_11c920.h"
#include "visibility_storage.h"
#include "camera_storage.h"
#include "frame_storage.h"
#include "reset_storage.h"
#include "globals.h"
#include "index_cache_storage.h"
#include "depth_groups_storage.h"
#include "physical_memory.h"
#include <string.h>
#include <xtl.h>
#include "unknown_030290.h"




struct s_bit_vector_pool;
dword *function_1332b0(long bit_count, s_bit_vector_pool *data);
long function_016ae0(real value);
void function_30290(point3f const *point, s_view const *view, s_camera const *camera, real radius, box2f *bounds);
extern byte g_509410, g_509411;

void *function_44940(short index, bool instance);
long function_2d000(long object_tag, long tag, long index);
__declspec(noinline) void *function_449e0(short index, bool load, bool instance);

/* retail inlines 0x16ae0's body at the first two uses in 0x2cbf0 */
static __forceinline long floor_to_long_inline(real value)
{
	__asm {
		movss xmm0, value
		cvttss2si eax, xmm0
		cvtsi2ss xmm1, eax
		cmpneqss xmm1, xmm0
		cmpltss xmm0, g_45dbd8
		andps xmm0, xmm1
		movmskps ecx, xmm0
		sub eax, ecx
	}
}

// score 17 (was 17): left: the first word's OR order (retail ORs flags and category before the
// kept top bits), the intensity spill slot (BLOCKED-BY 0x133390 and 0x133520: level is pushed as a
// word there, held file), the kind-word block (retail shifts the kind in eax, copies it to esi and
// stores the word after the loader's argument setup, just before the call) and the pool register
// set before the bit-count test. Earlier rounds' notes follow.
// Earlier at 17 (was 17): this round's source permuter over the body found no better form (22 tries in
// 310s, best 17 on the reduced image and on the full image, equal to the baseline, no confirming
// build), so the body is unchanged.
// Earlier at 17 (was 17): this round's kind-block forms (the new word stored before the 0x449e0 call, with
// the instance bit read back from the stored field, the tag read into a local, both together) score
// 78, 79, 17 and 78, so the store's position and the tag's register stay as they were.
// Earlier at 17 (was 17): this round's first-word OR forms (the flags term first, the category term
// first, the unknown00 term first, a named low-word local, the two-statement split, the nested
// tree shape and the previous-word local) all compile to the same 17; retail evaluates the first
// word's terms as flags, then category, then the kept top bits, and MSVC keeps the reassociated
// order whatever the source says. The intensity spill slot and the kind block still differ.
// Earlier at 17 (was 17): this round's kind-word forms all compile to the same block (one expression 81,
// a named shifted word 81, a dword kind 17, the long-domain shift 17, the store before the call with
// the bit read back from the field 79). Retail shifts the kind in eax before the copy to esi and
// stores the word before the call; the first word's OR order and the intensity spill slot still differ.
// Earlier at 17 (was 36): the new kind word is now built in esi and its instance bit is taken from a copy
// of it (`and eax, 0xffffff01`); the word is stored to the entry just after the 0x449e0 call, which is
// what keeps it live in esi. The first word's OR order, the scaled-intensity spill slot and the
// reduced image's thunk calls, thunk branches and global addresses still differ.
// Earlier at 36 (was 36): this round's kind-word forms (a named word stored first, the instance bit read
// back from the stored field) compile identically to the baseline; the packed-word order, the
// intensity spill slot and the new kind word in esi still differ, and the reduced image's thunk
// calls, thunk branches and global addresses cannot match.
// Earlier at 36 (was 36): retail ORs the incoming flags and category before the kept top bits of the first
// word; the scaled intensity spills to `outside`'s home (ours `level`'s: retail's callers push level as a
// word, BLOCKED-BY 0x133390 and 0x133520, whose held file holds the byte declaration); retail builds the
// new kind word in esi, takes the instance bit from a copy (`and eax, 0xffffff01`) and stores the word
// just before the 0x449e0 call (ours stores it at once and passes the named bool through a stack byte);
// retail sets edx to the state before the bit-count test.
// Tried: separate allocation increment, entry reference, position memcpy and early group-pointer scope
// 80; object-tag result first 97; object-index guard first 175; separate inline floor results 80;
// missing-geometry guard first 276; flags-first or-chain, two-statement split and &=/|= split 80
// (reassociated); named word local in the call, named bools before the store, kind term first, dword
// kind 80; separate call-value copy for the out-of-line floor 80; all eight ordinary/inline/separate-
// noinline section/kind declaration pairs 46 (1036/1033). On the mask test: kind term first 80;
// packed-word local in either order 80; load named before the kind store 81; kind call inside the store
// 80; &=/|= kind split 80; shifted dword kind 80; instance bit read before the store 89; old word copied
// to a local 91; only the instance named 38; only the load named 79; instance named first 39; both named
// with the field-bit test 73; flags-last and category-first word-one or-chains 46 (reassociated); a
// bitfield view for the kind store (diagnostic) 80. Grouped new fields (either order), old word last,
// kind word split or with a named shifted kind, dword kind result, `!= 0` bit argument: identical code;
// bitfield view of the word (kind only / all fields) 80/84 on the `& 0x40` form; word and both bools
// before the store 46; old word read through the global array in the first/kind word 47/57.
// Round 97 (on the `& 0x40` form; aligned counts in brackets, baseline 17): instance bit passed inline
// with the load named 79 [15], both inline as the loader's other callers write them 80 [13] (retail's
// `and ..., 0xffffff01; push`, block two instructions short), that with the kind term first 79 [15];
// a named kind word (store, then the bit from it) 80/80/79 [13/13/15] (plain, kind first, load named);
// `(word & 0x1000) != 0` inline/named 80/36 [13/17]; kind call inside the store with inline arguments,
// either order 80/80 [17/17].
// packed-word load order (ours reads the old word first), the intensity
// spill (ours in level's dead slot, retail in outside's: retail's callers push level as a
// word, BLOCKED-BY 0x133390 and 0x133520, whose file holds the byte declaration), the new kind
// word (retail builds it in esi, takes the instance bit from a copy and stores the word just
// before the 0x449e0 call; ours stores it at once and spills instance_bit to `mode`'s home),
// the pool argument (retail sets edx before the bit-count test) and the flag-clear register
// (retail ecx, ours edx). The bit-6 test is retail's `test al, 0x40`; the loader's two flags
// are named before the call, so the load test is set first as in retail.
// the bit-6 test is retail's `test al, 0x40` and the loader's two flags are named
// before the call, so the load test is set first as in retail. Left: retail ORs the incoming flags and
// category before the kept top bits; the scaled intensity spills to `outside`'s home (ours `level`'s:
// retail's callers push level as a word, `mov cx, word`, but the held src/unknown_1321f0.cpp declares
// it a byte for 0x133390 and 0x133520; level as a word gives 34 here and 0x133f70 16 -> 8, not kept
// because those callers would need a byte-level forwarding overload: retry once that file is free);
// retail keeps the new kind word in esi, takes the instance bit from a copy and stores the word just
// before the 0x449e0 call (ours stores it at once and spills instance_bit to `mode`'s home); retail
// sets edx to the state before the bit-count test.
// packed-word load order, intensity spill and final word home.
// the bit-6 test is retail's `test al, 0x40` again (the field-bit form only
// shifted the listing, 46 at 1036 bytes) and the loader's two flags are named before the call, so
// the load test is set first as in retail. Left: retail ORs the incoming flags and category before
// the kept top bits; the scaled intensity spills to `outside`'s home (ours `level`'s); retail keeps
// the new kind word in esi, takes the instance bit from a copy and stores the word just before the
// 0x449e0 call (ours stores it at once and spills instance_bit to `mode`'s home); retail sets
// edx to the state before the bit-count test.
// Tried: separate allocation increment, entry reference, position memcpy and early group-pointer
// scope 80; object-tag result first 97; object-index guard first 175; separate inline floor results
// 80; missing-geometry guard first 276; flags-first or-chain, two-statement split and &=/|= split 80
// (reassociated); named word local in the call, named bools before the store, kind term first, dword
// kind 80; separate call-value copy for the out-of-line floor 80; all eight ordinary/inline/
// separate-noinline section/kind declaration pairs 46. On the mask test: kind term first 80;
// packed-word local in either order 80; load named before the kind store 81; kind call inside the
// store 80; &=/|= kind split 80; shifted dword kind 80; instance bit read before the store 89; old
// word copied to a local 91; only the instance named 38; only the load named 79; instance named first
// 39; both named with the field-bit test 73; flags-last and category-first word-one or-chains 46
// (reassociated); a bitfield view for the kind store (diagnostic) 80; grouped new fields (either
// order), old word last, kind word split or with a named shifted kind, dword kind result, `!= 0` bit
// argument: identical code; bitfield view of the word (kind only / all fields) 80/84 on the `& 0x40`
// form; word and both bools before the store 46; old word read through the global array in the
// first/kind word 47/57. With the word level: word-one orders (old last, category first, old in the
// middle, grouped) 34 each; byte category 281; kind word local stored after the load test, only the
// load named, everything in the call, instance computed first 78/77/78/78 (1033 bytes, the kind
// block two instructions short: the word is shifted in place after an early store); only the
// instance named 36; kind term first 34.
// scope 80; object-tag result first 97; object-index guard first 175; separate inline floor
// results 80; missing-geometry guard first 276; flags-first or-chain, two-statement split and
// &=/|= split 80 (reassociated); flags-last and category-first or-chains, grouped new fields
// (either order), old word last 46 (reassociated); named word local in the call, named bools
// before the store, kind term first, dword kind 80; separate call-value copy for the out-of-line
// floor 80; all eight ordinary/inline/separate-noinline section/kind declaration pairs 46. On the
// mask test: kind term first 80; packed-word local in either order 80; load named before the
// kind store 81; kind call inside the store 80; &=/|= kind split 80; shifted dword kind 80;
// instance bit read before the store 89; old word copied to a local 91; only the instance named
// 38; only the load named 79; instance named first 39; both named with the field-bit test 73;
// word and both bools before the store 46; old word read through the global array in the
// first/kind word 47/57; a bitfield view for the kind store (diagnostic) 80, of the whole word
// 84; `!= 0` bit argument identical; `(word & 0x1000) != 0` inline / named 80/36; both loader
// arguments inline 80 (retail's `and eax, 0xffffff01; push`, but the word is stored at once).
// Round 41: kind store and bit read through the array index 83; kind masked to three bits, a
// pool pointer local (before the section call or before the bit test) and a separate variable
// for the second section 36 (identical code); category through a byte cast 281 (category goes
// to the stack).
// packed-word load order, intensity spill, final kind-word carrier and state setup differ.
// Earlier at 46: packed-word load order (ours reads the old word first), the intensity
// packed-word load order, intensity spill and final word home.
// the bit-6 test is retail's `test al, 0x40` again (the field-bit form only
// shifted the listing, 46 at 1036 bytes) and the loader's two flags are named before the call, so
// the load test is set first as in retail. Left: retail ORs the incoming flags and category before
// the kept top bits; the scaled intensity spills to `outside`'s home (ours `level`'s); retail keeps
// the new kind word in esi, takes the instance bit from a copy and stores the word just before the
// 0x449e0 call (ours stores it at once and spills instance_bit to `mode`'s home); retail sets
// edx to the state before the bit-count test.
// &=/|= split 80 (reassociated); named word local in the call, named bools before the store, kind
// term first, dword kind 80; separate call-value copy for the out-of-line floor 80; all eight
// ordinary/inline/separate-noinline section/kind declaration pairs 46. On the mask test: kind term
// first 80; packed-word local in either order 80; load named before the kind store 81; kind call
// inside the store 80; &=/|= kind split 80; shifted dword kind 80; instance bit read before the
// store 89; old word copied to a local 91; only the instance named 38; only the load named 79;
// instance named first 39; both named with the field-bit test 73; flags-last and category-first
// word-one or-chains 46 (reassociated); a bitfield view for the kind store (diagnostic) 80.
// packed-word load order (ours reads the old word first), the intensity
// spill (ours in level's dead slot, retail in outside's: retail's callers push level as a
// word, BLOCKED-BY 0x133390 and 0x133520, whose file holds the byte declaration) and the new kind word
// (retail builds it in esi and stores it after the push). The field-bit test of bit 6 is
// not retail's code (retail: test al, 0x40); `& 0x40` gives it at 80 (1033 bytes), but the
// kind-word block is then two instructions short and the tail shifts.
// Separate allocation increment, entry reference, position memcpy and early
// group-pointer scope 80; object-tag result first 97; object-index guard first
// 175; separate inline floor results 80; missing-geometry guard first 276.
// Flags-first or-chain, two-statement split and &=/|= split 80 (reassociated);
// named word local in the call, named bools before the store, kind term first,
// dword kind 80; separate call-value copy for the out-of-line floor 80.
// Earlier: a mask test read as a field bit (TEST_FIELD_BIT) gives retail's load-and-shift; the rest still differs.
// Round 409: All eight ordinary/inline/separate-noinline section/kind declaration pairs 46 (1036/1033); no gain. Original contracts restored. Retained-word load order, intensity slot, final word carrier and shifted flag test remain.
// Grouped new fields (either order), old word last, kind word split or with a
// named shifted kind, dword kind result, `!= 0` bit argument: identical code;
// bitfield view of the word (kind only / all fields) 80/84 on the `& 0x40` form;
// word and both bools before the store 46; old word read through the global
// array in the first/kind word 47/57.
// Round 421: shared masked input 110; uniform record indexing 374; masked/indexed record 364; short record index
// 36; masked/short index 110; indexed/short index 375; all record inputs 364; entry-scope masked input/all record
// inputs 394.
// Round 51: clamp/scaled declarations swapped 36; scaled/clamped role names 36/36; combined names and
// order 36; kind declared before object tag 36; kind result renamed 36; first word flags/category/old 36;
// two-use kind word 80 (1033 bytes); its shift split from the OR 79 (1033 bytes). Original restored.
// Round 134 (m3 lane 4): the word stored between the load test and the call 79 (the store moves
// ahead of the bit, the word stays in eax); the bit named before that store 79 (same code); the
// field masked and ORed in place with the bit read from it 79; the kind word as one expression 81;
// the load test named before the kind word 82. All restored.
// @retail 0x2cbf0
short function_2cbf0(long geometry, long category, short instance, dword flags,
    long tag, dword key, long material, byte *object, short mask, byte mode,
    void *context, long object_index, real intensity, long group, long group_type,
    bool outside, bool whole, bool first_component, real first_opacity,
    real second_opacity, real depth, byte level, point3f const *position, real radius)
{
    long index = g_4c0b78.current++;
    long result = index;
    if (index >= 850 || geometry == NONE)
    {
        result = NONE;
        --g_4c0b78.current;
    }
    else
    {
        s_44940_entry *entry = &g_4ba138[(short)index];
        entry->tag = tag;
        entry->unknown08 = key;
        entry->unknown00 = (entry->unknown00 & 0xe0000000) | (flags & 0x1fffff) | (((dword)category & 0xff) << 21);
        *(byte **)entry->unknown10 = object;
        entry->unknown10[10] = mode;
        *(void **)(entry->unknown10 + 12) = context;
        entry->flags = (entry->flags & 0xe0000000) |
            (((((instance & 0x7ff) << 9) | (geometry & 0x1ff)) << 5) | (group & 0x1f)) << 4 |
            (material & 0xf);
        entry->unknown10[11] = (entry->unknown10[11] & 0xf0) | (level & 15);
        long group_index = (entry->flags >> 4) & 31;
        if (group_index != 31)
        {
            if ((dword)g_4c0b78.entry_count <= (dword)(group_index + 1))
                g_4c0b78.entry_count = group_index + 1;
            ((s_42760_entry *)g_4c0b78.entries)[(entry->flags >> 4) & 31].type = group_type;
        }
        group_index = (entry->flags >> 4) & 31;
        if (group_index != 31 && object_index != NONE)
        {
            s_42760_entry *group_entry = &((s_42760_entry *)g_4c0b78.entries)[group_index];
            group_entry->depth = depth;
            group_entry->position = *position;
            group_entry->type = group_type;
            if (outside || g_509410)
            {
                ((box2f *)group_entry->unknown10)->x0 = -3.4028234663852886e+38f;
                ((box2f *)group_entry->unknown10)->y0 = -3.4028234663852886e+38f;
                ((box2f *)group_entry->unknown10)->x1 = 3.4028234663852886e+38f;
                ((box2f *)group_entry->unknown10)->y1 = 3.4028234663852886e+38f;
            }
            else if (whole || g_509411)
            {
                ((box2f *)group_entry->unknown10)->x0 = 3.4028234663852886e+38f;
                ((box2f *)group_entry->unknown10)->y0 = 3.4028234663852886e+38f;
                ((box2f *)group_entry->unknown10)->x1 = -3.4028234663852886e+38f;
                ((box2f *)group_entry->unknown10)->y1 = -3.4028234663852886e+38f;
            }
            else
                function_30290(position, (s_view const *)&g_4b9da0, &g_4b9e14, radius, (box2f *)group_entry->unknown10);
        }
        *(short *)(entry->unknown10 + 8) = mask;
        g_4c0b78.unknown0c[850 + index] = 0xff;
        if (object_index != NONE && g_4c0b78.count < 192)
        {
            g_4c0b78.unknown0c[850 + index] = (byte)g_4c0b78.count;
            ((long *)(g_4c0b78.unknown0c + 1700))[g_4c0b78.count] = object_index;
            ++g_4c0b78.count;
        }
        real scaled = intensity * 0.25f;
        long value;
        if (floor_to_long_inline(scaled) < 0) value = 0;
        else if (floor_to_long_inline(scaled) > 255) value = 255;
        else value = function_016ae0(scaled);
        g_4c0b78.unknown0c[index] = (byte)value;
        byte *target = *(byte **)entry->unknown10;
        if (target && object_index != NONE)
        {
            if (first_component)
            {
                long amount = (long)(first_opacity * 256.0f);
                if (amount < 0) amount = 0;
                else if (amount > 255) amount = 255;
                target[0x76] = (byte)amount;
            }
            else
            {
                long amount = (long)(second_opacity * 256.0f);
                if (amount < 0) amount = 0;
                else if (amount > 255) amount = 255;
                target[0x77] = (byte)amount;
            }
        }
        byte *section = (byte *)function_44940((short)index, (bool)((flags >> 12) & 1));
        entry->flags = (entry->flags & 0x1fffffff) | ((dword)*(word *)(section + 0x14) << 29);
        long object_tag = *(byte **)entry->unknown10 ? **(long **)entry->unknown10 : NONE;
        long kind = function_2d000(object_tag, entry->tag, (entry->flags >> 18) & 0x7ff);
        dword kind_word = (dword)kind << 29;
        kind_word |= entry->unknown00 & 0x1fffffff;
        bool load = entry->tag == NONE;
        void *added = function_449e0((short)index, load, (bool)((kind_word >> 12) & 1));
        entry->unknown00 = kind_word;
        if (added)
        {
            s_2cb30_state *state = g_4c0b78.state;
            if (!state->active)
                ++g_4c0b78.previous;
            if (entry->unknown00 & 0x40)
            {
                section = (byte *)function_44940((short)index, (bool)((entry->unknown00 >> 12) & 1));
                *(void **)(entry->unknown10 + 4) = NULL;
                word bits = *(word *)(section + 0x24);
                if (bits > 0)
                    *(void **)(entry->unknown10 + 4) = function_1332b0(bits, (s_bit_vector_pool *)state);
                if (!*(void **)(entry->unknown10 + 4))
                    entry->unknown00 = (entry->unknown00 & ~0x40) | 1;
            }
        }
        else
        {
            result = NONE;
            --g_4c0b78.current;
        }
    }
    return (short)result;
}
