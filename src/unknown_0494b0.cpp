// @flags /O2 /Op /arch:SSE /Gr
#include "render_source_storage.h"
/* UNKNOWN_0494B0.CPP: rasterizer render state helpers */

#include "unknown_11c920.h"
#include "camera_storage.h"
#include <xtl.h>
#include "sampler_state.h"
#include "globals.h"

PRIVATE bool test_bit(dword flags, long bit)
{
	return (flags >> bit) & 1;
}

dword g_4b8344;
dword g_4b8340;
dword g_4b8464;
dword g_4b8450;
dword g_4b846c;
dword g_4b8448;
dword g_4b8308;
dword g_5093b4;
byte g_47fe84;
dword g_4b8348;
long g_4c1a14;
byte g_509416;
bool g_4aeb48;
extern long g_4858b8;
void function_14bc0(short index, short element, bool use_depth);

// @retail 0x35aa0
void function_35aa0(void)
{
    g_4c1a14 = 0;
    g_509416 = 0;
    function_14bc0((short)g_4858b8, 0, true);
    if (g_4aeb48)
    {
        __asm wbinvd
        g_4aeb48 = false;
    }
    g_4b8348 = 1;
    D3DDevice_SetRenderState(D3DRS_STIPPLEENABLE, 1);
}

dword g_4b82e8, g_4b82f4, g_4b82f8, g_4b8324;
dword const g_4506b8[12] = {32774,32774,32774,32774,32779,32775,32776,32774,32774,32774,0xffffffff,0xffffffff};
dword const g_4506e8[12] = {771,0,768,1,1,1,1,771,32770,32769,0xffffffff,0xffffffff};
dword const g_450718[12] = {770,774,774,1,1,1,1,1,32769,32770,0xffffffff,0xffffffff};

// @retail 0x142f0
void function_142f0(short mode)
{
    dword enabled = mode != 10;
    g_4b82e8 = enabled;
    D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, enabled);
    if (mode != 10)
    {
        dword source = g_450718[mode];
        g_4b82f4 = source;
        D3DDevice_SetRenderState(D3DRS_SRCBLEND, source);
        dword destination = g_4506e8[mode];
        g_4b82f8 = destination;
        D3DDevice_SetRenderState(D3DRS_DESTBLEND, destination);
        dword operation = g_4506b8[mode];
        g_4b8324 = operation;
        D3DDevice_SetRenderState(D3DRS_BLENDOP, operation);
    }
}

#pragma auto_inline(off)
// @retail 0x48010
void function_48010(real const *color)
{
    D3DDevice_SetVertexData4f(9, color[1], color[2], color[3], color[0]);
}
#pragma auto_inline(on)


dword g_4b8334;
dword g_4b8330;


real g_4c9798[36];
byte g_4b5144;
dword g_4b50c4;
dword g_4b50d8;
dword g_4b5130;
dword g_4b5134;

PRIVATE real normalize(real value, real lower, real upper)
{
	return (value - lower) / (upper - lower);
}

// @retail 0x494b0
void function_0494b0(real const *bounds, real const *window)
{
	real width = bounds[1] - bounds[0];
	real left = 1.0f - (window[0] - bounds[0]) / width;
	real right = 1.0f - (window[1] - bounds[0]) / width;
	real height = bounds[3] - bounds[2];
	real top = (window[2] - bounds[2]) / height;
	real bottom = (window[3] - bounds[2]) / height;
	real scale_x = 1.0f / (right - left);
	real scale_y = 1.0f / (bottom - top);

	g_4c9798[8] = 512.0f;
	g_4c9798[9] = 512.0f;
	g_4c9798[32] = scale_x;
	g_4c9798[33] = scale_y;
	g_4b50d8 = g_4b50c4 = g_4b5144 ? 0x1f1ff1ff : 0;
	g_4b5134 = g_4b5130 = 0x200;
	g_4c9798[10] = 16777215.0f;
	g_4c9798[11] = 1.0f;
	g_4c9798[34] = scale_x * left * -512.0f;
	g_4c9798[35] = scale_y * top * -512.0f;
	g_4ba014 |= 0x48;
	D3DDevice_SetVertexShaderConstantFast(-56, g_4c9798, 9);
}

// @retail 0x495e0
void function_0495e0(void)
{
	dword value = test_bit(g_4ba014, 4) ? 16 : 1;
	g_4b8344 = value;
	D3DDevice_SetRenderState(D3DRS_DEPTHCLIPCONTROL, value);
	value = g_47fe84 ? 2 : 0;
	g_4b8464 = value;
	D3DDevice_SetRenderState(D3DRS_MULTISAMPLEMODE, value);
	g_4b8340 = 1;
	D3DDevice_SetRenderState(D3DRS_SOLIDOFFSETENABLE, 1);
	real z_offset = 550.0f;
	g_4b8334 = *(dword *)&z_offset;
	D3DDevice_SetRenderState(D3DRS_POLYGONOFFSETZOFFSET, *(dword *)&z_offset);
	real z_slope = 2.0f;
	g_4b8330 = *(dword *)&z_slope;
	D3DDevice_SetRenderState(D3DRS_POLYGONOFFSETZSLOPESCALE, *(dword *)&z_slope);
}

