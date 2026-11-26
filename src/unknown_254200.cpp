// @flags /O2 /arch:SSE /Gr
#include "shader_storage.h"
#include "view_context_storage.h"
#include "unknown_11c920.h"
#include "globals.h"
#include <xtl.h>
#include <string.h>
#include "unknown_13927e.h"
#include "sampler_state.h"

struct s_bitmap_view;
struct s_type_7ba8e9;
D3DTexture *function_12360(s_bitmap_view *arg_0, real arg_1);
bool function_14390(short arg_0, s_type_7ba8e9 *arg_1, real arg_2);
void function_14bc0(short arg_0, short arg_1, bool arg_2);
dword function_1cc30(long arg_0);
void __stdcall function_1c710(void *arg_0);
void function_15180(D3DPIXELSHADERDEF const *arg_0);
extern long g_4858b8;
extern D3DPIXELSHADERDEF g_484f68;
extern dword g_4b8448, g_4b8308, g_4b82e8, g_4b82f4, g_4b82f8;
extern dword g_4b8324, g_4b82ec, g_4b82e0, g_4b8450;
byte g_51ec9a;
#include "texture_stage_storage.h"
#include "texture_stage_dimensions.h"
D3DTexture *function_12310(s_bitmap_view *arg_0, real arg_1);
void function_1cf50(void);
void function_14f60(short arg_0, short arg_1);
void function_0222d0(D3DRENDERSTATETYPE arg_0, dword arg_1);
void function_0224f0(dword arg_0, D3DTEXTURESTAGESTATETYPE arg_1, dword arg_2);
real function_1392a9(long arg_0);
extern long g_4ba04c;

/* the tag index of one of the global interface tag references (src/interface.cpp) */
long function_13e615(short index);

PRIVATE __forceinline s_bitmap_view *function_254202(long arg_0)
{
	byte *local_0 = g_4e3b44[arg_0 & 0xffff].bytes;
	s_bitmap_view *local_1 = NULL;
	if (local_0 && *(long *)(local_0 + 0x44) > 0)
	{
		local_1 = *(s_bitmap_view **)(local_0 + 0x48);
	}
	return local_1;
}

PRIVATE __forceinline void function_254491(real arg_0, color3f const *arg_1)
{
	real local_0 = arg_1->blue * arg_0;
	real local_1 = arg_1->green * arg_0;
	real local_2 = arg_1->red * arg_0;
	D3DDevice_SetVertexData4f(5, local_2, local_1, local_0, 1.f);
}

#include "frame_storage.h"
#include "reset_storage.h"
extern byte g_4670bc;
struct s_render_reset_state;
void function_16b10(s_render_reset_state *state);
void function_14ac0(void);
void __stdcall function_15720(real x, real y);

#include <stddef.h>
typedef char assert_254200_offset[(offsetof(D3DPIXELSHADERDEF, PSAlphaInputs) == 0) ? 1 : -1];
#define program_words_254200 (&g_484f68.PSAlphaInputs)

__declspec(noinline) void function_254200(void);

// @retail 0x254200
void function_254200(void)
{
    s_bitmap_view *first = function_254202(function_13e615(12));
    s_bitmap_view *second = function_254202(function_13e615(13));
    g_4670bc = true;
    function_16b10((s_render_reset_state *)g_485b48);
    g_51ec9a = false;
    if (function_12360(first, 0.0f) && function_12360(second, 0.0f))
    {
        g_51ec9a = true;
        function_14bc0(0x1d, 0, false);
        if (g_485607 && !g_4858b8)
            function_14ac0();
        D3DDevice::Clear(0, 0, 0xf0, 0, 1.0f, 0);
        function_15720(0.0f, 0.0f);
        function_14390(0, (s_type_7ba8e9 *)first, 0.0f);
        sampler_state_write(0, D3DTSS_ADDRESSU, 4);
        sampler_state_write(0, D3DTSS_ADDRESSV, 4);
        sampler_state_write(0, D3DTSS_MAGFILTER, 2);
        sampler_state_write(0, D3DTSS_MINFILTER, 2);
        sampler_state_write(0, D3DTSS_MIPFILTER, 1);
        sampler_state_write(0, D3DTSS_MAXANISOTROPY, 0);
        sampler_state_write(0, D3DTSS_MIPMAPLODBIAS, 0);
        sampler_state_write(0, D3DTSS_MAXMIPLEVEL, 0);
        sampler_state_write(0, D3DTSS_COLORSIGN, 0);
        sampler_state_write(0, D3DTSS_ALPHAKILL, 0);
        g_4b8448 = 0x901;
        D3DDevice::SetRenderState(D3DRS_CULLMODE, 0x901);
        g_4b8308 = 0x10101;
        D3DDevice::SetRenderState(D3DRS_COLORWRITEENABLE, 0x10101);
        g_4b82e8 = 1;
        D3DDevice::SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
        g_4b82f4 = 1;
        D3DDevice::SetRenderState(D3DRS_SRCBLEND, 1);
        g_4b82f8 = 1;
        D3DDevice::SetRenderState(D3DRS_DESTBLEND, 1);
        g_4b8324 = 0x8006;
        D3DDevice::SetRenderState(D3DRS_BLENDOP, 0x8006);
        g_4b82ec = 0;
        D3DDevice::SetRenderState(D3DRS_ALPHATESTENABLE, 0);
        g_4b82e0 = 0x207;
        D3DDevice::SetRenderState(D3DRS_ZFUNC, 0x207);
        g_4b8450 = 0;
        D3DDevice::SetRenderState(D3DRS_ZBIAS, 0);
        function_1cc30(12);
        function_1c710(g_51f0f0);
        memset((D3DPIXELSHADERDEF *)program_words_254200, 0, sizeof(g_484f68));
        g_484f68.PSTextureModes = 1;
        g_484f68.PSCombinerCount = 1;
        g_484f68.PSFinalCombinerInputsABCD = 0x8040000;
        function_15180((D3DPIXELSHADERDEF *)program_words_254200);
    }
}

#undef program_words_254200

