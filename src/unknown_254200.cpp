// @flags /O2 /arch:SSE /Gr
#include "shader_storage.h"
#include "view_context_storage.h"
#include "unknown_11c920.h"
#include "globals.h"
#include <xtl.h>
#include <string.h>
#include "unknown_13927e.h"

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

PRIVATE __forceinline byte *function_254201(void)
{
	byte *local_0 = (byte *)g_4e034c;
	byte *local_1;
	if (*(long *)(local_0 + 0x110))
	{
		local_1 = *(byte **)(local_0 + 0x114);
	}
	else
	{
		local_1 = NULL;
	}
	return local_1;
}

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
    s_bitmap_view *first = function_254202(*(long *)(function_254201() + 0x64));
    s_bitmap_view *second = function_254202(*(long *)(function_254201() + 0x6c));
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
        D3DDevice::SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE const &)D3DTSS_ADDRESSU, 4);
        D3DDevice::SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE const &)D3DTSS_ADDRESSV, 4);
        D3DDevice::SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE const &)D3DTSS_MAGFILTER, 2);
        D3DDevice::SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE const &)D3DTSS_MINFILTER, 2);
        D3DDevice::SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE const &)D3DTSS_MIPFILTER, 1);
        D3DDevice::SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE const &)D3DTSS_MAXANISOTROPY, 0);
        D3DDevice::SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE const &)D3DTSS_MIPMAPLODBIAS, 0);
        D3DDevice::SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE const &)D3DTSS_MAXMIPLEVEL, 0);
        D3DDevice::SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE const &)D3DTSS_COLORSIGN, 0);
        D3DDevice::SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE const &)D3DTSS_ALPHAKILL, 0);
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
// @retail 0x254490
void __stdcall function_254490(point2f const *arg_0, real arg_1, real arg_2, color3f const *arg_3, bool arg_4)
{
	s_bitmap_view *local_0 = function_254202(*(long *)(function_254201() + 0x64));
	s_bitmap_view *local_1 = function_254202(*(long *)(function_254201() + 0x6c));
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
			g_4b8448 = 0;
			D3DDevice_SetRenderState(D3DRS_CULLMODE, 0);
			g_4b8450 = 0;
			D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
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

// score 18 (was 18): the texture-coordinate sums are computed at the first vertex and reused for
// the other three, and the colour is read through a pointer with the alpha as a byte; what is left
// is that in the last quad's third and fourth vertices retail loads y into a register before it
// adds the offset, where ours copies the offset and folds the load into the addition, and that
// after the second reservation retail reloads the lower coordinate before the 16777215 constant.
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
// @retail 0x2548f0
void __stdcall function_2548f0(point2f const *arg_0, real arg_1)
{
	s_bitmap_view *local_0 = function_254202(*(long *)(function_254201() + 0x3c));
	s_bitmap_view *local_1 = function_254202(*(long *)(function_254201() + 0x44));
	if (g_51ec9a)
	{
		if (function_12360(local_0, 0.f) && function_12360(local_1, 0.f))
		{
			g_4b8448 = 0x901;
			D3DDevice_SetRenderState(D3DRS_CULLMODE, 0x901);
			g_4b8308 = 0x10101;
			D3DDevice_SetRenderState(D3DRS_COLORWRITEENABLE, 0x10101);
			g_4b82e8 = 1;
			D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
			g_4b8324 = 0x8006;
			D3DDevice_SetRenderState(D3DRS_BLENDOP, 0x8006);
			g_4b82ec = 0;
			D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, 0);
			g_4b82e0 = 0x207;
			D3DDevice_SetRenderState(D3DRS_ZFUNC, 0x207);
			g_4b8450 = 0;
			D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
			function_14390(0, (s_type_7ba8e9 *)local_0, 0.f);
			D3DDevice::SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE const &)D3DTSS_ADDRESSU, 4);
			D3DDevice::SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE const &)D3DTSS_ADDRESSV, 4);
			D3DDevice::SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE const &)D3DTSS_BORDERCOLOR, 0x46000000);
			D3DDevice::SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE const &)D3DTSS_MAGFILTER, 2);
			D3DDevice::SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE const &)D3DTSS_MINFILTER, 2);
			D3DDevice::SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE const &)D3DTSS_MIPFILTER, 1);
			D3DDevice::SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE const &)D3DTSS_MAXANISOTROPY, 0);
			D3DDevice::SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE const &)D3DTSS_MIPMAPLODBIAS, 0);
			D3DDevice::SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE const &)D3DTSS_MAXMIPLEVEL, 0);
			D3DDevice::SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE const &)D3DTSS_COLORSIGN, 0);
			D3DDevice::SetTextureStageState(0, (D3DTEXTURESTAGESTATETYPE const &)D3DTSS_ALPHAKILL, 0);
			g_4b82f4 = 1;
			D3DDevice_SetRenderState(D3DRS_SRCBLEND, 1);
			g_4b82f8 = 0x302;
			D3DDevice_SetRenderState(D3DRS_DESTBLEND, 0x302);
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
			#pragma inline_depth()
			function_14390(0, (s_type_7ba8e9 *)local_1, 0.f);