// @retail 0x496a0
void function_0496a0(void)
{
	g_4b8344 = 0;
	D3DDevice_SetRenderState(D3DRS_DEPTHCLIPCONTROL, 0);
	g_4b8340 = 0;
	D3DDevice_SetRenderState(D3DRS_SOLIDOFFSETENABLE, 0);
	g_4b8464 = 0;
	D3DDevice_SetRenderState(D3DRS_MULTISAMPLEMODE, 0);
	g_4b8450 = 0;
	D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
}

// @retail 0x496f0
void function_0496f0(void)
{
	g_4b846c = 0x204;
	D3DDevice_SetRenderState(D3DRS_SHADOWFUNC, 0x204);
	g_4b8344 = 0x10;
	D3DDevice_SetRenderState(D3DRS_DEPTHCLIPCONTROL, 0x10);
	g_4b8448 = g_5093b4;
	D3DDevice_SetRenderState(D3DRS_CULLMODE, g_5093b4);
}

// @retail 0x49740
void function_049740(void)
{
	g_4b846c = 0x200;
	D3DDevice_SetRenderState(D3DRS_SHADOWFUNC, 0x200);
	g_4b8344 = 0;
	D3DDevice_SetRenderState(D3DRS_DEPTHCLIPCONTROL, 0);
	g_4b8308 = 0x1010101;
	D3DDevice_SetRenderState(D3DRS_COLORWRITEENABLE, 0x1010101);
}

struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;
real g_485b28[7];

// @retail 0x1beb0
void function_1beb0(void)
{
    real height = (real)(g_4b9dd4 - g_4b9dd0);
    g_485b28[0] = 1.0f / height;
    g_485b28[1] = 0.0f / height;
    g_485b28[2] = 0.0f / height;
    g_485b28[3] = 2.0f / height;
    g_485b28[4] = 4.0f / height;
    g_485b28[5] = 0.0f / height;
    g_485b28[6] = 1.0f / height;
    if (g_510c50 && ((byte *)g_510c50)[5])
        g_485b28[6] *= 2.0f;
}

// @retail 0x47ea0
void function_47ea0(short stage, byte flags)
{
	byte const *flags_reference = &flags;
	if (*flags_reference & 1)
	{
		sampler_state_write(stage, D3DTSS_ADDRESSU, D3DTADDRESS_BORDER);
		sampler_state_write(stage, D3DTSS_ADDRESSV, D3DTADDRESS_BORDER);
	}
	else
	{
		sampler_state_write(stage, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		sampler_state_write(stage, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
	}
	if (*flags_reference & 2)
	{
		sampler_state_write(stage, D3DTSS_MAGFILTER, D3DTEXF_POINT);
		sampler_state_write(stage, D3DTSS_MINFILTER, D3DTEXF_POINT);
		sampler_state_write(stage, D3DTSS_MIPFILTER, D3DTEXF_POINT);
	}
	else
	{
		sampler_state_write(stage, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		sampler_state_write(stage, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		sampler_state_write(stage, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
	}
	sampler_state_write(stage, D3DTSS_MAXANISOTROPY, 0);
	sampler_state_write(stage, D3DTSS_MIPMAPLODBIAS, 0);
	sampler_state_write(stage, D3DTSS_MAXMIPLEVEL, 0);
	sampler_state_write(stage, D3DTSS_COLORSIGN, 0);
	sampler_state_write(stage, D3DTSS_ALPHAKILL, 0);
}

extern byte *g_485a80;
bool function_14560(long tag, short stage, long fallback, short fallback_index, short index);
bool function_14480(long tag, short stage, short index);

/* sets a texture stage's bitmap from the tag (or the default one), then its
   sampler states unless the bitmap call reports a skip */
// score 7 (was 7): the only difference is the call to 0x14560: retail moves
// the stage into ecx and passes the flags on the stack (ret 0xc), ours pushes
// the stage and passes four stack arguments (ret 0x10); 0x14560 lives in
// src/unknown_01cf50.cpp, which this lane may not edit (BLOCKED-BY 0x14560)
// Round 19: our 0x14560 reads the stage from its stack slot just before the 0x14390 call, where
// retail copies it into ESI at entry, so LTCG leaves it on the stack. Caller forms leave 0x14560
// unchanged: the calls in an if/else 7; the stage read through its address 24; a false-preset result
// assigned in the if/else 7; an early return on success 24; the NONE test first in the if/else 11.
// Round 70: nested success tests 41; nested failure tests 20; an inverted skip result 29;
// the sampler call in both arms 23; a widened stage local 24; a widened bitmap-index local 25;
// the default tag by array indexing 7; the ternary tested into a false-preset result 31.
// @retail 0x47fd0
bool function_47fd0(long tag, short stage, short index, byte flags)
{
    bool result = tag != NONE ? function_14560(tag, stage, 0, 1, index) :
        function_14480(*(long *)(g_485a80 + 0x34), stage, index);
    if (!result)
        function_47ea0(stage, flags);
    return result;
}

real g_485a34, g_485a40;
byte g_485a47;
real g_485a0c[8];