// score 4 (was 4): in the two records that follow the second reservation retail loads the
// 16777215 constant before it reloads the spilled y+h from the stack, ours reloads y+h
// first; only that order differs, the pool address of the constant is masked by the check.
// MiniMax G (11-12): no new form, no lower score (4): the constant through a named local used for both coordinates keeps 4, the point copy declared before the half size gives 15, the point's coordinates through parallel pointers give 341; only the order of the two loads after the second reservation differs.
// MiniMax C (11-09): no new form, no lower score (4 in the full build; the fast check's reduced image carries a
// caveat, one global reference unresolved, so the 80 it reports is not this function's score): the only difference
// left is the order of the two loads after the second buffer reservation, where retail loads the 16777215
// constant before it reloads the spilled y+h and we reload first.
// MiniMax S (11-09): no new form, no lower score (4); in the two records after the
// reservations retail loads the 16777215 constant before it reloads the spilled coordinate
// and we reload first; the constant through a named local and the member form of the 4f
// call keep the same code, so only that order is left.
// MiniMax E (11-09): no new form, no lower score (4).
// MiniMax I (11-06): no new form, no lower score; the 16777215 constant through a named local keeps 4.
// MiniMax C (11-05): no new form, no lower score; the only difference left is the order of the two loads after the buffer reservation in the second and third vertex records, where retail loads the 16777215 constant before it reloads the spilled y+h and we reload first; the permuter tried 173 forms in 1803 s and none beat 4.
// MiniMax G (11-04): the point's x and y taken through parallel pointers 341 (the point copy is
// retail's shape); the permuter did not reach a variant on this loaded machine, so it was stopped:
// no lower score.
// MiniMax I (11-01): the member 4f form and the swapped y operands keep 4, the constant as a helper parameter 8, the point and size swapped 15, the y sum as a named local 151, the point and size below the colour call 214 and the free 2s form 392; no new form, no lower score.
// Round 34 (lane A, machine 3): the constant through a named local keeps 4, so the order is the
// live-in order of the whole-program build (retail reloads ascending, ours descending) and not
// reachable from here.
// first; the constant's pool address differs as well and the check cannot mask it.
// shared left edge lifetimes 162;
// shared right edge lifetimes 118;
// shared left/right edge lifetimes 171;
// shared bottom edge lifetimes 150;
// shared left/bottom edge lifetimes 146;
// shared right/bottom edge lifetimes 142;
// shared left/right/bottom edge lifetimes 189;
// read-only point and extent snapshots 4;
// second-vertex point after/before the texture call 133/138; scoped point 133;
// position helper for vertices 2 and 3/all four 8/8;
// texture-position helper for vertices 2 and 3/all four 4/4; comma-sequenced pair 4.
// inline color helper 339; direct color products 4; color aggregate 33; color value input 218;
// scalar color inputs 4; red-first color products 9; color home names 4; snapshot home names 4;
// native point copy 4; point copy assignment 4; reference color input 4.
// MiniMax R (11-10): no new form, no lower score (4): both sides are 1109 bytes and the only
// difference is the order of two independent loads in the second and third vertex records, where
// retail loads the 16777215 constant before it reloads the spilled y+h and we reload first; the
// pool address of the constant is masked by the check, so only the order counts. The forms the
// earlier rounds record (the constant through a named local, the member 4f form, the swapped y
// operands, the y sum as a named local, the free 2s form, the point through parallel pointers) all
// keep 4. Note that the fast check asked for several addresses at once reports 80 for this
// function, because the reduced image then leaves a global reference unresolved and the constant's
// pool address is no longer masked; the single-address fast check and the full build both give 4.
// Round 98 (m3 lane 10, single-address fast, control 4): the 16777215 constant as a file-scope `const real`, a
// `static const real`, a double literal, an integer literal or `(real)0xffffff` 4 each (same code: the constant folds in the
// front end); the point as two scalar coordinate locals after or before the size 107/182 (1113 bytes: float copies and a
// `push ecx` frame, so the copied point record is retail's shape), declared y, x 107; no point copy at all 288; the member
// 4f form in the first vertex only or the second only 4/4; a const size or a const point copy 4/4; the second vertex's x
// sum or both y sums with the size first 4/4. What differs is still only the order of the constant's load and the
// spilled y + size reload after the second and third reservations: retail's order is the order in which the two values are
// defined in the first vertex's code (the constant at its stores, then the y sum), ours puts the sum first; 0x2548f0 has
// the same pair the other way round (retail the earlier-defined coordinate first, ours the constant).
// Round 416: Both cached-state helper calls 4; native member state calls 4; member End 4;
// free Begin 247 (1053 bytes); direct availability rejection 4; active-quad rejection 4;
// explicit texture selection 4; separate availability failures 4. Original calls and body restored;
// the two depth/top-coordinate reload pairs remain.
// Round 115 (m3 lane 16, single-address fast, control 4; diagnostics reverted): 0-7 dead initialised locals at the top
// 4 each (no phase effect); k distinct float stores to probe globals at the top of the drawing block flip the pair to
// retail's order only at k = 7 (k = 1-6, 8, 9, 15 and 16 keep ours), so the order behaves like a hash-order tie, not a
// period. Source forms: the size as `2.f * arg_2` 4 (same code); an early return when the draw flag is clear 4 (same
// code); the two dimension stores reading the bitmap fields directly, without the two named locals 8 (the field reads
// swap).
// Also: the two readiness tests as nested ifs 4; the bitmap pointer declared in the if condition 4; the two
// dimension locals declared width first 4 (all same code).
// Round 105 (m3 lane 1, single-address fast, control 4): the two dimension locals typed short 224 (1113 bytes); the
// texture and dimension block as a call of the existing 0x14390 (`function_14390(0, bitmap, 0.f)`) 265 (1073 bytes, it
// stays a call); the size doubled in place in its parameter (`arg_2 *= 2.f;`) 19 (slot changes only, the reload pair
// unchanged); the bitmap chosen by default-then-override (`= local_0; if (arg_4) = local_1;`) 13; one named globals
// getter result for both bitmap lookups 284 (1088 bytes). The two reloads stay swapped. The texture and dimension block is
// 0x14390's body: with `inline` on 0x14390's definition (its file is /Ob1) the call expands here to the same code, 4, but
// five other callers then expand it as well (0x144f0 79, 0x14560 119, 0x43a40 295, 0x254200 150, 0x2548f0 553): not kept.
// Round 98 (m3 lane 14, single-address fast, control 4): the bitmap choice as an if/else 4; the third vertex's x sum
// written size first 4; the colour helper's 4f call in member form 4; the two dimension locals as `int` 4 (all the same
// code); the two cached render-state stores after their SetRenderState calls 10.
// Round 91 (m3 lane 15, single-address fast): through the XDK's IDirect3DDevice8_ wrappers with a NULL device: the four
// vertex 4f calls 10 (the pair unchanged; the colour helper's three spill slots permuted), the four 2s calls 4,
// Begin/End 4, the two SetRenderState calls 4, the colour helper's 4f call 4, every stream call 10.
// Round 102 (m3 lane 14, single-address fast, control 4): the colour helper's parameters in the other order 4 (same
// code); the four texture/position pairs as a forceinline quad helper taking the point pointer and the size 259 (1205 B,
// the point re-read at each use), taking the point copy by value 135 (1097 B); the point copy and size declared at the
// top of the body 4 (same code); diagnostic (not kept) a dead float product ahead of the point copy 4.
// M3 lane 8 r177 (single-address fast, control 4; references that are not folded back): the size through a `real &`
// 259 (1205 B, the size re-read at each use); the point copy through a `point2f const &` 259 (1205 B); the 16777215
// constant through a `real const &` to a named value 8 (the colour products' spill slots swap, the pair unchanged); the
// first vertex's y sum written size first, the others as now 4; the second vertex's y sum size first only 4 (same code).
// Reading: the colour block reloads its three spilled products from the highest slot down in both builds; only the
// constant and the y sum after the second and third reservations come back in the other order.
// Round 194 (m3 lane 5, single-address fast, control 4): the size `arg_2 * 2.f` written at each of its eight uses
// 221 (1118 B); the point copy and size declared after the 0x1cf50/0x1c710 calls 16 (the pair unchanged); the colour
// helper ahead of Begin 48 (diagnostic); each corner through one `point2f corner` assigned per vertex 270 (1205 B); the
// colour helper plain `inline` 339 (985 B, out of line). Reading: retail reloads the depth constant and y + size in the
// order they are first computed (call arguments right to left: the constant first), ours in the reverse order; the
// colour helper's three products reload in declaration order in both builds.
// Round 98 (m3 lane 2, single-address fast, control 4): descriptive names, to test a name-hash tie: the point copy
// and size as center/size 4; the three bitmap pointers as first/second/bitmap 4; the dimension locals as height/width
// 4; all seven together 4; the five parameters named 4 (all the same code: the reload pair does not follow names).
// M3 lane 15 r98 (single-address fast, control 4): the doubled size as a two-component point used x for x and y for y
// 136 (1113 B); the four edges computed into a four-real array ahead of the calls 173; the depth pair from a two-real local
// array 4 (same code); the first vertex's corner in a point copy updated in place (x -= size, y += size) and reused 206
// (1114 B); two corner points (low, high) computed ahead of the calls 173. The constant and y + size still reload in our order.
// M3 lane 15 r100 (single-address fast, control 4): the two dimension shorts through named width/height fields of the
// file's bitmap view 4; the draw-ready flag global typed bool 4; the readiness test as an early return 4; the point copy
// through memcpy 4 (all the same code); the vertex register spelled 0 as in unknown_01cf50.cpp's quads 8 (not the same
// register: the opcode word changes).
// M3 lane 5 r203 (fast with 0x254200 matched; control 4): the colour helper's products written scale first 4; the
// texture lookup's result through a named texture local before the store 4; the colour helper's scale as a `real const &`
// 4; the two dimension locals `long const` 4; the chosen bitmap declared ahead of the readiness test and assigned inside
// it 4 (all the same code). Reading: the swapped pair is loaded twice per record, at the top of the block after the
// texture-coordinate reservation and again after the vertex reservation, in the same order both times, so the order is
// set where the three values' registers are first loaded after a call, not at their uses.
// M3 lane 12 r143 (single-address fast, control 4): 8, 9, 10, 11, 12, 13, 14, 16, 20 and 24 unused initialised locals at the
// top 4 each (round 115 measured 0-7: the pair does not follow the referenced-local count); the four edges named where
// first used (left and top before vertex 1, right before vertex 2, bottom before vertex 3) 174 (1133 B: the sums move
// ahead of the reservations), top ahead of left 174, only the two y edges named 126 (1119 B), only the two x edges 153
// (1119 B). Diagnostic: 0x2548f0's body moved above this function in the file 4 (0x2548f0 2), so the order in which the
// constant's pool entry is first used in the file does not set the pair either.
// M3 lane 4 r220 (single-address fast, control 4; the reload triple after the second and third reservations is retail
// x, constant, y + size, ours x, y + size, constant): vertices 2 and 3 through the member 4f form 4; y + size named as a
// const local ahead of the state calls 206; the four edges as const locals after the colour call 173; integer zero
// literals for the bitmap calls' real arguments 4 (same code); both cached render-state stores embedded in the call
// arguments 4, with the triple then constant, x, y + size (one alone leaves ours); the cache read back as the argument
// the same 4. Diagnostic: 6 or 7 float probe stores ahead of the size give retail's triple, 0-5 and 8 ours, and with the
// embedded stores 1, 2, 3, 5 and 7 all give ours: a hash-like tie that no natural form has reached.
// M3 lane 4 r223 (single-address fast, control 4): the size as `arg_2 * 2` 4 and as a copy doubled in place
// (`size *= 2.f`) 4 (same code each); the point copied by an aggregate initialiser from its two fields 88 (1117 B,
// float loads); the cull mode spelled with its enumerator 4; the size as a copy added to itself 15 (addss).
// M3 lane 7 r190 (single-address fast, control 4): the shared y + size bound to a `real const &` for vertices 1-2 64
// (1113 B); the point copied member by member 88 (1117 B); the depth constant through a no-code forceinline accessor at
// all eight uses 4; every positive sum, x and y, written size first 4; every device call of the body in member form
// (Begin, 2s, 4f, render state, End) 4. All restored.
// M3 lane 10 r132 (single-address fast; control 4): the doubled size computed after the 0x1cf50/0x1c710 calls with the
// point copy still ahead of them 20 (the colour products' spill slots swap, the pair unchanged); the point copy after the
// calls with the size ahead 16; both after Begin 116 (1093 B); the chosen bitmap selected ahead of the readiness test 52;
// the choice as `if (!arg_4) ... else ...` 4 (same code). All restored.
// M3 lane 2 r116 (single-address fast, control 4): the point copy declared ahead of the size 15 (the copy's two dword
// moves land after the size product, the colour spills swap); the colour helper's products computed red, green, blue
// 9 (the colour loads and stores reorder); the products written straight into the helper's 4f call 4 (same code); the
// products stored into a scaled colour aggregate 28 (first +0xe: it takes a frame home); the colour by `color3f const &`
// 4 (same code). The constant and y + size still reload in our order after the second and third reservations.
// M3 lane 3 r278 (single-address fast, control 4): the vertex 4f calls through the member wrapper for vertex 3 only 4,
// vertex 4 only 4, vertices 1 and 4 4, vertices 3 and 4 4, vertices 1 and 2 4 (all the same code; the wrapper's extra
// inline level does not move the constant / y + size reload pair).
// M3 lane 6 r213 (fast, control 4): 48 random combinations (2 to 13 at once) of thirteen same-code natural forms: each
// vertex's 4f call in member form (four separate choices), the colour helper's 4f call in member form, each render-state
// call in member form (two choices), the closing call in member form, the size as `arg_2 * 2`, the bitmap choice as if/else, the
// dimension locals as `int`, the readiness test as nested ifs, the colour helper taking `color3f const &`: all 48 give
// 4 with the same aligned code, so the extra inline levels and their argument temporaries never reach the reload pair.
// Also r213: the four vertices through a file-local forceinline position helper (x, y) issuing the far-depth 4f call 8
// (the colour helper's spills swap, the pair stays).
// M3 lane 7 r198 (single-address fast, control 4): the readiness test held in a named bool 4 (same code); the point copy
// as a `point2f const &` to the parameter's point 341 (1162 B); the point copy, then the size, after the 0x1cf50/0x1c710
// calls 22 (the pair unchanged); the two dimension reads ahead of the texture store 56 (1110 B); the bitmap selection
// written in the test and again in the block, no shared local, 239 (1116 B). Reading: in this function, in 0x2548f0 and
// in 0x281b60's walk, ours emits each two- or three-value call-path reload group in exactly the reverse of retail's order
// (here retail x, constant, y + size; ours x, y + size, constant), while a one-value group (the constant alone after the
// second vertex's reservation in 0x2548f0) agrees. All restored.
// M3 lane 3 r281 (fast, control 4): the two cached render-state writes through the existing 0x222d0 setter, as 0x2548f0
// does, 4 (same code; kept, it replaces that setter's body written by hand); on it: the four vertex 4f calls in member
// form 4; End in member form 4; the point copy and size `const` 4 (same code each); the four 2s calls in C form 392
// (914 B, they stop inlining); Begin in C form 247 (1053 B). The setter's two inlined parameters do not move the pair.
// M3 lane 3 r283 (fast, control 4): the colour helper copying the colour into a const local before its products 218
// (1133 B); the colour products and the colour 4f call written at the call site 4 (same code); the point copy declared
// and then assigned 4 (same code); the point copy and size declared in a block of their own around Begin..End 21 (the
// pair unchanged); the four vertex 4f calls through the device-pointer wrapper given the device global 4 (same code).
// M3 lane 3 r286 (single-address fast, control 4): separate `width` and `height` products for the x and y offsets 127
// (1113 B); the doubled size computed at the top of the readiness block 33 (the pair stays ours); at the top of the
// function 283 (1103 B); the size parameter doubled in place with no size local 19; the bitmap choice as `if (arg_4)
// local_0 = local_1;` reusing the first view local 41. All restored.
// M3 lane 1 r144 (single-address fast, control 4): the depth through a `real const &` to a named value 8 (the colour
// block's three spills and reloads come back red, green, blue where the control has blue, green, red; the pair stays ours);
// that with the two dimension locals dropped (fields read at the stores) 12; that with the point copy declared ahead of
// the size 17 (the colour reloads then xmm2, xmm1, xmm0 from 0x24, 0x18, 0x20: neither slot nor register order); the size
// parameter doubled in place with the point copy after the 0x1cf50/0x1c710 calls 30; the point copy at the top of the
// readiness block, ahead of the bitmap choice 96 (1093 B). All restored; the x, y + size, constant group stays ours.
// M3 lane 14 r135 (fast, control 4): the bitmap lookup helper as two nested ifs 4 (same code); each corner through a
// forceinline (u, v, x, y) helper issuing the texture-coordinate and position calls 4 (same code); the drawing block in a
// forceinline helper taking the point, scale, size and colour 230 (1095 B); the colour helper taking the colour by value
// 218 (1133 B); the four texture-coordinate calls through a forceinline (u, v) helper 8. Reading: a scan of every function's
// reload groups right after a call on a skipped path finds matched functions in both orders (0x22aff1 reloads its
// second-declared local first, 0x13f700 its first-declared), so no register, declaration or definition order sets this pair.
// M3 lane 5 r225 (single-address fast, control 4): named real values that the optimiser merges, to change how many real
// values the body has without adding code (round 150's reading on 0x2548f0): the y offsets through a second real copied
// from the doubled size 4; the depth and w as two named reals (`w = depth`) 4; x and y read into two reals from the point
// copy 4; the doubled size as a product with a named `2.f` 4; w written as the depth times `1.f` 4 (all the same code).
// M3 lane 8 r219 (fast, control 4; 0x254200 matched, 0x2548f0 2): the system headers included first 4; the view getter
// helper as one ternary return 4; the colour helper defined just above this function 4 (all the same code); the draw-ready
// flag read into a bool ahead of the texture block 238; the bitmap lookup index as a word cast 279 (1100 B). Reading: in this
// function, 0x2548f0 and 0x270d90 ours emits each reload group in descending register order (here x, then xmm1, xmm0), retail
// in ascending order after any plain load (x, then xmm0, xmm1). All restored.
// M3 lane 11 r147 (single-address fast, control 4): a dead forceinline helper holding 1-8 named locals called at the top,
// ahead of the size, ahead of Begin, ahead of the first or the second vertex 4 each (40 runs); one to three redundant
// paren pairs around the size product, the y sums, every sum, the first x or y sum or a colour product 4 each (17 runs);
// 0-40 unused initialised locals at the top 4 each (no period-8 or period-32 state); diagnostic, the depth as a non-const
// file-scope real 259 (1205 B); the point read through pointers to its x and y (0x2548f0's form) 341 (1162 B).
// M3 lane 5 r231 (fast, control 4): a whole-image scan of call-edge reload groups (two or more float reloads on a
// reservation's slow path) finds only this function and 0x2548f0 with a differing order; the matched 0x2353a5 and
// 0x2477b0 reload theirs in descending register order as ours does. The depth as a `real const` declared at the top 4;
// the point copy direct-initialised 4; the point parameter a const pointer 4 (all the same code). Diagnostics: the first
// vertex's depth and w as 0.f 134, its depth alone 0.f 125 (the constant no longer stays live). All restored.
// M3 lane 15 r130 (single-address H2M_FAST, control 4): true `__assume(p != NULL)` on the point and colour parameters at
// the top, ahead of the point copy, both, and on the chosen bitmap inside its test 4 each (6 runs); the minus edges
// written `x + -size` 4; the two dimension locals, the chosen bitmap, or the size and point copy declared bare at the top
// and assigned in place 4 / 4 / 4 (all the same code: the x, y + size, constant group stays ours).
// M3 lane 1 r153 (single-address H2M_FAST, control 4): the scale, size and flag parameters `real const` / `bool const`
// 4; the chosen bitmap a const pointer 4; the drawing block (primitive start to end) in a scope block of its own 4
// (all the same code); the size read through a `real const *` to the size local at the x sums 259 (1205 B, re-read per
// use). Diagnostic: 0x2548f0's body emptied 4 (0x254200 MATCH), so the pair does not follow the other function's
// compilation.
// M3 lane 6 r229 (single-address H2M_FAST, control 4): the colour helper scaling a copy of the colour in place 218 (1133 B);
// the two lookup tags read into named longs before each bitmap lookup 4 (same code); the colour helper storing its three
// products into a colour local member by member 28; the point copy and size declared after the bitmap choice, ahead of
// the texture block 57; the size alone there 25. The x, y + size, constant group stays ours.
// M3 lane 7 r216 (single-address H2M_FAST, control 4): a 32-run grid of five same-code named values in every
// combination (the texture lookup result named before its store, the doubling factor as a named `2.f`, x and y read
// into named reals from the point copy for all four vertices, the depth as a named real, w as a second named real)
// 4 each; the depth through an `extern real const` declared above the file's first function and defined after this
// one (so the front end cannot fold it) 4 (the whole-program build folds it); that in both functions 4. Diagnostic,
// restored: the file built with each processor target (G5, G6, G7) 4 (0x2548f0 2): the reload order ignores them.
// M3 lane 5 r236 (single-address H2M_FAST, control 4): the depth written as a double literal `16777215.0` 4, as
// `(real)0xffffff` 4, w alone spelled `16777215.0f` 4 (all fold to the same constant); the size as `arg_2 * 2.0` 60
// (1099 B, a double product); Begin alone through the device-interface inline with a NULL device 4, End alone through it
// 4 (same code). Diagnostics, reverted at once: a favour-size optimize pragma around this function 773 (608 B), so the
// file was not built for size; the colour call after the first texture-coordinate call (push order changed) 52.
// M3 lane 7 r217 (H2M_FAST), at 4: the four vertex position calls through the platform's device-interface inline that
// takes a device pointer (one more inline layer) 10 (the colour block's three spills change slots, the pair stays ours);
// the two-short texture calls through that inline 4 (same code), in the plain C form 392 (914 B, no longer expanded);
// every Begin, texture, position and End call through it 10; the half size as `2.f * arg_2` 4 (same code).
// M3 lane 7 r219 (single-address H2M_FAST, control 4): the doubled size as `arg_2 / 0.5f` 4 (same code); the colour helper's
// green product ahead of blue 8; the two dimension stores height first 6; the first vertex's w spelled `1.6777215e7f` 4
// (folds); the bitmap choice parenthesised 4 (same code). The x, y + size, constant group stays ours.
// M3 lane 7 r222 (single-address H2M_FAST, control 4): cause work. A stand-alone compile-only model of the inline
// reservation (kit in build/r222 of this lane) shows the reload group's order is set by the optimiser's own numbering
// of the function's values, not by any source order: it changes with the count of unrelated statements, in windows
// where the shared temporary's number crosses a multiple of 32. On this body, measured with k probe stores to a scratch
// array ahead of the size (diagnostic): ours at k = 0-2, retail's order at k = 3-6, ours again from 7; three integer
// copies, or two real stores with one integer immediate store, count the same, so the body is three assignments that
// leave no code short of retail's window. Distance of same-code forms (first k with retail's order): the tag index
// through the existing 0x13e615 3 (KEPT: it replaces the hand-written getter in the three functions here, bytes
// unchanged); the stage states through the shared sampler_state_write 3 (KEPT, bytes unchanged); the colour products
// written in the colour call 3; the lookup through the existing 0x137550 with `inline` on its definition 3; the texture
// result, the scale or the shader-state pointer held in a named local 3 each (named values of this file are folded
// before the numbering). The two cached state writes by hand, as before round 281, 6 (window 6-7): each setter inlined
// at link time moves the window. LEAD, not keepable (0x14390's file is /Ob1, so it needs `inline` on that definition):
// the texture and dimension block as `function_14390(0, bitmap, 0.f)` 4 at 1109 B with the window at 2-6; the step is
// 0x14390's `long slot = stage` (with the slot written as literal zeros the window is back at 3, with the priority
// unused it stays at 2): a local copied from a constant argument of a function inlined at link time is folded only
// after the numbering. Two more such assignments would give retail's order; no existing function this body could call
// supplies them.
// M3 lane 6 r238 (H2M_FAST and plain; control 4): cause work on the reload group. The three values come back after the
// slow-path call in the order the allocator numbered them when it split their ranges there, lowest number first (ours
// x, y + size, the depth constant; retail has the constant's number between the other two), and that numbering follows
// the optimiser's own numbering of the body's expressions, which moves in steps of 32. Diagnostic stores ahead of the
// size (not source) flip the group once their weight reaches 5: a store to an array element weighs 2, a store to a
// scalar global 1 (also when nothing reads the global); three array stores give retail's order for 3 to 24 stores, ours
// again at 27 to 33, retail's at 36. Declarations ahead of the function (1 or 40), an unused helper and same-file
// inline helpers weigh nothing; each state setter inlined from another file weighs about 3 (the two state writes by hand
// turn the three-store probe back to ours). Source forms: 0x14390 moved into this file (same flags without /Ob1, so the
// link may expand it; retail has its body inline here) and called for the texture and dimension block 4 at 1109 B, fast
// and plain, but its other callers expand it too (0x144f0 79, 0x14560 119, 0x43a40 295, 0x254200 150, 0x2548f0 512),
// reverted; the lookup helper taking the bitmap index as 0x137550 does (index 0, range test, scaled element address) 4;
// the two readiness tests through a file-local inline with a real priority parameter 4; the texture and dimension block
// through a file-local inline taking (stage, bitmap, priority) with the stage copied to a slot local 4 (all the same
// code). Measured against the count with two probe stores (one unit short of the flip): the four vertex position calls
// in member form, the index-taking lookup helper, the real-parameter readiness wrapper and the primitive start in C form
// add nothing.
// M3 lane 7 r225 (H2M_FAST, control 4): both readiness tests written `!= NULL` 4; the chosen bitmap tested `!= NULL` 4;
// the draw-ready flag tested `!= 0` 4; all three 4; the two dimension locals declared width first 4 (all the same
// code). Reading of the back end (diagnostic): the slow-path block after the second and third reservations holds the
// three reloads, all at the same scheduling height, so the scheduler keeps their insertion order (x, y + size, depth);
// one forced decision, the y + size reload's height lowered by one, gives retail's order in the first of the two
// groups (4 -> 2). No source form here changes that height or the insertion order.
// Roasted-Codes #1524 (released 2026-11-26; fast reduced-image scores, control 4): the four corners as a counted
// loop with the per-corner expressions and calls unchanged 607 (766 B; the loop stays rolled and the top edge is
// recomputed each iteration); only the two top-edge corners as a counted loop, the two bottom submissions unchanged,
// 379 (1000 B; still rolled, the top moves ahead of its texture reservation and the function returns with ret 0x10
// for retail's ret 0x14); the permuter on the whole image, 113 tries, best 4.
// @retail 0x254490
void __stdcall function_254490(point2f const *arg_0, real arg_1, real arg_2, color3f const *arg_3, bool arg_4)
{
	s_bitmap_view *local_0 = function_254202(function_13e615(12));
	s_bitmap_view *local_1 = function_254202(function_13e615(13));
	if (function_12360(local_0, 0.f) && function_12360(local_1, 0.f))
	{
		s_bitmap_view *local_2 = arg_4 ? local_1 : local_0;
		if (local_2)
		{
			g_51f3c8[1][0] = function_12310(local_2, 0.f);
			long local_8 = *(short *)((byte *)local_2 + 6);
			long local_9 = *(short *)((byte *)local_2 + 4);
			g_485af4[0] = local_9;
			g_485b04[0] = local_8;
		}
		if (g_51ec9a)
		{
			real local_4 = arg_2 * 2.f;
			point2f local_3 = *arg_0;
			function_1cf50();
			function_1c710(g_51f0f0);
			function_0222d0(D3DRS_CULLMODE, 0);
			function_0222d0(D3DRS_ZBIAS, 0);
			D3DDevice::Begin(D3DPT_TRIANGLEFAN);
			function_254491(arg_1, arg_3);
			D3DDevice::SetVertexData2s(1, 0, 0);
			D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, local_3.x - local_4, local_3.y + local_4, 16777215.f, 16777215.f);
			D3DDevice::SetVertexData2s(1, 1, 0);
			D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, local_3.x + local_4, local_3.y + local_4, 16777215.f, 16777215.f);
			D3DDevice::SetVertexData2s(1, 1, 1);
			D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, local_3.x + local_4, local_3.y - local_4, 16777215.f, 16777215.f);
			D3DDevice::SetVertexData2s(1, 0, 1);
			D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, local_3.x - local_4, local_3.y - local_4, 16777215.f, 16777215.f);
			D3DDevice_End();
		}
	}
}

