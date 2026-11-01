// @flags /O2 /Oi /Gr
#include "unknown_11c920.h"
#include "language.h"
#include "unknown_120d80.h"
#include <string.h>

struct s_cache_file
{
    HANDLE handle;
    byte unknown04[0x800];
};

struct s_cache_copy
{
    char path[256];
    long last_used;
    long state;
};

struct s_cache_copy_request
{
    char map_name[256];
    long priority;
};

struct s_cache_copy_progress
{
    long completed;
    dword total;
};

struct s_copy_header_view;
extern s_cache_file g_557c90[6];
extern long g_55aca8;
extern s_cache_copy g_55acac[8];
extern s_copy_header_view g_55b4f0;
extern dword g_55bcf0;
extern long g_55bcf4;
extern s_cache_copy_progress g_55bcf8;
extern long g_55bd00;
extern long g_55bd04;
extern long g_55bd08;
extern bool g_55bd0c;
extern long g_55bd10;
extern s_file_handle g_55bd14;
extern long g_55bd18;
extern long g_55bd1c;
extern bool g_55bd20;
extern char g_55bd21[0x103];
extern s_cache_copy_request g_55be24[2];
extern dword g_55c02c;

void __stdcall function_2138f0(long arg_0, long arg_1);
void function_213970(void);