#pragma inline_depth(0)
			function_0222d0((D3DRENDERSTATETYPE)0x3e, 0);
			function_0222d0((D3DRENDERSTATETYPE)0x3f, 0x302);
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
			function_0224f0(0, (D3DTEXTURESTAGESTATETYPE)0, 3);
			function_0224f0(0, (D3DTEXTURESTAGESTATETYPE)1, 3);
			function_0224f0(0, (D3DTEXTURESTAGESTATETYPE)3, 2);
			function_0224f0(0, (D3DTEXTURESTAGESTATETYPE)4, 2);
			function_0224f0(0, (D3DTEXTURESTAGESTATETYPE)5, 1);
			#pragma inline_depth()
			D3DDevice_SetTextureStageState(0, D3DTSS_BORDERCOLOR, 0);
			#pragma inline_depth(0)
			function_0224f0(0, D3DTSS_MAXANISOTROPY, 0);
			function_0224f0(0, D3DTSS_MIPMAPLODBIAS, 0);
			function_0224f0(0, D3DTSS_MAXMIPLEVEL, 0);
			function_0224f0(0, D3DTSS_COLORSIGN, 0);
			function_0224f0(0, D3DTSS_ALPHAKILL, 0);
			function_0222d0((D3DRENDERSTATETYPE)0x3e, 1);
			function_0222d0((D3DRENDERSTATETYPE)0x3f, 0x303);
			byte local_6 = (byte)(long)(function_1392a9(g_4b9ed8) * 255.f);
			memset(&g_484f68, 0, sizeof(g_484f68));
			g_484f68.PSConstant0[0] = (((((dword)local_6 << 8) | local_6) << 8 | local_6) << 8) | local_6;
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
			function_1cf50();
			D3DDevice_Begin(D3DPT_TRIANGLEFAN);
			D3DDevice_SetVertexData2s(1, 0, 0);
			D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, arg_0->x - local_7, arg_0->y - local_7, 16777215.f, 16777215.f);
			D3DDevice_SetVertexData2s(1, 1, 0);
			D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, arg_0->x + local_7, arg_0->y - local_7, 16777215.f, 16777215.f);
			D3DDevice_SetVertexData2s(1, 1, 1);
			D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, arg_0->x + local_7, arg_0->y + local_7, 16777215.f, 16777215.f);
			D3DDevice_SetVertexData2s(1, 0, 1);
			D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, arg_0->x - local_7, arg_0->y + local_7, 16777215.f, 16777215.f);
			D3DDevice_End();
			#pragma inline_depth(255)
		}
		else if (g_51ec9a)
		{
			function_14bc0((short)g_4858b8, 0, true);
		}
	}
}
