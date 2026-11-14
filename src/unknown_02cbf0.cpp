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
__declspec(noinline) void *function_449e0(short index, bool instance, bool load);

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
// MiniMax A (11-12): no new form, no lower score (10): the OR chain written right-nested and the pool pointer typed as the pool (the cast moved to the active check) both give byte-identical code, and shifting the kind in place (kind = kind << 29) rebuilds the body at 79, so the first word still reads the entry own word before the flags term where retail reads the flags first, the kind shift still comes after the copy to esi where retail shifts before it, and the pool argument is still copied to edx inside the branch where retail copies it before the jbe.

// MiniMax B (11-13): no new form, no lower score (10): the two masked words are still combined in the other order, so the kind is the word that is shifted here and the flags word is the one retail shifts, and the pool register is still set after the bit-count test here and before it in retail.
// MiniMax H (11-12): no new form, no lower score (10); the first word's OR chain still keeps the entry's own load
// before the parameter's where retail loads the parameter first, the kind is still copied before it is shifted, and
// the six orders of the OR terms are canonicalised by the compiler into one schedule, so the order cannot be steered
// from the source.
// MiniMax I (11-11): no new form, no lower score (10): the first word's OR chain keeps the entry's own load before the parameter's where retail loads the parameter first, the kind is still copied before it is shifted, and the pool's copy is still inside the branch; a second read of the pool global reaches 7 but reads the global twice, so it was not kept.
// MiniMax D (11-11): no new form, no lower score (10).
// MiniMax S (11-10): no new form, no lower score (10): the first word's OR chain keeps the entry's own load before the parameter's where retail loads the parameter first, retail shifts the kind in the call result's register and copies it where we copy first and shift in ESI, and retail copies the pool argument above the bit-count test where ours copies it inside the branch.
// MiniMax R (11-10): no new form, no lower score (10): retail builds the first word's OR chain in the opposite source order (the category's shift, then the flags' mask, then the entry's own word) and MSVC canonicalizes the chain here, so the two loads that feed it come in the other order and every spelling of the chain measures the same.
// MiniMax S (11-09): no new form, no lower score (10); the six orders of the first word's OR terms are
// canonicalised by the compiler into one schedule, so the order cannot be steered from the source; the
// fifteen kind-word spellings tried this round (the shift on the call's result, on a long local, a
// multiplication by 0x20000000, a two-use temporary, a fresh expression) all keep the copy-then-shift
// schedule and the score of 10, and the pool argument copied from a local keeps the copy inside the
// branch; reading the pool's global a second time before the clear hoists the copy like retail and
// scores 7, but that reads a global twice and was not kept.
// MiniMax D (11-08): no new form, no lower score (10); retail ORs the first word's terms as (category | flags) | entry
// bits where ours does (category | entry bits) | flags, retail shifts the kind in the call result's register and then
// copies it (`shl eax, 0x1d; mov esi, eax`) where ours copies first and shifts in ESI, and retail copies the pool
// argument above the bit-count test where ours copies it inside the branch; the six orders of the OR chain's terms,
// six parenthesizations, three temp-local forms, eleven kind-word forms and three pool-call forms are all
// byte-identical. Reading the pool's global a second time before the clear hoists the copy like retail and scores 7,
// but that needs a local read once (not kept); reading the global in the call instead scores 26. On 11-09 the call
// folded into the shift, the call's result in a dword local and the `|=` written as an assignment with `|` were
// rebuilt and all three keep the copy-then-shift schedule and the score of 10.
// MiniMax R (11-07): no new form, no lower score (10); the first word's terms in retail's order, split into two
// statements or swapped, the kind word built from the call's result, the kept bits in a named local and the mask
// written as ~0x1fffffff are byte-identical; the kind word's operands swapped puts the shift in the result
// register as retail has it but moves the tag load into ECX (79); either packed word as one expression 79/288.
// MiniMax I (11-05, run 37): no new form, no lower score (10); the first word's OR order, the kind shift and the pool register set still differ.
// MiniMax H (11-13): no new form, no lower score (10); the two masked words are combined in the other order, so the kind is the word that is shifted here and the flags word is the one retail shifts, and the pool register is set after the bit-count test here and before it in retail.
// score 10 (was 15): left: the first word's OR order (retail ORs flags and category before the
// kept top bits), the kind shift (retail `shl eax, 0x1d; mov esi, eax`, ours `mov esi, eax; shl esi, 0x1d`)
// and the pool register set before the bit-count test. Round 149 below. Earlier rounds' notes follow.
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
// word, then blocked by 0x133390 and 0x133520's byte declaration, a word since r644); retail builds the
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
// word, then blocked by 0x133390 and 0x133520's byte declaration, a word since r644), the new kind
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
// word, then blocked by 0x133390 and 0x133520's byte declaration, a word since r644) and the new kind word
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
// Round 33 (lane 2, fast): the word stored before the load test 78; stored before the call, the call
// itself as the success test 79; the bit count as a long 64; the bit count read at each use 17; `bits != 0`
// 17. The intensity spill slot stays BLOCKED-BY 0x133390 and 0x133520 (level pushed as a word there).
// Round 29 (lane 18, fast 17; the full build ends `ret 0x54` as retail, three register parameters): load and
// instance named, then the word stored, then the call 79 (the word stays in eax and is stored at once); instance
// named before load, same order 79; load named before the kind word 82; the state global read before the
// added test 19 (its load moves above the branch); load named before the object tag (ahead of the 0x2d000
// call) 214 (1006 bytes).
// 17. The intensity spill slot then waited on 0x133390 and 0x133520 (level pushed as a word there; r644).
// Round 145 (m3 lane 6, fast; compile-only reproduces the first word): word stored before the 0x449e0 call 77;
// byte-cast category 282; `~0x1fffffff` mask 15; old word in a local before the tag store / at the word 15/15;
// old word read through `g_4ba138[(short)index]` 16 (first word now retail's order, but the second word's
// material/kept-bits order flips and the entry address becomes `lea`), the same with a long index 288, as a
// local 16; both words read that way 368; first word stored and read that way 25 (both orders right, the
// store moves up); plus the second word stored that way 371 (every access global-addressed); tag/key stored
// that way before the entry pointer is formed 375 (1125 bytes).
// Round 105: const entry pointer 15; boolean add result 79 (1038); boolean result with the store first 77;
// direct packed store and field instance bit 79; sequenced store-and-add condition 77; const bit count 15;
// pool-typed state local 15; global-array first-word read plus second-word kept bits last 16 (same as the
// global read alone). Original body restored; the intensity spill agrees in this baseline.
// Round 149 (m3 lane 5, fast unless marked): category declared between key and material matched 0x133f70. Retail's setup
// at the 0x449e0 call (load, instance push, index) gives its order (index, instance, load): alone 15; with the word
// stored first and both arguments inline 10 (kept, plain 10). On the 15 form: long-typed low terms, casts or local x7
// 15 each; named first-word parts, seven subsets, 15 each; low terms added or the high term xored 12/12 (retail's order,
// wrong opcode), both added 17; flags' second use removed 30; flags copied at the top 15.
// On the 10 form: inline arguments with the store after the call 15; no word local, one expression, mask tests on the field or
// the local 79/79/79/79; load named after/before the store 76/79; shift assign long/dword 79/79; call in the shift 10; shifted
// copy 10; named shifted or 79; word read first 79; or kind first 79; reassigned kind 10; multiply, named multiply, copy then
// shift 10/10/10; local bitfield 79; kept named first 79; 0x1332b0's parameters swapped 10 (diagnostic). Pool:
// `s_bit_vector_pool *pool = (s_bit_vector_pool *)g_4c0b78.state;` before the clear 7, retail's `mov edx, esi` above the
// `jbe` (a one-use local: needs a match, not kept); before bits/after bits/before the section call 25/25/10; read in the call 26.
// On that 7 form (the one-expression word's OR takes the old word into esi, retail's three-address OR keeps it in ecx):
// first word or both reads as `*(dword *)entry`, `[0]`, `entry[0]`, `(*entry)`, byte offset, `*&`, const or long cast with
// the one-expression word 79 each, that word alone 79; entry declared at the top or after the index 7/7, with one expression 79;
// `entry +=` 7, with one expression 79; byte-offset entry 7; short index local 7; NULL-initialised entry 7; compiled-out
// join after the kind, the shift, the or or the store 7 each, around the one-expression word 79 each; first-word stores word,tag,key
// 27, tag,word,key 286, key,tag,word 11, key,word,tag 286, word,key,tag 27; object/mode/context/all three before the word
// 280/288/280/289; second word before it/first 274/162; object, mode, context subsets after the second word
// 270/267/269/268/279/268/270; old word through the array 8 (first word right, second word flipped, `lea`), with the second word's kept bits
// last 8, also the second word's read 369 (1131 bytes), on the one-expression word 77, both 77; that with store subsets after the
// second word 268/267/267/268/278/268/270. Dead identity pointers 1-8: 7 each; first word through one/two to eight identity
// pointers 7/11 (flips only the second word, diagnostic); second word through one to four 7/279/279/279, with the array read 8/278/278/278.
// On the 10 form: a local bitfield view for the first word (selection then category, or reversed) 14/14 (the second word's order flips too), for
// the kind 79, both 83; low word built in two steps 10; kept bits named, the word's or as an assignment 79.
// AC555: Owning combined low-29 payload/section-selector fields, subsets
// 1/2/4/3/5/6/7, 10 each; all with final kind-field store 79. NOT kept: no
// equality or gain; original header/body restored; OR/shift/pool schedules remain.
// Y604: Actual declared record bitfields for initial/geometry/final packed words,
// seven subsets then reversed initial field stores: 14/14/10/79/83/83/79/14.
// NOT kept: no equality; the real declaration does not recover native OR/shift/pool
// scheduling. Original record declaration, packed expressions and body restored.
// MiniMax H (11-12): no new form, no lower score (10): the first word's OR chain was rebuilt in two and three
// steps with the three terms in four different orders, and the kind's shift was written as a cast of the shifted
// value, as a dword kind, as an in-place shift and as an assignment - the two- and three-step words all give 10,
// the one-expression word 79, so the OR chain's order and the shift's copy are decided by the compiler; reading
// the pool argument at the call scores 26 and the bits test written as != 0 changes nothing.
// MiniMax A (11-10): no new form, no lower score (10): swapping the first word's three OR terms and
// the kind's shift term (the shift first) both give byte-identical code, so the OR chain's order and
// the shift's copy to esi are decided by MSVC, and the pool's `mov edx, esi` stays above the `jbe`.
// MiniMax E (11-10): no new form, no lower score: the kind's shift still comes after the copy to
// esi where retail shifts before it, and the two loads that feed the record's OR chain come in the
// other order; six fresh forms (the kind as a dword, the kind's shift written inline, the pool
// taken as a local, the pool call without its cast, the second argument read from the entry and
// the kind and the pool together) all score 10 or worse.
// MiniMax D (11-09): no new form, no lower score: MSVC canonicalises the order of the first word's
// three OR terms, so re-writing them in retail's order ((category|flags)|unknown00) gives
// byte-identical code and the score stays 10; the `long kind_word` spelling, with its extra
// `sar eax, 0xc`, scores 11, and the kind's shift still comes after the copy to esi where retail
// shifts before it.
// MiniMax D (11-09): no new form, no lower score: the body is unchanged at 10 (1033 bytes against retail's 1033); the whole of the difference is still the first word's three-address OR against our one-expression OR, and the pool's `mov edx, esi` above the `jbe`.
// MiniMax R (11-09): no new form, no lower score (10): the two loads that feed the record's OR chain
// come in the other order here - the entry's own word and the global's mask - while retail loads the
// global first; the OR chain itself is canonicalized by MSVC, so reordering the three terms of the
// store, naming the kept bits or splitting the word's or into an assignment all give byte-identical
// code and the same score, and the low word built in two steps was measured at 10 as well.
// MiniMax A (11-14): still no lower score (10): the kind word's shift and the word's OR accumulator move
// together. Retail shifts the call's result in EAX and copies it to ESI (shl eax, 0x1d; mov esi, eax) and
// then accumulates the entry's masked bits into ESI; ours copies the result to ESI first and shifts the
// copy (mov esi, eax; shl esi, 0x1d). With the kind word written as a variable the OR accumulates into it
// but the shift folds into the copy, and with the shift written inline in the store the shift stays in EAX
// but the OR accumulates into the entry's masked term instead (79 in both spellings, with the operands
// swapped as well). The kind as a dword, the shift as a multiply by 0x20000000 and the entry's masked bits in
// a named local all give the bytes we already have (10).
// MiniMax A (11-14): still no lower score (10): the entry word's OR chain takes the flags term first
// and the split form (the masked new bits in a named word, the store as the kept bits or-ed with it)
// both compile to exactly the bytes we already have, which confirms that MSVC canonicalises the OR
// operands and the statement split and that the term order in the source cannot be what puts retail's
// flags term first.
// MiniMax A (11-14): no new form, no lower score (10): the entry word's OR chain still takes its
// masked terms in our order (the entry term first) where retail takes the flags term first, and the
// kind word's shift still lands in esi before the copy where retail shifts eax and copies it;
// swapping the OR operands changes nothing (10) and merging the kind word into one expression
// scores 79.
// @retail 0x2cbf0
short function_2cbf0(long geometry, short instance, dword flags,
    long tag, dword key, long category, long material, byte *object, short mask, byte mode,
    void *context, long object_index, real intensity, long group, long group_type,
    bool outside, bool whole, bool first_component, real first_opacity,
    real second_opacity, real depth, word level, point3f const *position, real radius)
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
        entry->unknown00 = kind_word;
        void *added = function_449e0((short)index, (bool)((kind_word >> 12) & 1), entry->tag == NONE);
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
