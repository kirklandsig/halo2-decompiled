// @flags /O2 /arch:SSE /Gr
#include <string.h>

// @retail 0x52020
bool __stdcall function_52020(void *destination, long unknown, void const *source)
{
    memcpy(destination, source, 80);
    return true;
}

// Retail analysis: docs/unknown_052040.md; shared layout with function_35b90.
#include "unknown_11c920.h"
#include "globals.h"
#include <xtl.h>
#include <string.h>

struct s_shader_cache;
struct s_bitmap_view;
extern short g_485602;
extern word g_485648, g_48564a, g_48564c, g_48564e;
extern byte *g_485a80;
extern byte g_51f0f0[0x2d8];
extern IDirect3DBaseTexture8 *g_51f3c8[2][4];
extern long g_485af4[4], g_485b04[4];
extern D3DPIXELSHADERDEF g_484f68;
void function_0222d0(D3DRENDERSTATETYPE state, dword value);
void function_142f0(short mode);
D3DTexture *function_12310(s_bitmap_view *bitmap, real priority);
void function_1c590(s_shader_cache *state, long tag, long index);
void function_1ccb0(long format);
void function_1c6b0(void *state);
extern byte const g_43f408[63][21];
void __stdcall function_1c710(void *state);
void function_1cf50(void);
dword __cdecl pack_color4f(color4f const *color);
bool __stdcall function_52020(void *destination, long unknown, void const *source);
typedef void (__stdcall *t_4b220_fill)(void *, long, void *);
long function_4b220(long count, long mode, long primitive, long stride, t_4b220_fill fill, void *context);

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
