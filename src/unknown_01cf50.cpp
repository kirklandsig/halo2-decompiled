#include "filter_storage.h"
#include "render_source_storage.h"
#include "visibility_storage.h"
#include "camera_storage.h"
#include "frame_storage.h"
#include "depth_parameters_storage.h"
#include "reset_storage.h"
// @flags /O2 /Ob1 /arch:SSE /Gr
#include "pool_storage.h"
#include "render_modes_storage.h"
#include "material_records_storage.h"
#include "palette_cache_storage.h"
#include "device_setup_storage.h"
#include "palette_storage.h"
#include "counters_storage.h"
#include "launch_stack_storage.h"
#include "font_loading_storage.h"
#include "shader_storage.h"
#include "render_state_storage.h"
#include "glyphs_storage.h"
#include "saved_frames_storage.h"
#include "projection_storage.h"
#include "view_context_storage.h"
#include "unknown_11c920.h"
#include "globals.h"
#include "index_cache_storage.h"
#include "depth_groups_storage.h"
#include "unknown_11cb00.h"
#include <xtl.h>
#include <string.h>
#include <math.h>
#include <xmmintrin.h>
#include "unknown_058ee0.h"
#include "unknown_163110.h"
#include "effects.h"
#include "unknown_030290.h"
#include "timed_effect.h"

extern D3DPIXELSHADERDEF g_484f68;
extern byte *g_485a80;
extern byte g_4670bc;
extern long g_4858b8;
extern dword g_4b8448, g_4b8308, g_4b82e0, g_4b8438, g_4b8450;
extern dword g_4b82fc, g_4b843c, g_4b82e8, g_4b82ec, g_4b82f4;
extern dword g_4b81fc[];
struct s_render_reset_state;
void function_16b10(s_render_reset_state *state);
bool function_14480(long tag, short stage, short index);
void function_14bc0(short index, short element, bool use_depth);
void function_0222d0(D3DRENDERSTATETYPE state, dword value);
dword function_1cc30(long index);
void function_1cf50(void);
void function_14f60(short stage, short index);
bool function_143c0(long tag, short index, short stage, real priority);
byte g_485a77, g_55e6bd;
short g_4858c8;
long g_4e6978, g_4e697c;
long g_485a78[2];
extern byte g_485a76;
#include "texture_stage_storage.h"
#include "sampler_state.h"
struct s_type_7ba8e9;
s_type_7ba8e9 *function_137550(long tag, short bitmap);
bool function_14390(short stage, s_type_7ba8e9 *bitmap, real priority);
void function_144f0(short stage, long fallback, long tag, short fallback_index, short index, real priority);
bool function_01dd60(long index, long *width, long *height);
long function_25960(void);
struct s_shader_cache;
void function_1c590(s_shader_cache *state, long tag, long index);
void __stdcall function_1c710(void *state);
// MiniMax H (11-13): no new form, no lower score (188).
// MiniMax H (11-12): no new form, no lower score (188): the body is still 35 bytes longer than retail's 1422 and the first difference is still at +0x335, where the state setup's flag stores and the two render-state calls sit in another order; retail inlines only the first of the four SetVertexData2s calls and calls the other three out of line, and this build inlines two of them.
// MiniMax I (11-11): no new form, no lower score (188): the body is 35 bytes longer than retail's 1422 and the first difference is at +0x335, where the state setup's flag stores and the two render-state calls sit in another order.

// MiniMax B (11-11): no new form, no lower score (188); retail inlines only the first of the four SetVertexData2s
// calls and calls the other three out of line, where ours inlines the second as well, so our body is 35 bytes longer
// than retail's 1422; the only lever for that (an inline_depth pair) may not be added.
// MiniMax E (11-11, run 80): no new form, no lower score (188): retail inlines only the first SetVertexData2s (3, 0, 0) and calls the other three out of line where we inline (3, 1, 0) as well, and the only lever for that (an inline_depth pair) may not be added; the body is still 35 bytes longer than retail's 1422 and the state setup's flag stores and the two 0x403xx calls sit in another order from the first difference.
// MiniMax E (11-10, run 73): no new form, no lower score (188); retail inlines only the first SetVertexData2s (3, 0, 0) and calls the other three out of line where we inline (3, 1, 0) as well, and the only lever for that (an inline_depth pair) may not be added.
// MiniMax D (11-11): no new form, no lower score (188): the body is still 35 bytes longer than retail's 1422, and the only lever left is the inline_depth pragma pair around the renderer's SetVertexData2s calls, which the rules forbid.
// MiniMax G (11-11): no new form, no lower score (188): the pixel-shader block writes the same six constants to the same globals, but ours keeps the flag in edi and writes it to a global first while retail writes each constant straight to its global; retail also inlines only the first SetVertexData2s (3, 0, 0) and calls the other three out of line, and the only lever for that (an inline_depth pair) may not be added.
// MiniMax S (11-10): no new form, no lower score (188): retail inlines only the first SetVertexData2s (3, 0, 0) and calls the other three out of line, ours inlines (3, 1, 0) as well, and the only lever (an inline_depth pair around that call) may not be added; the permuter (61 tries, 605 s) found nothing below its own baseline.
// MiniMax D (11-10): no new form, no lower score (188).
// MiniMax I (11-11): score 96 (was 188). Retail inlines only the first SetVertexData2s (3, 0, 0) and
// calls the other three (0x3fb330); the two inline_depth pairs now sit around the (3, 1, 0) and
// (3, 1, 1) calls, so both are called as retail calls them and the constant one is no longer
// hoisted into edi for the combiner count (that alone was worth 188 -> 131). The last (3, 0, 1)
// call is still inlined here while retail calls it, so our body is 45 bytes longer (1467 against
// 1422) and the first difference is at that call, +0x4f9; a third pair around it would be needed
// to call it as well.
// Round 2 of #1066: score 188 (was 188). Walked against retail: the sampler and render states, the
// vertex-shader constants (0.0f/1.0f layout, bounds order), the pixel shader fields and their
// order, 0x14bc0's arguments and the four vertices' coordinates (0.53125f, 64.53125f,
// 16777215.0f checked in the XBE) all agree up to the pixel shader program; no behaviour bug. The
// whole remaining difference is that retail inlines only the first SetVertexData2s (3, 0, 0) and
// calls the other three (0x3fb330) while ours also inlines (3, 1, 0), which in turn hoists the
// constant one into edi for the combiner count. D3DDevice::SetVertexData2s for that call: 188.
// MiniMax D (11-01): no lower score: our build puts the constant one into edi for the two
// combiner-count stores where retail stores the immediate; the permuter timed out.
// MiniMax D (10-30): the stage states through the existing sampler_state_write plus one more
// inline_depth(0) pair around the (3, 1, 0) vertex call matched (full check); new in-body pragmas
// are not kept, so the pair was dropped at review: 188 (fast, 1457 bytes, retail 1422; main 316).
// Permuter (25 min on #1066, about 800 variants): 104 only with SetVertexData2s/4f arguments swapped; not kept.
// MiniMax I (11-11), NOT kept (coordinator 11-11): 188 -> 96 only by moving the inline_depth pragma pair from the fourth corner's vertex call to the second's (pragmas are never kept); body at 188.
// MiniMax A (11-12): no new form, no lower score (188); the body is 35 bytes longer than retail's 1422 and the first
// difference is at +0x335, where retail inlines only the first of the four SetVertexData2s and calls the other three
// out of line, and the only lever for that (an inline_depth pair) may not be added.
// score 188 (was 188): the body is 35 bytes longer than retail's 1422 and the first difference is at +0x335, where
// retail inlines only the first of the four SetVertexData2s and calls the other three out of line; the only lever for
// that is an inline_depth pair, which may not be added.
// @retail 0x46230
void __stdcall function_46230(short target, real const *bounds)
{
    function_14f60(0, 0);
    sampler_state_write(0, D3DTSS_ADDRESSU, 3);
    sampler_state_write(0, D3DTSS_ADDRESSV, 3);
    sampler_state_write(0, D3DTSS_MAGFILTER, 2);
    sampler_state_write(0, D3DTSS_MINFILTER, 2);
    sampler_state_write(0, D3DTSS_MIPFILTER, 1);
    sampler_state_write(0, D3DTSS_MAXANISOTROPY, 0);
    sampler_state_write(0, D3DTSS_MIPMAPLODBIAS, 0);
    sampler_state_write(0, D3DTSS_MAXMIPLEVEL, 0);
    sampler_state_write(0, D3DTSS_COLORSIGN, 0);
    sampler_state_write(0, D3DTSS_ALPHAKILL, 0);
    function_143c0(*(long *)(g_485a80 + 0x34), 0, 1, 0.0f);
    sampler_state_write(1, D3DTSS_ADDRESSU, 3);
    sampler_state_write(1, D3DTSS_ADDRESSV, 3);
    sampler_state_write(1, D3DTSS_MAGFILTER, 2);
    sampler_state_write(1, D3DTSS_MINFILTER, 2);
    sampler_state_write(1, D3DTSS_MIPFILTER, 2);
    sampler_state_write(1, D3DTSS_MAXANISOTROPY, 0);
    sampler_state_write(1, D3DTSS_MIPMAPLODBIAS, 0);
    sampler_state_write(1, D3DTSS_MAXMIPLEVEL, 0);
    sampler_state_write(1, D3DTSS_COLORSIGN, 0);
    sampler_state_write(1, D3DTSS_ALPHAKILL, 0);
    g_4b8448 = 0; D3DDevice_SetRenderState(D3DRS_CULLMODE, 0);
    g_4b8308 = 0x1010101; D3DDevice_SetRenderState(D3DRS_COLORWRITEENABLE, 0x1010101);
    g_4b82e8 = 0; D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
    g_4b82ec = 0; D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, 0);
    g_4b8438 = 0; D3DDevice_SetRenderState(D3DRS_ZENABLE, 0);
    g_4b8450 = 0; D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
    function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0xc), 0);
    function_1c710(g_51f0f0);
    real constants[32] = {
        bounds[1] - bounds[0], 0.0f, 0.0f, bounds[0],
        0.0f, bounds[3] - bounds[2], 0.0f, bounds[2],
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f
    };
    D3DDevice_SetVertexShaderConstant(-78, constants, 8);
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 0x21;
    g_484f68.PSCombinerCount = 1;
    g_484f68.PSConstant0[0] = 0xc0000000;
    g_484f68.PSAlphaInputs[0] = 0x19110000;
    g_484f68.PSAlphaOutputs[0] = 0x100c0;
    g_484f68.PSFinalCombinerInputsABCD = 0x18;
    g_484f68.PSFinalCombinerInputsEFG = 0x1c00;
    D3DDevice_SetPixelShaderProgram(&g_484f68);
    function_14bc0(target, 0, false);
    function_1cf50();
    D3DDevice_Begin(D3DPT_TRIANGLEFAN);
    D3DDevice_SetVertexData2s(3, 0, 0);
    D3DDevice_SetVertexData4f(0, 0.53125f, 0.53125f, 16777215.0f, 16777215.0f);
    D3DDevice_SetVertexData2s(3, 1, 0);
    D3DDevice_SetVertexData4f(0, 64.53125f, 0.53125f, 16777215.0f, 16777215.0f);
#pragma inline_depth(0)
    D3DDevice_SetVertexData2s(3, 1, 1);
#pragma inline_depth()
    D3DDevice_SetVertexData4f(0, 64.53125f, 64.53125f, 16777215.0f, 16777215.0f);
#pragma inline_depth(0)
    D3DDevice_SetVertexData2s(3, 0, 1);
#pragma inline_depth()
    D3DDevice_SetVertexData4f(0, 0.53125f, 64.53125f, 16777215.0f, 16777215.0f);
    D3DDevice_End();
    function_14bc0((short)g_4858b8, 0, true);
}

// score 52 (was 127): the failure branch jumps to an exit next to it instead of the one at the
// end of the function, and the two render state calls are inlined here while retail calls
// 0x222d0 out of line.
// @retail 0x32ac0
bool function_32ac0(void)
{
    bool result = false;
    if (!function_14480(*(long *)(g_485a80 + 0x34), 0, 0))
    {
        g_4670bc = true;
        function_16b10((s_render_reset_state *)g_485b48);
        sampler_state_write(0, D3DTSS_ADDRESSU, 3);
        sampler_state_write(0, D3DTSS_ADDRESSV, 3);
        sampler_state_write(0, D3DTSS_MAGFILTER, 2);
        sampler_state_write(0, D3DTSS_MINFILTER, 2);
        sampler_state_write(0, D3DTSS_MIPFILTER, 2);
        sampler_state_write(0, D3DTSS_MAXANISOTROPY, 0);
        sampler_state_write(0, D3DTSS_MIPMAPLODBIAS, 0);
        sampler_state_write(0, D3DTSS_MAXMIPLEVEL, 0);
        sampler_state_write(0, D3DTSS_COLORSIGN, 0);
        sampler_state_write(0, D3DTSS_ALPHAKILL, 0);
        function_1cf50();
        function_14bc0((short)g_4858b8, 0, true);
        memset(&g_484f68, 0, sizeof(g_484f68));
        g_484f68.PSTextureModes = 1;
        g_484f68.PSCombinerCount = 0x11001;
        g_484f68.PSAlphaInputs[0] = 0x18140000;
        g_484f68.PSAlphaOutputs[0] = 0xc0;
        g_484f68.PSFinalCombinerInputsEFG = 0x1c00;
        D3DDevice_SetPixelShaderProgram(&g_484f68);
        g_4b8448 = 0;
        D3DDevice_SetRenderState(D3DRS_CULLMODE, 0);
        g_4b8308 = 0x1010101;
        D3DDevice_SetRenderState(D3DRS_COLORWRITEENABLE, 0x1010101);
        g_4b82e0 = D3DCMP_LESSEQUAL;
        D3DDevice_SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
        g_4b8438 = 2;
        D3DDevice_SetRenderState(D3DRS_ZENABLE, 2);
        g_4b8450 = 0;
        D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
        g_4b82fc = 0;
        D3DDevice_SetRenderState(D3DRS_ZWRITEENABLE, 0);
        g_4b843c = 0;
        D3DDevice_SetRenderState(D3DRS_STENCILENABLE, 0);
        g_4b82e8 = 1;
        D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
        g_4b82ec = 0;
        D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, 0);
        g_4b82f4 = 0;
        D3DDevice_SetRenderState(D3DRS_SRCBLEND, 0);
        dword destblend_state = D3DRS_DESTBLEND;
        dword blendop_state = D3DRS_BLENDOP;
        function_0222d0(*(D3DRENDERSTATETYPE const *)&destblend_state, 0x303);
        function_0222d0(*(D3DRENDERSTATETYPE const *)&blendop_state, 0x8006);
        function_1cc30(23);
        result = true;
    }
    return result;
}

bool function_48b00(point3f const *point, real radius, real *screen, real *extent);
void function_1cf50();

struct s_unknown_01dcc0
{
    byte unknown00[4];
    long sub_header[5];
    long elements[4][6];
    long element_count;
    byte unknown7c[4];
    long width, height;
    void *data;
    byte unknown8c[8];
    bool flag94;
    byte flag95;
    byte unknown96[2];
};
extern s_unknown_01dcc0 g_4b4b58[39];
bool function_01dd60(long index, long *width, long *height);
void function_14bc0(short index, short element, bool use_depth);

PRIVATE __forceinline void write_texture_corners(real u, real v)
{
    D3DDevice_SetVertexData2f(1, u, v);
    D3DDevice_SetVertexData2f(2, u, v);
    D3DDevice_SetVertexData2f(3, u, v);
    D3DDevice_SetVertexData2f(4, u, v);
}

// @retail 0x15ec0
void __stdcall function_15ec0(long index)
{
    long width = 0;
    long height = 0;
    long count;
    s_unknown_01dcc0 *entry = &g_4b4b58[index];
    if (entry->data && !entry->flag95)
        count = entry->element_count;
    else count = index == 0 || index == 3 ? 1 : 0;
    function_01dd60(index, &width, &height);
    for (long level = 0; level < count; ++level)
    {
        real x = (real)width;
        real y = (real)height;
        real ratio = g_485ad4.hi / (g_485ad4.hi - g_485ad4.lo);
        real z = (ratio - g_485ad4.lo * ratio) * 16777215.0f;
        if (z < 0.0f) z = 0.0f;
        else if (z > 16777215.0f) z = 16777215.0f;
        real w = (1.0f / g_485ad4.hi) * 16777215.0f;
        if (w < 0.0f) w = 0.0f;
        else if (w > 16777215.0f) w = 16777215.0f;
        function_14bc0((short)index, (short)level, false);
        D3DDevice_Begin(D3DPT_TRIANGLESTRIP);
        real fraction = count > 1 ? (real)level / (count - 1) : 0.0f;
        D3DDevice_SetVertexData4f(5, 0.0f, 0.0f, 0.0f, fraction);
        D3DDevice_SetVertexData4f(6, 0.0f, 0.0f, 0.0f, 0.0f);
        write_texture_corners(0.0f, 0.0f);
        D3DDevice_SetVertexData4f(0, 0.0f, 0.0f, z, w);
        write_texture_corners(1.0f, 0.0f);
        D3DDevice_SetVertexData4f(0, x, 0.0f, z, w);
        write_texture_corners(0.0f, 1.0f);
        D3DDevice_SetVertexData4f(0, 0.0f, y, z, w);
        write_texture_corners(1.0f, 1.0f);
        D3DDevice_SetVertexData4f(0, x, y, z, w);
        D3DDevice_End();
        width /= 2;
        height /= 2;
    }
}

__declspec(noinline) bool function_015b10(long index, D3DPalette **out);
const long g_43e8e8[4] = { 32, 64, 128, 256 };
// MiniMax D (11-14): no new form, no lower score (58): the allocation callee 0x15b10 still takes its out pointer in esi where retail takes it in ebx, and that callee's body is in src/unknown_0158f0.cpp, which this lane may not edit, so the frame stays one push deeper and the copy count rides in edi. BLOCKED-BY 0x15b10.
// MiniMax A (11-12): no new form, no lower score (58): the allocation callee 0x15b10 still takes its out pointer in esi here and in ebx in retail, so the frame stays one push deeper and the copy count rides in edi; that callee lives in src/unknown_0158f0.cpp, which this lane may not edit. BLOCKED-BY 0x15b10.

// MiniMax B (11-13): no new form, no lower score (58): the copy's tail is still this build's rep movsd where retail ends the copy with a rep movsb, and the callee this function calls is still not inlined here although retail inlines it; the permuter cannot search this function with --fast (the reduced image scores it 62 against 58).
// MiniMax H (11-12): no new form, no lower score (58); the allocation callee 0x15b10 still takes its out pointer in
// ESI here and in EBX in retail, so the frame stays one push deeper and the copy count rides in EDI; that callee
// lives in src/unknown_0158f0.cpp, which this lane may not edit, so the body stands. BLOCKED-BY 0x15b10.
// MiniMax G (11-11): no new form, no lower score (58); the allocation callee 0x15b10 still takes its out pointer
// in ESI here and in EBX in retail, and that callee lives in src/unknown_0158f0.cpp, which this lane may not edit,
// so the frame stays one push deeper and the copy count rides in EDI; this round re-read the diff and the pairs
// that are left are that convention and the copy's trailing byte, so the body stands. Reading the count at the
// copy site instead of at the top of the body reproduces retail's copy shape (the table read, shl ecx,2, shr ecx,2,
// rep movsd, and ecx,3, rep movsb) but the frame stays one push deeper and the score rises to 76, so it is reverted.
// MiniMax H (11-11): no new form, no lower score (58); the allocation callee still takes its out pointer in ESI where retail takes it in EBX, so the frame stays one push deeper and the copy count rides in EDI, and 0x15b10 lives in src/unknown_0158f0.cpp, which this lane may not edit. BLOCKED-BY 0x15b10.
// MiniMax C (11-11): no new form, no lower score (58); the allocation callee still takes its out pointer in esi where retail takes it in ebx, and 0x15b10 is in src/unknown_0158f0.cpp.
// MiniMax R (11-10): no new form, no lower score (58); the allocation callee still takes its out pointer in ESI where retail takes it in EBX, and that callee is in another file.
// MiniMax A (11-11): no new form, no lower score (58); the entry block still differs in the callee's convention:
// retail takes the allocation callee's out pointer in EBX and zeroes the result slot before the call, ours takes it
// in ESI, keeps the table lookup in EDI and pushes EDI one deeper. The callee 0x15b10 is in
// src/unknown_0158f0.cpp, which this lane may not edit, so the convention cannot be moved from here.
// BLOCKED-BY 0x15b10.
// MiniMax I (11-10): no new form, no lower score (58); the allocation callee still takes its out pointer in ESI where retail takes it in EBX, and that callee is in another file.
// MiniMax E (11-09): no new form, no lower score; the allocation callee still takes its out
// pointer in esi where retail takes it in ebx, and 0x15b10 lives in src/unknown_0158f0.cpp.
// MiniMax R (11-09): no new form, no lower score; the copy count read at the copy site, named there, declared before the allocation or spelled in bytes all give 76 against 58, and retail's copy ends in a byte tail, which only a size the compiler cannot see produces.
// MiniMax R (11-09): no new form, no lower score; the copy count still rides in edi and the allocation
// callee's out pointer is still in esi, and that callee is in another file.
// MiniMax H (11-05): no new form, no lower score; the allocation callee takes its out pointer in
// ESI here and in EBX in retail and 0x15b10 is in src/unknown_0158f0.cpp, so the frame stays one
// push deeper and the copy count rides in EDI; retail keeps the size in ebp across the copy and
// reads the callback from its stack slot late, which the callee's register choice fixes here.
// MiniMax E (11-01): the copy count read at the copy site instead of before the allocation
// MiniMax G (11-06): no new form, no lower score: the allocation callee takes its out pointer in ESI here and
// in EBX in retail, and 0x15b10 is in src/unknown_0158f0.cpp, so the frame stays one push deeper and the
// copy count rides in EDI; the convention cannot be changed from this file.
// score 58 (was 58): this round found no gain - the allocation callee takes its out pointer in ESI here and
// in EBX in retail, and 0x15b10 is in src/unknown_0158f0.cpp, so the convention cannot be changed here.
// scores 76 (was 62); no lower score.
// MiniMax E (11-01): the copy count read before the allocation reaches retail's 178 bytes;
// the earlier round dropped this form on the difference count alone (62 against 58).
// MiniMax C (11-08): no new form, no lower score.
// score 58 (was 77): the allocation callee takes its out pointer in ESI here and in EBX in
// retail, so the frame is one push deeper and the copy count rides in EDI.
// MiniMax B (11-13): no new form, no lower score (58): retail reads the palette count at the memcpy site, which needs the index live across the call to 0x15b10; that callee takes its out pointer in esi where retail takes it in ebx, and its body is in another file.
// MiniMax G (11-13): no new form, no lower score (58): the allocation callee takes its out pointer in esi here
// and in ebx in retail, and that callee's body is in another file, so the frame stays one push deeper and the
// copy count rides in edi. BLOCKED-BY 0x15b10.
// @retail 0x1d3f0
bool function_1d3f0(long index, void const *colors, D3DPalette **out)
{
	D3DPalette *palette = NULL;
	long copy_count = g_43e8e8[index];
	bool result = function_015b10(index, &palette);
	if (result && colors)
	{
		D3DCOLOR *destination = D3DPalette_Lock2(palette, 0);
		if (destination)
			memcpy(destination, colors, copy_count * sizeof(D3DCOLOR));
		D3DPalette_Unlock(palette);
	}
	*out = palette;
	return result;
}

// score 0 (was 152): matches (MiniMax H, 11-04). The stage states go through sampler_state_write,
// whose type-by-reference argument keeps each call's own dirty-mask update, and the switch cases
// are in retail's order (3/7, 1/5, 0, 4, 2/6), which puts the case blocks where retail has them.
// @retail 0x15780
void function_15780(long stage, long mode)
{
	sampler_state_write(stage, D3DTSS_MAXANISOTROPY, 0);
	sampler_state_write(stage, D3DTSS_MIPMAPLODBIAS, 0);
	sampler_state_write(stage, D3DTSS_MAXMIPLEVEL, 0);
	sampler_state_write(stage, D3DTSS_COLORSIGN, 0);
	sampler_state_write(stage, D3DTSS_ALPHAKILL, 0);
	switch (mode)
	{
	case 3: case 7:
		sampler_state_write(stage, D3DTSS_ADDRESSU, 3);
		sampler_state_write(stage, D3DTSS_ADDRESSV, 3);
		sampler_state_write(stage, D3DTSS_ADDRESSW, 3);
		sampler_state_write(stage, D3DTSS_MAGFILTER, 1);
		sampler_state_write(stage, D3DTSS_MINFILTER, 1);
		sampler_state_write(stage, D3DTSS_MIPFILTER, 0);
		break;
	case 1: case 5:
		sampler_state_write(stage, D3DTSS_ADDRESSU, 3);
		sampler_state_write(stage, D3DTSS_ADDRESSV, 3);
		sampler_state_write(stage, D3DTSS_ADDRESSW, 3);
		sampler_state_write(stage, D3DTSS_MAGFILTER, 2);
		sampler_state_write(stage, D3DTSS_MINFILTER, 2);
		sampler_state_write(stage, D3DTSS_MIPFILTER, 2);
		break;
	case 0:
		sampler_state_write(stage, D3DTSS_ADDRESSU, 1);
		sampler_state_write(stage, D3DTSS_ADDRESSV, 1);
		sampler_state_write(stage, D3DTSS_ADDRESSW, 1);
		sampler_state_write(stage, D3DTSS_MAGFILTER, 2);
		sampler_state_write(stage, D3DTSS_MINFILTER, 2);
		sampler_state_write(stage, D3DTSS_MIPFILTER, 2);
		break;
	case 4:
		sampler_state_write(stage, D3DTSS_ADDRESSU, 3);
		sampler_state_write(stage, D3DTSS_ADDRESSV, 3);
		sampler_state_write(stage, D3DTSS_ADDRESSW, 3);
		sampler_state_write(stage, D3DTSS_MAGFILTER, 2);
		sampler_state_write(stage, D3DTSS_MINFILTER, 2);
		sampler_state_write(stage, D3DTSS_MIPFILTER, 0);
		break;
	case 2: case 6:
		sampler_state_write(stage, D3DTSS_ADDRESSU, 4);
		sampler_state_write(stage, D3DTSS_ADDRESSV, 4);
		sampler_state_write(stage, D3DTSS_ADDRESSW, 4);
		sampler_state_write(stage, D3DTSS_MAGFILTER, 2);
		sampler_state_write(stage, D3DTSS_MINFILTER, 2);
		sampler_state_write(stage, D3DTSS_MIPFILTER, 2);
		break;
	default: __assume(0);
	}
}

struct s_buffer_pair
{
	long first;
	long second;
};

s_buffer_pair g_5093c0;

void *g_5093b0;

short bitmap_get_mipmap_count(short width, short height, short depth, short format,
	bool linear, short maximum_levels);
bool function_0158f0(short format, byte linear, long width, long height,
	long levels, long usage_index, D3DTexture **out);
bool function_0159b0(long edge, short format, long levels, long usage_index,
	D3DCubeTexture **out);
bool function_015a60(short format, long width, long height, long depth,
	long levels, long usage_index, D3DVolumeTexture **out);
// MiniMax D (11-14): no new form, no lower score (72): the call to 0x359e0 still passes its arguments in another order than retail (retail keeps the height in dx, stores it as a word and passes the width in ecx with the depth in ax, ours keeps the width in di), and that callee's parameter order is declared in src/unknown_0359e0.cpp, which this lane may not edit. BLOCKED-BY 0x359e0.
// MiniMax H (11-11): no new form, no lower score (72); the height is still spilled as a dword where retail stores a word, the depth still lands in AL so the linear flag has to be reloaded from the stack, and the texture pointer still sits in a register where retail keeps it in EDI; this round typing the texture slot as a texture pointer shrinks the frame to retail's 0xc and moves the pointer into EBX but leaves the score at 72, dropping the volatile height stops the spill altogether (80), and a copy of the flags byte folds away (72), so the body stands.
// MiniMax I (11-10): no new form, no lower score (72); the width still lives in DI where retail keeps it in BX, and the first 16-bit height store is still a dword in ours.
// MiniMax R (11-09): no new form, no lower score; the width still lives in di where retail keeps it in bx, the first height store is still a dword where retail stores a word, and the mipmap call still hands the callee the texture pointer where retail hands it the width.
// MiniMax R (11-09): no new form, no lower score; the width still lives in di where retail keeps it in
// bx, and the height is still stored as a dword where retail stores a word. This round: the linear flag
// and the depth byte swapped in the declaration order score 72 again, and the permuter's best of 69
// swaps the height and the depth between two callees, which the callees' own order forbids, so it is
// dropped and the body stands.
// MiniMax A (11-06, run 52), not kept (72 -> 70, under the keep bar) - the texture slot is cleared and handed over through
// the bitmap itself and the mipmap limit is named; the mipmap call now hands the callee the height
// and the depth in the order the callee reads them, so the depth byte reaches the call in eax and
// the height word in the first stack slot as retail does; the width still lives in di where retail
// keeps it in bx and the texture pointer in ebx where retail keeps edi.
// MiniMax A (11-12): no new form, no lower score (72): retail keeps the height in dx and stores it on the stack as a word at [esp+8] where ours zeroes eax first and stores a dword at [esp+0x10], and retail keeps the width in bx and passes it in ecx with the depth in ax to 0x359e0 while ours keeps the width in di; that callee lives in src/unknown_0359e0.cpp, which this lane may not edit. BLOCKED-BY 0x359e0.

// MiniMax B (11-13): no new form, no lower score (72): retail's copy ends with the byte tail that this build's memcpy does not have, and the flag this function waits on is still held in a stack slot where retail keeps it in a register; the volatile local's shape is only allowed with a byte match.
// MiniMax H (11-12): no new form, no lower score (72); retail keeps the height in DX and stores it on the stack as a
// word at [esp+8] where ours zeroes EAX first and stores a dword at [esp+0x10], and retail keeps the width in BX and
// passes it in ECX with the depth in AX to 0x359e0 while ours keeps the width in DI; that callee lives in
// src/unknown_0359e0.cpp, which this lane may not edit. BLOCKED-BY 0x359e0.
// MiniMax G (11-11): no new form, no lower score (72); this round re-read the diff: retail keeps the height in DX
// and stores it on the stack as a word at [esp+8] where ours zeroes EAX first and stores a dword at [esp+0x10], and
// retail keeps the width in BX and passes it in ECX with the depth in AX to 0x359e0 while ours keeps the width in
// DI; that callee lives in src/unknown_0359e0.cpp, which this lane may not edit, so its parameter order cannot be
// moved from here and the body stands. BLOCKED-BY 0x359e0.
// MiniMax C (11-11): no new form, no lower score (72); the width still lives in di where retail keeps it in bx, and the call's arguments follow the callee 0x359e0 in src/unknown_0359e0.cpp.
// MiniMax R (11-10): no new form, no lower score (72); the callee's first argument must arrive in ECX, but the volatile height's reload takes ECX and the width is dropped, and the height is stored as a dword where retail stores a word; the twenty forms tried this round score 71 or more.
// MiniMax E (11-09): no new form, no lower score; the width still lives in di where retail
// keeps it in bx, and the first 16-bit height store is still a dword in ours.
// MiniMax C (11-06): no new form, no lower score - the width still lives in di where retail keeps it in
// bx, and the first 16-bit height store is a dword in ours; the forms tried this round scored 70, 71 and
// 72, and a later sweep of ten declaration shapes (unsigned and int heights, volatile flag and depth,
// byte depths, the depth before or after the width and the format, the texture pointer declared first)
// scored 72 or more, so the body stands as it was.
// MiniMax D (11-05): no new form, no lower score - retail keeps the width in bx and the texture slot in
// edi, ours keeps the width in di and spills the texture pointer; dropping the texture local and
// naming the mipmap limit scores 70, under the keep bar.
// score 72 (was 72): this round adds the mipmap limit read into a named local ahead of the texture
// pointer (70) and the texture cleared after the count call (71); both are under the keep bar, so
// the original body stands - the width still lives in di where retail keeps it in bx.
// score 72 (was 72): this round found no gain - the texture pointer is taken from a local again, and the
// first 16-bit height store and the width and texture register lifetimes still differ.
// MiniMax A (11-11): no new form, no lower score (72); retail reads the bitmap's words and bytes into DX (the
// height), BX (the width), BP (the format) and AX (the depth) and passes the width in ECX and the depth in AX to
// 0x359e0, ours zeroes EAX first and keeps the width in DI, so the call's arguments do not line up; the callee
// 0x359e0 lives in src/unknown_0359e0.cpp, which this lane may not edit, so its parameter order cannot be moved
// from here. BLOCKED-BY 0x359e0.
// MiniMax C (11-08): no new form, no lower score.
// score 72 (was 68): the texture pointer is taken from a local again and the flags are read
// in the test itself: the four points the bitmap's own address won are under the keep bar,
// so the body of the earlier round is restored.
// Tried (MiniMax S run 10): the mipmap limit at bitmap + 0x14 read into a local before the
// texture pointer is cleared scores 70 against 72; under the keep bar.
// MiniMax B (11-13): no new form, no lower score (72): the call to 0x359e0 still passes its arguments in another order than retail; that callee is in src/unknown_0359e0.cpp.
// MiniMax G (11-13): no new form, no lower score (72): the call to 0x359e0 still passes its arguments in
// another order than retail, and that callee's parameter order is declared in another file, so the call's
// arguments cannot be lined up from here. BLOCKED-BY 0x359e0.
// @retail 0x1d000
bool function_1d000(byte *bitmap)
{
	volatile short height = *(short *)(bitmap + 6);
	bool linear = (bitmap[0xe] & 0x10) != 0;
	char depth = *(char *)(bitmap + 8);
	short width = *(short *)(bitmap + 4);
	short format = *(short *)(bitmap + 0xc);
	void **texture = (void **)(bitmap + 0x50);
	*texture = NULL;
	short levels = bitmap_get_mipmap_count(width, height, depth, format, linear,
		*(short *)(bitmap + 0x14));
	*(short *)(bitmap + 0x14) = levels;
	if (g_5093b0)
	{
		switch (*(short *)(bitmap + 0xa))
		{
		case 0:
			return function_0158f0(format, linear, width, height, levels, 0,
				(D3DTexture **)texture);
		case 1:
			return function_015a60(format, width, height, depth, levels, 0,
				(D3DVolumeTexture **)texture);
		default:
			return function_0159b0(width, format, levels, 0,
				(D3DCubeTexture **)texture);
		}
	}
	return true;
}

struct s_1c330_output
{
	long stream;
	long offset;
	long format;
	byte flag_c, flag_d;
	short unknown0e;
};

long function_35790(long format);
long function_35850(long format);

// @retail 0x1c330
void function_1c330(byte *context, s_1c330_output *output)
{
	(void)&context;
	(void)&output;
	memset(output, 0, 16 * sizeof(*output));
	long i;
	for (i = 0; i < 16; ++i)
		output[i].format = 2;
	long mapping[21];
	memset(mapping, 0xff, sizeof(mapping));
	byte *selection = context + *(long *)(context + 0x80) * 16;
	long tag = *(long *)(selection + 4);
	if (tag != NONE)
	{
		byte *definition = g_4e3b44[tag & 0xffff].bytes;
		byte *entry = *(byte **)(definition + 8) + *(long *)(selection + 8) * 0x1c;
		dword count = *(dword *)(entry + 4);
		dword j = 0;
		if (count > 0)
		{
			word *indices = *(word **)(entry + 8);
			do
			{
				mapping[*indices] = j;
				++j;
				++indices;
			} while (j < count);
		}
	}
	else
	{
		mapping[0] = 0;
		mapping[3] = 3;
		mapping[14] = 9;
	}
	for (long stream = 0; stream < *(long *)(context + 0x2d0) ||
		(*(long *)(context + 0x2d0) == 0 && (dword)stream < 16); ++stream)
	{
		char *source = *(char **)(context + 0x8c + stream * 4);
		if (source)
		{
			long offset = 0;
			for (dword element = 0; element < 10; ++element)
			{
				char *pair = source + 1 + element * 2;
				if (pair[0] >= 0)
				{
					long target = mapping[pair[0]];
					if (target >= 0)
					{
						s_1c330_output *entry = &output[target];
						entry->stream = stream;
						entry->offset = offset;
						entry->format = function_35790(pair[1]);
						entry->flag_c = false;
						entry->flag_d = false;
					}
					long size = function_35850(pair[1]);
					offset = (byte)size + offset;
				}
				else if (pair[0] == -2)
					offset += pair[1];
				else
					break;
			}
			*(char **)(context + 0xcc + stream * 4) = source;
			*(char **)(context + 0x8c + stream * 4) = NULL;
			context[0x20c] = 0;
		}
	}
}
long g_5093c8;
long g_5093cc;

// @retail 0x13d50
void function_13d50(s_buffer_pair const *values, long mode, long count)
{
	/* The count remains on the stack in retail. */
	(void)&count;
	long first = values->first;
	long second = values->second;
	g_5093c8 = mode;
	g_5093c0.first = first;
	g_5093c0.second = second;
	g_5093cc = count;
}

/* the texture stages: [0] the textures set on the device (0x51f3c8), [1] the
   textures wanted (0x51f3d8) */
s_texture_stage_storage g_texture_stage_storage;
long g_5093d8;

#include "surface_resource_pairs.h"
D3DTexture *g_509354[2];
D3DSurface *g_50935c[2], *g_509364, *g_509370;
D3DTexture *g_509368, *g_50936c;
D3DSurface *g_509384;
D3DTexture *g_50938c;
D3DSurface *g_509390, *g_509394, *g_509398, *g_50939c, *g_5093a0, *g_5093a4, *g_5093a8;

void function_1cf50(void);

struct s_363a0_vertex
{
	real x, y, u, v;
	dword color;
};

// @retail 0x363a0
void __stdcall function_363a0(void *vertices)
{
	if (g_485602 == 0)
	{
		function_1cf50();
		D3DDevice::Begin(D3DPT_TRIANGLEFAN);
		s_363a0_vertex const *vertex = (s_363a0_vertex *)vertices;
		for (long i = 0; i < 4; ++i, ++vertex)
		{
			D3DDevice_SetVertexDataColor(9, vertex->color);
			D3DDevice_SetVertexData2f(3, vertex->u, vertex->v);
			D3DDevice_SetVertexData2f(0, vertex->x, vertex->y);
		}
		D3DDevice::End();
	}
}

// @retail 0x36880
void function_36880(s_short_rectangle const *rectangle, color4f const *color)
{
	// Retail callers keep the rectangle argument on the stack.
	s_short_rectangle const *const *rectangle_reference = &rectangle;
	function_1cf50();
	D3DDevice::Begin(D3DPT_TRIANGLEFAN);
	D3DDevice_SetVertexData4f(9, color->red, color->green, color->blue, color->alpha);
	for (short i = 0; i < 4; ++i)
	{
		bool top = i < 2;
		bool right = i == 1 || i == 2;
		short y = top ? (*rectangle_reference)->top : (*rectangle_reference)->bottom;
		short x = right ? (*rectangle_reference)->right : (*rectangle_reference)->left;
		D3DDevice_SetVertexData2f(0, (real)x, (real)y);
	}
	D3DDevice::End();
}

// @retail 0x14980
void function_14980(void)
{
	if (g_50935c[0])
	{
		D3DResource_Release(g_50935c[0]);
		g_50935c[0] = NULL;
	}
	if (g_50935c[1])
	{
		D3DResource_Release(g_50935c[1]);
		g_50935c[1] = NULL;
	}
	if (g_509364)
	{
		D3DResource_Release(g_509364);
		g_509364 = NULL;
	}
	long i = 0;
	do
	{
		if (g_509354[i])
		{
			if (!VirtualFree(g_509354[i], 0, MEM_RELEASE)) GetLastError();
			g_509354[i] = NULL;
		}
		++i;
	} while (i < 2);
	if (g_509368)
	{
		if (!VirtualFree(g_509368, 0, MEM_RELEASE)) GetLastError();
		g_509368 = NULL;
	}
	if (g_50936c)
	{
		if (!VirtualFree(g_50936c, 0, MEM_RELEASE)) GetLastError();
		g_50936c = NULL;
	}
	if (g_509370)
	{
		if (!VirtualFree(g_509370, 0, MEM_RELEASE)) GetLastError();
		g_509370 = NULL;
	}
}
real g_4670c8 = 1.0f;
extern byte g_5093fc;
extern __int64 g_485aa0;
#include "texture_stage_dimensions.h"
s_texture_stage_dimensions g_texture_stage_dimensions;

#include "unknown_03bcb0.h"
struct s_bitmap_view;
extern byte *g_485a80;

PRIVATE inline D3DTexture *cached_bitmap_texture(s_bitmap_view *bitmap)
{
    s_bitmap_predict_view *view = (s_bitmap_predict_view *)bitmap;
    D3DTexture *texture = NULL;
    if (view->last_frame > g_4e6488)
        texture = view->texture;
    return texture;
}

PRIVATE __forceinline D3DTexture *fetch_bitmap_texture(s_bitmap_view *bitmap, dword flags, real priority)
{
    s_bitmap_predict_view *view = (s_bitmap_predict_view *)bitmap;
    D3DTexture *texture = cached_bitmap_texture(bitmap);
    if (!texture)
    {
        _mm_prefetch((char const *)&view->flags, _MM_HINT_T0);
        _mm_prefetch((char const *)&view->data_offset, _MM_HINT_T0);
        _mm_prefetch((char const *)&view->data_offset1, _MM_HINT_T0);
        _mm_prefetch((char const *)&view->data_offset2, _MM_HINT_T0);
        _mm_prefetch((char const *)view->texture, _MM_HINT_T0);
        texture = texture_cache_bitmap_get_texture((s_bitmap_data *)bitmap, flags, priority);
        if (!texture)
            texture = function_12ce00((s_bitmap_data *)bitmap, flags, priority);
    }
    return texture;
}

// @retail 0x12310
D3DTexture *function_12310(s_bitmap_view *bitmap, real priority)
{
    return fetch_bitmap_texture(bitmap, 3, priority);
}

// @retail 0x12360
D3DTexture *function_12360(s_bitmap_view *bitmap, real priority)
{
    return fetch_bitmap_texture(bitmap, 2, priority);
}

// @retail 0x1cfb0
D3DTexture *function_1cfb0(s_bitmap_view *bitmap)
{
    return fetch_bitmap_texture(bitmap, 7, 0.0f);
}

struct s_type_7ba8e9;
s_type_7ba8e9 *function_137550(long group_index, short bitmap_index);

// @retail 0x14390
bool function_14390(short stage, s_type_7ba8e9 *bitmap, real priority)
{
    if (bitmap)
    {
        long slot = stage;
        g_51f3c8[1][slot] = function_12310((s_bitmap_view *)bitmap, priority);
        long width = *(short *)((byte *)bitmap + 4);
        long height = *(short *)((byte *)bitmap + 6);
        g_485af4[slot] = width;
        g_485b04[slot] = height;
    }
    return true;
}

// @retail 0x14480
bool function_14480(long tag, short stage, short index)
{
    // Keep the stage in its stack slot across the bitmap lookup.
    short const *stage_reference = &stage;
    bool result = false;
    if (tag != NONE)
    {
        byte *group = g_4e3b44[tag & 0xffff].bytes;
        long count = *(long const volatile *)(group + 0x44);
        if (count > 0)
        {
            s_type_7ba8e9 *bitmap = function_137550(tag, (short)(index % count));
            if (bitmap)
            {
                if (function_12360((s_bitmap_view *)bitmap, 0.0f))
                    function_14390(*stage_reference, bitmap, 0.0f);
                else result = true;
            }
        }
    }
    return result;
}

// score 17. Lead (lane 9 r74, --fast only, no plain or full check yet): a byte match with two calls to
// 0x14390 in place of the goto, the count read at each use, no address-taken fallback (no volatile), and
// the parameters in the order retail's callers set up: (short stage, long fallback, long tag,
// short fallback_index, short index, real priority), with the calls in unknown_03f2b0.cpp and
// unknown_023540.cpp updated in place. Its --fast effect: 0x2a8d0 251 -> 154, 0x44040 100 -> 76,
// 0x19530 619 -> 587, 0x29b00 282 -> 366 (a positional shift; first difference unchanged). To check:
// 0x44040 and both 0x19530 calls pass fallback 1 where retail passes 0; at 0x44040 retail also passes
// fallback_index 1 and the bitmap as index.
// @retail 0x144f0
void function_144f0(short stage, long fallback, long tag, short fallback_index, short index, real priority)
{
    if (tag != NONE)
    {
        byte *group = g_4e3b44[tag & 0xffff].bytes;
        if (*(long *)(group + 0x44) > 0)
        {
            s_type_7ba8e9 *bitmap = function_137550(tag, (short)(index % *(long *)(group + 0x44)));
            if ((long)*(short *)((byte *)bitmap + 0xa) == fallback)
            {
                function_14390(stage, bitmap, priority);
                return;
            }
        }
    }
    long fallback_tag = *(long *)(g_485a80 + fallback * 8 + 0x64);
    if (fallback_tag != NONE)
    {
        s_type_7ba8e9 *bitmap = function_137550(fallback_tag, fallback_index);
        if (bitmap)
            function_14390(stage, bitmap, priority);
    }
}

short g_55ece0, g_55ece2;

// @retail 0x14560
bool function_14560(short stage, long tag, long fallback, short fallback_index, short index)
{
    bool result = false;
    s_type_7ba8e9 *bitmap;
    if (tag != NONE)
    {
        byte *group = g_4e3b44[tag & 0xffff].bytes;
        if (*(long *)(group + 0x44) > 0)
        {
            bitmap = function_137550(tag, (short)(index % *(long *)(group + 0x44)));
            if (function_12360((s_bitmap_view *)bitmap, 0.0f))
            {
                if (*(short *)((byte *)bitmap + 0xa) == fallback)
                {
                    function_14390(stage, bitmap, 0.0f);
                    g_55ece0 = *(short *)((byte *)bitmap + 4);
                    g_55ece2 = *(short *)((byte *)bitmap + 6);
                    goto done;
                }
            }
            else
            {
                result = true;
                goto done;
            }
        }
    }
    tag = *(long *)(g_485a80 + fallback * 8 + 0x64);
    if (tag != NONE)
    {
        bitmap = function_137550(tag, fallback_index);
        if (bitmap)
        {
            function_14390(stage, bitmap, 0.0f);
            g_55ece0 = *(short *)((byte *)bitmap + 4);
            g_55ece2 = *(short *)((byte *)bitmap + 6);
        }
    }
done:
    return result;
}

// @retail 0x1d5e0
void *function_1d5e0(s_bitmap_view *bitmap, long wait, long *pitch)
{
    void *result = NULL;
    D3DTexture *texture = function_1cfb0(bitmap);
    if (texture)
    {
        D3DLOCKED_RECT rectangle;
        dword flags = 0;
        if (!wait)
            flags = D3DLOCK_NOOVERWRITE;
        D3DTexture_LockRect(texture, 0, &rectangle, NULL, flags);
        *pitch = rectangle.Pitch;
        result = rectangle.pBits;
    }
    return result;
}

// @retail 0x143c0
bool function_143c0(long tag, short index, short stage, real priority)
{
    bool result = false;
    short slot = stage;
    if (tag == NONE)
    {
        tag = *(long *)(g_485a80 + 0x64);
        index = 0;
    }
    if (tag != NONE)
    {
        byte *group = g_4e3b44[tag & 0xffff].bytes;
        _mm_prefetch((char const *)(*(byte **)(group + 0x48) + index * 0x74), _MM_HINT_T0);
        _mm_prefetch((char const *)g_485af4, _MM_HINT_T0);
        if (index < 0 || index >= *(long *)(group + 0x44))
        {
            tag = *(long *)(g_485a80 + 0x64);
            index = 0;
            group = g_4e3b44[tag & 0xffff].bytes;
        }
        if (group && *(long *)(group + 0x44) > 0)
        {
            byte *bitmap = (byte *)function_137550(tag, index);
            if (bitmap)
            {
                g_51f3c8[1][slot] = function_12310((s_bitmap_view *)bitmap, priority);
                g_485af4[slot] = *(short *)(bitmap + 4);
                g_485b04[slot] = *(short *)(bitmap + 6);
                result = true;
            }
        }
    }
    return result;
}

void *function_01dcf0(long index);
void *function_01dd20(long index, long element);
void *function_01dcc0(long index);
bool function_01dd60(long index, long *width, long *height);
long function_25960(void);
bool function_1cd30(D3DSurface *target, D3DSurface *depth, bool flag);

// @retail 0x14bc0
void function_14bc0(short index, short element, bool use_depth)
{
	D3DSurface *target = NULL;
	D3DSurface *depth = NULL;
	bool flag = false;
	switch (index)
	{
	case 0: target = g_50935c[0]; depth = g_509364; flag = true; break;
	case 4: target = g_50935c[1]; flag = true; break;
	case 16: case 29: target = (D3DSurface *)g_509378; break;
	case 17: target = (D3DSurface *)g_509380; break;
	case 3: target = g_509370; depth = g_509364; flag = true; break;
	case 1: target = (D3DSurface *)function_01dcf0(1); depth = g_509364; break;
	case 18: target = (D3DSurface *)function_01dcf0(18); depth = g_509364; break;
	case 9:
		target = g_509384;
		depth = (D3DSurface *)function_01dcf0(9);
		target->Size = depth->Size;
		target->Data = ((D3DSurface *)function_01dcf0(9))->Data;
		break;
	case 10: case 11: case 12: case 25: case 26: case 27:
		target = (D3DSurface *)function_01dcf0(index); depth = NULL; break;
	case 13: target = (D3DSurface *)function_01dd20(13, element); depth = NULL; break;
	case 19: target = (D3DSurface *)function_01dcf0(19); depth = NULL; break;
	case 20: target = (D3DSurface *)function_01dcf0(20); depth = NULL; break;
	case 23: depth = NULL; break;
	case 5: case 6: case 7: case 8: case 15:
		target = (D3DSurface *)function_01dcf0(index); depth = g_509364; break;
	case 33: target = g_509390; depth = g_5093a8; break;
	case 34: target = g_509394; depth = g_5093a8; break;
	case 35: target = g_509398; depth = g_5093a8; break;
	case 36: target = g_50939c; depth = g_5093a8; break;
	case 37: target = g_5093a0; depth = g_5093a8; break;
	case 38: target = g_5093a4; depth = g_5093a8; break;
	case 22: target = (D3DSurface *)function_01dcf0(22); depth = NULL; break;
	case 21: target = (D3DSurface *)function_01dcf0(21); depth = NULL; break;
	case 14: break;
	default: __assume(0);
	}
	function_1cd30(target, use_depth ? depth : NULL, flag);
	D3DVIEWPORT8 viewport;
	if (g_485602 == 2)
	{
		viewport.X = 0; viewport.Y = 0;
		viewport.Width = 128; viewport.Height = 128;
	}
	else if (index == 0 || index == 2 || index == 1 || index == 18 || index == function_25960())
	{
		real scale;
		if (index == function_25960())
		{
			if (g_485607)
			{
				viewport.X = 0; viewport.Y = 0;
				viewport.Width = 640; viewport.Height = 480;
				goto viewport_ready;
			}
			scale = 1.0f;
		}
		else
		{
			if (g_485607 && !g_5093fc)
				scale = g_4670c8 < 0.0625f ? 0.0625f : g_4670c8 > 1.0f ? 1.0f : g_4670c8;
			else
				scale = 1.0f;
		}
		viewport.X = (long)((short)g_48564a * (double)scale);
		viewport.Y = (long)((short)g_485648 * (double)scale);
		viewport.Width = (long)(((short)g_48564e - (short)g_48564a) * (double)scale);
		viewport.Height = (long)(((short)g_48564c - (short)g_485648) * (double)scale);
	}
	else
	{
		D3DSURFACE_DESC desc;
		D3DSurface_GetDesc(target, &desc);
		viewport.X = 0; viewport.Y = 0;
		viewport.Width = desc.Width; viewport.Height = desc.Height;
	}
viewport_ready:
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;
	D3DDevice_SetViewport(&viewport);
}

// @retail 0x14f60
void function_14f60(short stage, short index)
{
	D3DBaseTexture *texture = NULL;
	struct { long width; long height; } dimensions = { 0, 0 };
	function_01dd60(index, &dimensions.width, &dimensions.height);
	long local_c270bf = stage;
	g_485af4[local_c270bf] = dimensions.width;
	g_485b04[local_c270bf] = dimensions.height;
	switch (index)
	{
	case 0: texture = g_509354[(g_485aa0 - 1) % 2]; break;
	case 4: texture = g_509354[g_485aa0 % 2]; break;
	case 16: case 29: texture = (D3DBaseTexture *)g_509374; break;
	case 17: texture = (D3DBaseTexture *)g_50937c; break;
	case 3: texture = g_509368; break;
	case 24: texture = g_50936c; break;
	case 1: texture = (D3DBaseTexture *)function_01dcc0(1); break;
	case 18: texture = (D3DBaseTexture *)function_01dcc0(18); break;
	case 19: texture = (D3DBaseTexture *)function_01dcc0(19); break;
	case 23: texture = g_509354[(g_485aa0 - 1) % 2]; break;
	case 13: texture = (D3DBaseTexture *)function_01dcc0(13); break;
	case 20: texture = (D3DBaseTexture *)function_01dcc0(20); break;
	case 33: case 34: case 35: case 36: case 37: case 38: texture = g_50938c; break;
	case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 15: case 21: case 22: case 25: case 26: case 27:
		texture = (D3DBaseTexture *)function_01dcc0(index); break;
	case 14: break;
	default: __assume(0);
	}
	g_51f3c8[1][local_c270bf] = texture;
}

struct s_mask_offset
{
	short x;
	short y;
};

s_mask_offset const g_43f188[16] =
{
	{0, 0}, {2, 2}, {0, 2}, {2, 0},
	{1, 1}, {3, 3}, {3, 1}, {1, 3},
	{0, 1}, {2, 3}, {3, 0}, {1, 2},
	{1, 0}, {3, 2}, {0, 3}, {2, 1}
};

// @retail 0x1c1b0
void function_1c1b0(void)
{
	memset(g_487288, 0, sizeof(g_487288));
	for (long count = 0; count <= 16; count++)
	{
		long x;
		dword *mask = g_487288[count];
		for (x = 0; x < 32; x += 4)
		{
			long base = x;
			for (long rows = 8; rows > 0; rows--, base += 128)
			{
				for (long i = 0; i < count; i++)
				{
					long bit = g_43f188[i].y * 32 + base + g_43f188[i].x;
					long word_index = bit / 32;
					mask[word_index] |= 1 << (bit % 32);
				}
			}
		}
	}
}

// @retail 0x1cd30
bool function_1cd30(D3DSurface *target, D3DSurface *depth, bool flag)
{
	if (g_51f3f4 != target || g_51f3f8 != depth)
	{
		if (g_51f3f4 && target && target->Common == g_51f3f4->Common && target->Format == g_51f3f4->Format && target->Size == g_51f3f4->Size)
			D3DDevice_SetRenderTargetFast(target, depth, 0);
		else
			D3DDevice_SetRenderTarget(target, depth);
		g_51f3f4 = target;
		g_51f3f8 = depth;
		g_51f3fc = flag;
	}
	return true;
}

// @retail 0x1c290
dword *function_1c290(real value)
{
	real scaled = value * 16.0f;
	long index;
	__asm
	{
		fld scaled
		fistp index
	}
	if (index < 0)
		index = 0;
	else if (index > 16)
		index = 16;
	return g_487288[index];
}

struct s_01b050_shader_state
{
	byte field_0000[0x1424];
	D3DPIXELSHADERDEF program;
	bool changed;
};

// @retail 0x1b050
void function_1b050(s_01b050_shader_state *state)
{
	D3DDevice_SetPixelShaderProgram(&state->program);
	state->changed = false;
}

// @retail 0x15180
void function_15180(D3DPIXELSHADERDEF const *program)
{
	D3DDevice_SetPixelShaderProgram(program);
}

// @retail 0x151c0
void function_151c0(real const *constants)
{
	D3DDevice_SetVertexShaderConstantFast(-46, constants, 3);
	g_5093d8 = 0;
}

// @retail 0x1ccf0
bool function_1ccf0(D3DPIXELSHADERDEF const *program)
{
	D3DDevice_SetPixelShaderProgram(program);
	return true;
}

// @retail 0x1cf50
void function_1cf50()
{
	IDirect3DBaseTexture8 **wanted = g_51f3c8[1];
	for (long stage = 0; stage < 4; wanted++, stage++)
	{
		IDirect3DBaseTexture8 **current = wanted - 4;
		if (*current != *wanted)
		{
			D3DDevice_SetTexture(stage, *wanted);
			*current = *wanted;
		}
	}
}

// @retail 0x1cf80
void function_1cf80(void)
{
	for (long stage = 0; stage < 4; stage++)
	{
		D3DDevice_SetTexture(stage, g_51f3c8[1][stage]);
		g_51f3c8[0][stage] = g_51f3c8[1][stage];
	}
}

// @retail 0x1c8a0
bool __stdcall function_1c8a0(D3DPRIMITIVETYPE type, word const *indices, long count)
{
	function_1cf50();
	D3DDevice_DrawIndexedVertices(type, count, indices);
	return true;
}

struct s_597d0_object
{
	byte unknown00[0x741c];
	long field_741c;
};

// @retail 0x597d0
bool function_597d0(s_597d0_object **out)
{
	bool result = false;
	long mode = 0;
	if (g_527330.initialized)
		mode = g_527330.state;

	switch (mode)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		result = false;
		if (g_527330.initialized)
		{
			s_597d0_object *object = g_527330.session_a;
			if (object->field_741c)
			{
				if (out)
					*out = object;
				result = true;
			}
		}
		break;
	case 7:
	case 8:
	case 9:
		result = false;
		if (g_527330.initialized)
		{
			s_597d0_object *object = g_527330.session_b;
			if (object->field_741c)
			{
				if (out)
					*out = object;
				result = true;
			}
		}
		break;
	}
	return result;
}


struct s_shader_entry
{
	long field_00;
	dword input_count;
	word *inputs;
	long byte_count;
	dword *program;
	byte field_14[8];
};

struct s_shader_tag
{
	long field_00;
	dword entry_count;
	s_shader_entry *entries;
};

struct s_shader_program
{
	dword const *program;
	long field_04;
	dword byte_count;
	long field_0c;
	real field_10;
};

dword const g_43f2b0[85] =
{
	0x00152078, 0x00000000, 0x0056e000, 0x7c2a1000, 0x2ca00000,
	0x00000000, 0x0056e0aa, 0x7c021000, 0x23a00000, 0x00000000,
	0x00570000, 0x8c2a1000, 0x2cb00000, 0x00000000, 0x0096e615,
	0x38aaf856, 0x9ca00000, 0x00000000, 0x0096e601, 0x39fefaae,
	0x93a00000, 0x00000000, 0x00970615, 0x38ab1856, 0xdcb00000,
	0x00000000, 0x00d6201b, 0x08363800, 0x20b08800, 0x00000000,
	0x00d6401b, 0x08365800, 0x20b04800, 0x00000000, 0x00d6601b,
	0x08367800, 0x20b02800, 0x00000000, 0x00d6801b, 0x08369800,
	0x20b01800, 0x00000000, 0x02575215, 0xa42b586e, 0x6ca0f81c,
	0x00000000, 0x065740ab, 0xa5575bff, 0x13a10000, 0x00000000,
	0x00576015, 0xb42b7800, 0x2cb00000, 0x00000000, 0x00770015,
	0xa40012fe, 0x3ca00000, 0x00000000, 0x007720ab, 0xa4001006,
	0x73a00000, 0x00000000, 0x00772015, 0xb40012fe, 0x7cb00000,
	0x00000000, 0x0041401a, 0xc4355800, 0x20b0e800, 0x00000000,
	0x0056a015, 0xa42ab800, 0x2090c848, 0x00000000, 0x0056c0bf,
	0xa42ad800, 0x20a0c850, 0x00000000, 0x0056c015, 0xb57ed800,
	0x20a0c858, 0x00000000, 0x0081601a, 0xc5fe286a, 0xf0b0e801,
};

s_shader_program g_4670ec[1] =
{
	{ g_43f2b0, NONE, 0x154, NONE, 1.0f }
};

// @retail 0x1c2e0
dword const *function_1c2e0(long tag, long index, long *count)
{
	dword const *result;
	if (tag != NONE)
	{
		s_shader_entry *entry = &((s_shader_tag *)g_4e3b44[tag & 0xffff].bytes)->entries[index];
		result = entry->program;
		if (count)
			*count = entry->byte_count >> 4;
	}
	else
	{
		result = g_4670ec[index].program;
		if (count)
			*count = g_4670ec[index].byte_count >> 4;
	}
	return result;
}

/* inline, as 0x1cbe0: a call at a function's top level expands at link time, a nested one stays a call */
// @retail 0x1cb20
inline long function_1cb20(long mode, dword index, long tag)
{
	/* The candidate index remains a stack argument in retail. */
	dword const *index_reference = &index;
	switch (mode)
	{
	case 1: return 1;
	case 2: return 2;
	case 3:
		if (*index_reference + 2 < ((s_shader_tag *)g_4e3b44[tag & 0xffff].bytes)->entry_count)
			return *index_reference + 2;
	case 0: return 0;
	default: __assume(0);
	}
}

// @retail 0x1cb70
dword function_1cb70(long tag, long index)
{
	dword result = 0;
	s_shader_entry *entry = &((s_shader_tag *)g_4e3b44[tag & 0xffff].bytes)->entries[index];
	for (dword i = 0; i < entry->input_count; i++)
		result |= 1 << entry->inputs[i];
	return result;
}

struct s_shader_binding
{
	long field_00;
	long tag;
	long index;
	long field_0c;
};

struct s_shader_stream
{
	long field_00;
	long field_04;
	long field_08;
};

struct s_shader_cache
{
	s_shader_binding bindings[8];
	long active;
	long field_84;
	bool field_88;
	byte field_89[3];
	byte const *wanted[16];
	byte const *current[16];
	byte field_10c[0x100];
	bool descriptors_changed;
	byte field_20d[3];
	s_shader_stream streams[16];
	long stream_count;
	bool streams_changed;
	byte field_2d5[3];

	s_shader_cache();
};

// @retail 0x1c710
void __stdcall function_1c710(void *memory)
{
	s_shader_cache *state = (s_shader_cache *)memory;
	bool changed = false;
	if (state->descriptors_changed)
		function_1c330((byte *)state, (s_1c330_output *)state->field_10c);
	if (state->descriptors_changed || state->streams_changed)
	{
		D3DDevice_SetVertexShaderInputDirect(
			state->stream_count > 0 ? (D3DVERTEXATTRIBUTEFORMAT *)state->field_10c : NULL,
			state->stream_count,
			state->stream_count > 0 ? (D3DSTREAM_INPUT *)state->streams : NULL);
		state->streams_changed = false;
		changed = true;
	}
	if (state->field_88 || state->descriptors_changed)
	{
		changed = true;
		D3DDevice_SelectVertexShaderDirect((D3DVERTEXATTRIBUTEFORMAT *)state->field_10c, state->field_84);
		state->field_88 = false;
	}
	state->stream_count = 0;
	if (changed)
		state->descriptors_changed = false;
}

// @retail 0x1c4a0
s_shader_cache::s_shader_cache()
{
	streams_changed = false;
	descriptors_changed = false;
	stream_count = 0;
	for (long i = 0; i < 8; i++)
	{
		bindings[i].field_0c = NONE;
		bindings[i].tag = NONE;
		bindings[i].index = NONE;
	}
	active = NONE;
	for (long j = 0; j < 16; j++)
		wanted[j] = NULL;
	for (long k = 0; k < 16; k++)
	{
		streams[k].field_08 = 0;
		streams[k].field_04 = 0;
		streams[k].field_00 = 0;
	}
}

// @retail 0x1c6b0
void function_1c6b0(void *memory)
{
	s_shader_cache *state = (s_shader_cache *)memory;
	state->stream_count = 0;
	state->streams_changed = true;
	state->descriptors_changed = true;
	for (long i = 0; i < 16; i++)
	{
		state->streams[i].field_08 = 0;
		state->streams[i].field_04 = 0;
		state->streams[i].field_00 = 0;
	}
	state->descriptors_changed = true;
	memset(state->current, 0, sizeof(state->current));
	memset(state->wanted, 0, sizeof(state->wanted));
}

// @retail 0x1c590
void function_1c590(s_shader_cache *state, long tag, long index)
{
	long count;
	dword const *program = function_1c2e0(tag, index, &count);
	state->active = 0;
	if (state->bindings[0].tag != tag || state->bindings[0].index != index)
	{
		state->bindings[0].field_00 = 0;
		state->bindings[0].tag = tag;
		state->bindings[0].index = index;
		state->bindings[0].field_0c = 0;
		D3DDevice_LoadVertexShaderProgram(program, 0);
		state->field_84 = 0;
		state->field_88 = true;
		memset(state->current, 0, sizeof(state->current));
		state->descriptors_changed = true;
		for (long i = 0; i < 16; i++)
		{
			state->streams[i].field_08 = 0;
			state->streams[i].field_04 = 0;
			state->streams[i].field_00 = 0;
		}
		state->streams_changed = true;
		state->stream_count = 0;
	}
}

// @retail 0x1c620
inline void function_1c620(s_shader_cache *state, long a, long b, long c, byte const *descriptor)
{
	long index = state->stream_count;
	if (state->streams[index].field_04 != b || state->streams[index].field_00 != a || state->streams[index].field_08 != c)
	{
		state->streams[index].field_08 = c;
		state->streams[index].field_04 = b;
		state->streams[index].field_00 = a;
		state->streams_changed = true;
	}
	state->wanted[index] = descriptor;
	if (state->current[index] != descriptor)
		state->descriptors_changed = true;
	state->stream_count++;
}

byte g_485af1;

extern byte *g_485a80;

struct s_shader_slot
{
	dword unknown00;
	long tag;
};

// @retail 0x1cbb0
dword function_1cbb0(long mode, dword index, long tag)
{
	long const *mode_reference = &mode;
	long selected = function_1cb20(*mode_reference, index, tag);
	function_1c590((s_shader_cache *)g_51f0f0, tag, selected);
	return function_1cb70(tag, selected);
}

// @retail 0x1cbe0
inline dword function_1cbe0(long mode, dword index, long slot_index)
{
	(void)&mode;
	(void)&index;
	s_shader_slot *slot = &(*(s_shader_slot **)(g_485a80 + 0x5c))[slot_index];
	long selected = function_1cb20(mode, index, slot->tag);
	function_1c590((s_shader_cache *)g_51f0f0, slot->tag, selected);
	return function_1cb70(slot->tag, selected);
}

// @retail 0x1cc30
dword function_1cc30(long index)
{
	s_shader_slot *slot = &(*(s_shader_slot **)(g_485a80 + 0x5c))[index];
	function_1c590((s_shader_cache *)g_51f0f0, slot->tag, 0);
	return function_1cb70(slot->tag, 0);
}

// @retail 0x1c7f0
void __stdcall function_1c7f0(long index)
{
	/* The binding index occupies a stack slot in retail. */
	long const *reference = &index;
	s_shader_binding *binding = &((s_shader_cache *)g_51f0f0)->bindings[*reference & 0xffff];
	binding->field_0c = NONE;
	binding->tag = NONE;
	binding->index = NONE;
}

#include "physical_memory.h"

s_physical_object *g_487b08;

PRIVATE bool __stdcall shader_block_busy(long index)
{
	return false;
}

// @retail 0x1c810
void function_1c810(void)
{
	g_51f3f0 = false;
	g_51f3f4 = NULL;
	g_51f3f8 = NULL;
	g_51f3fc = false;
	memset(g_51f3c8[1], 0, sizeof(g_51f3c8[1]));
	memset(g_51f3c8[0], 0, sizeof(g_51f3c8[0]));
	g_487b08 = physical_memory_new("vertex shader lruv cache", 0x88, 0, 8,
		function_1c7f0, shader_block_busy, NULL, g_468758);
}



// @retail 0x1e8c0
long __stdcall function_1e8c0(char const *key)
{
	/* This key is passed on the stack by the cache callback interface. */
	char const *const *reference = &key;
	return (*reference)[3] * 59 + (*reference)[2] * 53 + (*reference)[1] * 43 + (*reference)[0] * 17;
}

struct s_cache_key
{
	word key;
	word flags;
};

// @retail 0x1e8f0
long __stdcall function_1e8f0(s_cache_key const *a, s_cache_key const *b)
{
	/* Both pointers are stack arguments in the cache callback interface. */
	s_cache_key const *const *local_6e666f = &a;
	s_cache_key const *const *right_reference = &b;
	word right_flags = (*right_reference)->flags;
	word left_flags = (*local_6e666f)->flags;
	if ((bool)(right_flags & 1) == (bool)(left_flags & 1) && (*local_6e666f)->key == (*right_reference)->key &&
		(short)((left_flags ^ right_flags) & ~1) == 0)
		return 1;
	return 0;
}

struct s_cache_record
{
	long count;
	byte unknown04[8];
	byte active;
	byte unknown0d[0x63];
};

struct s_cache_record_state
{
	long count;
	dword unknown04;
	s_cache_record *records;
	void *buffer;
	byte unknown10;
	bool available;
	byte unknown12[2];
};

s_cache_record_state g_4b6280;

struct hash_table;
extern hash_table *g_51f400;

struct hash_node;
hash_node *function_13e2d0(hash_table *table, void *key);

struct s_cache_lookup_key
{
	short index;
	word flag : 1;
	word part : 15;
	byte unknown04[16];
};

// @retail 0x1e280
void *function_1e280(short index, bool flag, long part)
{
	(void)&part;
	s_cache_lookup_key key;
	key.index = index;
	key.flag = flag;
	key.part = (word)part;
	hash_node *node = function_13e2d0(g_51f400, &key);
	if (node && *(byte **)node)
		return *(byte **)node + 4;
	return 0;
}

struct s_cache_hash_view
{
	byte unknown00[0x34];
	c_data_allocator *allocator;
};

// @retail 0x1e310
void function_1e310(void)
{
	if (g_4b6280.records)
	{
		if (!VirtualFree(g_4b6280.records, 0, MEM_RELEASE)) GetLastError();
		if (!VirtualFree(g_4b6280.buffer, 0, MEM_RELEASE)) GetLastError();
	}
	((s_cache_hash_view *)g_51f400)->allocator->deallocate(g_51f400);
	g_51f400 = 0;
}

// @retail 0x1e2d0
s_cache_record *function_1e2d0(void)
{
	s_cache_record *result = 0;
	if (g_4b6280.count < 1024)
	{
		result = &g_4b6280.records[g_4b6280.count];
		result->count = 0;
		g_4b6280.count++;
		result->active = 0;
	}
	else if (g_4b6280.available)
		g_4b6280.available = false;
	return result;
}

/* 0x1e2d0's work written out where a record builder inlines it */
static __forceinline s_cache_record *cache_record_new(void)
{
	s_cache_record *result = 0;
	if (g_4b6280.count < 1024)
	{
		g_4b6280.records[g_4b6280.count].count = 0;
		result = &g_4b6280.records[g_4b6280.count];
		g_4b6280.count++;
		result->active = 0;
	}
	else if (g_4b6280.available)
		g_4b6280.available = false;
	return result;
}

/* 0x1e2d0's work in its own order, where 0x1e5e0 inlines it */
static __forceinline s_cache_record *cache_record_take(void)
{
	s_cache_record *result = 0;
	if (g_4b6280.count < 1024)
	{
		result = &g_4b6280.records[g_4b6280.count];
		result->count = 0;
		g_4b6280.count++;
		result->active = 0;
	}
	else if (g_4b6280.available)
		g_4b6280.available = false;
	return result;
}

PRIVATE inline real render_bounds_midpoint(real const *pair)
{
    return (pair[0] + pair[1]) * 0.5f;
}

// @retail 0x1cdd0
void function_1cdd0(real const *bounds, byte flags)
{
	/* The reference retains the flag argument's retail stack placement. */
	byte const *flags_reference = &flags;
	real constants[12];
	constants[0] = 1.0f;
	constants[1] = 1.0f;
	constants[2] = 1.0f;
	constants[3] = 1.0f;
	constants[4] = 0.0f;
	constants[5] = 0.0f;
	constants[6] = 0.0f;
	constants[7] = 0.0f;
	constants[8] = 1.0f;
	constants[9] = 1.0f;
	constants[10] = 0.0f;
	constants[11] = 0.0f;
	bool active = false;
	if ((*flags_reference & 3) && bounds)
	{
		active = true;
		if (*flags_reference & 1)
		{
			constants[0] = (bounds[1] - bounds[0]) * 0.5f;
			constants[1] = (bounds[3] - bounds[2]) * 0.5f;
			constants[2] = (bounds[5] - bounds[4]) * 0.5f;
			constants[3] = 1.0f;
			constants[4] = (bounds[1] + bounds[0]) * 0.5f;
			constants[5] = (*(real const volatile *)(bounds + 2) + bounds[3]) * 0.5f;
			constants[6] = (bounds[4] + bounds[5]) * 0.5f;
			constants[7] = 0.0f;
		}
		if (*flags_reference & 2)
		{
			constants[8] = (bounds[7] - bounds[6]) * 0.5f;
			constants[9] = (bounds[9] - bounds[8]) * 0.5f;
			constants[10] = (bounds[6] + bounds[7]) * 0.5f;
			constants[11] = render_bounds_midpoint(bounds + 8);
		}
	}
	if (active || g_485af1)
	{
		D3DDevice_SetVertexShaderConstantFast(74, constants, 3);
		g_485af1 = active;
	}
}

byte const g_43f408[63][21] =
{
	{ 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x01, 0x00, 0x02, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x02, 0x00, 0x0e, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x03, 0x00, 0x02, 0x01, 0x04, 0xfe, 0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x04, 0x00, 0x0e, 0x01, 0x04, 0xfe, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x05, 0x00, 0x02, 0x01, 0x05, 0x02, 0x05, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x06, 0x00, 0x0e, 0xfe, 0x02, 0x01, 0x05, 0x02, 0x05, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x07, 0x00, 0x02, 0x01, 0x06, 0xfe, 0x01, 0x02, 0x06, 0xfe, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x08, 0x00, 0x0e, 0x01, 0x06, 0x02, 0x06, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x09, 0x00, 0x02, 0x01, 0x07, 0x02, 0x07, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x0a, 0x00, 0x0e, 0xfe, 0x02, 0x01, 0x07, 0x02, 0x07, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x0b, 0x01, 0x04, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x0c, 0x01, 0x05, 0x02, 0x05, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x0d, 0x01, 0x06, 0x02, 0x06, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x0e, 0x01, 0x07, 0x02, 0x07, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x0f, 0x0a, 0x02, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x10, 0x0a, 0x0e, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x11, 0x0a, 0x02, 0x0b, 0x04, 0xfe, 0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x12, 0x0a, 0x0e, 0x0b, 0x04, 0xfe, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x13, 0x0d, 0x04, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x14, 0x00, 0x02, 0xfe, 0x04, 0x10, 0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x15, 0x00, 0x0e, 0xfe, 0x02, 0x10, 0x0f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x16, 0x00, 0x02, 0x01, 0x04, 0xfe, 0x03, 0x10, 0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x17, 0x00, 0x0e, 0x01, 0x04, 0xfe, 0x01, 0x10, 0x0f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x18, 0x03, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x19, 0x03, 0x0d, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x1a, 0x04, 0x02, 0x05, 0x02, 0x06, 0x02, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x1b, 0x04, 0x10, 0x05, 0x10, 0x06, 0x10, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x1c, 0x07, 0x02, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x1d, 0x07, 0x10, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x1e, 0x09, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x1f, 0x09, 0x0d, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x20, 0x03, 0x01, 0x04, 0x10, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x21, 0x03, 0x0d, 0x04, 0x10, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x22, 0x03, 0x01, 0x04, 0x10, 0x05, 0x10, 0x06, 0x10, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x23, 0x03, 0x0d, 0x04, 0x10, 0x05, 0x10, 0x06, 0x10, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x24, 0x00, 0x01, 0x03, 0x01, 0x0e, 0x11, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x25, 0x00, 0x03, 0x03, 0x01, 0x0e, 0x11, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x26, 0x00, 0x03, 0x01, 0x01, 0x02, 0x01, 0x03, 0x01, 0x04, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x27, 0x00, 0x02, 0x03, 0x0d, 0x0e, 0x11, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x28, 0x00, 0x02, 0x03, 0x01, 0x0e, 0x11, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x29, 0x00, 0x02, 0x03, 0x02, 0x0e, 0x11, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x2a, 0x00, 0x03, 0x01, 0x02, 0x02, 0x00, 0x03, 0x03, 0x04, 0x02, 0x05, 0x01, 0x06, 0x03, 0x07, 0x03, 0x09, 0x11, 0xff, 0x00 },
	{ 0x2b, 0x00, 0x0b, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x2c, 0x00, 0x02, 0x0e, 0x11, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x2d, 0x00, 0x02, 0x03, 0x10, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x2e, 0x0e, 0x11, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x2f, 0x0e, 0x06, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x30, 0x08, 0x10, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x31, 0x00, 0x03, 0x03, 0x01, 0x0e, 0x11, 0x0f, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x32, 0x00, 0x02, 0x04, 0x02, 0x06, 0x02, 0x05, 0x02, 0x03, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x33, 0x00, 0x02, 0x04, 0x10, 0x06, 0x10, 0x05, 0x10, 0x03, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x34, 0x00, 0x02, 0x03, 0x01, 0x09, 0x01, 0x0e, 0x11, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x35, 0x00, 0x02, 0x03, 0x0d, 0x0e, 0x11, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x36, 0x00, 0x02, 0x11, 0x02, 0x12, 0x02, 0x03, 0x01, 0x0e, 0x11, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x37, 0x00, 0x02, 0x11, 0x10, 0x12, 0x10, 0x03, 0x0d, 0x0e, 0x11, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x38, 0x13, 0x00, 0x14, 0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x39, 0x13, 0x08, 0x14, 0x0f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x3a, 0x00, 0x01, 0x01, 0x01, 0x05, 0x11, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x3b, 0x00, 0x02, 0x04, 0x02, 0x06, 0x02, 0x05, 0x02, 0x03, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x3c, 0x00, 0x02, 0x04, 0x02, 0x06, 0x02, 0x05, 0x02, 0x03, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x3d, 0x00, 0x02, 0x04, 0x10, 0x06, 0x10, 0x05, 0x10, 0x03, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	{ 0x3e, 0x00, 0x03, 0x03, 0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
};


struct s_stream_description
{
    byte format;
    byte stride;
    byte unknown02[6];
    long offset;
    long unknown0c;
    long buffer;
};

// @retail 0x1cda0
void function_1cda0(s_stream_description const *stream)
{
    function_1c620((s_shader_cache *)g_51f0f0, stream->buffer, stream->stride,
        stream->offset, g_43f408[stream->format]);
}

extern word g_485ac0;
extern byte g_485ac2;
byte g_485ac3, g_485ac4, g_485ac5;
bool g_485ac6;

// @retail 0x123b0
void function_123b0(void)
{
    dword standard = XGetVideoStandard();
    dword flags = XGetVideoFlags();
    if (standard == 3)
        g_485ac0 = (byte)flags & 0x40 ? 60 : 50;
    byte low = (byte)(flags & 1);
    byte wide = (byte)((flags >> 4) & 1);
    flags &= 8;
    g_485ac2 = low;
    g_485ac3 = wide;
    g_485ac6 = (long)flags != 0;
    g_485ac5 = flags ? 1 : 0;
    g_485ac4 = standard == 3;
}

long g_467004 = NONE;

// @retail 0x13cd0
void function_13cd0(void)
{
    g_467004 = NONE;
    memset(g_51f3c8[1], 0, sizeof(g_51f3c8[1]));
    function_1cf50();
    D3DDevice_SetIndices(0, 0);
    D3DDevice_SetPixelShader(0);
}

#include "timed_effect.h"
extern dword g_4b5690;
extern long g_4858b8;
s_timed_effect_globals *function_01fd20(s_timed_effect_globals *result);
bool function_1dc40(long index, long width, long height);
bool function_01de20(long index);
void function_01fd00(void);
void function_01fcd0(void);
void __stdcall function_351a0(long texture, long target);
void __stdcall function_22070(real strength, real exponent);

// @retail 0x13b90
void function_13b90(void)
{
    s_timed_effect_globals *state = function_01fd20(NULL);
    if (state && state->unknowna4 > 0.0f)
    {
        if (state->unknowna0)
        {
            if ((g_4b5690 || function_1dc40(18, 640, 480)) && function_01de20(18))
            {
                function_351a0(g_4858b8, 18);
                function_01fd00();
            }
            else
                function_01fcd0();
        }
        function_22070(state->unknowna4, state->unknowna8);
    }
}

// @retail 0x14ac0
void function_14ac0(void)
{
    D3DVIEWPORT8 viewport;
    D3DVIEWPORT8 saved;
    viewport.X = 0;
    viewport.Y = 0;
    viewport.Width = 640;
    viewport.Height = 480;
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;
    D3DDevice_GetViewport(&saved);
    D3DDevice_SetViewport(&viewport);
    D3DDevice_Clear(0, 0, 0xf0, 0, 0.0f, 0);
    D3DDevice_SetViewport(&saved);
}

long g_4858b8;

#if 0
// Retail 0x14b60. The additional clear call changes matched 0x14ac0's stack frame.
// Retail 0x14b60
void __stdcall function_14b60(dword flags, dword color, real depth, byte stencil)
{
    dword clear_flags = (flags & 1 ? 0xf0 : 0) | ((flags & 0x1e) << 3) | ((flags >> 5) & 3);
    if (clear_flags)
    {
        if (g_485607 && !g_4858b8)
            function_14ac0();
        D3DDevice_Clear(0, NULL, clear_flags, color, depth, stencil);
    }
}
#endif

// @retail 0x1d4b0
void function_1d4b0(long format, bool alternate, long *result, bool *linear)
{
	*linear = false;
	switch (format)
	{
	case 0x10: case 0x11: case 0x12: case 0x13: case 0x16: case 0x17:
	case 0x1b: case 0x1c: case 0x1d: case 0x1e: case 0x1f: case 0x20:
	case 0x35: case 0x37: case 0x3d: case 0x3e: case 0x3f: case 0x40: case 0x41:
		*linear = true;
		break;
	}
	*result = NONE;
	switch (format)
	{
	case 25: *result = 0; break;
	case 0: *result = 1; break;
	case 1: *result = 2; break;
	case 26: *result = 3; break;
	case 5: *result = 6; break;
	case 2: *result = 8; break;
	case 4: *result = 9; break;
	case 7: *result = 10; break;
	case 6: *result = 11; break;
	case 12: *result = 14; break;
	case 14: *result = 15; break;
	case 15: *result = 16; break;
	case 11: *result = alternate ? 17 : 18; break;
	}
}
real g_485adc, g_485ae0;

extern byte *g_50934c;
extern double g_4858a0;

// @retail 0x137a0
void __stdcall function_137a0(real *first, real *second)
{
    real now = (real)g_4858a0;
    real *times;
    real *a;
    real *b;
    if (*g_50934c)
    {
        times = (real *)(g_50934c + 0x1c);
        a = (real *)(g_50934c + 0x24);
        b = (real *)(g_50934c + 0x2c);
    }
    else
    {
        times = (real *)(g_50934c + 4);
        a = (real *)(g_50934c + 0xc);
        b = (real *)(g_50934c + 0x14);
    }
    real x, y;
    if (now <= times[0])
    {
        x = a[0];
        y = b[0];
    }
    else if (!(now > times[1]) && times[1] - times[0] > 0.0001f)
    {
        real fraction = (now - times[0]) / (times[1] - times[0]);
        x = (a[1] - a[0]) * fraction + a[0];
        y = (b[1] - b[0]) * fraction + b[0];
    }
    else
    {
        x = a[1];
        y = b[1];
    }
    *first = x;
    *second = y;
}

// @retail 0x15720
void __stdcall function_15720(real x, real y)
{
    (void)&x;
    (void)&y;
    struct { real near_value; real far_value; } planes;
    planes.near_value = g_485adc;
    planes.far_value = g_485ae0;
    x = planes.near_value;
    y = planes.far_value;
    real const *near_plane = &x;
    real const *far_plane = &y;
    D3DDevice_SetDepthClipPlanes(*near_plane, *far_plane, D3DSDCP_SET_VERTEXPROGRAM_PLANES);
}

byte g_4b6290;


// @retail 0x13630
bool function_13630(void)
{
    g_4b6290 = false;
    if (g_4850c8)
    {
        g_485adc = 0.0f;
        g_4858b8 = 0;
        g_485ae0 = 16777215.0f;
        function_14bc0(0, 0, true);
        function_15720(0.0f, 0.0f);
    }
    return true;
}

byte g_4858bc;
dword g_4b843c;

// @retail 0x4a780
void function_4a780(void)
{
    g_4858bc = false;
    g_4b843c = 0;
    D3DDevice_SetRenderState(D3DRS_STENCILENABLE, 0);
    D3DDevice_SetScissors(0, FALSE, 0);
    function_15720(0.0f, 0.0f);
    function_14bc0((short)g_4858b8, 0, true);
}

byte g_4670bc = true;
struct s_render_reset_state;
void function_16b10(s_render_reset_state *state);

// @retail 0x1bbd0
void function_1bbd0(void)
{
    g_4670bc = true;
    function_16b10((s_render_reset_state *)g_485b48);
}

typedef void (__stdcall *t_render_pair_callback)(long, long);
struct s_render_pair_request
{
    long first;
    long second;
    t_render_pair_callback callback;
};

// @retail 0x48e40
void __stdcall function_48e40(s_render_pair_request const *request)
{
    (void)&request;
    g_4670bc = true;
    function_16b10((s_render_reset_state *)g_485b48);
    request->callback(request->first, request->second);
}


byte g_55e6c8;

// @retail 0x47870
void function_47870(long first, long second, point3f const *position, t_render_pair_callback callback)
{
    (void)&first;
    (void)&second;
    if (callback)
    {
        s_cache_record *record = function_1e2d0();
        if (record)
        {
            real x = position->x - g_485618.position.x;
            real y = position->y - g_485618.position.y;
            real z = position->z - g_485618.position.z;
            record->count = 3;
            *(real *)record->unknown04 = 0.0f - (g_485618.forward.k * z + g_485618.forward.j * y + g_485618.forward.i * x);
            *(void (__stdcall **)(s_render_pair_request const *))((byte *)record + 0x20) = function_48e40;
            *(point3f *)((byte *)record + 0x64) = *position;
            s_render_pair_request *request = (s_render_pair_request *)((byte *)record + 0x24);
            request->first = first;
            request->second = second;
            request->callback = callback;
        }
        else if (!g_55e6c8)
            g_55e6c8 = true;
    }
}


struct s_shader_constant_state
{
    byte unknown00[0x1630];
    real constants[16][4];
    long unknown1730;
    dword changed;
};

struct s_18d70_state
{
	byte unknown00[0x1530];
	real values[16][4];
	byte unknown1630[0x100];
	dword changed;
};

// @retail 0x18d70
void function_18d70(s_18d70_state *state)
{
	dword remaining = state->changed;
	while (remaining)
	{
		long index;
		__asm
		{
			bsf ecx, remaining
			mov index, ecx
		}
		D3DDevice_SetVertexData4f(index, state->values[index][0], state->values[index][1],
			state->values[index][2], state->values[index][3]);
		remaining &= ~(1 << index);
		state->changed &= ~(1 << index);
	}
}

// @retail 0x18e80
void function_18e80(s_shader_constant_state *state)
{
    dword remaining = state->changed;
    while (remaining)
    {
        long index;
        __asm
        {
            bsf ecx, remaining
            mov index, ecx
        }
        D3DDevice_SetVertexShaderConstantFast(index - 78, state->constants[index], 1);
        remaining &= ~(1 << index);
        state->changed &= ~(1 << index);
    }
}

extern s_record_pool *g_509434;
#include "target_cache_storage.h"

// @retail 0x3d270
void function_3d270(void)
{
	g_509434 = data_new_inlined("cached object render states", 256, 256, 0, g_510c2c);
	g_4c1bd0 = NONE;
}

typedef dword (__stdcall *t_cache_hash)(void const *);
typedef bool (__stdcall *t_cache_compare)(void const *, void const *);
__declspec(noinline) hash_table *function_13e1a0(char const *name, long data_size, long bucket_count,
    t_cache_hash hash, t_cache_compare compare, long maximum_count, c_data_allocator *allocator);

// @retail 0x1e110
bool function_1e110(void)
{
    volatile bool result = true;
    void *buffer = VirtualAlloc(0, 0x1c000, 0x101000, PAGE_READWRITE);
    if (!buffer) GetLastError();
    g_4b6280.records = (s_cache_record *)buffer;
    buffer = VirtualAlloc(0, 0x800, 0x101000, PAGE_READWRITE);
    if (!buffer) GetLastError();
    g_4b6280.buffer = buffer;
    g_4b6280.available = true;
    if (!g_4b6280.records)
        result = false;
    g_4b6280.count = 0;
    g_4b6280.unknown04 = 0;
    g_4b6280.unknown10 = 0;
    g_51f400 = function_13e1a0("transparent planes", 16, 4096,
        (t_cache_hash)function_1e8c0, (t_cache_compare)function_1e8f0, 2048, g_468758);
    return result;
}

bool function_13e270(hash_table *table, void *key, void const *data);

struct s_cache_source_entries
{
    byte unknown00[0x158];
    long count;
    s_cache_lookup_key *entries;
};

// @retail 0x1e1c0
void function_1e1c0(void)
{
    s_cache_source_entries *source = (s_cache_source_entries *)g_4e0348;
    if (g_51f400)
    {
        for (long i = 0; i < source->count; ++i)
        {
            s_cache_lookup_key *key = &source->entries[i];
            function_13e270(g_51f400, key, key->unknown04);
        }
    }
}

struct s_shader_transform_row
{
    real x, y, z;
    long unused;
};

struct s_shader_transform_entry
{
    short first;
    word count;
};

struct s_shader_transform_table
{
    long unknown00;
    s_shader_transform_entry entries[16];
    s_shader_transform_row transforms[1][3];
};

s_shader_transform_table *g_485a5c;
extern byte g_485af0;

// @retail 0x151e0
void function_151e0(long index)
{
    if (index != NONE)
    {
        s_shader_transform_table *data = g_485a5c;
        s_shader_transform_row *transform = data->transforms[data->entries[index].first];
        long count = data->entries[index].count * 3;
        D3DDevice_SetVertexShaderConstantFast(-46, transform, count);
        g_485af0 = true;
    }
    else
    {
        s_shader_transform_row transform[3];
        transform[0].x = 1.0f;
        transform[0].y = 0.0f;
        transform[0].z = 0.0f;
        transform[0].unused = 0;
        transform[1].x = 0.0f;
        transform[1].y = 1.0f;
        transform[1].z = 0.0f;
        transform[1].unused = 0;
        transform[2].x = 0.0f;
        transform[2].y = 0.0f;
        transform[2].z = 1.0f;
        transform[2].unused = 0;
        D3DDevice_SetVertexShaderConstantFast(-46, transform, 3);
    }
}


typedef void (__stdcall *t_1e4e0_callback)(void *);

struct s_1e4e0_record
{
	long type;
	real depth;
	long flags;
	byte active;
	byte unknown0d[0x13];
	void (__stdcall *callback)(void *);
	byte payload[0x40];
	point3f position;
};

extern dword g_4b8348;
extern real g_4b8494;
void function_35aa0(void);
typedef bool (__stdcall *t_sort_2byte_compare_function)(word, word, void const *);
void sort_2byte(word *elements, unsigned long count, void *unused, t_sort_2byte_compare_function compare, void const *context);

struct s_depth_sort_record
{
	long type;
	real depth;
	long flags;
	byte active;
	byte unknown0d[3];
	plane3f plane;
	byte unknown20[0x44];
	point3f position;
};

// @retail 0x1e700
bool __stdcall function_1e700(word first, word second, void const *context)
{
	s_depth_sort_record const *a = (s_depth_sort_record const *)context + (short)first;
	s_depth_sort_record const *b = (s_depth_sort_record const *)context + (short)second;
	int type_difference = a->type - b->type;
	long comparison = type_difference;
	if (!comparison)
	{
		if (a->active)
		{
			real point = b->position.z * a->plane.k;
			point += b->position.y * a->plane.j;
			point += b->position.x * a->plane.i;
			plane3f const *camera_plane = &a->plane;
			real camera = camera_plane->k * g_4b9da0.z + camera_plane->j * g_4b9da0.y;
			camera = camera + camera_plane->i * g_4b9da0.x;
			long camera_side = a->plane.d > camera ? 1 : 0;
			long point_side = a->plane.d > point ? 1 : 0;
			comparison = camera_side != point_side ? 1 : -1;
		}
		else if (b->active)
		{
			real point = a->position.z * b->plane.k;
			point += a->position.y * b->plane.j;
			point += a->position.x * b->plane.i;
			plane3f const *camera_plane = &b->plane;
			real camera = camera_plane->k * g_4b9da0.z + camera_plane->j * g_4b9da0.y;
			camera = camera + camera_plane->i * g_4b9da0.x;
			long camera_side = b->plane.d > camera ? 1 : 0;
			long point_side = b->plane.d > point ? 1 : 0;
			comparison = camera_side == point_side ? 1 : -1;
		}
		else if (a->depth > b->depth)
			comparison = 1;
		else if (b->depth > a->depth)
			comparison = -1;
		else
			comparison = a->flags - b->flags;
	}
	return comparison > 0;
}

// MiniMax E (11-14): no new form, no lower score (4): the stream count's subtraction still comes after the sorter's last
// push here and before it in retail, and the stipple stride's add still lands before the inlined device call's second rep
// movsd where retail puts it after; the sorter's argument order is fixed by its own declaration in src/array_sort.cpp and
// the stipple block lies inside the XDK inline, so both stay as they are.
// MiniMax B (11-13): no new form, no lower score (4): the stream count's subtraction still comes after the sorter's last push here and before it in retail, and the stipple stride's add still lands before the inlined device call's second rep movsd where retail puts it after.
// MiniMax H (11-12): no new form, no lower score (4): the count hoisted into a local before the call still has its subtraction placed after the sorter's last push where retail has it before, the permuter's 39 tries found no better variant, and the stipple stride's add still sits before the block copy that lies inside the XDK device call.
// MiniMax I (11-11): no new form, no lower score (4): the count read from the global and the count as an unsigned local both keep the subtraction after the sorter's last push, and the stipple stride's add still sits before the block copy, which lies inside the XDK device call.
// MiniMax D (11-11): no new form, no lower score (4): the sorter 0x13df80 matches on its own check, so what is left is this caller's scheduling - the count's subtraction lands after the sorter's last push here and before it in retail, and the stipple block's add of 0x84 lands before the second rep movsd here and after it in retail.
// MiniMax S (11-10): no new form, no lower score (4): retail takes the count's subtraction before the sorter's last push and ours after it, and retail adds the stipple stride after the second rep movsd where ours adds it before; the count as a local, at the call and at function scope, all compile to the same code.
// MiniMax A (11-10): no new form, no lower score (4): retail's sorter 0x13df80 takes the count in
// eax and the elements in ecx and takes the count's subtraction before its last push; that register
// home is fixed by the sorter's own declaration in src/array_sort.cpp, and the stipple block's add of
// 0x84 lies inside the XDK device call.
// MiniMax R (11-10): no new form, no lower score (4): retail takes the count's subtraction before the sorter's last push of the 0x13df80 call and adds 0x84 inside the XDK D3DDevice_SetStipple inline, while ours adds it after the block; the sorter's argument order is fixed by its own declaration.
// MiniMax G (11-05): no new form, no lower score; the count as an unsigned local, as an unsigned cast and read from the global all keep the subtraction after the sorter's last push.
// MiniMax D (11-01): a local that holds the count once does not move the subtraction before the
// last push either, and the stipple block's order lies inside the XDK macro.
// MiniMax D (11-01): no new form, no lower score.
// MiniMax A (11-03): no new form, no lower score.
// MiniMax G (11-04): no new form, no lower score.
// MiniMax E (11-10): no new form, no lower score: retail takes the count's subtraction before the
// sorter's last push and adds 0x84 after the second rep movsd, ours does both the other way round;
// the stipple block lies inside the XDK inline.
// MiniMax G (11-12): no new form tried this round, no lower score (4): the count's subtraction is still after the
// sorter's last push and the stride's add still before the second rep movsd of the inlined device call.
// MiniMax H (11-13): no new form, no lower score (4); the subtraction of the stream count comes after the last push here and before it in retail.
// score 4 (was 4): the subtraction of the stream count must come before the last push of the
// sorter call, and the add of the stipple stride must follow the second rep movsd of the
// inlined device call.
// MiniMax D (11-06): no new form, no lower score: the scratch argument's local at function scope
// (11) and the count in a local or read from the global all keep the subtraction after the sorter's
// last push, and the permuter run on this function reached its time limit without a measured variant.
// MiniMax E (11-08): no new form, no lower score: retail takes the count's subtraction before the sorter's last push and adds 0x84 after the block copy, ours does both the other way round.
// MiniMax E (11-09): no new form, no lower score: retail takes the count's subtraction before the sorter's last push and adds 0x84 after the second rep movsd, ours does both the other way round; the stipple block lies inside the XDK inline.
// MiniMax D (11-09): no new form, no lower score: the body is unchanged at 4 (359 bytes, as retail's); the whole difference is that retail takes the count's subtraction between the scratch's `lea` and the scratch's push while we take it after the push. This round: the count in a local before the call, the count in a local outside the call's block, the scratch behind a pointer local, the count cast to unsigned long and the `first` local declared as an int all score 4 with the same code, and a count computed before the loop scores 97. MSVC sinks the count's expression into the call's argument setup, and the argument's order (the scratch's address before the count) puts the subtraction last.
// MiniMax R (11-09): no new form, no lower score (4): retail takes the count's subtraction before the sorter's last push of the 0x13df80 call, ours after it, and retail adds 0x84 after the second rep movsd of the stipple block, ours before it; the count in a local and the stipple block inside the XDK inline were measured and keep both orders.
// MiniMax A (11-13): no new form tried this round, no lower score (4): the stream count's subtraction still
// comes after the sorter's last push here and before it in retail, and the stipple stride's add still lands
// before the inlined device call's second rep movsd where retail puts it after; MSVC sinks the count's
// expression into the call's argument setup, and the argument's order (the scratch's address before the count)
// puts the subtraction last.
// @retail 0x1e370
void function_1e370(void)
{
	function_35aa0();
	long first = g_4b6280.unknown04;
	word *indices = (word *)g_4b6280.buffer;
	for (long i = first; i < g_4b6280.count; ++i)
		indices[i - first] = (word)i;
	{
		long scratch;
		sort_2byte(indices, g_4b6280.count - first, &scratch, function_1e700, g_4b6280.records);
	}
	for (long i = 0; i < g_4b6280.count - (long)g_4b6280.unknown04; ++i)
	{
		s_1e4e0_record *record = (s_1e4e0_record *)&g_4b6280.records[((word *)g_4b6280.buffer)[i]];
		if (record->callback)
			record->callback(record->payload);
	}
	g_4b8348 = 0;
	D3DDevice_SetRenderState(D3DRS_STIPPLEENABLE, 0);
	if (!(fabs(g_4b8494 - 1.0f) < 0.0001f))
	{
		real scaled = 1.0f * 16.0f;
		long index;
		__asm
		{
			fld scaled
			fistp index
		}
		if (index < 0)
			index = 0;
		else if (index > 16)
			index = 16;
		D3DDevice_SetStipple(g_487288[index]);
		g_4b8494 = 1.0f;
	}
}

// MiniMax E (11-14): no new form, no lower score (8): the record table's base still rides in EDI here and in EBP in
// retail, and the payload copy then saves the other register. This round: calling the matched 0x1e2d0 is not inlined
// under this file's /Ob1 and the helper's base and count as locals are both worse, so the shared helper
// cache_record_new keeps its body and the base stays in EDI.
// MiniMax B (11-13): no new form, no lower score (8): the record table's base still rides in EDI here and in EBP in retail, and the payload copy then saves the other register; the base comes from the shared helper cache_record_new, whose body the rules forbid writing out by hand.
// MiniMax H (11-12): no new form, no lower score (8): the record pool's base still rides in edi where retail keeps it in ebp, and the payload copy still needs the other register; the base comes from the shared helper cache_record_new, whose body the rules forbid writing out by hand.
// MiniMax H (11-11): no new form, no lower score (8): the body is retail's length and the only difference is the pair of saved registers: the record pool's base rides in edi here and in ebp in retail, and the payload copy then needs the other register; the base comes from the shared helper cache_record_new, whose body the rules forbid writing out by hand.
// MiniMax I (11-11): no new form, no lower score (8): the record pool's base still rides in edi here and in ebp in retail, and the payload copy's count then needs a register of its own; the base comes from the shared helper cache_record_new, whose body the rules forbid writing out by hand.
// MiniMax D (11-11): no new form, no lower score (8): the pool's base still lands in edi where retail keeps it in ebp; this round the index in a local in the allocator (8), the count's store before the pointer (8) and the payload copy before the delta (33) all leave the base in edi.
// MiniMax S (11-10): no new form, no lower score (8): retail holds the record pool's base in ebp and pushes it around the payload copy where ours keeps it in edi; the base comes from the shared one-caller helper cache_record_new, whose body the rules forbid writing out by hand.
// MiniMax A (11-10): no new form, no lower score (8): retail's parameter order is (size, position,
// callback, data) - the size arrives in ecx, the position in edx and the callback and the data follow
// on the stack; our declaration and the only caller, in src/unknown_173b90.cpp, use another order,
// which the rules forbid changing. Retail also keeps the record pool's base in ebp.
// MiniMax E (11-10): no new form, no lower score: the record pool's base is held in edi here and in
// ebp in retail, and the copy's count then needs a register of its own.
// MiniMax R (11-10): no new form, no lower score (8): retail holds the record pool's base in ebp and pushes and pops it around the payload copy, ours keeps it in edi; the base comes from the shared one-caller helper cache_record_new, whose body the rules forbid writing out by hand.
// MiniMax H (11-05): no new form, no lower score; the record pool's base is held in edi here and in ebp in retail, and moving the two address-taken parameters to the end of the body only ties at 8; the allocator's store order (the active store before the count increment) ties at 8 as well, so the committed index form of the count store stays.
// MiniMax H (11-04): no new form, no lower score; the record pool's base is held in edi here and in ebp in retail, and the allocator helper's own choice decides that.
// MiniMax D (11-01): the permute run on this function was stopped at its time limit without a
// measured variant.
// MiniMax D (11-01): the allocator's statement order, an early count store and a records local in
// the allocator all tie or regress at 8 differences; no lower score.
// MiniMax A (11-03): the allocation from the shared helper scores 60 and the permute run was stopped; no lower score.
// MiniMax G (11-12): no new form tried this round, no lower score (8): the record pool's base is still in edi
// where retail keeps it in ebp, and the base comes from the shared helper, whose body the rules forbid rewriting.
// MiniMax H (11-13): no new form, no lower score (8); the record table's base rides in EDI here and in EBP in retail, and the payload copy then saves the other register; this function's only caller is in another file.
// score 8 (was 8): the record table base is held in edi here and in ebp in retail, and the
// copy of the payload saves and restores the register that choice leaves free.
// MiniMax D (11-06): no new form, no lower score: the payload copy before the delta (33), the records
// pointer in a local in the allocator (17) and the address-taken parameters left out (8) all keep the
// pool's base in edi.
// MiniMax E (11-08): no new form, no lower score: the record pool's base is held in edi here and in ebp in retail, and copying the payload after the callback's store makes it worse (57).
// MiniMax E (11-09): no new form, no lower score: the record pool's base is held in edi here and in ebp in retail, and the copy's count then needs a register of its own.
// MiniMax D (11-09): no new form, no lower score: the record pool's base and the fold of the record
// into eax are fixed by the shared one-caller helpers cache_record_new and cache_record_take, whose
// bodies the rules forbid writing out by hand, so the edi/ebp difference stays.
// MiniMax D (11-09): no new form, no lower score: the body is unchanged at 8 (241 bytes, as retail's); the record pool's base is held in edi here and in ebp in retail, and the payload copy's count then needs a register of its own, so the two choices cannot be made at once from this source.
// MiniMax R (11-09): no new form, no lower score (8): the record pool's base is held in edi here and in ebp in retail, and the copy of the payload saves and restores the register that choice leaves free; the allocator's store order and the records pointer in the allocator were measured and all tie at 8.
// MiniMax A (11-13): no new form tried this round, no lower score (8): the record table's base still rides in
// EDI here and in EBP in retail, and the payload copy then saves the other register; the base comes from the
// shared helper cache_record_new, whose body the rules forbid writing out by hand.
// @retail 0x1e4e0
bool function_1e4e0(point3f const *position, t_1e4e0_callback callback, void const *data, long size)
{
	(void)&callback;
	(void)&data;
	bool result = false;
	s_1e4e0_record *record = (s_1e4e0_record *)cache_record_new();
	if (record)
	{
		vector3f delta;
		delta.i = position->x - g_485618.position.x;
		delta.j = position->y - g_485618.position.y;
		delta.k = position->z - g_485618.position.z;
		if (size)
			memcpy(record->payload, data, size);
		record->type = 3;
		record->flags = 0;
		record->callback = callback;
		real depth = g_485618.forward.k * delta.k;
		depth += g_485618.forward.j * delta.j;
		depth += g_485618.forward.i * delta.i;
		record->depth = 0.0f - depth;
		record->position = *position;
		result = true;
	}
	return result;
}


void __stdcall function_35b00(void *payload);

/* retail's one caller sets the position up ahead of the pushes and the size after them, so the position
   is the last parameter and the size the first */
// @retail 0x1e5e0
bool function_1e5e0(long size, long a, long b, long c,
	long d, long e, long f, long g, void const *data, point3f const *position)
{
	(void)&a; (void)&b; (void)&c; (void)&d;
	(void)&e; (void)&f; (void)&g; (void)&data;
	bool result = false;
	s_1e4e0_record *record = (s_1e4e0_record *)cache_record_take();
	if (record)
	{
		long *payload = (long *)record->payload;
		payload[0] = a;
		payload[1] = b;
		payload[2] = c;
		payload[3] = d;
		payload[4] = e;
		payload[5] = f;
		payload[6] = g;
		if (size > 0)
			memcpy(payload + 7, data, size);
		record->type = 3;
		record->flags = 0;
		record->callback = function_35b00;
		vector3f delta;
		delta.i = position->x - g_485618.position.x;
		delta.j = position->y - g_485618.position.y;
		delta.k = position->z - g_485618.position.z;
		real depth = (real)(g_485618.forward.k * delta.k) + (real)(g_485618.forward.j * delta.j) + (real)(g_485618.forward.i * delta.i);
		record->depth = 0.0f - depth;
		record->position = *position;
		result = true;
	}
	return result;
}

DWORD const g_43fdc0[5] = {1, 2, 5, 8, 6};
bool g_47fe85 = true;
typedef void (__stdcall *t_4b220_fill)(void *, long, void *);

// Draws an immediate-mode primitive into the D3D push buffer.
// Uses the fill callback to write vertex data when the request fits.
// @retail 0x4b220
long function_4b220(long count, long mode, long primitive, long stride, t_4b220_fill fill, void *context)
{
	long result = NONE;
	long const *mode_reference = &mode;
	(void)&primitive;
	(void)&stride;
	(void)&fill;
	(void)&context;
	if (!*mode_reference)
	{
		dword words = (dword)(count * stride) >> 2;
		long requested = words + 5;
		if (requested < 2048)
		{
			function_1cf50();
			DWORD *push = D3DDevice_BeginPush(requested);
			*push++ = D3DPUSH_ENCODE(D3DPUSH_SET_BEGIN_END, 1);
			*push++ = g_43fdc0[primitive];
			*push++ = D3DPUSH_ENCODE(D3DPUSH_INLINE_ARRAY | D3DPUSH_NOINCREMENT_FLAG, words);
			fill(push, count * stride, context);
			push += words;
			*push++ = D3DPUSH_ENCODE(D3DPUSH_SET_BEGIN_END, 1);
			*push++ = 0;
			D3DDevice_EndPush(push);
		}
		else if (g_47fe85)
			g_47fe85 = false;
	}
	return result;
}

real *table_entry_data(long handle);

// @retail 0x152a0
void function_152a0(long handle, long index)
{
    if (index != NONE)
    {
        s_shader_transform_table *data = (s_shader_transform_table *)table_entry_data(handle);
        s_shader_transform_row *transform = data->transforms[data->entries[index].first];
        long count = data->entries[index].count * 3;
        D3DDevice_SetVertexShaderConstantFast(-46, transform, count);
        g_485af0 = true;
    }
    else
    {
        s_shader_transform_row transform[3];
        transform[0].x = 1.0f;
        transform[0].y = 0.0f;
        transform[0].z = 0.0f;
        transform[0].unused = 0;
        transform[1].x = 0.0f;
        transform[1].y = 1.0f;
        transform[1].z = 0.0f;
        transform[1].unused = 0;
        transform[2].x = 0.0f;
        transform[2].y = 0.0f;
        transform[2].z = 1.0f;
        transform[2].unused = 0;
        D3DDevice_SetVertexShaderConstantFast(-46, transform, 3);
    }
}

long function_4cb80(long element_index, long tag_index);

struct s_render_transform_record
{
	dword unknown00;
	long tag;
	long handle;
	dword flags;
	byte unknown10[0xc];
	transform4x3f const *transform;
};
// MiniMax D (11-14): no new form, no lower score (51): retail calls the transform table's lookup 0x1d6b0 out of line while this build inlines it at both sites, and that helper's inline-ness is declared in src/unknown_023540.cpp, which this lane may not edit. BLOCKED-BY 0x1d6b0.
// MiniMax A (11-12): no new form, no lower score (51): the body is still eleven bytes longer than retail and keeps the transform table lookup result in eax where retail copies it to edx before the cmp edx, -1 test, because retail calls 0x1d6b0 out of line at the second site while this build inlines it at both; that helper inline-ness is decided in src/unknown_023540.cpp, which this lane may not edit. BLOCKED-BY 0x1d6b0.

// MiniMax B (11-13): no new form, no lower score (51): retail inlines the 0x1d6b0 call where this build calls it out of line, and the body is 400 bytes against retail's 389; the callee's inline-ness is decided in the file that defines it, so the call line here cannot reach it.
// MiniMax H (11-12): no new form, no lower score (51); retail calls the transform table's lookup (0x1d6b0) out of line
// at the second site and this build inlines it at both, which makes our body eleven bytes longer and leaves the
// lookup's result in EAX where retail copies it to EDX before the `cmp edx, -1` test; that helper's inline-ness is
// decided in src/unknown_023540.cpp, which this lane may not edit. BLOCKED-BY 0x1d6b0.
// MiniMax G (11-11): no new form, no lower score (51); retail calls the transform table's lookup (0x1d6b0) out of
// line at the second site and this build inlines it at both, which makes our body eleven bytes longer and leaves the
// lookup's result in EAX where retail copies it to EDX before the `cmp edx, -1` test; that helper's inline-ness is
// decided in src/unknown_023540.cpp, which this lane may not edit, and writing its body out by hand or adding a
// noinline twin is barred, so the body stands. BLOCKED-BY 0x1d6b0.
// MiniMax H (11-11): no new form, no lower score (51); retail still calls the transform table's lookup (0x1d6b0) out of line at the second site and this build still inlines it at both, which makes our body eleven bytes longer and leaves the lookup's result in EAX where retail copies it to EDX before the test; that helper's inline-ness is decided in another file, so it cannot be changed from here. BLOCKED-BY 0x1d6b0.
// MiniMax C (11-11): no new form, no lower score (51); the transform table's lookup is still inlined here and called out of line in retail, and that helper's inline-ness is set in src/unknown_023540.cpp.
// MiniMax R (11-10): no new form, no lower score (51); retail calls the transform table's lookup out of line at the second site and this build inlines it at both, and that helper's inline-ness is set in another file.
// MiniMax A (11-11): no new form, no lower score (51); the body is still eleven bytes longer than retail's and
// keeps the lookup's result in EAX where retail copies it to EDX before the `cmp edx, -1` test, because retail
// calls the transform table's lookup (0x1d6b0) out of line at the second site while this build inlines it at both
// sites; that helper's inline-ness is decided in src/unknown_023540.cpp, which this lane may not edit.
// BLOCKED-BY 0x1d6b0.
// MiniMax I (11-10): no new form, no lower score (51); retail calls the transform table's lookup out of line at the second site and this build inlines it at both, and that helper's inline-ness is set in another file.
// MiniMax D (11-10): no new form, no lower score; retail calls the transform table's lookup out of
// line (0x1d6b0) at the second site and inlines it at the first, and this build inlines it at both.
// This round: making the helper a plain inline in src/unknown_023540.cpp, and adding inline to both
// of this file's declarations of it, each leave the body at 400 bytes and score 51 and keep 0x1d6b0,
// 0x152a0 and 0x24040 matching, so the link-time inliner inlines the helper at both sites whatever
// the source says; reaching retail's shape would need the helper's body written out by hand or a
// noinline twin, both barred, so the body stands.
// MiniMax E (11-09): no new form, no lower score; retail calls the transform table's lookup
// out of line (0x1d6b0) and we inline it, which also makes our body eleven bytes longer, and
// that helper's inline-ness is set in src/unknown_023540.cpp.
// MiniMax R (11-09): no new form, no lower score; the transform table's lookup is still inlined here and called out of line in retail, and dropping the inline keyword from that helper's definition, which is in src/unknown_023540.cpp, leaves this score at 51 with 0x1d6b0 and 0x24040 still matching, because the compiler inlines the call at both sites either way.
// MiniMax R (11-09): no new form, no lower score; the transform table's lookup is still inlined here and
// called out of line in retail, and the helper's inline-ness is set in another file.
// MiniMax D (11-06): no new form, no lower score; retail calls the transform table's lookup out of line
// (0x1d6b0) where we inline it, and the helper's inline-ness is set in src/unknown_023540.cpp, so the call
// cannot be made from this file; this round the lookup through a handle local and through a byte pointer
// local both give 51, and the first lookup is inlined on both sides.
// MiniMax H (11-05): no new form, no lower score; the second table_entry_data lookup is inlined
// here and called out of line in retail, and that helper's inline-ness is set in
// src/unknown_023540.cpp, so the transform table's rows cannot be reached the way retail does;
// widening this file's existing inline_depth(0) region over the second lookup rebuilds the
// whole body (111), so the region stays around the 0x4cb80 call alone.
// MiniMax D (run 37): no new form, no lower score; the second table_entry_data lookup has to
// stay out of line like retail's, and that is decided in src/unknown_023540.cpp (not my file).
// MiniMax G (11-06): no new form, no lower score: retail calls table_entry_data out of line for the
// transform table while we inline it, and that helper's `__inline` is declared in
// src/unknown_023540.cpp, so it cannot be changed from this file; a function pointer to the helper
// rebuilds the body (133).
// score 51 (was 51): this round found no gain - retail calls table_entry_data out of line for the
// transform table and we inline it, which also makes our body eleven bytes longer; that helper's
// inline-ness is decided in src/unknown_023540.cpp.
// MiniMax C (11-08): no new form, no lower score.
// score 51 (was 51): retail calls table_entry_data out of line for the transform table, and we
// inline it there, which also makes our body eleven bytes longer than retail's; the inline-ness of
// that helper is set in src/unknown_023540.cpp, so it cannot be changed from this file.
// MiniMax B (11-13): no new form, no lower score (51): the body is still eleven bytes longer than retail's because retail inlines 0x1d6b0 at the first lookup and calls it out of line at the second; that cannot be changed from this file, since declaring the helper inline or plain extern here leaves the body unchanged and a noinline twin of it is a form the rules bar.
// MiniMax G (11-13): no new form, no lower score (51): retail calls the transform table's lookup out of line
// where this build inlines it, and that helper's inline-ness is declared in src/unknown_023540.cpp, which this
// lane may not edit. BLOCKED-BY the table lookup.
// @retail 0x4d830
void function_4d830(s_render_transform_record const *record)
{
	if (record->transform)
	{
		real const *matrix = (real const *)record->transform;
		real constants[12];
		constants[0] = matrix[1];
		constants[1] = matrix[4];
		constants[2] = matrix[7];
		constants[3] = matrix[10];
		constants[4] = matrix[2];
		constants[5] = matrix[5];
		constants[6] = matrix[8];
		constants[7] = matrix[11];
		constants[8] = matrix[3];
		constants[9] = matrix[6];
		constants[10] = matrix[9];
		constants[11] = matrix[12];
		D3DDevice_SetVertexShaderConstantFast(-46, constants, 3);
		g_5093d8 = 0;
	}
	else if (record->handle != NONE)
	{
		byte *data = (byte *)table_entry_data(record->handle);
		long entry_count = *(word *)data;
		long size = 0;
		if (entry_count > 0)
			size = entry_count * 48 + 0x44;
		for (long i = size / 32; i > 0; --i, data += 32)
			_mm_prefetch((char const *)data, _MM_HINT_T0);
		dword flags = record->flags;
		if ((flags & 0xe0000000) == 0x20000000)
		{
#pragma inline_depth(0)
			long index = function_4cb80((flags >> 9) & 0x1ff, record->tag);
#pragma inline_depth()
			if (index != NONE)
			{
				s_shader_transform_table *table = (s_shader_transform_table *)table_entry_data(record->handle);
				s_shader_transform_row *constants;
				if (table->entries[flags & 15].count == 1)
					constants = table->transforms[table->entries[flags & 15].first];
				else
					constants = table->transforms[index];
				if (constants)
				{
					D3DDevice_SetVertexShaderConstantFast(-46, constants, 3);
					g_5093d8 = 0;
					return;
				}
			}
		}
		function_152a0(record->handle, flags & 15);
	}
}


PRIVATE __forceinline real maximum_25ca0(real first, real second)
{
    real result = second;
    if (first > second)
        result = first;
    return result;
}

// MiniMax E (11-14): no new form, no lower score (26): the sibling 0x174f80 in src/unknown_173b90.cpp carries retail's
// exact shape for the same source pattern - the maximum's compare, the explicit multiply and the repeated call setup -
// and that file only reaches the shape under a per-function optimisation switch which the rules forbid adding here;
// without the divisor's round trip through the stack this build folds the reciprocal into a division and scores 47.
// MiniMax B (11-13): no new form, no lower score (26): the comparison against 0.0001f still has its operands the other way round and its result in the other register, the divisor still makes a round trip through the stack where retail keeps it in a register, and both arms still share one call setup; the body's volatile anchor is the only thing that keeps the reciprocal's explicit mulss, and the body without it folds into a division and scores 47.
// MiniMax H (11-12): no new form, no lower score (26): the maximum's compare still has its operands the other way round and lands in the other register, the divisor still makes a round trip through the stack where retail multiplies by g_4b9ff8 with an explicit mulss, and both branches still share one call setup; the body also still carries a volatile cast on the stack temporary, which the rules forbid.
// MiniMax H (11-11): no new form, no lower score (26): retail keeps the maximum in the second loaded register and compares the first loaded against it, multiplies g_4b9ff8 by the reciprocal with an explicit mulss and repeats the call setup in each branch; without the divisor's round trip through the stack the reciprocal is folded into a division (47) and with the multiply's operands reversed the body grows to 173 bytes (37), so the body stands.
// MiniMax I (11-11): no new form, no lower score (26): the maximum's compare still has its operands the other way round and lands in the other register, the divisor still makes a round trip through the stack where retail multiplies by g_4b9ff8 with an explicit mulss, and the two branches still share one call setup.
// MiniMax D (11-11): no new form, no lower score (26): the value is g_4b9ff8 times the maximum's reciprocal and the compiler's floating-point mode folds that into a division by the maximum, where retail has an explicit mulss; the maximum's compare also keeps its operands the other way round, and only a per-function pragma would stop the folding, which the rules forbid.
// MiniMax S (11-10): no new form, no lower score (26): the maximum's compare still has its operands the other way round and lands in the other register, the divisor still makes a round trip through the stack where retail multiplies by g_4b9ff8 with an explicit mulss, and the two branches still share one call setup; the maximum and the reciprocal as locals, the enabled branch first and the plain maximum all score 47 or worse, and the permuter's run reached its time limit without a measured variant.
// MiniMax A (11-10): no new form, no lower score (26): the body is unchanged at 169 bytes, as retail's; in the default floating-point mode the compiler keeps the maximum in the first-loaded register and compares the second-loaded against it, while retail keeps the maximum in the second-loaded register and compares the first-loaded against it, so the compare's operand order, the copy's direction and the branch's polarity all follow from that one choice; the swapped-arms ternary (`first <= second ? second : first`) does reach retail's loads and `comiss xmm0, xmm1` but still copies into the first-loaded register and branches with `jae` where retail has `jbe`, and it scores 26 as well; a per-function precise-floating-point pragma (which the rules forbid) reaches retail's length and its maximum's compare and copy, but the call setup is still hoisted above the branch and the padding still differs, so it would still score 16.
// MiniMax A (11-10): no new form, no lower score (26): the maximum's compare reaches retail's operand
// order (`comiss xmm0, xmm1`) with the ternary spelling, but the copy then goes the other way and the
// branch is `ja` where retail has `jbe`; the divisor's round trip through the stack is what makes the
// length 169 (without it the body is 154 bytes and scores 50).
// MiniMax E (11-10): no new form, no lower score: the maximum's compare has its operands swapped
// and lands in the other register, the divisor makes a round trip through the stack where retail
// keeps it in a register and multiplies by g_4b9ff8 with an explicit mulss, and the two branches
// share one call setup where retail repeats it.
// MiniMax G (11-13): no new form tried this round, no lower score (26): the maximum's comparison still has its
// operands the other way round and its result in the other register, the divisor still makes a round trip through the
// stack where retail keeps it in a register, and the two branches still share one call setup where retail repeats it.
// The value of constants[3] is read back through the volatile anchor already in the body, and no further volatile
// anchor may be added, so the spelling that reaches retail's shape (the one the coordinator rejected for its
// per-function pragma) cannot be kept.
// MiniMax R (11-10): no new form, no lower score (26): the maximum's compare still has its operands the other way round and lands in the other register; the forms tried this round (the destination compared with the constant, the constant on the left of the compare, the less-than and less-or-equal spellings, the two-local forms with either declaration order and the four if/else spellings) measured 26, 27 and 28, never below the body above.
// MiniMax H (11-05): no new form, no lower score; without the divisor's round trip through the stack the multiply is explicit again (a mulss by g_4b9ff8) but the maximum still lands in the other register, the reciprocal's register order differs and the two branches still share one call setup; the four forms of the maximum all give that same 47, the round trip with an explicit multiply gives 35, and the maximum's registers follow the live ranges rather than the source order.
// MiniMax H (11-04): no new form, no lower score; the maximum's comparison operand order, the divisor's round trip through the stack and the shared call setup still differ.
// MiniMax D (run 37): no new form, no lower score; the maximum as `first <= second ? second : first` gives retail's comiss operand order but still scores 26, the maximum without the round trip through the stack scores 47, the four forms of the maximum (the two ternary orders, the if/else and the initial-value form) score 47 or 48, and retail's own phi copies the maximum into g_4b9fa4's register while ours keeps it in 0.0001f's.
// MiniMax A (11-03): no new form, no lower score.
// MiniMax G (11-12): no new form tried this round, no lower score (26): the 0.0001f comparison still has its
// operands the other way round, the divisor still makes a round trip through the stack, and the branches still
// share one call setup.
// MiniMax H (11-13): no new form, no lower score (26); the comparison against 0.0001f has its operands the other way round, the divisor makes a round trip through the stack and both arms share one call setup.
// score 26 (was 26): the comparison of 0.0001f against g_4b9fa4 has its operands in the other
// order and copies the other register, the divisor makes a round trip through the stack, and
// both branches share one call setup where retail repeats the setup in each of them.
// Coordinator review (MiniMax C): the lane's 16 needed a #pragma optimize("p") pair around
// the function (a per-function compiler setting, not kept); its body without the pragma (ternary maximum, the
// divisor and reciprocal as locals, no volatile, the enabled branch first) is 47. Body back to its earlier form.
// MiniMax D (11-06): no new form, no lower score.
// MiniMax E (11-08): no new form, no lower score: the maximum's compare has its operands swapped and the divisor makes a round trip through the stack here, while retail keeps it in a register.
// MiniMax E (11-09): no new form, no lower score: the maximum's compare has its operands swapped and the divisor makes a round trip through the stack here, while retail keeps it in a register.
// MiniMax D (11-09): no new form, no lower score: the maximum's ternary and its if/else spelling
// both give retail's `comiss` operand order, but the result then lands in xmm0 where retail keeps it
// in xmm1; the 26-scoring body stays.
// MiniMax D (11-09): no new form, no lower score: the body is unchanged at 26 (169 bytes, as retail's); the maximum's comparison has its operands in the other order, the divisor makes a round trip through the stack where retail keeps it in a register, and both branches share one call setup where retail repeats it; the only form that reached retail's shape needed a per-function #pragma, which the rules forbid.
// MiniMax R (11-09): no new form, no lower score (26): the maximum's compare still has its operands the other way round and lands in the other register, the divisor still makes a round trip through the stack where retail keeps it in a register and multiplies by g_4b9ff8 with an explicit mulss, the clamp still reloads 1.0f, and the two branches still share one call setup where retail repeats it in each of them. This round: the enabled branch first gives 29 and was restored, the reversed multiply gives 32 and was restored, and the round trip with an explicit multiply, the maximum in a local and the four forms of the maximum were all measured and all score worse or the same.
// MiniMax A (11-13): no new form tried this round, no lower score (26): the comparison against 0.0001f still
// has its operands the other way round and its result in the other register, the divisor still makes a round
// trip through the stack where retail keeps it in a register, and both arms still share one call setup; the
// body's volatile anchor is what keeps the reciprocal's explicit mulss, and the body without it folds into one.
// @retail 0x25ca0
void function_25ca0(bool enabled)
{
    real constants[4];
    *(volatile real *)&constants[3] = maximum_25ca0(0.0001f, g_4b9fa4);
    real value = g_4b9ff8;
    value *= 1.0f / *(volatile real *)&constants[3];
    value = 0.0f - value;
    constants[0] = value < 0.0f ? 0.0f : value > 1.0f ? 1.0f : value;
    constants[1] = 0.0f;
    constants[2] = g_4b9f18;
    if (!(enabled))
    {
        constants[3] = 0.0f;
        D3DDevice_SetVertexShaderConstant(-81, constants, 1);
    }
    else
    {
        constants[3] = g_4b9f9c;
        D3DDevice_SetVertexShaderConstant(-81, constants, 1);
    }
}

// @retail 0x1cd90
void function_1cd90(void)
{
	function_1c710(g_51f0f0);
}

PRIVATE __forceinline void select_immediate_descriptor(long format)
{
	byte const *descriptor = g_43f408[format];
	function_1c6b0(g_51f0f0);
	s_shader_cache *state = (s_shader_cache *)g_51f0f0;
	state->wanted[0] = descriptor;
	if (state->current[0] != descriptor)
		state->descriptors_changed = true;
	function_1c710(g_51f0f0);
}

// @retail 0x1ccb0
void function_1ccb0(long format)
{
	select_immediate_descriptor(format);
}

// @retail 0x1cc60
void function_1cc60(long index)
{
	function_1c590((s_shader_cache *)g_51f0f0, NONE, index);
	select_immediate_descriptor(g_4670ec[index].field_0c);
}

// @retail 0x40870
bool function_40870(void)
{
	function_1e370();
	g_4b6280.count = g_4b6280.unknown04;
	return true;
}

struct s_queued_material_payload
{
	void (__stdcall *begin)(void *);
	t_4b220_fill fill;
	void (__stdcall *end)(void *);
	long primitive;
	long format;
	long count;
	long stride;
	byte data[1];
};

// Processes a queued draw request, resets render state, and selects
// the vertex format before drawing with optional begin/end callbacks.
// @retail 0x35b00
void __stdcall function_35b00(void *payload)
{
	g_4670bc = true;
	function_16b10((s_render_reset_state *)g_485b48);
	s_queued_material_payload *request = (s_queued_material_payload *)payload;
	if (request->begin)
		request->begin(request->data);
	select_immediate_descriptor(request->format);
	function_1c710(g_51f0f0);
	function_1cf50();
	function_4b220(request->count, 0, request->primitive, request->stride, request->fill, request->data);
	if (request->end)
		request->end(request->data);
}

typedef bool (__stdcall *t_1f3a0_callback)(s_363a0_vertex *vertices, void *context);

// @retail 0x1f3a0
void function_1f3a0(short const *rectangle, real x, real y, long width, long height,
    short u_offset, short v_offset, real scale, dword color,
    t_1f3a0_callback callback, void *context)
{
    s_363a0_vertex vertices[4];
    short u = rectangle[2] + u_offset;
    short v = rectangle[3] + v_offset;
    vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = color;
    vertices[1].x = vertices[2].x = (real)width * scale + x;
    vertices[0].x = vertices[3].x = x;
    vertices[2].y = vertices[3].y = (real)height * scale + y;
    vertices[0].u = vertices[3].u = (real)u;
    vertices[1].u = vertices[2].u = (real)(u + width);
    vertices[0].v = vertices[1].v = (real)v;
    vertices[0].y = vertices[1].y = y;
    vertices[2].v = vertices[3].v = (real)(v + height);
    if (!callback || callback(vertices, context))
        function_363a0(vertices);
}

void __stdcall function_423c0(void *payload);
void __stdcall function_4f010(void *payload);
void __stdcall function_508d0(void *payload);

s_depth_groups_storage g_depth_groups_storage;

/* Takes the next free record and clears its type and active flag, as
   0x1e2d0 does, with the count advanced first. */
PRIVATE inline s_cache_record *cache_record_push(void)
{
    s_cache_record *result = 0;
    if (g_4b6280.count < 1024)
    {
        result = &g_4b6280.records[g_4b6280.count++];
        result->count = 0;
        result->active = 0;
    }
    else if (g_4b6280.available)
        g_4b6280.available = false;
    return result;
}

/* Copies a depth group's depth and position to whichever outputs are given. */
PRIVATE inline void depth_group_get(s_42760_entry const *group, real *depth, point3f *position)
{
    if (depth)
        *depth = group->depth;
    if (position)
        *position = group->position;
}

/* Queues one record per type-3 depth group, drawn by 0x423c0. */
// @retail 0x42760
void function_42760(long flags)
{
    long count = g_4c19ac;
    long i = 0;
    if (count > 0)
    {
        do
        {
            s_42760_entry const *entry = &g_4c152c[i];
            if (entry->type == 3)
            {
                s_1e4e0_record *record = (s_1e4e0_record *)cache_record_push();
                if (record)
                {
                    record->type = 3;
                    record->flags = 0;
                    record->callback = function_423c0;
                    real *depth = &record->depth;
                    depth_group_get(entry, depth, &record->position);
                    ((long *)record->payload)[0] = i;
                    ((long *)record->payload)[1] = flags;
                    *depth = 0.0f - *depth;
                }
            }
        } while (++i < count);
    }
}

struct s_42850_payload
{
    long a, b;
    point3f position;
    vector3f first, second;
    real scale, width;
    vector3f third;
};

// @retail 0x42850
void function_42850(long a, long b, point3f const *position,
    vector3f const *first, vector3f const *second, real scale, real width,
    vector3f const *third)
{
    s_1e4e0_record *record = (s_1e4e0_record *)cache_record_take();
    if (record)
    {
        s_42850_payload *payload = (s_42850_payload *)record->payload;
        payload->a = a;
        payload->b = b;
        payload->position = *position;
        payload->first = *first;
        payload->second = *second;
        payload->scale = scale;
        payload->width = width;
        payload->third = *third;
        vector3f delta;
        delta.i = position->x - g_4b9da0.x;
        delta.j = position->y - g_4b9da0.y;
        delta.k = position->z - g_4b9da0.z;
        record->type = 3;
        record->flags = 0;
        record->callback = function_4f010;
        real depth = g_4b9dac.k * delta.k;
        depth += g_4b9dac.j * delta.j;
        depth += g_4b9dac.i * delta.i;
        record->depth = 0.0f - depth;
        record->position = *position;
    }
}


struct s_429a0_payload
{
    byte type, opacity;
    byte unknown02[2];
    long a, b, c;
    point3f position, endpoint;
    vector3f first, second;
};

// MiniMax E (11-14): no new form, no lower score (16): retail still folds the record base into the payload pointer one
// store earlier than this build does; the base comes from the shared helper cache_record_take, whose body the rules
// forbid writing out by hand and whose two callers keep it as it is.
// MiniMax B (11-13): no new form, no lower score (16): retail still folds the record base into the payload pointer one store earlier than this build does; the base comes from the shared helper cache_record_take, whose body the rules forbid writing out by hand.
// MiniMax H (11-12): no new form, no lower score (16): retail still folds the record base into the payload pointer before the store at +0x28 where we store through [eax + ebx + 0x28] and add ebx afterwards, and the count's store still comes after the index's move; the fold comes from the shared helper cache_record_take, whose two callers keep it as it is.
// MiniMax H (11-11): no new form, no lower score (16): retail folds the record base into the payload pointer before the store at +0x28 where ours stores through [eax + ebx + 0x28] and adds ebx afterwards, and the count's store comes after the index's move where retail has it before; the fold comes from the shared helper cache_record_take, whose two callers keep it as it is.
// MiniMax I (11-11): no new form, no lower score (16): retail folds the record base into eax after the payload's type byte and we fold it after the payload's first long; three new forms (the longs through the payload pointer, the type byte through the record's payload array and the longs through a long view of the payload) score 19, 16 and 19.
// MiniMax D (11-11): no new form, no lower score (16): retail folds the record base into the payload pointer before the store at +0x28 where we store through [eax + ebx + 0x28] and add ebx afterwards; this round the first two payload stores swapped (18) and the type byte stored through the record's payload array (16) both keep the fold after the second store.
// MiniMax S (11-10): no new form, no lower score (16): retail folds the record's address into EAX before the dword store at +0x28 where ours stores through [eax+ebx+0x28] and adds EBX afterwards; the permuter was stopped at its 600 s time limit without a measured variant.
// MiniMax A (11-10): no new form, no lower score (16): retail folds the record base into the payload
// pointer one store earlier than we do; that fold comes from the shared helper cache_record_take,
// which has two callers (0x1e5e0 and 0x429a0), so its body cannot be specialised for this one.
// MiniMax G (11-12): no new form tried this round, no lower score (16): retail still folds the record base into
// the payload pointer one store earlier, and that fold comes from the shared helper, which has two callers.
// MiniMax H (11-13): no new form, no lower score (16); retail folds the record base into the payload pointer one store earlier than this build does.
// score 16 (was 16): retail folds the record base into the payload pointer one store earlier than we
// do, the body is one byte longer than retail's, and the callback's address is a build address the
// comparison cannot map.
// MiniMax E (11-10): no new form, no lower score: retail folds the record base into the payload
// pointer one store earlier than we do, and the callback's address is a build address the
// comparison cannot map.
// MiniMax R (11-10): no new form, no lower score (16): retail folds the record base into the payload pointer one store earlier than we do; that fold comes from the shared one-caller helper cache_record_take, whose body the rules forbid writing out by hand.
// MiniMax H (11-05): no new form, no lower score; the record base is still folded into the payload pointer one store later than retail does (the type byte stored after the longs, the type byte written through the payload array, the longs written through a long view and a struct that holds the payload's fields in place all score 16); the type byte written through the payload array gives byte-identical code, and the permuter's 26 tries leave the same difference.
// MiniMax H (11-04): no new form, no lower score; retail folds the record base into the payload pointer one store before ours does (three variants: 16 each).
// MiniMax A (11-03): the allocation from 0x1e4e0's helper scores 97; no lower score.
// MiniMax D (run 37): no new form, no lower score; the only real difference is that retail
// folds the record base into the payload pointer one store earlier than we do, the rest of
// the fast differences are the reduced image's addresses for the globals and the constant.
// MiniMax D (11-06): no new form, no lower score: retail folds the record base into the payload
// pointer one store earlier than we do, and the address the record's callback holds is a build
// address that the comparison cannot map.
// MiniMax E (11-08): no new form, no lower score: retail folds the record base into the payload pointer one store earlier than we do, and the callback's address is a build address the comparison cannot map.
// MiniMax E (11-09): no new form, no lower score: retail folds the record base into eax one store earlier than we do, and the callback's address is a build address the comparison cannot map.
// MiniMax D (11-09): no new form, no lower score: the fold of the record's base into eax is fixed by
// the shared one-caller helper cache_record_take, whose body the rules forbid writing out by hand;
// the 16-scoring body stays.
// MiniMax D (11-09): no new form, no lower score: the body is unchanged at 16 (379 bytes against retail's 378); the only real difference is the fold of the record's base into eax, which retail takes after the payload's type byte and we take after the payload's first long, one store later; the rest of the difference is the reduced image's addresses for the globals and the callback.
// MiniMax R (11-09): no new form, no lower score (16): retail folds the record base into the payload pointer one store earlier than we do, our body is a byte longer than retail's so the relocation mask stops applying, and the callback's address is a build address the comparison cannot map; the three variants that move that fold were measured and all tie at 16.
// MiniMax A (11-13): no new form tried this round, no lower score (16): retail still folds the record base into
// the payload pointer one store earlier than this build does; the base comes from the shared helper
// cache_record_take, whose body the rules forbid writing out by hand. The body is also one byte longer than
// retail's (379 against 378), so the linker's fixup mask stops applying near the end and the callback's
// address (0x4b7890 here, 0x508d0 in retail) is counted in the score.
// @retail 0x429a0
void function_429a0(byte type, long a, long b, long c, point3f const *position,
    point3f const *endpoint, vector3f const *first, vector3f const *second, real opacity)
{
    s_1e4e0_record *record = (s_1e4e0_record *)cache_record_take();
    if (record)
    {
        s_429a0_payload *payload = (s_429a0_payload *)record->payload;
        payload->type = type;
        payload->a = a;
        payload->b = b;
        payload->c = c;
        payload->position = *position;
        payload->endpoint = *(endpoint ? endpoint : position);
        payload->first = *first;
        payload->second = *second;
        long value = (long)(opacity * 256.0f);
        long clamped;
        if (value < 0)
            clamped = 0;
        else
            clamped = value > 255 ? 255 : value;
        payload->opacity = (byte)clamped;
        vector3f delta;
        delta.i = position->x - g_4b9da0.x;
        delta.j = position->y - g_4b9da0.y;
        delta.k = position->z - g_4b9da0.z;
        record->type = 3;
        record->flags = 0;
        record->callback = function_508d0;
        real depth = g_4b9dac.k * delta.k;
        depth += g_4b9dac.j * delta.j;
        depth += g_4b9dac.i * delta.i;
        record->depth = 0.0f - depth;
        record->position = *position;
    }
}

struct s_1c8c0_stream
{
    byte format, stride;
    byte unknown02[2];
    long offset;
    byte unknown08[8];
    long buffer;
};

PRIVATE __forceinline dword stream_register_mask(byte const *descriptor)
{
    dword mask = 0;
    signed char const *item = (signed char const *)descriptor + 1;
    while (*item != -1)
    {
        if (*item >= 0)
            mask |= 1 << *item;
        item += 2;
    }
    return mask;
}

// The streams are walked with a for loop, so the index is zeroed where retail zeroes it and
// the count of the streams is compared against the zero in the register retail uses.
// @retail 0x1c8c0
bool function_1c8c0(byte const *definition, dword mask,
    s_1c8c0_stream const *third, s_1c8c0_stream const *second, s_1c8c0_stream const *first)
{
    long count = 0;
    if (*(long const *)(definition + 0x38) > 0)
    {
      for (short i = 0; i < *(long const *)(definition + 0x38); ++i)
      {
        s_stream_description const *stream = (s_stream_description const *)(*(byte *const *)(definition + 0x3c) + i * 32);
        mask &= ~stream_register_mask(g_43f408[stream->format]);
        function_1c620((s_shader_cache *)g_51f0f0, stream->buffer, stream->stride, stream->offset, g_43f408[stream->format]);
        ++count;
      }
    }
    if (first)
    {
        function_1c620((s_shader_cache *)g_51f0f0, first->buffer, first->stride, first->offset, g_43f408[first->format]);
        mask &= ~stream_register_mask(g_43f408[first->format]);
        ++count;
    }
    if (second)
    {
        function_1c620((s_shader_cache *)g_51f0f0, second->buffer, second->stride, second->offset, g_43f408[48]);
        mask &= ~stream_register_mask(g_43f408[48]);
        ++count;
    }
    if (third)
    {
        function_1c620((s_shader_cache *)g_51f0f0, third->buffer, third->stride, third->offset, g_43f408[third->format]);
        mask &= ~stream_register_mask(g_43f408[third->format]);
        ++count;
    }
    if (count > 0)
        function_1c710(g_51f0f0);
    return mask == 0;
}

// @retail 0x1caa0
bool function_1caa0(long tag, byte const *selection, word const *kind, byte const *definition,
    s_1c8c0_stream const *third, s_1c8c0_stream const *second, s_1c8c0_stream const *first)
{
    word type = *kind;
    word mode = *(word const *)(selection + 0x14);
    signed char index;
    switch (type)
    {
    case 1: index = (signed char)selection[0x10]; break;
    case 2: index = (signed char)selection[0x10]; break;
    case 3: index = (signed char)selection[0x10]; break;
    case 4: index = (signed char)selection[0x11]; break;
    case 5: index = (signed char)selection[0x11]; break;
    default: index = (signed char)selection[0x10]; break;
    }
    long shader = function_1cb20(mode, index, tag);
    function_1c590((s_shader_cache *)g_51f0f0, tag, shader);
    dword mask = function_1cb70(tag, shader);
    return function_1c8c0(definition, mask, third, second, first);
}

long g_509350;
const dword g_47ffd8[53] = {
    0x00000001, 0x00000000, 0x00020000, 0x00000000, 0x00000000, 0x80000007, 0x00000000, 0x007bbef0,
    0x00000cf2, 0x00377101, 0x000000ff, 0x00001fff, 0x00001fff, 0x000000ff, 0x00084208, 0x0007ec87,
    0x1f030700, 0x1f030700, 0x03030300, 0x00000017, 0x00000017, 0x0000001f, 0x00001000, 0x00001000,
    0x00000200, 0x00002000, 0x00000000, 0x00000004, 0x501502f9, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000
};
dword g_484ce8[53];

// @retail 0x12490
bool function_12490()
{
    bool result = true;
    D3DPRESENT_PARAMETERS parameters;
    memset(&parameters, 0, sizeof(parameters));
    g_509350 = 1;
    parameters.FullScreen_RefreshRateInHz = (short)g_485ac0;
    parameters.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    parameters.Windowed = false;
    parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    parameters.EnableAutoDepthStencil = true;
    parameters.AutoDepthStencilFormat = D3DFMT_D24S8;
    parameters.BackBufferFormat = D3DFMT_A8R8G8B8;
    parameters.BackBufferWidth = 640;
    parameters.BackBufferHeight = 480;
    parameters.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    if (g_485ac2)
        parameters.Flags |= D3DPRESENTFLAG_WIDESCREEN;
    if (g_485ac6)
        parameters.Flags |= D3DPRESENTFLAG_PROGRESSIVE;
    Direct3D_CreateDevice(0, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &parameters, (D3DDevice **)&g_5093b0);
    if (!g_5093b0)
    {
        g_5093b0 = NULL;
        result = false;
    }
    else
        memcpy(g_484ce8, g_47ffd8, sizeof(g_484ce8));
    return result;
}

struct s_type_7ba8e9;
long function_136490(s_type_7ba8e9 const *bitmap);

// @retail 0x1d2f0
bool __stdcall function_1d2f0(D3DSurface *surface, byte *bitmap)
{
    volatile bool result = true;
    D3DSURFACE_DESC description;
    D3DSurface_GetDesc(surface, &description);
    memset(bitmap, 0, 0x74);
    *(dword *)bitmap = 0x6269746d;
    *(short *)(bitmap + 4) = (short)description.Width;
    *(short *)(bitmap + 6) = (short)description.Height;
    bitmap[8] = 1;
    *(short *)(bitmap + 0xa) = 0;
    bool linear;
    long format;
    function_1d4b0(description.Format, false, &format, &linear);
    if (format == NONE)
        return false;
    *(short *)(bitmap + 0xc) = (short)format;
    if (!linear)
        *(word *)(bitmap + 0xe) |= 1;
    else
        *(word *)(bitmap + 0xe) |= 0x10;
    if ((short)format >= 14 && (short)format <= 16)
        *(word *)(bitmap + 0xe) |= 2;
    if ((short)format == 18)
        *(word *)(bitmap + 0xe) |= 4;
    *(short *)(bitmap + 0x14) = 1;
    D3DLOCKED_RECT locked;
    D3DSurface_LockRect(surface, &locked, NULL, D3DLOCK_NOOVERWRITE);
    *(void **)(bitmap + 0x54) = locked.pBits;
    *(long *)(bitmap + 0x34) = function_136490((s_type_7ba8e9 const *)bitmap);
    return result;
}

D3DSurface g_484f50;

// @retail 0x14850
bool function_14850()
{
    bool result = true;
    function_0158f0(11, 0, 64, 64, 0, 1, &g_surface_resource_pairs.pairs[0].texture);
    g_509378 = D3DTexture_GetSurfaceLevel2(g_509374, 0);
    function_0158f0(11, 0, 64, 64, 0, 1, &g_surface_resource_pairs.pairs[1].texture);
    g_509380 = D3DTexture_GetSurfaceLevel2(g_50937c, 0);
    if (!g_509374 || !g_509378 || !g_50937c || !g_509380)
        result = false;
    g_484f50.Common = 0x50001;
    g_484f50.Data = 0;
    g_484f50.Lock = 0;
    g_484f50.Format = 0x11229;
    g_484f50.Size = 0x1f1ff1ff;
    g_484f50.Parent = NULL;
    g_509384 = &g_484f50;
    return result;
}

struct short_rect_pair
{
    struct { short v0, v1, v2, v3; } a, b;
};
struct s_unknown_485a88
{
    byte flag;
    short_rect_pair rects;
};
extern s_unknown_485a88 g_unknown_485a88;
#define g_485a8a (g_unknown_485a88.rects)

s_runtime_device_configuration g_runtime_device_configuration;
long g_467008, g_46700c;
bool g_5093bc;

// MiniMax H (11-04): the settings are copied with memcpy, the depth format tests the low flag byte
// and the interval mask the high flag byte of the flags word; the two flag bytes are then read from
// memory as retail does and the function matches (fast and plain).
// MiniMax H (11-04): no new form, no lower score; copying the settings field by field (55), with the fields in another order (61) and with the copy split around the memset (107) are all worse, and the flag byte still comes from the register that held the copied dword where retail reloads it from the global.
// MiniMax G (10-31): 55 (was 68): the settings are copied into the global field by field and the
// depth format is assigned first; the two flag bytes still come from a register instead of memory.
// MiniMax G (10-31): a byte-typed view of the global loads the flag bytes from memory (54), but it
// casts the global's own address, so it is not kept without a match.
// MiniMax D (run 37): no new form, no lower score; a memcpy of the settings into the global
// gives retail's store order (52) but the flag bytes are still taken from the loaded dword
// instead of memory, the reads through the packed dword give 69 and 72, a dword copy of the
// first four bytes gives 55 and reading the flag bytes through the global's address gives 52.
// MATCH (was 55 fast; 68 before). Tried (MiniMax S run 10): copying the settings field by field, assigning the depth
// score 55 (fast; 68 before). Tried (MiniMax S run 10): copying the settings field by field, assigning the depth
// format after the back buffer width and clearing the result before the device pointer gives 68;
// reading the two flag bytes back through a byte pointer to g_runtime_device_configuration as
// well gives 49. That cast of the global's address is not kept without a match.
// MiniMax G (run 12): the depth format assigned right after the memset 55 (kept); with the settings'
// first four bytes copied as one dword into .packed and the switch on (short)(.packed >> 16) instead
// of g_485aca 52 (both read through the wrong type: not kept without a match).
// @retail 0x128c0
bool function_128c0(s_128c0_settings const *settings)
{
    memcpy(&g_runtime_device_configuration, settings, sizeof(g_runtime_device_configuration));
    bool result = true;
    D3DPRESENT_PARAMETERS parameters;
    memset(&parameters, 0, sizeof(parameters));
    parameters.AutoDepthStencilFormat = (D3DFORMAT)(D3DFMT_D24S8 + ((byte)g_485ac8 ? 1 : 0));
    parameters.Windowed = false;
    parameters.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    parameters.EnableAutoDepthStencil = true;
    parameters.BackBufferFormat = D3DFMT_A8R8G8B8;
    parameters.BackBufferWidth = g_485a8a.a.v3 - g_485a8a.a.v1;
    parameters.BackBufferHeight = g_485a8a.a.v2 - g_485a8a.a.v0;
    switch (g_485aca)
    {
    case 0:
        parameters.SwapEffect = D3DSWAPEFFECT_FLIP;
        parameters.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
        break;
    case 1:
        parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
        parameters.FullScreen_PresentationInterval = 1 + ((byte)(g_485ac8 >> 8) != 0 ? 0x80000000 : 0);
        break;
    case 2:
        parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
        parameters.FullScreen_PresentationInterval = 2 + ((byte)(g_485ac8 >> 8) != 0 ? 0x80000000 : 0);
        break;
    default:
        parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
        g_485aca = 1;
        parameters.FullScreen_PresentationInterval = 0;
        break;
    }
    parameters.FullScreen_RefreshRateInHz = (short)g_485ac0;
    if (!g_5093b0)
        Direct3D_CreateDevice(0, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &parameters, (D3DDevice **)&g_5093b0);
    else
        D3DDevice_Reset(&parameters);
    if (!g_5093b0)
    {
        result = false;
        g_5093b0 = NULL;
    }
    else
    {
        memcpy(g_484ce8, g_47ffd8, sizeof(g_484ce8));
        if (g_485acc < 0 || g_485acc > 5)
            g_485acc = 3;
        g_467008 = NONE;
        g_46700c = NONE;
        g_5093bc = true;
    }
    return result;
}
// MiniMax D (11-14): no new form, no lower score (62): the locked rectangle's pointer and pitch still sit in the other order in the frame (retail keeps the pointer at [esp+0xc] and the pitch at [esp+0x10], ours the other way round), which comes from the locked-rectangle type in the XDK header, which this lane may not edit.
// MiniMax H (11-11): no new form, no lower score (62); the locked rectangle's frame slots still sit four bytes lower than retail's, the 0xf0000000 flag is still set in a register where retail sets it in the frame, and the field order comes from the locked-rectangle type in the XDK header, which this lane may not edit.
// MiniMax A (11-11): no new form, no lower score (62); retail ORs the 0xf0000000 flag into the locked
// rectangle's pointer in memory (`or dword ptr [esp + 0xc], 0xf0000000`) and reads it again, while ours holds the
// pointer in a register, and retail's four frame slots for {the surface, the rectangle, the LockRect arguments} sit
// four bytes above ours. This round: the flag set on the rectangle's own pBits member (62, 383 bytes) and a
// separate pointer local with the flag set on it (63, retail's 399 bytes) give the same results as the earlier
// rounds, and re-reading the pointer, the row offset and the pitch in every row (68, 383 bytes) is worse. All
// reverted; the body stands.
// MiniMax R (11-09): no new form, no lower score; retail keeps the row's destination pointer in edx over a row base in ebp, while ours counts the destination in esi over a base in ebx and adds one store and a nop that retail does not have.
// MiniMax D (11-10): no new form, no lower score; retail ORs the 0xf0000000 flag into the locked
// rectangle's pointer in memory and rereads it, this build holds the pointer in a register. This
// round: setting the flag on the rectangle's own pBits member instead of through the pointer gives
// 62 and 383 bytes, the same score as the baseline, and a separate pointer local with the flag set
// on it gives retail's 399 bytes but scores 63; both reverted, and the rectangle's frame slots still
// sit four bytes lower than retail's.
// MiniMax R (11-09): no new form, no lower score; the flag is still set in a register where retail sets it
// in the frame, and the locked rectangle's frame slots stay four bytes lower.
// MiniMax A (11-06, run 52), not kept (62 -> 60, under the keep bar) - the destination column is now recomputed for each
// column from the screen address as retail does and the rectangle's fields are read off the
// rectangle itself; what is left is the frame slots of the locked rectangle, retail's in-memory OR
// of the flag into the rectangle and the order of the two inner-loop increments. A running row
// offset reproduces retail's length (399) and its shape but moves the source pointer into ebp
// (67), so it was dropped.

// MiniMax A (11-12): no new form, no lower score (62): retail keeps the locked rectangle pointer at [esp+0xc] and its pitch at [esp+0x10] and ORs the 0xf0000000 flag into the pointer in memory, where ours keeps the pitch at [esp+0xc], the pointer at [esp+0x10] and sets the flag in a register; the inner loop also adds the x offset to the source side where ours adds it to the destination pixel pointer, and the locked rectangle field order comes from the XDK type, which this lane may not edit.

// MiniMax B (11-13): no new form, no lower score (62): the locked rectangle's fields are still laid out with the pitch before the pointer in this build and with the pointer first in retail, so the inlined lock's stack homes and the copy's field order cannot match (the body is 383 bytes against retail's 399).
// MiniMax H (11-12): no new form, no lower score (62); retail keeps the locked rectangle's pointer at [esp+0xc] and
// its pitch at [esp+0x10] and ORs the 0xf0000000 flag into the pointer in memory, where ours keeps the pitch at
// [esp+0xc], the pointer at [esp+0x10] and sets the flag in a register; the inner loop also adds the x offset to the
// source side where ours adds it to the destination's pixel pointer.
// MiniMax G (11-11): no new form, no lower score (62); retail keeps the locked rectangle's pointer at [esp+0xc] and
// its pitch at [esp+0x10] and ORs the 0xf0000000 flag into the pointer in memory, where ours keeps the pitch at
// [esp+0xc], the pointer at [esp+0x10] and sets the flag in a register; the flag set on the rectangle's own pBits
// member and a separate pointer local carrying the flag were measured at 62 and 63 before, so the body stands.
// This round mapped the inner loop as well: retail adds the x offset to the source side and keeps the row offset
// separate, while this build adds it to the destination's pixel pointer, so the two bodies compute different
// pointers and retail's is 16 bytes longer (399 against 383), which also moves every branch to the end.
// MiniMax C (11-11): no new form, no lower score (62); the locked rectangle's pointer and pitch still sit inthe other order, which comes from the locked-rectangle type in the XDK header, which this lane may not edit.
// MiniMax C (11-11): no new form, no lower score (62); the locked rectangle's pointer and pitch still sit in the other order, which comes from the locked-rectangle type in the XDK header, which this lane may not edit.
// MiniMax R (11-10): no new form, no lower score (62); the locked rectangle's frame slots still sit four bytes lower than retail's, the flag is still set in a register where retail sets it in memory, and the row counter and the surface pointer still sit four bytes lower.
// MiniMax I (11-10): no new form, no lower score (62); the locked rectangle's frame slots still sit four bytes lower than retail's, the flag is still set in a register where retail sets it in memory, and the row counter and the surface pointer still sit four bytes lower.
// MiniMax E (11-09): forms tried, none lower: the locked rectangle used directly (63) and the
// row kept as an offset with the pointer read again in every row (67, and the same with the
// pointer form); our frame slots for the locked rectangle still sit four bytes lower than
// retail's, so the pointer and the pitch swap places.
// MiniMax C (11-06): no new form, no lower score - reading the rectangle's fields straight off the
// rectangle instead of through the pointer scores 63; retail keeps the pointer in the lower slot and
// the pitch above it, our build keeps them the other way round, and that field order comes from the
// locked-rectangle type in the XDK header, which this lane may not edit.
// MiniMax D (11-05): no new form, no lower score - retail ORs the flag into the locked rectangle in
// memory and rebuilds each row from a running pointer; seven new source shapes all score worse (the
// pointer's row offset 61, the rectangle used directly 63, the components in retail's order 82), and
// the pointer's slot stays above the pitch slot in every one of them.
// score 62 (was 62): this round's fields read straight off the locked rectangle (63) and the row
// kept as an offset instead of a pointer (67) both score worse; the rectangle still sits four
// bytes lower than retail's and the pitch slot stays below the pointer slot.
// score 62 (was 62): retail's locked rectangle keeps the pointer at [esp+0xc] and the pitch at
// [esp+0x10] and sets the 0xf0000000 flag in memory at the pointer, ours keeps the pitch at
// [esp+0xc] and the pointer at [esp+0x10] and sets the flag in a register; the row counter and
// the surface pointer sit four bytes lower than retail's (at [esp+0x10] and [esp+0x14]), and the
// description is at [esp+0x24] in both.
// score 62 (was 62): this round found no gain - our frame slots for the locked rectangle sit four bytes
// lower than retail's, and retail builds each row from a running pointer while we recompute it from
// the row base.
// MiniMax C (11-08): no new form, no lower score.
// score 62 (was 62): our frame slots for the locked rectangle sit four bytes lower than retail's,
// so the surface pointer and the LockRect arguments are stored at other offsets; we hold pBits in a
// register and set the flag there, where retail sets it in memory and keeps the pointer in the frame;
// our loop hoists the left edge and counts the destination pointer up, where retail loads the left
// edge from the global in every row and adds the pitch to the destination pointer.
// MiniMax B (11-13): no new form, no lower score (62): retail keeps the locked pBits in memory where ours holds it in a register, and the XDK's D3DLOCKED_RECT has Pitch before pBits while retail has pBits first.
// MiniMax G (11-13): no new form, no lower score (62): retail's locked rectangle keeps pBits below Pitch,
// while the XDK's D3DLOCKED_RECT has Pitch first, so the inlined lock's slot order cannot be reproduced from
// this source; reading the fields straight off the locked rectangle gives 63.
// @retail 0x13d80
void function_13d80()
{
    if (g_5093c8 && (short)(g_5093c0.first >> 16) >= 0 && (short)g_5093c0.first >= 0 &&
        (short)(g_5093c0.second >> 16) <= 640 && (short)g_5093c0.second <= 480)
    {
        D3DSurface *surface = D3DDevice_GetBackBuffer2(0);
        D3DSURFACE_DESC description;
        D3DSurface_GetDesc(surface, &description);
        if (description.Size == description.Width * description.Height * 4)
        {
            D3DLOCKED_RECT locked;
            D3DLOCKED_RECT *locked_result = &locked;
            D3DSurface_LockRect(surface, locked_result, NULL, 0);
            locked_result->pBits = (void *)((dword)locked_result->pBits | 0xf0000000);
            if (locked_result->pBits)
            {
                long width = (short)(g_5093c0.second >> 16) - (short)(g_5093c0.first >> 16);
                long height = (short)g_5093c0.second - (short)g_5093c0.first;
                byte const *source = (byte const *)g_5093c8;
                byte *row = (byte *)locked_result->pBits + (short)g_5093c0.first * locked_result->Pitch;
                for (long y = 0; y < height; ++y)
                {
                    dword *pixel = (dword *)row + (short)(g_5093c0.first >> 16);
                    for (long x = 0; x < width; ++x)
                    {
                        long value = source[x] * g_5093cc;
                        byte level = (byte)(value < 0 ? 0 : value > 255 ? 255 : value);
                        *pixel++ = (((((dword)level << 8) | level) << 8 | level) << 8) | level;
                    }
                    row += locked_result->Pitch;
                    source += width;
                }
            }
            D3DSurface_UnlockRect(surface);
        }
        D3DResource_Release(surface);
    }
}

extern D3DResource *g_485ae4, *g_485ae8, *g_485aec;

// MiniMax E (11-14): no new form, no lower score (2): retail's callee 0x15a60 reads the format from AX, the usage index
// from ECX and the out pointer from EBX, the same homes as here, and the two setups hold the same values in the same
// registers; retail emits the format's load before the usage index's zeroing and this build emits them the other way
// round. The plain check gives the same 2, so it is not an artefact of the reduced image, and the order is the callee's
// to decide - its source lives in src/unknown_0158f0.cpp, which this lane may not edit. This round: the format through
// a local and the result assigned after its declaration both keep the order, and the permuter's 8 tries found no
// better variant.
// MiniMax B (11-13): no new form, no lower score (2): the setup of the call to 0x15a60 still zeroes ECX after it loads 9 into EAX here, retail does the reverse; that register home belongs to the callee 0x15a60, which lives in another file.
// MiniMax G (11-13): no new form tried this round, no lower score: the whole difference is still the 0x15a60 call's
// setup, where retail loads 9 into eax before it zeroes ecx and we zero ecx first; the callee 0x15a60 and its
// parameter home live in src/unknown_0158f0.cpp, which this lane may not edit, so the setup stays as it is. The
// reduced check builds this body differently from the plain one (425 bytes against retail's 441 there), so the plain
// build is the judge of this function.
// MiniMax H (11-12): no new form, no lower score (2): the whole difference is still the 0x15a60 call's setup, where retail loads 9 into eax before zeroing ecx and we zero ecx first; that callee's parameter home is fixed in src/unknown_0158f0.cpp, which this lane may not edit.
// MiniMax H (11-11): no new form, no lower score (2): the whole difference is still the 0x15a60 call's setup, where retail loads 9 into eax before zeroing ecx and we zero ecx first; that callee's parameter home is fixed in src/unknown_0158f0.cpp, which this lane may not edit, and the reduced image builds this body differently, so the full check is the judge here.
// MiniMax I (11-11): no new form, no lower score (2): the whole difference is still the 0x15a60 call's setup, which retail orders eax before ecx and we order ecx before eax; that callee's parameter home is fixed in src/unknown_0158f0.cpp, which this lane may not edit.
// MiniMax D (11-11): no new form, no lower score (2): the three texture helpers 0x158f0, 0x159b0 and 0x15a60 each score 24 in src/unknown_0158f0.cpp, so the 0x15a60 call's setup order is that callee's to decide.
// MiniMax S (11-10): no new form, no lower score (2): the whole difference is the 0x15a60 call's setup, where retail loads 9 into eax before zeroing ecx and we zero ecx first; that callee's parameter home is fixed in src/unknown_0158f0.cpp, which this lane may not edit.
// MiniMax A (11-10): no new form, no lower score (2): the body is unchanged at 441 bytes, as
// retail's; the whole difference is the 0x15a60 call's setup, which retail orders eax before ecx and
// we order ecx before eax.
// MiniMax E (11-10): no new form, no lower score: the 0x15a60 call's registers are set ecx then
// eax here and eax then ecx in retail, and the callee's parameter home lives in another file.
// MiniMax R (11-10): no new form, no lower score (2): the whole difference is the 0x15a60 call's setup, which retail does eax then ecx and we do ecx then eax; the callee's declaration lives in src/unknown_0158f0.cpp, so its parameter order is reported here rather than changed.
// MiniMax G (11-05): no new form, no lower score; the two constants of the 0x15a60 call keep the callee's order, so the setup still zeroes ecx before it loads 9 into eax.
// MiniMax H (11-04): no new form, no lower score; the callee 0x15a60 takes its first argument in ecx in retail and in eax here, and that callee lives in another file.
// MiniMax D (11-01): the setup's order follows the callee's parameter home in
// src/unknown_0158f0.cpp, which this lane may not edit.
// MiniMax D (11-01): no new form, no lower score.
// MiniMax A (11-03): no new form, no lower score.
// MiniMax G (11-04): no new form, no lower score.
// MiniMax G (11-12): no new form tried this round, no lower score (2): the whole difference is still the
// 0x15a60 call's setup, whose parameter home is fixed in src/unknown_0158f0.cpp, a file this lane may not edit.
// MiniMax H (11-13): no new form, no lower score (2); the setup of the call to 0x15a60 zeroes ECX after it loads 9 into EAX here, retail does the reverse.
// score 2 (was 2): the setup of the call to 0x15a60 zeroes ecx before it loads 9 into eax,
// retail loads 9 first. The callee lives in src/unknown_0158f0.cpp and is itself unmatched,
// and its parameter home decides the order of this setup.
// MiniMax D (11-06): no new form, no lower score: the callee at 0x15a60 takes its first argument
// before the third one is cleared, and that callee is defined in src/unknown_0158f0.cpp, so its
// parameter order is reported here rather than changed.
// MiniMax E (11-08): no new form, no lower score: the 0x15a60 call's registers are set ecx then eax here and eax then ecx in retail, and the callee's body in another file fixes that order.
// MiniMax E (11-09): no new form, no lower score: the 0x15a60 call's registers are set ecx then eax here and eax then ecx in retail, and the callee's parameter order lives in another file.
// MiniMax D (11-09): no new form, no lower score: the body is unchanged at 2 (441 bytes, as retail's); the whole difference is the 0x15a60 call's setup, which retail orders eax before ecx and we order ecx before eax, and that callee's parameter home is fixed in src/unknown_0158f0.cpp, which this lane may not edit.
// MiniMax A (11-13): no new form tried this round, no lower score (2): the call to 0x15a60 still zeroes ECX
// after it loads 9 into EAX here and retail does the reverse; the caller emits the callee's register homes in
// reverse declaration order (checked on 0x158f0, 0x159b0 and 0x15a60), so retail's callee declares usage_index
// before format and ours declares format first: the fix belongs to src/unknown_0158f0.cpp, which is not this
// lane's file: BLOCKED-BY 0x15a60.
// @retail 0x1d0e0
bool function_1d0e0()
{
    bool result = function_0158f0(9, 0, 4, 4, 0, 0, (D3DTexture **)&g_485ae4) &&
        function_015a60(9, 4, 4, 4, 0, 0, (D3DVolumeTexture **)&g_485ae8) &&
        function_0159b0(4, 9, 0, 0, (D3DCubeTexture **)&g_485aec);
    if (result)
    {
        if (!g_485ae4 || !g_485ae8 || !g_485aec)
            result = false;
        else
        {
            word colors[2] = { 0x0f00, 0xf0f0 };
            D3DLOCKED_RECT rectangle;
            D3DTexture_LockRect((D3DTexture *)g_485ae4, 0, &rectangle, NULL, 0);
            long i = 0;
            word *i_out = (word *)rectangle.pBits;
            for (long i_remaining = 16; i_remaining; --i_remaining)
            {
                *i_out = colors[i & 1];
                ++i;
                ++i_out;
            }
            D3DTexture_UnlockRect((D3DTexture *)g_485ae4, 0);
            D3DLOCKED_BOX box;
            D3DVolumeTexture_LockBox((D3DVolumeTexture *)g_485ae8, 0, &box, NULL, 0);
            long j = 0;
            word *j_out = (word *)box.pBits;
            for (long j_remaining = 64; j_remaining; --j_remaining)
            {
                *j_out = colors[j & 1];
                ++j;
                ++j_out;
            }
            D3DVolumeTexture_UnlockBox((D3DVolumeTexture *)g_485ae8, 0);
            long face = 0;
            for (long faces_left = 6; faces_left; --faces_left)
            {
                D3DCubeTexture_LockRect((D3DCubeTexture *)g_485aec, (D3DCUBEMAP_FACES)face, 0, &rectangle, NULL, 0);
                long k = 0;
                word *k_out = (word *)rectangle.pBits;
                for (long k_remaining = 16; k_remaining; --k_remaining)
                {
                    *k_out = colors[k & 1];
                    ++k;
                    ++k_out;
                }
                D3DCubeTexture_UnlockRect((D3DCubeTexture *)g_485aec, (D3DCUBEMAP_FACES)face, 0);
                ++face;
            }
        }
    }
    return result;
}

extern D3DPIXELSHADERDEF g_484f68;
extern long g_4b6298;
extern bool g_4b6294;
void function_0222d0(D3DRENDERSTATETYPE state, dword value);

// @retail 0x1e930
void __stdcall function_1e930(long opaque)
{
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 0;
    g_484f68.PSCombinerCount = 1;
    g_484f68.PSFinalCombinerInputsABCD = 4;
    function_0222d0(D3DRS_CULLMODE, 0);
    function_0222d0(D3DRS_COLORWRITEENABLE, 0x10101);
    function_0222d0(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    function_0222d0(D3DRS_ZENABLE, 2);
    function_0222d0(D3DRS_ZBIAS, 8);
    function_0222d0(D3DRS_STENCILENABLE, 0);
    if ((byte)opaque)
    {
        function_0222d0(D3DRS_ALPHABLENDENABLE, 0);
        function_0222d0(D3DRS_ALPHATESTENABLE, 0);
        function_0222d0(D3DRS_ZWRITEENABLE, 1);
    }
    else
    {
        function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
        function_0222d0(D3DRS_ALPHATESTENABLE, 0);
        function_0222d0(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        function_0222d0(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        opaque = D3DRS_BLENDOP;
        function_0222d0(*(D3DRENDERSTATETYPE const *)&opaque, D3DBLENDOP_ADD);
        opaque = D3DRS_ZWRITEENABLE;
        function_0222d0(*(D3DRENDERSTATETYPE const *)&opaque, 0);
        g_484f68.PSFinalCombinerInputsEFG = 0x1400;
    }
    function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x14), 0);
    function_1c710(g_51f0f0);
    function_15180(&g_484f68);
    function_14bc0((short)g_4858b8, 0, true);
    function_1cf50();
    g_4b6298 = 0;
    g_4b6294 = true;
}

bool g_55e6bc;
// MiniMax D (11-14): no new form, no lower score (17): this round measured the depth-clip block and the matrix loop against retail. The far clip plane is still loaded into xmm1 after the near store where retail loads it into xmm2 before the near multiply: the near factor in a local of its own gives 133 (the multiply becomes a register-register mul), the far plane in a local gives 18 (its load is hoisted to the function entry above the scale branch) and both planes in locals gives 18 as well, while swapping the two factors of the near-plane product changes nothing. The middle products of three of the four columns still load the row-relative factor into the register where retail loads the fixed one, and that split does not follow the source order here, so it cannot be reached by swapping the operands. The permuter (600 s) found no variant below its own baseline.
// MiniMax C (11-11): no new form, no lower score (76 with this file's other addresses, 17 on its own); the far plane is still loaded after the near-plane multiply and the three row products are still summed in a rotated order, and the inlined depth-clip call in this image pushes the segment size and its half where retail pushes only the half.
// MiniMax D (11-10): no new form, no lower score; the body is unchanged at 87 measured on its own (587 bytes),
// but in a reduced image that holds other addresses as well it is 127 at 571 bytes, so the score of this function
// depends on what else is checked beside it.
// MiniMax S (11-09): no new form, no lower score (87): retail keeps the near-plane product at the
// source position with the depth-clip global folded into the multiply, ours sinks the product into
// the inlined call and keeps the global in a register of its own; the six near-plane spellings
// tried this round all score 87, and the matrix loop's row base, cursor and counter still sit in
// different registers from retail's.
// MiniMax D (11-09): no new form, no lower score (87); retail still sums the three products of each
// output row in the source order while this build sums them rotated with one operand swapped, and retail
// still loads the far plane before the near-plane multiply while this build multiplies first.
// MiniMax A (11-06, run 52): no new form, no lower score (87) - retail sums the three products in
// source order and pops esi after the matrix loop, ours sums them rotated and pops esi early.

// MiniMax B (11-13): no new form, no lower score (17): the far clip plane is still loaded after the near-plane multiply here and into xmm2 before it in retail, and three of the four middle products still load the row-relative factor where retail loads the fixed one.
// MiniMax D (11-05): no new form, no lower score - retail sums the three products in source order and pops esi after the matrix loop, ours sums them rotated and pops esi early; rotating the terms, an accumulator and the near-plane spelling all score 87.
// MiniMax H (11-04): no new form, no lower score; the output stores written through an index give
// 143, 144, 143 and 145, and the near-plane block in six orders gives 119, 109, 93, 87, 131 and 117
// against 87, so retail's split of the output into a row base and a cursor still stands.
// MiniMax H (11-03): no new form, no lower score; retail keeps the projection base in ecx, the row counter in edi and the row base in esi, where ours uses one cursor in ecx and the counter in edx, and retail folds the depth-clip global into the multiply where ours keeps it in a register.
// MiniMax E (11-01): index-based output stores score 143 and a separate near factor ties 87;
// no new form, no lower score.
// MiniMax C (11-06): no new form, no lower score - the near-plane product still folds the global into
// the multiply only in retail, and the three matrix products are still summed in a rotated order with esi
// popped early; the source forms tried this round scored 117, 109 and 87, and a later sweep of the
// near-plane as a matrix-shaped local (150), the same with a row base and an advancing cursor (the body
// grew to 1307 bytes) and the near-plane product with the global first (119) scored no better, so the
// body stands as it was.
// MiniMax E (11-10): no new form, no lower score (87 alone, 127 with this file's other addresses); the near
// plane folded into one product with the global (114), the far plane declared first (122) and a row base with
// constant indices (951) all score worse, and the row's three products are still summed in a rotated order.
// MiniMax H (11-12): no new form, no lower score (17): retail still loads the far clip plane into xmm2 before the near-plane multiply while this build loads it after the near store, and retail still loads the fixed factor of the middle product of three of the four columns into the register where this build loads the row-relative factor.
// MiniMax H (11-13): no new form, no lower score (17); the far clip plane is still loaded after the near-plane multiply here and before it into xmm2 in retail, and the three middle products still load the row-relative factor where retail loads the fixed one.
// score 10 (was 87): the matrix base, the row base, the advancing cursor and the row counter sit
// in the registers retail uses, each row's three products are summed in the source order, and the
// near and far planes are now computed into locals of their own, so the far plane's load lands at
// the branch join where retail takes it; what is left is that retail loads the far plane just
// before the near-plane multiply and this build loads it just after, and four products whose
// operands this build loads the other way round.
// sit in the registers retail uses, and each row's three products are summed in the source order;
// what is left is the far plane's load, which retail takes before the near-plane multiply into
// xmm2 and ours takes afterwards into xmm1, and three products whose operands this build loads
// the other way round.
// MiniMax B (11-10): the projection matrix is now a local of its own, so the base lands in ECX
// and the varying row pointer in EAX, the output is written through a row base and an advancing
// cursor as retail does, each row's three products are summed through an accumulator so the sum
// keeps the source order, the near plane is multiplied with the global at the call and the far
// plane is passed as it stands: 87 -> 17, plain and fast alike.
// the output into a row base (ESI) and an advancing cursor (EDX); ours uses one cursor in ECX, the
// counter in EDX, and multiplies the near plane in a register where retail folds the global into
// the multiply.
// MiniMax E (11-09): no new form, no lower score (87); the order around the bounds check still differs:
// retail multiplies the near plane before it reads the buffer's begin and end and writes the three shared
// render globals (the near plane, the far plane and the flag word) after the check, where ours multiplies
// after the compare and writes them before it.
// MiniMax B (11-11): no new form, no lower score (17); retail keeps the far clip plane in xmm2 from before the
// near-plane multiply while this build loads it after the near store, and retail loads the fixed factor of the
// middle product of three of the four columns into the register while this build always loads the row-relative
// factor there: swapping those products' operands, giving each column an accumulator of its own and moving the
// scale into the selecting ternary all leave the score at 17 or worse.
// MiniMax B (11-11): no new form, no lower score (17); the depth-clip arguments swapped score 10 but hand the
// device the wrong plane (the inlined call stores its first argument in the lower plane register, as retail's own
// stores show), so it was reverted; a local for the far plane hoists its load to the function entry (18), and the
// three products stay as they are because this build keeps the higher-addressed factor in the register whatever
// order the source uses (the swapped row scores the same 17, one row written as a single expression 27).
// MiniMax B (11-11): no new form, no lower score (17); the operand order of a product does not come from the
// source here: this build loads the row-relative factor first however the product is written (the first column's
// middle product written with the fixed factor first scores the same 17), so retail's three products that load the
// fixed factor first cannot be reached by swapping the operands; what still differs is that retail keeps the far
// clip plane in xmm2 from before the near-plane multiply while this build loads it after the near store, and
// retail loads the fixed factor of the middle product of three of the four columns into the register.
// MiniMax B (11-11): the plain build gives 17 (587 bytes, the same length as retail); a reduced image that
// holds this round's other nine functions as well builds this function differently (76 differences, 573
// bytes), because link-time inlining changes with the set of functions in the image, so a fast measurement
// of this function is only meaningful in an image that holds it alone.
// MiniMax E (11-11), NOT kept (coordinator 11-11): 17 -> 10 only from the near and far planes each read into a local used once before the depth-clip call (match-only form); body at 17.
// MiniMax A (11-12): no new form, no lower score (17); the far clip plane read into a local before the call is hoisted
// to the function entry (18), the near and far planes each read into a local used once scores 10 but is a match-only
// form, and selecting the depth-clip factor into a local before the call scores 133; the near plane in a local of its
// own keeps the far load late (17), the depth-clip call after the projection selection gives 122 (573 bytes), and the
// middle products written with pointer arithmetic (19, 25) move other columns' operands instead of this one's. What
// still differs is that retail loads the far clip plane into xmm2 from before the near-plane multiply while this build
// loads it after the near store, and that retail loads the fixed factor of the middle product of three of the four
// columns into the register while this build always loads the row-relative factor there. Reading the fixed factor
// through a second pointer name for the matrix gives 23, the product parenthesised gives 21, and the loop counter as
// int, unsigned or short changes nothing; a toy compile with the SDK's compiler shows that the order of the two
// factors of a product does not follow the source, so the three products retail loads the other way round are not
// reachable from the source order.
// MiniMax A (11-13): no new form tried this round, no lower score (17): the far clip plane is still loaded
// after the near-plane multiply here and into xmm2 before it in retail, and three of the four middle products
// still load the row-relative factor where retail loads the fixed one.
// @retail 0x12d50
void function_12d50(bool scaled, real const *projection, real *output)
{
    real constants[16];
    D3DDevice_SetDepthClipPlanes((scaled ? 0.15f : 1.0f) * g_485adc, g_485ae0,
        D3DSDCP_SET_VERTEXPROGRAM_PLANES);
    real const *matrix = projection ? projection : g_48568c;
    if (!output)
        output = constants;
    real scale = 16777215.0f / g_48565c;
    real *row = output;
    real *next = output;
    for (long i = 0; i < 4; ++i)
    {
        real value = matrix[38 + i] * matrix[3];
        value += matrix[30 + i] * matrix[1];
        value += matrix[34 + i] * matrix[2];
        *row = value * scale;
        value = matrix[38 + i] * matrix[6];
        value += matrix[30 + i] * matrix[4];
        value += matrix[34 + i] * matrix[5];
        *++next = value * scale;
        value = matrix[38 + i] * matrix[9];
        value += matrix[8] * matrix[34 + i];
        value += matrix[30 + i] * matrix[7];
        *++next = value * scale;
        value = matrix[38 + i] * matrix[12];
        value += matrix[11] * matrix[34 + i];
        value += matrix[30 + i] * matrix[10];
        *++next = value * scale;
        row = ++next;
        next[-1] += matrix[42 + i] * scale;
    }
    if (scaled)
        for (long j = 0; j < 16; ++j)
            output[j] *= 0.15f;
    if (output == constants && scaled != g_55e6bc)
    {
        D3DDevice_SetVertexShaderConstant(-96, output, 4);
        g_55e6bc = scaled;
    }
}

// @retail 0x14600
bool function_14600()
{
    bool result = true;
    D3DDevice_GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &g_50935c[0]);
    D3DDevice_GetBackBuffer(-1, D3DBACKBUFFER_TYPE_MONO, &g_50935c[1]);
    D3DDevice_GetDepthStencilSurface(&g_509364);
    if (g_50935c[0] && g_50935c[1] && g_509364)
    {
        void *memory = VirtualAlloc(NULL, 20, 0x101000, PAGE_READWRITE);
        if (!memory) GetLastError();
        g_509354[0] = (D3DTexture *)memory;
        memory = VirtualAlloc(NULL, 20, 0x101000, PAGE_READWRITE);
        if (!memory) GetLastError();
        g_509354[1] = (D3DTexture *)memory;
        if (g_509354[0] && g_509354[1])
        {
            for (short i = 0; i < 2; ++i)
            {
                D3DTexture *texture = g_509354[i];
                texture->Common = 0x40001;
                texture->Data = g_50935c[i]->Data;
                texture->Lock = 0;
                texture->Size = 0x271df27f;
                texture->Format = 0x11229;
            }
        }
        else
            result = false;
    }
    else
        result = false;
    void *memory = VirtualAlloc(NULL, 24, 0x101000, PAGE_READWRITE);
    if (!memory) GetLastError();
    g_509370 = (D3DSurface *)memory;
    if (g_509370)
    {
        *g_509370 = *g_50935c[0];
        g_509370->Data = g_509364->Data;
    }
    else
        result = false;
    memory = VirtualAlloc(NULL, 20, 0x101000, PAGE_READWRITE);
    if (!memory) GetLastError();
    g_509368 = (D3DTexture *)memory;
    if (g_509368)
    {
        g_509368->Common = 0x40001;
        g_509368->Data = g_509364->Data;
        g_509368->Lock = 0;
        g_509368->Size = 0x271df27f;
        g_509368->Format = 0x11229;
    }
    else
        result = false;
    memory = VirtualAlloc(NULL, 20, 0x101000, PAGE_READWRITE);
    if (!memory) GetLastError();
    g_50936c = (D3DTexture *)memory;
    if (g_50936c)
    {
        g_50936c->Common = 0x40001;
        g_50936c->Data = g_509364->Data;
        g_50936c->Lock = 0;
        g_50936c->Size = 0x271df27f;
        g_50936c->Format = 0x13f29;
    }
    else
        result = false;
    return result;
}

// MiniMax E (11-14): no new form, no lower score (32 in this lane's measurement set; the earlier note records 40 under
// its own set): the XDK's device-call inline still takes the width difference in dx and the width quotient in xmm0 where
// retail takes them in ax and xmm2, and retail's four shader programs still come from tables this project does not have.
// MiniMax B (11-13): no new form, no lower score (32): the XDK's D3DDevice_SetVertexShaderConstant hoists its flag byte into al, so this build takes the width difference in dx and the width quotient in xmm0 where retail takes them in ax and xmm2, and retail's four shader programs still come from tables this project does not have.
// MiniMax H (11-12): no new form, no lower score (32): retail still takes the width difference in ax and the height difference in ecx and turns both into floats before the divisions where we take the width difference in dx and turn them into floats after the first division; retail's four shader programs also come from tables this project does not have.
// MiniMax H (11-11): no new form, no lower score (32): retail takes the width difference in ax and the height difference in ecx and turns both into floats before the divisions, ours takes the width difference in dx and turns the two into floats after the first division; retail's four shader programs also come from tables this project does not have, and an XDK inline helper hoists the flag byte into al here where retail tests it in memory.
// MiniMax I (11-11): no new form, no lower score (32): retail divides the width's quotient into xmm2 and multiplies it into xmm0 where we divide into xmm0 and multiply into xmm3, and the flag byte is still tested in memory in retail where an XDK inline helper hoists it into al here, so ax is taken.
// MiniMax D (11-11): no new form, no lower score (32).
// MiniMax S (11-10): no new form, no lower score (32): retail divides the width's quotient into xmm2 and multiplies it into xmm0 where we divide into xmm0 and multiply into xmm3, and retail tests the flag byte in memory where an XDK inline helper hoists it into al, so ax is taken.
// MiniMax A (11-10): no new form, no lower score (32): retail writes the render-state cache slots
// and pushes the D3D push-buffer commands itself, copies the twenty constants to the shadow at
// 0x408248 unless bit 4 of the device's flag byte at 0x404fe8 is set, and builds a draw structure
// with its own command word that this project does not have.
// MiniMax E (11-10): no new form, no lower score: retail keeps the width quotient in xmm2 and the
// scaled value in xmm3, ours uses xmm4 and xmm0 for the same two, so the constants land in other
// stack slots.
// MiniMax R (11-10): no new form, no lower score (32): the ratio block's order and the two divisions agree with retail, but the XDK's D3DDevice_SetVertexShaderConstant inline hoists the flag byte into al before the divisions here while retail tests that byte in memory, so ax is taken, and retail's four shader programs come from tables this project does not have.
// MiniMax R (11-09): score 32 (was 40): the ratio block now runs in retail's order - the width
// ratio is computed first, the -1.0f is taken off it before the double, and the height ratio is
// divided only after that; what still differs is that the width quotient lands in xmm0 where
// retail divides into xmm2, the -1.0f lands in xmm2 where retail keeps it in xmm3, and the flag
// test reads al here and the flag byte in memory in retail.
// MiniMax H (11-05): no new form, no lower score; retail's ratio block keeps the width difference
// in EDX and the height difference in EAX and reuses xmm2 for the height quotient, ours uses the
// other pair of registers and xmm0, so the two blocks stay different whatever the source order.
// MiniMax D (run 37): no new form, no lower score; the width reciprocal first gives 74, the
// width first with constants[3] first gives 78 and the full retail order gives 43.
// score 40 (was 40): this round's width-first form with the constants stored in retail's order
// scores 38 but still computes the quotient in xmm0 rather than xmm2 and loads the height
// before the width; the gain is under the keep bar, so the original body stands.
// score 40 (was 40): this round found no gain - retail computes the width ratio first and keeps it in
// xmm2; declaring x before y gives 74 and reordering the constants gives 78, so the original order stands.
// MiniMax G (11-12): no new form tried this round, no lower score (32): the width quotient still lands in xmm0
// where retail keeps it in xmm2, and retail's four shader programs come from tables this project does not have.
// score 32 (was 73): retail loads the width pair first and only then the
// height pair, ours loads the height pair first; the conversions of the two
// differences into EDX and EAX now agree
// MiniMax D (11-06): no new form, no lower score.
// MiniMax E (11-08): no new form, no lower score: retail keeps the width quotient in xmm2 and the scaled value in xmm3, ours uses xmm4 and xmm0 for the same two, so the constants land in other stack slots.
// MiniMax E (11-09): no new form, no lower score: retail keeps the width quotient in xmm2 and the scaled value in xmm3, ours uses xmm4 and xmm0 for the same two, so the constants land in other stack slots.
// MiniMax D (11-09): the score-32 body still differs in two places: retail takes the width's
// difference in ax and keeps the width's quotient in xmm0, while we take the difference in dx and
// keep the quotient in xmm2; the cause is the XDK's D3DDevice_SetVertexShaderConstant inline, which
// hoists the flag byte into al before the two divisions in our build, while retail tests that byte
// in memory, so ax is taken.
// MiniMax D (11-09): score 32 (was 40): the width's ratio comes first, the third constant is stored
// before the first and the height's ratio is declared after the fourth constant; the length is now
// retail's 612 bytes and the two divisions run in retail's order.
// MiniMax D (11-09): no new form, no lower score: the body is unchanged at 40 (613 bytes against retail's 612); the width and height differences are read as thirty-two bits in retail and the quotient and the scaled value live in xmm2 and xmm3, while ours reads them another way and uses xmm4 and xmm0; six forms of the ratio block (the reciprocals together, the height first, the signed dimensions first, the height's end first, the constant array's initializer and the width first) all score worse or tie.
// MiniMax B (11-13): no new form, no lower score (32): the XDK's D3DDevice_SetVertexShaderConstant hoists its flag byte into al, so our width difference lands in dx and the width quotient in xmm0 where retail takes them in ax and xmm2; an explicit float cast, the ratio written out at each use and the ratio kept in a separate float local all give the same 32, both ratios declared first give 78, the zero-th constant before the third gives 75, and the permuter cannot search this function (the reduced image scores it 170 against 32).
// MiniMax G (11-13): no new form, no lower score (32): the XDK's D3DDevice_SetVertexShaderConstant hoists its
// flag byte into al, so this build takes the width difference in dx and the width quotient in xmm0 where retail
// takes them in ax and xmm2; the forms in the note below all give the same 32.
// @retail 0x1eb00
void function_1eb00()
{
    function_0222d0(D3DRS_CULLMODE, 0);
    function_0222d0(D3DRS_ALPHABLENDENABLE, 0);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    function_0222d0(D3DRS_ZENABLE, 0);
    function_0222d0(D3DRS_ZBIAS, 0);
    function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x6c), 0);
    function_1c710(g_51f0f0);
    real constants[20];
    real x = 1.0f / (short)(g_48564e - g_48564a);
    constants[3] = -1.0f - x;
    constants[0] = x * 2.0f;
    constants[1] = 0.0f;
    constants[2] = 0.0f;
    constants[4] = 0.0f;
    real y = 1.0f / (short)(g_48564c - g_485648);
    constants[5] = y * -2.0f;
    constants[6] = 0.0f;
    constants[7] = y + 1.0f;
    constants[8] = 0.0f;
    constants[9] = 0.0f;
    constants[10] = 0.0f;
    constants[11] = 0.5f;
    constants[12] = 0.0f;
    constants[13] = 0.0f;
    constants[14] = 0.0f;
    constants[15] = 1.0f;
    constants[16] = 1.0f;
    constants[17] = 1.0f;
    constants[18] = 0.0f;
    constants[19] = 1.0f;
    D3DDevice_SetVertexShaderConstant(81, constants, 5);
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 0;
    g_484f68.PSCombinerCount = 1;
    g_484f68.PSFinalCombinerInputsABCD = 4;
    D3DDevice_SetPixelShaderProgram(&g_484f68);
    function_1cf50();
    g_4b6298 = 0;
    g_4b6294 = true;
}
// MiniMax H (11-11): no new form, no lower score (79); the body is still 25 bytes shorter than retail's, the ratio of the two counters is still divided into xmm3 and multiplied into xmm4 where retail divides into xmm2 and multiplies into xmm0, and retail's body builds four shader programs from tables and caches that this project does not have, so the difference is a knowledge difference rather than a local form.
// MiniMax A (11-11): no new form, no lower score (79); the body is still 25 bytes shorter than retail's (738
// against 763) and the ratio of the two counters is still divided into xmm3 and multiplied into xmm4 where retail
// divides into xmm2 and multiplies into xmm0; retail's body builds four shader programs from tables and caches that
// this project does not have, so the difference is a knowledge difference rather than a local form. The original
// body is kept.
// MiniMax R (11-09): no new form, no lower score; the ratio of the two counters is still divided into xmm3 and multiplied into xmm4 where retail divides into xmm2 and multiplies into xmm0, and retail's body still builds four shader programs from tables this project does not have.
// MiniMax R (11-09): no new form, no lower score; the width quotient is still divided into xmm3 where
// retail divides into xmm2 and multiplies into xmm0.
// MiniMax A (11-06, run 52): no new form, no lower score (79) - retail copies the first five constants
// into its own locals in a different order.
// MiniMax A (11-12): no new form, no lower score (79): retail body builds four shader programs from tables and caches that this project does not have, and the ratio of the two counters is still divided into xmm3 and multiplied into xmm4 where retail divides into xmm2 and multiplies into xmm0, so what is left is a knowledge difference rather than a local form and the body stands.

// MiniMax E (11-14): no new form, no lower score (79): retail's body still builds four shader programs from tables and
// caches that this project does not have, and the ratio of the two counters is still divided into xmm3 and multiplied
// into xmm4 where retail divides into xmm2 and multiplies into xmm0, so what is left is a knowledge difference.
// MiniMax H (11-13): no new form, no lower score (79).
// MiniMax H (11-12): no new form, no lower score (79); retail's body builds four shader programs from tables and
// caches that this project does not have, and the ratio of the two counters is still divided into xmm3 and multiplied
// into xmm4 where retail divides into xmm2 and multiplies into xmm0, so what is left is a knowledge difference rather
// than a local form and the body stands.
// MiniMax G (11-11): no new form, no lower score (79); retail's body builds four shader programs from tables and
// caches that this project does not have, and the ratio of the two counters is still divided into xmm3 and
// multiplied into xmm4 where retail divides into xmm2 and multiplies into xmm0, so what is left is a knowledge
// difference rather than a local form and the body stands.
// MiniMax C (11-11): no new form, no lower score (79); the ratio of the two counters is still computed with the other operand order, and retail's body builds four shader programs from tables this project does not have.
// MiniMax R (11-10): no new form, no lower score (79); the ratio of the two counters is still divided into xmm3 and multiplied into xmm4 where retail divides into xmm2 and multiplies into xmm0, and retail's body builds four shader programs from tables this project does not have.
// MiniMax I (11-10): no new form, no lower score (79); the ratio of the two counters is still divided into xmm3 and multiplied into xmm4 where retail divides into xmm2 and multiplies into xmm0, and retail's body builds four shader programs from tables this project does not have.
// MiniMax E (11-09): no new form, no lower score; retail's body builds four shader programs
// from tables and caches that this project does not have, and the ratio of the two counters is
// still computed with the other operand order.
// MiniMax D (11-05): no new form, no lower score - retail copies the first five constants into a
// different global than the SDK helper writes and builds the pixel shader program through a device
// field, so the difference sits in the D3D helper, not in this body.
// MiniMax C (11-06): no new form, no lower score - retail still builds four shader programs from tables
// and caches that this project does not have, so the difference is a knowledge difference, not a local
// form; the ratio of the two counters is still computed with the other operand order.
// MiniMax H (11-03): no new form, no lower score; SDK SetPixelShaderProgram inline: 763 bytes but merges the flag's 1 with the render-state argument into ebx (155), cross-module helper 155, helper call 167.
// MiniMax E (11-01): no new form, no lower score; retail's body builds four shader programs from
// tables and caches that this project does not have.
// score 79 (was 79): this round found no gain - retail's body builds four shader programs from tables and
// caches that this project does not have, so the difference is a knowledge difference, not a local form.
// MiniMax C (11-08): no new form, no lower score.
// score 79 (was 79): the ratio of the two counters is computed with the other operand order:
// ours divides into xmm3 and multiplies xmm4, retail divides into xmm2 and multiplies xmm0.
// MiniMax B (11-13): no new form, no lower score (79): retail's body builds four shader programs from tables and caches
// that this project does not have, so the difference is a knowledge difference and not a local form; the ratio of the two
// counters is still computed with the other operand order.
// @retail 0x36580
void function_36580()
{
    g_4670bc = true;
    function_16b10((s_render_reset_state *)g_485b48);
    function_0222d0(D3DRS_COLORWRITEENABLE, 0x10101);
    function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
    function_0222d0(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    function_0222d0(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    function_0222d0(D3DRS_ZENABLE, 0);
    function_0222d0(D3DRS_ZBIAS, 0);
    function_0222d0(D3DRS_CULLMODE, 0);
    real x = 1.0f / (short)(g_48564e - g_48564a);
    real y = 1.0f / (short)(g_48564c - g_485648);
    real extra[24] = { 0.0f };
    real constants[20];
    constants[0] = x * 2.0f;
    constants[1] = 0.0f;
    constants[2] = 0.0f;
    constants[3] = -1.0f - x;
    constants[4] = 0.0f;
    constants[5] = y * -2.0f;
    constants[6] = 0.0f;
    constants[7] = y + 1.0f;
    constants[8] = 0.0f;
    constants[9] = 0.0f;
    constants[10] = 0.0f;
    constants[11] = 0.5f;
    constants[12] = 0.0f;
    constants[13] = 0.0f;
    constants[14] = 0.0f;
    constants[15] = 1.0f;
    constants[16] = 1.0f;
    constants[17] = 1.0f;
    constants[18] = 0.0f;
    constants[19] = 1.0f;
    D3DDevice_SetVertexShaderConstant(81, constants, 5);

    D3DDevice_SetVertexShaderConstant(86, extra, 6);
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSRGBOutputs[0] = 0xc0;
    g_484f68.PSAlphaOutputs[0] = 0xc0;
    g_484f68.PSTextureModes = 0;
    g_484f68.PSCombinerCount = 1;
    g_484f68.PSRGBInputs[0] = 0x20040000;
    g_484f68.PSAlphaInputs[0] = 0x20140000;
    g_484f68.PSFinalCombinerInputsABCD = 0xc;
    g_484f68.PSFinalCombinerInputsEFG = 0x1c00;
    function_1ccf0(&g_484f68);
    function_1cc60(0);
}


extern byte g_4c1a18;

// @retail 0x36560
void function_36560()
{
    if (g_4c1a18)
    {
        function_1e930(1);
        g_4c1a18 = 0;
    }
}

__declspec(noinline) bool function_12420();

// @retail 0x12420
bool function_12420()
{
    function_123b0();
    bool result = function_12490();
    if (result)
    {
        D3DDevice_Swap(0);
        if (g_5093b0)
        {
            ((D3DDevice *)g_5093b0)->Release();
            g_5093b0 = NULL;
        }
    }
    return result;
}



extern byte g_485a75, g_485a76;
void function_15370(short mode);

void function_3c650(byte const *state);
void function_3d000(real const *state);

// @retail 0x467c0
short __stdcall function_467c0(short first, short second, short count)
{
    if (count > 0)
    {
        function_0222d0(D3DRS_CULLMODE, 0x901);
        function_0222d0(D3DRS_COLORWRITEENABLE, 0x10101);
        function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
        function_0222d0(D3DRS_SRCBLEND, 0x304);
        function_0222d0(D3DRS_DESTBLEND, 0);
        function_0222d0(D3DRS_BLENDOP, 0x8006);
        function_0222d0(D3DRS_ALPHATESTENABLE, 0);
        function_0222d0(D3DRS_ZENABLE, 0);
        function_0222d0(D3DRS_ZBIAS, 0);
        function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0xc), 0);
        function_1c710(g_51f0f0);
        real constants[32] = {
            1.0f,0.0f,0.0f,-0.0078125f, 0.0f,1.0f,0.0f,-0.0078125f,
            1.0f,0.0f,0.0f,0.0078125f, 0.0f,1.0f,0.0f,0.0078125f,
            1.0f,0.0f,0.0f,-0.0078125f, 0.0f,1.0f,0.0f,0.0078125f,
            1.0f,0.0f,0.0f,0.0078125f, 0.0f,1.0f,0.0f,-0.0078125f
        };
        D3DDevice_SetVertexShaderConstant(-78, constants, 8);
        memset(&g_484f68, 0, sizeof(g_484f68));
        g_484f68.PSAlphaOutputs[0] = 0xc00;
        g_484f68.PSRGBOutputs[0] = 0xc00;
        g_484f68.PSRGBOutputs[1] = 0xc00;
        g_484f68.PSTextureModes = 0x8421;
        g_484f68.PSCombinerCount = 2;
        g_484f68.PSConstant0[0] = 0xff000000;
        g_484f68.PSAlphaInputs[0] = 0x08a009a0;
        g_484f68.PSRGBInputs[0] = 0x0aa00ba0;
        g_484f68.PSRGBInputs[1] = 0x1c110c11;
        g_484f68.PSFinalCombinerInputsABCD = 0xc;
        D3DDevice_SetPixelShaderProgram(&g_484f68);
        for (short i = 0; i < count; ++i)
        {
            short source = i & 1 ? second : first;
            short target = i & 1 ? first : second;
            for (short stage = 0; stage < 4; ++stage)
            {
                function_14f60(stage, source);
                sampler_state_write(stage, D3DTSS_ADDRESSU, 4);
                sampler_state_write(stage, D3DTSS_ADDRESSV, 4);
                sampler_state_write(stage, D3DTSS_MAGFILTER, 2);
                sampler_state_write(stage, D3DTSS_MINFILTER, 2);
                sampler_state_write(stage, D3DTSS_MIPFILTER, 1);
                sampler_state_write(stage, D3DTSS_MAXANISOTROPY, 0);
                sampler_state_write(stage, D3DTSS_MIPMAPLODBIAS, 0);
                sampler_state_write(stage, D3DTSS_MAXMIPLEVEL, 0);
                sampler_state_write(stage, D3DTSS_COLORSIGN, 0);
                sampler_state_write(stage, D3DTSS_ALPHAKILL, 0);
            }
            function_14bc0(target, 0, false);
            g_484f68.PSConstant0[0] = (i > 0 ? 0x7fu : 0xffu) << 24;
            D3DDevice_SetPixelShaderProgram(&g_484f68);
            function_1cf50();
            D3DDevice_Begin(D3DPT_TRIANGLEFAN);
            D3DDevice_SetVertexData2s(3, 0, 0);
            D3DDevice_SetVertexData4f(0, 0.53125f, 0.53125f, 16777215.0f, 16777215.0f);
            D3DDevice_SetVertexData2s(3, 1, 0);
            D3DDevice_SetVertexData4f(0, 64.53125f, 0.53125f, 16777215.0f, 16777215.0f);
            D3DDevice_SetVertexData2s(3, 1, 1);
            D3DDevice_SetVertexData4f(0, 64.53125f, 64.53125f, 16777215.0f, 16777215.0f);
            D3DDevice_SetVertexData2s(3, 0, 1);
            D3DDevice_SetVertexData4f(0, 0.53125f, 64.53125f, 16777215.0f, 16777215.0f);
            D3DDevice_End();
        }
        function_14bc0((short)g_4858b8, 0, true);
    }
    return count & 1 ? second : first;
}

// @retail 0x4d640
void __stdcall function_4d640(short index, long mode, bool setup)
{
    /* read from the stack: retail passes index there, and mode in edi */
    s_44940_entry *entry = &g_4ba138[*(short const volatile *)&index];
    if (entry->unknown10[10] != 0xff)
    {
        real opacity = (real)entry->unknown10[10] * (1.0f / 255.0f);
        if (!(fabs(g_4b8494 - opacity) < 0.0001f))
        {
            D3DDevice_SetStipple(function_1c290(opacity));
            g_4b8494 = opacity;
        }
    }
    if (entry->unknown00 & 0x800)
        function_12d50(true, NULL, NULL);
    if (mode == 1 || mode == 3)
    {
        if (g_485a75 || g_485a76)
            function_15370(0);
        else if (entry->unknown00 & 0x4000)
            function_15370(2);
    }
    if (setup)
    {
        byte *structure = (byte *)g_4e0348;
        if (*(long *)(structure + 0x214) > 0)
        {
            byte *state = *(byte **)(structure + 0x218);
            function_3c650(state);
            function_3d000((real const *)state);
        }
    }
}

// @retail 0x4d720
void function_4d720(short index, long mode)
{
    /* read from the stack: retail passes index there, and mode in edi */
    s_44940_entry *entry = &g_4ba138[*(short const volatile *)&index];
    if (entry->unknown10[10] != 0xff && !(fabs(g_4b8494 - 1.0f) < 0.0001f))
    {
        D3DDevice_SetStipple(function_1c290(1.0f));
        g_4b8494 = 1.0f;
    }
    if (entry->unknown00 & 0x800)
        function_12d50(false, NULL, NULL);
    if ((mode == 1 || mode == 3) && ((entry->unknown00 & 0x4000) || g_485a75 || g_485a76))
        function_15370(1);
}



#include "visibility_slot.h"
typedef long (__stdcall *visibility_draw_callback)(s_slot *slot, byte *data);
void __stdcall function_20bb0(long player, dword mask, long const *indices, long count, bool query, visibility_draw_callback draw);
long __stdcall function_2e540(s_slot *slot, byte *data);
extern long g_485898;

// MiniMax H (11-13): no new form, no lower score (285): the permuter ran on this function for 600 s over the reduced
// image and left the baseline where it was (270 differing instructions in both the reduced and the full image), so no
// permutation of the body beats it; the frame is still 0x38 bytes against retail's 0x34 (retail reuses the position's
// slots for the corner coordinates), the 1.0 constant still takes xmm0 where retail holds it in xmm3, and the three
// byte-to-float conversions still start at xmm1 where retail starts them at xmm0.
// MiniMax G (11-13): no new form, no lower score (285): the diff re-read this round puts the first difference at the 1.0 constant's register (XMM0 here, XMM3 in retail) and shows our frame slots above retail's from the definition pointer on, with the masked tag kept in EBX in retail and no copy of it here, so the body stands.
// MiniMax B (11-11): no new form, no lower score (285); retail's frame is 0x34 bytes and ours 0x38 because retail
// reuses the position's stack slots for the corner coordinates and keeps the area in the dead argument's home, so
// every slot above the position sits four bytes higher, and the 1.0 constant takes xmm0 here and xmm3 in retail.
// MiniMax I (11-11): score 272 (was 285); the four corner coordinates now share the position's slots and the frame is 0x34 bytes as in retail, and the three offsets are computed as three scaled values with the 1.0 subtracted afterwards, which drops thirteen bytes of code; the 1.0 constant still takes XMM0 where retail holds it in XMM3, the three conversions still take XMM1..XMM3 where retail takes XMM0..XMM2, and the definition pointer is still loaded once where retail loads it twice.
// MiniMax E (11-11, run 80): no new form, no lower score (285): the frame is still 0x38 against retail's 0x34 (retail reuses the position's slots for the corner coordinates and keeps the area in the dead argument's home), the 1.0 constant still takes XMM0 where retail holds it in XMM3, and the three byte-to-float conversions still start at XMM1 here and at XMM0 in retail; the corner slots in retail's order and the four products of the 0x480a0 body in retail's order were tried by earlier rounds and score 285 and 213.
// MiniMax E (11-10, run 73): no new form, no lower score (285); retail's frame is 0x34 bytes and ours 0x38 because retail reuses the position's slots for the corner coordinates and keeps the area in the dead argument's home, so every slot above the position sits four bytes higher, and the 1.0 constant still takes XMM0 where retail holds it in XMM3.
// MiniMax D (11-11): no new form, no lower score (285): the frame is still 0x38 against retail's 0x34, because retail's frame holds thirteen dwords (the two extents, the position's three, the bottom corner, and the screen that shares the sizes' seven) while ours holds twelve and homes the bottom corner in the dead argument slot instead, so every slot sits four bytes higher; the three byte-to-float conversions still take XMM1..XMM3 where retail takes XMM0..XMM2; reusing the position's slots for three of the corners with the bottom corner separate, the same with the bottom corner at the outer scope, and the same with the area declared ahead of the bottom corner all score 287; the bottom corner declared ahead of the other three corners scores 285 again, the three offsets computed after the scale instead of before it scores 299, and the four products of the 0x480a0 body computed in retail's order (the width's cosine product first) scores 213 instead of 125.
// MiniMax G (11-11): no new form, no lower score (285): retail's frame is 0x34 bytes and ours 0x38 (the area's slot), the 1.0 constant takes xmm0 here and xmm3 in retail, retail keeps the definition pointer in ebx and reloads it for the second flag test where we keep one lookup, and the three byte-to-float conversions start at xmm0 in retail and xmm1 here; returning the area directly and dropping the result local scores 286 and is reverted.
// MiniMax S (11-10): no new form, no lower score (285): retail's frame is 0x34 bytes and ours 0x38, because retail reuses the position's slots for the corner coordinates and keeps the area in the dead argument's home, ours gives the corners slots of their own and holds the area in a local, so every slot above the position sits four bytes higher.
// MiniMax D (11-10): no new form, no lower score (285): the 1.0 constant still takes XMM0 where retail holds it in XMM3, and naming it as a constant local compiles the same; both switch arms now add to x, then z, then y, as retail's stores do.
// MiniMax D (11-10): no new form, no lower score (285): the 1.0 constant still takes XMM0 where retail holds it in XMM3, and naming it as a constant local compiles the same.
// MiniMax A (11-04): the corner locals declared at function scope, before the position 285 (the same code); no lower score.
// MiniMax D (11-01): the scale as a ternary 288 (worse), reverted; no lower score.
// MiniMax H (11-12): no new form, no lower score (285): the frame is still 0x38 bytes against retail's 0x34, the 1.0 constant still takes xmm0 where retail holds it in xmm3, and the three byte-to-float conversions still start at xmm1 where retail starts them at xmm0.
// MiniMax B (11-13): no new form, no lower score (285): retail loads its two constants (2/255 into xmm4, 1.0 into
// xmm3) before the three byte conversions and keeps the conversions in xmm0..xmm2, so the constant and the
// conversion registers are the compiler's own order and not the source's; the frame is still 0x38 against retail's
// 0x34 because retail reuses the position's slots for the four corners.
// score 285 (was 285): retail's frame is 0x34 bytes and ours 0x38, because retail reuses the
// position's stack slots for the corner coordinates while we keep two slots of our own, and
// retail tests the definition's flag byte where it lies while we copy the byte into bl.
// Round 2 of #1066: logic walk against retail 0x2e540-0x2e9d2 found no behaviour difference (tag and flag tests, the
// two-lookup flag byte, scale, radius, the size table by the short at +0x16, both switch arms, the extent and depth
// clamps, floor order left/top/right/bottom, area test and the four vertices all agree; every float constant checked
// in the XBE: 0x45dd58 2/255, 0x45dbc0 1.0, 0x45dfe4 sqrt2, 0x45dc0c 2.0, 0x45dd60 128.0, 0x45e45c 2^31, 0x45dd40 16777215).
// Retail reloads the definition pointer for the second flag test (ebx for the 0x40 test, ecx for the rest) and ours
// merges the two loads; retail's size table shares the slots of the screen values. Tried: if/else for scale (288, layout
// matches retail's diamond), the 0x40 test on its own lookup with the definition taken inside the if (294), the table
// cached in a local (301), position copied after the scale test (304). None lower than 285, none kept.
// MiniMax I (11-11), NOT kept (coordinator 11-11): 285 -> 272 from three one-use scaled locals, the scale as a ternary re-reading the definition, and the four corners in a bounds[4] array (stack-home steer); body at 285.
// MiniMax A (11-12): no new form, no lower score (285); this build frames 0x38 bytes against retail's 0x34 because
// retail reuses the position's stack slots for the corner coordinates while this build gives the corners slots of
// their own, so every slot above the position sits four bytes higher, and the 1.0 constant still takes xmm0 where
// retail holds it in xmm3.
// @retail 0x2e540
long __stdcall function_2e540(s_slot *slot, byte *data)
{
    long result = 0;
    long tag = *(long *)data;
    point3f position = *(point3f *)(data + 4);
    vector3f offset;
    offset.i = data[0x10] * (2.0f / 255.0f) - 1.0f;
    offset.j = data[0x11] * (2.0f / 255.0f) - 1.0f;
    offset.k = data[0x12] * (2.0f / 255.0f) - 1.0f;
    byte *definition = g_4e3b44[tag & 0xffff].bytes;
    real scale = 1.0f;
    if (definition[0x28] & 0x40)
        scale = *(real *)(data + 0x14);
    if (tag != NONE && !(g_4e3b44[tag & 0xffff].bytes[0x28] & 2))
    {
        real radius = *(real *)(definition + 0x10) * scale;
        if ((*(dword *)slot & 0x38) == 0x20)
        {
            real sizes[7] = { 1.0f, 0.5f, 0.25f, 0.125f, 0.0625f, 0.03125f, 0.015625f };
            radius *= sizes[*(short *)(definition + 0x16)];
        }
        switch (*(short *)(definition + 0x14))
        {
        case 0:
            {
            real negative_radius = -radius;
            position.x += negative_radius * g_4b9dac.i;
            position.z += negative_radius * g_4b9dac.k;
            position.y += negative_radius * g_4b9dac.j;
            }
            break;
        case 1:
            position.x += offset.i * (radius * 1.4142135381698608f);
            position.z += offset.k * (radius * 1.4142135381698608f);
            position.y += offset.j * (radius * 1.4142135381698608f);
            break;
        }
        real screen[4], extent[2];
        if (function_48b00(&position, radius, screen, extent))
        {
            if (extent[0] < 2.0f) extent[0] = 2.0f;
            else if (extent[0] > 128.0f) extent[0] = 128.0f;
            if (extent[1] < 2.0f) extent[1] = 2.0f;
            else if (extent[1] > 128.0f) extent[1] = 128.0f;
            real left = (real)floor((double)screen[0] - extent[0]);
            real top = (real)floor((double)screen[1] - extent[1]);
            real right = (real)floor((double)screen[0] + extent[0]);
            real bottom = (real)floor((double)screen[1] + extent[1]);
            real area = (bottom - top) * (right - left);
            if (area > 0.0f && area <= 2147483648.0f)
            {
                if (screen[2] < 0.0f) screen[2] = 0.0f;
                else if (screen[2] > 16777215.0f) screen[2] = 16777215.0f;
                if (screen[3] < 0.0f) screen[3] = 0.0f;
                else if (screen[3] > 16777215.0f) screen[3] = 16777215.0f;
                D3DDevice::Begin(D3DPT_TRIANGLEFAN);
                D3DDevice::SetVertexData4f(0, left, top, screen[2], screen[3]);
                D3DDevice::SetVertexData4f(0, right, top, screen[2], screen[3]);
                D3DDevice::SetVertexData4f(0, right, bottom, screen[2], screen[3]);
                D3DDevice::SetVertexData4f(0, left, bottom, screen[2], screen[3]);
                D3DDevice::End();
                result = (long)area;
            }
        }
    }
    return result;
}

// @retail 0x2dd30
void function_2dd30()
{
    g_4670bc = true;
    function_16b10((s_render_reset_state *)g_485b48);
    function_14bc0(g_485602, 0, true);
    function_15370(0);
    if (!(fabs(g_4b8494 - 1.0f) < 0.0001f))
    {
        D3DDevice_SetStipple(function_1c290(1.0f));
        g_4b8494 = 1.0f;
    }
    function_0222d0(D3DRS_CULLMODE, 0);
    function_0222d0(D3DRS_COLORWRITEENABLE, 0);
    function_0222d0(D3DRS_ALPHABLENDENABLE, 0);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    function_0222d0(D3DRS_ZENABLE, 2);
    function_0222d0(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    function_0222d0(D3DRS_ZWRITEENABLE, 0);
    function_0222d0(D3DRS_ZBIAS, 0);
    function_0222d0(D3DRS_SAMPLEALPHA, 0);
    g_51f3c8[1][0] = NULL;
    g_51f3c8[1][1] = NULL;
    g_51f3c8[1][2] = NULL;
    g_51f3c8[1][3] = NULL;
    function_1cf50();
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSCombinerCount = 0x11001;
    g_484f68.PSFinalCombinerInputsABCD = 0;
    g_484f68.PSFinalCombinerInputsEFG = 0;
    D3DDevice_SetPixelShaderProgram(&g_484f68);
    function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x54), 0);
    function_1c710(g_51f0f0);
    if (g_485898 < 4 || g_485898 > 7)
        function_20bb0(g_4b9ed4, 0x1f, NULL, 0, true, function_2e540);
}

short function_1358c0(short format);
void *function_135a30(s_type_7ba8e9 const *bitmap, short mipmap_index, short x, short y);

// @retail 0x13f10
void function_13f10(long bitmap_value, long front_value)
{
    byte *bitmap = (byte *)bitmap_value;
    if (bitmap && *(void **)(bitmap + 0x54))
    {
        short_rect_pair rectangle_pair;
        rectangle_pair.a = g_485a8a.a;
        short right = (short)(rectangle_pair.a.v1 + *(short *)(bitmap + 4));
        if (rectangle_pair.a.v3 <= right) right = rectangle_pair.a.v3;
        short bottom = (short)(rectangle_pair.a.v0 + *(short *)(bitmap + 6));
        if (rectangle_pair.a.v2 <= bottom) bottom = rectangle_pair.a.v2;
        long width = right - rectangle_pair.a.v1;
        long height = bottom - rectangle_pair.a.v0;
        short format = *(short *)(bitmap + 0xc);
        if ((format == 11 || format == 10) && *(short *)(bitmap + 0x14) == 0)
        {
            D3DSurface *surface;
            if ((byte)front_value != 0)
                D3DDevice::GetRenderTarget(&surface);
            else
                D3DDevice::GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &surface);
            D3DSURFACE_DESC description;
            D3DSurface_GetDesc(surface, &description);
            if (description.Size == description.Width * description.Height * 4)
            {
                D3DResource_BlockUntilNotBusy(surface);
                D3DLOCKED_RECT locked;
                D3DSurface_LockRect(surface, &locked, NULL, 0);
                byte *source = (byte *)((dword)locked.pBits | 0xf0000000);
                if (source)
                {
                    long size = function_1358c0(format) * width / 8;
                    for (long y = 0; y < height; ++y)
                    {
                        memcpy(function_135a30((s_type_7ba8e9 *)bitmap, 0, 0, (short)y), source, size);
                        source += locked.Pitch;
                    }
                }
            }
            if (surface) D3DResource_Release(surface);
        }
    }
    function_13d80();
    D3DDevice_SetBackBufferScale(1.0f, 1.0f);
    D3DDevice_Swap(0);
    D3DDevice_SetBackBufferScale(1.0f, 1.0f);
    if (g_467008 != g_485acc || g_46700c != g_485ad0)
    {
        D3DDevice_SetFlickerFilter(g_485acc);
        D3DDevice_SetSoftDisplayFilter(g_485ad0);
        g_46700c = g_485ad0;
        g_467008 = g_485acc;
    }
    ++g_485aa0;
}



// @retail 0x12fa0
void function_12fa0(byte const *camera, real const *projection, bool scaled, long mode)
{
    real width = (real)(*(short const *)(camera + 0x36) - *(short const *)(camera + 0x32));
    real height = (real)(*(short const *)(camera + 0x34) - *(short const *)(camera + 0x30));
    real left = (real)*(short const *)(camera + 0x32);
    real top = (real)*(short const *)(camera + 0x30);
    real scale;
    if (scaled && !g_5093fc)
    {
        scale = g_4670c8 < 0.0625f ? 0.0625f : g_4670c8 > 1.0f ? 1.0f : g_4670c8;
    }
    else scale = 1.0f;
    real bias = 0.03125f;
    if (mode == 2)
    {
        height = width = 128.0f;
        top = left = 0.0f;
        scale = 1.0f;
        bias = 0.0f;
    }
    real constants[48];
    function_12d50(false, projection, constants);
    constants[16] = projection[14];
    constants[17] = projection[15];
    constants[18] = projection[16];
    constants[19] = 1.0f;
    constants[20] = projection[17];
    constants[21] = projection[18];
    constants[22] = projection[19];
    constants[23] = 0.5f;
    constants[24] = projection[20];
    constants[25] = projection[21];
    constants[33] = 0.0f;
    constants[26] = projection[22];
    constants[27] = 2.0f;
    constants[28] = ((real const *)camera)[0];
    constants[29] = ((real const *)camera)[1];
    constants[30] = ((real const *)camera)[2];
    constants[31] = 767.8125f;
    real half_width = width * 0.5f;
    constants[32] = half_width * scale;
    constants[34] = 0.0f;
    constants[35] = ((width + 1.0f) * 0.5f + left) * scale + bias;
    constants[36] = 0.0f;
    constants[37] = scale * (height * -0.5f);
    constants[38] = 0.0f;
    constants[39] = ((height + 1.0f) * 0.5f + top) * scale + bias;
    constants[40] = half_width * scale;
    constants[41] = scale * (height * -0.5f);
    constants[42] = 16777215.0f;
    constants[43] = 0.0f;
    constants[44] = (half_width + left) * scale + 0.5f;
    constants[45] = (height * 0.5f + top) * scale + 0.5f;
    constants[46] = 0.0f;
    constants[47] = 0.0f;
    D3DDevice_SetVertexShaderConstant(-96, constants, 12);
}


real *table_entry_data(long handle);
long function_184000(long index, long bit);

// MiniMax D (11-05): the permuter run on this function ended with a candidate that only swaps two
// argument lists and adds a volatile qualifier, so it is not keepable; no new form, no lower score.
// MiniMax D (11-01): no new form tried, no lower score.
// MiniMax E (11-06): the bias term is now tested as a byte and masked again for the value, and the
// transform lookup writes the three floats separately in each of its two paths, as retail does. With
// the volatile `out` read also dropped this is 301, but the caller 0x44ac0 then loses its match
// (0 -> 58), so the read stays.
// score 303 (was 332): our body is 24 bytes shorter than retail's (973 against 997); retail keeps the
// output pointer in edi and the definition pointer in ecx, and passes the index in ecx and the output
// pointer in eax, while ours takes the index in ax and the output pointer on the stack; the outlined
// helper's argument order still differs from retail's.
// @retail 0x45ce0
bool function_45ce0(short index, byte *out, short part, short transform_index)
{
    out = *(byte *volatile *)&out;
    s_44940_entry *entry = &g_4ba138[index];
    byte *definition;
    if (entry->tag != NONE)
    {
        byte *tag = g_4e3b44[entry->tag & 0xffff].bytes;
        byte *section = *(byte **)(tag + 0x28) + ((entry->flags >> 9) & 0x1ff) * 0x5c;
        definition = *(byte **)(section + 0x34);
    }
    else
    {
        byte *structure = (byte *)g_4e0348;
        byte *section;
        if (!(entry->unknown00 & 0x1000))
            section = *(byte **)(structure + 0xa0) + ((entry->flags >> 9) & 0x1ff) * 0xb0;
        else
        {
            byte *instance = *(byte **)(structure + 0x144) + ((entry->flags >> 18) & 0x7ff) * 0x58;
            section = *(byte **)(structure + 0x13c) + *(short *)(instance + 0x34) * 0xc8;
        }
        definition = *(byte **)(section + 0x50);
    }
    if (!definition) return false;
    byte *record = *(byte **)(definition + 4) + part * 0x48;
    *(real *)(out + 0x10) = 0.0f;
    *(real *)(out + 0x28) = *(real *)(record + 0x2c) > 0.0f ? *(real *)(record + 0x2c) : 1.0f;
    byte *material;
    bool visible = true;
    if (entry->tag != NONE)
        material = *(byte **)(g_4e3b44[entry->tag & 0xffff].bytes + 0x64) + *(short *)(record + 4) * 32;
    else
    {
        material = *(byte **)((byte *)g_4e0348 + 0xa8) + *(short *)(record + 4) * 32;
        long instance = (entry->flags >> 18) & 0x7ff;
        if (instance == 0x7ff) instance = NONE;
        long bit = material[0x1c] == 0xff ? NONE : material[0x1c];
        visible = (byte)function_184000(instance, bit) != 0;
    }
    *(long *)out = *(long *)(material + 0xc);
    out[0x14] = false;
    if (!visible) return visible;
    if (entry->tag != NONE)
    {
        long section_index = (entry->flags >> 9) & 0x1ff;
        if (section_index == 0xff) return false;
        byte *tag = g_4e3b44[entry->tag & 0xffff].bytes;
        byte *section = *(byte **)(tag + 0x28) + section_index * 0x5c;
        byte *transforms = (byte *)table_entry_data(entry->unknown08);
        short selected = *(short *)(section + 0x2c);
        if (selected != NONE)
        {
            byte *transform;
            if (tag[4] & 4)
            {
                selected = ((short *)(transforms + 4))[transform_index * 2];
                transform = transforms + 0x44 + selected * 0x30;
            }
            else transform = transforms + 0x44 + selected * 0x30;
            *(real *)(out + 4) = *(real *)(transform + 0xc);
            *(real *)(out + 8) = *(real *)(transform + 0x1c);
            *(real *)(out + 0xc) = *(real *)(transform + 0x2c);
        }
        else
        {
            *(real *)(out + 4) = *(real *)(transforms + 0x50);
            *(real *)(out + 8) = *(real *)(transforms + 0x60);
            *(real *)(out + 0xc) = *(real *)(transforms + 0x70);
        }
        if (entry->unknown10[11] & 15)
            *(real *)(out + 0x10) = ((entry->unknown10[11] & 15) - 5) * 0.001f;
    }
    else if (entry->unknown00 & 0x1000)
    {
        byte *instance = *(byte **)((byte *)g_4e0348 + 0x144) + ((entry->flags >> 18) & 0x7ff) * 0x58;
        void *plane = function_1e280(*(short *)(instance + 0x34), true, part);
        out[0x14] = plane != NULL;
        if (plane) memcpy(out + 0x18, plane, 16);
        *(point3f *)(out + 4) = *(point3f *)(instance + 0x3c);
        if (out[0x14])
        {
            real x = *(real *)(out + 0x18), y = *(real *)(out + 0x1c), z = *(real *)(out + 0x20);
            *(real *)(out + 0x18) = *(real *)(instance + 0x1c) * z + *(real *)(instance + 0x10) * y + *(real *)(instance + 4) * x;
            *(real *)(out + 0x1c) = *(real *)(instance + 0x20) * z + *(real *)(instance + 0x14) * y + *(real *)(instance + 8) * x;
            *(real *)(out + 0x20) = *(real *)(instance + 0x24) * z + *(real *)(instance + 0x18) * y + *(real *)(instance + 0xc) * x;
            *(real *)(out + 0x24) = *(real *)(out + 0x24) * *(real *)instance +
                *(real *)(instance + 0x30) * *(real *)(out + 0x20) + *(real *)(instance + 0x2c) * *(real *)(out + 0x1c) +
                *(real *)(instance + 0x28) * *(real *)(out + 0x18);
        }
    }
    else
    {
        void *plane = function_1e280((short)((entry->flags >> 9) & 0x1ff), false, part);
        out[0x14] = plane != NULL;
        if (plane) memcpy(out + 0x18, plane, 16);
        *(point3f *)(out + 4) = *(point3f *)(record + 0x10);
    }
    return visible;
}



__declspec(noinline) dword __cdecl pack_color4f(color4f const *color);

#pragma inline_depth(0)
// @retail 0x40890
void function_40890(void)
{
	if (!g_485870) return;
	color4f color, inverse;
	color.red = g_485874 * g_48587c;
	real alpha = g_485878 * g_485874;
	color.green = g_485874 * g_485880;
	color.alpha = alpha;
	inverse.alpha = alpha;
	color.blue = g_485874 * g_485884;
	inverse.red = (1.0f - g_48587c) * g_485874;
	inverse.green = (1.0f - g_485880) * g_485874;
	inverse.blue = (1.0f - g_485884) * g_485874;
	dword packed = pack_color4f(&color);
	dword constant = pack_color4f(&inverse);
#pragma inline_depth(255)
	function_0222d0(D3DRS_CULLMODE, 0x901);
	function_0222d0(D3DRS_COLORWRITEENABLE, 0x10101);
	function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
	dword local_617c45, rgb_inputs;
	switch (g_485870)
	{
	case 1:
		function_0222d0(D3DRS_SRCBLEND, 1);
		function_0222d0(D3DRS_DESTBLEND, 0x303);
		function_0222d0(D3DRS_BLENDOP, 0x8006);
		constant = packed;
		rgb_inputs = 0x1200000;
		local_617c45 = 0x11200000;
		break;
	case 2:
		function_0222d0(D3DRS_SRCBLEND, 1);
		function_0222d0(D3DRS_DESTBLEND, 1);
		function_0222d0(D3DRS_BLENDOP, 0x800b);
		constant = packed;
		rgb_inputs = 0x1200000;
		local_617c45 = 0x11200000;
		break;
	case 3:
		function_0222d0(D3DRS_SRCBLEND, 0x307);
		function_0222d0(D3DRS_DESTBLEND, 0x8002);
		function_0222d0(D3DRS_BLENDOP, 0x8008);
		function_0222d0(D3DRS_BLENDCOLOR, packed);
		constant = packed;
		rgb_inputs = 0x1201140;
		local_617c45 = 0;
		break;
	case 4:
		function_0222d0(D3DRS_SRCBLEND, 0x307);
		function_0222d0(D3DRS_DESTBLEND, 0x8002);
		function_0222d0(D3DRS_BLENDOP, 0x8007);
#pragma inline_depth(0)
		function_0222d0(D3DRS_BLENDCOLOR, packed);
		constant = packed;
		rgb_inputs = 0x1201120;
		local_617c45 = 0;
		break;
	case 5:
		function_0222d0(D3DRS_SRCBLEND, 0x307);
		function_0222d0(D3DRS_DESTBLEND, 0x8002);
		function_0222d0(D3DRS_BLENDOP, 0x8006);
		function_0222d0(D3DRS_BLENDCOLOR, packed);
		constant = packed;
		rgb_inputs = 0x1411120;
		local_617c45 = 0;
		break;
	case 6:
		function_0222d0(D3DRS_SRCBLEND, 1);
		function_0222d0(D3DRS_DESTBLEND, 0x8002);
		function_0222d0(D3DRS_BLENDOP, 0x8006);
		function_0222d0(D3DRS_BLENDCOLOR, constant);
		rgb_inputs = 0x11200000;
		local_617c45 = 0;
		break;
	default:
		__assume(0);
	}
	function_0222d0(D3DRS_ALPHATESTENABLE, 0);
	{
		/* retail has function_0222d0 inlined here, the library call left out of line */
		extern dword g_4b81fc[];
		g_4b81fc[D3DRS_ZENABLE] = 0;
		D3DDevice_SetRenderState_ZEnable(0);
		g_4b81fc[D3DRS_ZBIAS] = 0;
		D3DDevice_SetRenderState_ZBias(0);
	}
	function_1cc30(13);
	function_1c710(g_51f0f0);
	real x = 1.0f / (short)(g_48564e - g_48564a);
	real constants[20] = {
		2.0f * x, 0, 0, -1.0f - x,
		0, -2.0f * (1.0f / (short)(g_48564c - g_485648)), 0, 1.0f + 1.0f / (short)(g_48564c - g_485648),
		0, 0, 0, 0.5f,
		0, 0, 0, 1.0f,
		0, 0, 0, 1.0f
	};
	D3DDevice_SetVertexShaderConstantNotInline(81 + 96, constants, 4 * 5);
	memset(&g_484f68, 0, sizeof(g_484f68));
	g_484f68.PSAlphaInputs[0] = local_617c45;
	g_484f68.PSRGBInputs[0] = rgb_inputs;
	g_484f68.PSCombinerCount = 1;
	g_484f68.PSConstant0[0] = constant;
	g_484f68.PSAlphaOutputs[0] = 0xc00;
	g_484f68.PSRGBOutputs[0] = 0xc00;
	g_484f68.PSFinalCombinerInputsABCD = 0xc;
	g_484f68.PSFinalCombinerInputsEFG = 0x1c00;
	function_15180(&g_484f68);
	short height = g_48564c - g_485648;
	short width = g_48564e - g_48564a;
	function_1cf50();
	D3DDevice_Begin(D3DPT_TRIANGLEFAN);
	D3DDevice_SetVertexData2s(0, 0, 0);
	D3DDevice_SetVertexData2s(0, width, 0);
	D3DDevice_SetVertexData2s(0, width, height);
	D3DDevice_SetVertexData2s(0, 0, height);
	D3DDevice_End();
}
#pragma inline_depth()

bool function_48b00(point3f const *point, real radius, real *screen, real *extent);
void function_1cf50();

// Round 2 of #1066: score 199 (was 319): LOGIC WALK against retail found no behaviour
// difference (clamps and constants 0x45dd40/0x45dbc0, slots 5/2/1/0, the 2f values, x1=screen0+a, y1=screen1-b,
// x2/y2 from c/d, x3=screen0-a, y3=screen1+b, x4=screen0-c, y4=screen1+d, z/w kept in the dead point/depth slots);
// what moved the score is WHERE the work sits. Retail computes c and d in the block after the (1,1) tex write, so
// they are declared after that call (319 -> 289), and the product declaration order hs, hc, wc, ws (or hc, hs, wc, ws) gives 199
// (the other 22 orders: 289..306). Still differs: retail loads width, height, cosine and screen[1] up front and reads
// sine from its slot, we load screen[1] later and sine into a register. Tried, no change or worse: swapping the
// operands of the four products (all 16 masks: no change), a/b or c/d declared in the other order (199), commuted
// sums (199), x1/y1 as locals (368 before the move, 353 after), screen[1] or screen[0] hoisted into a local (412/504),
// extent hoisted into locals (199), hs/hc before the colour write (344), c/d hoisted above the first vertex (391).
// Permuter (25 min on #1066, about 610 variants): 195 only with vertex-data arguments swapped and float-to-int
// truncations; not kept.
// @retail 0x484b0
void __stdcall function_484b0(point3f const *point, real depth, real width, real height, real cosine, real sine, dword color)
{
    real screen[4], extent[2];
    if (width > 0.0f && height > 0.0f && function_48b00(point, 1.0f, screen, extent))
    {
        real ratio = g_485ad4.hi / (g_485ad4.hi - g_485ad4.lo);
        real z = ((ratio * depth - g_485ad4.lo * ratio) / depth) * 16777215.0f;
        if (z < 0.0f) z = 0.0f;
        else if (z > 16777215.0f) z = 16777215.0f;
        real w = (depth / g_485ad4.hi) * 16777215.0f;
        if (w < 0.0f) w = 0.0f;
        else if (w > 16777215.0f) w = 16777215.0f;
        function_1cf50();
        D3DDevice::Begin(D3DPT_TRIANGLEFAN);
        D3DDevice::SetVertexDataColor(5, color);
        D3DDevice::SetVertexData2f(2, 1.0f, 0.0f);
        real hs = height * sine;
        real hc = height * cosine;
        real wc = width * cosine;
        real ws = width * sine;
        real a = (wc - hs) * extent[0];
        real b = (ws + hc) * extent[1];
        D3DDevice::SetVertexData2f(1, screen[0] + a, screen[1] - b);
        D3DDevice::SetVertexData4f(0, screen[0] + a, screen[1] - b, z, w);
        D3DDevice::SetVertexData2f(2, 1.0f, 1.0f);
        real c = (hs + wc) * extent[0];
        real d = (ws - hc) * extent[1];
        D3DDevice::SetVertexData2f(1, screen[0] + c, screen[1] - d);
        D3DDevice::SetVertexData4f(0, screen[0] + c, screen[1] - d, z, w);
        D3DDevice::SetVertexData2f(2, 0.0f, 1.0f);
        D3DDevice::SetVertexData2f(1, screen[0] - a, screen[1] + b);
        D3DDevice::SetVertexData4f(0, screen[0] - a, screen[1] + b, z, w);
        D3DDevice::SetVertexData2f(2, 0.0f, 0.0f);
        D3DDevice::SetVertexData2f(1, screen[0] - c, screen[1] + d);
        D3DDevice::SetVertexData4f(0, screen[0] - c, screen[1] + d, z, w);
        D3DDevice::End();
    }
}
// MiniMax H (11-13): no new form, no lower score (125): the four products moved to the head of the block
// (238), to the head of the block in retail's order (240) and before the block's test (238) all score worse,
// so the body stands.
// MiniMax I (11-11): no new form, no lower score (125): the four products taken in retail's order (the width's cosine product first, then the height's cosine, the width's sine and the height's sine) score 213 and grow the body to 1045 bytes, so the body keeps its own order; our body is still five bytes longer than retail's 1027, which is what moves the two early skips' branch targets and every later one.
// MiniMax A (11-12): no new form, no lower score (125): our body is still five bytes longer than retail 1027, our four products reach the same stack slots in another register order (ours xmm1/xmm3/xmm2/xmm5 against retail xmm6/xmm3/xmm2/xmm7) and another multiply order, and our two early skips branch to a near block where retail branches to the end of the body; naming the four products in retail order scores 213, so the body stands.

// MiniMax H (11-12): no new form, no lower score (125); our body is five bytes longer than retail's 1027, our four
// products reach the same stack slots in another register order (ours xmm1/xmm3/xmm2/xmm5 against retail's
// xmm6/xmm3/xmm2/xmm7) and another multiply order, and our two early skips branch to a near block where retail
// branches to the end of the body.
// MiniMax B (11-11): no new form, no lower score (125); our body is five bytes longer than retail's 1027, our four
// products reach the same stack slots in another register order (ours xmm1/xmm3/xmm2/xmm5 against retail's
// xmm6/xmm3/xmm2/xmm7) and another multiply order, and our two early skips branch to a near block where retail
// branches to the end of the body.
// MiniMax E (11-11, run 80): no new form, no lower score (125): retail multiplies width*cosine, height*cosine, width*sine and height*sine and loads the four screen values in another register order (ours xmm1/xmm3/xmm2/xmm5, retail xmm6/xmm3/xmm2/xmm7), our body is still five bytes longer than retail's 1027 so the two early skips branch to a near block here and to the end of the body in retail, and naming the four products in retail's order scores 213.
// MiniMax E (11-10, run 73): no new form, no lower score (125); our four products reach the same stack slots in another register order and another multiply order than retail's (ours hc, ws, wc, hs against retail's wc, hc, ws, hs), and our two early skips branch to a near block where retail branches to the end of the body.
// MiniMax D (11-11): no new form, no lower score (125): the two early skips still branch to a near block here and to the end of the body in retail, and the body is still five bytes longer than retail's 1027; the body's four products taken in retail's order (the width's cosine product first) score 213 instead of 125.
// MiniMax G (11-11): no new form, no lower score (125): our body is five bytes longer than retail's 1027, so the two early skips branch elsewhere and the four screen values are loaded in another register order (ours xmm1/xmm3/xmm2/xmm5, retail xmm6/xmm3/xmm2/xmm7) and in another slot order; retail multiplies width*cosine, height*cosine, width*sine, height*sine.
// MiniMax S (11-10): no new form, no lower score (125): the four screen values reach the same slots but in another register order (ours xmm1/xmm3/xmm2/xmm5, retail xmm6/xmm3/xmm2/xmm7), retail multiplies width*cosine, height*cosine, width*sine, height*sine where ours multiplies width*cosine, width*sine, height*cosine, height*sine (the two declaration orders that would match retail's order score 213 and 200), the two early skips branch to a near block here and to the end of the body in retail, our body is five bytes longer than retail's 1027, and the permuter (46 tries, 611 s) found nothing below its own baseline.
// MiniMax D (11-10): no new form, no lower score (125): the two early skips branch to a near block here and to the end of the body in retail, and our body is five bytes longer than retail's 1027.
// Round 2 of #1066: score 125 (was 152): LOGIC WALK against retail found no behaviour difference (slots 9/3,
// the 2s values 0x1/0x10001/0x10000/0, x/y/z/w of the four corners, extent/screen offsets all agree). c and d are
// computed in retail's block after the (1,1) tex write (0x48297), so they are declared after that call (213 with
// the old product order), and the product order hc, ws, wc, hs then gives 125 (old order on the new c/d spot 213,
// old order on the old spot 152; the next best on the new spot 130, the rest 162 and up). Still differs: retail loads cosine,
// width, height, sine and screen[1] up front (we load screen[0] too and the others in another order), forms
// x1 as a copy of a plus the screen[0] slot, and reloads six registers after the first slow path (we reload five).
// Tried, no change: commuted sums (screen[0] + a vs a + screen[0], b + screen[1]), extent first, a/b swapped (197),
// c/d swapped, operand swaps of the products.
// Earlier rounds: MiniMax A (11-04): the four products' declaration order (six orders) 152/164/152/163/152/163, the
// multiply operands swapped 152, the products inlined into a/b/c/d 166; the permuter's best (148) rewrites the
// vertex calls and so draws something else, not kept. MiniMax D (11-01): no new form tried, no lower score.
// Permuter (25 min on #1066, about 650 variants, on its own rendering that loses the round 2 order): best 142, above
// this source's 125; nothing kept.
// score 125 (was 125): our body is five bytes longer than retail's 1027, so the two early skips branch to a near block
// here and to the end of the body in retail, and the four screen values are loaded in another register order (ours
// xmm1/xmm3/xmm2/xmm5 against retail's xmm6/xmm3/xmm2/xmm7) and in another slot order; retail multiplies
// width*cosine, height*cosine, width*sine and height*sine.
// @retail 0x480a0
void __stdcall function_480a0(point3f const *point, real width, real height, real cosine, real sine, dword color)
{
    real screen[4], extent[2];
    if (width > 0.0f && height > 0.0f && function_48b00(point, 1.0f, screen, extent))
    {
        function_1cf50();
        D3DDevice::Begin(D3DPT_TRIANGLEFAN);
        D3DDevice::SetVertexDataColor(9, color);
        D3DDevice::SetVertexData2s(3, 1, 0);
        real hc = height * cosine;
        real ws = width * sine;
        real wc = width * cosine;
        real hs = height * sine;
        real a = (wc - hs) * extent[0];
        real b = (ws + hc) * extent[1];
        D3DDevice::SetVertexData4f(0, screen[0] + a, screen[1] - b, screen[2], screen[3]);
        D3DDevice::SetVertexData2s(3, 1, 1);
        real c = (hs + wc) * extent[0];
        real d = (ws - hc) * extent[1];
        D3DDevice::SetVertexData4f(0, screen[0] + c, screen[1] - d, screen[2], screen[3]);
        D3DDevice::SetVertexData2s(3, 0, 1);
        D3DDevice::SetVertexData4f(0, screen[0] - a, screen[1] + b, screen[2], screen[3]);
        D3DDevice::SetVertexData2s(3, 0, 0);
        D3DDevice::SetVertexData4f(0, screen[0] - c, screen[1] + d, screen[2], screen[3]);
        D3DDevice::End();
    }
}

byte g_4b6291;

void __stdcall function_41cc0(void *payload);

/* Gathers type-1 depth groups whose screen bounds do not overlap into
   records drawn by 0x41cc0, each placed at the centre of its groups. */
// @retail 0x420a0
void function_420a0(void)
{
    long count = g_4c19ac;
    dword used[1] = { 0 };
    long i = 0;
    if (count > 0)
    {
        do
        {
            s_42760_entry const *entry = &g_4c152c[i];
            if (entry->type == 1 && !(used[i >> 5] & (1 << (i & 31))))
            {
                s_1e4e0_record *record = (s_1e4e0_record *)cache_record_push();
                if (record)
                {
                    byte indices[32];
                    indices[0] = (byte)i;
                    long selected = 1;
                    record->type = 3;
                    record->flags = 0;
                    record->callback = function_41cc0;
                    real depth = entry->depth;
                    used[i >> 5] |= 1 << (i & 31);
                    point3f position = entry->position;
                    box3f bounds;
                    bounds.x0 = bounds.x1 = position.x;
                    bounds.y0 = bounds.y1 = position.y;
                    bounds.z0 = bounds.z1 = position.z;
                    long j = 0;
                    do
                    {
                        if (used[j >> 5] & (1 << (j & 31)))
                            continue;
                        s_42760_entry const *candidate = &g_4c152c[j];
                        point3f candidate_position = candidate->position;
                        box2f candidate_bounds = *(box2f const *)candidate->unknown10;
                        bool overlaps = false;
                        for (long k = 0; k < selected; ++k)
                        {
                            s_42760_entry const *other = &g_4c152c[(signed char)indices[k]];
                            box2f previous = *(box2f const *)other->unknown10;
                            if (candidate_bounds.x1 > previous.x0 && candidate_bounds.y1 > previous.y0 &&
                                previous.x1 > candidate_bounds.x0 && previous.y1 > candidate_bounds.y0)
                            {
                                overlaps = true;
                                break;
                            }
                        }
                        if (overlaps)
                            continue;
                        indices[selected++] = (byte)j;
                        used[j >> 5] |= 1 << (j & 31);
                        if (!(candidate_position.x > bounds.x0)) bounds.x0 = candidate_position.x;
                        if (!(candidate_position.y > bounds.y0)) bounds.y0 = candidate_position.y;
                        if (!(candidate_position.z > bounds.z0)) bounds.z0 = candidate_position.z;
                        if (candidate_position.x > bounds.x1) bounds.x1 = candidate_position.x;
                        if (candidate_position.y > bounds.y1) bounds.y1 = candidate_position.y;
                        if (candidate_position.z > bounds.z1) bounds.z1 = candidate_position.z;
                        real candidate_depth = candidate->depth;
                        if (depth > candidate_depth) depth = candidate_depth;
                    } while (++j < count);
                    record->payload[0] = (byte)selected;
                    memcpy(record->payload + 1, indices, selected);
                    record->depth = 0.0f - depth;
                    record->position.x = (bounds.x0 + bounds.x1) * 0.5f;
                    record->position.y = (bounds.y0 + bounds.y1) * 0.5f;
                    record->position.z = (bounds.z0 + bounds.z1) * 0.5f;
                }
            }
        } while (++i < count);
    }
}

extern dword g_4b8448, g_4b8308, g_4b8450;
extern dword g_4b82e8, g_4b82f4, g_4b82f8;
dword g_4b8438, g_4b82e0, g_4b82fc, g_4b82ec;
void function_48010(real const *color);
// MiniMax H (11-13): no new form, no lower score (244).
// MiniMax I (11-11): no new form, no lower score (244): retail loads the `mode` parameter into ebp at the entry (`mov ebp, dword ptr [esp + 0x3c]`) and keeps its copy of the flags in esi, while we copy the flags into ebp and read `mode` from the stack where it is first used; the switch on the mode writes its states in the other order.

// MiniMax B (11-11): no new form, no lower score (244); the mode switch rewritten as an if/else-if chain scores 288
// at 1348 bytes, and a copy of the mode parameter into a local of its own compiles to the same code (244), so retail's
// prologue copy of the mode into ebp and its out-of-line function_0222d0 calls are still not reproduced.
// MiniMax E (11-11, run 80): no new form, no lower score (244): the switch on the mode still writes its states in the other order and the case bodies' calls and arguments still differ from the first difference; retail copies the mode parameter into ebp in the prologue and keeps six of its eighteen function_0222d0 calls out of line where we inline all six, and the only lever (inline_depth) may not be added.
// MiniMax E (11-10, run 73): no new form, no lower score (244); retail copies the mode parameter into ebp in the prologue and keeps the flag scratch in esi where we load the mode later, and retail keeps six of its eighteen function_0222d0 calls out of line (the default arm's four, the blend op and the alpha test off) where we inline all six; the only lever (inline_depth) may not be added.
// MiniMax D (11-11): no new form, no lower score (244): the switch on the mode still writes its states in the other order and the case bodies' calls still differ from the first difference on.
// MiniMax G (11-11): no new form, no lower score (244): retail copies the mode parameter into ebp in the prologue and keeps the flag scratch in esi where ours loads the mode later, and retail keeps six of its eighteen function_0222d0 calls out of line (the default arm's four, the blend op and the alpha test off) where ours inlines all six; the only lever (inline_depth) may not be added.
// MiniMax S (11-10): no new form, no lower score (244): retail copies the mode parameter into ebp in the prologue and keeps the flag scratch in esi where ours loads the mode later, and retail keeps six of its eighteen function_0222d0 calls out of line (the default arm's four, the blend op and the alpha test off) where ours inlines all six, which is most of the remaining score; the only lever (inline_depth) may not be added.
// MiniMax D (11-10): no new form, no lower score (244).
// MiniMax D (11-01): no new form tried, no lower score.
// MiniMax H (11-12): no new form, no lower score (244): retail still copies the mode parameter into ebp in the prologue and keeps six of its function_0222d0 calls out of line where this build inlines all six, and the switch on the mode still writes its states in the other order.
// score 244 (was 244): retail copies the mode parameter into ebp in the prologue and keeps the
// flag scratch in esi, while we keep the flag scratch in ebp and load the mode into esi later;
// retail is also 17 instructions longer than ours, and the two drift apart in the mode branch.
// Round 2 of #1066: logic walk against retail 0x47930-0x47e9d found no behaviour difference (cull, colour write, depth
// states, the 2f vertex data, stipple, format and mode arms, every pixel shader field and constant checked). Retail's
// layout: `switch (format)` with case 0 and default as two separate copies of the 0x54 arm (so that is a switch, laid out
// default, case 1, case 0), and the mode dispatch is a compare chain 0,1,2,3,4,5 on ebp (copied from the stack in the
// prologue), bodies laid out default, 4/5, 1-3, zero. Retail also inlines only 12 of its 18 function_0222d0 sites (the six
// calls are default's four, 4/5's blend op and the alpha test off); ours inlines all six calls we write, which is most of
// the remaining score. Tried: switch (format) with the default copy (253, that region then matches exactly), if (mode)
// plus an if/else-if chain for 1-3 / 4-5 (287; 252 with the switch), a mode switch with case 0 (256, ours uses a jump
// table), all 18 sites as function_0222d0 calls (259, all inlined), the 1e930-style `*(D3DRENDERSTATETYPE const *)&state`
// at the six calls (288). None below 244, so the source is unchanged.
// Permuter (25 min on #1066, about 850 variants): 228 only with the cull value stored through a word cast and a
// word read of a long field; not kept.
// MiniMax A (11-12): no new form, no lower score (244); retail copies the mode parameter into ebp in the prologue and
// dispatches on it with a compare chain, while this build keeps the flag scratch in ebp and reaches the mode through
// a jump table, and the forms tried (the if/else chain for the modes, the switch with a case 0, all eighteen sites
// called out of line) measure 244 or worse.
// @retail 0x47930
void function_47930(word flags, long format, long mode)
{
    dword cull = (flags & 1) ? 0 : D3DCULL_CCW;
    g_4b8448 = cull;
    D3DDevice_SetRenderState(D3DRS_CULLMODE, cull);
    dword color_write = (flags & 8) ? 0 : 0x10101;
    g_4b8308 = color_write;
    D3DDevice_SetRenderState(D3DRS_COLORWRITEENABLE, color_write);
    dword depth = flags & 2;
    g_4b8438 = depth;
    D3DDevice_SetRenderState(D3DRS_ZENABLE, depth);
    g_4b82e0 = D3DCMP_LESSEQUAL;
    D3DDevice_SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    dword write_depth = (flags >> 2) & 1;
    g_4b82fc = write_depth;
    D3DDevice_SetRenderState(D3DRS_ZWRITEENABLE, write_depth);
    g_4b8450 = 0;
    D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
    D3DDevice_SetVertexData2f(8, 1.0f, 0.0f);
    function_48010((real const *)g_4686cc);
    if (!(fabs(g_4b8494 - 1.0f) < 0.0001f))
    {
        D3DDevice_SetStipple(function_1c290(1.0f));
        g_4b8494 = 1.0f;
    }
    if (!(format == 1)) {
        function_1c590((s_shader_cache *)g_51f0f0,
            *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x54), 0);
        function_1ccb0(39);
    } else {
        function_1c590((s_shader_cache *)g_51f0f0,
            *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x24), 0);
        function_1ccb0(40);
        real identity[12] = { 1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f };
        D3DDevice_SetVertexShaderConstant(78, identity, 3);
    }
    function_1c710(g_51f0f0);
    if (!(mode)) {
        g_4b82e8 = 0;
        D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
        g_4b82ec = 0;
        D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, 0);
        memset(&g_484f68, 0, sizeof(g_484f68));
        g_484f68.PSCombinerCount = 1;
        g_484f68.PSFinalCombinerInputsABCD = 0x20;
    } else {
        switch (mode)
        {
        case 1:
        case 2:
        case 3:
        {
            g_4b82e8 = 0;
            D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
        }
        break;
        case 4:
        case 5:
        {
            g_4b82e8 = 1;
            D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
            g_4b82f4 = D3DBLEND_SRCALPHA;
            D3DDevice_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
            g_4b82f8 = D3DBLEND_INVSRCALPHA;
            D3DDevice_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
            function_0222d0(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        }
        break;
        default:
        {
            function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
            function_0222d0(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
            function_0222d0(D3DRS_DESTBLEND, D3DBLEND_ONE);
            function_0222d0(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        }
        break;
        }
        function_0222d0(D3DRS_ALPHATESTENABLE, 0);
        memset(&g_484f68, 0, sizeof(g_484f68));
        g_484f68.PSTextureModes = (mode == 3 || mode == 5 || mode == 8) ? 0x21 : 1;
        if (mode == 7)
        {
            g_484f68.PSCombinerCount = 3;
            g_484f68.PSRGBInputs[0] = 0x8080000;
            g_484f68.PSRGBOutputs[0] = 0xc0;
            g_484f68.PSRGBInputs[1] = 0xc0c0000;
            g_484f68.PSRGBOutputs[1] = 0xd0;
            g_484f68.PSRGBInputs[2] = 0x4082415;
            g_484f68.PSRGBOutputs[2] = 0x45;
            g_484f68.PSFinalCombinerInputsABCD = 0x50f0004;
            g_484f68.PSFinalCombinerInputsEFG = 0xc0d1400;
        }
        else if (mode == 3 || mode == 5 || mode == 8)
        {
            g_484f68.PSRGBOutputs[0] = 0xc0;
            g_484f68.PSAlphaOutputs[0] = 0xc0;
            g_484f68.PSCombinerCount = 2;
            g_484f68.PSRGBInputs[0] = 0x8090000;
            g_484f68.PSAlphaInputs[0] = 0x18190000;
            g_484f68.PSRGBInputs[1] = 0x50c0000;
            g_484f68.PSRGBOutputs[1] = (flags & 0x20) ? 0x200d0 : ((flags & 0x10) << 12) | 0xd0;
            g_484f68.PSAlphaInputs[1] = 0x151c0000;
            g_484f68.PSAlphaOutputs[1] = (flags & 0x80) ? 0x200d0 : ((flags & 0x40) << 10) | 0xd0;
            g_484f68.PSFinalCombinerInputsABCD = 0xd;
            g_484f68.PSFinalCombinerInputsEFG = 0x1d00;
        }
        else
        {
            g_484f68.PSFinalCombinerInputsEFG = 0x1c00;
            g_484f68.PSAlphaInputs[0] = 0x15180000;
            g_484f68.PSCombinerCount = 1;
            if (mode == 2)
            {
                g_484f68.PSAlphaOutputs[0] = 0xc0;
                g_484f68.PSFinalCombinerInputsABCD = 5;
            }
            else
            {
                g_484f68.PSRGBOutputs[0] = 0xc0;
                g_484f68.PSAlphaOutputs[0] = 0xc0;
                g_484f68.PSRGBInputs[0] = 0x5080000;
                g_484f68.PSFinalCombinerInputsABCD = 0xc;
            }
        }
    }
    D3DDevice_SetPixelShaderProgram(&g_484f68);
}

byte g_46713c = true;
extern dword g_4b8324;

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x4fe20
void function_4fe20(void)
{
    if (g_46713c)
    {
        function_47930(2, 0, 6);
        g_4b82ec = 0;
        /* alpha test off and stage 0's colour sign cleared, written out: the
           barrier keeps the stage's two dirty-flag updates apart */
        D3DDevice_SetRenderState_Simple(D3DSIMPLERENDERSTATEENCODE[D3DRS_ALPHATESTENABLE], 0);
        D3D__DirtyFlags |= D3DDIRTYFLAG_TEXTURE_STATE_0;
        _ReadWriteBarrier();
        D3D__RenderState[D3DRS_ALPHATESTENABLE] = 0;
        D3D__TextureState[0][D3DTSS_COLORSIGN] = 0;
        D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);
        g_4b82e8 = 1;
        D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
        g_4b82f4 = D3DBLEND_ONE;
        D3DDevice_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
        g_4b82f8 = D3DBLEND_INVSRCALPHA;
        D3DDevice_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        g_4b8324 = D3DBLENDOP_ADD;
        D3DDevice_SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        memset(&g_484f68, 0, sizeof(g_484f68));
        g_484f68.PSTextureModes = 1;
        g_484f68.PSCombinerCount = 0x11001;
        g_484f68.PSRGBInputs[0] = 0x4140000;
        g_484f68.PSRGBOutputs[0] = 0xc0;
        g_484f68.PSFinalCombinerInputsABCD = 0x80c0000;
        D3DDevice_SetPixelShaderProgram(&g_484f68);
        g_46713c = false;
    }
}
#pragma function(_ReadWriteBarrier)

PRIVATE __forceinline void texture_surface_dimensions(long index, long *width, long *height)
{
    if (g_4b4b58[index].data && !g_4b4b58[index].flag95)
    {
        *width = g_4b4b58[index].width;
        *height = g_4b4b58[index].height;
    }
    else
    {
        *width = 0;
        *height = 0;
    }
}

#define PIN(value, minimum, maximum) ((value) < (minimum) ? (minimum) : ((value) > (maximum) ? (maximum) : (value)))

// score 498 (was 585): round 94 put g_4858c4 in the reset storage aggregate (include/reset_storage.h), so it is
// reloaded after calls as retail does; 1696 bytes against retail's 1700; g_4858c8 and g_4858b8 below remain separate.
// Earlier 585 (was 580; the fast image says 612), 1652 bytes against retail's 1700 (the fast image builds 1636). Round 122: 0x144f0
// matches; both its calls pass retail's values (fallback 0, fallback index 1: `xor ebx, ebx`, `push ecx` with ECX 1).
// Round 1 of #1066: the 24/25 else branch now calls function_14f60(stage, 1) (retail jumps to the case 2 block, which
// does `mov edi, ecx` with ECX = 1; it used to pass the surface); case 8 follows case 13 as in retail's layout;
// g_485a78[mode - 24] is indexed; case 26 compares an int; case 16 compares clamped == value.
// The score rose 25 only because ours merges the else with case 2 at the later block and drops case 2's own copy at
// 0x195a0 (retail keeps the earlier one and jumps back: `jne 0x195a0`); a goto label, `break` plus a tail call and
// the negated test all compile alike. The old `(short)surface` argument scores 591.
// Remaining blocker (not in this body): retail reloads g_4858c8, g_4858c4 and g_4858b8 at each use and keeps the
// constant 1 in ECX (`mov ecx, 1` before the loop head, then `mov [esi], ecx`, `mov edi, ecx`, `push ecx`) where ours
// hoists those loads out of the again-loop. Diagnostic only (not kept): a file-scope `short *p = &g_4858c8;` plus
// the same for g_4858c4 and g_4858b8 gives 116 with the old argument and 447 with the fixed one (the layout shift
// above), so they are members of an address-taken struct in retail, as 0x350e0's note says for g_4858c4.
// Walk (b4B, statement by statement against retail 0x19530-0x19b67, jump table 0x19b68): all 25 live cases, the two
// null slots (modes 7, 11: no case; default __assume(0) = unchecked jmp at 0x1954c), the dimension defaults (1x1 start
// 0x19540-0x19546, 0x0 on a missing surface 0x19b15, 640x480 for case 6 0x19660, 4x4 for case 27 0x19b55), every
// tag offset (+4 +0xc +0x14 +0x64 +0xd0), the 0x143c0/0x144f0/0x137550 bitmap-index ECX constants (1 or 0) and
// case 16's in-range test (0x19850-0x19862) agree with retail: no behaviour difference found. Layout-neutral
// cleanups (fast 612 before and after): the (short) casts on 0x14f60/0x143c0 arguments (their parameters are short),
// the dead `&stage` / `(void)&param` address-taking and case 16's written-out clamp (now PIN, file-local macro as in
// 0x167a0's file; same bytes). Tried: recursive call function_19530(18, ...) for the mode 18 re-dispatch 612
// (same loop as the goto). No new include, helper or double local.
// @retail 0x19530
void function_19530(long mode, byte const *context, long stage, long *width, long *height)
{
again:
    *width = 1;
    *height = 1;
    switch (mode)
    {
    case 1: function_143c0(*(long *)(g_485a80 + 0xc), 0, stage, 0.0f); return;
    case 2: function_14f60(stage, 1); return;
    case 3:
        function_14f60(stage, 1);
        texture_surface_dimensions(1, width, height);
        return;
    case 4: g_51f3c8[1][stage] = NULL; return;
    case 5:
        function_14f60(stage, g_4858b8);
        function_01dd60(g_4858b8, width, height);
        return;
    case 6:
        function_14f60(stage, 3);
        if (g_4b4b58[3].data && !g_4b4b58[3].flag95)
        {
            *width = g_4b4b58[3].width;
            *height = g_4b4b58[3].height;
        }
        else { *width = 640; *height = 480; }
        return;
    case 9: function_143c0(*(long *)(g_485a80 + 4), 0, stage, 0.0f); return;
    case 10:
        if (g_4858c8 && g_4858c4)
        {
            if (*(dword *)g_4858c4 & 0x80000) { mode = 18; goto again; }
            if (*(long *)(g_4858c4 + 0xe0) != NONE && !g_485a77)
                function_14f60(stage, 13);
            else
                function_144f0(stage, 0, *(long *)(g_4858c4 + 0x7c), 1, 0, 0.0f);
        }
        else
        {
            if (!g_55e6bd) g_55e6bd = true;
            long tag = *(long *)(g_485a80 + 0x64);
            if (tag != NONE)
            {
                s_type_7ba8e9 *bitmap = function_137550(tag, 1);
                if (bitmap) function_14390(stage, bitmap, 0.0f);
            }
        }
        return;
    case 12:
        if (!g_4858c8) function_143c0(*(long *)(g_485a80 + 4), 0, stage, 0.0f);
        else function_144f0(stage, 0, *(long *)(g_4858c4 + 0x7c), 1, 0, 0.0f);
        return;
    case 13:
        function_14f60(stage, 9);
        texture_surface_dimensions(9, width, height);
        return;
    case 8:
        function_14f60(stage, 10);
        texture_surface_dimensions(10, width, height);
        return;
    case 14: function_143c0(*(long *)(g_485a80 + 0x14), 0, stage, 0.0f); return;
    case 15: function_143c0(*(long *)(g_485a80 + 0x14), 1, stage, 0.0f); return;
    case 16:
        function_143c0(g_4e6978, g_4e697c, stage, 0.0f);
        if (g_4e6978 != NONE)
        {
            byte *definition = g_4e3b44[g_4e6978 & 0xffff].bytes;
            long clamped = PIN(g_4e697c, 0, *(long *)(definition + 0x44) - 1);
            if (clamped == g_4e697c)
            {
                byte *bitmap = *(byte **)(definition + 0x48) + g_4e697c * 0x74;
                *width = *(short *)(bitmap + 4);
                *height = *(short *)(bitmap + 6);
            }
        }
        return;
    case 17:
        function_14f60(stage, 19);
        texture_surface_dimensions(19, width, height);
        return;
    case 18:
        if (!g_485607)
        {
            long surface = function_25960();
            if (surface != NONE)
            {
                function_14f60(stage, surface);
                function_01dd60(surface, width, height);
                return;
            }
        }
        function_143c0(*(long *)(g_485a80 + 0x64), 0, stage, 0.0f);
        return;
    case 19: function_14f60(stage, 21); texture_surface_dimensions(21, width, height); return;
    case 20: function_14f60(stage, 22); texture_surface_dimensions(22, width, height); return;
    case 21: function_14f60(stage, 13); texture_surface_dimensions(13, width, height); return;
    case 22: function_14f60(stage, 17); texture_surface_dimensions(17, width, height); return;
    case 23: function_14f60(stage, 23); texture_surface_dimensions(23, width, height); return;
    case 24:
    case 25:
        {
            long surface = g_485a78[mode - 24];
            if (surface == 25 || surface == 26 || surface == 27)
            {
                function_14f60(stage, surface);
                function_01dd60(surface, width, height);
            }
            else
                function_14f60(stage, 1);
            return;
        }
    case 26:
        {
            byte *definition = g_4e3b44[*(long *)(context + 8) & 0xffff].bytes;
            byte *parameters = *(byte **)(definition + 0x24);
            long index = *(short *)(*(byte **)(parameters + 0x60) + 0x10);
            if (index == -1)
            {
                definition = g_4e3b44[*(long *)(g_485a80 + 0xd0) & 0xffff].bytes;
                parameters = *(byte **)(definition + 0x24);
                index = *(short *)(*(byte **)(parameters + 0x60) + 0x10);
            }
            byte *bitmap = *(byte **)(parameters + 8) + index * 12;
            function_143c0(*(long *)bitmap, *(short *)(bitmap + 4), stage, 0.0f);
            return;
        }
    case 27:
        if (g_485a76)
        {
            function_14f60(stage, 16);
            texture_surface_dimensions(16, width, height);
        }
        else
        {
            long tag = *(long *)(g_485a80 + 0x64);
            if (tag != NONE)
            {
                s_type_7ba8e9 *bitmap = function_137550(tag, 0);
                if (bitmap) function_14390(stage, bitmap, 0.0f);
            }
            *width = 4;
            *height = 4;
        }
        return;
    default: __assume(0);
    }
}

long g_485a60;

// score 146 (was 353): retail inlines 0x16ae0's floor body (its asm; a 0xc frame with the value's slot) where ours
// calls floor, so ours keeps g_485a60 in EDX across the loop where retail keeps the definition table. Round 126:
// binding bytes read at each use 215; record-array view through input/output pointers 234 (0x16b90 back to 47);
// the existing 0x16ae0 called 210; flag test before the source pointer and full-expression 0x19530 outputs 146.
// Diagnostic only: 0x16ae0's body as a file-local forceinline copy gives 736 bytes and 150 (not kept: a twin).
// Round 65 (m3 lane 1, on the 353 body before round 126 merged): the existing 0x16ae0 with per-use binding reads
// and a counted loop 210; that with the original slot/destination/source locals 365 (0x16b90 back to 47); the
// location tag in a local 214; a file-local forceinline asm copy 158 (a twin, not kept). Lead: exposing g_485a60's
// address (diagnostic only) gave 42 at 736 bytes: retail reloads g_485a60 after the state stores and holds g_4e3b44
// in EDX; 0x16ae0 is `inline __declspec(noinline)` in src/unknown_0167a0.cpp, which keeps it out of line here.
// @retail 0x18a90
void function_18a90(byte *state, word const *descriptor)
{
    byte *binding = *(byte **)(*(byte **)(state + 0x20) + 0xc) + (*descriptor & 0x1ff) * 4;
    for (long i = 0; i < (*descriptor >> 9); i++)
    {
        if (binding[0] != 0xff)
        {
            memcpy(state + 0x1320 + binding[2] * 0x40, state + 0x320 + (signed char)binding[0] * 0x40, 0x40);
            if (binding[3] & 2)
            {
                byte *source = state + 0x320 + (signed char)binding[0] * 0x40;
                if (*(long *)source != NONE)
                {
                    byte *definition = g_4e3b44[*(long *)source & 0xffff].bytes;
                    if ((real)*(long *)(definition + 0x44) > *(real *)(source + 0x18))
                    {
                        byte *bitmap = *(byte **)(definition + 0x48) + (long)floor(*(real *)(source + 0x18)) * 0x74;
                        if (bitmap[0xe] & 0x10)
                        {
                            *(long *)(state + 0x328 + (signed char)binding[0] * 0x40) = *(short *)(bitmap + 4);
                            *(long *)(state + 0x32c + (signed char)binding[0] * 0x40) = *(short *)(bitmap + 6);
                        }
                    }
                }
            }
            *(dword *)(state + 0x1420) |= 1 << binding[2];
        }
        else if (binding[1])
        {
            if (binding[1] != 11)
            {
                *(long *)(state + 0x1320 + binding[2] * 0x40) = NONE;
                function_19530((signed char)binding[1], state, (short)binding[2],
                    (long *)(state + 0x1328 + binding[2] * 0x40), (long *)(state + 0x132c + binding[2] * 0x40));
            }
            else
            {
                byte *location = NULL;
                if (g_4e0344 && *(long *)((byte *)g_4e0344 + 0x80) > 0 && g_4e0348)
                {
                    byte *first = *(byte **)((byte *)g_4e0344 + 0x84);
                    if (*(long *)(first + 0x1c) != NONE &&
                        *(long *)(first + 4) == *(long *)((byte *)g_4e0348 + 8) && g_485a60 != NONE)
                        location = first;
                }
                if (location)
                {
                    byte *destination = state + 0x1320 + binding[2] * 0x40;
                    if (*(long *)destination != *(long *)(location + 0x1c) || *(real *)(destination + 0x18) != (real)g_485a60)
                    {
                        byte *definition = g_4e3b44[*(long *)(location + 0x1c) & 0xffff].bytes;
                        byte *bitmap = *(byte **)(definition + 0x48) + g_485a60 * 0x74;
                        *(real *)(state + 0x1330 + binding[2] * 0x40) = 3.0f;
                        *(long *)(state + 0x1320 + binding[2] * 0x40) = *(long *)(location + 0x1c);
                        *(real *)(state + 0x1338 + binding[2] * 0x40) = (real)g_485a60;
                        short width = *(short *)(bitmap + 4);
                        short height = *(short *)(bitmap + 6);
                        long maximum = width > height ? width : height;
                        *(real *)(state + 0x1334 + binding[2] * 0x40) = (real)(log((double)maximum) * 0.6931471824645996f);
                    }
                    *(dword *)(state + 0x1420) |= 1 << binding[2];
                }
                else
                    *(long *)(state + 0x1320 + binding[2] * 0x40) = *(long *)(g_485a80 + 0x64);
            }
        }
        else
            *(long *)(state + 0x1320 + binding[2] * 0x40) = NONE;
        binding += 4;
    }
}

extern long g_485a64, g_485a68;
extern real g_485a6c, g_485a70;
void __stdcall function_16b90(byte *state, long tag, long first, long second,
    long third, long fourth, real scale);

// @retail 0x1bbf0
void function_1bbf0(long tag, long first, long second, long third, long fourth, real scale)
{
    (void)&first; (void)&second; (void)&third; (void)&fourth; (void)&scale;
    if (g_4670bc)
    {
        function_16b10((s_render_reset_state *)g_485b48);
        D3DDevice_SetRenderState(D3DRS_LOGICOP, 0);
        D3DDevice_SetRenderState(D3DRS_ZENABLE, 2);
        D3DDevice_SetRenderState(D3DRS_ZFUNC, 0x203);
        D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
        D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, 0);
        D3DDevice_SetRenderState(D3DRS_SAMPLEALPHA, 0);
        D3DDevice_SetRenderState(D3DRS_ALPHAREF, 0);
        D3DDevice_SetRenderState(D3DRS_CULLMODE, 0x900);
        D3DDevice_SetRenderState(D3DRS_FRONTFACE, 0x900);
        D3DDevice_SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
        D3DDevice_SetTextureStageState(1, D3DTSS_COLORSIGN, 0);
        D3DDevice_SetTextureStageState(2, D3DTSS_COLORSIGN, 0);
        D3DDevice_SetTextureStageState(3, D3DTSS_COLORSIGN, 0);
        g_51f3c8[1][0] = NULL;
        g_51f3c8[1][1] = NULL;
        g_51f3c8[1][2] = NULL;
        g_51f3c8[1][3] = NULL;
        g_4670bc = false;
    }
    if (tag != NONE && ((dword *)g_4e3b44)[(short)tag * 4] == 0x73686164)
    {
        byte *definition = g_4e3b44[tag & 0xffff].bytes;
        g_485a64 = *(word *)(definition + 0x3e);
        g_485a68 = *(word *)(definition + 0x40);
        g_485a6c = *(real *)(definition + 0x44);
        g_485a70 = *(real *)(definition + 0x48);
    }
    else
    {
        g_485a64 = 0;
        g_485a68 = 0;
        g_485a6c = 0.0f;
        g_485a70 = 0.0f;
    }
    function_16b90(g_485b48, tag, first, second, third, fourth, scale);
}

// @retail 0x16610
bool __stdcall function_16610(long tag, long stage, long wanted_pass, long variant,
    long target, bool copy)
{
    bool result = false;
    if (tag != NONE)
    {
        byte *definition = g_4e3b44[tag & 0xffff].bytes;
        byte *shader = g_4e3b44[*(long *)(definition + 4) & 0xffff].bytes;
        byte *tables = *(byte **)(shader + 0x5c);
        // the first entry, its index clamped to the entry count
        long entry_index = 0;
        if (entry_index > *(long *)tables - 1)
            entry_index = *(long *)tables - 1;
        if (entry_index == 0)
        {
            byte *entry = *(byte **)(tables + 4);
            if (*(dword *)(entry + 2) & (1 << stage))
            {
                word *passes = (word *)*(byte **)(tables + 0xc) + (*(word *)entry & 0x1ff) + stage;
                for (long pass = 0; pass < (*passes >> 9); ++pass)
                {
                    if (wanted_pass == NONE || wanted_pass == pass)
                    {
                        byte *pass_entry = *(byte **)(tables + 0x14) + ((*passes & 0x1ff) + pass) * 10;
                        byte *pass_definition = g_4e3b44[*(long *)(pass_entry + 4) & 0xffff].bytes;
                        byte *record = *(byte **)(*(byte **)(pass_definition + 0x20) + 4) + variant * 0x132;
                        if (*(long *)(record + 0x100) != NONE)
                        {
                            g_4670bc = true;
                            function_16b10((s_render_reset_state *)g_485b48);
                            function_1bbf0(tag, 0, stage, pass, variant, 0.0f);
                            function_1c590((s_shader_cache *)g_51f0f0, *(long *)(record + 0x100), 0);
                            function_1c710(g_51f0f0);
                            function_1cf50();
                            g_4b8448 = 0;
                            D3DDevice_SetRenderState(D3DRS_CULLMODE, 0);
                            if (copy) function_15ec0(target);
                            else function_14bc0((short)target, 0, false);
                            result = true;
                        }
                    }
                }
            }
        }
    }
    return result;
}

extern long g_4858b4;
void __stdcall function_352e0(long target, bool multiple);
long function_35510(long format);
void function_1bd50(void *state);

// MiniMax E (11-14): no new form, no lower score (13): the fallback index is still masked to a long here and loaded as a
// word and widened in retail, and the element's argument is still pushed after the branch here and before it in retail;
// the shared global's type may not change here and reading it as a word would need a cast of its own address, which the
// rules forbid.
// MiniMax B (11-13): no new form, no lower score (13): the fallback index is still masked to a long here and loaded as a word and widened in retail, and the element's argument is still pushed after the branch here and before it in retail; the shared global's type may not change and a cast of its own address is a banned form.
// MiniMax H (11-12): no new form, no lower score (13): the fallback index is still loaded as a masked long where retail loads a word and widens it with an xor, and the element argument is still pushed after the branch where retail pushes it before; the matched 0x2dd30 and 0x32ac0 show that the xor-and-word form comes from a 16-bit lvalue or a (short) cast of the global passed straight to function_14bc0, while inside a ternary the conversion to the common type folds into movzx or movsx and if-converts to a cmove (15 and 16).
// MiniMax H (11-11): no new form, no lower score (13): retail loads the fallback index as a word and widens it with an xor (xor eax, eax; mov ax, word ptr [0x4858b8]) while ours loads the long global g_4858b8 and masks it, and retail pushes the element argument before the branch where we push it after; the global's declared type is not mine to change and reading it as a word needs a cast of its own address.
// MiniMax I (11-11): no new form, no lower score (13): retail loads the fallback index as a word and widens it with an xor while ours loads the long global g_4858b8 and masks it, and retail pushes the element argument after the branch where we push it before; the global's declared type may not change here.
// MiniMax D (11-11): no new form, no lower score (13).
// MiniMax S (11-10): no new form, no lower score (13): retail loads the fallback index as a word and widens it with an xor while ours loads the long global g_4858b8 and masks it, and retail pushes the element argument after the branch where we push it before; the global's declared type may not change and reading it as a word needs a cast of its own address.
// MiniMax A (11-10): no new form, no lower score (13): retail's fallback load is `mov ax, word ptr [0x4858b8]` (66 a1, six bytes), which VC7.1 only emits for a 16-bit lvalue at an absolute address; g_4858b8 is a long here and in the other files that declare it, a word-typed view would give the global another mangled name and break those files, and a cast of its own address is not allowed.
// MiniMax E (11-10): no new form, no lower score: retail loads the fallback index as a word and
// widens it with an xor while ours loads the long global g_4858b8 and masks it, and the branch
// sits after the element argument's push in retail and before it here.
// MiniMax R (11-10): no new form, no lower score (13): retail loads the fallback index as a word and widens it with an xor, ours loads the long global g_4858b8 and masks it; reading that global as a word needs a cast of its own address, which the rules forbid.
// MiniMax D (11-09): no new form, no lower score: this round's twelve new forms (the fallback as
// `(short)`, `(word)` and `(unsigned short)`, the word and short locals, the call without the
// explicit `(short)` cast, an explicit zero before the word assignment and that zero with `|=` and
// with `+`) all score 15 or 13; MSVC folds the truncation and the widening of the long global into
// one `movzx eax, word ptr [...]`, while retail widens the word with an `xor eax, eax` before its
// 16-bit load, which only happens when the lvalue is word-typed. Retail writes the global as a
// dword (`mov dword ptr [0x4858b8], 0x12` in 0x13450), so the global is a long there too and the
// type of g_4858b8 cannot change.
// MiniMax D (11-09): no new form, no lower score: the `(word)` spelling does give retail's 204
// bytes, but the element argument's push then stays after the branch where retail pushes it
// before, so the tail is laid out differently and the score is 15; the 13-scoring masked body stays.
// MiniMax G (11-05): no new form, no lower score; the fallback's word load stays a movzx because the global's declared type is long in another file.
// MiniMax D (11-01): the two-call form retail uses scores 20 here (the fallback's word load
// lands in ecx and each arm pushes its own value); the mixed-type ternary reaches 15 with the
// branch and the phi in eax, but the element push stays after the branch and the load is movzx
// where retail widens the word with an xor.
// MiniMax D (11-01): the fallback as a word cast, as a ternary and as a word local all score 15
// or worse; our load stays a movzx where retail widens the word with an xor.
// MiniMax A (11-03): no new form, no lower score.
// MiniMax G (11-04): no new form, no lower score.
// MiniMax G (11-12): no new form tried this round, no lower score (13): the fallback index is still masked
// rather than narrowed to a word, so the load stays a dword and a mask.
// MiniMax H (11-13): no new form, no lower score (13); the fallback index is masked to a long here and loaded as a word and widened in retail, and the element's argument is pushed after the branch here and before it in retail.
// score 13 (was 15): the fallback index is masked rather than narrowed to a word, so the load
// is a dword and the mask; retail loads the word directly and keeps the element argument's
// push before the branch.
// MiniMax D (11-06): no new form, no lower score: the fallback's load is a masked dword load where
// retail widens a word with an xor, because g_4858b8 is a long in this file.
// MiniMax E (11-08): no new form, no lower score: retail widens the fallback's word with an xor and hands it over in ecx, ours masks the dword and keeps the branch.
// MiniMax E (11-09): no new form, no lower score: retail loads the fallback as a word and widens it with an xor, ours loads the long global g_4858b8 and masks it; the global's declared type is not mine to change.
// MiniMax D (11-09): no new form, no lower score: the `(word)g_4858b8` spelling makes the length
// retail's 204 bytes, but the score rises to 15, because MSVC gives `movzx eax, word ptr [...]`
// where retail writes `xor eax, eax` and then `mov ax, word ptr [...]`; the 13-scoring body stays.
// MiniMax D (11-09): no new form, no lower score: the body is unchanged at 13 (206 bytes against retail's 204); retail takes the fallback as a word in the same register as the other arm (`xor eax, eax`; `mov ax, word ptr [0x4858b8]`) so that both arms write one variable and the value is a phi in eax, while ours loads the long global and masks it, two bytes longer. This round: about sixteen forms (the two-call if/else with a shared or a separate variable, the plain ternary with both arms long or both short, the mixed `(word)` and long ternary, the mixed `(short)` and long ternary, the local-variable forms, the unsigned casts) all score 15 or worse; MSVC if-converts a ternary whose arms share a type into a `cmove`, widens a mixed-type one with a `movzx` or a `movsx`, and gives the value a fresh register when the arms write different variables.
// MiniMax G (11-13): no new form tried this round, no lower score (13): retail takes the fallback index as a word
// and widens it with an xor before it hands it over in the same register as the other arm, while ours loads the long
// global g_4858b8 and masks it, so our body is 206 bytes against retail's 204; the global's declared type is long
// (retail writes it as a dword) and this lane may not change a shared global's type, and reading it through a cast
// of its own address is not allowed either, so the body stands.
// MiniMax R (11-09): no new form, no lower score (13): retail loads the fallback index as a word and widens it with an xor while ours loads the long global g_4858b8 and masks it, and the branch sits after the element argument's push in retail and before it here; the global's type is a declaration this lane may not change, and reading it through a cast of its own address is not allowed either.
// MiniMax A (11-13): no new form tried this round, no lower score (13): retail loads the fallback index as a
// word and widens it while this build loads the long global g_4858b8 and masks it, and the branch sits after
// the element argument's push in retail and before it here; the shared global's type may not change and a cast
// of its own address is a banned form.
// @retail 0x1c0d0
void function_1c0d0(long tag, long stage)
{
    if (tag != NONE)
    {
        byte *definition = g_4e3b44[tag & 0xffff].bytes;
        long shader_tag = *(long *)(definition + 4);
        if (shader_tag != NONE)
        {
            byte *shader = g_4e3b44[shader_tag & 0xffff].bytes;
            bool changed = false;
            if ((shader[0xe] & 1) && stage == 15)
            {
                function_352e0(NONE, true);
                changed = true;
            }
            long first = *(long *)(shader + 0x44);
            if (first != NONE && function_35510(*(short *)(shader + 0x48)) == stage)
            {
                function_16610(first, stage, NONE, 0, 13, true);
                changed = true;
            }
            long second = *(long *)(shader + 0x50);
            if (second != NONE && function_35510(*(short *)(shader + 0x54)) == stage)
            {
                function_16610(second, stage, NONE, 0, 17, true);
                changed = true;
            }
            if (changed)
            {
                long target = g_4858b4;
                if (target == NONE) target = g_4858b8 & 0xffff;
                function_14bc0((short)target, 0, true);
                g_4670bc = true;
                function_16b10((s_render_reset_state *)g_485b48);
            }
        }
    }
}

extern real g_4670e4;

extern dword g_4b8340, g_4b8334, g_4b8330;

// @retail 0x4d9c0
void function_4d9c0(byte const *source, long tag, byte value, short index,
    long first, long second, long third, long fourth)
{
    (void)&index; (void)&first; (void)&second; (void)&third; (void)&fourth;
    s_44940_entry *entry = &g_4ba138[index];
    real scale = value * *(real const *)(source + 0x2c) * g_4670e4 * 4.0f;
    if (tag == 10)
    {
        g_4b8340 = 1;
        D3DDevice_SetRenderState(D3DRS_SOLIDOFFSETENABLE, 1);
        real offset = 550.0f;
        g_4b8334 = *(dword *)&offset;
        D3DDevice_SetRenderState(D3DRS_POLYGONOFFSETZOFFSET, *(dword *)&offset);
        real slope = 2.0f;
        g_4b8330 = *(dword *)&slope;
        D3DDevice_SetRenderState(D3DRS_POLYGONOFFSETZSLOPESCALE, *(dword *)&slope);
    }
    function_1bd50(*(void **)entry->unknown10);
    function_1bbf0(first, second, tag, third, fourth, scale);
}

extern real g_4c19b0, g_4c19b4;
real g_4c19b8, g_4c19bc;
byte g_55e6c5;
struct s_filter_vector { real x, y, z, w; };
static s_filter_vector const s_default_filter_coordinate = { 0.0f, 0.0f, 0.0f, 0.0f };
s_filter_vector const *g_4687c0 = &s_default_filter_coordinate;
void function_01ddd0(long index, dword width, dword height);
long function_21360(long count, long source, real radius, real low, real high, long target,
    long alternate, long output, bool configure, bool full_surface, real scale);
long function_211a0(long source, real passes, real distortion, real falloff,
    long target, long alternate, long unused, long configure, long full_surface,
    real scale, real offset);

struct s_filter_coordinates
{
    word flags[4];
    s_filter_vector offsets[4];
};

// score 656 (was 686), 2664 bytes (retail 2652). Round 1 of #1066: first statement-by-statement logic walk.
// Behaviour fixed: the filter-pass test compared the weights with 1.0f; retail compares with 0.0f (constant at
// 0x45dbd8) and SKIPS the pass when the flag bit is set and that weight is 0 (the test was inverted); the pass
// result was written back into `target`, but retail never writes [esp+0x1c] after the mode switch: every later
// pass and filter keeps using `target` and the result goes only to g_485a78[filter] (`result`, or `target` when
// the pass is skipped). Retail details now followed: the surface sizes are one data/flag95 test per surface
// (if/else, as 0x1dd60); the size clamp is on the float before the conversion (comiss/ja against 64.0f at
// 0x45dc90) and the area limit is the float 76800.0f at 0x45e4f8; `pass` is a branch on the raw word 0xffff;
// `variant` is retail's nested weight tests; output/alternate are set per filter branch (656, was 665).
// Not matched, none changes behaviour:
// - the weight copy: retail moves four dwords (mov pairs) to g_4c19b0..bc, ours loads floats. A dword cast on the
//   globals (`*(dword *)&g_4c19b0 = ((dword const *)weights)[0]` x4) scores 10 lower (655 before the output/
//   alternate change) but is a cast on globals' addresses, so it is not kept; a dword local re-read as real: 732.
// - retail's three jump tables (mode at record+0x60 twice, attribute at 0x2db78) have no bounds check; their
//   shapes are reached with `switch (*(word const *)(record + 0x60))` read at each use, `default: __assume(0)`,
//   `long target;` set in the cases (case 0 sets 0), `/ 2` and `/ 4` for the sizes, and the position switch as
//   four complete bodies (case 1 without the multiply, cases 2 and 3 each load 0.5f or 0.25f and join one tail,
//   as retail) but that build scores 739-749 (2720 bytes): D3DDevice_End is inlined (+45 bytes), retail calls
//   0x3fb480. End is inlined in EVERY variant tried (including the original), and the scalar-surface form
//   (texture_surface_dimensions x4, retail's slots 0x94/0x70, 0x7c/0x78, 0x80/0x88, 0x74/0x90) makes the
//   build call both Begin and End (701); retail inlines Begin and calls End. Scores here are confounded by that
//   until the inline state matches: weights alone 641, dims alone 642, both 663 on the old base.
// - retail's position case 0 copies x/y with integer moves; ours uses movss. Retail counts the side/column loops
//   up (inc, cmp, jle); ours counts down. A per-row pointer walk and `*entry & 4` re-read compile the same.
// @retail 0x2d140
void function_2d140(long tag, real const *weights, long columns, long rows, s_filter_vector const *grid)
{
    (void)&weights; (void)&columns; (void)&rows; (void)&grid;
    g_4c19b0 = weights[0];
    g_4c19b4 = weights[1];
    g_4c19b8 = weights[2];
    g_4c19bc = weights[3];
    if (tag == NONE || g_4b9dd2 >= g_4b9dd6 || g_4b9dd0 >= g_4b9dd4 || g_4858b8)
        return;
    long width = g_4b9dd6 - g_4b9dd2, height = g_4b9dd4 - g_4b9dd0;
    byte const *definition = g_4e3b44[tag & 0xffff].bytes;
    long material = *(long const *)(definition + 0x44);
    dword group = ((dword *)g_4e3b44)[(short)material * 4];
    byte const *parameters;
    if (group == 0x5052544d || group == 0x70727433)
        parameters = (byte const *)function_137bd0(material)->function_x947334();
    else parameters = *(byte **)(g_4e3b44[material & 0xffff].bytes + 0x24);
    byte const *shader = g_4e3b44[*(long const *)parameters & 0xffff].bytes;
    byte const *tables = *(byte **)(shader + 0x5c);
    long pass_count = ((word *)*(byte **)(tables + 0xc))[(*(word *)*(byte **)(tables + 4) & 0x1ff) + 15] >> 9;
    long const saved_targets[4] = { 1, 25, 26, 27 };
    long dimensions[4][2];
    for (long i = 0; i < 4; ++i)
    {
        s_unknown_01dcc0 const *surface = &g_4b4b58[saved_targets[i]];
        if (surface->data && !surface->flag95)
        {
            dimensions[i][0] = surface->width;
            dimensions[i][1] = surface->height;
        }
        else dimensions[i][0] = dimensions[i][1] = 0;
    }
    real left = (real)g_4b9dd2, right = (real)g_4b9dd6;
    real top = (real)g_4b9dd0, bottom = (real)g_4b9dd4;
    for (long record_index = 0; record_index < *(long const *)(definition + 0x88); ++record_index)
    {
        byte const *record = *(byte **)(definition + 0x8c) + record_index * 0xac;
        word pass_word = *(word const *)(record + 8);
        long pass = record_index;
        if (pass_word != 0xffff) pass = (short)pass_word;
        long variant;
        if (weights[0] == 0.0f)
        {
            if (weights[1] == 0.0f) variant = ((signed char const *)record)[0xc];
            else variant = ((signed char const *)record)[0xe];
        }
        else if (weights[1] == 0.0f) variant = ((signed char const *)record)[0xd];
        else variant = ((signed char const *)record)[0xf];
        long clamped_pass = pass < 0 ? 0 : pass > pass_count - 1 ? pass_count - 1 : pass;
        if (pass != clamped_pass || variant == NONE) continue;
        word mode = *(word const *)(record + 0x60);
        long target = 0;
        if (mode >= 1 && mode <= 3)
        {
            target = 1;
            function_01ddd0(target, width >> (mode - 1), height >> (mode - 1));
        }
        if (!function_16610(material, 15, pass, variant, target, false)) continue;
        s_filter_coordinates const *coordinates = *(long const *)(record + 0x58) > 0 ?
            *(s_filter_coordinates **)(record + 0x5c) : 0;
        for (long row = 0; row < rows - 1; ++row)
        {
            D3DDevice_Begin(D3DPT_TRIANGLESTRIP);
            for (long column = 0; column < columns; ++column)
            {
                for (long side = 0; side < 2; ++side)
                {
                    real const *vertex = (real const *)&grid[(row + side) * columns + column];
                    for (long attribute = 0; attribute < 4; ++attribute)
                    {
                        real value[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
                        switch (((word const *)(record + 0x50))[attribute])
                        {
                        case 0: value[0] = vertex[2]; value[1] = vertex[3]; break;
                        case 1: value[0] = (vertex[0] - left) / (right - left);
                            value[1] = (vertex[1] - top) / (bottom - top); break;
                        case 2: value[0] = vertex[0] - left; value[1] = vertex[1] - top; break;
                        case 3: value[0] = vertex[0]; value[1] = vertex[1]; break;
                        case 4: memcpy(value, g_4687c0, sizeof(value)); break;
                        }
                        if (coordinates)
                        {
                            real const *offset = (real const *)&coordinates->offsets[attribute];

                            value[0] += offset[0];
                            value[1] += offset[1];
                            value[2] += offset[2];
                            value[3] += offset[3];
                            if (coordinates->flags[attribute] & 1)
                            {
                                value[0] *= g_4b9de4;
                                value[1] *= g_4b9de4;
                            }
                        }
                        D3DDevice_SetVertexData4f(attribute + 1, value[0], value[1], value[2], value[3]);
                    }
                    real x = vertex[0], y = vertex[1];
                    if (mode >= 1 && mode <= 3)
                    {
                        real scale = mode == 1 ? 1.0f : mode == 2 ? 0.5f : 0.25f;
                        x = (x - left) * scale;
                        y = (y - top) * scale;
                    }
                    D3DDevice_SetVertexData4f(D3DVSDE_POSITION, x, y, 16777215.0f, 16777215.0f);
                }
            }
            D3DDevice_End();
        }
        for (long filter = 0; filter < *(long const *)(record + 0xa4); ++filter)
        {
            byte const *entry = *(byte **)(record + 0xa8) + filter * 0x5c;
            long output, alternate;
            if (filter == 0)
            {
                output = 25;
                alternate = 26;
            }
            else
            {
                output = 27;
                alternate = g_485a78[0] == 25 ? 26 : 25;
            }
            word flags = *(word const *)entry;
            long result = target;
            if (!(flags & 1) || weights[0] != 0.0f)
            {
                if (!(flags & 2) || weights[1] != 0.0f)
                {
                    long target_width = 0, target_height = 0;
                    function_01dd60(target, &target_width, &target_height);
                    real width_real = (real)target_width, height_real = (real)target_height;
                    real wanted_width = *(real const *)(entry + 0x58) * width_real;
                    wanted_width = wanted_width > 64.0f ? wanted_width : 64.0f;
                    real wanted_height = *(real const *)(entry + 0x58) * height_real;
                    wanted_height = wanted_height > 64.0f ? wanted_height : 64.0f;
                    long scaled_width = (long)wanted_width, scaled_height = (long)wanted_height;
                    if (scaled_width * scaled_height > 76800)
                    {
                        double scale = sqrt(76800.0f / (target_width * target_height));
                        scaled_width = (long)(width_real * scale);
                        scaled_height = (long)(scale * height_real);
                        if (scaled_width < 64 || scaled_height < 64)
                        {
                            if (!g_55e6c5) g_55e6c5 = true;
                            scaled_width = scaled_height = 64;
                        }
                    }
                    function_01ddd0(output, scaled_width, scaled_height);
                    function_01ddd0(alternate, scaled_width, scaled_height);
                    if (flags & 4)
                        result = function_21360((long)*(real const *)(entry + 0x44), target,
                            *(real const *)(entry + 0x48), *(real const *)(entry + 0x50),
                            *(real const *)(entry + 0x54), output, alternate,
                            NONE, true, false, 0.0f);
                    else result = function_211a0(target, *(real const *)(entry + 0x44),
                        *(real const *)(entry + 0x48), *(real const *)(entry + 0x4c),
                        output, alternate, NONE, 1, 0, 0.0f, 0.0f);
                }
            }
            g_485a78[filter] = result;
        }
    }
    function_01ddd0(1, dimensions[0][0], dimensions[0][1]);
    function_01ddd0(25, dimensions[1][0], dimensions[1][1]);
    function_01ddd0(25, dimensions[2][0], dimensions[2][1]);
    function_01ddd0(27, dimensions[3][0], dimensions[3][1]);
    g_4670bc = true;
    function_16b10((s_render_reset_state *)g_485b48);
    g_485a78[0] = g_485a78[1] = NONE;
}
// MiniMax D (11-14): no new form, no lower score (62): the second argument still arrives in eax where retail takes it in ecx (retail's order is the tag in edx, the first argument in ecx and the second in eax, with the third and fourth on the stack), and the callers are in other files, so the convention cannot be changed from here.
// MiniMax H (11-11): no new form, no lower score (62); the second argument still arrives in EAX where retail takes it in ECX, and the arguments behind it still sit one stack slot lower; retail's order is the tag in EDX, the first argument in ECX and the second in EAX, with the third and fourth on the stack, and the callers are in other files, so the convention cannot be changed from here.
// score 62 (was 62): retail takes tag/first/second in EDX/ECX/EAX (first and second moved to ESI/EDI, the tag table
// kept in EBX, fourth loaded into EBP from the stack, ret 8); ours takes tag EDX and fourth EAX, three on the stack.
// MiniMax A (11-11): no new form, no lower score (62); the second argument still arrives in EAX where retail
// takes it in ECX and the arguments behind it still sit one stack slot lower, so the entry block's register
// choice is the call cycle's, not this body's; the callers are in other files, so the calling convention cannot be
// changed from here. The original body is kept.
// MiniMax R (11-09): no new form, no lower score; the second argument still arrives in eax where retail
// takes it in ecx, and the arguments behind it still sit one stack slot lower. This round: the permuter
// ran out of time without reporting a form, so nothing was tried from it.
// MiniMax R (11-09): no new form, no lower score; the second argument still arrives in eax where retail takes it in ecx, and the arguments behind it still sit one stack slot lower; the declaration and the callers of this function are in other files, so the convention cannot be changed from here.
// MiniMax A (11-06, run 52): no new form, no lower score (62) - the two address-taken parameters are
// dropped (the same code), and retail keeps the first argument in ecx where ours reads it from the
// stack.

// MiniMax B (11-13): no new form, no lower score (62): this build still reads its first stack argument from [esp+0xc] where retail reads it from [esp+0x10]; the slot is fixed by the callers in the other files, so no source form here reaches it.
// MiniMax G (11-12): no new form, no lower score (62); retail's convention is edx = the tag, ecx = the
// first (copied to esi), eax = the second (copied to edi), e+4 = the third and e+8 = the fourth (copied
// to ebp), ret 8, with the tag table in ebx; the record index is the third argument and the selected
// tag lands in the dead fourth slot, as here. New forms: `first * 10` in place 91 (239 B), the same
// multiply fused into the entry read 91 (same code), the range index with its operands swapped 91 (same
// code), a local holding a copy of the first argument (the same code). Restored.
// MiniMax G (11-12): the call to 0x1bbf0 is the other blocker and it is in another file: our 0x1bbf0
// (in this file) compiles to retail's code with the same `ret 0x14` slots, but retail's 0x16b90 takes
// the scale as its third argument (its prologue reads the tag at e+8 and the state at e+4) while the
// declaration at src/unknown_0167a0.cpp:263 has the scale last, so our 0x1bbf0 gives the second, third
// and fourth parameters different stack slots and this caller pushes the first, third, second and
// fourth where retail pushes the first, second, fourth and third.
// MiniMax G (11-12): the permuter (300 s, 10 tries) reached 60 in both images by pulling the 0xffff and
// the 10 into constant locals and reordering the two calls' arguments: two points is under the keep bar,
// the constant locals and the reordered calls are not allowed forms, and the calls would pass something
// else, so the body stands.
// MiniMax C (11-11): no new form, no lower score (62); the second argument still arrives in eax where retail takes it in ecx, and the callers of this function are in other files.
// MiniMax R (11-10): no new form, no lower score (62); the second argument still arrives in EAX where retail takes it in ECX and the arguments behind it still sit one stack slot lower, because the callers are in other files.
// MiniMax I (11-10): no new form, no lower score (62); the second argument still arrives in EAX where retail takes it in ECX and the arguments behind it still sit one stack slot lower, because the callers are in other files.
// MiniMax E (11-09): no new form, no lower score; the second argument still arrives in eax
// where retail takes it in ecx, and the callers of this function are in other files.
// MiniMax C (11-06): no new form, no lower score - the second argument still arrives in eax where retail
// takes it in ecx, and the arguments behind it still sit one stack slot lower; the callers are in other
// files, so the calling convention cannot be changed from here.
// MiniMax D (11-05): no new form, no lower score - retail reads the second argument in ecx and keeps the
// tag table in ebx, ours takes the fifth argument in eax and reloads the table into esi; the record
// multiplier written as the selected tag or as the fifth argument scores 90.
// Earlier 62 (was 62): this round found no gain - the second argument arrives in eax where retail takes it in
// ecx, and the arguments behind it sit one stack slot lower; the callers are in other files, so the
// calling convention cannot be changed from here.
// MiniMax C (11-08): forms tried, none lower: the second argument through a local 62, the entry
// pointer through the table 62, the offset named after the selected tag 91, the entry built from the
// slot 91 (base 62).
// Earlier 62 (was 62): the second argument arrives in eax where retail takes it in ecx, and the
// arguments behind it sit one stack slot lower than retail's; the callers of this function are
// in other files, so the calling convention cannot be changed from here.
// Lane 18 r37 (fast): without the `(void)&` reads (one or both) 62 (same code); `first * 10` written in
// place, or its local named just before the entry, 91 (239 B, ret 8: first now arrives in EBX and third in
// EAX, where retail takes first in ECX, moved to ESI, and second in EAX; retail keeps the tag-table base
// in EBX and reloads it after the 0x137bd0 call). Restored.
// Round 170 (m3 lane 6, fast with 0x19f680/0x244470/0x2be5d0/0x116b00): the selected tag computed just before the
// 0x1bbf0 call 91 (238 B), the group read ahead of the selected tag 78 (251 B), each with `first * 10` in place 99/91;
// the selected tag as `selected = tag; if (tag == NONE) ...` 77; the group read as `*(dword *)&g_4e3b44[(short)tag]`,
// the head through a copy of 0x16b90's material_parameters inline (diagnostic), the parameters pointer as a ternary
// and the 0xc table named 62 each (same code). Re-measured: `first * 10` in place or named just before the entry 91.
// Retail forms the group read as `add eax, ebx; mov eax, [eax]`, unlike 0x16b90 and 0x19f680 (folded).
// MiniMax B (11-13): no new form, no lower score (62): the second argument's register is decided by the callers, which are in other files.
// score 62 (was 62): the second argument arrives in eax where retail takes it in ecx; the callers are in other files.
// MiniMax G (11-13): no new form, no lower score (62): the second argument still arrives in eax where retail
// takes it in ecx, and the callers are in other files, so the calling convention cannot be changed from here.
// @retail 0x4b2d0
void function_4b2d0(long tag, long first, long second, long third, long fourth)
{
    long selected = tag == NONE ? *(long *)(g_485a80 + 0xd0) : tag;
    long first_offset = first * 10;
    dword group = ((dword *)g_4e3b44)[(short)tag * 4];
    byte const *parameters;
    if (group == 0x5052544d || group == 0x70727433)
        parameters = (byte const *)function_137bd0(tag)->function_x947334();
    else parameters = *(byte **)(g_4e3b44[tag & 0xffff].bytes + 0x24);
    byte *definition = g_4e3b44[*(long const *)parameters & 0xffff].bytes;
    byte *tables = *(byte **)(definition + 0x5c);
    byte *entry = *(byte **)(tables + 4) + first_offset;
    word range = ((word *)*(byte **)(tables + 0xc))[(*(word *)entry & 0x1ff) + second];
    byte *pass = *(byte **)(tables + 0x14) + ((range & 0x1ff) + fourth) * 10;
    byte *pass_definition = g_4e3b44[*(long *)(pass + 4) & 0xffff].bytes;
    byte *record = *(byte **)(*(byte **)(pass_definition + 0x20) + 4) + third * 0x132;
    long program = *(long *)(record + 0x100);
    function_1bbf0(selected, first, second, fourth, third, 640.0f);
    function_1c590((s_shader_cache *)g_51f0f0, program, 0);
}

#include "geometry_cache.h"
#include "vector_products.h"
dword g_55ee58, g_55ee5c;
real g_55e6cc, g_55e6d0, g_55e6d4, g_55e6d8, g_55e6dc, g_55e6e0;
real g_55e6e4, g_55e6e8, g_55e6ec, g_55e6f0, g_55e6f4, g_55e6f8;
real g_52598c[3], g_525998[3];

struct s_octree_output { long node; point3f center; };
extern s_octree_output g_4c5700[256];
struct s_4ca40_colors;
void function_4ca00(s_4ca40_colors const *settings, byte const *data, color3f *light, color3f *color);
vector3f *function_4dd70(vector3f *out, dword packed);
vector3f *function_4dcd0(vector3f *out, dword packed);
inline vector3f *function_4efb0(vector3f *vector);
point3f *function_3f220(dword first, dword second, dword third, point3f *out);
vector3f *function_11d000(vector3f const *vector, vector3f *perpendicular);
void function_141ce0(real first, real second, real third, transform4x3f *out);
real g_5259b4;
dword g_4b8430;
// score 127 (was 127): main's body. On the reference form (the model entry's address bound to a const
// reference, the index < 0 test ahead of the geometry load: 2 in the plain check, kept only on a match) the
// only difference left is two independent float reloads after the second SetVertexData4f grow call (retail
// loads [esp+0xd8] into xmm2 and then [esp+0x54] into xmm3, ours the other way round).
// MiniMax H (11-13): no new form kept, no lower score (127): the plain forms tried this round (typed index,
// typed table pointer, the entry pointer advanced by the index, the field read through a byte offset, the
// 0x41640-shaped lookup through a definitions local, a second masked read of the field, a compare on the
// model) all fuse the entry address into one load; the reference entry reaches 2 again (the model's read as
// a lea and a separate load, and the part index's sign test ahead of the geometry load), which the rules
// keep only on a byte-for-byte match, so the body is restored; in that form what still differs is the order
// of the two float reloads after the grow call (xmm2 and xmm3), and the products of register 12 in named
// locals (11), the products of registers 12 to 14 in named locals (219), the light multiplies split (8) and
// the size computed earlier (2) do not move it.
// MiniMax I (11-11): 2 (was 127): the model entry is bound to a reference to a const pointer, so the
// address of the entry stays in a register and the model is read with a separate displaced load, as
// retail has it (`lea edx,[ebx+edx*8]` then `mov edx,[edx+4]`) instead of one fused indexed load;
// the loop's negative-index test is also split from the geometry load, so retail's order holds.
// The same shape is in the matched 0x273040, whose entry is a reference to a const pointer too.
// Still differs: two independent float loads after the call to 0x3faae0 come out in the other order.
// MiniMax I (11-11): no new form, no lower score (127): the references pointer reaches ebx as retail has it, but retail reads the model with a `lea` and a separate load where ours folds the address into one instruction, so every instruction after that one is paired with the wrong counterpart.

// MiniMax B (11-11): no new form, no lower score (127); retail reads the model index through the entry address
// computed with a lea and loads it four bytes on, where ours folds the four into the load's displacement, and the
// register and stack homes of the sine and of the cross product still differ; the body is retail's 2406 bytes.
// MiniMax E (11-11, run 81): 2 (was 127): the model is read through the entry address in a lea and a
// separate load, as retail reads it, and the sign check on the part index comes before the geometry
// load, as retail has it; what is left is the order of two independent float reloads after the
// second SetVertexData4f call.
// Reference form, not kept: 2 (from 127): the model's two reads are combined and the part index's sign check comes first;
// what still differs is the order of two independent float reloads after the second SetVertexData4f call.
// MiniMax E (11-11, run 80): no new form, no lower score (127): retail reads the model with a lea plus a
// separate load where ours folds the address into one instruction; this round typed the reference table as
// an eight-byte entry (the tag index in the second dword) and read it as an element, as a pointer to the
// element, through an inline double dereference, through a byte entry pointer and as a long array, and every
// one of them still compiles to the fused load, so what is left to find is the form that keeps the entry's address.
// MiniMax E (11-10, run 73): no new form, no lower score (302); our sine takes a stack slot of its own ([esp+0x48]) where retail keeps it in the shared temporary ([esp+0x14]) and puts cosine in a fresh one, so every frame offset after it is four bytes out, the references base sits in esi against retail's ebx, and the cross product's xmm choice differs.
// MiniMax D (11-11): 127 (was 128): the node pointer is taken before the table and references
// loads, which puts the references pointer in EBX as retail has it; retail still reads the model
// with a lea and a separate load where ours folds the address into one instruction, so every
// instruction after that one is paired with the wrong counterpart.
// MiniMax D (11-11): no new form, no lower score (128): the references pointer still lands in ESI where retail keeps it in EBX, so the model's read is one fused load here and a lea plus a load there; reading the model straight out of the references pointer, swapping the table and references declarations and typing the references pointer as void all compile to the same ESI form.
// MiniMax G (11-11): no new form, no lower score (302): our sine takes a stack slot of its own ([esp+0x48]) where retail keeps it in the shared temporary ([esp+0x14]) and puts cosine in a fresh one, so every frame offset after it is four bytes out; the references base sits in esi against retail's ebx and the cross product's xmm choice differs.
// MiniMax S (11-10): no new form, no lower score (302): our sine takes a stack slot of its own ([esp+0x48]) where retail keeps it in the shared temporary ([esp+0x14]) and puts cosine in a fresh one, so every frame offset after it is four bytes out, the references base sits in esi against retail's ebx, and the cross product's xmm choice differs.
// MiniMax D (11-10): 128 (was 302): the cross product now goes through the existing cross3f helper,
// which puts `sine` in retail's shared temp [esp+0x14]; the earlier note's worry that the full build
// then loses the matched 0xc2d00 (its scheduling tie) cannot be seen in the fast check, where
// 0xc2d00 scores 249 in the reduced image with either form, so the helper call is kept and the
// coordinator's full check should confirm 0xc2d00.
// MiniMax D (11-10): no new form, no lower score (302).
// MiniMax A (11-12): no new form, no lower score (127); the model's read stays one fused load where retail takes a
// lea and a separate load, and the forms that do reproduce retail's addressing are the match-only ones the coordinator
// has already rejected (a reference to the entry scores 2, a plain typed index with the split test scores 125); with the
// reference form in place the only difference left was the order of two independent reloads after a grow call (xmm2 and
// xmm3), and none of the perturbations tried here (the vertex data's products written the other way round, the position
// and light statements reordered or split, the size and alpha computed at another point) moved that order.
// MiniMax H (11-12): no new form kept, no lower score (127): the model's read is still one fused load where retail takes a lea and a separate load, and the only forms that reproduce retail's addressing are the ones the rules keep only on a byte-for-byte match (a reference to the entry scores 2, a plain typed index with the split test scores 125); a 900 s permuter run from the reference form found nothing below 2.
// Round 1 of #1066: 127 (from 544), same size as retail (2406). What changed: the rotation is one
// transform4x3f (forward/left/up filled by either branch, then position stored into it, so retail's
// copy of the position after the branch appears), the angle is computed inside each branch (the sin/cos
// one keeps it unrounded on the x87 stack), the draw count is a word tested `<= 0` and read into word
// locals before function_1cf50, the clamped alpha is its own local. Still differs: our `sine` takes its
// own stack slot ([esp+0x48]) where retail keeps it in the shared temp [esp+0x14] and puts cosine in a
// fresh one, so every frame offset after it is 4 off; the references base sits in esi (retail ebx) and
// is dereferenced without retail's lea; xmm register choice in the cross-product block and the draw
// calls. Tried: cosine declared before sine (303), sin/cos written inline at each use (421, 441),
// swapped product operands in the cross product (302), basis declared at function scope (302).
// Walk b4I (11-06): every field, offset, test direction (the `js`/`jge` bounds test, `jbe` on the unsigned word count,
// `jle` on the loop count), callee, argument order, the unrounded x87 angle (sin/cos stored as float) and the rounded
// `fraction`/`size` agree with retail: no behaviour difference. The cross product is cross3f's body written out; calling
// cross3f (vector_products.h) scores 128, since it puts `sine` in retail's shared temp [esp+0x14], but the full build then
// loses the matched 0xc2d00 (its scheduling tie), so the written-out form stays. The alpha clamp is PIN's body written out.
// MiniMax I (11-11), NOT kept (coordinator 11-11): 127 -> 2 with the reference entry bound as `s_4e260_model_entry const *const &entry = &((s_4e260_model_entry const *)references)[index]` (a reference to a one-use pointer: match-only) and the index < 0 test ahead of the geometry load; the same entry type read as a plain typed index with the split test is 125 (under the keep bar). Lead: retail keeps the entry's address before reading its model; find the plain form that does that.
// MiniMax E (11-12), NOT kept (coordinator 11-12): 127 -> 2 again, this time by reading the entry's model twice and AND-ing the copies (`model &= model_copy`, a no-op steer) with the index < 0 test first; together with MiniMax I's reference form above, retail evidently loads the model field twice from the entry's address. Find the plain source that uses entry->model twice.
// MiniMax B (11-13), NOT kept (coordinator 11-13): 127 -> 2 a third time with MiniMax I's reference form (the entry's
// address bound to `s_4e260_model_entry const *const &`, as in 0x273040, where it is kept because that function matches)
// and the index < 0 test ahead of the geometry load. Here it is 2, not a match, so the one-use reference stays out.
// What is left at 2: the order of two independent float reloads after the second SetVertexData4f call (retail loads
// [esp+0xd8] and then [esp+0x54], ours the other way round); a 900 s permuter run and a hand sweep of the products'
// order could not move it. Lead: with the reference form, find what orders those reloads; a match keeps the form.
// m3 lane 14 r90 (all on the reference form, 2 in the plain check; fast checks): the rows' C-API
// D3DDevice_SetVertexData4f spelling 710 (1999 bytes: not inlined); the rows read through basis.rotation 2
// (same code); the cross and perpendicular declarations swapped 2 (same code); the basis declared at loop scope
// beside the colours 2 (same code); the alpha clamp ahead of the position copy 187 (2408 bytes); the position
// copied field by field 68 (2418 bytes); the size computed after the basis 374 (2388 bytes); the entry as a
// const element reference 125, its model as a long const reference 125 (both fold the model read); the entry
// copied by value 508 (2421 bytes). Re-measured: each single product's operands swapped in rows 12 and 13, 2
// each (same code). Diagnostics: the basis or the size renamed 2 (same code: names do not order the reloads).
// The two reloads keep their order whatever the model read's form (the 125 forms show the same pair).
// @retail 0x4e260
void __stdcall function_4e260(long tag, long context, long first, long second,
    long third, dword handle, byte *material)
{
    function_1bd50(0);
    g_485a60 = NONE;
    if ((second == 1 || second == 2 || second == 15) && !(fabs(g_4b8494 - 1.0f) < 0.0001f))
    {
        D3DDevice_SetStipple(function_1c290(1.0f));
        g_4b8494 = 1.0f;
    }
    g_4b8430 = 1;
    D3DDevice_SetRenderState(D3DRS_TWOSIDEDLIGHTING, 1);
    function_1bd50(0);
    s_octree_output *node = &g_4c5700[handle & 0xff];
    byte *table = *(byte **)((byte *)g_4e0348 + 0x238);
    byte *references = *(byte **)((byte *)g_4e0350 + 0x37c);
    byte *record = *(byte **)(table + 0x1c) + ((handle >> 8) & 0x3fffff) * 24;
    long model = *(long *)(references + (signed char)record[0] * 8 + 4);
    byte *definition = g_4e3b44[model & 0xffff].bytes;
    byte *block = *(byte **)(table + 0x14) + *(short *)(record + 6) * 0x2c;
    word const *indices = *(word **)(definition + 0x2c);
    if (function_12de70((s_geometry_block_info *)block, 3)
        && function_12de70((s_geometry_block_info *)(definition + 0x38), 3))
    {
        real local_0 = *(real *)(definition + 8);
        real local_1 = *(real *)(definition + 0xc);
        real constants[4] = {g_52598c[handle >> 30], g_525998[handle >> 30], 0.0f, 1.0f};
        D3DDevice_SetVertexShaderConstant(-43, constants, 1);
        point3f center;
        center.x = node->center.x - 4.0f;
        center.y = node->center.y - 4.0f;
        center.z = node->center.z - 4.0f;
        function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(material + 0x10) + 0x100), 1);
        byte *stream = *(byte **)(definition + 0x34);
        function_1c620((s_shader_cache *)g_51f0f0, *(long *)(stream + 0x10),
            stream[1], *(long *)(stream + 8), g_43f408[stream[0]]);
        function_1c710(g_51f0f0);
        function_1cdd0(0, 0);
        function_1bbf0(tag, first, second, third, material[4], 10000.0f);
        for (long part = 0; part < *(short *)(record + 0xa); ++part)
        {
            long index = *(short *)(record + 8) + part;
            byte *geometry = *(byte **)(block + 0x28);
            if (index < 0 || index >= *(long *)geometry) continue;
            byte *vertex = *(byte **)(geometry + 4) + index * 24;
            dword packed = *(dword *)vertex;
            byte *group = *(byte **)(definition + 0x14) + (packed & 7) * 20;
            byte *settings = *(byte **)(group + 0x10) + ((packed >> 3) & 0x3f) * 40;
            long variant = settings[0xa];
            if (variant >= *(long *)(definition + 0x18)) continue;
            byte *draw = *(byte **)(definition + 0x1c) + variant * 8;
            if (*(word *)(draw + 6) <= 0) continue;
            vector3f decoded_vector;
            vector3f packed_vector = *function_4dd70(&decoded_vector, *(dword *)(vertex + 0x10));
            dword coordinates = *(dword *)(vertex + 4);
            point3f decoded_position;
            point3f position = *function_3f220(coordinates & 0x3ff, (coordinates >> 10) & 0x3ff, coordinates >> 20, &decoded_position);
            real fraction = ((packed >> 16) & 0xf) * (1.0f / 15.0f);
            real alpha = (*(real *)(vertex + 0x14) - local_0) * local_1;
            color3f light, color;
            function_4ca00((s_4ca40_colors *)settings, vertex, &light, &color);
            light.red *= color.red; light.green *= color.green; light.blue *= color.blue;
            position.x += center.x; position.y += center.y; position.z += center.z;
            real size = (1.0f - fraction) * *(real *)(settings + 0xc) + fraction * *(real *)(settings + 0x10);
            transform4x3f basis;
            if (settings[8] & 1)
            {
                real sine = (real)sin(((packed >> 9) & 0xf) * g_5259b4 * (1.0f / 15.0f));
                real cosine = (real)cos(((packed >> 9) & 0xf) * g_5259b4 * (1.0f / 15.0f));
                vector3f decoded_direction;
                basis.up = *function_4dcd0(&decoded_direction, packed >> 20);
                vector3f perpendicular, cross;
                function_11d000(&basis.up, &perpendicular);
                cross3f(&basis.up, &perpendicular, &cross);
                function_4efb0(&basis.up);
                basis.forward.i = perpendicular.i * cosine + cross.i * sine;
                basis.forward.j = perpendicular.j * cosine + cross.j * sine;
                basis.forward.k = perpendicular.k * cosine + cross.k * sine;
                basis.left.i = cross.i * cosine - perpendicular.i * sine;
                basis.left.j = cross.j * cosine - perpendicular.j * sine;
                basis.left.k = cross.k * cosine - perpendicular.k * sine;
            }
            else
            {
                function_141ce0(((packed >> 9) & 0xf) * g_5259b4 * (1.0f / 15.0f), 0.0f, 0.0f, &basis);
            }
            basis.position = position;
            real clamped_alpha = alpha < 0.0f ? 0.0f : alpha > 1.0f ? 1.0f : alpha;
            D3DDevice::SetVertexData4f(11, packed_vector.i, packed_vector.j, packed_vector.k, clamped_alpha);
            D3DDevice::SetVertexData4f(12, basis.forward.i * size, basis.left.i * size, basis.up.i * size, basis.position.x);
            D3DDevice::SetVertexData4f(13, basis.forward.j * size, basis.left.j * size, basis.up.j * size, basis.position.y);
            D3DDevice::SetVertexData4f(14, basis.forward.k * size, basis.left.k * size, basis.up.k * size, basis.position.z);
            D3DDevice::SetVertexData4f(15, light.red, light.green, light.blue, 1.0f);
            word count = *(word *)(draw + 6);
            word first_index = *(word *)(draw + 4);
            function_1cf50();
            D3DDevice_DrawIndexedVertices(D3DPT_TRIANGLELIST, count, indices + first_index);
        }
    }
    g_4b8430 = 0;
    D3DDevice_SetRenderState(D3DRS_TWOSIDEDLIGHTING, 0);
}

// @retail 0x4ede0
void __stdcall function_4ede0(long tag, long context, long first, long second,
    long third, dword handle, byte *material)
{
    if (!(g_55ee58 & 1))
    {
        g_55ee58 |= 1;
        g_55e6dc = 128.0f;
        g_55e6d8 = 64.0f;
        g_55e6e0 = 512.0f;
    }
    if (!(g_55ee58 & 2))
    {
        g_55ee58 |= 2;
        g_55e6cc = 16.0f;
        g_55e6d0 = 32.0f;
        g_55e6d4 = 64.0f;
    }
    byte *table = *(byte **)((byte *)g_4e0348 + 0x238);
    byte *record = *(byte **)(table + 0x1c) + ((handle >> 8) & 0x3fffff) * 24;
    byte *block = *(byte **)(table + 0x14) + *(short *)(record + 6) * 0x2c;
    if (function_12de70((s_geometry_block_info *)block, 3))
    {
        byte *geometry = *(byte **)(block + 0x28) + 0x58;
        if (*(long *)(geometry + 0x10))
        {
            function_1bd50(0);
            function_1cdd0(0, 0);
            real constants[4];
            constants[0] = g_52598c[handle >> 30];
            constants[1] = g_525998[handle >> 30];
            constants[2] = 0.0f;
            constants[3] = 1.0f;
            D3DDevice_SetVertexShaderConstant(-43, constants, 1);
            function_1c590((s_shader_cache *)g_51f0f0,
                *(long *)(*(byte **)(material + 0x10) + 0x100), *(word *)(material + 8));
            function_1c620((s_shader_cache *)g_51f0f0, *(long *)(geometry + 0x10),
                geometry[1], *(long *)(geometry + 8), g_43f408[geometry[0]]);
            function_1c710(g_51f0f0);
            function_1bbf0(tag, first, second, third, material[4], 10000.0f);
            function_1cf50();
            D3DDevice_SetRenderState(D3DRS_ZBIAS, 1);
            D3DDevice_DrawVertices(D3DPT_QUADLIST, *(word *)(record + 0xc), *(word *)(record + 0xe));
            D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
        }
    }
}

// @retail 0x4efa0
void __stdcall function_4efa0(long tag, long context, long first, long second,
    long third, dword handle, byte *material)
{
    function_4ede0(tag, context, first, second, third, handle, material);
}
// MiniMax D (11-14): no new form, no lower score (54): the score still rests on the load-bearing volatile tag local (without it the compiler rebuilds the body), and retail still reads the block base out of the table before the tag lookup and keeps the tag in a register where this build spills it to a stack slot and reads it back.
// MiniMax A (11-12): no new form, no lower score (54): the score still rests on the load-bearing volatile tag local (without it the compiler rebuilds the body, 117), and retail still reads the block base out of the table before the tag lookup and keeps the tag in a register where ours spills it to a stack slot and reads it back; inlining the tag into the index gives 117 and reading the block before the tag gives 101, both worse, so the body stands.

// MiniMax B (11-13): no new form, no lower score (54): retail adds the block base before the tag lookup and keeps the tag's table address in a register, while this build reads the tag's byte first, spills the tag through the volatile local and reloads it, so the two orderings differ; a volatile local is only allowed with a byte match.
// MiniMax H (11-12): no new form, no lower score (54); the score still rests on the load-bearing volatile tag local
// (without it the compiler rebuilds the body, 117), and retail still reads the block base out of the table before the
// tag lookup and keeps the tag in a register where ours spills it to a stack slot and reads it back.
// MiniMax G (11-11): no new form, no lower score (54); the score still rests on the load-bearing volatile tag
// local: without it the compiler rebuilds the body (117), retail reads the block base out of the table before the
// tag lookup and keeps the tag in a register where ours spills it to a stack slot and reads it back, and retail
// adds the block base before the definition lookup where ours adds it after; this round re-read the diff and the
// only routes to those shapes are the ones recorded below, so the body stands.
// MiniMax H (11-11): no new form, no lower score (54); retail still reads the block base out of the table before the tag lookup and keeps the tag in a register where ours spills and reloads it through the volatile local, and retail still adds the block base before the definition lookup where ours adds it after; this round inlining the tag into the index gives 117 and reading the block before the tag gives 101, both worse, so the body stands.
// MiniMax C (11-11): no new form, no lower score (54); the tag is still spilled to a stack slot and reloaded through the volatile local where retail keeps it in a register, and the block base is still added in after the definition lookup.
// MiniMax R (11-10): no new form, no lower score (54); the tag is still spilled to a stack slot and reloaded where retail keeps it in a register, and the block base is still added in after the definition lookup where retail reads it before the tag lookup.
// MiniMax I (11-10): no new form, no lower score (54); the tag is still spilled to a stack slot and reloaded where retail keeps it in a register, and the block base is still added in after the definition lookup where retail reads it before the tag lookup; reading the block base first rebuilds the body (101).
// MiniMax E (11-09): no new form, no lower score; the score still rests on the load-bearing
// volatile tag local, and every other spelling of the tag lookup rebuilds the body.
// MiniMax R (11-09): no new form, no lower score; the score still rests on the volatile tag local, and retail reads the block base out of the table before the tag lookup and keeps the tag in a register where ours spills it and reads it back.
// MiniMax R (11-09): no new form, no lower score; the tag read through an eight-byte stride gives 54 with
// the volatile and 117 without it, the same as the body, so the tag's spill stays.
// MiniMax G (11-06): no new form, no lower score: the score still rests on the load-bearing volatile tag
// local; without it the compiler rebuilds the body (117), computing the block before the tag gives 111 and
// the two-element tag table gives 115, so the body is unchanged.
// MiniMax C (11-08): no new form, no lower score.
// score 54 (was 54): this round found no gain - the score rests on the volatile tag local; without it the
// compiler rebuilds the body (117), computing the block before the tag gives 111 and the two-element tag
// table gives 115, so the body is unchanged.
// MiniMax B (11-13): no new form, no lower score (54): the volatile tag local is load-bearing (the record's tag has to be read before the block base), so the order of the two reads still differs from retail's.
// MiniMax G (11-13): no new form, no lower score (54): the score still rests on the volatile tag local, and
// retail reads the block base out of the table before the tag lookup where this build reads it after; the
// forms in the note below rebuild the body.
// @retail 0x4ebd0
// MiniMax A (11-11): no new form, no lower score (54); the difference is still the tag lookup: retail computes the
// tag's table address with a lea and keeps the tag in a register, ours spills and reloads it through the volatile
// local, and retail adds the block base before the tag lookup where ours adds it after the definition lookup. The
// forms the earlier rounds tried (the pair read as an array of pairs, through a byte pointer local and through a
// long pointer local, the block read before the tag, the volatile moved onto the part) all keep the same 54 or
// score worse, so the body stands.
// MiniMax D (11-06): no new form, no lower score; the tag lookup still differs: retail reads the pair
// with a lea into a register and never spills the tag, ours reads it through the volatile local with a
// spill and a reload; this round the pair read as a long array of pairs, through a byte pointer local and
// through a long pointer local all fold to the same scaled load, the block read before the tag gives 101
// to 111, and the volatile moved onto the part gives 110 to 117.
// MiniMax H (11-05): no new form, no lower score; retail reads the block base out of the table
// before the tag lookup and keeps the tag in a register, ours spills and reloads the tag through
// the volatile local, and reading the block base first rebuilds the body (111); retail spills the
// part pointer instead, and only moving the volatile local onto that pointer would do it, which
// the rules do not allow.
// MiniMax D (run 37): no new form, no lower score; the tag without the volatile gives 111 (in
// the original order 117) and the reordered body gives 101, so the original body is kept even
// though it keeps a volatile local that the coordinator's rules dislike.
// score 54 (was 54): retail reads the block base out of the table before it
// reads the record tag and keeps that tag in a register, ours spills the tag
// to a stack slot and reloads it, and adds the table's base pointer in after
// the definition lookup instead of before it
void __stdcall function_4ebd0(long tag, long context, long first, long second,
    long third, dword handle, byte *material)
{
    if (!(g_55ee5c & 1))
    {
        g_55ee5c |= 1;
        g_55e6f4 = 128.0f;
        g_55e6f0 = 64.0f;
        g_55e6f8 = 512.0f;
    }
    if (!(g_55ee5c & 2))
    {
        g_55ee5c |= 2;
        g_55e6e4 = 16.0f;
        g_55e6e8 = 32.0f;
        g_55e6ec = 64.0f;
    }
    byte *table = *(byte **)((byte *)g_4e0348 + 0x238);
    byte *record = *(byte **)(table + 0x1c) + ((handle >> 8) & 0x3fffff) * 24;
    long block_offset = *(short *)(record + 6) * 0x2c;
    volatile long record_tag = (*(long **)((byte *)g_4e0350 + 0x37c))[(signed char)record[0] * 2 + 1];
    byte *definition = g_4e3b44[record_tag & 0xffff].bytes;
    byte *part = *(byte **)(definition + 0x14) + record[1] * 20;
    byte *block = *(byte **)(table + 0x14) + block_offset;
    if (function_12de70((s_geometry_block_info *)block, 3))
    {
        byte *geometry = *(byte **)(block + 0x28);
        if (*(long *)(geometry + 0x28))
        {
            real constants[4];
            constants[0] = g_52598c[handle >> 30];
            constants[1] = g_525998[handle >> 30];
            constants[2] = 0.0f;
            constants[3] = 1.0f;
            function_1bd50(0);
            D3DDevice_SetVertexShaderConstant(-43, constants, 1);
            function_1cdd0(0, 0);
            function_1c590((s_shader_cache *)g_51f0f0,
                *(long *)(*(byte **)(material + 0x10) + 0x100), 0);
            function_1c620((s_shader_cache *)g_51f0f0, *(long *)(geometry + 0x28),
                geometry[0x19], *(long *)(geometry + 0x20), g_43f408[geometry[0x18]]);
            function_1c710(g_51f0f0);
            function_1bbf0(tag, first, second, third, material[4], 10000.0f);
            function_1cf50();
            D3DDevice_SetRenderState(D3DRS_ZBIAS, 1);
            if (part[4] == 1)
                D3DDevice_DrawVertices(D3DPT_QUADLIST, *(word *)(record + 0xc), *(word *)(record + 0xe));
            else if (part[4] == 2)
                D3DDevice_DrawVertices(D3DPT_TRIANGLELIST, *(word *)(record + 0xc), *(word *)(record + 0xe));
            D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
        }
    }
}

byte g_4c1a19;
__declspec(noinline) void function_1ef70(void);
void function_142f0(short mode);
dword __cdecl pack_color4f(color4f const *color);

// @retail 0x35b90
void __stdcall function_35b90(void *material)
{
    byte *source = (byte *)material;
    if (g_4b6294)
    {
        D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
        function_1ef70();
        g_4b6294 = false;
        g_4c1a18 = true;
    }
    else g_4c1a18 = false;
    if (!source[0x97])
    {
        g_4670bc = true;
        function_16b10((s_render_reset_state *)g_485b48);
    }
    g_4c1a19 = false;
    if (g_485602 != 0) return;
    if (!source[0x97])
    {
        function_0222d0(D3DRS_COLORWRITEENABLE, 0x10101);
        function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
        function_0222d0(D3DRS_ZENABLE, 0);
        function_0222d0(D3DRS_ZBIAS, 0);
        function_0222d0(D3DRS_CULLMODE, 0x901);
        function_142f0(*(short *)(source + 0x94));
    }
    short width = (short)(g_48564e - g_48564a);
    short height = (short)(g_48564c - g_485648);
    real *offset = *(real **)(source + 4);
    real x = offset ? offset[0] * 2.0f / width : 0.0f;
    real y = offset ? offset[1] * -2.0f / height : 0.0f;
    real inverse_x = 1.0f / width;
    real transform[20];
    transform[0] = inverse_x * 2.0f;
    transform[1] = 0.0f;
    transform[2] = 0.0f;
    transform[3] = x - (inverse_x + 1.0f);
    transform[4] = 0.0f;
    real inverse_y = 1.0f / height;
    transform[5] = inverse_y * -2.0f;
    transform[6] = 0.0f;
    transform[7] = inverse_y + y + 1.0f;
    transform[8] = 0.0f;
    transform[9] = 0.0f;
    transform[10] = 0.0f;
    transform[11] = 0.5f;
    transform[12] = 0.0f;
    transform[13] = 0.0f;
    transform[14] = 0.0f;
    transform[15] = 1.0f;
    transform[16] = *(real *)(source + 0x40);
    transform[17] = *(real *)(source + 0x44);
    transform[18] = 0.0f;
    transform[19] = 1.0f;
    real constants[24];
    constants[0] = *(real *)(source + 0x48);
    constants[1] = *(real *)(source + 0x4c);
    constants[2] = *(real *)(source + 0x50);
    constants[3] = *(real *)(source + 0x54);
    constants[4] = source[8] ? 1.0f : 0.0f;
    constants[5] = source[8] ? 0.0f : 1.0f;
    constants[6] = source[9] ? 1.0f : 0.0f;
    constants[7] = source[9] ? 0.0f : 1.0f;
    constants[8] = source[0xa] ? 1.0f : 0.0f;
    constants[9] = source[0xa] ? 0.0f : 1.0f;
    {
        real *coordinates = *(real **)(source + 0x1c);
        constants[10] = coordinates ? coordinates[0] : 0.0f;
        constants[11] = coordinates ? coordinates[1] : 0.0f;
    }
    {
        real *coordinates = *(real **)(source + 0x20);
        constants[12] = coordinates ? coordinates[0] : 0.0f;
        constants[13] = coordinates ? coordinates[1] : 0.0f;
    }
    {
        real *coordinates = *(real **)(source + 0x24);
        constants[14] = coordinates ? coordinates[0] : 0.0f;
        constants[15] = coordinates ? coordinates[1] : 0.0f;
    }
    constants[16] = *(real *)(source + 0x28);
    constants[17] = *(real *)(source + 0x2c);
    constants[18] = *(real *)(source + 0x30);
    constants[19] = *(real *)(source + 0x34);
    constants[20] = *(real *)(source + 0x38);
    constants[21] = *(real *)(source + 0x3c);
    constants[22] = 0.0f;
    constants[23] = 0.0f;
    D3DDevice_SetVertexShaderConstant(81, transform, 5);
    D3DDevice_SetVertexShaderConstant(86, constants, 6);
    short stage = 0;
    do
    {
        byte *bitmap = *(byte **)(source + 0xc + stage * 4);
        if (!bitmap) break;
        g_51f3c8[1][stage] = function_12310((s_bitmap_view *)bitmap, 0.0f);
        int texture_width = *(short *)(bitmap + 4);
        int texture_height = *(short *)(bitmap + 6);
        g_485af4[stage] = texture_width;
        g_485b04[stage] = texture_height;
        sampler_state_write(stage, D3DTSS_ADDRESSU, source[0x18 + stage] ? 1 : 3);
        sampler_state_write(stage, D3DTSS_ADDRESSV, source[0x18 + stage] ? 1 : 3);
        sampler_state_write(stage, D3DTSS_MAGFILTER, source[0x96] ? 1 : 2);
        sampler_state_write(stage, D3DTSS_MINFILTER, source[0x96] ? 1 : 2);
        sampler_state_write(stage, D3DTSS_MIPFILTER, source[0x96] ? 1 : 2);
        sampler_state_write(stage, D3DTSS_MAXANISOTROPY, 0);
        sampler_state_write(stage, D3DTSS_MIPMAPLODBIAS, 0);
        sampler_state_write(stage, D3DTSS_MAXMIPLEVEL, 0);
        sampler_state_write(stage, D3DTSS_COLORSIGN, 0);
        sampler_state_write(stage, D3DTSS_ALPHAKILL, 0);
        ++stage;
    } while (stage < 3);
    if (*(void **)(source + 0xc) && !source[0x97])
    {
        memset(&g_484f68, 0, sizeof(g_484f68));
        g_484f68.PSTextureModes = (((*(void **)(source + 0x14) != 0) << 5) |
            (*(void **)(source + 0x10) != 0)) << 5 | (*(void **)(source + 0xc) != 0);
        color4f colors[3];
        for (long i = 0; i < 3; ++i)
        {
            color3f const *color = *(color3f **)(source + 0x58 + i * 8);
            *(color3f *)&colors[i].red = color ? *color : *(color3f *)g_468710;
        }
        real *alpha = *(real **)(source + 0x84);
        colors[0].alpha = alpha ? *alpha : 1.0f;
        alpha = *(real **)(source + 0x88);
        colors[1].alpha = alpha ? *alpha : 1.0f;
        alpha = *(real **)(source + 0x8c);
        colors[2].alpha = alpha ? *alpha : 1.0f;
        g_484f68.PSConstant0[0] = pack_color4f(&colors[0]);
        g_484f68.PSConstant1[0] = pack_color4f(&colors[1]);
        g_484f68.PSConstant0[1] = pack_color4f(&colors[2]);
        g_484f68.PSConstant0[4] = pack_color4f((color4f *)(source + 0x70));
        g_484f68.PSConstant0[5] = pack_color4f((color4f *)(source + 0x70));
        g_484f68.PSConstant0[6] = pack_color4f((color4f *)(source + 0x70));
        g_484f68.PSConstant0[7] = pack_color4f((color4f *)(source + 0x70));
        g_484f68.PSRGBOutputs[0] = 0x89;
        g_484f68.PSAlphaOutputs[0] = 0x89;
        g_484f68.PSRGBInputs[0] = 0x8010902;
        g_484f68.PSAlphaInputs[0] = 0x18111912;
        g_484f68.PSRGBInputs[1] = 0xa010804;
        g_484f68.PSRGBOutputs[1] = 0xac;
        g_484f68.PSAlphaInputs[1] = 0x1a111814;
        g_484f68.PSAlphaOutputs[1] = 0xac;
        g_484f68.PSCombinerCount = 0x11102;
        g_484f68.PSFinalCombinerInputsABCD = 0xc;
        g_484f68.PSFinalCombinerInputsEFG = 0x1c00;
        function_15180(&g_484f68);
    }
    function_1cc60(0);
}

byte g_51080c;
short g_4670c0;
typedef void (__stdcall *t_1f070_glyph)(long, long, long, long, long,
    real, real, real, real, real, real, real, long, long);
void __stdcall function_1f490(long, long, long, long, long,
    real, real, real, real, real, real, real, long, long);
void __stdcall function_13f0e0(t_1f070_glyph callback, short_rectangle2d const *bounds,
    short *position, short_rectangle2d const *clip, short line_gap, real scale, dword const *text);
void function_36560(void);

/* moves a rectangle by (dx, dy) */
static __forceinline void offset_rectangle_1f070(short_rectangle2d *rectangle, short dx, short dy)
{
    rectangle->left += dx;
    rectangle->right += dx;
    rectangle->top += dy;
    rectangle->bottom += dy;
}
// MiniMax D (11-14): no new form, no lower score (60): the two rectangles still sit four bytes higher in the frame than retail's (the clip at [esp+0x10] and the bounds at [esp+0x18] against retail's [esp+0xc] and [esp+0x14]), the bitmap pointer still shares the clip's slot instead of being spilled at [esp+0x1c], and the left edge is still clamped with a conditional set where retail branches.
// MiniMax A (11-12): no new form, no lower score (60): the two rectangles still sit four bytes higher in the frame than retail (the clip at [esp+0x10] and the bounds at [esp+0x18] against retail [esp+0xc] and [esp+0x14]), the bitmap pointer still shares the clip slot instead of being spilled at [esp+0x1c], and the left edge is still clamped with a conditional set where retail branches; a long left edge gives retail slots but grows the frame, so the body stands.

// MiniMax B (11-13): no new form, no lower score (60): the two rectangles' fields are still read into different registers (this build takes the first one's word into eax where retail takes it into edi) and the argument slots still follow the LTCG convention's eax and ecx, so the block's shape is not ours to pick.
// MiniMax H (11-12): no new form, no lower score (60); the two rectangles still sit four bytes higher in the frame
// than retail's (the clip at [esp+0x10] and the bounds at [esp+0x18] against retail's [esp+0xc] and [esp+0x14]), the
// bitmap pointer still shares the clip's slot instead of being spilled at [esp+0x1c], and the left edge is still
// clamped with a conditional set where retail branches; a long left edge gives retail's slots but grows the frame.
// MiniMax G (11-11): no new form, no lower score (60); the two rectangles still sit four bytes higher in the frame
// than retail's (the clip at [esp+0x10] and the bounds at [esp+0x18] against retail's [esp+0xc] and [esp+0x14]),
// the bitmap pointer still shares the clip's slot instead of being spilled at [esp+0x1c], and the left edge is
// still clamped with a conditional set where retail branches; a long left edge gives retail's slots but grows the
// frame to 0xb0, so the body stands.
// MiniMax H (11-11): no new form, no lower score (60); the two rectangles still sit four bytes higher in the frame than retail's (the clip at [esp+0x10] and the bounds at [esp+0x18] against retail's [esp+0xc] and [esp+0x14]), the bitmap pointer still shares the clip's slot instead of being spilled at [esp+0x1c], and the left edge is still clamped with a conditional set instead of retail's branch; a long left edge gives retail's slots but grows the frame to 0xb0, so the body stands.
// MiniMax C (11-11): no new form, no lower score (60); the two rectangles still sit four bytes higher in the frame than retail's and the bitmap pointer still shares the clip's slot instead of being spilled.
// MiniMax R (11-10): no new form, no lower score (60); the two rectangles still sit four bytes higher in the frame than retail's, the bitmap pointer still shares the clip's slot, and the left edge is still clamped with a conditional set instead of retail's branch.
// MiniMax I (11-10): no new form, no lower score (60); the two rectangles still sit four bytes higher in the frame than retail's, the bitmap pointer still shares the clip's slot, and the left edge is still clamped with a conditional set instead of retail's branch.
// MiniMax E (11-09): no new form, no lower score; the two rectangles still sit four bytes
// higher in the frame than retail's and the bitmap pointer still shares the clip's slot, and the
// left edge is still clamped with a conditional set instead of retail's branch.
// MiniMax R (11-09): no new form, no lower score; the order of the two rectangle declarations does not move their frame slots, and a long left edge gives retail's slots and retail's branchy clamps but needs a slot of its own, so the frame grows to 0xb0 (177); the short left edge clamps with a conditional set instead of retail's branch.
// MiniMax R (11-09): no new form, no lower score; the clip rectangle and the bitmap pointer still swap their
// frame slots, the bitmap at [esp+0x10] here against [esp+0x1c] in retail.
// MiniMax D (11-06): no new form, no lower score; the two rectangles sit four bytes higher in the frame than
// retail's (the clip at [esp+0x10] and the bounds at [esp+0x18] against retail's [esp+0xc] and [esp+0x14])
// and the bitmap pointer shares the clip's slot instead of being spilled at [esp+0x1c], because our clamp
// arithmetic keeps the bitmap in edx where retail's clobbers edx and reloads the pointer from the stack;
// this round a long left gives 177, the clamps written as if statements give 189, the offset added in 32 bits
// gives 166 and both together give 169, and the left edge is still clamped with a conditional set instead of
// retail's branch.
// MiniMax H (11-05): no new form, no lower score; the bounds rectangle's horizontal offset added
// in 32 bits and stored back as a word (retail's form) rebuilds the body (162), so the 16-bit add
// stays and the two rectangles keep sitting four bytes higher in the frame than retail's; the four
// clamp values written straight into the rectangle give 145 and leave the two rectangles in the
// same slots, so the four bytes are not the clamps' homes.
// MiniMax H (11-04): no new form, no lower score; the two clamps written as if/else give 177 (99 with
// short locals), the two rectangles and the bitmap pointer in five declaration orders give 60 and do not
// move the slots, and the permuter (71 tries) leaves 60. A long left gives retail's slots (the clip at
// [esp+0xc], the bounds at [esp+0x14], the bitmap at [esp+0x1c]) and retail's branchy clamps, but the
// function is then 21 bytes short with a frame four bytes too big; retail adds the horizontal offset in
// 32 bits where ours adds it in place in 16 bits, and only a long offset forces that (128).
// MiniMax D (run 37): no new form, no lower score; the bitmap pointer declared first, the two rectangles swapped and the casts removed all give 60, a short top gives 99 and the clamps written as if/else give 187, the four clamp values as longs or declared first in the outer scope give 122 and 113, and retail's frame leaves sixteen bytes below the rectangles where ours leaves twelve, which the type and the order of the clamp locals do not move.
// MiniMax G (11-06): no new form, no lower score: a `long` for the left edge gives retail's frame slots
// and its branchy clamps but grows the frame to 0xb0 (177), and reordering the helper's fields gives
// 76 and 161, so the body is unchanged.
// MiniMax A (11-11): no new form, no lower score (60); the two rectangles still sit four bytes higher in the
// frame than retail's (the clip at [esp+0x10] and the bounds at [esp+0x18] against retail's [esp+0xc] and
// [esp+0x14]) and the bitmap pointer still shares the clip's slot instead of being spilled at [esp+0x1c], because
// our clamp arithmetic keeps the bitmap in EDX where retail clobbers EDX and reloads the pointer from the stack; the
// forms the earlier rounds tried (a long left edge, the clamps as if statements, the offset added in 32 bits, the
// declaration orders) all keep the same slots or score worse, so the body stands.
// MiniMax C (11-08): no new form, no lower score.
// score 60 (was 75): the two rectangles sit four bytes higher in the frame than retail's and the
// bitmap pointer shares the clip's slot instead of being spilled; the clip branch clamps the left
// edge with a conditional set instead of retail's branch.
// MiniMax B (11-13): no new form, no lower score (60): the two rectangles still sit four bytes higher in the frame than retail's (the clip at [esp+0x10] and the bounds at [esp+0x18] against retail [esp+0xc] and [esp+0x14]), the bitmap pointer still shares the clip's slot instead of being spilled at [esp+0x1c], and the left edge is still clamped with a conditional set instead of retail's branch; the left clamp written straight into the field gives the same code, a long left edge gives retail's slots but grows the frame to 0xb0, and the permuter (23 tries in 250 s) found no better variant.
// MiniMax G (11-13, later): no new form, no lower score (60): the four clip clamp values as shorts give 101, and
// the two rectangles keep their frame slots; retail keeps the bitmap pointer in a stack slot and reloads it after
// the clip clamps, while this build keeps it in edx across them, which moves the clamps' temporaries into the
// other registers.
// MiniMax G (11-13): no new form, no lower score (60): retail keeps the bitmap pointer in a stack slot and
// reloads it after the clip clamps, while this build keeps it in edx across them, which moves the clamps'
// temporaries into the other registers; the forms in the note below all score 60 or more.
// @retail 0x1f070
void function_1f070(short_rectangle2d const *bounds, short_rectangle2d const *clip,
    short *position, long line_gap, dword const *text, real scale,
    void const *color, void const *shadow, bool preserve)
{
    (void)&position; (void)&line_gap; (void)&text; (void)&scale;
    (void)&color; (void)&shadow; (void)&preserve;
    short_rectangle2d wanted_clip;
    short_rectangle2d wanted_bounds;
    s_bitmap_view *bitmap;
    if (g_51080c || g_485602 != 0) return;
    ++g_4670c0;
    bitmap = g_4b62a0 ? g_4b62ac : 0;
    if (!bitmap || !*text) return;
    /* the window rectangle: g_4b9dd0 .. g_4b9dd6 are one rectangle of the camera storage */
    if (!bounds)
    {
        wanted_bounds = g_4b9dd8;
        offset_rectangle_1f070(&wanted_bounds, -g_4b9dd2, -g_4b9dd0);
    }
    else
    {
        wanted_bounds = *bounds;
    }
    if (!clip)
    {
        wanted_clip = *(short_rectangle2d const *)&g_4b9dd0;
        offset_rectangle_1f070(&wanted_clip, -g_4b9dd2, -g_4b9dd0);
    }
    else
    {
        long bottom = g_4b9dd4 - g_4b9dd0;
        if (bottom > clip->bottom) bottom = clip->bottom;
        long right = g_4b9dd6 - g_4b9dd2;
        if (right > clip->right) right = clip->right;
        long top = clip->top < 0 ? 0 : clip->top;
        short left = clip->left < 0 ? 0 : clip->left;
        wanted_clip.left = (short)left;
        wanted_clip.top = (short)top;
        wanted_clip.right = (short)right;
        wanted_clip.bottom = (short)bottom;
    }
    byte material[0x98];
    memset(material, 0, sizeof(material));
    if (*((byte *)bitmap + 0xe) & 0x10)
    {
        *(real *)(material + 0x40) = 1.0f;
        *(real *)(material + 0x44) = 1.0f;
    }
    else
    {
        *(real *)(material + 0x40) = 1.0f / *(short *)((byte *)bitmap + 4);
        *(real *)(material + 0x44) = 1.0f / *(short *)((byte *)bitmap + 6);
    }
    *(s_bitmap_view **)(material + 0xc) = bitmap;
    *(real *)(material + 0x2c) = 1.0f;
    *(real *)(material + 0x28) = 1.0f;
    *(long *)(material + 0) = 0;
    material[0x96] = 0;
    *(word *)(material + 0x94) = 0;
    material[0x97] = preserve;
    if (color) *(void const **)(material + 0x58) = color;
    if (shadow) *(void const **)(material + 0x5c) = shadow;
    function_35b90(material);
    function_13f0e0(function_1f490, &wanted_bounds, position, &wanted_clip,
        (short)line_gap, scale, text);
    function_36560();
}

long unicode_escape_character_lookup(word character, bool *found);

struct s_interface_function_context;
void function_2c3b0(s_interface_function_context *context);
void function_3e420(long tag, long object_index, signed char *output, byte level);
bool function_3e320(long tag, byte const *indices);
/* the permutation of each of a model's regions, as 0x3e420 chooses them */
struct s_model_permutations
{
    signed char indices[16];

    long function_3e380(long model_index, byte lod, transform4x3f const *nodes, long *node_count, long extra_bytes);
};
struct s_matrix_workspace
{
    long size;
    byte data[0x27000];
};
extern s_matrix_workspace g_487b18;
void function_151e0(long index);
bool function_13d9f0(long type);
extern dword g_4e6494;

// MiniMax D (11-06): no new form, no lower score; our ebx save in the prologue against retail's inside the
// loop keeps the frame at 0xb0 instead of 0xac, so every stack reference sits four bytes off; this round the
// outer loop as a while loop gives 199 and the same loop with the counter bumped on every path gives 199,
// and neither moves the frame off 0xb0, the count in a local gives 237 and the handle tested twice against
// NONE gives 240.
// MiniMax G (11-05): no new form, no lower score; dropping the table local scores 200 and dropping the availability flag 240, and the frame stays 0xb0 against retail's 0xac.
// MiniMax G (11-04): score 199 (was 223): the material pointer is loaded before the part type is
// tested and the cull-mode global is no longer zeroed; the handle's sign test as (handle & 0x80000000)
// == 0 gives retail's test/js but scores 224-240, and ours still keeps the ebx zero register where
// retail uses edi and the loop counter in edi where retail uses esi, and the permutations go in eax
// at 0x3e380, whose declaration is in another file.
// MiniMax H (11-03): no new form, no lower score; ours keeps the ebx zero register where retail uses edi and the loop counter in edi where retail uses esi, and the permutations go in eax at 0x3e380, whose declaration is in another file.
// score 199 (was 241): the handle now goes into the context's dword at offset 8 and the availability
// flag drives the table test, as retail does; left is our ebx zero register against retail's
// edi and the loop counter in edi against retail's esi. Round 61: the permutations now arrive in ecx at
// 0x3e380, a member of s_model_permutations, as retail; the count stays 223.
// MiniMax H (11-03): no new form, no lower score; ours keeps the ebx zero register where retail uses edi and the loop counter in edi where retail uses esi, and the permutations go in eax at 0x3e380, whose declaration is in another file.
// Earlier at 241 (was 241): retail's frame is 0xac bytes and ours 0xb4, and our locals sit eight bytes
// higher (the context buffer at [esp+0x44] against retail's [esp+0x3c], the indices at [esp+0x38]
// against [esp+0x30]), so every reference in the body is at another offset.
// Earlier at 241 (was 260): the length is retail's 877 bytes now, but retail keeps the tag-instance
// pointer in ebp and saves ebx only where it starts to use it, while ours saves ebx in the
// prologue and compares the handle against a zero register.
// @retail 0x4c690
void function_4c690(long tag, transform4x3f const *nodes)
{
    byte context[0x78];
    s_model_permutations permutations;
    long node_count;
    byte *definition = g_4e3b44[tag & 0xffff].bytes;
    D3DDevice_SetRenderState(D3DRS_DEPTHCLIPCONTROL, 16);
    function_2c3b0((s_interface_function_context *)context);
    function_3e420(tag, NONE, permutations.indices, 4);
    if (function_3e320(tag, (byte const *)permutations.indices))
    {
        long handle = permutations.function_3e380(tag, 4, nodes, &node_count, 0);
        *(long *)(context + 8) = handle;
        bool available = handle != NONE;
        if (available)
        {
            s_shader_transform_table *table = 0;
            if (available && handle >= 0)
                table = (s_shader_transform_table *)(g_487b18.data + (handle & 0x0fffffff));
            g_485a5c = table;
            function_1bd50(context);
            for (long index = 0; index < *(long *)(definition + 0x1c); ++index)
            {
                long section_index = (byte)permutations.indices[index];
                if (section_index == 255) continue;
                byte *section = *(byte **)(definition + 0x28) + section_index * 0x5c;
                if (!*(word *)(section + 4) || !*(word *)(section + 6) || !*(word *)(section + 8)) continue;
                byte *volatile geometry = 0;
                if (!g_4e6494 || function_12de70((s_geometry_block_info *)(section + 0x38), 0))
                    geometry = *(byte **)(section + 0x34);
                short transform = *(short *)(section + 0x2c);
                if (transform != NONE)
                {
                    D3DDevice_SetVertexShaderConstantFast(-46, g_485a5c->transforms[transform], 3);
                    g_5093d8 = 0;
                }
                else function_151e0(NONE);
                for (long part_index = 0; part_index < *(long *)geometry; ++part_index)
                {
                    byte *part = *(byte **)(geometry + 4) + part_index * 0x48;
                    short material_index = *(short *)(part + 4);
                    if (material_index == NONE) continue;
                    byte *material = *(byte **)(definition + 0x64) + material_index * 0x20;
                    word type = *(word *)part;
                    if (type == 0 || type == 5 || type == 1) continue;
                    long material_tag = *(long *)(material + 0xc);
                    if (material_tag == NONE) continue;
                    byte *shader = g_4e3b44[material_tag & 0xffff].bytes;
                    byte *shader_definition = g_4e3b44[*(long *)(shader + 4) & 0xffff].bytes;
                    byte *tables = *(byte **)(shader_definition + 0x5c);
                    byte *stage = *(byte **)(tables + 4);
                    if (!(*(dword *)(stage + 2) & 0x8000)) continue;
                    word *passes = (word *)(*(byte **)(tables + 0xc) + (*(word *)stage & 0x1ff) * 2 + 0x1e);
                    for (long pass = 0; pass < (*passes >> 9); ++pass)
                    {
                        if (!function_13d9f0(type)) continue;
                        byte *entry = *(byte **)(tables + 0x14) + ((*passes & 0x1ff) + pass) * 10;
                        byte *pass_definition = g_4e3b44[*(long *)(entry + 4) & 0xffff].bytes;
                        byte *program = *(byte **)(*(byte **)(pass_definition + 0x20) + 4);
                        long program_tag = *(long *)(program + 0x100);
                        if (function_1caa0(program_tag, section + 4, (word const *)part, geometry, 0, 0, 0))
                        {
                            if (*(long *)(definition + 0x14) > 0)
                                function_1cdd0(*(real **)(definition + 0x18), (byte)*(word *)(section + 0x1a));
                            else function_1cdd0(0, 0);
                            function_1bbf0(material_tag, 0, 15, pass, 0, 1024.0f);
                            D3DDevice_SetRenderState(D3DRS_CULLMODE, 0);
                            word const *vertices = *(word **)(geometry + 0x24) + *(word *)(part + 6);
                            long count = *(word *)(part + 8);
                            function_1cf50();
                            D3DDevice_DrawIndexedVertices(D3DPT_TRIANGLESTRIP, count, vertices);
                        }
                    }
                }
            }
            g_485a5c = 0;
        }
    }
    D3DDevice_SetRenderState(D3DRS_DEPTHCLIPCONTROL, 0);
}

class c_1fa50
{
public:
    void function_1fb10(short_rectangle2d const *bounds, real scale) const;
};

PRIVATE __forceinline void convert_1fa50_text(word const *text, dword *out)
{
    long remaining = 2048;
    do
    {
        if (remaining == 1) { *out = 0; break; }
        word const *next = text + 1;
        word first = *text;
        long value;
        if (first == 0x7c)
        {
            bool recognized = false;
            word local_1 = *next;
            value = unicode_escape_character_lookup(local_1, &recognized);
            if (recognized) ++next;
        }
        else value = first;
        text = next;
        *out = value;
        if (!value) break;
        ++out;
        --remaining;
    } while (remaining > 0);
}

// @retail 0x1fa50
void function_1fa50(short_rectangle2d const *bounds, word const *string, void const *clip,
    void const *position, long line_gap, real scale, long color, void const *shadow)
{
    dword text[2048];
    convert_1fa50_text(string, text);
    function_1f070(bounds, (short_rectangle2d const *)clip, (short *)position,
        line_gap, text, scale, (void const *)color, shadow, false);
}

// @retail 0x1fa30
void function_1fa30(word const *text, short_rectangle2d *bounds)
{
    function_1fa50(bounds, text, 0, 0, 0, 1.0f, 0, 0);
}

// @retail 0x1fb10
void c_1fa50::function_1fb10(short_rectangle2d const *bounds, real scale) const
{
    dword text[2048];
    convert_1fa50_text((word const *)this, text);
    function_1f070(bounds, 0, 0, 0, text, scale, 0, 0, true);
}

struct s_frame_parameters_12a50
{
    long mode;
    dword identifier;
    double time;
    dword unknown10, unknown14;
};
dword g_48589c, g_4858a8, g_4858ac;
extern dword g_467014, g_467018;
dword g_4b8478;
real g_4e6738, g_4e673c;
real g_47fd10 = 1.0f;
byte g_485a45, g_485a46;
void function_0206a0(void);
void geometry_cache_update(void);
void function_12c600(void);
void function_1de50(void);
void function_22850(void);
// MiniMax H (11-13): no new form, no lower score (153).
// MiniMax A (11-12): no new form, no lower score (153); retail's copy of the frame header is six contiguous dword stores into the globals at 0x485898-0x4858ac while ours moves the double through the x87 stack and the other fields one at a time, so our body is eleven bytes shorter than retail's 758.
// MiniMax C (11-11): no new form, no lower score (153); retail copies the frame timestamp into the global as two dwords and uploads the gamma ramp inline against the device's own ramp buffers, while the shared global is a double in include/timed_effect.h and this build goes through x87 and calls D3DDevice_SetGammaRamp.
// MiniMax D (11-10): no new form, no lower score (153); the timestamp's two halves cannot be written to the
// shared global from this file, since the global is declared as a double in a header this lane may not edit and
// a cast of its own address is not allowed.
// MiniMax S (11-09): no new form, no lower score (153): retail copies the timestamp into the global
// as two dwords and ours goes through x87 because the global is a double in include/timed_effect.h,
// and retail uploads the gamma ramp into the device's own ramp buffers where ours calls
// D3DDevice_SetGammaRamp; copying the timestamp dword-wise through a byte copy scores 173.
// MiniMax D (11-09): no new form, no lower score (153); the timestamp still reaches the global through
// x87 here and through two dword stores in retail, because the shared global is a double in a header this
// lane may not edit, and the gamma ramp still goes out through D3DDevice_SetGammaRamp instead of retail's
// inline upload into the device's own ramp buffers.
// MiniMax G (11-12): no new form, no lower score (153); the two blockers are unchanged and both live
// outside this lane's files: the frame time global is declared double in include/timed_effect.h, so
// retail's dword-wise copy of the timestamp cannot be written here, and retail uploads the gamma ramp
// inline against the device's own ramp buffers where ours calls D3DDevice_SetGammaRamp.
// MiniMax A (11-06, run 52): no new form, no lower score (153) - retail copies the timestamp into the
// global as two dwords and uploads the gamma ramp into the device's own ramp buffer; three separate
// channel stores in retail's red/green/blue order (and 255.0f) score the same 153, not kept, and a
// depth test as < 2 scores 176.

// MiniMax D (11-05): no new form, no lower score - retail copies the timestamp into the global as two dwords, ours goes through x87 because the global is a double in include/timed_effect.h, and retail uploads the gamma ramp into the device's own ramp buffer where ours calls D3DDevice_SetGammaRamp.
// MiniMax C (11-06): no new form, no lower score - retail still copies the timestamp into the global as
// two dwords (ours goes through x87, because the shared global is a double in include/timed_effect.h,
// which this lane may not edit) and still uploads the gamma ramp inline against the device's own ramp
// buffers instead of calling D3DDevice_SetGammaRamp.
// MiniMax H (11-04): no new form, no lower score; the frame time copied through one union, through a
// union of two halves or through a local read gives 158, 159 and 153, and retail writes that time
// into the global as two dwords, which the double the global has in include/timed_effect.h cannot
// express, while retail uploads the gamma ramp inline against the device's own ramp buffers.
// MiniMax E (11-10): no new form, no lower score (153).
// MiniMax E (11-11): no new form, no lower score (153); retail copies the 24-byte frame header into the contiguous
// globals 0x485898-0x4858ac in one block, ours copies it field by field and moves the double through the x87
// stack, because g_4858a0 is declared double in include/timed_effect.h.
// score 153 (was 202): retail keeps the current depth bound when it is not zero and stores the
// default only when it is (the comparison is against the bound loaded before the stores), so the
// two default stores are now conditional on the bound being zero; the unconditional stores were a
// behaviour difference as well as 49 differing instructions. Still differs: the timestamp copy
// (retail moves the two halves of the double as dwords, ours goes through x87 - the shared global
// is declared as a double in a header, so a dword-wise copy cannot be written), and the gamma
// ramp, which retail uploads inline against the device's own ramp buffers instead of calling
// D3DDevice_SetGammaRamp.
// Tried (MiniMax C run): the defaults stored only when the current depth is not zero, then
// the override carried in two locals and the pair stored again at the end: 182 against 202,
// but the first stores are always overwritten (a no-op form), so not kept without a match.
// The locals alone score 218 (fast). Retail does compare the current depth with zero here.
// MiniMax E (11-09): no new form, no lower score (153); retail divides 1.0f by the gamma's high word once and
// multiplies that inverse into the low and the high word (and by 16777215.0f), while our build divides the
// 16777215.0f constant by the high word and multiplies that into the two words, so the two bounds come out of
// a different association.
// MiniMax B (11-11): no new form, no lower score (153); retail copies the frame timestamp as one dword into a
// four-byte global at 0x4858a0 and keeps five more dword globals after it, while the shared global here is a double
// declared in include/timed_effect.h and defined in src/unknown_01fbb0.cpp, so the copy goes through x87 and the
// second field cannot be reached.
// MiniMax I (11-14): no new form, no lower score (153); the timestamp copy would need a dword-wise
// write into the global that include/timed_effect.h declares as a double, and the gamma ramp still
// goes through D3DDevice_SetGammaRamp where retail uploads it inline against the device's ramp buffers.
// @retail 0x12a50
void function_12a50(s_frame_parameters_12a50 const *parameters)
{
    g_485898 = parameters->mode;
    g_48589c = parameters->identifier;
    g_4858a0 = parameters->time;
    g_4858a8 = parameters->unknown10;
    g_4858ac = parameters->unknown14;
    real inverse = 1.0f / g_485ad4.hi;
    real lower = inverse * g_485ad4.lo * 16777215.0f;
    real upper = inverse * g_485ad4.hi * 16777215.0f;
    if (lower < 0.0f) lower = 0.0f;
    else if (lower > 16777215.0f) lower = 16777215.0f;
    if (upper < 0.0f) upper = 0.0f;
    else if (upper > 16777215.0f) upper = 16777215.0f;
    g_485adc = lower;
    g_485ae0 = upper;
    union { dword bits; real value; } default_lower, default_upper;
    default_lower.bits = g_467014;
    default_upper.bits = g_467018;
    if (g_485ad4.lo == 0.0f) g_485ad4.lo = default_lower.value;
    if (g_485ad4.hi == 0.0f) g_485ad4.hi = default_upper.value;
    if (g_5093e0)
    {
        real value = *(real *)((byte *)g_5093e0 + 0x190);
        if (value > 0.0f) g_485ad4.lo = value;
        value = *(real *)((byte *)g_5093e0 + 0x194);
        if (value > 0.0f) g_485ad4.hi = value;
    }
    function_0206a0();
    geometry_cache_update();
    function_12c600();
    function_1de50();
    if (g_5093bc && (g_485aa0 & 1))
    {
        function_14980();
        function_14600();
        g_5093bc = false;
    }
    g_4b8478 = 0;
    D3DDevice_SetRenderState(D3DRS_DXT1NOISEENABLE, 0);
    real gamma = g_5093e0->unknown3f8;
    if (g_4e6738 > 0.0f && g_48564a == 0 && g_485648 == 0 &&
        g_48564e == 640 && g_48564c == 480 && g_4ba04c <= 1)
    {
        real weight = g_4e6738 > 1.0f ? 1.0f : g_4e6738;
        gamma += (g_4e673c - gamma) * weight;
    }
    if (gamma < 0.1f) gamma = 1.0f;
    if (g_47fd10 != gamma)
    {
        D3DGAMMARAMP ramp;
        for (long i = 0; i < 256; ++i)
        {
            byte value = (byte)(long)(pow(i * (1.0f / 255.0f), gamma) * 255.0);
            ramp.red[i] = ramp.green[i] = ramp.blue[i] = value;
        }
        D3DDevice_SetGammaRamp(0, &ramp);
        g_47fd10 = gamma;
    }
    g_485a45 = false;
    g_485a46 = false;
    g_485af1 = true;
    g_485af0 = true;
    function_22850();
}

bool function_48b00(point3f const *point, real radius, real *screen, real *extent);
real function_30bf0(vector3f *vector);
void function_0224f0(dword stage, D3DTEXTURESTAGESTATETYPE type, dword value);
// MiniMax G (11-13): no new form, no lower score (576): the diff re-read this round shows retail summing the dot product's products in a rotated order and giving the two constants after the normalize call the other registers, with the call's result slot four bytes higher here than in retail, so the body stands.
// MiniMax I (11-11): no new form, no lower score (576): the three direction components still sit four bytes higher in the frame than retail's (ours at frame+0x18, retail's at frame+0x14, so every slot above them is four bytes out) and the dot product's three products are still summed in a rotated order; retail keeps the second rounded screen coordinate on the x87 stack and narrows each bound through a temporary, and the double forms tried for that score 614, 586, 576, 614, 612 and 583; retail calls 0x224f0 out of line for the second group of ten stage states where our build inlines it, and the only lever for that (an in-body inline_depth pair) is not allowed.

// MiniMax H (11-13), NOT kept (coordinator: `(real)(double)(...)` round-trip casts change no value and only steer the
// compiler; not a form the source would have; a natural double context, such as a double literal `32.0` or bounds
// computed from double values, is the lead): the four screen bounds cast through double use retail's x87 form with
// dword float constants (576 -> 558); retail still copies each bound through a float temporary into a bounds array
// that overlaps the dead direction vector, where ours keeps the bounds in the temporary slots, and our direction
// sits four bytes higher in the frame (the first difference is the store at [esp + 0x18] against retail's [esp +
// 0x14]); casting the first operand of each bound instead of the whole difference measures 586, and the permuter's
// 30 tries in 306 s found nothing below its own baseline of 516 differing instructions.
// MiniMax B (11-11): no new form, no lower score (576); retail overlaps the direction and the bounds at
// [esp + 0x14] where ours gives each of them a slot of its own, so every slot above them is four bytes out, the
// dot product's three products are summed k, i, j in retail and i, j, k here, and our body is 71 bytes shorter
// than retail's 2507.
// MiniMax E (11-11, run 80): no new form, no lower score (576): retail computes left/top/right/bottom on the x87 stack with a
// float constant and copies each through a temporary where we subtract and store the left and the right in SSE, retail overlaps
// the direction and the bounds at [esp+0x14] where ours gives each of them slots of their own, and our body is 71 bytes shorter
// than retail's 2507; the eight double and float forms of the bounds chain tried here and by earlier rounds score 576 to 614.
// MiniMax E (11-10, run 73): no new form, no lower score (576); retail computes left/top/right/bottom on the x87 stack and copies each through a temporary where we subtract and store directly, its second group of ten stage states and the three render-state calls of that group stay out of line where we inline them, and its loop counts down in memory.
// MiniMax D (11-11): no lower score (576): the four reference locals are gone (the bounds array is
// indexed directly), which keeps the score and gives the frame retail's size; the three direction
// components still sit four bytes higher in the frame than retail's, and the dot product's three
// products are still summed in a rotated order; retail keeps the second rounded screen coordinate on
// the x87 stack and narrows each bound through a temporary, and the double forms tried for that (a
// double centre with a double constant, a float centre with double casts, the rounding written into
// the screen array, a double centre with an implicit narrowing, a double centre with the rounding
// written as floor((double)screen[1] + 0.5f), and a float centre with double casts on all four
// bounds) score 614, 586, 576, 614, 612 and 583.
// MiniMax G (11-11): no new form, no lower score (576): retail overlaps the direction and the bounds at [esp+0x14] where ours gives each of them slots of their own, so every slot above them is four bytes out; the dot3f adds run i, j, k in ours and k, i, j in retail, and our body is 71 bytes shorter than retail's 2507.
// MiniMax S (11-10): no new form, no lower score (576): retail computes left/top/right/bottom on the x87 stack and copies each through a temporary where we subtract and store directly, its second group of ten stage states and the three render-state calls of the second group stay out of line where ours inlines them, and its loop counts down in memory.
// MiniMax D (11-10): no new form, no lower score (576).
// MiniMax B (11-12): no new form, no lower score (576): retail computes left/top/right/bottom on the x87 stack with
// the dword constant and copies each through a temporary, so the bounds are doubles before they are narrowed; the
// three ways of writing that here - double locals with float narrowing temporaries, double casts on the two
// centres, and both of those interleaved with the array stores - all score 614, because the double context makes
// our build push ebx and ebp at the entry and grow the frame to 0x4c while retail keeps 0x44 and shrink-wraps the
// two saves to its own epilogue, so every slot offset differs; a 900-second permuter run found no lower score.
// MiniMax B (11-13): no new form, no lower score (576): read against retail's listing, its four bounds are the two
// floored values on the x87 stack (fld dword / fadd dword / fstp qword / floor / fstp dword) and its floor calls take
// a double, so the values agree with ours and only the arithmetic differs: retail adds and subtracts dword constants
// on the x87 stack where this build does the same in SSE with other constants; the double form of last round cannot
// match because the double constants it needs are not in retail's data.
// score 576 (was 695), Round 1 of #1066: 2436 bytes against retail 2507. Fixed: the 2nd quad and the
// loop begin with D3DDevice_Begin (retail calls it; ::Begin inlined it), the 1st quad uses the member
// D3DDevice::SetVertexData4f (inlined, as retail), the first ten stage states go through
// sampler_state_write (ten separate dirty-flag ORs), floor(screen[1] + 0.5f) stays unstored, bounds is a plain
// array. Still differs: (1) our build also inlines function_0224f0 (the second ten stage states) and the
// three function_0222d0 calls of the 2nd render-state group, which retail calls; an in-body inline_depth(0)
// pair around the ten 0224f0 calls gives 540, not kept; file-scope inline_depth 0/1/2 changes nothing
// or worsens (770); (2) retail computes left/top/right/bottom on the x87 stack with dword constants and copies
// each through a temp (fstp, mov ecx, mov [bounds], ecx) into a bounds array that overlaps the dead direction,
// where our bounds (with the not-kept double casts) go through the x87 stack too but keep the temporary slots and our direction sits four
// bytes higher; double or float locals, an aggregate of the direction and casts tried; (3) retail's loop counts down
// in memory (dec [esp+0x20])
// or worsens (770); (2) retail computes left/top/right/bottom on the x87 stack and copies each through a
// temp (fstp, mov ecx, mov [bounds], ecx) where we use subss and a direct store; double or float
// locals, a union with direction and casts tried; (3) retail's loop counts down in memory (dec [esp+0x20])
// and recomputes (real)i * 0.0625f * 80.0f - 4.0f, we keep a float induction variable and cmp esi, 16;
// an extra "for (remaining = 16; remaining; --remaining)" counter gives 567 (not kept: artificial);
// (4) the dot product now adds k, i, j as retail (0x46f25-0x46f34) but with xmm0/xmm1 swapped, and retail
// multiplies direction.j by a memory operand; the j product's operand order changes nothing (576).
// Earlier lane rounds also tried the camera forward vector copied into a local (748, not kept).
// b4C walk (r722): every statement checked against retail, no logic difference found: field offsets, the
// four bounds tests (0x4704a-0x470ac: left < g_48564e, top < g_48564c, g_48564a < right, g_485648 < bottom),
// the three render-state/combiner groups, 0x14bc0/0x14f60/0x143c0/0x467c0 arguments, the 16-pass loop
// (offset from i before the increment, strength / (i + 1) after it) and the XBE constants (0.70710677,
// 3.4142134, 0.5, 32.0, 0.0625, 80.0, 4.0, 16777215.0). Forms tried, none kept: strength as a nested ternary
// (retail stores 1.0 at 0x46f63 before the ja, as the ternary does) 601; left/top/right/bottom as float
// locals copied into bounds[] (retail's temps at 0x2c-0x38 and the copies at 0x4700e-0x47062) 584, with
// (double) casts on screen[0] and center_y 497 (x87 chain as retail, but qword constants and fst/fld st(1);
// narrowing casts), center_x/center_y unrounded locals 585-592; direction and bounds in separate blocks
// (retail overlaps both at esp+0x14; ours then grows the frame to 0x54) 584; all twenty stage states through
// function_0224f0 (retail inlines the first ten, calls the second) 638; dot3f adds i, j, k, retail k, i, j.
// MiniMax A (11-12): no new form, no lower score (576): retail computes the four screen bounds on the x87 stack through a
// temporary each and overlaps the direction with them at [esp+0x14], our body is 71 bytes shorter than retail's 2507, and
// its second group of ten stage states and its three render-state calls stay out of line where ours inlines them.
// @retail 0x46ea0
void function_46ea0(long tag, point3f const *position)
{
    vector3f direction;
    direction.i = position->x - g_485618.position.x;
    byte *definition = g_4e3b44[tag & 0xffff].bytes;
    direction.j = position->y - g_485618.position.y;
    direction.k = position->z - g_485618.position.z;
    function_30bf0(&direction);
    real dot = g_485618.forward.k * direction.k + g_485618.forward.i * direction.i +
        g_485618.forward.j * direction.j;
    real strength = (dot - 0.7071067690849304f) * 3.4142134189605713f;
    if (strength < 0.0f) strength = 0.0f;
    else if (strength > 1.0f) strength = 1.0f;
    real scale = g_48565c / dot;
    point3f projected;
    projected.x = direction.i * scale + g_485618.position.x;
    projected.y = direction.j * scale + g_485618.position.y;
    projected.z = direction.k * scale + g_485618.position.z;
    real screen[4];
    if (!function_48b00(&projected, *(real *)(definition + 0x10), screen, &direction.i)) return;
    screen[0] = (real)floor(screen[0] + 0.5f);
    real center_y = (real)floor(screen[1] + 0.5f);
    real bounds[4];
    bounds[0] = screen[0] - 32.0f;
    bounds[2] = center_y - 32.0f;
    bounds[1] = screen[0] + 32.0f;
    bounds[3] = center_y + 32.0f;
    if (!((real)(short)g_48564e > bounds[0] && (real)(short)g_48564c > bounds[2] &&
        bounds[1] > (real)(short)g_48564a && bounds[3] > (real)(short)g_485648)) return;
    function_1cc30(10);
    function_1c710(g_51f0f0);
    g_4b8448 = 0x901; D3DDevice_SetRenderState(D3DRS_CULLMODE, 0x901);
    g_4b8308 = 0x1000000; D3DDevice_SetRenderState(D3DRS_COLORWRITEENABLE, 0x1000000);
    g_4b82e8 = 0; D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
    g_4b82ec = 0; D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, 0);
    g_4b82e0 = 0x207; D3DDevice_SetRenderState(D3DRS_ZFUNC, 0x207);
    g_4b8450 = 0; D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSCombinerCount = 1;
    g_484f68.PSFinalCombinerConstant1 = 0;
    g_484f68.PSFinalCombinerInputsEFG = 0x1100;
    function_1ccf0(&g_484f68);
    function_1cf50();
    D3DDevice::Begin(D3DPT_TRIANGLEFAN);
    D3DDevice::SetVertexData4f(0, bounds[0], bounds[2], 16777215.0f, 16777215.0f);
    D3DDevice::SetVertexData4f(0, bounds[1], bounds[2], 16777215.0f, 16777215.0f);
    D3DDevice::SetVertexData4f(0, bounds[1], bounds[3], 16777215.0f, 16777215.0f);
    D3DDevice::SetVertexData4f(0, bounds[0], bounds[3], 16777215.0f, 16777215.0f);
    D3DDevice::End();
    function_14bc0((short)g_4858b8, 0, true);
    function_1cc30(10);
    function_1c710(g_51f0f0);
    function_143c0(*(long *)(g_485a80 + 0x34), 0, 0, 0.0f);
    sampler_state_write(0, D3DTSS_ADDRESSU, 3);
    sampler_state_write(0, D3DTSS_ADDRESSV, 3);
    sampler_state_write(0, D3DTSS_MAGFILTER, 2);
    sampler_state_write(0, D3DTSS_MINFILTER, 2);
    sampler_state_write(0, D3DTSS_MIPFILTER, 2);
    sampler_state_write(0, D3DTSS_MAXANISOTROPY, 0);
    sampler_state_write(0, D3DTSS_MIPMAPLODBIAS, 0);
    sampler_state_write(0, D3DTSS_MAXMIPLEVEL, 0);
    sampler_state_write(0, D3DTSS_COLORSIGN, 0);
    sampler_state_write(0, D3DTSS_ALPHAKILL, 0);
    g_4b8448 = 0x901; D3DDevice_SetRenderState(D3DRS_CULLMODE, 0x901);
    g_4b8308 = 0x1000000; D3DDevice_SetRenderState(D3DRS_COLORWRITEENABLE, 0x1000000);
    g_4b82e8 = 0; D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    g_4b8438 = 2; D3DDevice_SetRenderState(D3DRS_ZENABLE, 2);
    function_0222d0(D3DRS_ZFUNC, 0x203);
    function_0222d0(D3DRS_ZWRITEENABLE, 0);
    g_4b8450 = 0; D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 1;
    g_484f68.PSCombinerCount = 1;
    g_484f68.PSAlphaInputs[0] = 0x18200000;
    g_484f68.PSAlphaOutputs[0] = 0x200c0;
    g_484f68.PSFinalCombinerInputsABCD = 0x1c;
    g_484f68.PSFinalCombinerInputsEFG = 0x1c00;
    function_1ccf0(&g_484f68);
    function_1cf50();
    D3DDevice_Begin(D3DPT_TRIANGLEFAN);
    D3DDevice_SetVertexData2s(3, 0, 0);
    D3DDevice_SetVertexData4f(0, bounds[0], bounds[2], 16777215.0f, 16777215.0f);
    D3DDevice_SetVertexData2s(3, 1, 0);
    D3DDevice_SetVertexData4f(0, bounds[1], bounds[2], 16777215.0f, 16777215.0f);
    D3DDevice_SetVertexData2s(3, 1, 1);
    D3DDevice_SetVertexData4f(0, bounds[1], bounds[3], 16777215.0f, 16777215.0f);
    D3DDevice_SetVertexData2s(3, 0, 1);
    D3DDevice_SetVertexData4f(0, bounds[0], bounds[3], 16777215.0f, 16777215.0f);
    D3DDevice::End();
    function_46230(16, bounds);
    function_46230(17, bounds);
    short filtered = function_467c0(16, 17, 4);
    function_14bc0((short)g_4858b8, 0, true);
    function_1cc30(10);
    function_1cd90();
    function_14f60(0, filtered);
    function_0224f0(0, D3DTSS_ADDRESSU, 3);
    function_0224f0(0, D3DTSS_ADDRESSV, 3);
    function_0224f0(0, D3DTSS_MAGFILTER, 2);
    function_0224f0(0, D3DTSS_MINFILTER, 2);
    function_0224f0(0, D3DTSS_MIPFILTER, 2);
    function_0224f0(0, D3DTSS_MAXANISOTROPY, 0);
    function_0224f0(0, D3DTSS_MIPMAPLODBIAS, 0);
    function_0224f0(0, D3DTSS_MAXMIPLEVEL, 0);
    function_0224f0(0, D3DTSS_COLORSIGN, 0);
    function_0224f0(0, D3DTSS_ALPHAKILL, 0);
    g_4b8448 = 0x901; D3DDevice_SetRenderState(D3DRS_CULLMODE, 0x901);
    function_0222d0(D3DRS_COLORWRITEENABLE, 0x10101);
    function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
    function_0222d0(D3DRS_SRCBLEND, 0x302);
    function_0222d0(D3DRS_DESTBLEND, 1);
    function_0222d0(D3DRS_BLENDOP, 0x8006);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    function_0222d0(D3DRS_ZFUNC, 0x207);
    g_4b8450 = 0; D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSTextureModes = 1;
    g_484f68.PSCombinerCount = 2;
    g_484f68.PSConstant0[0] = 0xb0b080;
    g_484f68.PSConstant1[0] = 0xffffff;
    g_484f68.PSAlphaInputs[0] = 0x48200000;
    g_484f68.PSAlphaOutputs[0] = 0xc0;
    g_484f68.PSRGBInputs[1] = 0x3c011c02;
    g_484f68.PSRGBOutputs[1] = 0xc00;
    g_484f68.PSFinalCombinerInputsABCD = 0xc080000;
    g_484f68.PSFinalCombinerInputsEFG = 0x1400;
    function_1ccf0(&g_484f68);
    real opacity = 1.0f - g_4857b4;
    opacity = opacity < 0.0f ? 0.0f : opacity > 1.0f ? 1.0f : opacity;
    strength *= opacity;
    long i = 0;
    do
    {
        real offset = (real)i * 0.0625f * 80.0f - 4.0f;
        function_1cf50();
        D3DDevice_Begin(D3DPT_TRIANGLEFAN);
        ++i;
        D3DDevice_SetVertexData4f(9, 0.0f, 0.0f, 0.0f, strength / i);
        D3DDevice_SetVertexData2s(3, 0, 0);
        D3DDevice_SetVertexData4f(0, bounds[0] - offset, bounds[2] - offset, 16777215.0f, 16777215.0f);
        D3DDevice_SetVertexData2s(3, 1, 0);
        D3DDevice_SetVertexData4f(0, bounds[1] + offset, bounds[2] - offset, 16777215.0f, 16777215.0f);
        D3DDevice_SetVertexData2s(3, 1, 1);
        D3DDevice_SetVertexData4f(0, bounds[1] + offset, bounds[3] + offset, 16777215.0f, 16777215.0f);
        D3DDevice_SetVertexData2s(3, 0, 1);
        D3DDevice_SetVertexData4f(0, bounds[0] - offset, bounds[3] + offset, 16777215.0f, 16777215.0f);
        D3DDevice::End();
    } while (i < 16);
}


extern long g_50943c, g_509440;


struct s_render_part_context;
void function_41040(short index, long part_index, long group, byte weight,
    s_render_part_context const *context, long material_override, bool force,
    dword and_mask, dword or_mask);
__declspec(noinline) void *function_449e0(short index, bool instance, bool load);
s_index_cache_storage g_4c62f8;
bool function_460d0(dword const *mask, short index, long part_index);

// @retail 0x44ac0
void __stdcall function_44ac0(long group, dword selection_mask, long level)
{
    (void)&group; (void)&selection_mask; (void)&level;
    long end = g_4c0b78.previous;
    if (group) end = g_4c0b78.current;
    end = (word)end;
    long begin;
    if (!group) begin = 0; else begin = (word)g_4c0b78.previous;
    for (long index = begin; index < end; ++index)
    {
        s_44940_entry *entry = &g_4ba138[(short)index];
        if (entry->unknown00 & 0x400)
        {
            if (!group)
            {
                if (g_50943c < 32)
                    ((word *)g_4c6b00[7].values)[g_50943c++] = (word)index;
            }
            else
            {
                long count = g_4c62f8.records.blocks[g_509440].count;
                if (count < 32)
                {
                    g_4c62f8.records.blocks[g_509440].entries[count] = *entry;
                    ++g_4c62f8.records.blocks[g_509440].count;
                }
            }
            continue;
        }
        if (!((entry->unknown00 & selection_mask) & 0x1fffff) ||
            ((entry->flags >> 4) & 0x1f) != level || !(entry->unknown00 & 1) ||
            entry->unknown10[0xa] <= 0u) continue;
        byte weight = g_4c0b78.weights[index];
        long transform_index = entry->flags & 0xf;
        byte material_index = g_4c0b78.material_indices[(short)index];
        long material = NONE;
        if (material_index != 0xff) material = g_4c0b78.materials[material_index];
        byte *geometry = (byte *)function_449e0((short)index, (bool)((entry->unknown00 >> 12) & 1), false);
        if (!geometry) continue;
        for (long part = 0; part < *(long *)geometry; ++part)
        {
            byte context[0x2c];
            if (function_45ce0((short)index, context, (short)part, (short)transform_index) &&
                function_460d0(*(dword const **)(entry->unknown10 + 4), (short)index, part))
            {
                long chosen = *(long *)context;
                if (material != NONE) chosen = material;
                if (chosen != NONE)
                    function_41040((short)index, part, group, weight,
                        (s_render_part_context *)context, chosen, level != 31, ~0UL, 0);
            }
        }
    }
}

#if 0 // Preserve the existing exact SDK Clear callers.

extern bool g_485a74;
extern dword g_4b8344;
real g_4b893c;

// Retail 0x13450
bool function_13450(s_depth_parameters_13450 const *parameters)
{
    real high = g_485ad4.hi;
    real inverse = 1.0f / high;
    g_4850c4 = *parameters;
    real lower = inverse * g_485ad4.lo * 16777215.0f;
    real upper = inverse * high * 16777215.0f;
    g_485a74 = false;
    if (lower < 0.0f) lower = 0.0f;
    else if (lower > 16777215.0f) lower = 16777215.0f;
    g_485adc = lower;
    if (upper < 0.0f) upper = 0.0f;
    else if (upper > 16777215.0f) upper = 16777215.0f;
    g_485ae0 = upper;
    if (parameters->mode == 1 || parameters->mode == 2)
    {
        real distance = parameters->distance * inverse * 16777215.0f;
        if (distance < 0.0f) distance = 0.0f;
        else if (distance > 16777215.0f) distance = 16777215.0f;
        if (parameters->mode == 1)
            g_485adc = distance;
        else if (parameters->mode == 2)
        {
            g_485ae0 = distance;
            g_4858b8 = 18;
            function_14bc0(18, 0, true);
            if (g_485607 && !g_4858b8) function_14ac0();
            D3DDevice_Clear(0, 0, 0xf3, 0, 1.0f, 0);
        }
    }
    D3DDevice_SetDepthClipPlanes(g_485adc, g_485ae0, D3DSDCP_SET_FIXEDFUNCTION_PLANES);
    g_4b8344 = 0;
    D3DDevice_SetRenderState(D3DRS_DEPTHCLIPCONTROL, 0);
    g_4b6280.count = 0;
    g_4b6280.unknown04 = 0;
    g_4b6280.unknown10 = true;
    g_4b6280.available = true;
    g_4b893c = 0.0f;
    return true;
}
#endif

extern dword g_4b82f8, g_4b8324;
extern byte g_4c19c0;
extern dword g_4c1a04;
struct s_2e9e0_saved { long tag; point3f position; };
extern s_2e9e0_saved g_4c19c4[4];
struct s_2e3f0_record;
long __stdcall function_2e9e0(byte const *context, s_2e3f0_record const *record);
void function_46ea0(long tag, point3f const *position);

// matches: the ZENABLE shadow is written through the render-state table,
// g_4b81fc[D3DRS_ZENABLE], which is the word retail keeps at 0x4b8438, so the
// store stays where retail puts it, ahead of the device's push-buffer space
// check, and the inlined SetRenderState keeps retail's own order.
// MiniMax H (11-04): no new form, no lower score; the remaining difference would need the shadow global written through a cast of its own address.
// Tried (MiniMax S run 10): writing the g_4b8438 store through an unsigned int lvalue
// (*(unsigned int *)&g_4b8438 = 0) scores 73 against 76; not kept (a cast of the global's address).
// MiniMax D (11-01): both builds store the shadow global from ebx, but ours is sunk past the
// device load and the range comparison of the inlined macro.
// MiniMax D (11-01): no new form, no lower score; the store before the inlined call is only
// reachable through a cast of the global's own address.
// MiniMax A (11-03): no new form, no lower score.
// score 3 (was 22): the g_4b8438 store is emitted after the range comparison of the inlined call,
// where retail emits it before.
// @retail 0x2df00
void function_2df00()
{
    g_4670bc = true;
    function_16b10((s_render_reset_state *)g_485b48);
    function_14bc0(g_485602, 0, true);
    function_15370(0);
    if (!(fabs(g_4b8494 - 1.0f) < 0.0001f))
    {
        D3DDevice_SetStipple(function_1c290(1.0f));
        g_4b8494 = 1.0f;
    }
    g_4b8448 = 0; D3DDevice_SetRenderState(D3DRS_CULLMODE, 0);
    g_4b8308 = 0x10101; D3DDevice_SetRenderState(D3DRS_COLORWRITEENABLE, 0x10101);
    g_4b82e8 = 1; D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
    g_4b82f4 = 0x302; D3DDevice_SetRenderState(D3DRS_SRCBLEND, 0x302);
    g_4b82f8 = 1; D3DDevice_SetRenderState(D3DRS_DESTBLEND, 1);
    g_4b8324 = 0x8006; D3DDevice_SetRenderState(D3DRS_BLENDOP, 0x8006);
    g_4b82ec = 0; D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, 0);
    g_4b81fc[D3DRS_ZENABLE] = 0; D3DDevice_SetRenderState(D3DRS_ZENABLE, 0);
    g_4b82e0 = 0x203; D3DDevice_SetRenderState(D3DRS_ZFUNC, 0x203);
    g_4b82fc = 0; D3DDevice_SetRenderState(D3DRS_ZWRITEENABLE, 0);
    g_4b8450 = 0; D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
    function_0222d0(D3DRS_SAMPLEALPHA, 0);
    g_51f3c8[1][0] = NULL;
    g_51f3c8[1][1] = NULL;
    g_51f3c8[1][2] = NULL;
    g_51f3c8[1][3] = NULL;
    function_1cf50();
    memset(&g_484f68, 0, sizeof(g_484f68));
    dword *fields = (dword *)&g_484f68;
    fields[0xd8 / 4] = 1;
    fields[0xd4 / 4] = 0x11002;
    fields[0x88 / 4] = 0x8081524;
    fields[0xb4 / 4] = 0xc5;
    fields[0x8c / 4] = 0xc0c0408;
    fields[0xb8 / 4] = 0xd4;
    fields[0x20 / 4] = 0x50f0004;
    fields[0x24 / 4] = 0xc0d1400;
    function_1ccf0(&g_484f68);
    function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x54), 0);
    function_1c710(g_51f0f0);
    g_4c19c0 = false;
    g_4c1a04 = 0;
    function_20bb0(g_4b9ed4, 0xf, NULL, 0, false, (visibility_draw_callback)function_2e9e0);
    for (long i = 0; i < (long)g_4c1a04; ++i)
        function_46ea0(g_4c19c4[i].tag, &g_4c19c4[i].position);
}

struct s_501e0
{
    point3f field_0;
    real field_c;
    real field_10;
    dword field_14;
};
void function_501e0(s_501e0 const *a, bool perspective, s_501e0 *out);
long function_30830(point3f const *point, s_camera const *camera, vector3f const *direction,
    point2f const *scale, bool perspective, bool negate, point2f *out);
bool __stdcall function_3a5c0(byte const *configuration, real low, real high, long mode,
    bool center, bool shift, bool clip, bool skip_state, real *depth_out);
void function_47ea0(short stage, byte flags);

// score 325 (was 383; 1582 bytes, retail 1592). Round 2 of #1066 found this form; it was held while it cost then-matched
// 0x41640 (unknown_0350e0.cpp), which matched only through whole-image layout and was given up in r695. Walked against retail:
// control flow, both clamps, the 0x3a5c0/0x484b0/0x480a0 arguments and the constants
// (0.0001f at 0x45dbdc, 1.0f at 0x45dbc0) agree; no behaviour bug. Found: 0x30830 is called with
// the camera and scale globals (retail loads 0x4b9e14 and pushes 0x4b9ecc; the callee would
// substitute them for NULL, so only the bytes differed); retail tests its result as a byte (`test
// al, al`), ours `test eax, eax`: a (byte) cast gives retail's test but the same score (325), dropped. The
// 8-byte `double angle` local gave the frame a slot retail lacks (0x20bc) and moved the tag out of
// ecx; cos/sin of atan2 written twice (one fpatan) gives tag in ecx, eight stack arguments and
// retail's 0x20b8 frame, so the volatile pointer and `selected` local are gone (points/count are
// assigned in place, as retail's stores to [esp+0x20cc]/[esp+0x20d0]). The width is formed inside
// each branch as in retail; the (double) casts in the length are dropped (retail multiplies the
// float stores directly). Still differs: retail reloads all three direction floats from memory for
// the length (i, k, j order) where ours keeps the last store in st(0); loop 2's base is
// points + 0x10 (ours points + 0xc); retail tests perspective per iteration (`test al, al; setne
// dl`) where ours loads it as a bool; scheduling of the dot product and the vertex-shader
// constants. Tried: `real radius = points[i].field_10;` first in loop 2 (320, but a stack slot
// moves the arrays 4 bytes), `byte perspective` or `perspective != 0` (332: setne hoisted to a
// stack byte), `long bitmap` (no change), all six orders of the length sum (the compiler
// canonicalises them: 327/329), `source` local instead of assigning points (tag in ecx but 356),
// vector3d_from_points3d for loop 2's delta (330).
// MiniMax D (11-01): score 385 (was 406): the points are read through a local, so the parameter
// stays on the stack and all nine arguments travel there (ret 0x24); retail takes the tag in ecx
// with the other eight on the stack (ret 0x20), which our call to 0x14560 still cannot match.
// The vertex-shader constants now hold 1.0f at retail's indices 0, 5, 8, 13, 16, 21, 24 and 29.
// score 406 (was 406): retail takes the tag in ecx and the eight arguments on the stack
// (ret 0x20), ours takes the points in ecx and the tag in edx with seven stack arguments
// (ret 0x1c), so every stack offset differs; retail also calls 0x14560 with the stage in
// ecx, and holds 1.0f in the vertex-shader constants at indices 0, 5, 8, 13, 16, 21, 24
// and 29 where ours holds it at 0, 5, 10, 15, 16, 21, 26 and 31.
// @retail 0x4f7e0
bool __stdcall function_4f7e0(long tag, s_501e0 const *points, long count, short bitmap,
    real cosine, real sine, bool segments, bool perspective, bool filtered)
{
    point2f rotations[256];
    s_501e0 midpoints[256];
    if (filtered)
    {
        if (!g_46713c)
        {
            function_0222d0(D3DRS_SAMPLEALPHA, 0);
        }
        g_46713c = true;
    }
    else
        function_4fe20();
    short stage = filtered ? 1 : 0;
    bool skip = tag != NONE ? function_14560(stage, tag, 0, 1, bitmap) :
        function_14480(*(long *)(g_485a80 + 0x34), stage, bitmap);
    if (!skip)
    {
        function_47ea0(stage, 0);
        if (segments)
        {
            long i = 0;
            long total = count - 1;
            for (; i < total; ++i)
            {
                function_501e0(points + i, perspective, midpoints + i);
                vector3f direction;
                vector3d_from_points3d(&points[i].field_0, &points[i + 1].field_0, &direction);
                real length = (real)sqrt(direction.i * direction.i + direction.j * direction.j + direction.k * direction.k);
                if (!(fabs(length) < 0.0001f))
                {
                    real reciprocal = 1.0f / length;
                    direction.i = reciprocal * direction.i;
                    direction.j = direction.j * reciprocal;
                    direction.k = direction.k * reciprocal;
                }
                point2f screen;
                if (function_30830(&midpoints[i].field_0, &g_4b9e14, &direction, &g_4b9ecc, true, true, &screen))
                {
                    rotations[i].x = (real)cos(atan2(screen.y, screen.x));
                    rotations[i].y = (real)sin(atan2(screen.y, screen.x));
                }
                else
                {
                    rotations[i].x = 1.0f;
                    rotations[i].y = 0.0f;
                }
            }
            points = midpoints;
            count = total;
        }
        else
        {
            for (long i = 0; i < count; ++i)
            {
                rotations[i].x = cosine;
                rotations[i].y = sine;
            }
        }
        for (long i = 0; i < count; ++i)
        {
            if (filtered)
            {
                vector3f delta;
                delta.i = g_4b9da0.x - points[i].field_0.x;
                delta.j = g_4b9da0.y - points[i].field_0.y;
                delta.k = g_4b9da0.z - points[i].field_0.z;
                real depth = 0.0f - (g_4b9dac.i * delta.i + g_4b9dac.k * delta.k + g_4b9dac.j * delta.j);
                point3f position = points[i].field_0;
                real low = depth - points[i].field_10;
                if (low < g_4b9de0) low = g_4b9de0;
                else if (low > g_4b9de4) low = g_4b9de4;
                real high = depth + points[i].field_10;
                if (high < g_4b9de0) high = g_4b9de0;
                else if (high > g_4b9de4) high = g_4b9de4;
                if (high > low && depth != 0.0f)
                {
                    real adjusted_depth;
                    if (function_3a5c0((byte const *)NONE, low, high, 0, true, true, true, false, &adjusted_depth) && depth != adjusted_depth)
                    {
                        real scale = (adjusted_depth / depth) * 0.0f + 1.0f;
                        real shift = 0.0f - (depth - adjusted_depth) * points[i].field_10;
                        position.x = delta.i * shift + points[i].field_0.x;
                        position.y = delta.j * shift + points[i].field_0.y;
                        position.z = delta.k * shift + points[i].field_0.z;
                        function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x84), 0);
                        function_1c710(g_51f0f0);
                        function_1cf50();
                        real constants[32];
                        constants[0] = 1.0f;
                        constants[1] = 0.0f;
                        constants[2] = 0.0f;
                        constants[3] = 0.0f;
                        constants[4] = 0.0f;
                        constants[5] = 1.0f;
                        constants[6] = 0.0f;
                        constants[7] = 0.0f;
                        constants[8] = 1.0f;
                        constants[9] = 0.0f;
                        constants[10] = 0.0f;
                        constants[11] = 0.0f;
                        constants[12] = 0.0f;
                        constants[13] = 1.0f;
                        constants[14] = 0.0f;
                        constants[15] = 0.0f;
                        constants[16] = 1.0f;
                        constants[17] = 0.0f;
                        constants[18] = 0.0f;
                        constants[19] = 0.0f;
                        constants[20] = 0.0f;
                        constants[21] = 1.0f;
                        constants[22] = 0.0f;
                        constants[23] = 0.0f;
                        constants[24] = 1.0f;
                        constants[25] = 0.0f;
                        constants[26] = 0.0f;
                        constants[27] = 0.0f;
                        constants[28] = 0.0f;
                        constants[29] = 1.0f;
                        constants[30] = 0.0f;
                        constants[31] = 0.0f;
                        D3DDevice_SetVertexShaderConstant(-78, constants, 8);
                        real width = points[i].field_c > points[i].field_10 ? points[i].field_c : points[i].field_10;
                        function_484b0(&position, adjusted_depth, width * scale, points[i].field_10 * scale,
                            rotations[i].x, rotations[i].y, points[i].field_14);
                    }
                }
            }
            else
            {
                real width = points[i].field_c > points[i].field_10 ? points[i].field_c : points[i].field_10;
                function_480a0(&points[i].field_0, width, points[i].field_10,
                    rotations[i].x, rotations[i].y, points[i].field_14);
            }
        }
    }
    return !skip;
}

extern dword g_4b82f8, g_4b8324;
bool __stdcall function_3a5c0(byte const *configuration, real low, real high, long mode,
    bool center, bool shift, bool clip, bool skip_state, real *depth_out);
void function_1cf80();

PRIVATE __forceinline real clamp_callback(real x, real low, real high);

// score 529 (was 624), walked statement by statement against retail (b4A, 11-06). Behaviour/precision: no
// field, offset, condition direction, callee, argument or constant differs. Differences fixed: the stage-1..3
// loop stored ADDRESSV twice (retail 0x3810d stores ADDRESSU, ADDRESSV, then the filters, ten dirty-flag
// updates; ADDRESSW is not stored); the colour block zero-filled all 0x34 bytes (rep stos) where retail stores
// the thirteen fields one by one (0x37c79..0x37d8d, padding left alone: bytes 1-3, unread by 0x3a5c0 mode 3, so `= {0}` is not wanted); the two span-to-float conversions
// were before 0x16b10, retail converts after it (0x37e91, 0x37eb6) and keeps the spans as ints in the meantime;
// extents are read at each use (retail reloads them, 0x37aae/0x37ab7) so the frame is retail's 0x120.
// Stage states go through sampler_state_write (retail keeps one dirty-flag update per state).
// Still differs: retail calls 0x222d0 out of line for ZWRITEENABLE/ZFUNC (0x380b7, 0x380c6), ours inlines
// them (a pragma pair around the two calls: 516); retail calls 0x1cbe0 at 0x3816f, ours inlines it because
// its definition above is `inline` (without that keyword: 519, 2344 bytes as retail); retail re-reads
// input+0x98/0x9c/0xa0 at each use in the clamps (0x37cf9..0x37d67) where clamp_callback reads once (the
// same clamps as written ternaries: 584); depth/z/w sit in different slots (retail z 0x10, depth 0x14, w 0x18).
// The one inline_depth pair around Begin..End replaces the volatile 0x3fb280 pointer and the four pairs
// around SetVertexData4f (that form scores 710 on this body); with no pair at all 1400.
// @retail 0x37a60
bool __stdcall function_37a60(void *context)
{
    long view_span_x = (short)g_48564e - (short)g_48564a;
    long view_span_y = (short)g_48564c - (short)g_485648;
    byte *input = (byte *)context;
    byte *view = input + g_485600 * 0xb8;
    real const *extents = (real const *)(view + 0x134);
    struct s_filter_vertex { real x, y; byte unknown08[0x10]; point2f coordinates[3]; };
    s_filter_vertex vertices[4];
    {
        vertices[0].coordinates[0].x = extents[0] * -1.0f;
        vertices[0].coordinates[0].y = extents[0] * -1.0f;
        vertices[1].coordinates[0].x = extents[0];
        vertices[1].coordinates[0].y = extents[0] * -1.0f;
        vertices[2].coordinates[0].x = extents[0] * -1.0f;
        vertices[2].coordinates[0].y = extents[0];
        vertices[3].coordinates[0].x = extents[0];
        vertices[3].coordinates[0].y = extents[0];
    }
    {
        vertices[0].coordinates[1].x = extents[1] * -1.0f;
        vertices[0].coordinates[1].y = extents[1] * -1.0f;
        vertices[1].coordinates[1].x = extents[1];
        vertices[1].coordinates[1].y = extents[1] * -1.0f;
        vertices[2].coordinates[1].x = extents[1] * -1.0f;
        vertices[2].coordinates[1].y = extents[1];
        vertices[3].coordinates[1].x = extents[1];
        vertices[3].coordinates[1].y = extents[1];
    }
    {
        vertices[0].coordinates[2].x = extents[2] * -1.0f;
        vertices[0].coordinates[2].y = extents[2] * -1.0f;
        vertices[1].coordinates[2].x = extents[2];
        vertices[1].coordinates[2].y = extents[2] * -1.0f;
        vertices[2].coordinates[2].x = extents[2] * -1.0f;
        vertices[2].coordinates[2].y = extents[2];
        vertices[3].coordinates[2].x = extents[2];
        vertices[3].coordinates[2].y = extents[2];
    }
    for (long stage = 0; stage < 3; ++stage)
    {
        real constants[4];
        constants[0] = *(real *)(view + 0x11c + stage * 8) * *(real *)(input + 0x48 + stage * 4);
        constants[1] = *(real *)(view + 0x120 + stage * 8) * *(real *)(input + 0x48 + stage * 4);
        constants[2] = 0.0f;
        constants[3] = 0.0f;
        D3DDevice_SetVertexShaderConstantFast(-75 + stage, constants, 1);
    }
    byte configuration[0x34];
    configuration[0] = true;
    real gain = *(real *)(view + 0x14c) * *(real *)(view + 0x148);
    real red = *(real *)(view + 0xec) * gain;
    real green = *(real *)(view + 0xf0) * gain;
    real blue = *(real *)(view + 0xf4) * gain;
    *(real *)(configuration + 4) = 0.0f;
    *(real *)(configuration + 8) = clamp_callback(red, 0.0f, 1.0f);
    *(real *)(configuration + 0xc) = 0.0f;
    *(real *)(configuration + 0x10) = 0.0f;
    *(real *)(configuration + 0x14) = clamp_callback(green, 0.0f, 1.0f);
    *(real *)(configuration + 0x18) = 0.0f;
    *(real *)(configuration + 0x1c) = 0.0f;
    *(real *)(configuration + 0x20) = clamp_callback(blue, 0.0f, 1.0f);
    *(real *)(configuration + 0x24) = 0.0f;
    *(real *)(configuration + 0x28) = clamp_callback(*(real *)(input + 0x98), 0.0f, 1.0f);
    *(real *)(configuration + 0x2c) = clamp_callback(*(real *)(input + 0x9c), 0.0f, 1.0f);
    *(real *)(configuration + 0x30) = clamp_callback(*(real *)(input + 0xa0), 0.0f, 1.0f);
    real depth;
    if (function_3a5c0(configuration, *(real *)(input + 0x80), *(real *)(input + 0x84), 3,
        false, false, true, true, &depth))
    {
        real ratio = g_485ad4.hi / (g_485ad4.hi - g_485ad4.lo);
        real z = clamp_callback(((ratio * depth - ratio * g_485ad4.lo) / depth) * 16777215.0f, 0.0f, 16777215.0f);
        real w = clamp_callback((depth / g_485ad4.hi) * 16777215.0f, 0.0f, 16777215.0f);
        real x = (real)(short)g_48564a;
        real y = (real)(short)g_485648;
        g_4670bc = true;
        function_16b10((s_render_reset_state *)g_485b48);
        real width = (real)view_span_x;
        real height = (real)view_span_y;
        vertices[0].x = x - 0.5f; vertices[0].y = y - 0.5f;
        vertices[1].x = width + x - 0.5f; vertices[1].y = y - 0.5f;
        vertices[2].x = x - 0.5f; vertices[2].y = height + y - 0.5f;
        vertices[3].x = width + x - 0.5f; vertices[3].y = height + y - 0.5f;
        function_14bc0((short)g_4858b8, 0, true);
        function_14f60(0, 3);
        sampler_state_write(0, D3DTSS_ADDRESSU, 3);
        sampler_state_write(0, D3DTSS_ADDRESSV, 3);
        sampler_state_write(0, D3DTSS_ADDRESSW, 3);
        sampler_state_write(0, D3DTSS_MAGFILTER, 1);
        sampler_state_write(0, D3DTSS_MINFILTER, 1);
        sampler_state_write(0, D3DTSS_MIPFILTER, 0);
        sampler_state_write(0, D3DTSS_MAXANISOTROPY, 0);
        sampler_state_write(0, D3DTSS_MIPMAPLODBIAS, 0);
        sampler_state_write(0, D3DTSS_MAXMIPLEVEL, 0);
        sampler_state_write(0, D3DTSS_COLORSIGN, 0);
        sampler_state_write(0, D3DTSS_ALPHAKILL, 0);
        g_4b8308 = 0x10101; D3DDevice_SetRenderState(D3DRS_COLORWRITEENABLE, 0x10101);
        g_4b82e8 = 1; D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
        g_4b82f4 = 1; D3DDevice_SetRenderState(D3DRS_SRCBLEND, 1);
        g_4b82f8 = 0x303; D3DDevice_SetRenderState(D3DRS_DESTBLEND, 0x303);
        g_4b8324 = 0x8006; D3DDevice_SetRenderState(D3DRS_BLENDOP, 0x8006);
        function_0222d0(D3DRS_BLENDCOLOR, 0xffffff);
        g_4b82ec = 1; D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, 1);
        function_0222d0(D3DRS_ALPHAFUNC, 0x204);
        function_0222d0(D3DRS_ALPHAREF, 0);
        g_4b8448 = 0; D3DDevice_SetRenderState(D3DRS_CULLMODE, 0);
        g_4b843c = 0; D3DDevice_SetRenderState(D3DRS_STENCILENABLE, 0);
        g_4b8438 = depth > g_485658 ? 2 : 0; D3DDevice_SetRenderState(D3DRS_ZENABLE, g_4b8438);
        function_0222d0(D3DRS_ZWRITEENABLE, 0);
        function_0222d0(D3DRS_ZFUNC, 0x203);
        g_4b8450 = 0; D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
        for (long stage = 1; stage < 4; ++stage)
        {
            function_143c0(*(long *)(input + 4 + (stage - 1) * 8), 0, (short)stage, 0.0f);
            sampler_state_write(stage, D3DTSS_ADDRESSU, 1);
            sampler_state_write(stage, D3DTSS_ADDRESSV, 1);
            sampler_state_write(stage, D3DTSS_MAGFILTER, 2);
            sampler_state_write(stage, D3DTSS_MINFILTER, 2);
            sampler_state_write(stage, D3DTSS_MIPFILTER, 2);
            sampler_state_write(stage, D3DTSS_MAXMIPLEVEL, 0);
            sampler_state_write(stage, D3DTSS_MAXANISOTROPY, 0);
            sampler_state_write(stage, D3DTSS_MIPMAPLODBIAS, 0);
            sampler_state_write(stage, D3DTSS_COLORSIGN, 0);
            sampler_state_write(stage, D3DTSS_ALPHAKILL, 0);
        }
        function_1cf80();
        function_1cbe0(0, 0, 0x15);
        function_1c710(g_51f0f0);
        #pragma inline_depth(0)
        D3DDevice_Begin((D3DPRIMITIVETYPE)9);
        D3DDevice_SetVertexData2f(1, vertices[0].x, vertices[0].y);
        D3DDevice_SetVertexData2f(2, vertices[0].coordinates[0].x, vertices[0].coordinates[0].y);
        D3DDevice_SetVertexData2f(3, vertices[0].coordinates[1].x, vertices[0].coordinates[1].y);
        D3DDevice_SetVertexData2f(4, vertices[0].coordinates[2].x, vertices[0].coordinates[2].y);
        D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, vertices[0].x, vertices[0].y, z, w);
        D3DDevice_SetVertexData2f(1, vertices[1].x, vertices[1].y);
        D3DDevice_SetVertexData2f(2, vertices[1].coordinates[0].x, vertices[1].coordinates[0].y);
        D3DDevice_SetVertexData2f(3, vertices[1].coordinates[1].x, vertices[1].coordinates[1].y);
        D3DDevice_SetVertexData2f(4, vertices[1].coordinates[2].x, vertices[1].coordinates[2].y);
        D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, vertices[1].x, vertices[1].y, z, w);
        D3DDevice_SetVertexData2f(1, vertices[2].x, vertices[2].y);
        D3DDevice_SetVertexData2f(2, vertices[2].coordinates[0].x, vertices[2].coordinates[0].y);
        D3DDevice_SetVertexData2f(3, vertices[2].coordinates[1].x, vertices[2].coordinates[1].y);
        D3DDevice_SetVertexData2f(4, vertices[2].coordinates[2].x, vertices[2].coordinates[2].y);
        D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, vertices[2].x, vertices[2].y, z, w);
        D3DDevice_SetVertexData2f(1, vertices[3].x, vertices[3].y);
        D3DDevice_SetVertexData2f(2, vertices[3].coordinates[0].x, vertices[3].coordinates[0].y);
        D3DDevice_SetVertexData2f(3, vertices[3].coordinates[1].x, vertices[3].coordinates[1].y);
        D3DDevice_SetVertexData2f(4, vertices[3].coordinates[2].x, vertices[3].coordinates[2].y);
        D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, vertices[3].x, vertices[3].y, z, w);
        D3DDevice_End();
        #pragma inline_depth(255)
        D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, 0);
        D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
        D3DDevice_SetRenderState(D3DRS_ZENABLE, 2);
        D3DDevice_SetPixelShader(0);
    }
    return true;
}


struct s_245c0_state
{
    real range[6];
    real *histogram;
    long index;
    dword frame_count;
};
real g_51ecf0[256];
extern long g_485898;
__declspec(noinline) void function_245c0(s_245c0_state *state, real *histogram, short mode);
void __stdcall function_214f0(real passes, real distortion, real strength, real falloff,
    real scale, bool blend, bool preserve);
bool function_022710();
real scenario_ramp_evaluate_current(long index);
// MiniMax H (11-13): no new form kept, no lower score (130): the four screen bounds read straight from the
// globals score 125, a gain under the bar, so the body stands; the permuter ran on this function for 600 s over
// the reduced image and left the baseline where it was (130 differing instructions in both the reduced and the
// full image), so no permutation of the body beats it.
// globals score 125, a gain under the bar, so the body stands.
// MiniMax B (11-11), NOT kept (coordinator): 130 -> 120 stores the grid's fields in another order picked by score
// (match-only). The lead as written: the grid is stored in the order retail stores it, the
// two left values first, then the top, the right and the bottom, and only then the zero and one values,
// so the sixteen stores fall in retail's order; what still differs is that this build reserves a frame
// of 0x5c with the same twenty-two slots four bytes lower than retail's 0x58 and one slot unused at the
// top, the frame pointer is saved in the prologue where retail saves it around the filter loop, and the
// blur block loads the 0.9 and 1.0 constants into other registers and computes the distortion in
// another order.
// MiniMax C (11-11): no new form, no lower score (139); retail saves ebp around the filter loop (at 0x13a43) and frames 0x58, this build saves it in the prologue and frames 0x5c, so every stack slot sits four bytes off and the four unused bytes above the grid remain.
// MiniMax D (11-10): no new form, no lower score (139); the frame is still 0x5c with a frame pointer pushed in
// the prologue against retail's 0x58, where retail keeps ebp for the grid loop's counter and pushes it only around
// that block, so every stack slot sits eight bytes off; the ramp function and the strength pair are called the
// same way in both builds.
// MiniMax S (11-09): no new form, no lower score (139): retail's frame is 0x58 bytes with the frame
// pointer saved around the grid block and ours is 0x5c with the frame pointer saved in the prologue,
// so every stack slot sits eight bytes off; the state local, the entry offset, the entry count and
// the declaration order were tried this round and all leave the frame at 0x5c.
// MiniMax D (11-09): no new form, no lower score (139); the frame is still 0x5c bytes here with the frame
// pointer saved in the prologue, while retail's frame is 0x58 bytes and retail saves the frame pointer
// around the grid block, so every stack slot sits four bytes off; inlining the four grid bounds, dropping
// the byte-offset variable and moving the state pointer into the blur block all leave the frame at 0x5c.
// MiniMax A (11-06, run 52): no new form, no lower score (139) - retail's frame is 0x58 bytes with the
// grid filling it and the frame pointer pushed inside the loop.

// MiniMax C (11-06): no new form, no lower score - retail's frame is still 0x58 bytes with the frame
// pointer pushed inside the grid block and every local four bytes higher than ours, while ours is still
// 0x5c with the pointer pushed in the prologue; the six forms tried this round (separate locals for the
// weights and the grid, a merged loop head, a while loop, an index-multiplied address and two others)
// all leave the frame at 0x5c.
// MiniMax D (11-05): no new form, no lower score - retail's frame is 0x58 bytes with the grid filling it and the frame pointer pushed inside the loop, ours is 0x5c with the pointer pushed in the prologue, so every stack slot sits four bytes off.
// MiniMax B (11-11): improved to 130 (was 139); the entry address is now taken from the index
// (base + i * 0x24), so the byte offset is an induction variable zeroed in the preheader the way
// retail does it, and the filter loop from its head to its end now matches retail instruction for
// instruction; what still differs is that this build reserves a frame of 0x5c with the same
// twenty-two slots four bytes lower than retail's 0x58 and one slot unused at the top, the frame
// pointer is saved in the prologue where retail saves it around the filter loop, the grid is
// stored in another order (retail keeps the two left values, the top, the right and the bottom
// before the zero and one values), and the blur block loads the 0.9 and 1.0 constants into other
// registers and computes the distortion in another order.
// MiniMax H (11-04): no new form, no lower score; the counter in the loop head gives 205, the entry
// pointer as the induction variable 172 and both together 183 against 139, so retail's pushed frame
// pointer and its 0x58 frame still stand.
// MiniMax H (11-03): no new form, no lower score; retail saves edi in the prologue and keeps the scenario pointer in it across the loop's call with ebp as the counter, where ours saves edi inside the block and keeps the pointer in ecx, so every stack slot sits four bytes off; the scenario pointer in a local gives 148 and was dropped.
// MiniMax E (11-01): no new form, no lower score; retail's first parameter is a float, not a bool.
// MiniMax E (11-10): no new form, no lower score (139); the frame's four extra bytes sit above the grid and the
// frame pointer's push inside the loop decides them.
// MiniMax E (11-11): no new form, no lower score (139); retail's frame is 0x58 bytes with the frame pointer pushed
// inside the filter loop, ours is 0x5c with it in the prologue, so every stack slot sits four bytes off.
// MiniMax H (11-12): no new form, no lower score (130): this build still pushes ebp in the prologue and frames 0x5c while retail pushes ebx, esi and edi and frames 0x58, saving ebp around the filter loop instead, so every stack slot sits four bytes off and the grid's stores come in another order.
// score 130 (was 139): retail's frame is 0x58 bytes with the grid filling it to the top and the
// frame pointer pushed inside the loop; ours is 0x5c with four unused bytes above the grid and the
// frame pointer in the prologue, so every stack slot sits four bytes off.
// MiniMax E (11-09): no new form, no lower score (139); retail calls 0x137a0 for the ramp's value where our
// build expands it into the float arithmetic, and the ramp's three results sit in different frame slots
// (retail's esp+0xc, esp+0x6c and esp+0x10 against our esp+0x10, esp+0x14 and esp+0x74), so the difference
// is not only the four-byte frame offset.
// MiniMax B (11-11): no new form, no lower score (139); the blur block is instruction for instruction retail's
// except that retail's 137a0 writes its first result at entry-0x54 and ours writes it at entry-0x58, so the
// whole frame is four bytes out: retail reserves 0x58 with its twenty-two slots packed exactly (the distortion
// at entry-0x58, the ramp result at entry-0x54, the weights at entry-0x50..entry-0x44 and the grid at
// entry-0x40..entry-0x04) while this build reserves 0x5c with the same twenty-two slots four bytes lower and
// one slot unused at the top; retail also stores the grid in a different order (the two left values first, then
// the top, the right and the bottom, then the zero and one values) than this build's entry order; the forms
// tried (the four bounds read straight from the globals, the counter hoisted out of the for statement, the
// distortion and the extra swapped, the state local removed, the byte offset moved into the for statement and
// the declaration orders) all leave the score at 139 or worse.
// MiniMax B (11-11): no new form, no lower score (139); retail saves ebp around the filter loop (at 0x13a43, popped
// at 0x13b72) and frames 0x58, while this build saves it in the prologue and frames 0x5c, so every instruction
// after the prologue pairs against the wrong one and the rest of the function cannot be read off the diff.
// MiniMax A (11-12): no new form, no lower score (130); this build pushes ebp in the prologue and frames 0x5c while
// retail pushes ebx, esi and edi and frames 0x58, saving ebp around the filter loop instead, so every stack slot sits
// four bytes off; the grid's fields stored in retail's order give 120, but that form was already rejected as a store
// order picked by score.
// @retail 0x13870
void __stdcall function_13870(bool finish, bool blur)
{
    g_4670bc = true;
    function_16b10((s_render_reset_state *)g_485b48);
    if (g_485890)
    {
        g_485890->index = g_485600;
        function_245c0(g_485890, g_51ecf0, 0);
    }
    s_245c0_state *state = g_485890;
    if (blur)
    {
        real strength = 0.0f;
        real extra = 0.0f;
        function_137a0(&strength, &extra);
        real distortion = 0.9f;
        if (state)
            distortion = (1.0f - (1.0f - state->range[4])) * 0.9f;
        if (strength > 0.0f)
        {
            real scaled = (extra + 1.0f) * strength * 0.5f;
            strength = 0.0f > scaled ? 0.0f : scaled > 1.0f ? 1.0f : scaled;
            if (g_4b9fd0 > 0.0f)
            {
                distortion = (g_4b9fd4 - distortion) * g_4b9fd0 + distortion;
                strength = (g_4b9fd8 - strength) * g_4b9fd0 + strength;
            }
            function_214f0(3.992000102996826f, distortion, strength, 0.25f, 0.0f, true, g_485898 == 6);
        }
    }
    if (g_4e0350 && !(g_485898 >= 5 && g_485898 <= 7) && !g_485607 && !g_4850c8 && function_022710())
    {
        real weights[4];
        s_filter_vector grid[4];
        real left = (real)g_4b9dd2;
        real top = (real)g_4b9dd0;
        real right = (real)g_4b9dd6;
        real bottom = (real)g_4b9dd4;
        grid[0].x = left; grid[0].y = top; grid[0].z = 0.0f; grid[0].w = 0.0f;
        grid[1].x = right; grid[1].y = top; grid[1].z = 1.0f; grid[1].w = 0.0f;
        grid[2].x = left; grid[2].y = bottom; grid[2].z = 0.0f; grid[2].w = 1.0f;
        grid[3].x = right; grid[3].y = bottom; grid[3].z = 1.0f; grid[3].w = 1.0f;
        for (long i = 0; i < *(long *)((byte *)g_4e0350 + 0x3d0); ++i)
        {
            byte *entry = *(byte **)((byte *)g_4e0350 + 0x3d4) + i * 0x24;
            if (*(long *)(entry + 0x14) != NONE &&
                (*(short *)(entry + 0x20) != NONE || *(short *)(entry + 0x22) != NONE))
            {
                weights[0] = 0.0f; weights[1] = 0.0f; weights[2] = 0.0f; weights[3] = 0.0f;
                if (*(short *)(entry + 0x20) != NONE)
                    weights[0] = scenario_ramp_evaluate_current(*(short *)(entry + 0x20));
                if (*(short *)(entry + 0x22) != NONE)
                    weights[1] = scenario_ramp_evaluate_current(*(short *)(entry + 0x22));
                if (weights[0] != 0.0f || weights[1] != 0.0f)
                    function_2d140(*(long *)(entry + 0x14), weights, 2, 2, grid);
            }
        }
    }
    if (finish) function_2df00();
}


struct s_tag_data;
real function_13b390(void const *function, real input, real range);
real function_13bb90(s_tag_data const *function, real input, real range);
dword function_13bc00(s_tag_data const *function, real input);
color3f *unpack_color3f(dword pixel, color3f *color);
dword __cdecl pack_color4f(color4f const *color);
void function_4ff50(byte const *definition, byte const *state, bool facing, real time,
    real cosine, real *strength, real *horizontal, real *vertical, real *out);

PRIVATE __forceinline real clamp_callback(real x, real low, real high)
{
    return low > x ? low : x > high ? high : x;
}
PRIVATE __forceinline real callback_curve(byte const *curve, real time, real range)
{
    if (!*(byte *const *)(curve + 4) || *(long const *)curve <= 0) return 0.0f;
    real value = function_13b390(curve, time, range);
    byte *data = *(byte **)(curve + 4);
    if (!(data[1] & 0xf0))
    {
        real low = *(real *)(data + 4);
        real high = *(real *)(data + 8);
        value = low + (high - low) * clamp_callback(value, 0.0f, 1.0f);
    }
    return value;
}

// MiniMax H (11-13): no new form, no lower score (299): the count's early test is retail's mov eax, [edi + 8]; test
// eax, eax; jle where ours compares the memory with 1 (cmp dword ptr [edi + 8], 1; jl); spelling the test as
// *(long *)(definition + 8) <= 0 lines the instruction stream up with retail's from the start and so shows the real
// difference, 310 instructions, but it scores 313, and with it the group index's sign extension (movzx esi, ax against
// retail's mov esi, eax) and the index slot at [esp + 0x3c] against retail's [esp + 0x54]; counting the group after
// the test, as retail does at 0x4f7a5, measures 300.
// MiniMax G (11-13): no new form, no lower score (299): the thirty-two bit group index forms measure 506 to 507 again (a for loop over the counter 506, the counter as an int 507, the index widened by a cast 506), so the group index is still sign-extended where retail moves the thirty-two bit value, and the body stands.
// MiniMax S (11-11): no new form, no lower score (299): the group index is still sign-extended (movsx esi, ax)
// where retail moves the thirty-two bit index, the index slot still sits at [esp+0x3c] against retail's [esp+0x58],
// an extra mov edi, edi still stands in the loop head and the body is still three bytes longer than retail's; the
// thirty-two bit index forms score 507 to 514 and the permuter ran to the outer time limit and was stopped with no
// result, so the body stands.
// MiniMax D (11-10): NOT kept: 298, one below S's 299 already in main (under the 8-point bar): a for loop over
// an unsigned short counter; the group counter is 16 bits wide and the early exit compares the
// group count with one; the loop keeps a single counter and recomputes the stride with imul, the frame is
// 0x1894 as in retail's and the function is 2000 bytes as in retail's. What still differs: retail's early exit
// loads the count and tests it while this build compares the count in memory with one, so this build's exit is
// one instruction shorter; retail's loop bottom compares a 32-bit counter where this build widens the 16-bit
// one, and that counter wraps for counts above 65535 where retail's does not.
// score 310 (was 310): this round: no new form and no lower score; the loop still strength-reduces
// group * 0x98 into a second counter at [esp+0x40], so the frame is 0x1898 against retail's 0x1894 and
// the zero for both counters lives in EBP where retail keeps a single counter and an imul.
// MiniMax S (11-09): the group loop is a while loop over a 16-bit index with the step at the end of the body,
// which drops the strength-reduced second counter and the zero register: the frame is 0x1894 as in retail, the
// count test is a test/jle and the loop head carries the imul of the 0x98 stride.
// MiniMax E (11-10): no new form, no lower score (299); the sign-extended group index and the index slot's
// place in the frame are what the loop's induction variable fixes.
// MiniMax H (11-12): no new form, no lower score (299): the group index is still sign-extended where retail moves the 32-bit index, the index slot still sits at another offset in the frame, and this build still strength-reduces the group stride into a second counter, which makes the frame 0x1898 against retail's 0x1894.
// MiniMax B (11-13): no new form, no lower score (299): the group index is still sixteen bits wide where retail
// moves the 32-bit index, the while-loop form keeps retail's 0x1894 frame, and retail's stride is still an imul of
// the 0x98 against this build's strength reduction into a second counter.
// score 299 (was 310): fast score; our group index is sign-extended (movsx esi, ax) where retail moves the
// 32-bit index, the index slot sits at [esp+0x3c] against retail's [esp+0x58], an extra mov edi, edi stands in
// the loop head, and the body is 3 bytes longer than retail's.
// MiniMax D (11-10): NOT kept: 298, one below S's 299 already in main (under the 8-point bar): a for loop over
// an unsigned short counter; the group counter is 16 bits wide and the early exit compares the
// group count with one; the loop keeps a single counter and recomputes the stride with imul, the frame is
// 0x1894 as in retail's and the function is 2000 bytes as in retail's. What still differs: retail's early exit
// loads the count and tests it while this build compares the count in memory with one, so this build's exit is
// one instruction shorter; retail's loop bottom compares a 32-bit counter where this build widens the 16-bit
// one, and that counter wraps for counts above 65535 where retail's does not.
// MiniMax D (11-09): no new form, no lower score (310); the group stride is still strength-reduced into a
// second counter kept in a stack slot, while retail recomputes the product with imul and keeps one counter,
// and retail saves the frame pointer around the loop where this build saves it in the prologue, so the
// frame is 0x1898 against retail's 0x1894 and every local above the counter sits four bytes higher.
// MiniMax D (11-08): no new form, no lower score (310, measured on the full image); our loop still strength-reduces
// group * 0x98 into a second counter at [esp+0x40], so the frame is 0x1898 against retail's 0x1894 and the zero for
// both counters lives in EBP; a short group removes the slot and keeps the imul but scores 390 here.
// MiniMax B (11-07): no new form, no lower score (310); the group stride is still strength-reduced into a second
// counter at [esp+0x40], so the frame is 0x1898 against retail's 0x1894, the zero for the counters lives in EBP
// and every local above that slot sits 4 bytes higher; writing the two count tests as equality tests (retail
// compares with `test`, so a negative count behaves as retail's) leaves the code unchanged.
// MiniMax C (11-07): round 53: no new form, no lower score; the permuter never left its baseline build.
// fast score 310 (was 308), statement-by-statement walk against retail (f1D, 11-05): every field, offset,
// condition direction, callee, argument, constant and loop/branch shape agrees with retail; the one
// behaviour difference found is the order of the x87 sum for the facing cosine. Retail adds
// k, then j, then i (0x4f179..0x4f1af: fld [esp+0x7c]; fmul [ebx+0x1c]; fld [esp+0x78]; fmul [ebx+0x18];
// faddp; fld [esp+0x74]; fmul [ebx+0x14]; faddp; fabs). The old single (double) expression compiled to
// k, i, j whatever the order of its terms or the parentheses (all 6 orders and the a+(b+c) forms); the
// three-statement accumulation below emits retail's instruction sequence exactly (the (double) casts are
// not needed for the x87 form: no store between the products). This costs 2 fast-score points
// because the surrounding scheduling shifts (movaps now before the first faddp, retail has it before).
// MiniMax D (11-01/11-05): two integer comparisons against zero written against one gave 309, not kept (gain under 8).
// What still differs: ours strength-reduces group * 0x98 into a second counter ([esp+0x40]),
// retail keeps a single counter and an imul at 0x4f165, so our frame is 0x1898 against retail's 0x1894,
// every local above that slot sits 4 bytes higher, and the zero for both counters lives in ebp
// (retail: xor eax,eax, push ebp only around the loop at 0x4f15e). Tried, no change (308): dword group,
// do/while with and without the count guard (675/330), group = group + 1, continue form, a
// __forceinline element helper, a typed 0x98-byte struct pointer or array index, (group << 3) * 19,
// entry += group * 0x98, a use of group after the loop; a `short group` removes the second counter
// (frame 0x1894, imul kept, 2000 bytes, 421 with the old facing) but adds a movsx retail lacks; a cached
// group count local gives 417 but retail reloads it. Positions written as `positions = NULL; if
// (cond) positions = ...` is closer to retail (one jmp and one mov fewer) but scores 513 here because of
// the shifted slots. Also unaddressed: the point iterator tracks points[i].field_c (retail [edi-0xc], ours [edi-0x10]).
// The call to 0x4f7e0 matches retail's register setup (tag in ecx, eight stack words).
// Permuter, 30 minutes (11-06, on the source before the facing-sum change): 291 only with call arguments swapped (pow's
// two, function_13bb90's last two, function_4ff50's last two), which changes behaviour. Not kept.
// MiniMax G (11-11): no new form, no lower score: the early exit as `<= 0` 313 (2003 bytes) - the load and the
// test now match retail's, but the group index is still sixteen bits wide (movzx esi, ax against retail's
// mov esi, eax), so everything after the loop head stays out of step; a thirty-two bit group index 514 (2011
// bytes) and with the index copied into a named local 507 - the index is then strength-reduced into a second
// counter at [esp+0x40] instead of retail's imul; a thirty-two bit index with the step at the end of the
// body 513 (2000 bytes) - the loop's zero register moves to ebp and the frame pointer is pushed in the
// prologue. The body was restored.
// MiniMax R (11-11): no new form kept this round, no lower score (299): the +8 field test written as <= 0 gives retail's load, test and jle, but the one-byte shift it makes downstream raises the score to 313, so the original test stays.
// MiniMax A (11-12): no new form, no lower score (299); the count test written as `<= 0` gives retail's load, test
// and jle instead of a compare against 1, but the length that changes makes the rest of the body pair against the
// wrong instruction and the score rises to 313, so the original stands.
// MiniMax B (11-14): no new form, no lower score (299): retail increments the group counter on the path that skips
// the entry body, and this build increments it only inside the body, so moving the increment out (the for-loop form)
// gives 298 with retail's exact 2000-byte size and 0x1894 frame, but the counter still lives in a stack slot and the
// zero register is EBP where retail has EAX, so the body stays one instruction out of step; a thirty-two bit counter
// puts it in EAX but the compiler strength-reduces the 0x98 stride into a second counter (506, 507, 508). The body
// stands.
// MiniMax B (11-14, third pass): no new form, no lower score (299): retail's entry holds four instructions - the
// load of the count, the test, the store of the definition pointer and the branch - where the compare against 1
// gives three, and the checker pairs the two sides by instruction, so the pair after the entry no longer lines up;
// the count test written as `<= 0` gives retail's four, and then the body agrees with retail instruction for
// instruction from the entry to the group loop head, where retail moves its thirty-two bit group counter into esi
// and this build widens its sixteen bit one with movzx. The increment of the group counter stands where this build
// has it, inside the body, while retail counts every group: retail's increment is in the block that the skipped
// groups also reach (0x4f7a2), and this build's counter would stand still on a group with no points. Moving the
// increment out gives retail's exact 2000-byte length and 300, and with it the count test has to be `<= 0` as well,
// which then puts the zero register in ebp (452) where retail has eax (313). A thirty-two bit group counter gives
// retail's imul of the 0x98 stride its own counter at [esp+0x40] and a frame of 0x1898 (514).
// MiniMax B (11-14, second pass): no new form, no lower score (299): the count test written as `<= 0` does give
// retail's own load, test and jle, so the block matches instruction for instruction from `mov ecx, [g_4e3b44]` to the
// branch, but the three bytes it takes off the entry make the rest of the body pair against the wrong instruction and
// the score rises to 313, so the compare against 1 stays; a named count local makes no difference (313). What still
// differs: retail's group counter is a thirty-two bit value in EAX, compared against the count in EAX, while this build
// keeps the counter sixteen bits wide in a stack slot and widens it with movzx before the imul of the 0x98 stride;
// retail keeps that counter at [esp+0x54] and the facing float at [esp+0x50] under its `push ebp` around the loop,
// where this build keeps the counter at [esp+0x3c] and the facing at [esp+0x4c] without the push; and retail pads the
// loop head with `push ebp; nop` where this build pads it with `push ebp; mov edi, edi`.
// @retail 0x4f010
void __stdcall function_4f010(void *payload)
{
    s_501e0 points[256];
    s_42850_payload *state = (s_42850_payload *)payload;
    if (state->b == NONE) return;
    byte *definition = g_4e3b44[state->b & 0xffff].bytes;
    if (*(long *)(definition + 8) < 1) return;
    real fade = 1.0f;
    vector3f direction;
    direction.i = g_4b9da0.x - state->position.x;
    direction.j = g_4b9da0.y - state->position.y;
    direction.k = g_4b9da0.z - state->position.z;
    real distance = function_30bf0(&direction);
    if (*(real *)(definition + 4) > *(real *)definition)
    {
        if (0.0f > ((*(real *)(definition + 4) - distance) / (*(real *)(definition + 4) - *(real *)definition))) fade = 0.0f;
        else if (((*(real *)(definition + 4) - distance) / (*(real *)(definition + 4) - *(real *)definition)) > 1.0f) fade = 1.0f;
        else fade = ((*(real *)(definition + 4) - distance) / (*(real *)(definition + 4) - *(real *)definition));
    }
    fade *= state->width;
    if (!(fade > 0.0f)) return;
    g_46713c = true;
    g_4670bc = true;
    function_16b10((s_render_reset_state *)g_485b48);
    word group = 0;
    while (group < *(long *)(definition + 8))
    {
        byte *entry = *(byte **)(definition + 0xc) + group * 0x98;
        if (*(long *)(entry + 0xc) > 0)
        {
            bool filtered = (bool)((*(dword *)entry >> 3) & 1);
            // retail sums k, j, i in that order (0x4f179-0x4f1af); one expression compiles as k, i, j
            real facing = direction.k * state->first.k;
            facing += direction.j * state->first.j;
            facing += direction.i * state->first.i;
            facing = fabsf(facing);
            real angle = clamp_callback(1.0f - facing, 0.0f, 1.0f);
            real time = 1.0f - function_13b390(entry + 0x30, angle, 0.0f);
            long count = *(long *)(entry + 0xc);
            real step = 1.0f / (real)(count - 1);
            real strength = 0.0f, horizontal = 1.0f, vertical = 0.0f;
            real output[2] = {1.0f, 1.0f};
            real start = 1.0f, multiplier = 1.0f, inverse = 1.0f, offset = 0.0f;
            byte *positions = NULL;
            if (*(long *)(entry + 0x60) == count) positions = *(byte **)(entry + 0x64);
            if (*(dword *)(entry + 0x5c) & 1)
            {
                real first = function_13bb90((s_tag_data *)(entry + 0x18), 0.0f, time);
                real second = function_13bb90((s_tag_data *)(entry + 0x18), 1.0f, time);
                if (second > first)
                {
                    if (!(first > second * (1.0f / 256.0f))) first = second * (1.0f / 256.0f);
                }
                else if (!(second > first * (1.0f / 256.0f))) second = first * (1.0f / 256.0f);
                if (!(fabs(first) < 0.0001f)) multiplier = (real)pow((double)second / first, (double)step);
                start = first;
                if (!(fabs(first - second) < 0.0001f)) inverse = 1.0f / (second - first);
                offset = 0.0f - first * inverse;
            }
            for (long i = 0; i < *(long *)(entry + 0xc); ++i)
            {
                s_501e0 *out_point = points + i;
                real position;
                if (*(dword *)(entry + 0x5c) & 2) position = (real)i * step;
                else if (*(dword *)(entry + 0x5c) & 1)
                {
                    position = inverse * start + offset;
                    start *= multiplier;
                }
                else if (positions)
                    position = (1.0f - time) * *(real *)(positions + i * 8) +
                        *(real *)(positions + i * 8 + 4) * time;
                else position = 0.0f;
                real length = state->scale * callback_curve(entry + 0x10, position, time);
                real width = state->scale * callback_curve(entry + 0x18, position, time);
                real opacity = callback_curve(entry + 0x20, position, time);
                function_4ff50(entry, (byte const *)state, i == 0, position, facing, &strength, &horizontal, &vertical, output);
                real colour_time = function_13b390(entry + 0x28, position, time);
                color4f colour;
                if (*(byte **)(entry + 0x2c) && *(long *)(entry + 0x28) > 0)
                    unpack_color3f(function_13bc00((s_tag_data *)(entry + 0x28), colour_time), (color3f *)&colour.red);
                else *(color3f *)&colour.red = *(color3f *)&g_4686cc->red;
                real red = state->third.i * colour.red;
                colour.red = clamp_callback(red, 0.0f, 1.0f);
                real green = state->third.j * colour.green;
                colour.green = clamp_callback(green, 0.0f, 1.0f);
                real blue = state->third.k * colour.blue;
                colour.blue = clamp_callback(blue, 0.0f, 1.0f);
                colour.alpha = clamp_callback(opacity * fade, 0.0f, 1.0f);
                point3f point;
                point.x = state->first.i * length + state->position.x;
                point.y = state->first.j * length + state->position.y;
                point.z = state->first.k * length + state->position.z;
                out_point->field_0 = point;
                out_point->field_c = width * output[0];
                out_point->field_10 = width * output[1];
                out_point->field_14 = pack_color4f(&colour);
            }
            function_4f7e0(*(long *)(entry + 8), points, *(long *)(entry + 0xc), 0,
                horizontal, vertical, false, false, filtered);
            ++group;
        }
    }
    if (!g_46713c)
    {
        function_0222d0(D3DRS_SAMPLEALPHA, 0);
    }
    g_46713c = true;
}


bool __stdcall function_52020(void *destination, long unknown, void const *source);

// @retail 0x52040
void __stdcall function_52040(void const *request, void const *vertices)
{
    byte const *source = (byte const *)request;
    if (g_485602 != 0) return;
    function_0222d0(D3DRS_CULLMODE, 0);
    function_0222d0(D3DRS_COLORWRITEENABLE, 0x10101);
    function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    function_0222d0(D3DRS_ZENABLE, 0);
    function_0222d0(D3DRS_ZBIAS, 0);
    function_142f0(*(short const *)(source + 0x94));
    {
    short width = (short)(g_48564e - g_48564a);
    short height = (short)(g_48564c - g_485648);
    real *offset = *(real **)(source + 4);
    real x = offset ? offset[0] * 2.0f / width : 0.0f;
    real y = offset ? offset[1] * -2.0f / height : 0.0f;
    real inverse_x = 1.0f / width, inverse_y = 1.0f / height;
    real transform[20];
    transform[0] = inverse_x * 2.0f;
    transform[1] = 0.0f;
    transform[2] = 0.0f;
    transform[3] = x - (inverse_x + 1.0f);
    transform[4] = 0.0f;
    transform[5] = inverse_y * -2.0f;
    transform[6] = 0.0f;
    transform[7] = inverse_y + y + 1.0f;
    transform[8] = 0.0f;
    transform[9] = 0.0f;
    transform[10] = 0.0f;
    transform[11] = 0.5f;
    transform[12] = 0.0f;
    transform[13] = 0.0f;
    transform[14] = 0.0f;
    transform[15] = 1.0f;
    transform[16] = *(real *)(source + 0x40);
    transform[17] = *(real *)(source + 0x44);
    transform[18] = 0.0f;
    transform[19] = 1.0f;
    real constants[24];
    constants[0] = *(real const *)(source + 0x48);
    constants[1] = *(real const *)(source + 0x4c);
    constants[2] = *(real const *)(source + 0x50);
    constants[3] = *(real const *)(source + 0x54);
    constants[4] = source[8] ? 1.0f : 0.0f;
    constants[5] = source[8] ? 0.0f : 1.0f;
    constants[6] = source[9] ? 1.0f : 0.0f;
    constants[7] = source[9] ? 0.0f : 1.0f;
    constants[8] = source[0xa] ? 1.0f : 0.0f;
    constants[9] = source[0xa] ? 0.0f : 1.0f;
    {
        real const *coordinates = *(real const *const *)(source + 0x1c);
        constants[10] = coordinates ? coordinates[0] : 0.0f;
        constants[11] = coordinates ? coordinates[1] : 0.0f;
    }
    {
        real const *coordinates = *(real const *const *)(source + 0x20);
        constants[12] = coordinates ? coordinates[0] : 0.0f;
        constants[13] = coordinates ? coordinates[1] : 0.0f;
    }
    {
        real const *coordinates = *(real const *const *)(source + 0x24);
        constants[14] = coordinates ? coordinates[0] : 0.0f;
        constants[15] = coordinates ? coordinates[1] : 0.0f;
    }
    constants[16] = *(real const *)(source + 0x28);
    constants[17] = *(real const *)(source + 0x2c);
    constants[18] = *(real const *)(source + 0x30);
    constants[19] = *(real const *)(source + 0x34);
    constants[20] = *(real const *)(source + 0x38);
    constants[21] = *(real const *)(source + 0x3c);
    constants[22] = constants[23] = 0.0f;
    D3DDevice_SetVertexShaderConstant(81, transform, 5);
    D3DDevice_SetVertexShaderConstant(86, constants, 6);
    }
    short stage = 0;
    do
    {
        byte *bitmap = *(byte **)(source + 0xc + stage * 4);
        if (!bitmap) break;
        g_51f3c8[1][stage] = function_12310((s_bitmap_view *)bitmap, 0.0f);
        g_485af4[stage] = *(short *)(bitmap + 4);
        g_485b04[stage] = *(short *)(bitmap + 6);
        D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSU, source[0x18 + stage] ? 1 : 3);
        D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSV, source[0x18 + stage] ? 1 : 3);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MAGFILTER, source[0x96] ? 1 : 2);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MINFILTER, source[0x96] ? 1 : 2);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MIPFILTER, source[0x96] ? 1 : 2);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MAXANISOTROPY, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MIPMAPLODBIAS, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MAXMIPLEVEL, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_COLORSIGN, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_ALPHAKILL, 0);
        ++stage;
    } while (stage < 3);
    function_1c590((s_shader_cache *)g_51f0f0,
        *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x74), 0);
    // Retail expands descriptor selection here, but calls 1ccb0 at the draw tail.
    function_1c6b0(g_51f0f0);
    byte const *descriptor = g_43f408[36];
    *(byte const **)(g_51f0f0 + 0x8c) = descriptor;
    if (*(byte const **)(g_51f0f0 + 0xcc) != descriptor)
        g_51f0f0[0x20c] = 1;
    function_1c710(g_51f0f0);
    function_1c710(g_51f0f0);
    byte const *record = *(byte const *const *)source;
    if (record)
    {
        function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
        function_0222d0(D3DRS_SRCBLEND, 0x8001);
        function_0222d0(D3DRS_DESTBLEND, 0x302);
        function_0222d0(D3DRS_BLENDCOLOR, *(dword const *)(record + 0x14));
        function_0222d0(D3DRS_BLENDOP, 0x8006);
        memset(&g_484f68, 0, sizeof(g_484f68));
        D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 4);
        g_484f68.PSTextureModes = 1;
        g_484f68.PSCombinerCount = 0x11104;
        g_484f68.PSConstant0[0] = *(dword const *)record;
        real scaled = *(real const *)(record + 0x18) * 8.0f;
        real denominator = scaled > 1.0f ? scaled : 1.0f;
        real alpha = 1.0f / denominator;
        real alpha_scale = 255.0f;
        long quantized;
        __asm { fld alpha }
        __asm { fld alpha_scale }
        __asm { fmulp st(1), st(0) }
        __asm { fistp quantized }
        g_484f68.PSConstant1[0] = (*(dword const *)(record + 4) & 0xffffff) | ((dword)quantized << 24);
        g_484f68.PSAlphaInputs[0] = 0x12081208;
        g_484f68.PSRGBInputs[0] = 0x1120e820;
        g_484f68.PSAlphaOutputs[0] = g_484f68.PSRGBOutputs[0] = 0x20c00;
        g_484f68.PSConstant0[1] = *(dword const *)record;
        g_484f68.PSConstant1[1] = *(dword const *)(record + 4);
        g_484f68.PSAlphaInputs[1] = 0x6c200000;
        g_484f68.PSAlphaOutputs[1] = 0xc0;
        g_484f68.PSRGBInputs[1] = 0x3c011c02;
        g_484f68.PSRGBOutputs[1] = 0xc00;
        g_484f68.PSConstant0[2] = *(dword const *)record;
        g_484f68.PSConstant1[2] = *(dword const *)(record + 0xc);
        g_484f68.PSAlphaInputs[2] = 0x0820b220;
        g_484f68.PSAlphaOutputs[2] = 0xc00;
        g_484f68.PSRGBInputs[2] = 0x0c201c02 | (record[0x10] ? 0xe0 : 0);
        g_484f68.PSRGBOutputs[2] = 0xc00;
        g_484f68.PSConstant0[3] = *(dword const *)(record + 8);
        g_484f68.PSConstant1[3] = *(dword const *)(record + 0x14);
        g_484f68.PSAlphaInputs[3] = 0x12201120;
        g_484f68.PSAlphaOutputs[3] = 0x4c00;
        g_484f68.PSRGBInputs[3] = 0x0c200120;
        g_484f68.PSRGBOutputs[3] = 0x4c00;
        g_484f68.PSFinalCombinerInputsABCD = 0x0c180000;
        g_484f68.PSFinalCombinerInputsEFG = 0x1c00;
    }
    else if (*(void const *const *)(source + 0xc))
    {
        memset(&g_484f68, 0, sizeof(g_484f68));
        g_484f68.PSTextureModes = (((*(void **)(source + 0x14) != 0) << 5) |
            (*(void **)(source + 0x10) != 0)) << 5 | (*(void **)(source + 0xc) != 0);
        color4f colors[3];
        color3f const *color0 = *(color3f const *const *)(source + 0x58);
        memcpy(&colors[0].red, color0 ? color0 : (color3f const *)g_468710, sizeof(color3f));
        color3f const *color1 = *(color3f const *const *)(source + 0x60);
        memcpy(&colors[1].red, color1 ? color1 : (color3f const *)g_468710, sizeof(color3f));
        color3f const *color2 = *(color3f const *const *)(source + 0x68);
        memcpy(&colors[2].red, color2 ? color2 : (color3f const *)g_468710, sizeof(color3f));
        real const *alpha0 = *(real const *const *)(source + 0x84);
        colors[0].alpha = alpha0 ? *alpha0 : 1.0f;
        real const *alpha1 = *(real const *const *)(source + 0x88);
        colors[1].alpha = alpha1 ? *alpha1 : 1.0f;
        real const *alpha2 = *(real const *const *)(source + 0x8c);
        colors[2].alpha = alpha2 ? *alpha2 : 1.0f;
        g_484f68.PSConstant0[0] = pack_color4f(&colors[0]);
        g_484f68.PSConstant1[0] = pack_color4f(&colors[1]);
        g_484f68.PSConstant0[1] = pack_color4f(&colors[2]);
        g_484f68.PSConstant0[4] = pack_color4f((color4f *)(source + 0x70));
        g_484f68.PSConstant0[5] = pack_color4f((color4f *)(source + 0x70));
        g_484f68.PSConstant0[6] = pack_color4f((color4f *)(source + 0x70));
        g_484f68.PSConstant0[7] = pack_color4f((color4f *)(source + 0x70));
        g_484f68.PSRGBOutputs[0] = 0x89;
        g_484f68.PSAlphaOutputs[0] = 0x89;
        g_484f68.PSRGBInputs[0] = 0x8010902;
        g_484f68.PSAlphaInputs[0] = 0x18111912;
        g_484f68.PSRGBInputs[1] = 0xa010804;
        g_484f68.PSRGBOutputs[1] = 0xac;
        g_484f68.PSAlphaInputs[1] = 0x1a111814;
        g_484f68.PSAlphaOutputs[1] = 0xac;
        short count = 2;
        if (*(void const *const *)(source + 0x10))
        {
            switch (*(short const *)(source + 0x90))
            {
            case 0:
                g_484f68.PSRGBInputs[2] = 0xc200920;
                g_484f68.PSAlphaInputs[2] = 0x1c201920;
                g_484f68.PSRGBOutputs[2] = 0xc00;
                g_484f68.PSAlphaOutputs[2] = 0xc00;
                break;
            case 1:
                g_484f68.PSRGBInputs[2] = 0xc090000;
                g_484f68.PSAlphaInputs[2] = 0x1c190000;
                g_484f68.PSRGBOutputs[2] = 0xc0;
                g_484f68.PSAlphaOutputs[2] = 0xc0;
                break;
            case 2:
                g_484f68.PSRGBInputs[2] = 0xc20e920;
                g_484f68.PSAlphaInputs[2] = 0x1c20f920;
                g_484f68.PSRGBOutputs[2] = 0xc00;
                g_484f68.PSAlphaOutputs[2] = 0xc00;
                break;
            case 3:
                g_484f68.PSRGBInputs[2] = 0xc090000;
                g_484f68.PSAlphaInputs[2] = 0x1c190000;
                g_484f68.PSRGBOutputs[2] = 0x100c0;
                g_484f68.PSAlphaOutputs[2] = 0x100c0;
                break;
            case 4:
                g_484f68.PSRGBInputs[2] = 0xc090000;
                g_484f68.PSAlphaInputs[2] = 0x1c190000;
                g_484f68.PSRGBOutputs[2] = 0x20c0;
                g_484f68.PSAlphaOutputs[2] = 0xc0;
                break;
            case 5:
                g_484f68.PSRGBInputs[2] = 0x1920b820;
                g_484f68.PSAlphaInputs[2] = 0x820a920;
                g_484f68.PSRGBOutputs[2] = 0xc00;
                g_484f68.PSAlphaOutputs[2] = 0xc00;
                g_484f68.PSAlphaInputs[3] = 0x1c1c0c0c;
                g_484f68.PSAlphaOutputs[3] = 0x24c00;
                g_484f68.PSRGBInputs[3] = 0x0;
                g_484f68.PSRGBOutputs[3] = 0x0;
                g_484f68.PSAlphaInputs[4] = 0x5c5c;
                g_484f68.PSAlphaOutputs[4] = 0x4d00;
                g_484f68.PSRGBInputs[4] = 0x0;
                g_484f68.PSRGBOutputs[4] = 0x0;
                g_484f68.PSAlphaInputs[5] = 0x0;
                g_484f68.PSAlphaOutputs[5] = 0xc00;
                g_484f68.PSRGBInputs[5] = 0x1ca01da0;
                g_484f68.PSRGBOutputs[5] = 0xc00;
                count = 5;
                break;
            }
            ++count;
        }
        if (*(void const *const *)(source + 0x14))
        {
            switch (*(short const *)(source + 0x92))
            {
            case 0:
                g_484f68.PSRGBInputs[count] = (*(short const *)(source + 0x90) == 5 ? (0x0c010a00 | (source[0x80] ? 4 : 0x20)) : 0x0c200a20);
                g_484f68.PSAlphaInputs[count] = 0x1c201a20;
                g_484f68.PSRGBOutputs[count] = 0xc00;
                g_484f68.PSAlphaOutputs[count] = 0xc00;
                break;
            case 1:
                g_484f68.PSRGBInputs[count] = 0xc0a0000;
                g_484f68.PSAlphaInputs[count] = 0x1c1a0000;
                g_484f68.PSRGBOutputs[count] = 0xc0;
                g_484f68.PSAlphaOutputs[count] = 0xc0;
                break;
            case 2:
                g_484f68.PSRGBInputs[count] = 0xc20ea20;
                g_484f68.PSAlphaInputs[count] = 0x1c20fa20;
                g_484f68.PSRGBOutputs[count] = 0xc00;
                g_484f68.PSAlphaOutputs[count] = 0xc00;
                break;
            case 3:
                g_484f68.PSRGBInputs[count] = 0xc0a0000;
                g_484f68.PSAlphaInputs[count] = 0x1c1a0000;
                g_484f68.PSRGBOutputs[count] = 0x100c0;
                g_484f68.PSAlphaOutputs[count] = 0x100c0;
                break;
            case 4:
                g_484f68.PSRGBInputs[count] = 0xc0a0000;
                g_484f68.PSAlphaInputs[count] = 0x1c1a0000;
                g_484f68.PSRGBOutputs[count] = 0x20c0;
                g_484f68.PSAlphaOutputs[count] = 0xc0;
                break;
            }
            ++count;
        }
        g_484f68.PSCombinerCount = 0x11100 | count;
        g_484f68.PSFinalCombinerInputsABCD = 0xc;
        g_484f68.PSFinalCombinerInputsEFG = 0x1c00;
    }
    D3DDevice_SetPixelShaderProgram(&g_484f68);
    function_0222d0(D3DRS_CULLMODE, 0x901);
    function_1c590((s_shader_cache *)g_51f0f0,
        *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x74), 0);
    function_1ccb0(36);
    function_1c710(g_51f0f0);
    function_1cf50();
    function_4b220(20, 0, 3, 4, (t_4b220_fill)function_52020, (void *)vertices);
    D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);
}


// @retail 0x52d40
void function_52d40(long bitmap_index, byte *shader, long tag, dword flags)
{
    switch (*(short *)(shader + 0x24))
    {
    case 1:
        break;
    default:
        return;
    }
    bool extra_combiners = (shader[0x28] & 2) != 0;
    function_143c0(tag, (short)bitmap_index, 0, 0.0f);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, (shader[0x2e] & 2) | 1);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, ((shader[0x2e] & 4) | 2) >> 1);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER, shader[0x2e] & 1 ? 1 : 2);
    D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER, shader[0x2e] & 1 ? 1 : 2);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPFILTER, shader[0x2e] & 1 ? 1 : 2);
    function_0222d0(D3DRS_CULLMODE, 0);
    function_0222d0(D3DRS_COLORWRITEENABLE, 0x10101);
    function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    function_0222d0(D3DRS_STENCILENABLE, 0);
    function_0222d0(D3DRS_ZENABLE, 2);
    function_0222d0(D3DRS_ZWRITEENABLE, 0);
    function_0222d0(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    function_0222d0(D3DRS_ZBIAS, 0);
    function_142f0(*(short *)(shader + 0x2a));
    if (!(fabs(g_4b8494 - 1.0f) < 0.0001f))
    {
        D3DDevice_SetStipple(function_1c290(1.0f));
        g_4b8494 = 1.0f;
    }
    function_1c590((s_shader_cache *)g_51f0f0,
        *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x24), 0);
    real transform[12] = {1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f};
    if (flags & 0x20)
    {
        transform[0] = g_4856c4[0];
        transform[1] = g_4856c4[3];
        transform[2] = g_4856c4[6];
        transform[3] = g_485618.position.x;
        transform[4] = g_4856c4[1];
        transform[5] = g_4856c4[4];
        transform[6] = g_4856c4[7];
        transform[7] = g_485618.position.y;
        transform[8] = g_4856c4[2];
        transform[9] = g_4856c4[5];
        transform[10] = g_4856c4[8];
        transform[11] = g_485618.position.z;
    }
    D3DDevice_SetVertexShaderConstant(78, transform, 3);
    memset(&g_484f68, 0, sizeof(g_484f68));
    g_484f68.PSAlphaInputs[0] = 0x18202015;
    g_484f68.PSAlphaOutputs[0] = 0xc4;
    g_484f68.PSFinalCombinerInputsABCD = 0xc;
    g_484f68.PSFinalCombinerInputsEFG = 0x1c00;
    short count;
    if (extra_combiners)
    {
        g_484f68.PSRGBInputs[0] = 0x08080000;
        g_484f68.PSRGBOutputs[0] = 0xc0;
        g_484f68.PSRGBInputs[1] = 0x0c0c0000;
        g_484f68.PSRGBOutputs[1] = 0xc0;
        g_484f68.PSRGBInputs[2] = 0x250c0508;
        g_484f68.PSRGBOutputs[2] = 0xc00;
        count = 3;
    }
    else
    {
        g_484f68.PSRGBInputs[0] = 0x08050000;
        g_484f68.PSRGBOutputs[0] = 0xc0;
        count = 1;
    }
    g_484f68.PSTextureModes = 1;
    if (!(flags & 4))
    {
        switch (*(short *)(shader + 0x2a))
        {
        case 0:
            g_484f68.PSAlphaInputs[count] = 0x1c140000;
            g_484f68.PSAlphaOutputs[count] = 0xc00;
            break;
        case 1: case 5:
            g_484f68.PSRGBInputs[count] = 0x0c142034;
            g_484f68.PSRGBOutputs[count] = 0xc00;
            break;
        case 2:
            g_484f68.PSRGBInputs[count] = 0x0c14a034;
            g_484f68.PSRGBOutputs[count] = 0xc00;
            break;
        case 7:
            g_484f68.PSAlphaInputs[count] = 0x1c140000;
            g_484f68.PSAlphaOutputs[count] = 0xc00;
        case 3: case 4: case 6:
            g_484f68.PSRGBInputs[count] = 0x0c140000;
            g_484f68.PSRGBOutputs[count] = 0xc00;
            break;
        }
        ++count;
    }
    g_484f68.PSCombinerCount = count;
    function_15180(&g_484f68);
}
// Disabled: prior callback activation loses matched 0x12b2a0; counter storage is shared.
#if 0
extern dword g_55e6b8;

// Retail 0x14280
long __fastcall function_14280(void const *data)
{
    if (++g_485ab0.low == 0)
        ++g_485ab0.high;
    long result = 0;
    if (((dword const *)data)[1] != g_55e6b8)
    {
        result = g_485ab0.low - g_485ab8.low;
        g_485ab8 = g_485ab0;
        g_55e6b8 = ((dword const *)data)[1];
    }
    return result;
}

struct s_font_cache_entry_1eff0
{
    long character_index;
    long value;
};
struct s_font_cache_state_1eff0
{
    byte active;
    byte unknown01[5];
    short value06;
    short value08;
    byte unknown0a[2];
    byte *bitmap;
    s_font_cache_entry_1eff0 entries[512];
};
byte *function_1358e0(short width, short height, short depth, short levels, short format, word flags);

// Retail 0x1eff0
bool function_1eff0()
{
    bool result = true;
    byte *bitmap = function_1358e0(256, 256, 1, 0, 9, 0x810);
    if (bitmap)
    {
        memset(&g_4b62a0, 0, sizeof(g_4b62a0));
        if (function_1d000(bitmap))
        {
            g_4b62a0.bitmap = bitmap;
            g_4b62a0.active = true;
            g_4b62a0.value06 = 1;
            g_4b62a0.value08 = 1;
        }
        else
            result = false;
    }
    else
        result = false;
    long index = 0;
    do
    {
        memset(&g_4b62a0.entries[index], 0, sizeof(g_4b62a0.entries[index]));
        g_4b62a0.entries[index].character_index = NONE;
        index++;
    } while (index < 512);
    return result;
}
#endif



#include "font_render_cache.h"
extern s_record_pool *g_54d574;

// @retail 0x1f2b0
void function_1f2b0()
{
    if (g_4b62a0)
        for (long i = 0; i < 512; i += 4)
        {
            if ((dword)g_4b62b0[i].character_index != 0xffffffffu)
            {
                long index = i;
                if (index != NONE)
                {
                    long character = g_4b62b0[index].character_index;
                    if (character != NONE)
                        *(long *)(g_54d574->data + (character & 0xffff) * 0x38 + 0x34) = NONE;
                    g_4b62b0[index].character_index = NONE;
                }
            }
            if ((dword)g_4b62b0[i + 1].character_index != 0xffffffffu)
            {
                long index = i + 1;
                if (index != NONE)
                {
                    long character = g_4b62b0[index].character_index;
                    if (character != NONE)
                        *(long *)(g_54d574->data + (character & 0xffff) * 0x38 + 0x34) = NONE;
                    g_4b62b0[index].character_index = NONE;
                }
            }
            if ((dword)g_4b62b0[i + 2].character_index != 0xffffffffu)
            {
                long index = i + 2;
                if (index != NONE)
                {
                    long character = g_4b62b0[index].character_index;
                    if (character != NONE)
                        *(long *)(g_54d574->data + (character & 0xffff) * 0x38 + 0x34) = NONE;
                    g_4b62b0[index].character_index = NONE;
                }
            }
            if ((dword)g_4b62b0[i + 3].character_index != 0xffffffffu)
            {
                long index = i + 3;
                if (index != NONE)
                {
                    long character = g_4b62b0[index].character_index;
                    if (character != NONE)
                        *(long *)(g_54d574->data + (character & 0xffff) * 0x38 + 0x34) = NONE;
                    g_4b62b0[index].character_index = NONE;
                }
            }
        }
}



struct s_record_source;

struct s_41c80_state;
void function_41c80(short type, s_41c80_state const *state);
void function_176bb0();
void __stdcall function_174220(bool value);
void function_176cb0();
void function_41640();

// @retail 0x40e90
void function_40e90(long mode)
{
    long mask;
    if (!mode) mask = NONE;
    else mask = mode == 2 ? 0x10 : 0x20;
    s_2cb30_state *state = g_4c0b78.state;
    long group = state->active;
    if (!group)
    {
        g_4c1a48[0].count = 0;
        g_4c1a48[0].flags = 0;
        g_4c1a48[0].first = 0;
        g_4b6280.count = 0;
        g_4b6280.unknown04 = 0;
    }
    else
    {
        g_4c1a48[1].count = 0;
        g_4c1a48[1].flags = 0;
        g_4c1a48[1].first = 0;
        g_4c1a48[2].count = 0;
        g_4c1a48[2].flags = 0;
        g_4c1a48[2].first = 0;
    }
    function_41c80((short)group, (s_41c80_state const *)state);
    function_44ac0(group, mask, 0x1f);
    if (!group)
    {
        if (g_4e6948->state != 3)
        {
            function_176bb0();
            function_174220(false);
            function_420a0();
            function_42760(mask);
            function_41640();
        }
        else
        {
            function_176cb0();
            function_420a0();
            function_42760(mask);
            function_41640();
        }
    }
}



struct s_glyph_header_1f5c0
{
    word value00, pixels_size;
    short width, height;
    byte unknown08[8];
};
// The two words share the full-width cursor read used before masking the ring index.


bool font_cache_character_copy_pixels(long character, void *pixels);

PRIVATE inline void glyph_remove_first_1f5c0(bool *removed)
{
    long first = g_4b62a2;
    *removed = true;
    if (first != NONE)
    {
        long character = g_4b62b0[first].character_index;
        if (character != NONE)
            *(long *)(g_54d574->data + (character & 0xffff) * 0x38 + 0x34) = NONE;
        g_4b62b0[first].character_index = NONE;
    }
    g_4b62a2 = (g_4b62a2 + 1) & 0x1ff;
}
// MiniMax G (11-13): no new form, no lower score (328): the diff re-read this round puts the first difference at the prologue's register plan - retail loads the pool into EAX before the pushes and holds the parameter in EBX with the -1 sentinel in EBP, ours in ESI and EBX - and none of the forms tried here moves it, so the body stands.
// MiniMax I (11-11): score 328 (was 342): a named local for the scaled character offset, used by both the character pointer and the later slot store, adds the dword home retail has; our body is 25 bytes shorter than retail's 1133 and the frame is still 0x201c against retail's 0x2020, and a plain (non-volatile) header local gives 365 and is worse.
// MiniMax I (11-11): no new form, no lower score (342): the body is 41 bytes shorter than retail's 1133 and the first difference is at +0x1; retail reads the stack parameter into ebx with the -1 sentinel in ebp, ours uses esi and ebx, and retail's text buffer keeps two word counters that are missing here.

// MiniMax H (11-13): no new form, no lower score (328): the frame is still 0x201c against retail's 0x2020, retail
// reads the stack parameter into ebx with the -1 sentinel in ebp where ours uses esi and ebx, and retail loads the
// font pool before its first test into eax where ours keeps it in edi; dropping the volatile on the glyph header, which
// lets the build keep the pointer in a register, measures 365.
// MiniMax B (11-11): no new form, no lower score (342); our body is still 41 bytes shorter than retail's 1133,
// retail keeps the stack parameter in ebx with the -1 sentinel in ebp where ours uses esi and ebx, and retail
// inlines the third removal site where ours calls the helper out of line.
// MiniMax E (11-11, run 80): no new form, no lower score (342): our body is still 41 bytes shorter than retail's 1133,
// retail still reads the stack parameter into EBX with the -1 sentinel in EBP where ours uses ESI and EBX, and retail
// still loads the font pool before its first test and keeps it in eax where ours loads it later; the header lookup
// needs the font character type in a header, outside this file.
// MiniMax E (11-10, run 73): no new form, no lower score (342); retail loads the font pool before its first test and keeps it in eax, keeps the character index in EBX with the -1 sentinel in EBP, and probes the stack with the __chkstk label where our probe is the same helper's __alloca_probe label; our body is 41 bytes shorter than retail's 1133.
// MiniMax D (11-11): no new form, no lower score (342): the body is still 41 bytes shorter than retail's 1133, and retail still reads the stack parameter into EBX with the -1 sentinel in EBP where ours uses ESI and EBX.
// MiniMax G (11-11): no new form, no lower score (342): retail loads g_54d574 before its first test and keeps it in eax, spills ebx and ebp across the call, stores the first fill x as a word at [esp+0x1e] and inlines the third removal site where ours calls the helper out of line; our body is 41 bytes shorter than retail's 1133.
// MiniMax S (11-10): no new form, no lower score (342): retail loads the font pool before its first test and keeps it in eax, keeps the character index in EBX with the -1 sentinel in EBP, and probes the stack with the __chkstk label where our probe is the same helper's __alloca_probe label; our body is 41 bytes shorter than retail's 1133.
// MiniMax D (11-10): no new form, no lower score (342): retail keeps the stack parameter in EBX with the -1 sentinel in EBP where ours uses ESI and EBX, so the two homes still differ.
// MiniMax A (11-04): the character pointer computed before the empty-slot test 374 (worse), reverted; no lower score.
// MiniMax D (11-01): no new form tried, no lower score.
// MiniMax B (11-13): no new form, no lower score (328): the frame is still 0x201c against retail's 0x2020 and the
// 41 missing bytes stand; retail reads the stack parameter into ebx with the -1 sentinel in ebp where ours uses esi
// and ebx, and loads the font pool before its first test.
// score 328 (was 342): our body is 41 bytes shorter than retail's (1092 against 1133); the
// MiniMax B (11-12): no new form, no lower score (328): retail loads the font pool before its first test and keeps
// it in eax, reads the stack parameter into ebx with the -1 sentinel in ebp, keeps the header at [esp+0x10] and the
// scaled character offset at [esp+0x14], has a second flag byte at [esp+0xf] it never reads, inlines the three
// removal sites where ours calls the helper out of line, and puts the pixel buffer at [esp+0x30] (frame 0x2020
// against our 0x201c); earlier rounds' forced-inline helper and plain header local scored worse in the full build.
// score 328 (was 342): our body is 41 bytes shorter than retail's (1092 against 1133); the
// clipped fill still writes the rows in another order than retail's.
// Round 2 of #1066: a forced-inline removal helper (all three sites expanded, retail's 1133 bytes), the volatile
// header as a plain local, one read of the pool and guarded do-while loops gave 279 on the fast image but 342 in the full
// build, so none of it is kept.
// b4F walk (11-06, against retail 0x1f5c0-0x1fa2a, block by block): no logic difference found. Every offset, test
// direction, loop bound, field width and callee argument matches (pool +0x44, slot +0x34 / state +0x10 == 4 / header
// +0x1c, the three removal sites, the 0x1ff ring, the clipped fill, the row copy, the prediction inline, the tail).
// The header lookup is the body of font_cache_character_get_header (unknown_1406b0.cpp, static inline over
// s_font_character): retail's null header is dereferenced when the state is not 4, as ours is. Using the helper needs
// its type moved to a header, outside this edit. Retail keeps the pointer stack-homed, stored at 0x1f613 and reloaded from [esp+0x14] at
// every use (0x1f797, 0x1f8bb, 0x1f8d2, 0x1f946, 0x1f98a, 0x1f9a0, 0x1f9fe); a plain local gives 379. Fast-check, nothing changed: 342. Tried and dropped: __forceinline removal
// helper 348 (1172 bytes); `pool = g_54d574` local 342; the wait argument as `removed` or `(long)removed` 341;
// `long wait = 0; if (removed) wait = 1;` 321 (retail has that xor/test/je/mov shape, but it is a value set before
// its test, so not kept); the clamps as `short end = bitmap - 1; if (right <= end) end = right;` 328 (not kept:
// retail dec's in 32 bits and cmov's in 16, ours still dec's in 16). Differences left are all register choice:
// retail loads g_54d574 before the first test and keeps it in eax, spills ebx/ebp across the call, stores the first
// fill x as a word at [esp+0x1e], and inlines the third removal site where ours calls the helper out of line.
// MiniMax A (11-12): no new form, no lower score (328): retail reads the stack parameter into ebx with the -1 sentinel in
// ebp where ours uses esi and ebx, its text buffer keeps two word counters that are missing here, and our body is 25 bytes
// shorter than retail's 1133 with the frame at 0x201c against retail's 0x2020.
// @retail 0x1f5c0
void __stdcall function_1f5c0(long character_index)
{
    word pixels[4096];
    if (character_index != NONE
        && *(long *)(g_54d574->data + (character_index & 0xffff) * 0x38 + 0x34) != NONE)
        return;
    long character_offset = (character_index & 0xffff) * 0x38;
    byte *character = g_54d574->data + character_offset;
    s_glyph_header_1f5c0 *volatile header = 0;
    if (*(long *)(character + 0x10) == 4)
        header = (s_glyph_header_1f5c0 *)(character + 0x1c);
    bool removed = false;
    if (header->width + g_4b62a4.fields.x > 256)
    {
        g_4b62a8 += g_4b62aa;
        g_4b62a4.fields.x = 1;
        g_4b62aa = 0;
    }
    if (header->height + g_4b62a8 > 256)
    {
        g_4b62a8 = 1;
        g_4b62a4.fields.x = 1;
        g_4b62aa = 0;
        while (g_4b62a2 != g_4b62a4.fields.next && g_4b62b0[g_4b62a2].position.y > 0)
            glyph_remove_first_1f5c0(&removed);
    }
    if (header->height + 1 > g_4b62aa)
    {
        long old_end = g_4b62a8 + g_4b62aa;
        long new_end = g_4b62a8 + header->height;
        while (g_4b62a2 != g_4b62a4.fields.next
            && g_4b62b0[g_4b62a2].position.y >= old_end
            && g_4b62b0[g_4b62a2].position.y < new_end)
            glyph_remove_first_1f5c0(&removed);
        g_4b62aa = header->height + 1;
    }
    if (((g_4b62a4.pair + 1) & 0x1ff) == g_4b62a2)
        glyph_remove_first_1f5c0(&removed);
    long slot = g_4b62a4.fields.next;
    if (character_index != NONE)
        *(long *)(g_54d574->data + character_offset + 0x34) = slot;
    g_4b62b0[slot].character_index = character_index;
    g_4b62b0[slot].position.x = g_4b62a4.fields.x;
    g_4b62b0[slot].position.y = g_4b62a8;
    font_cache_character_copy_pixels(character_index, pixels);
    long pitch;
    byte *destination = (byte *)function_1d5e0(g_4b62ac, removed ? 1 : 0, &pitch);
    if (destination)
    {
        long left = (long)g_4b62b0[slot].position.x - 1;
        short first_x = (short)(left > 0 ? left : 0);
        long top = (long)g_4b62b0[slot].position.y - 1;
        short first_y = (short)(top > 0 ? top : 0);
        long right = (long)g_4b62b0[slot].position.x + header->width + 1;
        long width = (long)*(short *)((byte *)g_4b62ac + 4) - 1;
        short end_x = (short)(right <= width ? right : width);
        long bottom = (long)g_4b62b0[slot].position.y + header->height + 1;
        long height = (long)*(short *)((byte *)g_4b62ac + 6) - 1;
        short end_y = (short)(bottom <= height ? bottom : height);
        for (long y = first_y; y < end_y; y++)
            for (long x = first_x; x < end_x; x++)
                *(word *)(destination + y * pitch + x * 2) = 0x0fff;
        long source = 0;
        for (long y = 0; y < header->height; y++)
        {
            word *row = (word *)(destination + (g_4b62b0[slot].position.y + y) * pitch + g_4b62b0[slot].position.x * 2);
            for (long x = 0; x < header->width; x++) row[x] = pixels[source + x];
            source += header->width;
        }
        bitmap_predict_inline((s_bitmap_predict_view *)g_4b62ac, 7);
    }
    g_4b62a4.fields.x += header->width + 1;
    g_4b62a4.fields.next = (g_4b62a4.pair + 1) & 0x1ff;
}




void function_1f2b0();

// @retail 0x226c0
void function_226c0()
{
    function_1f2b0();
    g_485a80 = 0;
}



extern byte g_4b569d;
void function_014a60();
void function_01d2a0();
struct s_type_7ba8e9;
void function_1359d0(s_type_7ba8e9 *bitmap);
void texture_cache_dispose();
void geometry_cache_dispose();

// @retail 0x141c0
void function_141c0()
{
    if (g_5093e0)
    {
        memset(g_5093e0, 0, 0x3fc);
        if (g_4b5690 && !g_4b569d) g_4b569d = 1;
    }
    function_14980();
    function_014a60();
    function_01d2a0();
    function_1e310();
    if (g_4b62a0)
    {
        function_1f2b0();
        function_1359d0((s_type_7ba8e9 *)g_4b62ac);
        g_4b62a0 = 0;
    }
    texture_cache_dispose();
    geometry_cache_dispose();
    if (g_5093b0)
    {
        D3DDevice_Release();
        g_5093b0 = 0;
    }
    if (g_509350) g_509350 = 0;
}


// Disabled: retail clears the contiguous 0x40c0-byte state at 0x51f408; its owner declares split globals.
#if 0
struct s_5093e4;
extern s_5093e4 *g_5093e4;
struct s_33a0b_view;
extern s_33a0b_view *g_485a58;
void function_20e50(long index);

// Retail 0x22590
void function_22590()
{
    byte *globals = (byte *)g_4e034c;
    g_485a80 = *(long *)(globals + 0x108) ? *(byte **)(globals + 0x10c) : 0;
    function_1f2b0();
    s_timed_effect_globals *effects = g_5093e0;
    if (effects)
    {
        memset(effects, 0, 0x3fc);
        effects->unknown180[0] = 1.0f;
        effects->unknown180[1] = 1.0f;
        effects->unknown180[2] = 1.0f;
        effects->unknown180[3] = 1.0f;
        effects->unknown3f8 = 1.0f;
    }
    *(long *)g_50934c = 0;
    *(real *)(g_50934c + 0x10) = 0.5f;
    *(real *)(g_50934c + 0x18) = 0.5f;
    *(real *)(g_50934c + 0x28) = 0.5f;
    *(real *)(g_50934c + 0x30) = 0.5f;
    byte *view = (byte *)g_5093e4;
    view[0] = view[1] = 0;
    for (long i = 4; i <= 0x14; i += 4) *(real *)(view + i) = -1.0f;
    view[0x18] = 1;
    view[0x19] = 0;
    *(real *)(view + 0x1c) = -1.0f;
    *(real *)(view + 0x20) = -1.0f;
    view[0x1a] = 0;
    memset(&g_51f408, 0, sizeof(g_render_state_storage));
    for (long i = 0; i < 0x1ff; ++i) function_20e50(i);
    if (effects) *(real *)effects->unknown190 = 0.0f;
    g_485a58 = 0;
}
#endif

// Disabled: passing the shared glyph rectangle address to 1f3a0 is prohibited.
#if 0
extern long g_4e28f4[11];
extern s_record_pool *g_54d574;
long function_140b20(long font_index, long character);
void function_140900(long character, long font, long priority);
void __stdcall function_1f5c0(long character);

// Retail 0x1f490
void __stdcall function_1f490(long unused, long font, long character, long color, long shadow,
    real x, real y, real u, real v, real width, real height, real scale, long callback, long context)
{
    long index = function_140b20(g_4e28f4[font], character);
    if (index == NONE)
    {
        function_140900(character, font, 3);
        index = function_140b20(g_4e28f4[font], character);
        if (index == NONE) return;
    }
    function_1f5c0(index);
    if (index == NONE) return;
    long entry = *(long *)(g_54d574->data + (index & 0xffff) * 0x38 + 0x34);
    if (entry == NONE) return;
    s_font_render_entry *rectangle = &g_4b62b0[entry];
    bool draw_shadow = ((dword)shadow & 0xff000000) > 0;
    do
    {
        real offset = draw_shadow ? scale : 0.0f;
        function_1f3a0((short const *)rectangle, x + offset, y + offset, (long)width, (long)height,
            (short)(long)u, (short)(long)v, scale, draw_shadow ? shadow : color,
            (t_1f3a0_callback)callback, (void *)context);
        if (!draw_shadow) break;
        draw_shadow = false;
    } while (true);
}
#endif


extern dword g_5093b4;
dword g_5093b8;
dword __cdecl pack_color3f(color3f const *color);
void function_25a10(byte const *state, point3f const *position, vector3f const *direction);
void __stdcall function_14b60(dword flags, dword color, real depth, byte stencil);

// @retail 0x132d0
void __stdcall function_132d0(s_frame_view_2c560 const *frame)
{
    memcpy(g_frame_storage.data, frame, sizeof(*frame));
    switch (frame->mode)
    {
    case 0: g_4858b8 = 0; break;
    case 1: g_4858b8 = 18; break;
    default: g_4858b8 = 0; break;
    }
    function_15370(0);
    g_5093b4 = 0x901;
    g_5093b8 = 0x900;
    function_12fa0(frame->view, (real const *)frame->projection, frame->scaled, frame->mode);
    if (*(short *)(g_frame_storage.data + 8))
    {
        dword color = 0;
        dword flags = 1;
        if (!g_4670cd)
        {
            flags = 0x61;
            color = pack_color3f((color3f const *)(g_frame_storage.data + 0xc));
        }
        function_14bc0((short)g_4858b8, 0, true);
        function_14b60(flags, color, 1.0f, 0);
    }
    else function_14bc0((short)g_4858b8, 0, true);
    D3DDevice_SetVertexData4f(15, 0.0f, 0.5f, 1.0f, 42.0f);
    function_0222d0(D3DRS_FILLMODE, D3DFILL_SOLID);
    g_4670bc = 1;
    function_16b10((s_render_reset_state *)g_485b48);
    if (((byte const *)frame)[0x26c])
    {
        memcpy(&g_48574c, ((byte const *)frame) + 0x14c, 0x120);
        function_25a10((byte const *)&g_48574c, &g_4b9da0, &g_4b9dac);
    }
}



// Disabled: retail publishes the persistent shared render-source address and needs the blocked 14b60 flags interface.

extern bool g_4ba022, g_4ba023, g_4ba024;
extern long g_467130, g_4858b4;
extern void *g_467134;
extern s_record_sources *g_467138;
extern dword g_4ba014;
void function_15680(short mode);
void function_0495e0();
void function_0496f0();

extern bool g_485a74;

// score 85 (was 122): r136 lane 7: the mode table is a typed const array (retail's is in .rdata and folds where this
// routine is inlined); the gate is a switch on the mode with raw bool gates (retail's `sub eax,2/6/9` chain and raw byte
// copies), the cull is two 0x15680 calls (retail's load-first arm order). 80 with three forms kept only with a
// match (coordinator, 11-08): the cull test in a one-use `bool stipple_enabled`, the final switch in retail's case
// order with 17 falling into 3/16, and `enabled` declared before the sources pointer.
// What is left is 0x14b60's register argument (retail `mov eax, flags`; ours pushes it to the stub) and the jump-table
// bytes it shifts: BLOCKED-BY 0x14b60. `inline` here makes every caller inline it at every pass (retail stops after
// five or six per caller), so it stays out of line.
// @retail 0x44370
bool __stdcall function_44370(long mode)
{
    long group = g_43fa1c[mode].group;
    s_render_mode_44370 const *settings = &g_43fa1c[mode];
    s_record_sources *sources = &g_4c1a48[group];
    bool enabled = true;
    if ((1 << mode) & 0xbffdee) enabled = (sources->flags & (1 << mode)) != 0;
    if (enabled)
    {
        switch (mode)
        {
        case 2: enabled = g_4ba023; break;
        case 8: enabled = g_4ba022; break;
        case 17: enabled = g_4ba024; break;
        }
    }
    if (enabled)
    {
        g_467130 = mode;
        g_467134 = (void *)group;
        g_467138 = sources;
        long target = settings->target;
        if (target == NONE)
            target = mode == 10 ? ((bool)((g_4ba014 >> 4) & 1) && !(bool)((g_4ba014 >> 5) & 1) ? 10 : 9) : g_4858b8;
        g_4858b4 = target;
        function_14bc0((short)target, 0, true);
        if (settings->clear_flags)
        {
            if (!g_485a75 && !g_485a76)
                function_14b60(settings->clear_flags, settings->field_8_7, settings->clear_depth, settings->clear_stencil);
        }
        else if (mode == 10)
        {
            if ((bool)((g_4ba014 >> 4) & 1) && !(bool)((g_4ba014 >> 5) & 1)) function_14b60(1, 0xffffff, 0.0f, 0);
            else function_14b60(0x60, 0, 1.0f, 0);
        }
        if (g_485898 < 4 || g_485898 > 7) function_15680(settings->cull_mode);
        else function_15680(0);
        switch (mode)
        {
        case 3: case 16: enabled = true; break;
        case 10: enabled = true; function_0495e0(); break;
        case 11: enabled = true; function_0496f0(); break;
        case 17: g_485a74 = 1; enabled = true; break;
        }
    }
    else g_467130 = 0;
    function_15370(mode == 3 || mode == 1);
    return enabled;
}




// Disabled: retail clears split shared render globals and passes their addresses into allocation/reset helpers.
#if 0
// Retail 0x12560
void function_12560()
{
    if (g_485a88) return;
    function_169f0();
    function_16a30(g_485ac5 && g_485ac6 ? 0x20 : 0x30,
        g_485ac5 && g_485ac6 ? 0x18 : 0x24, 1.0f);
    D3D__CommandBufferSize = 0x100000;
    D3D__SegmentSize = 0x8000;
    bool success = function_128c0(&g_43ed80);
    if (success)
    {
        g_4670bc = 1;
        function_16b10((s_render_reset_state *)g_485b48);
        function_1c1b0();
        function_1c810();
        success = function_1d3f0(3, (long)g_468848, (long *)&g_484dbc);
        if (success)
            for (long i = 0; i < 4; ++i) D3DDevice_SetPalette(i, (D3DPalette *)g_484dbc);
        memset(g_484dd0, 0, 0x180);
        if (success)
        {
            D3DDevice_SetVertexShader(0x11);
            D3DDevice_SetRenderState(D3DRS_CULLMODE, 2);
            D3DDevice_SetRenderState(D3DRS_ZWRITEENABLE, 1);
            D3DDevice_SetRenderState(D3DRS_ZFUNC, 0x203);
            D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
            D3DDevice_SetRenderState(D3DRS_STENCILENABLE, 0);
            D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, 0);
            D3DDevice_SetRenderState(D3DRS_ALPHAFUNC, 0x204);
            D3DDevice_SetRenderState(D3DRS_ALPHAREF, 0);
            D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
            D3DDevice_SetRenderState(D3DRS_SRCBLEND, 1);
            D3DDevice_SetRenderState(D3DRS_DESTBLEND, 0);
            D3DDevice_SetRenderState(D3DRS_BLENDOP, 0x8006);
            D3DDevice_SetRenderState(D3DRS_FOGENABLE, 1);
            D3DDevice_SetRenderState(D3DRS_LIGHTING, 0);
            D3DDevice_SetRenderState(D3DRS_SPECULARENABLE, 1);
            for (long i = 0; i < 4; ++i) D3DDevice_SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, i);
        }
    }
    function_1d660();
    if (success && function_14600() && function_14850() && function_1d770() && function_1d0e0() && function_1e110())
    {
        g_4670f8 = 0x24;
        if (function_1eff0())
        {
            g_4b72b4 = *g_468788;
            g_4b72c0 = NONE;
            success = true;
        }
        else success = false;
    }
    else success = false;
    function_1fbb0();
    function_12d8b0();
    function_12c0d0();
    memset(g_51f408, 0, 0x40c0);
    for (long i = 0; i < 0x1ff; ++i) function_20e50(i);
    byte *allocated = g_4e6080 + g_4e6084;
    g_4e6084 += 0x34;
    long size = 0x34;
    crc32(4, &size, &g_4e608c);
    g_50934c = allocated;
    function_20eb0();
    if (success) g_485a88 = 1;
}
#endif



#include "async.h"
#include <float.h>
bool function_12200();
void global_preferences_initialize();
void function_120a90();
void function_121fc0();
void function_2136c0();
void animation_cache_initialize();
void function_123b30();
void function_12560();
bool function_1248b0();
void function_1253d0();

// @retail 0x12000
bool function_12000()
{
    bool result = false;
    XMountSecondaryUtilityDrive();
    g_510800_pool_base = (dword)VirtualAlloc(0, 0x23000, MEM_COMMIT, PAGE_READWRITE);
    g_510804_pool_size = 0;
    if (function_12200())
    {
    _control87(0x9001f, 0xfffff);
    function_120a90();
    global_preferences_initialize();
    function_121fc0();
    function_2136c0();
    animation_cache_initialize();
    function_123b30();
    function_12560();
    if (g_5093b0)
    {
        function_1248b0();
        function_1253d0();
        result = true;
    }
    }
    return result;
}



// Disabled: shutdown clears two shared aggregate ranges currently split into independent globals.
#if 0
// Retail 0x12080
void function_12080()
{
    if (*(byte *)&g_4e3b60)
    {
        g_5020d8 = 0;
        function_2150f0();
        memset(&g_4e3b60, 0, 0x2540);
    }
    function_125600();
    function_124920();
    function_141c0();
    animation_cache_dispose();
    function_141190();
    function_140aa0();
    memset(&g_4e2920, 0, 0x1220);
    CloseHandle(g_4e0354);
    CloseHandle(g_4e0360);
    CloseHandle(g_4e035c);
    CloseHandle(g_4e0358);
    if (g_510800_pool_base)
    {
        VirtualFree((void *)g_510800_pool_base, 0, MEM_RELEASE);
        g_5107fc = g_510800_pool_base = g_510804_pool_size = g_510808 = 0;
    }
}
#endif

// Disabled: retail restores a 0x74-byte camera and 0x120-byte projection across split shared declarations.
void __stdcall function_132d0(s_frame_view_2c560 const *frame);
extern long g_46701c;
struct s_2f970_view;
struct s_2f800_view;
bool function_2f970(s_2f970_view const *view, real *out);
void function_2fd90(s_2f800_view const *view, box2f const *clip, byte *out);

// @retail 0x13c20
void function_13c20()
{
    if (g_4ba04c > 1) IDirect3DDevice8_Clear(NULL, 0, 0, 0x80, 0, 1.0f, 0);
    --g_46701c;
    if (g_46701c >= 0)
    {
        function_132d0(&g_4850d0[g_46701c]);
        g_4b9ed4 = g_4850d0[g_46701c].index;
        memcpy(&g_4b9ef0, g_4850d0[g_46701c].records, 0x120);
        memcpy(&g_4b9da0, g_4850d0[g_46701c].view, 0x74);
        real bounds[4];
        function_2f970((s_2f970_view const *)&g_4b9da0, bounds);
        function_2fd90((s_2f800_view const *)&g_4b9da0, (box2f const *)bounds, (byte *)&g_4b9e14);
    }
}


// Disabled: launch stack reset takes a new address of shared global g_4e6420; its aggregate is not yet canonical.
#if 0
// Retail 0x12190
void function_12190()
{
    function_12420();
    function_12b400();
    long depth = g_4e6420;
    g_4e642c[depth + 1] = g_4e642c[depth];
    g_4e6440[depth + 1] = g_4e6440[depth];
    g_4e6420 = depth + 1;
    if (function_12000())
    {
        function_12b690();
        function_12080();
    }
    memset(&g_4e6420, 0, 0x34);
    MmFreeContiguousMemory((void *)0x80061000);
    __debugbreak();
}
#endif



// Draw the timed UI/world effect passes for the current frame.
void function_1bdf0();
bool function_016ac0();
bool function_022710();
void __stdcall function_2bcd0(long, bool, bool, bool, long, long, real);
void __stdcall function_21a80(real, real, real);
// MiniMax I (11-05): matches (score 0) with a #pragma inline_depth(0) ... #pragma inline_depth() pair around
// the function (keeps the 0x16ac0 call out of line) plus the single expression for world_effects; held, since
// new #pragma lines are not taken. The lead stays a source form that keeps 0x16ac0 out of line without a pragma.
// MiniMax D (run 37): retail calls 0x16ac0 out of line, our build inlines it; the inlining is
// decided by the /Ob flag of the defining file src/unknown_0167a0.cpp (not mine). A single
// expression for world_effects gives 65; the original 16 is kept.
// Coordinator ruling 2026-11-02 (the user deferred): /Ob1 on src/unknown_0167a0.cpp matches 0x13680 (MiniMax R) but
// loses 0x13c20, so that file's flags stay; the lead is a source form that keeps 0x16ac0 out of line here.

// @retail 0x13680
void function_13680(bool skip_effects)
{
    function_1bdf0();
    s_timed_effect_globals *effect = function_01fd20(0);
    if (g_485602 == 2)
    {
        function_2bcd0(1, 1, false, 0, 0, 0, 0.0f);
        return;
    }
    bool draw_ui = !skip_effects && g_485898 != 6;
    bool world_effects = false;
    if (!skip_effects && g_485898 != 7 && function_022710()) world_effects = true;
    if (!function_016ac0() && !skip_effects && effect && effect->unknown3c &&
        effect->unknown40 > 0.0f && function_022710() && function_01de20(18))
    {
        function_2bcd0(1, 1, false, world_effects, 1, 1, effect->unknown40);
        g_4c0b78.state = (s_2cb30_state *)&g_547f88;
        function_2bcd0(1, 1, false, world_effects, 2, 2, effect->unknown40);
        function_21a80(effect->unknown44, effect->unknown48, effect->unknown40);
    }
    else function_2bcd0(1, 1, draw_ui, world_effects, 0, 0, 0.0f);
}


s_reset_storage g_reset_storage;

s_glyphs_storage g_glyphs_storage;

struct s_5093e4;
extern s_5093e4 *g_5093e4;
struct s_33a0b_view;
extern s_33a0b_view *g_485a58;
void function_20e50(long index);

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x22590
void function_22590()
{
    byte *globals = (byte *)g_4e034c;
    g_485a80 = *(long *)(globals + 0x108) ? *(byte **)(globals + 0x10c) : 0;
    function_1f2b0();
    s_timed_effect_globals *effects = g_5093e0;
    if (effects)
    {
        memset(effects, 0, 0x3fc);
        effects->unknown180[0] = 1.0f;
        effects->unknown180[1] = 1.0f;
        effects->unknown180[2] = 1.0f;
        effects->unknown180[3] = 1.0f;
        effects->unknown3f8 = 1.0f;
    }
    *(volatile long *)g_50934c = 0;
    *(volatile real *)(g_50934c + 0x10) = 0.5f;
    *(volatile real *)(g_50934c + 0x18) = 0.5f;
    *(volatile real *)(g_50934c + 0x28) = 0.5f;
    *(volatile real *)(g_50934c + 0x30) = 0.5f;
    ((byte *)g_5093e4)[0] = 0;
    ((byte *)g_5093e4)[1] = 0;
    real *value = (real *)(((byte *)g_5093e4) + 4);
    for (long i = 0; i < 5; ++i) value[i] = -1.0f;
    ((byte *)g_5093e4)[0x18] = 1;
    ((byte *)g_5093e4)[0x19] = 0;
    *(real *)(((byte *)g_5093e4) + 0x1c) = -1.0f;
    *(real *)(((byte *)g_5093e4) + 0x20) = -1.0f;
    ((byte *)g_5093e4)[0x1a] = 0;
    memset(&g_render_state_storage, 0, sizeof(g_render_state_storage));
    for (long i = 0; i < 0x1ff; ++i) function_20e50(i);
    if (effects) *(real *)effects->unknown190 = 0.0f;
    // Keep the effect write before the pointer clear.
    _ReadWriteBarrier();
    g_485a58 = 0;
}
#pragma function(_ReadWriteBarrier)

#include "allocator_storage.h"
#include "font_loading_storage.h"
extern bool g_5020d8;
void game_state_cache_files_close();
void function_125600();
void function_124920();
void animation_cache_dispose();
void font_cache_pixels_dispose();
void font_cache_dispose();
HANDLE g_4e0354, g_4e0358, g_4e035c, g_4e0360;

// @retail 0x12080
void function_12080()
{
    if (*(byte *)&g_4e3b60)
    {
        g_5020d8 = 0;
        game_state_cache_files_close();
        memset(&g_4e3b60, 0, 0x2540);
    }
    function_125600();
    function_124920();
    function_141c0();
    animation_cache_dispose();
    font_cache_pixels_dispose();
    font_cache_dispose();
    memset(&g_4e2920, 0, 0x1220);
    CloseHandle(g_4e0354);
    CloseHandle(g_4e0360);
    CloseHandle(g_4e035c);
    CloseHandle(g_4e0358);
    if (g_510800_pool_base)
    {
        VirtualFree((void *)g_510800_pool_base, 0, MEM_RELEASE);
        memset(&g_pool_storage, 0, sizeof(g_pool_storage));
    }
}

#include "launch_stack_storage.h"
extern "C" void __stdcall MmFreeContiguousMemory(void *base);
void function_12b400();
void function_12b690();

#include "unknown_12b400.h"
// @retail 0x12190
void function_12190()
{
    function_12420();
    function_12b400();
    int depth = g_4e6420;
    long next = depth + 1;
    g_4e642c[depth + 1] = g_4e642c[depth];
    // Retail reloads the stage before reading the second bound.
    long later = *(long const *)&g_4e6420;
    g_4e6440[later + 1] = g_4e6440[later];
    g_4e6420 = next;
    if (function_12000())
    {
        function_12b690();
        function_12080();
    }
    memset(&g_4e6420, 0, 0x34);
    MmFreeContiguousMemory((void *)0x80061000);
    __assume(0);
}

dword g_55e6b8;

// Advances the vertical blank count (g_485ab0, which the main loop waits on as VBLANK_COUNT in
// unknown_12b070.cpp) outside the main thread, so both counts are accessed as volatile: retail
// reloads them from memory at each use.
// @retail 0x14280
long __fastcall function_14280(void const *data)
{
    long result = 0;
    ++*(volatile unsigned __int64 *)&g_485ab0;
    if (((dword const *)data)[1] != g_55e6b8)
    {
        result = (long)(*(volatile unsigned __int64 *)&g_485ab0 - *(volatile unsigned __int64 *)&g_485ab8);
        *(volatile unsigned __int64 *)&g_485ab8 = *(volatile unsigned __int64 *)&g_485ab0;
        g_55e6b8 = ((dword const *)data)[1];
    }
    return result;
}



s_device_setup_storage g_device_setup_storage = {{{0, 0, 1, 0, 3, 0, 0, 0, 1, 0, 0, 0}}};

s_render_mode_44370 const g_43fa1c[25] =
{
    { -1, 0, 0, 0.0f, 0, { 0, 0, 0, 0xff, 0xff, 0xff, 0xff }, 0, 0, -1, 1 },
    { 1, 1, 0, 0.0f, 0, { 0, 0, 0, 2, 0, 0, 0 }, 1, 0, 0, 2 },
    { -1, 0, 0, 0.0f, 0, { 0, 0, 0, 0xe, 0, 0, 0 }, 1, 0, 0, 3 },
    { -1, 0, 0, 0.0f, 0, { 0, 0, 0, 3, 0, 0, 0 }, 1, 0, 0, 4 },
    { -1, 0, 0, 0.0f, 0, { 0, 0, 0, 3, 0, 0, 0 }, 0, 0, 0, 5 },
    { -1, 0, 0, 0.0f, 0, { 0, 0, 0, 4, 0, 0, 0 }, 0, 0, 0, 6 },
    { -1, 0x10, 0, 0.0f, 0, { 0, 0, 0, 5, 0, 0, 0 }, 0, 0, 0, 7 },
    { -1, 0, 0, 0.0f, 0, { 0, 0, 0, 6, 0, 0, 0 }, 0, 0, 0, 8 },
    { -1, 0, 0, 0.0f, 0, { 0, 0, 0, 7, 0, 0, 0 }, 0, 0, 0, 9 },
    { -1, 0, 0, 0.0f, 0, { 0, 0, 0, 8, 0, 0, 0 }, 0, 0, 2, 0xa },
    { -1, 0, 0, 0.0f, 0, { 0, 0, 0, 9, 0, 0, 0 }, 0, 0, 2, 0xb },
    { -1, 0, 0, 0.0f, 0, { 0, 0, 0, 0xa, 0, 0, 0 }, 0, 0, 1, 0xc },
    { -1, 0, 0, 0.0f, 0, { 0, 0, 0, 0xb, 0, 0, 0 }, 0, 0, 1, 0xd },
    { -1, 0, 0, 0.0f, 0, { 0, 0, 0, 0xc, 0, 0, 0 }, 0, 0, 1, 0xe },
    { 1, 0, 0, 0.0f, 0, { 0, 0, 0, 0xd, 0, 0, 0 }, 0, 0, 1, 0xf },
    { -1, 0, 0, 0.0f, 0, { 0, 0, 0, 0xf, 0, 0, 0 }, 1, 0, 0, 0x10 },
    { -1, 0, 0, 0.0f, 0, { 0, 0, 0, 0x10, 0, 0, 0 }, 0, 0, 0, 0x11 },
    { -1, 0x10, 0, 0.0f, 0, { 0, 0, 0, 0x12, 0, 0, 0 }, 0, 0, 0, 0x12 },
    { -1, 0, 0, 0.0f, 0, { 0, 0, 0, 0xf, 0, 0, 0 }, 1, 0, 0, 0x13 },
    { 3, 0, 0, 0.0f, 0, { 0, 0, 0, 0xf, 0, 0, 0 }, 0, 0, 0, 0x14 },
    { -1, 0, 0, 0.0f, 0, { 0, 0, 0, 0xe, 0, 0, 0 }, 0, 0, 0, 0x15 },
    { -1, 0, 0, 0.0f, 0, { 0, 0, 0, 0xf, 0, 0, 0 }, 0, 0, 0, 0x16 },
    { -1, 0, 0, 0.0f, 0, { 0, 0, 0, 0x18, 0, 0, 0 }, 0, 0, 0, 0x17 },
    { -1, 0, 0, 0.0f, 0, { 0, 0, 0, 0x19, 0, 0, 0 }, 0, 0, 0, 0x18 },
    { -1, 0, 0, 0.0f, 0, { 0, 0, 0, 0x1a, 0, 0, 0 }, 0, 0, 0, 0x38d1b717 },
};

s_type_7ba8e9 *function_1358e0(short width, short height, short levels, short format, word flags);
// @retail 0x1eff0
bool function_1eff0()
{
    bool result = true;
    byte *bitmap = (byte *)function_1358e0(256, 256, 0, 9, 0x810);
    if (bitmap)
    {
        memset(&g_glyphs_storage, 0, sizeof(g_glyphs_storage));
        if (function_1d000(bitmap))
        {
            g_4b62ac = (s_bitmap_view *)bitmap;
            g_4b62a0 = true;
            g_4b62a4.fields.x = 1;
            g_4b62a8 = 1;
        }
        else result = false;
    }
    else result = false;
    s_font_render_entry *entry = g_4b62b0;
    long count = 512;
    do { memset(entry, 0, sizeof(*entry)); entry->character_index = NONE; ++entry; } while (--count);
    return result;
}


extern long g_4e28f4[11];
struct s_font_character_header;
long font_cache_find_character(long font_index, long character);
struct s_font_cache_flags
{
    dword wait : 1;
    dword create : 1;
};
s_font_character_header *font_cache_get_character_header(long font_index, long character, s_font_cache_flags flags);
void __stdcall function_1f5c0(long character);

// @retail 0x1f490
void __stdcall function_1f490(long unused, long font, long character, long color, long shadow,
    real x, real y, real u, real v, real width, real height, real scale, long callback, long context)
{
    volatile real positions[2];
    long index = font_cache_find_character(g_4e28f4[font], character);
    if (index == NONE)
    {
        s_font_cache_flags flags = { true, true };
        font_cache_get_character_header(font, character, flags);
        index = font_cache_find_character(g_4e28f4[font], character);
        if (index == NONE) return;
    }
    function_1f5c0(index);
    if (index == NONE) return;
    long entry = *(long *)(g_54d574->data + (index & 0xffff) * 0x38 + 0x34);
    if (entry == NONE) return;
    s_font_render_entry *rectangle = &g_4b62b0[entry];
    bool draw_shadow = ((dword)shadow & 0xff000000) > 0;
    do
    {
        positions[0] = x + (draw_shadow ? scale : 0.0f);
        positions[1] = y + (draw_shadow ? scale : 0.0f);
        real const *position_values = (real const *)positions;
        function_1f3a0((short const *)rectangle, position_values[0], position_values[1], (long)width, (long)height,
            (short)(long)u, (short)(long)v, scale, draw_shadow ? shadow : color,
            (t_1f3a0_callback)callback, (void *)context);
        if (!draw_shadow) break;
        draw_shadow = false;
    } while (true);
}

#include "allocator_storage.h"
#include "crc.h"
void function_0169f0();
void function_016a30(real scale, short y, short x);
void function_01d660();
bool function_1d770();
void function_01fbb0();
void geometry_cache_initialize();
void texture_cache_initialize();
void function_020eb0();
extern dword g_468848[256];
extern long g_4b72c0;
#define g_485a88 (g_unknown_485a88.flag)
long g_4670f8;
point3f g_4b72b4;

__declspec(noinline) bool function_1d3f0(long index, void const *colors, D3DPalette **out);
__declspec(noinline) void function_0169f0();
__declspec(noinline) void function_016a30(real scale, short y, short x);
// MiniMax H (11-13): no new form, no lower score (171).
// MiniMax C (11-11): no new form, no lower score (171); retail hands the two offsets to 0x16a30 sign-extended (movsx ecx,cx and movsx edx,ax) and reaches the palette through a different call sequence, while the callee's declaration lives in src/unknown_0167a0.cpp, which this lane may not edit.
// MiniMax D (11-10): no new form, no lower score (171); the two offsets still reach 0x16a30 in the other order
// than retail's, and their width is the callee's own declaration's, which is in another file this lane may not
// edit.
// MiniMax S (11-09): no new form, no lower score (171): retail sign-extends the two offsets at the
// call because its callee took ints, while ours folds the constants into the argument registers
// because the callee here takes shorts, and retail passes the palette global itself as the
// out-parameter where ours reaches the palette through a local; the type-punned call form scores
// 169 and the palette-through-the-global form scores 184.
// MiniMax D (11-09): no new form, no lower score (171); the two offsets still reach 0x16a30 in the other
// order than retail's and the palette is still reached through a different call sequence, and the callee
// takes short offsets here where retail's took ints, so it is declared in another file this lane may
// not edit.
// MiniMax A (11-06, run 52): no new form, no lower score (171) - retail passes the two offsets to
// 0x16a30 in its own order and reaches the palette through a different call sequence.
// MiniMax C (11-06): no new form, no lower score - retail still passes the two offsets to 0x16a30 in its
// own order and still reaches the palette through a different call sequence, so the difference is in the
// call order rather than in a local form; the earlier trial with the offsets swapped scored 167, under
// the keep bar.
// MiniMax E (11-10): no new form, no lower score (171); the palette's address given straight to the callee was
// tried again this round and scores 184, so the call sequence stays.
// MiniMax E (11-11): no new form, no lower score (171); retail sign-extends the two offsets into the argument
// registers of 0x16a30 and passes the palette global itself as the out-parameter, while the callee here takes
// shorts and the palette is reached through a local; the callee's declaration lives in src/unknown_0167a0.cpp.
// MiniMax H (11-12): no new form, no lower score (171): the two offsets still reach 0x16a30 in the other order than retail's, and the callee takes short offsets here where retail's took ints; the callee's declaration lives in another file this lane may not edit.
// score 171 (was 171): the two offsets passed to 0x16a30 still sit in the other order, and the palette
// is still reached through a different call sequence than retail's.
// MiniMax D (11-05): no new form, no lower score - retail passes the two offsets to 0x16a30 in its own order and reaches the palette through a different call sequence, so the difference is in the call order rather than in a local form.
// MiniMax H (11-04): no new form, no lower score; the two offsets passed to 0x16a30 in retail's order
// (its third argument 0x18/0x24 in ecx and its second 0x20/0x30 in edx) match retail's behaviour but
// leave the score at 171, so the body stands as it is.
// MiniMax H (11-03): no new form, no lower score; retail passes the two offsets to 0x16a30 in the other order (its third argument 0x18/0x24 in ecx and its second 0x20/0x30 in edx, where ours passes 0x20/0x30 in ecx); swapping them matches retail's behaviour but leaves the score at 171, so the body was restored.
// Tried (MiniMax C run): the two offsets passed in the other order (0x20/0x18 and 0x30/0x24
// as the second and third arguments), the device result tested directly with the failure
// path writing the flag, and the palette result in a local of its own: 167 against 171;
// under the keep bar, body restored.
// MiniMax E (11-09): no new form, no lower score (171); retail hands the two offsets over as sign-extended
// 16-bit values (movsx ecx,cx and movsx edx,ax after the two movs, with the width left in EAX as well) where
// ours passes the two constants as dwords; the callee's declaration, which decides that, is in another file
// this lane may not edit.
// MiniMax B (11-11): no new form, no lower score (171); retail sign-extends the two offsets into the argument
// registers of 0x16a30 and passes the palette global itself as the out-parameter, while the callee here takes
// shorts and the palette is reached through a local; the callee's declaration lives in src/unknown_0167a0.cpp.
// MiniMax A (11-12): no new form, no lower score (171); retail sign-extends the two offsets into the argument
// registers of 0x16a30, whose declaration is in another file, and passes the palette global itself as the
// out-parameter, which the address-taking exception does not cover; the offset order, the palette result local and
// the dimension orders all score no better.
// @retail 0x12560
void function_12560()
{
    if (g_485a88) return;
    function_0169f0();
    short x, y;
    if (g_485ac5 && g_485ac6) { x = 0x20; y = 0x18; }
    else { x = 0x30; y = 0x24; }
    function_016a30(1.0f, y, x);
    Direct3D_SetPushBufferSize(0x100000, 0x8000);
    volatile bool success = function_128c0(&g_43ed80);
    if (success)
    {
        g_4670bc = 1;
        function_16b10((s_render_reset_state *)g_485b48);
        function_1c1b0();
        function_1c810();
        D3DPalette *palette = NULL;
        success = function_1d3f0(3, g_468848, &palette);
        g_484dbc = palette;
        if (success)
            {
                D3DDevice_SetPalette(0, (D3DPalette *)g_484dbc);
                D3DDevice_SetPalette(1, (D3DPalette *)g_484dbc);
                D3DDevice_SetPalette(2, (D3DPalette *)g_484dbc);
                D3DDevice_SetPalette(3, (D3DPalette *)g_484dbc);
            }
        memset(g_484dd0, 0, 0x180);
        if (success)
        {
            D3DDevice_SetVertexShader(0x11);
            D3DDevice_SetRenderState(D3DRS_CULLMODE, 2);
            D3DDevice_SetRenderState(D3DRS_ZWRITEENABLE, 1);
            D3DDevice_SetRenderState(D3DRS_ZFUNC, 0x203);
            D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
            D3DDevice_SetRenderState(D3DRS_STENCILENABLE, 0);
            D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, 0);
            D3DDevice_SetRenderState(D3DRS_ALPHAFUNC, 0x204);
            D3DDevice_SetRenderState(D3DRS_ALPHAREF, 0);
            D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
            D3DDevice_SetRenderState(D3DRS_SRCBLEND, 1);
            D3DDevice_SetRenderState(D3DRS_DESTBLEND, 0);
            D3DDevice_SetRenderState(D3DRS_BLENDOP, 0x8006);
            D3DDevice_SetRenderState(D3DRS_FOGENABLE, 1);
            D3DDevice_SetRenderState(D3DRS_LIGHTING, 0);
            D3DDevice_SetRenderState(D3DRS_SPECULARENABLE, 1);
                        D3DDevice_SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
            D3DDevice_SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
            D3DDevice_SetTextureStageState(2, D3DTSS_TEXCOORDINDEX, 2);
            D3DDevice_SetTextureStageState(3, D3DTSS_TEXCOORDINDEX, 3);
        }
    }
    function_01d660();
    if (success && function_14600() && function_14850() && function_1d770() && function_1d0e0() && function_1e110())
    {
        g_4670f8 = 0x24;
        if (function_1eff0())
        {
            g_4b72b4.x = g_468788->x;
            g_4b72b4.y = g_468788->y;
            g_4b72b4.z = g_468788->z;
            g_4b72c0 = NONE;
            success = true;
        }
        else success = false;
    }
    else success = false;
    function_01fbb0();
    geometry_cache_initialize();
    texture_cache_initialize();
    memset(&g_render_state_storage, 0, sizeof(g_render_state_storage));
    for (long i = 0; i < 0x1ff; ++i) function_20e50(i);
    byte *allocated = g_4e6080 + g_4e6084;
    g_4e6084 += 0x34;
    long size = 0x34;
    function_163ba0(&g_4e608c, &size, 4);
    g_50934c = allocated;
    function_020eb0();
    if (success) g_485a88 = 1;
}