#pragma optimize("s", on)
// score 131 (was 131): retail clears two storage blocks; separate globals require separate clears and
// cache-load scheduling differs. Tried language dirty first 145; version dirty first 145; current version
// tail 152; reset enabled first 145; scalar reset phase 145; forced language inline 169.
// Without the size pragma 172; stores in address order 145 (184 without the pragma); chained
// zero/NONE stores 145; language read at each use 143 (199 without the pragma). The two blocks
// are 0x557c90..0x55b4ec and 0x55b4f0..0x55c030; their globals are defined in three other files.
// Native progress/handle/both reset records 174/168/187; reversed clear phases 145;
// reversed with native progress/handle/both 175/168/187; native records after buffers 183.
// Separate allocation still prevents the two retail-wide clears; original body restored.
// Round 184: unsigned/native-int/immutable-signed/immutable-unsigned language results 145 each;
// query before all resets 142 (three-point gain dropped), after first bulk clears 144;
// requested-value-first comparisons and explicit void tail return 145/145. Original body restored;
// the separate allocations still prevent both complete bulk clears.
// Round 79: enum language result 145; first-element array clears 145; reversed progress fields 145;
// existing language wrapper 145; query after file/copy clears 144/144; version validation first 141;
// joint preference guard 158. All gains below the retention threshold; original body restored.
// Round 193: Native nonzero reset ownership at cache/copy/enabled phases, seven combinations 145 each;
// complete payload clears before scalar publication 131 kept, plain confirmed at 229 bytes. Two retail-wide
// blocks and expanded language query still differ.
// Round 193: On complete payloads: seven nonzero ownership combinations 131 each; NONE/zero/both chains 131
// each; query before all resets 128 (three-point gain dropped), between payloads and state 131. Retain only
// the complete-payload form at 131.
// Lane 17 r1: size pragma removed (diagnostic) 158; preferences pointer local 131; scalar resets in
// reverse order 131; progress record memset 141; enable flag after the NONE stores 131; every reset in
// address order 145. Ours hoists the NONE stores above the clears (separate globals don't alias them);
// BLOCKED-BY the global layout: both retail blocks' globals are also declared in src/unknown_214c30.cpp
// and src/unknown_12be90.cpp.
// Round 40: the two blocks need structs, but block A's globals are also used by src/unknown_214c30.cpp and
// block B's g_55bd04/g_55bd08 are defined in src/unknown_12be90.cpp. Without the size pragma 158 (language
// inlined, its read hoisted above the clears); with stores in address order/language first/NONE stores first
// 161/170/161; NONE stores first with the pragma 131.
// Round 79: enum language result 145; first-element array clears 145; reversed progress fields 145;
// existing language wrapper 145; query after file/copy clears 144/144; version validation first 141;
// joint preference guard 158. All gains below the retention threshold; original body restored.
// Round 198: Complete payload clear orders with the byte-tail block first/last/middle,
// size-ordered blocks and all blocks reversed: eight orders, all 131. Original body and bounds restored.
// Round 205: Coordinated invalidation/finish ordinary-inline pairs 1/0, 0/1, 1/1: 131/131/131;
// forced pairs 2/0, 0/2, 2/1, 1/2, 2/2: 392/973/392/973/1434.
// All provider declarations restored; separate allocation and the language expansion still block the two retail clears.
// Round 2 of lane 18: BLOCKED-BY the separate definitions (block A: src/unknown_213760.cpp and
// src/unknown_213d20.cpp, with an extern in src/unknown_214c30.cpp; block B: g_55bd04/g_55bd08 in
// src/unknown_12be90.cpp). Scalar zero stores before the clears 131, without the size pragma 158;
// enabled flag before the NONE stores 131; NONE stores before the zero stores 131, without the pragma 161.
// Round 37 (lane 1, fast): still BLOCKED-BY the global layout. Block A's NONE store between its two
// arrays 131; the enable flag before the NONE stores 131; NONE stores and flag first 131; retail's
// NONE/flag order right after the clears 131; the zero stores before the clears 131.
// Round 210: Actual preference-difference decisions reused for refresh and dirty publication:
// Boolean language/version/both 193/193/226; native language/version/both 185/185/221; byte/short
// pair 226/225. All regress with every match preserved. Original conditional publications,
// comparisons, calls, resets and whole body restored. Two complete storage blocks still require
// coordinated changes to protected definitions outside this queue.
// Round 53 (lane 9, fast): progress handle cleared by memset 146 (242 bytes); the language read at each use
// (`get_current_language()` in the test and the store; the size pragma keeps its out-of-line copy) 129 (232 bytes,
// two-point gain dropped); with the block-A clears after the block-B ones 129; with the NONE/flag stores first 129;
// with the language call on the left of the comparison 129. Still BLOCKED-BY the separate global definitions.
// Round 26 (lane 14, fast): retail also inlines the cached language getter 0x11cae0 (ours calls it under
// the size pragma). The existing function_11cae0 called directly 131 (same code); the language query
// between the zero stores and the NONE stores 131; the dirty flag set before the language store 131;
// the string clear after the enable byte 142; the two session words cleared last 131.
// Round 30 (lane 14, fast; still BLOCKED-BY the separate global definitions): the language local declared at
// the top 131; the last block word (g_55c02c) cleared first 131; the string and request clears after the
// scalar zero stores 131; the adjacent bool cleared just before the string clear 145 (242 bytes); block A's
// NONE store between block A's clears and block B's first clear 131.
// Round 116 (lane 6, fast): size pragma dropped with the existing getter 0x11cae0 called 158; that with
// the query first 170; the getter first under the pragma 128 (three-point gain dropped, same as the
// earlier query-first form); no pragma with the NONE/flag stores ahead of the scalar zeros 161; no pragma
// with the scalar zeros before the clears 158. Ours lays the five arrays apart (0x4bac30, 0x4bdd58,
// 0x4be9e8, 0x4be8d0, 0x4be598) where retail clears two contiguous blocks; still BLOCKED-BY the
// definitions in src/unknown_213760.cpp, src/unknown_213d20.cpp, src/unknown_2141f0.cpp,
// src/unknown_2148b0.cpp and src/unknown_12be90.cpp (machine 1).
// Round 33 (lane 14, fast; still BLOCKED-BY the separate global definitions): progress record / file handle
// reset by value-initialized assignment 171/165; constant-first version test 131 (same code); every scalar
// reset before the five clears 131 (aligned 44 against 35); progress record and handle both by memset 145.
// Diagnostic, size pragma removed 158: the preference tail is then retail's (version loaded to EAX, NONE
// stores after the clears), but the language load is hoisted into ESI above the clears and ESI/EDI are
// pushed at entry, where retail pushes EDI around its two block clears only and ESI after the language join.
// Round 40 (lane 11, fast): without the size pragma (diagnostic) 158: the shape after the clears is retail's
// (`or eax, -1`, the NONE stores among the language test, inlined getter calls) but the language read is
// hoisted above the clears into ESI, where retail reads it into EAX after them and copies it to ESI only
// after the first comparison (ESI pushed late). On that base: the existing wrapper 0x11cae0 / the header
// getter declared at the top 170/170; the wrapper in place 158; the wrapper between block A's and block B's
// clears 171. With the pragma: the NONE and enable stores after the preference updates 131 (same code).
// Round 64 (lane 12; one-link LTCG env with the build's stand-in caller, equal to the full listing at 229
// bytes): with the two blocks as struct globals (diagnostic only), no size pragma, a language getter that
// returns the global re-read after its fill (`if (g == NONE) g = ...; return g;`) and the two preference
// updates as inline compare-and-set helpers taking the new value, the env listing equals retail byte for
// byte (155 bytes, retail's late ESI push and early EDI pop included); so does one compare-and-set helper
// taking the field's address and the value, called for both fields. On the blocks: no pragma alone 156
// bytes (language load hoisted into ESI above the clears); the existing wrapper 0x11cae0, the comparison
// reversed, the local declared first: the same; NONE stores after the query worse; the helpers with the
// current getter: hoisted; the getter used at each use: load after the clears but the getter reruns after
// the call; the re-read getter alone: load after the clears in EAX, copied to ESI before the compare. On the
// separate globals (--fast): no pragma + re-read getter 153; with the helpers 153; helpers under the size
// pragma stay out of line. Still BLOCKED-BY the block definitions above.
// Round 36 (lane 13, fast): the language query and both preference updates in their own block 131 (same
// code), without the size pragma 158; one dirty store after both tests behind a changed flag 173 (259 bytes);
// block B's scalar zero stores ahead of its header clear 131; the version compared as a dword 131; the
// existing wrapper 0x11cae0 called in both the language test and the store 129 (232 bytes, two points). Still
// BLOCKED-BY the separate global definitions (block A in src/unknown_213760.cpp and src/unknown_213d20.cpp).
// Round 60 (lane 16, fast): without the size pragma (diagnostic base 158): the language local declared at the top
// and assigned after the NONE stores 158; block B's clears before block A's 158; block A's NONE store right after
// block A's clears 161; the handle record value-initialised (`= s_file_handle()`) 158; with the pragma 165.
// With the pragma: the progress record value-initialised 171; the request clear before the string clear 131;
// the last block word cleared right after the requests 131; the NONE stores g_55bd00, g_55bd10, g_55aca8 then the flag 131.
// Still BLOCKED-BY the separate global definitions (five arrays, two retail-wide clears).
// Round 126 (lane 5, fast): the language local on the left of its comparison 131; the NONE/enable stores
// ahead of the scalar zero stores 131; the version as a named const local 131; the dirty flag set before
// each preference store 131; the handle reset first among the scalar stores 131 (all the same code). The
// block definitions are now in src/unknown_213760.cpp, src/unknown_213d20.cpp, src/unknown_2141f0.cpp,
// src/unknown_2148b0.cpp and src/unknown_12be90.cpp.
// @retail 0x2136c0
void function_2136c0(void)
{
    memset(g_557c90, 0, sizeof(g_557c90));
    memset(g_55acac, 0, sizeof(g_55acac));
    memset(&g_55b4f0, 0, 0x800);
    memset(g_55bd21, 0, sizeof(g_55bd21));
    memset(g_55be24, 0, sizeof(g_55be24));
    g_55bcf0 = 0;
    g_55bcf4 = 0;
    g_55bcf8.completed = 0;
    g_55bcf8.total = 0;
    g_55bd04 = 0;
    g_55bd08 = 0;
    g_55bd14.handle = NULL;
    g_55bd18 = 0;
    g_55bd1c = 0;
    g_55bd20 = false;
    g_55c02c = 0;
    g_55aca8 = NONE;
    g_55bd0c = true;
    g_55bd00 = NONE;
    g_55bd10 = NONE;
    long local_0 = get_current_language();
    if (global_preferences_globals.current.unknown18 != local_0)
    {
        function_2138f0(0, 6);
        global_preferences_globals.current.unknown18 = local_0;
        global_preferences_globals.dirty = true;
    }
    if (global_preferences_globals.current.unknown1e8 != 0x2651)
    {
        function_2138f0(0, 6);
        global_preferences_globals.current.unknown1e8 = 0x2651;
        global_preferences_globals.dirty = true;
    }
    function_213970();
}
#pragma optimize("", on)