// score 2 (was 2): the texture-coordinate sums are computed at the first vertex and reused for
// the other three, and the colour is read through a pointer with the alpha as a byte; what is left
// is that after the second reservation retail reloads the lower coordinate before the 16777215
// constant (0x254490 has the same swap, in the same evaluation-order direction). The last quad's
// y sums now load y first, as retail does, through pointers to the point's two coordinates.
// The pool address of the constant is masked by the check; only the order of the two loads differs.
// Round 34 (lane A, machine 3): the y sums of the third and fourth vertices are folded by the
// commutative-operand canonicalisation of the whole-program build: swapping their operand order
// keeps 18, swapping the whole quad gives 91, and the constant through a named local keeps 18;
// what is left is the fold in those two vertices and the reload order after the reservation.
// after the second reservation retail reloads the lower coordinate before the 16777215 constant,
// whose pool address differs as well and that the check cannot mask.
// Round 83 on the round-220 near source (color pointer, per-use coordinates, byte alpha, scalar size;
// still 18 fast): size declared at function scope, const size, scale-first second texture products,
// size-first sum in the third vertex 18 each; retail loads y before the pushes in the third and fourth
// vertices (ours copies the size and folds y: 1 byte shorter each), and reloads the lower coordinate
// before the next vertex's 16777215 constant. All restored.
// keeps 0x13927e's result in EDI and reads it in the color call after Begin); the half-size texture
// coordinates (computed before the third reservation, retail after it) and the alpha byte remain.
// Round 60 lead: the result as a pointer read in the color call 349 alone (frame 0x10); with the
// half-size values written as expressions in the four texture-coordinate calls 28; plus a byte alpha
// 18 (2252 bytes; left: retail loads y and adds the offset in the offset quad's third and fourth
// vertices where ours copies the offset and folds the y load, and one reload order). Not kept without
// a match (computed values written at each use); same-length variants: swapped y operands 18, plain
// real offset 18, D3DDevice:: wrapper for the offset quad 18; a 600 s permuter run, none below 18.
// Earlier trials (scores at their time, frames 0x10/0x14):
// Fast control 246; final/entry center values 307/590; edge values 322; center with edges 301; signed alpha/C++
// frame 0x14 against 0x10; color copy and reservation homes remain.
// Round 137 lead (fast and plain 18 at retail length; not kept: it writes the texture
// coordinate sums out at each use): the getter result kept as a `color3f const *` read at the
// vertex call (retail keeps it in EDI across 0x1cf50 and Begin; frame 0x10), every texture
// coordinate written as `arg_1 * 0.5f + 0.5f` / `0.5f - arg_1 * 0.5f` in the four calls (only
// then does the product move after the first reservation, as in retail; a named half span 384,
// both locals after the first call 304, the lower or upper one named 309/20, assignments inside
// the call 372), and the alpha as a byte (`mov dl, al`). Left at 18: after the second
// coordinate call's reservation retail reloads the lower coordinate before the 16777215
// constant (ours the constant first), and `arg_0->y + size` in the last two vertices loads y
// first in retail (ours copies the size and folds y; operand order, a plain or if/else size
// local all 18). The color pointer alone 349 (the half-span block then computes ahead of the
// reservation), with the coordinate locals unchanged 349.
// Earlier: frame 0x14 versus 0x10; coordinate spills and reservation scheduling differ.
// Frame 0x14 versus 0x10; color/coordinate homes and reservation scheduling remain.
// Frame 0x14 versus 0x10 and reservation/coordinate homes remain. Fast control
// 246; final/entry center values 307/590; edge values 322; center with edges 301; signed alpha/C++
// color/both 246 each; alpha with edges 322; native color reference 276; color copies after
// Begin/commit 293/311.
// Native color value 246 fast / 237 plain; value half-size 276; replicated alpha 349;
// alpha after clear 265; second coordinate C entry 403; color with later alpha 252;
// memcpy color 246. Only the native color value is kept.
// Value half-size 342; disabled-path return 342; C begin/vertex calls 561;
// C texture-state calls 455; role-named texture coordinates 342.
// First-quad scope 342; texture expressions at calls 386; C++ color entry 342;
// complete descriptor value initialization 619; parallel alpha-byte packing 342.
// Coordinates before inlined begin 376; scale-parameter half-span 342; function-scope coordinates 342;
// shared resource pointer 511; dword alpha carrier 276.
// First coordinate C entry 373; folded half-span with inline reservations 276;
// delayed lower coordinate 387; delayed upper coordinate 384; lower initialized first 359.
// Shared color output 246; pointer/helper 276 (exact-only); consumer query/helper 305; byte alpha 166 fast / 157 plain; SDK packing 170; texture-mode-first stores 246 (fast control 246).
// Round 109: Fast: native color/coordinate names 246; color phase scope 246; final half-size value with native color 246; color after texture commit 248; immutable native color 246 (opening fast 246, plain 237).
// Result pointer 276; color query after Begin 263; function color lifetime/immutable alpha/
// chained descriptor counts/final short-coordinate API/half-scale product order/
// upper-coordinate term order 246/246/246/246/246/246 (opening fast 246, plain 237).
// Opening executable body and all inherited directives restored.
// Current native-color/byte-alpha frame: scoped color 166 fast; scalar half-size 166; disabled return 166; full
// readiness guards 166; lower coordinate first 378 (control 166). Pointer/byte controls 342 and value-half/pointer
// 342 reproduce older configurations and are excluded.
// Current byte-alpha frame: color after Begin 192; color after commit 168; C++ color entry 166; first position C++
// entry 166; second quad real-table loop 412 (fast control 166). Frame/color/reservation homes remain; complete
// opening body restored.
// Native memcpy in byte-alpha frame 166; complete component initializer 171; shared color helper with byte alpha 166;
// word alpha 162 fast / 153 plain (kept); explicit half-size assignments with word alpha 162 fast.
// Round 210: Signed-word alpha 162; coordinate lexical homes 162; immutable value half-size 162;
// native channel packing 238; chained descriptor counts 162 (fast control 162). Frame and
// reservation scheduling remain.
// Round 212: Immutable bitmap pointers/repeated texture coordinates 162/162; chained RGB/alpha outputs 165; shared color output with word alpha 162; center snapshot at first final vertex 296 (fast control 162). Frame and reservation scheduling remain.
// Round 157: Word-alpha frame: second/both expanded position device entries 162/162; first/second
// coordinate C entry 443/443; complete first-quad device entries 162; native color input reference
// alone/second position/color entry 349/349/349 (fast control 162, plain 153).
// Word-alpha context: color pointer/object reference 349/349 fast; immutable alpha/alpha
// lexical home 162/162; native texture pair/bounds 149/369 (unmatched grouped storage
// restored); bounds with color pointer/reference 373/373; immutable scalar coordinates 162;
// color scope 162; scoped lower-first coordinates 372; scoped immutable scalars 162. Reduced
// control 162, full baseline 153.
// Word-alpha pointer/shared-color helper 349; direct consumer query/helper 378; late query/helper 378;
// immutable query/helper 349; pointer/direct helper with scalar size 349/378;
// lower-first helper coordinates 372; symmetric role-named lower-first coordinates 372 (fast control 162).
// Complete first draw expiry at End/before End and coordinate-only expiry at either
// boundary 162f each; complete draw with alpha packing/program expiry 162f each;
// coordinate-vertex plus alpha packing and complete-vertex plus program expiry 162f each.
// Reduced control 162, full 153; original complete phase lifetimes and directives restored.
// RGB/BGR scalar channels 150/150; outer declarations 150/150; RGB after Begin/commit 379/152; BGR after Begin 378; BGR with value half-size 150 (fast control 162). Scalar channels recover frame 0x10 but are one-use field copies without a match, so all restored.
// Native query pointer with every nonempty public C++ color/first-two-position boundary set
// 349 each; with last-consumer pointer scope 349 (fast control 162). All restored.
// Round 204: first/second/both bitmap observations at each use 569/458/653; with value
// half-size 569/458/653; both observations with native color pointer alone/value size
// 684/684 (fast control 162, full 153). Complete opening body and directives restored.
// Round 159: Shared getter resource-result-first 517; bitmap failure returns 522; both null producers
// 516; ordinary inline resource/bitmap/both 162/162/162; both null producers with resource/bitmap inline
// 516/516 (fast control 162, full baseline 153). All getter bodies, metadata and target code restored.
// Round 224: Entry color/phase scalar/joint declaration ownership 162/162/162; later color lexical
// home and separate native assignment 162/162; pure half-span before color/commit/entry 405/403/527
// (fast control 162). Frame and reservation homes remain.
// Fast control 162: paired alpha with original/before-color/before-commit/entry half-span
// 251/394/392/462; staged packed alpha at the same sites 261/327/325/521.
// Round 215: First-quad real position/texture tables with counted first/last/both pairs
// 389/439/432; native end-bound walks 407/477/492; first/both counted pairs with
// actual color-query pointer 355/260 (fast control 162). Original complete body restored.
// Complete alpha product/plain byte accumulation/byte-index shifts 275/162/162; product/paired-word/
// byte accumulation with actual color pointer 349/265/349; product/paired-word with value half-size 275/251
// (fast control 162). Original native color, alpha packing and directives restored.
// Round 147: Real state-table address/filter/both/zero-tail/address-tail/filter-tail/all-
// three/complete setup 611/579/737/582/764/784/1030/520 (fast control 162).
// Round 149: Coherent color/alpha acquisition contracts 153/466/466/153/153/153/466/466; inline alpha
// loses four controls and is restored. On new coordinated contracts, RGB/BGR captures and their const
// forms 142 each, 2258 bytes, frame 0x10; all eight nonmatching one-use field forms rejected. Original
// body and providers restored.
// Round 174: existing native three-word color storage, immutable storage, each with shared output,
// library memcpy alone/with value half-size, native storage/value half-size and immutable/shared/value
// all 162 (fast control 162, full 153). Native-view forms require a match; original body restored.
// Bitmap-local name orders with alpha and coordinate names: eight combinations 162 (fast control 162).
// Round 220: Native color pointer, byte alpha and per-use coordinates with first/second/both
// texture C entries 369/375/467; second/both expanded position member entries 2/2 at retail length.
// Second texture/position boundaries 375; complete rectangle edges alone/with boundaries 161/446.
// On the two-difference graph: named half-span and both/lower/upper/no endpoints 342/363/363/386;
// scalar size/branched size/input role names 2/2/2; all named coordinates with scalar size 342.
// Member color/all positions/all texture calls/all first-quad entries 2 each. Scalar-size form
// plain confirms 2/2252, first +0x411: constant and lower-coordinate reloads reversed. Every
// per-use form is exact-only and restored; original native color, word alpha and coordinates retained.
// Pointer color, byte alpha and per-use coordinates: shared second-call lower value
// mutable/const 294/294; reused from that call 293/293; earlier declarations 294/293;
// native coordinate point aggregate/assigned 338/300. All restored.
// On that graph, uniform texture products/upper sums/both/alpha product, existing
// unit color helper alone/with products/with upper sums/all combined 2 each.
// One 600 s filtered permuter search, 39 tries: best 2; the two reloads remain.
// Every repeated-coordinate form restored; original color, alpha and locals retained.
// Round 197 on the two-difference SDK graph: scalar color input, immutable input, scale
// snapshot and all combinations, plus native input reference, 2 each; alpha word/short/long/
// dword/int/const-word/const-long/const-dword 28/28/112/112/112/28/112/112; lower/upper
// endpoints after the first position 293/330 and at the coordinate phase 363/363.
// No exact match; every per-use coordinate form restored with the original body.
// Round 81: the round-220 two-difference source (color pointer, per-use coordinates, byte alpha,
// scalar size, member entry for the second position) now builds 18 fast and plain: unchanged
// source, but the last two vertices' `y + size` again copy the size and fold y (other files'
// changes since; reverting the five changed units in its reduced image keeps 18). On it, member
// entries for both positions/color too/final quad, `0.5f * arg_1` products, `0.5f + ...` upper
// sums 18 each (fast). All restored.
// Round 7 (lane 10) on the two-difference graph (pointer color, byte alpha, per-use coordinates,
// member second position): C-form Begin 446 (2196 bytes); member color plus member first
// position 2; first lower coordinate with the product commuted 2; lower as a negated sum 2;
// half-span as a division by two 2. The two reloads after the second texture reservation stay
// swapped. All restored.
// Complete resource outcomes (match-only; fast control 162): ready bool 162; ready byte 162.
// ready short 162; ready long 162.
// unavailable bool 162; unavailable byte 162.
// unavailable short 162; unavailable long 162.
// Current color/alpha frame: half-span parameter 162; parameter/byte alpha 166.
// parameter/color pointer 349; parameter/value half-size 162.
// All executable forms restored.
// Lane 9 r29: the existing color helper `function_254491(1.f, color)` on the getter pointer 349
// (2260 bytes; color copy exact, half-span hoisted again); with per-use coordinates 28, plus byte
// alpha 18 at 2252 (lower-coordinate/constant reload order and the `y + size` fold remain); on it
// plain real size 18, `size + y` 18, member second position 18 (fast); permuter 51 tries, best 18.
// Round 12 (lane 10, fast control 162): parameter address taken (`(void)&arg_1;`) 162; the half
// size read once through the parameter's address 162 (the product still lands ahead of the third
// reservation); byte alpha 166; color pointer with the parameter address taken 349 (on the pointer
// body everything through the second reservation matches; ours then loads the size and computes the
// half span before the third reservation, retail after its call); color pointer with the lower
// coordinate first 372. All restored.
// Round 180: Final integral size selection long/dword/short/word 534/528/534/534; on the earlier
// pointer-color/byte-alpha/per-use reservation graph 548/542/548/548 (reduced control 162). All
// conversions and storage changes regress; original complete body and real size reference restored.
// Round 39 (m3 lane 12): 13 of the 18 come from the `y + size` fold (ours is 2 bytes shorter, so the
// three jumps to the else tail and the padding shift). A point pointer local for the last quad 18;
// the quad through a forceinline helper 348 (not inlined under the region's inline_depth(0)); the
// point parameter renamed to sort after the size 18; the size computed after 0x1cf50 26; the size's
// address taken 18. All restored.
// Round 134 (m3 lane 4): the size local named half_size 18; the size held in the scale parameter
// (diagnostic) 29 (the y fold stays, so it is not the size's symbol or slot); the size computed after
// 0x1cf50 26 (as before); the last quad's texture calls through D3DDevice:: 18; the size as a
// `real const &` 18. All restored.
// Round 54 (lane 10, fast): upper coordinate named at its first use 362 (2276 bytes); the depth constant as
// `(real)0xffffff` 18 (same code); the size ternary inverted 19; size-first x and y sums in the last two
// vertices 18; the size computed after the blip quad's Begin 85; the size defaulting to 32 and overridden
// for one view 90 (2256 bytes); the 3rd/4th y written `y - -size` (diagnostic) 64 (2263 bytes, a negation).
// 0x254490 shows the same reload swap: after a
// reservation call retail reloads the live xmm values in the order they were defined, ours in reverse.
// Round 51 (m3 lane 14, fast): a compile-only build of the file loads y first in all four vertices, as
// retail does; only our LTCG build copies the size and folds y. Code-neutral probes (1-7 unused locals
// at the top of the body, before the size local, or before a point pointer local's initialisation)
// move nothing: 18 each. The size as an if/else 18 (same code); computed before the third pass's
// memset 525 (2270 bytes) or at the top 563 (2282 bytes); a point pointer local for the last quad 18;
// a non-const point parameter (definition and the caller's declaration) 18.
// Round 162 (lane 4, fast; the plain check agrees on the kept form): pointers to the point's x and y for the last
// quad's four vertices 2 (kept; y alone 2, y and x in either declaration order 2, one pointer to both coordinates 18,
// a const reference to y 2, y's pointer at the size or at the block head 2). Before it: 10-64 unused locals 18, the size
// declared at the function or block top 18, y or x of the first vertex named 18, a const size 18. On the 2 form: named
// upper and lower 380, named half 408, half, upper and lower 380, lower alone 390, upper alone 423, constant-first sum
// or half 2, plain 2f calls 467, either or both 4f calls through the class 2, the scale through a pointer or reference
// 408, the scale in a local 2, the half as `arg_1 / 2.f` 2, the lower as `-arg_1 * 0.5f + 0.5f` 2, a named depth
// constant at the top or before Begin 2; a 600 s permuter run (53 tries), best 2.
// AC532: Actual parameter-home lower/upper endpoints with mutable/const scale 394/401/394/401;
// after first submission 335/391; complete half-span producers 363/368 (fast control 2).
// NOT kept: none matches; original parameter, observations and body restored.
// N369: Complete endpoint compound upper/lower/shared-half producer subsets 346 each;
// native two-coordinate compound output 343. NOT kept: no equality and extent grows;
// original two-reload body restored.
// K148: second-texture lower product commuted/division 2/2; second texture C entry 375;
// both coordinate references 2; point value copy 161; whole-object pointer 18; third-lower/
// fourth-upper products commuted 2/2. NOT kept: no equality; original two-reload body restored.
// Round 150 (m3 lane 6, fast): the reload order follows how many real values the body has, not their source order.
// Diagnostic extra code flips it to retail's: 3 pairs of float products or integer products ahead of Begin, 5 stores of
// different float constants, 8 of one constant, 12 integer copies (later in the body 12 stores); never integer immediates,
// dead locals, no-op inline helper calls or empty `do {} while (false)` blocks (16 of them). One or three extra reads of the
// lower coordinate or the constant do not flip it, so it is no reference weight. Every natural form 2, none flips: the state
// setter calls (kept, they were 0x222d0's body by hand), the existing color helper, member position entries, C Begin, a
// getter body with an index, all combinations; `inline` on the 0x137550 definition with direct calls 2 (retail inlines it
// here; its file is /Ob1) with its prologue unchanged. The direct call without that 577, and 0x254490 324 through the helper.
// Lead, the inline budget: with no inline-depth pragmas and the nine setter calls 440 (2388 bytes): ours keeps the 4f calls of
// vertices 3 and 4 and one later setter inline. One more setter ahead of Begin 441 (2287 bytes) then gives retail's whole call
// sequence but for 0x1392a9, which inlines: aligned 16, namely that setter, this reload pair and 0x1392a9. Two more 760 (1941 bytes)
// move every vertex out. Local helpers don't spend the budget (`inline` instead of forceinline on the two lookup helpers, the
// full getter body, the color helper, the inline 0x137550 getter: 440 each), so a consumer that emits no code is still missing.
// A 600 s permuter run on this form, 45 tries, best 2.
// Round 171 (lane 4, fast): the blend-state and texture-stage arguments by enum name 2 (same code, kept); the alpha
// constant through D3DCOLOR_ARGB 2 (same code, kept); the reference casts dropped from the first texture-stage calls 465
// (2236 bytes; 0x254200 loses its match); the bitmap getter as if/else 524; the render-target getter as a ternary 2.
// Round 78 (lane 10, fast): the alpha byte without the (long) step 2 (same code); the inline_depth pair around
// the second SRCBLEND/DESTBLEND setters dropped 436 (2262 bytes, the setters inline); the size as a 42 default
// overridden by `if (g_4ba04c > 1)` 94 (2258 bytes); the point pointers declared after the 0x1cf50 call 2 (same
// code); the guard as one condition with an `else if (g_51ec9a)` arm 2 (same code).
// Round 59 (m3 lane 2, fast): code-neutral forms alone and in every combination: the size as an if/else, the alpha
// byte without the (long) step, the alpha packed by hand instead of D3DCOLOR_ARGB, AB/AC/BC/ABC 2 each; the scale
// parameter renamed (two names) 2/2; the depth constants through D3DZ_MAX_D24S8 2; the alpha product as
// `255.f * ...` 2; the bitmap pointers declared const 2. The two reloads stay swapped.
// Round 83 (m3 lane 10, fast): bitmap, alpha and size locals named first_bitmap/second_bitmap/alpha/half_size 2;
// an early return on the clear flag in place of the outer test 2; the depth as a file-scope named real constant 2;
// the first quad's inline-depth pragma pair dropped 422 (2356 bytes); the bitmap locals typed as the stage-texture
// argument (casts moved to the 0x12360 tests) 2. Diagnostic: this function moved above 0x254490 in the file, both
// unchanged (2 and 4), so the reload order is not the source order of the file. The two reloads stay swapped.
// Round 75 (m3 lane 15, fast): the second quad's colour as a named `color3f const` aggregate 2; the quads' 64.f
// positions through one `real const size` 2; both 2; the alpha getter's result named before the product 2; the last
// quad's texture calls through `D3DDevice::SetVertexData2s` 2 (re-measure of round 134's form); every pixel-shader
// definition write and the commit through one `D3DPIXELSHADERDEF *shader` pointer 2 (all the same code: named
// constants fold before the reload order is set). The two reloads stay swapped. I found no other untried natural form;
// the forms above cover the rest.
// Round 96 (m3 lane 10, fast, with 0x254490 4 and 0x254200 matched as controls): the first quad's End through
// `D3DDevice::End()` 2; every Begin and End through the D3DDevice:: member forms 2; the globals lookup helper as a ternary 2
// (0x254490 4, 0x254200 still matched); the bitmap lookup helper with an early return 524 (2256 bytes; 0x254490 269,
// 0x254200 145); the globals lookup helper with a NULL default and no else 517 (0x254490 280, 0x254200 158). Restored.
// Round 88 (m3 lane 13, fast, single address, control 2). Diagnostic scan of the reload pair with neutral stores to a probe global:
// ahead of the first quad two or more stores of different float constants, two float copies, four stores of one float constant or
// four integer copies give retail's order (one float store, 2, 8 or 12 integer copies and 4 or 16 integer immediates do not); after
// the first quad 1-3 float stores do not; at the end of the body 6 do and 1-3 do not. So about four more float operations ahead of
// the pair flip it and integer code does so only at one count. 1-8 dead initialised locals (long or real) never move the pair (four
// of them give 18: the last quad's y fold comes back). Source forms, the pair unchanged in each: the first definition's two counts as
// one chained assignment 2; its texture modes and alpha outputs copied from the fields stored just before 2; both 2; the first quad's
// depth in a non-const local 2; its corners in four non-const locals 18 (four more symbols, the y fold again); corners and depth 2;
// a non-const zero priority local for the four bitmap calls 2; the scale parameter copied to a local 2; the half constant in a
// non-const local 332 (2260 bytes). All restored.
// Round 103 (m3 lane 10, fast, 0x254200 matched and 0x254490 4 throughout): the scale halved in place (`arg_1 *= 0.5f;`, then
// `arg_1 + 0.5f` / `0.5f - arg_1`) 386 (2272 bytes); the second quad's colour as a non-const colour aggregate declared at the top
// 2; the first quad's Begin and vertex calls through the IDirect3DDevice8_ wrappers with a NULL device (an extra inline level and an
// unused parameter, no code) 2; the first block's texture-stage calls through IDirect3DDevice8_SetTextureStageState 465 (2236 bytes,
// as with the reference casts dropped); the last quad through the IDirect3DDevice8_ wrappers 2. All restored.
// Round 164 (m3 lane 7, fast; 0x254200 matched, 0x254490 4): descriptive names for the scale, the point, the colour
// pointer, the two bitmaps, and all seven locals and parameters together 2 each (same code); the first quad (0x1cf50,
// Begin, the four vertices, End and both pragmas) in a forceinline helper taking the colour pointer and the scale 2 (same
// code), taking the colour and the lower and upper coordinates computed at the call 384 (2258 bytes). In this pair and in
// 0x254490's, retail emits the two reloads in ascending register order (xmm0 first), ours in descending order; the
// register each value gets already agrees.
// Round 248 (m3 lane 3, fast; 0x254200 matched, 0x254490 4 throughout): the first quad's texture bounds as one
// real_bounds aggregate {lower, upper} built after Begin 407 (2268 bytes), built before 0x1cf50 378 (2256 bytes);
// `real const half_size` at the success block's top 505 (2268 bytes); lower and upper named at the function's entry
// 257; the third and fourth texture calls through the D3DDevice:: member form like the first two 2 (same code). In this function
// and 0x254490 retail emits the post-call reload pair in ascending register order whichever values they hold (here
// xmm0 = lower first, there xmm0 = the depth constant first); ours emits xmm1 first in both.
// Round 168 (m3 lane 7, fast; 0x254200 matched, 0x254490 4 throughout): the colour getter after 0x1cf50 7; the alpha
// byte computed ahead of the texture-stage reset block 94 (2256 bytes), ahead of the third block's SRCBLEND/DESTBLEND
// pair 87; the second quad's colour as a const colour aggregate through the existing colour helper 254 (2273 bytes); the
// second block's SRCBLEND/DESTBLEND pair ahead of its 0x14390 call 12. Same code (2): the lower coordinate's product
// commuted in all four uses, the lower coordinate as `arg_1 * -0.5f + 0.5f`, the else arm as a separate test after an
// early return, the guard inverted with the success block in the else, the point pointers declared ahead of the size.
// Round 102 (m3 lane 14, single-address fast, control 2): the last quad's size `real const` 2; the last quad's size and
// point pointers after 0x1cf50 10; the colour through a `real const *` 2; every upper texture coordinate written
// constant first 2; the first two vertices' depth through the platform's 24-bit depth maximum macro 2 (same code).
// M3 lane 8 r177 (single-address fast, control 2; const references, which keep a value in memory): the first quad's lower
// texture coordinate bound by a `real const &` 363 (2278 B, computed ahead of 0x1cf50), the upper one 363 (2278 B), both
// 342 (2256 B); the colour as a `color3f const` value copied from the getter 392 (2238 B); the getter's result passed
// straight to the existing colour helper `function_254491(1.f, ...)` after Begin 31 (the getter moves after Begin; the
// reload pair unchanged).
// Round 194 (m3 lane 5, single-address fast, control 2): declaration-only moves to the function top (assigned in place)
// of the colour pointer, the alpha byte, the last quad's size and all three 2 each (same code: local symbol order does
// not enter the reload order); the first two texture calls in the free device-call form 467 (2157 B, out of line).
// Retail reloads the lower coordinate (defined at the first texture call) before the constant (first vertex), i.e. in
// the order the values are first computed, as in 0x254490; ours reverses both pairs.
// M3 lane 12 r139 (single-address fast, control 2): each SDK call of the body switched alone between the C and the
// class-member form (29 C sites to the member form 2 each, same code; the first quad's Begin and first two texture
// calls to the C form 446/369/375); the first block's eleven texture-stage calls through the existing wrapper 0x224f0
// 465 (2236 bytes); the colour bound by a `color3f const &` 2 (same code). Diagnostics: the first quad's inline-depth
// start moved one call later 2; an unreferenced float function added to the file leaves 2 and 0x254490's 4.
// Round 185 (m3 lane 8, fast with 0x254490 and 0x254200; control 2): the bitmap and stage calls' 0.f arguments as
// integer 0 2; every depth constant as a double literal 2; the first quad's four texture calls through a forceinline
// two-coordinate wrapper 263 (first difference +0x83: the texture-stage block inlines differently); every depth
// constant through a forceinline accessor returning it 340 (2197 bytes, same cause); the two lookup helpers plain
// `inline` in place of forceinline 2. The two reloads after the second texture reservation stay swapped.
// Round 264 (m3 lane 3, fast with 0x254490 4 and 0x254200 matched; control 2): the last quad's four edges named
// (left/right/top/bottom, each used twice, no point pointers) 164 (2179 bytes: the sums are shared); the two bitmap
// lookups moved inside the flag test 152 (first +0x0); the alpha as `long` 133; the texture tests as `!= NULL` 2 (same
// code); the alpha product named before the byte conversion 2 (same code); the colour's three components read into
// named `real const` locals ahead of 0x1cf50 377. The reload pair after the second texture reservation stays swapped.
// Round 195 (m3 lane 6, fast with 0x254490 4 and 0x254200 matched; control 2): the file's prototype and extern
// declarations in reverse order 2; every shader block's texture-mode store written ahead of the combiner count (as
// 0x254200) 2; the draw-ready flag defined as bool 2; the two lookup helpers defined after the colour helper 2; all
// four together 2 (all the same code). No matched caller of the reservation helper has a true two-value reload pair,
// so no matched function shows this build emitting retail's first-defined-first order.
// M3 lane 16 r142 (fast with 0x254490 4 and 0x254200 matched; control 2): the scale parameter declared `real const` 2;
// the inline-depth reset after the first quad's end call and the depth-0 start before the second block's blend-state setter pair
// dropped (function_14390 is a call either way) 2 (same code; kept, one pragma pair fewer); the closing `inline_depth(255)`
// dropped 2, as `inline_depth()` 2 (same code); the colour pointer declared `const` 2. In retail every call after the first
// quad's second vertex stays out of line except the border-colour stage call, the shape of an inline budget spent there.
// M3 lane 4 r226 (fast with 0x254490 4 and 0x254200 matched; control 2): the first quad's halving as `arg_1 / 2.f` 2
// (same code); the readiness test as two nested ifs 3 (first difference +0x95); the colour as a `real const *` indexed 2;
// the table getter called once into a named local for both lookups 545 (2233 bytes: retail calls it twice); the colour
// getter through End in their own block scope 2 (same code). All restored.
// M3 lane 11 r127 (fast with 0x254490 4 and 0x254200 matched; control 2): every depth constant of the first quad written
// `(real)0xffffff` 2; through a file-scope `static real const` 2 (both the same code); the depth-0 inline start moved one
// call earlier (before the second vertex) 356 (2190 bytes). Diagnostics, restored: an unused local holding the lower
// coordinate ahead of the colour fetch or of the first texture call 2/2 (same code: creating the lower value's common
// subexpression first does not reorder the pair); the first vertex's two depth constants made different 25 (the
// constant is then no longer carried into the third vertex, so the pair disappears). Retail's skip branch lands after
// both reloads (0x254d0c), so the pair is call-path compensation code; other functions with an adjacent reload swap
// (0x3a5c0, 0x185be0, 0x217c40, 0x4e260) show retail in both register orders, so it is not a fixed register order.
// M3 lane 13 r119 (fast with 0x254490 4 and 0x254200 matched; control 2): every depth constant written `(real)0xffffff`
// 2 (same code); file-local forceinline accessors returning the lower and upper texture coordinates from the scale, in
// all four texture calls 324 (2275 B), the lower one only 298, the upper one only 338, both in the first two calls only 2,
// the lower one in the first two calls only 2 (the accessors spend the inline budget once they reach the third call).
// Diagnostic: 1, 3, 5, 6 or 7 stores of one integer global to a probe array at the success block's top leave the pair
// in our order (aligned). Restored.
// M3 lane 12 r155 (fast with 0x254490 4 and 0x254200 matched; control 2; a compile-only build leaves the graphics
// calls out of line, so it cannot screen this pair): the first quad's two texture coordinates as `real const` upper and
// lower locals declared after the quad's start call 398 (2264 B), declared before 0x1cf50 382 (2258 B); the last quad's size as an
// if/else 2 (same code); the alpha converted straight to a byte 2 (same code); the point read as `arg_0->x`/`arg_0->y`
// at each use without the coordinate pointers 18 (the y fold returns). All restored.
// M3 lane 4 r231 (screened with 0x254490 4 and 0x254200 matched; control 2): the colour pointer read through a
// `color3f const *const &` binding 2 (same code); the colour components named red, green, blue after Begin 7 (first
// difference +0x2da); named blue, green, red (the order retail reads them) 2 (same code); that with a named `real alpha = 1.f`
// 466 (2159 bytes, first difference +0xe); the mip LOD bias stage value as a zero real read as a dword 443 (2271 bytes).
// All restored.
// M3 lane 6 r213 (fast with 0x254490 4 and 0x254200 matched; control 2): a file-local forceinline position helper (x, y)
// issuing the far-depth 4f call, used by the first quad's first two vertices 2, also used by 0x254490's four vertices 2
// (0x254490 8: its colour spills swap); a forceinline texture-coordinate helper (u, v) for the first two 2f calls 2; both
// helpers 2; the texture helper with the first two vertices' 4f calls in member form 2 (all the same code). Reading: a
// helper that evaluates the coordinates before the depth constant does not reverse the pair either, so the pair does not
// follow the order the values are first computed; the swapped reloads sit on the reservation call's edge only (the
// skip branch lands after them), and the unmatched 0x3a5c0, 0x185be0, 0x217c40 and 0x4e260 show the same kind of swap.
// M3 lane 7 r198 (single-address fast with 0x254490 4 and 0x254200 matched; control 2): the scale's half product named
// `real const half` just ahead of the first texture call and used in all four texture coordinates 386 (2272 B), named after
// the quad's start call, ahead of the colour call 423 (2286 B); the last quad's point read into one `point2f` copy in place of the two
// coordinate pointers 161 (2194 B); the readiness test held in a named bool 2 (same code). Diagnostic: every
// `#pragma inline_depth` line removed 440 (2388 B: every SDK call inlines), so retail's out-of-line calls after the
// second vertex need the pragmas in this source. All restored.
// M3 lane 3 r281 (fast, control 2), combinations of earlier same-code forms in the first quad: the colour and both
// vertex 4f calls in member form 2; colour member form, a const colour pointer and a block-local `real const` depth for
// the first two vertices 2; both vertices in member form with that depth 2; all five together 2; scale-first texture
// products with the colour member form and the depth 2. All restored.
// M3 lane 3 r283 (fast, control 2): the colour as a const reference bound to the getter's result 2 (same code); the
// first pixel-shader record cleared by value-initialisation instead of memset 576 (2287 B); the first quad's 64-unit
// extent as a named `real const` 2 (same code); both readiness tests written `!= NULL` 2 (same code); the depth pair
// through the platform header's 24-bit far-depth macro cast to real 2 (same code).
// M3 lane 3 r286 (single-address fast, control 2): the offset quad's half size as an if/else assignment 2 (same
// code); defaulted to 42 and overridden in an `if` 94 (2258 B); the closing `else if (g_51ec9a)` as a plain `else` 22
// (retail re-reads the flag); the alpha product scale first 2 (same code); the offset quad reading arg_0's fields
// directly instead of the two coordinate pointers 18. All restored.
// M3 lane 1 r144 (single-address fast, control 2): the first quad's depth constants through a `real const &` to a named
// value 2, every quad's 2 (both the same pair: the reference that reverses 0x254490's colour reloads does not reach this
// pair); the lower texture coordinate written `-(arg_1 * 0.5f) + 0.5f` 2; the upper written constant first 2; the first
// quad's reference with the upper constant first 2 (all the same code). All restored.
// M3 lane 14 r135 (fast, control 2): the bitmap lookup helper as nested ifs 2 (same code); the two 0x14bc0 calls without
// the (short) cast 2 (same code); a 600 s permuter run (60 tries), best 2; the last quad's corners through a forceinline
// (u, v, x, y) helper 162 (2176 B, first +0x83); the texture-stage values by enumerator name 2 (same code).
// M3 lane 5 r225 (single-address fast, control 2): the first quad's depth and w as two named reals (`w = depth`) 2; its
// zero coordinates through one named real 2; its w written as the depth times `1.f` 2 (all the same code); the colour
// call's alpha through a named `1.f` real 462 (2161 B, first +0xe); the second texture call's second lower coordinate
// written `-(arg_1 * 0.5f - 0.5f)` 369 (2303 B).
// M3 lane 8 r219 (fast, control 2; 0x254200 matched, 0x254490 4): the first quad's second texture-coordinate call in C form 375
// (2192 B), its first texture-coordinate call in C form 369 (2209 B), its Begin in C form 446 (2196 B); file-level forms measured
// with 0x254490's trials: the system headers first 2, the view getter as a ternary 2, the colour helper moved 2, the lookup index
// as a word cast 527. Retail reloads xmm0 (the lower coordinate) before xmm1 (the depth); ours always descending. All restored.
// M3 lane 11 r147 (fast, control 2): 0-40 unused initialised locals at the top 2, except at n = 4 mod 8 18 (the last
// quad's y + size fold returns: a period-8 state for the fold, none for the reload pair); a dead forceinline helper with
// 1-4 named locals ahead of the colour fetch, of the first quad's 0x1cf50 call or of the second texture call 2 each (12
// runs); diagnostic, the depth as a non-const file-scope real 369 (2276 B).
// M3 lane 5 r231 (fast, control 2): every first-quad `arg_1 * 0.5f` written `0.5f * arg_1` 2; the first quad's depth as a
// `real const` named ahead of the primitive-start call 2; the colour pointer a const pointer 2 (all the same code).
// Diagnostics: the first vertex's depth 0.f 74; the first vertex's two texture coordinates swapped 27, where the group then
// loads the constant into xmm0 first and the coordinate into xmm1: ours keeps the constant first whichever registers the
// two values get. All restored.
// M3 lane 15 r130 (single-address H2M_FAST, control 2): true `__assume(... != NULL)` on the point parameter at the top or
// ahead of the coordinate pointers, on the colour getter's result, and both 2 each; the offset quad's half size or the
// two coordinate pointers declared bare at the top 2 / 2; the coordinate pointers declared y first 2; the minus edges
// written `x + -size` 2 (all the same code).
// M3 lane 1 r153 (single-address H2M_FAST, control 2): the first quad (colour fetch to its end call) in a scope block
// of its own 2; the point parameter a const pointer 2 (same code each). Diagnostics: 0x254490's body emptied 2
// (0x254200 MATCH); the colour call after the first texture call 359 (first +0xe); the first region's inline depth 1
// instead of 0 419 (2365 B, the later calls inline).
// M3 lane 6 r229 (H2M_FAST with 0x254490, control 2): the pixel-shader clears and commits through the program-words view
// that the matched 0x254200 uses 2 (same code); the colour read as a `real const *` triple 2 (same code); both together 2;
// the first quad's first two texture coordinates through one `point2f` updated in place 393 (2286 B); its first two
// depth pairs from a `real const depth[2]` 18. Reading: 0xc2d00's note records the same kind of slow-path reload tie
// flipping with other files' contents.
// M3 lane 7 r216 (single-address H2M_FAST with 0x254490 4 and 0x254200 matched; control 2): the depth through an
// `extern real const` defined in the file 2, used in both functions 2 (folded); the four lower texture coordinates of
// the first quad through a file-local forceinline (scale) helper 298 (2264 B), lower and upper through two helpers 324
// (2275 B); both first-quad texture calls in C form 467 (2157 B); the first vertex's position call alone in member form 2
// (same code). Diagnostic, restored: each processor target on the file 2. Reading: in this pair and in 0x254490's triple,
// retail's call-edge reloads follow the order the values are first defined (0x254490: x, the depth, y + size; here the
// lower coordinate, then the depth), ours descend by register; neither named values nor a later-folded constant move it.
// M3 lane 5 r236 (single-address H2M_FAST, control 2): the first quad's depth pair after the first vertex written as a
// double literal 2; the first vertex's position as integer zeros 2; the second texture call's lower value written
// `0.5f - 0.5f * arg_1` 2; the second vertex's position call through the device-interface inline (NULL device) 2; the
// second texture call through it 2 (all the same code: the lower value / depth reload pair stays ours).
// M3 lane 7 r217 (H2M_FAST), at 2: the first quad's calls through the platform's device-interface inline that takes a
// device pointer: the four position calls 2, the texture calls 2, both 2, Begin and the colour call 2, all of them 2. In
// both this pair and 0x254490's, retail's two loads after the reservation are ascending by xmm number and ours descending.
// M3 lane 7 r219 (single-address H2M_FAST, control 2): the colour call's alpha `1.0f` 2; the colour fetched after the
// 0x1cf50 call 7; the first quad's second position as integer literals 2; the first quad's closing call in member form 2;
// the first texture-stage helper call given an integer zero 2; the last quad's size test written `1 < g_4ba04c` 2 (all the
// same code); the alpha byte through real_truncate 30 (first +0x83).
// M3 lane 7 r222 (single-address H2M_FAST, control 2): the ruler of 0x254490's note, k probe stores ahead of the
// colour fetch: ours at k = 0-1, retail's order for every k from 2 to 8. Here only real values count: two real copies,
// or one real store and one real copy, give retail's order; two integer copies, or one real store with an integer copy
// or an integer immediate store, do not. So the body is two real values short ahead of the pair. Same code and no step
// (still ours with one probe store): the tag index through the existing 0x13e615 (KEPT), the stage states through the
// shared sampler_state_write (KEPT), the first quad's colour through the file's colour helper with a unit scale, the
// lower coordinate as `arg_1 * -0.5f + 0.5f`, both coordinates as the corner formula `(1.f - 0.5f) * arg_1 + 0.5f` /
// `(0.f - 0.5f) * arg_1 + 0.5f`.
// M3 lane 6 r238 (H2M_FAST, control 2): the pair is the same effect as 0x254490's group (see its r238 lines): after the
// slow-path call the values come back in the allocator's numbering, ours with the depth constant numbered below the lower
// coordinate, retail above it. Forms: 0x14390 moved into this file (so the link may expand it) 512 (2289 B: both calls
// expand), reverted; the lookup helper taking the bitmap index as 0x137550 does 2; the readiness tests through a
// file-local inline with a real priority parameter 2; 0x254490's texture block through a file-local inline 2 (same code
// each); the three pixel-shader record clears through one file-local inline 126 (2252 B, first +0x83: the inline depth
// limits then fall differently); the first texture and dimension block written out as 0x254490 has it, in place of the
// 0x14390 call, 512 (2289 B); the scale's half named ahead of the 0x1cf50 call for the first two texture calls 436
// (2316 B).
// M3 lane 7 r225 (H2M_FAST, control 2): the first pixel-shader clear as a value-initialised aggregate assignment 576
// (2287 B); the first 0x14390 call's priority as an integer zero 2; the first texture call's upper coordinate written
// `0.5f + arg_1 * 0.5f` 2; the colour pointer itself const 2; the offset quad's half size as an if/else assignment 2
// (same code each). Diagnostic reading: the swapped pair sits in a slow-path block after a reservation where both loads
// have equal scheduling height (as 0x254490's group); a 20-decision what-if search found no single forced decision
// that fixes it.
// M3 lane 13 r155 (H2M_FAST with 0x254490 4 and 0x254200 matched; control 2). Reading: both reloads are pieces of
// values split at the slow-path call, and the pieces come back in the order their parents were numbered. On this body
// the depth constant's shared load is numbered ahead of the three coordinate values (product, upper, lower), so its
// piece reloads first; with two real stores to a probe array ahead of the colour fetch (diagnostic) the constant is
// numbered after them and the pair is retail's (one store: ours; two or three: retail's). By-reference temporaries
// weigh nothing: a file-local copy of the stage-state helper taking stage, type and value all by const reference 2,
// type and value 2, stage and type 2 (same code each); taking only the stage or only the value by reference 465
// (2236 B: the dirty-flag updates merge, as with the type by value). The first block's eleven stage states with a
// `dword stage = 0` local through the existing 0x224f0 465 (2236 B), through the device's inline setter 465, with a
// `short stage` shared with the 0x14390 call 465 / 465 (a local stage is folded before the updates are merged), the
// stage local with the by-reference helper 2 (same code); the eleven types read from a file-scope constant table
// through the inline setter or through 0x224f0 465 / 465 (a constant table element is folded as early as a literal).
// M3 lane 16 r189 (single-address H2M_FAST, control 2): the first quad's colour copied by value into a `color3f` 392 (2238 B,
// the frame grows), as a `color3f const` value 392, bound to a `color3f const &` 2 (same code); the colour read through the
// getter's own dword view with each component read as a real 2; one `real const` zero shared by both readiness tests and
// both 0x14390 calls 2 (same code each).
// M3 lane 3 r309 (H2M_FAST with 0x254490 4 and 0x254200 MATCH; control 2): the float-valued texture stage states
// written as float bit patterns (a named real read through a dword view), as the platform's interface takes them: the
// first block's LOD bias 443 (2271 B, first +0x83: the real takes a frame home), the second block's 160 (2263 B), the
// first block's 0x46000000 value as 8192.f 463 (2265 B); the first block's LOD bias as `(dword)0.f` 2 (same code: the
// conversion folds before the reload numbering); the colour fetched ahead of the first pixel-shader clear 292 (2260 B).
// The two reloads stay swapped.
// M3 lane 4 r262 (H2M_FAST, control 2). Position scan of the two-real-copy ruler (diagnostic stores to a probe array,
// removed): retail's order at every one of 13 places tried, from the top of the success block to the last quad's end
// (after the pair too), and ours with a single copy; so the body is two real values short anywhere in the function,
// not ahead of the pair. The last quad's centre copied by value into a `point2f` (two real copies) gives retail's order
// but 161 (2194 B, frame 0x14: the copy keeps a home); the centre as two named reals 122 (2222 B, pair unchanged); the
// fields read in place with no pointers 18 (pair unchanged); a `point2f const &` with the two pointers 2; the alpha
// product written scale-first 2; the alpha named ahead of the product 2; the colour through a `real const *` to its
// first component 2; the size local const 2 (same code each).
// @retail 0x2548f0
void __stdcall function_2548f0(point2f const *arg_0, real arg_1)
{
	s_bitmap_view *local_0 = function_254202(function_13e615(7));
	s_bitmap_view *local_1 = function_254202(function_13e615(8));
	if (g_51ec9a)
	{
		if (function_12360(local_0, 0.f) && function_12360(local_1, 0.f))
		{
			function_0222d0(D3DRS_CULLMODE, 0x901);
			function_0222d0(D3DRS_COLORWRITEENABLE, 0x10101);
			function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
			function_0222d0(D3DRS_BLENDOP, 0x8006);
			function_0222d0(D3DRS_ALPHATESTENABLE, 0);
			function_0222d0(D3DRS_ZFUNC, 0x207);
			function_0222d0(D3DRS_ZBIAS, 0);
			function_14390(0, (s_type_7ba8e9 *)local_0, 0.f);
			sampler_state_write(0, D3DTSS_ADDRESSU, 4);
			sampler_state_write(0, D3DTSS_ADDRESSV, 4);
			sampler_state_write(0, D3DTSS_BORDERCOLOR, 0x46000000);
			sampler_state_write(0, D3DTSS_MAGFILTER, 2);
			sampler_state_write(0, D3DTSS_MINFILTER, 2);
			sampler_state_write(0, D3DTSS_MIPFILTER, 1);
			sampler_state_write(0, D3DTSS_MAXANISOTROPY, 0);
			sampler_state_write(0, D3DTSS_MIPMAPLODBIAS, 0);
			sampler_state_write(0, D3DTSS_MAXMIPLEVEL, 0);
			sampler_state_write(0, D3DTSS_COLORSIGN, 0);
			sampler_state_write(0, D3DTSS_ALPHAKILL, 0);
			function_0222d0(D3DRS_SRCBLEND, 1);
			function_0222d0(D3DRS_DESTBLEND, 0x302);
			function_1cc30(0xc);
			function_1c710(g_51f0f0);
			memset(&g_484f68, 0, sizeof(g_484f68));
			g_484f68.PSCombinerCount = 1;
			g_484f68.PSTextureModes = 1;
			g_484f68.PSRGBInputs[0] = 0x8040000;
			g_484f68.PSRGBOutputs[0] = 0xc0;
			g_484f68.PSAlphaInputs[0] = 0x18140000;
			g_484f68.PSAlphaOutputs[0] = 0xc0;
			g_484f68.PSFinalCombinerInputsABCD = 0xc;
			g_484f68.PSFinalCombinerInputsEFG = 0x1c00;
			function_15180(&g_484f68);
			color3f const *color = (color3f const *)function_13927e(g_4b9ed8);
			function_1cf50();
			D3DDevice::Begin(D3DPT_TRIANGLEFAN);
			D3DDevice_SetVertexData4f(5, color->red, color->green, color->blue, 1.f);
			D3DDevice::SetVertexData2f(1, arg_1 * 0.5f + 0.5f, 0.5f - arg_1 * 0.5f);
			D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, 0.f, 0.f, 16777215.f, 16777215.f);
			D3DDevice::SetVertexData2f(1, 0.5f - arg_1 * 0.5f, 0.5f - arg_1 * 0.5f);
			D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, 64.f, 0.f, 16777215.f, 16777215.f);
			#pragma inline_depth(0)
			D3DDevice_SetVertexData2f(1, 0.5f - arg_1 * 0.5f, arg_1 * 0.5f + 0.5f);
			D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, 64.f, 64.f, 16777215.f, 16777215.f);
			D3DDevice_SetVertexData2f(1, arg_1 * 0.5f + 0.5f, arg_1 * 0.5f + 0.5f);
			D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, 0.f, 64.f, 16777215.f, 16777215.f);
			D3DDevice_End();
			function_14390(0, (s_type_7ba8e9 *)local_1, 0.f);
			function_0222d0(D3DRS_SRCBLEND, 0);
			function_0222d0(D3DRS_DESTBLEND, 0x302);
#pragma inline_depth()
			memset(&g_484f68, 0, sizeof(g_484f68));
			g_484f68.PSCombinerCount = 1;
			g_484f68.PSTextureModes = 1;
			g_484f68.PSFinalCombinerInputsABCD = 8;
			g_484f68.PSFinalCombinerInputsEFG = 0x1800;
			function_15180(&g_484f68);
			function_1cf50();
			#pragma inline_depth(0)
			D3DDevice_Begin(D3DPT_TRIANGLEFAN);
			D3DDevice_SetVertexData4f(5, 0.4f, 0.8f, 0.4f, 1.f);
			D3DDevice_SetVertexData2f(1, 1.f, 0.f);
			D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, 0.f, 0.f, 16777215.f, 16777215.f);
			D3DDevice_SetVertexData2f(1, 0.f, 0.f);
			D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, 64.f, 0.f, 16777215.f, 16777215.f);
			D3DDevice_SetVertexData2f(1, 0.f, 1.f);
			D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, 64.f, 64.f, 16777215.f, 16777215.f);
			D3DDevice_SetVertexData2f(1, 1.f, 1.f);
			D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, 0.f, 64.f, 16777215.f, 16777215.f);
			D3DDevice_End();
			function_14bc0((short)g_4858b8, 0, true);
			function_14f60(0, 0x1d);
			function_0224f0(0, D3DTSS_ADDRESSU, 3);
			function_0224f0(0, D3DTSS_ADDRESSV, 3);
			function_0224f0(0, D3DTSS_MAGFILTER, 2);
			function_0224f0(0, D3DTSS_MINFILTER, 2);
			function_0224f0(0, D3DTSS_MIPFILTER, 1);
			#pragma inline_depth()
			D3DDevice_SetTextureStageState(0, D3DTSS_BORDERCOLOR, 0);
			#pragma inline_depth(0)
			function_0224f0(0, D3DTSS_MAXANISOTROPY, 0);
			function_0224f0(0, D3DTSS_MIPMAPLODBIAS, 0);
			function_0224f0(0, D3DTSS_MAXMIPLEVEL, 0);
			function_0224f0(0, D3DTSS_COLORSIGN, 0);
			function_0224f0(0, D3DTSS_ALPHAKILL, 0);
			function_0222d0(D3DRS_SRCBLEND, 1);
			function_0222d0(D3DRS_DESTBLEND, 0x303);
			byte local_6 = (byte)(long)(function_1392a9(g_4b9ed8) * 255.f);
			memset(&g_484f68, 0, sizeof(g_484f68));
			g_484f68.PSConstant0[0] = D3DCOLOR_ARGB(local_6, local_6, local_6, local_6);
			g_484f68.PSCombinerCount = 1;
			g_484f68.PSTextureModes = 1;
			g_484f68.PSRGBInputs[0] = 0x8010000;
			g_484f68.PSRGBOutputs[0] = 0x80;
			g_484f68.PSAlphaInputs[0] = 0x18110000;
			g_484f68.PSAlphaOutputs[0] = 0x80;
			g_484f68.PSFinalCombinerConstant0 = 0xff00ff00;
			g_484f68.PSFinalCombinerInputsABCD = 8;
			g_484f68.PSFinalCombinerInputsEFG = 0x1800;
			function_15180(&g_484f68);
			real local_7 = g_4ba04c > 1 ? 32.f : 42.f;
			real const *point_x = &arg_0->x;
			real const *point_y = &arg_0->y;
			function_1cf50();
			D3DDevice_Begin(D3DPT_TRIANGLEFAN);
			D3DDevice_SetVertexData2s(1, 0, 0);
			D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, *point_x - local_7, *point_y - local_7, 16777215.f, 16777215.f);
			D3DDevice_SetVertexData2s(1, 1, 0);
			D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, *point_x + local_7, *point_y - local_7, 16777215.f, 16777215.f);
			D3DDevice_SetVertexData2s(1, 1, 1);
			D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, *point_x + local_7, *point_y + local_7, 16777215.f, 16777215.f);
			D3DDevice_SetVertexData2s(1, 0, 1);
			D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, *point_x - local_7, *point_y + local_7, 16777215.f, 16777215.f);
			D3DDevice_End();
			#pragma inline_depth(255)
		}
		else if (g_51ec9a)
		{
			function_14bc0((short)g_4858b8, 0, true);
		}
	}
}
