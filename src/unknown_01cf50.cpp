// @flags /O2 /Ob1 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "index_cache_storage.h"
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
extern byte g_4670bc, g_485b48[];
extern long g_4858b8;
extern dword g_4b8448, g_4b8308, g_4b82e0, g_4b8438, g_4b8450;
extern dword g_4b82fc, g_4b843c, g_4b82e8, g_4b82ec, g_4b82f4;
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
extern byte *g_4858c4;
extern byte g_485a76, g_485607;
extern IDirect3DBaseTexture8 *g_51f3c8[2][4];
struct s_type_7ba8e9;
s_type_7ba8e9 *function_137550(long tag, short bitmap);
bool function_14390(short stage, s_type_7ba8e9 *bitmap, real priority);
void function_144f0(long tag, short stage, long fallback, short fallback_index, short index, real priority);
bool function_01dd60(long index, long *width, long *height);
long function_25960(void);
struct s_shader_cache;
extern byte g_51f0f0[0x2d8];
void function_1c590(s_shader_cache *state, long tag, long index);
void __stdcall function_1c710(void *state);

// @retail 0x46230
void __stdcall function_46230(short target, real const *bounds)
{
    function_14f60(0, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, 3);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, 3);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER, 2);
    D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER, 2);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPFILTER, 1);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);
    function_143c0(*(long *)(g_485a80 + 0x34), 0, 1, 0.0f);
    D3DDevice_SetTextureStageState(1, D3DTSS_ADDRESSU, 3);
    D3DDevice_SetTextureStageState(1, D3DTSS_ADDRESSV, 3);
    D3DDevice_SetTextureStageState(1, D3DTSS_MAGFILTER, 2);
    D3DDevice_SetTextureStageState(1, D3DTSS_MINFILTER, 2);
    D3DDevice_SetTextureStageState(1, D3DTSS_MIPFILTER, 2);
    D3DDevice_SetTextureStageState(1, D3DTSS_MAXANISOTROPY, 0);
    D3DDevice_SetTextureStageState(1, D3DTSS_MIPMAPLODBIAS, 0);
    D3DDevice_SetTextureStageState(1, D3DTSS_MAXMIPLEVEL, 0);
    D3DDevice_SetTextureStageState(1, D3DTSS_COLORSIGN, 0);
    D3DDevice_SetTextureStageState(1, D3DTSS_ALPHAKILL, 0);
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
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    D3DDevice_SetVertexShaderConstant(18, constants, 8);
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
    D3DDevice_SetVertexData2s(3, 1, 1);
    D3DDevice_SetVertexData4f(0, 64.53125f, 64.53125f, 16777215.0f, 16777215.0f);
    D3DDevice_SetVertexData2s(3, 0, 1);
    D3DDevice_SetVertexData4f(0, 0.53125f, 64.53125f, 16777215.0f, 16777215.0f);
    D3DDevice_End();
    function_14bc0((short)g_4858b8, 0, true);
}

// @retail 0x32ac0
bool function_32ac0(void)
{
    bool volatile result = false;
    if (function_14480(*(long *)(g_485a80 + 0x34), 0, 0))
        return result;
    g_4670bc = true;
    function_16b10((s_render_reset_state *)g_485b48);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, 3);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, 3);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER, 2);
    D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER, 2);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPFILTER, 2);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);
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
    function_0222d0(D3DRS_DESTBLEND, 0x303);
    function_0222d0(D3DRS_BLENDOP, 0x8006);
    function_1cc30(23);
    return true;
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
    long width = 0, height = 0;
    long count;
    if (g_4b4b58[index].data && !g_4b4b58[index].flag95)
        count = g_4b4b58[index].element_count;
    else count = index == 0 || index == 3 ? 1 : 0;
    function_01dd60(index, &width, &height);
    for (long level = 0; level < count; ++level)
    {
        real upper = g_485ad4.hi;
        real lower = g_485ad4.lo;
        real y = (real)height;
        real x = (real)width;
        real ratio = upper / (upper - lower);
        real z = (ratio - lower * ratio) * 16777215.0f;
        if (z < 0.0f) z = 0.0f;
        else if (z > 16777215.0f) z = 16777215.0f;
        real w = (1.0f / upper) * 16777215.0f;
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

// @retail 0x1d3f0
bool function_1d3f0(long index, void const *colors, D3DPalette **out)
{
	D3DPalette *palette = NULL;
	bool result = function_015b10(index, &palette);
	if (result && colors)
	{
		D3DCOLOR *destination = D3DPalette_Lock2(palette, 0);
		if (destination)
			memcpy(destination, colors, g_43e8e8[index] * sizeof(D3DCOLOR));
		D3DPalette_Unlock(palette);
	}
	*out = palette;
	return result;
}

// @retail 0x15780
void function_15780(long stage, long mode)
{
	D3DDevice_SetTextureStageState(stage, D3DTSS_MAXANISOTROPY, 0);
	D3DDevice_SetTextureStageState(stage, D3DTSS_MIPMAPLODBIAS, 0);
	D3DDevice_SetTextureStageState(stage, D3DTSS_MAXMIPLEVEL, 0);
	D3DDevice_SetTextureStageState(stage, D3DTSS_COLORSIGN, 0);
	D3DDevice_SetTextureStageState(stage, D3DTSS_ALPHAKILL, 0);
	switch (mode)
	{
	case 0:
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSU, 1);
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSV, 1);
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSW, 1);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MAGFILTER, 2);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MINFILTER, 2);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MIPFILTER, 2);
		break;
	case 1: case 5:
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSU, 3);
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSV, 3);
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSW, 3);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MAGFILTER, 2);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MINFILTER, 2);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MIPFILTER, 2);
		break;
	case 2: case 6:
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSU, 4);
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSV, 4);
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSW, 4);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MAGFILTER, 2);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MINFILTER, 2);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MIPFILTER, 2);
		break;
	case 3: case 7:
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSU, 3);
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSV, 3);
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSW, 3);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MAGFILTER, 1);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MINFILTER, 1);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MIPFILTER, 0);
		break;
	case 4:
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSU, 3);
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSV, 3);
		D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSW, 3);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MAGFILTER, 2);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MINFILTER, 2);
		D3DDevice_SetTextureStageState(stage, D3DTSS_MIPFILTER, 0);
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
bool function_0158f0(byte linear, short format, long width, long height,
	long levels, long usage_index, D3DTexture **out);
bool function_0159b0(long edge, short format, long levels, long usage_index,
	D3DCubeTexture **out);
bool function_015a60(short format, long width, long height, long depth,
	long levels, long usage_index, D3DVolumeTexture **out);

// @retail 0x1d000
bool function_1d000(byte *bitmap)
{
	short width = *(short *)(bitmap + 4);
	volatile short height = *(short *)(bitmap + 6);
	char depth = *(char *)(bitmap + 8);
	short format = *(short *)(bitmap + 0xc);
	bool linear = (bitmap[0xe] & 0x10) != 0;
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
			return function_0158f0(linear, format, width, height, levels, 0,
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
IDirect3DBaseTexture8 *g_51f3c8[2][4];
long g_5093d8;
dword g_487288[17][32];
D3DSurface *g_51f3f4;
D3DSurface *g_51f3f8;
byte g_51f3fc;

extern D3DResource *g_509374, *g_509378, *g_50937c, *g_509380;
D3DTexture *g_509354[2];
D3DSurface *g_50935c, *g_509360, *g_509364, *g_509370;
D3DTexture *g_509368, *g_50936c;
D3DSurface *g_509384;
D3DTexture *g_50938c;
D3DSurface *g_509390, *g_509394, *g_509398, *g_50939c, *g_5093a0, *g_5093a4, *g_5093a8;
short g_485602;

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
void function_36880(color4f const *color, s_short_rectangle const *rectangle)
{
	(void)&rectangle;
	function_1cf50();
	D3DDevice::Begin(D3DPT_TRIANGLEFAN);
	D3DDevice_SetVertexData4f(9, color->red, color->green, color->blue, color->alpha);
	for (short i = 0; i < 4; ++i)
	{
		bool top = i < 2;
		bool right = i == 1 || i == 2;
		short y = top ? rectangle->top : rectangle->bottom;
		short x = right ? rectangle->right : rectangle->left;
		D3DDevice_SetVertexData2f(0, (real)x, (real)y);
	}
	D3DDevice::End();
}

// @retail 0x14980
void function_14980(void)
{
	if (g_50935c)
	{
		D3DResource_Release(g_50935c);
		g_50935c = NULL;
	}
	if (g_509360)
	{
		D3DResource_Release(g_509360);
		g_509360 = NULL;
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
extern byte g_485607, g_5093fc;
extern word g_485648, g_48564a, g_48564c, g_48564e;
extern __int64 g_485aa0;
long g_485af4[4], g_485b04[4];

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
    long slot = stage;
    if (bitmap)
    {
        g_51f3c8[1][slot] = function_12310((s_bitmap_view *)bitmap, priority);
        g_485af4[slot] = *(short *)((byte *)bitmap + 4);
        g_485b04[slot] = *(short *)((byte *)bitmap + 6);
    }
    return true;
}

// @retail 0x14480
bool function_14480(long tag, short stage, short index)
{
    bool result = false;
    if (tag != NONE)
    {
        byte *group = g_4e3b44[tag & 0xffff].bytes;
        long count = *(long *)(group + 0x44);
        if (count > 0)
        {
            s_type_7ba8e9 *bitmap = function_137550(tag, (short)(index % count));
            if (bitmap)
            {
                if (function_12360((s_bitmap_view *)bitmap, 0.0f))
                    function_14390(stage, bitmap, 0.0f);
                else result = true;
            }
        }
    }
    return result;
}

// @retail 0x144f0
void function_144f0(long tag, short stage, long fallback, short fallback_index, short index, real priority)
{
    s_type_7ba8e9 *bitmap;
    if (tag != NONE)
    {
        byte *group = g_4e3b44[tag & 0xffff].bytes;
        long count = *(long *)(group + 0x44);
        if (count > 0)
        {
            bitmap = function_137550(tag, (short)(index % count));
            if ((long)*(short *)((byte *)bitmap + 0xa) == (long)fallback)
                goto selected;
        }
    }
    tag = *(long *)(g_485a80 + fallback * 8 + 0x64);
    if (tag == NONE) return;
    bitmap = function_137550(tag, fallback_index);
    if (!bitmap) return;
selected:
    function_14390(stage, bitmap, priority);
}

short g_55ece0, g_55ece2;

// @retail 0x14560
bool function_14560(long tag, short stage, short fallback, short fallback_index, short index)
{
    volatile bool result = false;
    s_type_7ba8e9 *bitmap = NULL;
    if (tag != NONE)
    {
        byte *group = g_4e3b44[tag & 0xffff].bytes;
        volatile long count = *(long *)(group + 0x44);
        if (count > 0)
        {
            bitmap = function_137550(tag, (short)(index % count));
            if (!function_12360((s_bitmap_view *)bitmap, 0.0f))
                result = true;
        }
    }
    if (!result)
    {
        if (!bitmap || *(short *)((byte *)bitmap + 0xa) != fallback)
        {
            tag = *(long *)(g_485a80 + fallback * 8 + 0x64);
            if (tag == NONE) return result;
            bitmap = function_137550(tag, fallback_index);
        }
        if (bitmap)
        {
            function_14390(stage, bitmap, 0.0f);
            g_55ece0 = *(short *)((byte *)bitmap + 4);
            g_55ece2 = *(short *)((byte *)bitmap + 6);
        }
    }
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
    long slot = stage;
    if (tag == NONE)
    {
        tag = *(long *)(g_485a80 + 0x64);
        index = 0;
        if (tag == NONE) return result;
    }
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
        s_type_7ba8e9 *bitmap = function_137550(tag, index);
        if (bitmap)
        {
            g_51f3c8[1][slot] = function_12310((s_bitmap_view *)bitmap, priority);
            g_485af4[slot] = *(short *)((byte *)bitmap + 4);
            g_485b04[slot] = *(short *)((byte *)bitmap + 6);
            return true;
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
	case 0: target = g_50935c; depth = g_509364; flag = true; break;
	case 4: target = g_509360; flag = true; break;
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
	long width = 0, height = 0;
	function_01dd60(index, &width, &height);
	g_485af4[stage] = width;
	g_485b04[stage] = height;
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
	g_51f3c8[1][stage] = texture;
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
					g_487288[count][bit / 32] |= 1 << (bit % 32);
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

// @retail 0x1cb20
long function_1cb20(long mode, dword index, long tag)
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
void __cdecl function_1c4a0(s_shader_cache *state)
{
	state->streams_changed = false;
	state->descriptors_changed = false;
	state->stream_count = 0;
	for (long i = 0; i < 8; i++)
	{
		state->bindings[i].field_0c = NONE;
		state->bindings[i].tag = NONE;
		state->bindings[i].index = NONE;
	}
	state->active = NONE;
	for (long j = 0; j < 16; j++)
		state->wanted[j] = NULL;
	for (long k = 0; k < 16; k++)
	{
		state->streams[k].field_08 = 0;
		state->streams[k].field_04 = 0;
		state->streams[k].field_00 = 0;
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
void function_1c620(s_shader_cache *state, long a, long b, long c, byte const *descriptor)
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

extern byte g_51f0f0[0x2d8];
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
dword function_1cbe0(long mode, dword index, long slot_index)
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

byte g_51f3f0;
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
    x = g_485adc;
    y = g_485ae0;
    real const *near_plane = &x;
    real const *far_plane = &y;
    D3DDevice_SetDepthClipPlanes(*near_plane, *far_plane, D3DSDCP_SET_VERTEXPROGRAM_PLANES);
}

byte g_4b6290;
extern dword g_4850c8;

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

byte g_485b48[0x1fc0];
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

struct s_frame_offset
{
    point3f position;
    vector3f forward;
    vector3f up;
};
extern s_frame_offset g_485618;
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
extern long g_4c1bd0;

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

extern point3f g_4b9da0;
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
	long comparison = a->type - b->type;
	if (!comparison)
	{
		if (a->active)
		{
			real point = b->position.z * a->plane.k;
			point += b->position.y * a->plane.j;
			point += b->position.x * a->plane.i;
			// Retail rereads these record fields in camera-dot-product order.
			plane3f const volatile *camera_plane = &a->plane;
			real camera = camera_plane->k * g_4b9da0.z;
			camera = camera_plane->j * g_4b9da0.y + camera;
			camera = camera_plane->i * g_4b9da0.x + camera;
			long camera_side = a->plane.d > camera ? 1 : 0;
			long point_side = a->plane.d > point ? 1 : 0;
			comparison = camera_side != point_side ? 1 : -1;
		}
		else if (b->active)
		{
			real point = a->position.z * b->plane.k;
			point += a->position.y * b->plane.j;
			point += a->position.x * b->plane.i;
			plane3f const volatile *camera_plane = &b->plane;
			real camera = camera_plane->k * g_4b9da0.z;
			camera = camera_plane->j * g_4b9da0.y + camera;
			camera = camera_plane->i * g_4b9da0.x + camera;
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

// @retail 0x1e370
void function_1e370(void)
{
	long scratch;
	function_35aa0();
	long first = g_4b6280.unknown04;
	word *indices = (word *)g_4b6280.buffer;
	for (long i = first; i < g_4b6280.count; ++i)
		indices[i - first] = (word)i;
	sort_2byte(indices, g_4b6280.count - first, &scratch, function_1e700, g_4b6280.records);
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
		D3DDevice_SetStipple(function_1c290(1.0f));
		g_4b8494 = 1.0f;
	}
}

// @retail 0x1e4e0
bool function_1e4e0(point3f const *position, t_1e4e0_callback callback, void const *data, long size)
{
	(void)&callback;
	(void)&data;
	bool result = false;
	if (g_4b6280.count < 1024)
	{
		g_4b6280.records[g_4b6280.count].count = 0;
		s_1e4e0_record *record = (s_1e4e0_record *)&g_4b6280.records[g_4b6280.count];
		record->active = false;
		vector3f delta;
		delta.i = position->x - g_485618.position.x;
		delta.j = position->y - g_485618.position.y;
		delta.k = position->z - g_485618.position.z;
		++g_4b6280.count;
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
	else if (g_4b6280.available)
		g_4b6280.available = false;
	return result;
}


void __stdcall function_35b00(void *payload);

// @retail 0x1e5e0
bool function_1e5e0(point3f const *position, long size, long a, long b, long c,
	long d, long e, long f, long g, void const *data)
{
	(void)&a; (void)&b; (void)&c; (void)&d;
	(void)&e; (void)&f; (void)&g; (void)&data;
	bool result = false;
	if (g_4b6280.count < 1024)
	{
		s_1e4e0_record *record = (s_1e4e0_record *)&g_4b6280.records[g_4b6280.count++];
		record->type = 0;
		record->active = false;
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
		delta.k = position->z - g_485618.position.z;
		delta.j = position->y - g_485618.position.y;
		delta.i = position->x - g_485618.position.x;
		real depth = g_485618.forward.k * delta.k;
		depth += g_485618.forward.j * delta.j;
		depth += g_485618.forward.i * delta.i;
		record->depth = 0.0f - depth;
		record->position = *position;
		result = true;
	}
	else if (g_4b6280.available)
		g_4b6280.available = false;
	return result;
}

DWORD const g_43fdc0[5] = {1, 2, 5, 8, 6};
bool g_47fe85 = true;
typedef void (__stdcall *t_4b220_fill)(void *, long, void *);

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
		long bytes = count * stride;
		dword words = (dword)bytes >> 2;
		long requested = words + 5;
		if (requested < 2048)
		{
			function_1cf50();
			DWORD *push = D3DDevice_BeginPush(requested);
			*push++ = D3DPUSH_ENCODE(D3DPUSH_SET_BEGIN_END, 1);
			*push++ = g_43fdc0[primitive];
			*push++ = D3DPUSH_ENCODE(D3DPUSH_INLINE_ARRAY | D3DPUSH_NOINCREMENT_FLAG, words);
			fill(push, bytes, context);
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
		D3DDevice_SetVertexShaderConstant(-46, constants, 3);
		g_5093d8 = 0;
	}
	else if (record->handle != NONE)
	{
		byte *data = (byte *)table_entry_data(record->handle);
		long size = 0;
		if (*(word *)data > 0)
			size = *(word *)data * 48 + 0x44;
		for (long i = size / 32; i > 0; --i, data += 32)
			_mm_prefetch((char const *)data, _MM_HINT_T0);
		dword flags = record->flags;
		if ((flags & 0xe0000000) == 0x20000000)
		{
			long index = function_4cb80((flags >> 9) & 0x1ff, record->tag);
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
					D3DDevice_SetVertexShaderConstant(-46, constants, 3);
					g_5093d8 = 0;
					return;
				}
			}
		}
		function_152a0(record->handle, flags & 15);
	}
}

real g_4b9fa4, g_4b9ff8, g_4b9f18, g_4b9f9c;

PRIVATE __forceinline real maximum_25ca0(real first, real second)
{
    real result = second;
    if (first > second)
        result = first;
    return result;
}

// @retail 0x25ca0
void function_25ca0(bool enabled)
{
    volatile real divisor = maximum_25ca0(0.0001f, g_4b9fa4);
    real value = g_4b9ff8;
    value *= 1.0f / divisor;
    value = 0.0f - value;
    real constants[4];
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
extern vector3f g_4b9dac;

struct s_42760_entry
{
    real depth;
    point3f position;
    byte unknown10[0x10];
    long type;
};
s_42760_entry g_4c152c[32];
long g_4c19ac;

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x42760
void function_42760(long flags)
{
    volatile long count = g_4c19ac;
    long i = 0;
    if (count > 0)
    {
      do
      {
        s_42760_entry *entry = &g_4c152c[i];
        if (entry->type == 3)
        {
            if (g_4b6280.count < 1024)
            {
                s_1e4e0_record *record = (s_1e4e0_record *)&g_4b6280.records[g_4b6280.count];
                record->type = 0;
                record->active = false;
                ++g_4b6280.count;
                _ReadWriteBarrier();
                record->type = 3;
                record->flags = 0;
                record->callback = function_423c0;
                real *depth = &record->depth;
                point3f *position = &record->position;
                if (depth) *depth = entry->depth;
                if (position) *position = entry->position;
                ((long *)record->payload)[0] = i;
                ((long *)record->payload)[1] = flags;
                *depth = 0.0f - *depth;
            }
            else if (g_4b6280.available)
                g_4b6280.available = false;
        }
        ++i;
      } while (i < count);
    }
}
#pragma function(_ReadWriteBarrier)

struct s_42850_payload
{
    long a, b;
    point3f position;
    vector3f first, second;
    real scale, width;
    vector3f third;
};

#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x42850
void function_42850(long a, long b, point3f const *position,
    vector3f const *first, vector3f const *second, real scale, real width,
    vector3f const *third)
{
    if (g_4b6280.count < 1024)
    {
        s_1e4e0_record *record = (s_1e4e0_record *)&g_4b6280.records[g_4b6280.count++];
        record->type = 0;
        record->active = false;
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
        _ReadWriteBarrier();
    }
    else if (g_4b6280.available)
        g_4b6280.available = false;
}
#pragma function(_ReadWriteBarrier)


struct s_429a0_payload
{
    byte type, opacity;
    byte unknown02[2];
    long a, b, c;
    point3f position, endpoint;
    vector3f first, second;
};

// @retail 0x429a0
void function_429a0(byte type, long a, long b, long c, point3f const *position,
    point3f const *endpoint, vector3f const *first, vector3f const *second, real opacity)
{
    if (g_4b6280.count >= 1024)
    {
        if (g_4b6280.available) g_4b6280.available = false;
        return;
    }
    {
        s_1e4e0_record *record = (s_1e4e0_record *)&g_4b6280.records[g_4b6280.count];
        record->type = 0;
        record->active = false;
        s_429a0_payload *payload = (s_429a0_payload *)record->payload;
        payload->type = type;
        payload->a = a;
        payload->b = b;
        payload->c = c;
        ++g_4b6280.count;
        payload->position = *position;
        payload->endpoint = *(endpoint ? endpoint : position);
        payload->first = *first;
        payload->second = *second;
        long value = (long)(opacity * 256.0f);
        long clamped = 0;
        if (value >= 0)
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

// @retail 0x1c8c0
bool function_1c8c0(byte const *definition, dword mask,
    s_1c8c0_stream const *third, s_1c8c0_stream const *second, s_1c8c0_stream const *first)
{
    long count = 0;
    if (*(long const *)(definition + 0x38) > 0)
    {
      short i = 0;
      do
      {
        s_stream_description const *stream = (s_stream_description const *)(*(byte *const *)(definition + 0x3c) + i * 32);
        mask &= ~stream_register_mask(g_43f408[stream->format]);
        function_1c620((s_shader_cache *)g_51f0f0, stream->buffer, stream->stride, stream->offset, g_43f408[stream->format]);
        ++count;
        ++i;
      } while (i < *(long const *)(definition + 0x38));
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
    signed char index;
    switch (*kind)
    {
    case 1: index = (signed char)selection[0x10]; break;
    case 2: index = (signed char)selection[0x10]; break;
    case 3: index = (signed char)selection[0x10]; break;
    case 4: index = (signed char)selection[0x11]; break;
    case 5: index = (signed char)selection[0x11]; break;
    default: index = (signed char)selection[0x10]; break;
    }
    long mode = *(word const *)(selection + 0x14);
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
    parameters.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    parameters.Windowed = false;
    parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    parameters.EnableAutoDepthStencil = true;
    parameters.AutoDepthStencilFormat = D3DFMT_D24S8;
    parameters.BackBufferFormat = D3DFMT_A8R8G8B8;
    parameters.FullScreen_RefreshRateInHz = (short)g_485ac0;
    parameters.BackBufferWidth = 640;
    parameters.BackBufferHeight = 480;
    parameters.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    if (g_485ac2)
        parameters.Flags |= D3DPRESENTFLAG_WIDESCREEN;
    if (g_485ac6)
        parameters.Flags |= D3DPRESENTFLAG_PROGRESSIVE;
    D3DDevice *device = NULL;
    Direct3D_CreateDevice(0, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &parameters, &device);
    g_5093b0 = device;
    if (!g_5093b0)
    {
        g_5093b0 = NULL;
        return false;
    }
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
    volatile bool result = true;
    D3DTexture *texture;
    function_0158f0(0, 11, 64, 64, 0, 1, &texture);
    g_509374 = texture;
    g_509378 = D3DTexture_GetSurfaceLevel2(texture, 0);
    function_0158f0(0, 11, 64, 64, 0, 1, &texture);
    g_50937c = texture;
    g_509380 = D3DTexture_GetSurfaceLevel2(texture, 0);
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
extern short_rect_pair g_485a8a;
extern short g_485aca;
struct s_128c0_settings
{
    byte depth_format, interval_flag;
    short interval;
    long quality, extra;
};
word g_485ac8;
long g_485acc, g_485ad0;
long g_467008, g_46700c;
bool g_5093bc;

// @retail 0x128c0
bool function_128c0(s_128c0_settings const *settings)
{
    g_485ac8 = *(word const *)settings;
    g_485aca = settings->interval;
    g_485acc = settings->quality;
    g_485ad0 = settings->extra;
    volatile bool result = true;
    D3DPRESENT_PARAMETERS parameters;
    memset(&parameters, 0, sizeof(parameters));
    parameters.Windowed = false;
    parameters.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    parameters.EnableAutoDepthStencil = true;
    parameters.AutoDepthStencilFormat = (D3DFORMAT)(D3DFMT_D24S8 + ((byte)g_485ac8 != 0));
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
        parameters.FullScreen_PresentationInterval = 1 + ((g_485ac8 >> 8) ? 0x80000000 : 0);
        break;
    case 2:
        parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
        parameters.FullScreen_PresentationInterval = 2 + ((g_485ac8 >> 8) ? 0x80000000 : 0);
        break;
    default:
        parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
        g_485aca = 1;
        parameters.FullScreen_PresentationInterval = 0;
        break;
    }
    parameters.FullScreen_RefreshRateInHz = (short)g_485ac0;
    if (!g_5093b0)
    {
        D3DDevice *device = NULL;
        Direct3D_CreateDevice(0, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &parameters, &device);
        g_5093b0 = device;
    }
    else
        D3DDevice_Reset(&parameters);
    if (!g_5093b0)
    {
        g_5093b0 = NULL;
        return false;
    }
    memcpy(g_484ce8, g_47ffd8, sizeof(g_484ce8));
    if (g_485acc < 0 || g_485acc > 5)
        g_485acc = 3;
    g_467008 = NONE;
    g_46700c = NONE;
    g_5093bc = true;
    return result;
}

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
            D3DSurface_LockRect(surface, &locked, NULL, 0);
            locked.pBits = (void *)((dword)locked.pBits | 0xf0000000);
            if (locked.pBits)
            {
                long width = (short)(g_5093c0.second >> 16) - (short)(g_5093c0.first >> 16);
                long height = (short)g_5093c0.second - (short)g_5093c0.first;
                byte const *source = (byte const *)g_5093c8;
                byte *row = (byte *)locked.pBits + (short)g_5093c0.first * locked.Pitch;
                for (long y = 0; y < height; ++y)
                {
                    dword *pixel = (dword *)row + (short)(g_5093c0.first >> 16);
                    for (long x = 0; x < width; ++x)
                    {
                        long value = source[x] * g_5093cc;
                        byte level = (byte)(value < 0 ? 0 : value > 255 ? 255 : value);
                        *pixel++ = (((((dword)level << 8) | level) << 8 | level) << 8) | level;
                    }
                    row += locked.Pitch;
                    source += width;
                }
            }
            D3DSurface_UnlockRect(surface);
        }
        D3DResource_Release(surface);
    }
}

extern D3DResource *g_485ae4, *g_485ae8, *g_485aec;

// @retail 0x1d0e0
bool function_1d0e0()
{
    D3DTexture *texture;
    if (!function_0158f0(0, 9, 4, 4, 0, 0, &texture))
    {
        g_485ae4 = texture;
        return false;
    }
    g_485ae4 = texture;
    D3DVolumeTexture *volume;
    if (!function_015a60(9, 4, 4, 4, 0, 0, &volume))
    {
        g_485ae8 = volume;
        return false;
    }
    g_485ae8 = volume;
    D3DCubeTexture *cube;
    if (!function_0159b0(4, 9, 0, 0, &cube))
    {
        g_485aec = cube;
        return false;
    }
    g_485aec = cube;
    bool result = true;
    if (!g_485ae4 || !g_485ae8 || !g_485aec)
        return false;
    word colors[2] = { 0x0f00, 0xf0f0 };
    D3DLOCKED_RECT rectangle;
    D3DTexture_LockRect((D3DTexture *)g_485ae4, 0, &rectangle, NULL, 0);
    for (long i = 0; i < 16; ++i)
        ((word *)rectangle.pBits)[i] = colors[i & 1];
    D3DTexture_UnlockRect((D3DTexture *)g_485ae4, 0);
    D3DLOCKED_BOX box;
    D3DVolumeTexture_LockBox((D3DVolumeTexture *)g_485ae8, 0, &box, NULL, 0);
    for (long j = 0; j < 64; ++j)
        ((word *)box.pBits)[j] = colors[j & 1];
    D3DVolumeTexture_UnlockBox((D3DVolumeTexture *)g_485ae8, 0);
    for (long face = 0; face < 6; ++face)
    {
        D3DCubeTexture_LockRect((D3DCubeTexture *)g_485aec, (D3DCUBEMAP_FACES)face, 0, &rectangle, NULL, 0);
        for (long k = 0; k < 16; ++k)
            ((word *)rectangle.pBits)[k] = colors[k & 1];
        D3DCubeTexture_UnlockRect((D3DCubeTexture *)g_485aec, (D3DCUBEMAP_FACES)face, 0);
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
        *(volatile long *)&opaque = D3DRS_BLENDOP;
        function_0222d0((D3DRENDERSTATETYPE)*(volatile long *)&opaque, D3DBLENDOP_ADD);
        *(volatile long *)&opaque = D3DRS_ZWRITEENABLE;
        function_0222d0((D3DRENDERSTATETYPE)*(volatile long *)&opaque, 0);
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

extern real g_48565c;
real g_48568c[46];
bool g_55e6bc;

// @retail 0x12d50
void function_12d50(real const *projection, bool scaled, real *output)
{
    real constants[16];
    real near_plane = (scaled ? 0.15f : 1.0f) * g_485adc;
    real far_plane = g_485ae0;
    D3DDevice_SetDepthClipPlanes(near_plane, far_plane,
        D3DSDCP_SET_VERTEXPROGRAM_PLANES);
    if (!projection)
        projection = g_48568c;
    if (!output)
        output = constants;
    real scale = 16777215.0f / g_48565c;
    for (long i = 0; i < 4; ++i)
    {
        output[i * 4] = (projection[38 + i] * projection[3] + projection[1] * projection[30 + i]
            + projection[34 + i] * projection[2]) * scale;
        output[i * 4 + 1] = (projection[38 + i] * projection[6] + projection[30 + i] * projection[4]
            + projection[34 + i] * projection[5]) * scale;
        output[i * 4 + 2] = (projection[38 + i] * projection[9] + projection[8] * projection[34 + i]
            + projection[30 + i] * projection[7]) * scale;
        output[i * 4 + 3] = (projection[38 + i] * projection[12] + projection[11] * projection[34 + i]
            + projection[30 + i] * projection[10]) * scale;
        output[i * 4 + 3] += projection[42 + i] * scale;
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
    g_50935c = D3DDevice_GetBackBuffer2(0);
    g_509360 = D3DDevice_GetBackBuffer2(-1);
    g_509364 = D3DDevice_GetDepthStencilSurface2();
    if (g_50935c && g_509360 && g_509364)
    {
        void *memory = VirtualAlloc(NULL, 20, 0x101000, PAGE_READWRITE);
        if (!memory) GetLastError();
        g_509354[0] = (D3DTexture *)memory;
        memory = VirtualAlloc(NULL, 20, 0x101000, PAGE_READWRITE);
        if (!memory) GetLastError();
        g_509354[1] = (D3DTexture *)memory;
        if (g_509354[0] && g_509354[1])
        {
            for (long i = 0; i < 2; ++i)
            {
                D3DTexture *texture = g_509354[i];
                texture->Common = 0x40001;
                texture->Data = (i ? g_509360 : g_50935c)->Data;
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
        *g_509370 = *g_50935c;
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
    real x = 1.0f / (short)(g_48564e - g_48564a);
    real constants[20];
    constants[0] = x * 2.0f;
    constants[1] = 0.0f;
    constants[2] = 0.0f;
    constants[3] = -1.0f - x;
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
    real extra[24];
    real constants[20];
    memset(extra, 0, sizeof(extra));
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
            D3DDevice_Release();
            g_5093b0 = NULL;
        }
    }
    return result;
}



extern s_44940_entry g_4ba138[850];
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
        D3DDevice_SetVertexShaderConstant(18, constants, 8);
        D3DPIXELSHADERDEF program;
        memset(&program, 0, sizeof(program));
        program.PSAlphaOutputs[0] = 0xc00;
        program.PSRGBOutputs[0] = 0xc00;
        program.PSRGBOutputs[1] = 0xc00;
        program.PSTextureModes = 0x8421;
        program.PSCombinerCount = 2;
        program.PSConstant0[0] = 0xff000000;
        program.PSAlphaInputs[0] = 0x08a009a0;
        program.PSRGBInputs[0] = 0x0aa00ba0;
        program.PSRGBInputs[1] = 0x1c110c11;
        program.PSFinalCombinerInputsABCD = 0xc;
        g_484f68 = program;
        D3DDevice_SetPixelShaderProgram(&program);
        for (short i = 0; i < count; ++i)
        {
            volatile short source = i & 1 ? second : first;
            volatile short target = i & 1 ? first : second;
            for (short stage = 0; stage < 4; ++stage)
            {
                function_14f60(stage, source);
                D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSU, 4);
                D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSV, 4);
                D3DDevice_SetTextureStageState(stage, D3DTSS_MAGFILTER, 2);
                D3DDevice_SetTextureStageState(stage, D3DTSS_MINFILTER, 2);
                D3DDevice_SetTextureStageState(stage, D3DTSS_MIPFILTER, 1);
                D3DDevice_SetTextureStageState(stage, D3DTSS_MIPMAPLODBIAS, 0);
                D3DDevice_SetTextureStageState(stage, D3DTSS_MAXMIPLEVEL, 0);
                D3DDevice_SetTextureStageState(stage, D3DTSS_MAXANISOTROPY, 0);
                D3DDevice_SetTextureStageState(stage, D3DTSS_COLORSIGN, 0);
                D3DDevice_SetTextureStageState(stage, D3DTSS_ALPHAKILL, 0);
            }
            function_14bc0(target, 0, false);
            program.PSConstant0[0] = (i > 0 ? 0x7fu : 0xffu) << 24;
            g_484f68 = program;
            D3DDevice_SetPixelShaderProgram(&program);
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
    (void)&index;
    s_44940_entry *entry = &g_4ba138[index];
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
        function_12d50(NULL, true, NULL);
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
    s_44940_entry *entry = &g_4ba138[index];
    if (entry->unknown10[10] != 0xff && !(fabs(g_4b8494 - 1.0f) < 0.0001f))
    {
        D3DDevice_SetStipple(function_1c290(1.0f));
        g_4b8494 = 1.0f;
    }
    if (entry->unknown00 & 0x800)
        function_12d50(NULL, false, NULL);
    if ((mode == 1 || mode == 3) && ((entry->unknown00 & 0x4000) || g_485a75 || g_485a76))
        function_15370(1);
}



#include "visibility_slot.h"
typedef long (__stdcall *visibility_draw_callback)(s_slot *slot, byte *data);
void __stdcall function_20bb0(long player, dword mask, long const *indices, long count, bool query, visibility_draw_callback draw);
long __stdcall function_2e540(s_slot *slot, byte *data);
extern long g_4b9ed4;
extern long g_485898;
extern vector3f g_4b9dac;

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
    real scale = definition[0x28] & 0x40 ? *(real *)(data + 0x14) : 1.0f;
    if (tag != NONE && !(definition[0x28] & 2))
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
            position.x += -radius * g_4b9dac.i;
            position.y += -radius * g_4b9dac.j;
            position.z += -radius * g_4b9dac.k;
            break;
        case 1:
            position.x += offset.i * (radius * 1.4142135381698608f);
            position.y += offset.j * (radius * 1.4142135381698608f);
            position.z += offset.k * (radius * 1.4142135381698608f);
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
                D3DDevice_Begin(D3DPT_TRIANGLEFAN);
                D3DDevice_SetVertexData4f(0, left, top, screen[2], screen[3]);
                D3DDevice_SetVertexData4f(0, right, top, screen[2], screen[3]);
                D3DDevice_SetVertexData4f(0, right, bottom, screen[2], screen[3]);
                D3DDevice_SetVertexData4f(0, left, bottom, screen[2], screen[3]);
                D3DDevice_End();
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
    function_0222d0(D3DRS_ALPHABLENDENABLE, 0);
    function_0222d0(D3DRS_COLORWRITEENABLE, 0);
    function_0222d0(D3DRS_ALPHATESTENABLE, 0);
    function_0222d0(D3DRS_ZWRITEENABLE, 0);
    function_0222d0(D3DRS_ZENABLE, 2);
    function_0222d0(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    function_0222d0(D3DRS_CULLMODE, 0);
    function_0222d0(D3DRS_STENCILENABLE, 0);
    function_0222d0(D3DRS_ZBIAS, 0);
    function_1cf50();
    D3DPIXELSHADERDEF program;
    memset(&program, 0, sizeof(program));
    program.PSCombinerCount = 0x11001;
    program.PSFinalCombinerInputsABCD = 0;
    program.PSFinalCombinerInputsEFG = 0;
    g_484f68 = program;
    D3DDevice_SetPixelShaderProgram(&program);
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
    bool front = (byte)front_value != 0;
    if (bitmap && *(void **)(bitmap + 0x54))
    {
        short_rect_pair rectangle_pair = g_485a8a;
        short right = (short)(rectangle_pair.a.v1 + *(short *)(bitmap + 4));
        if (rectangle_pair.a.v3 <= right) right = rectangle_pair.a.v3;
        short bottom = (short)(rectangle_pair.a.v0 + *(short *)(bitmap + 6));
        if (rectangle_pair.a.v2 <= bottom) bottom = rectangle_pair.a.v2;
        long width = right - rectangle_pair.a.v1;
        long height = bottom - rectangle_pair.a.v0;
        short format = *(short *)(bitmap + 0xc);
        if ((format == 11 || format == 10) && *(short *)(bitmap + 0x14) == 0)
        {
            D3DSurface *surface = front ? D3DDevice_GetRenderTarget2() : D3DDevice_GetBackBuffer2(0);
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
void function_12fa0(real const *projection, byte const *camera, bool scaled, long mode)
{
    real width = (real)(*(short const *)(camera + 0x36) - *(short const *)(camera + 0x32));
    real height = (real)(*(short const *)(camera + 0x34) - *(short const *)(camera + 0x30));
    real left = (real)*(short const *)(camera + 0x32);
    real top = (real)*(short const *)(camera + 0x30);
    real scale = 1.0f;
    if (scaled && !g_5093fc)
        scale = g_4670c8 < 0.0625f ? 0.0625f : g_4670c8 > 1.0f ? 1.0f : g_4670c8;
    real bias = 0.03125f;
    if (mode == 2)
    {
        width = height = 128.0f;
        left = top = 0.0f;
        scale = 1.0f;
        bias = 0.0f;
    }
    real constants[48];
    function_12d50(projection, false, constants);
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
    constants[26] = projection[22];
    constants[27] = 2.0f;
    constants[28] = ((real const *)camera)[0];
    constants[29] = ((real const *)camera)[1];
    constants[30] = ((real const *)camera)[2];
    constants[31] = 767.8125f;
    constants[32] = width * 0.5f * scale;
    constants[33] = 0.0f;
    constants[34] = 0.0f;
    constants[35] = ((width + 1.0f) * 0.5f + left) * scale + bias;
    constants[36] = 0.0f;
    constants[37] = height * -0.5f * scale;
    constants[38] = 0.0f;
    constants[39] = ((height + 1.0f) * 0.5f + top) * scale + bias;
    constants[40] = width * 0.5f * scale;
    constants[41] = height * -0.5f * scale;
    constants[42] = 16777215.0f;
    constants[43] = 0.0f;
    constants[44] = (width * 0.5f + left) * scale + 0.5f;
    constants[45] = (height * 0.5f + top) * scale + 0.5f;
    constants[46] = 0.0f;
    constants[47] = 0.0f;
    D3DDevice_SetVertexShaderConstant(-96, constants, 12);
}


real *table_entry_data(long handle);
long function_184000(long bit, long index);

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
        visible = (byte)function_184000(bit, instance) != 0;
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
        byte *transform;
        if (selected != NONE)
        {
            if (tag[4] & 4) selected = ((short *)(transforms + 4))[transform_index * 2];
            transform = transforms + 0x44 + selected * 0x30;
        }
        else transform = transforms + 0x44;
        *(real *)(out + 4) = *(real *)(transform + 0xc);
        *(real *)(out + 8) = *(real *)(transform + 0x1c);
        *(real *)(out + 0xc) = *(real *)(transform + 0x2c);
        long bias = entry->unknown10[11] & 15;
        if (bias) *(real *)(out + 0x10) = (bias - 5) * 0.001f;
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



long g_485870;
real g_485874, g_485878, g_48587c, g_485880, g_485884;
__declspec(noinline) dword __cdecl pack_color4f(color4f const *color);

// @retail 0x40890
void function_40890(void)
{
	if (!g_485870) return;
	color4f color, inverse;
	color.alpha = inverse.alpha = g_485878 * g_485874;
	color.red = g_48587c * g_485874;
	color.green = g_485880 * g_485874;
	color.blue = g_485884 * g_485874;
	inverse.red = (1.0f - g_48587c) * g_485874;
	inverse.green = (1.0f - g_485880) * g_485874;
	inverse.blue = (1.0f - g_485884) * g_485874;
	volatile dword packed = pack_color4f(&color);
	dword constant = pack_color4f(&inverse);
	function_0222d0(D3DRS_CULLMODE, 0x901);
	function_0222d0(D3DRS_COLORWRITEENABLE, 0x10101);
	function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
	dword local_617c45, rgb_inputs;
	switch (g_485870)
	{
	case 1: case 2:
		function_0222d0(D3DRS_SRCBLEND, 1);
		function_0222d0(D3DRS_DESTBLEND, g_485870 == 1 ? 0x303 : 1);
		function_0222d0(D3DRS_BLENDOP, g_485870 == 1 ? 0x8006 : 0x800b);
		constant = packed;
		rgb_inputs = 0x1200000;
		local_617c45 = 0x11200000;
		break;
	case 3: case 4: case 5:
		function_0222d0(D3DRS_SRCBLEND, 0x307);
		function_0222d0(D3DRS_DESTBLEND, 0x8002);
		function_0222d0(D3DRS_BLENDOP, g_485870 == 3 ? 0x8008 : g_485870 == 4 ? 0x8007 : 0x8006);
		function_0222d0(D3DRS_BLENDCOLOR, packed);
		constant = packed;
		rgb_inputs = g_485870 == 3 ? 0x1201140 : g_485870 == 4 ? 0x1201120 : 0x1411120;
		local_617c45 = 0;
		break;
	default:
		function_0222d0(D3DRS_SRCBLEND, 1);
		function_0222d0(D3DRS_DESTBLEND, 0x8002);
		function_0222d0(D3DRS_BLENDOP, 0x8006);
		function_0222d0(D3DRS_BLENDCOLOR, constant);
		rgb_inputs = 0x11200000;
		local_617c45 = 0;
		break;
	}
	function_0222d0(D3DRS_ALPHATESTENABLE, 0);
	function_0222d0(D3DRS_ZENABLE, 0);
	function_0222d0(D3DRS_ZBIAS, 0);
	function_1cc30(13);
	function_1c710(g_51f0f0);
	real x = 1.0f / (short)(g_48564e - g_48564a);
	real y = 1.0f / (short)(g_48564c - g_485648);
	real constants[20] = {
		2.0f * x, 0, 0, -1.0f - x,
		0, -2.0f * y, 0, 1.0f + y,
		0, 0, 0, 0.5f,
		0, 0, 0, 1.0f,
		0, 0, 0, 1.0f
	};
	D3DDevice_SetVertexShaderConstant(81, constants, 5);
	D3DPIXELSHADERDEF program;
	memset(&program, 0, sizeof(program));
	program.PSAlphaInputs[0] = local_617c45;
	program.PSRGBInputs[0] = rgb_inputs;
	program.PSCombinerCount = 1;
	program.PSConstant0[0] = constant;
	program.PSAlphaOutputs[0] = 0xc00;
	program.PSRGBOutputs[0] = 0xc00;
	program.PSFinalCombinerInputsABCD = 0xc;
	program.PSFinalCombinerInputsEFG = 0x1c00;
	g_484f68 = program;
	function_15180(&program);
	short width = g_48564e - g_48564a;
	short height = g_48564c - g_485648;
	function_1cf50();
	D3DDevice_Begin(D3DPT_QUADLIST);
	D3DDevice_SetVertexData2s(0, 0, 0);
	D3DDevice_SetVertexData2s(0, width, 0);
	D3DDevice_SetVertexData2s(0, width, height);
	D3DDevice_SetVertexData2s(0, 0, height);
	D3DDevice_End();
}

bool function_48b00(point3f const *point, real radius, real *screen, real *extent);
void function_1cf50();

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
        D3DDevice_Begin(D3DPT_TRIANGLEFAN);
        D3DDevice_SetVertexDataColor(5, color);
        D3DDevice_SetVertexData2f(2, 1.0f, 0.0f);
        real wc = width * cosine;
        real hs = height * sine;
        real ws = width * sine;
        real hc = height * cosine;
        real a = (wc - hs) * extent[0];
        real b = (ws + hc) * extent[1];
        real x = screen[0] + a;
        real y = screen[1] - b;
        D3DDevice_SetVertexData2f(1, x, y);
        D3DDevice_SetVertexData4f(0, x, y, z, w);
        real c = (hs + wc) * extent[0];
        real d = (ws - hc) * extent[1];
        D3DDevice_SetVertexData2f(2, 1.0f, 1.0f);
        x = screen[0] + c;
        y = screen[1] - d;
        D3DDevice_SetVertexData2f(1, x, y);
        D3DDevice_SetVertexData4f(0, x, y, z, w);
        D3DDevice_SetVertexData2f(2, 0.0f, 1.0f);
        x = screen[0] - a;
        y = screen[1] + b;
        D3DDevice_SetVertexData2f(1, x, y);
        D3DDevice_SetVertexData4f(0, x, y, z, w);
        D3DDevice_SetVertexData2f(2, 0.0f, 0.0f);
        x = screen[0] - c;
        y = screen[1] + d;
        D3DDevice_SetVertexData2f(1, x, y);
        D3DDevice_SetVertexData4f(0, x, y, z, w);
        D3DDevice_End();
    }
}

// @retail 0x480a0
void __stdcall function_480a0(point3f const *point, real width, real height, real cosine, real sine, dword color)
{
    real screen[4], extent[2];
    if (width > 0.0f && height > 0.0f && function_48b00(point, 1.0f, screen, extent))
    {
        function_1cf50();
        D3DDevice_Begin(D3DPT_TRIANGLEFAN);
        D3DDevice_SetVertexDataColor(9, color);
        real wc = width * cosine;
        real hs = height * sine;
        real ws = width * sine;
        real hc = height * cosine;
        real a = (wc - hs) * extent[0];
        real b = (ws + hc) * extent[1];
        D3DDevice_SetVertexData2s(3, 1, 0);
        D3DDevice_SetVertexData4f(0, screen[0] + a, screen[1] - b, screen[2], screen[3]);
        real c = (hs + wc) * extent[0];
        real d = (ws - hc) * extent[1];
        D3DDevice_SetVertexData2s(3, 1, 1);
        D3DDevice_SetVertexData4f(0, screen[0] + c, screen[1] - d, screen[2], screen[3]);
        D3DDevice_SetVertexData2s(3, 0, 1);
        D3DDevice_SetVertexData4f(0, screen[0] - a, screen[1] + b, screen[2], screen[3]);
        D3DDevice_SetVertexData2s(3, 0, 0);
        D3DDevice_SetVertexData4f(0, screen[0] - c, screen[1] + d, screen[2], screen[3]);
        D3DDevice_End();
    }
}

byte g_4b6291;

void __stdcall function_41cc0(void *payload);

// @retail 0x420a0
void function_420a0(void)
{
    dword used[2] = { 0, 0 };
    long count = g_4c19ac;
    for (long i = 0; i < count; ++i)
    {
        s_42760_entry *entry = &g_4c152c[i];
        if (entry->type != 1 || (used[i >> 5] & (1 << (i & 31))))
            continue;
        if (g_4b6280.count >= 1024)
        {
            if (g_4b6291)
                g_4b6291 = false;
            continue;
        }
        s_1e4e0_record *record = (s_1e4e0_record *)&g_4b6280.records[g_4b6280.count++];
        record->type = 0;
        record->active = false;
        record->type = 3;
        record->flags = 0;
        record->callback = function_41cc0;
        used[i >> 5] |= 1 << (i & 31);
        byte indices[32];
        indices[0] = (byte)i;
        long selected = 1;
        point3f minimum = entry->position;
        point3f maximum = entry->position;
        real depth = entry->depth;
        for (long j = 0; j < count; ++j)
        {
            if (used[j >> 5] & (1 << (j & 31)))
                continue;
            s_42760_entry *candidate = &g_4c152c[j];
            box2f bounds = *(box2f *)candidate->unknown10;
            bool intersects = false;
            for (long k = 0; k < selected; ++k)
            {
                box2f previous = *(box2f *)g_4c152c[(signed char)indices[k]].unknown10;
                if (bounds.x1 > previous.x0 && bounds.y1 > previous.y0 &&
                    previous.x1 > bounds.x0 && previous.y1 > bounds.y0)
                {
                    intersects = true;
                    break;
                }
            }
            if (!intersects)
            {
                used[j >> 5] |= 1 << (j & 31);
                indices[selected++] = (byte)j;
                point3f position = candidate->position;
                if (!(position.x > minimum.x)) minimum.x = position.x;
                if (!(position.y > minimum.y)) minimum.y = position.y;
                if (!(position.z > minimum.z)) minimum.z = position.z;
                if (position.x > maximum.x) maximum.x = position.x;
                if (position.y > maximum.y) maximum.y = position.y;
                if (position.z > maximum.z) maximum.z = position.z;
                if (depth > candidate->depth) depth = candidate->depth;
            }
        }
        record->payload[0] = (byte)selected;
        memcpy(record->payload + 1, indices, selected);
        record->depth = 0.0f - depth;
        record->position.x = (minimum.x + maximum.x) * 0.5f;
        record->position.y = (minimum.y + maximum.y) * 0.5f;
        record->position.z = (minimum.z + maximum.z) * 0.5f;
    }
}

extern dword g_4b8448, g_4b8308, g_4b8450;
extern dword g_4b82e8, g_4b82f4, g_4b82f8;
dword g_4b8438, g_4b82e0, g_4b82fc, g_4b82ec;
void function_48010(real const *color);

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
        D3DDevice_SetVertexShaderConstantFast(78, identity, 3);
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
        if (mode == 1 || mode == 2 || mode == 3)
        {
            g_4b82e8 = 0;
            D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
        }
        else if (mode == 4 || mode == 5)
        {
            g_4b82e8 = 1;
            D3DDevice_SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
            g_4b82f4 = D3DBLEND_SRCALPHA;
            D3DDevice_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
            g_4b82f8 = D3DBLEND_INVSRCALPHA;
            D3DDevice_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
            function_0222d0(D3DRS_BLENDOP, D3DBLENDOP_ADD);
        }
        else
        {
            function_0222d0(D3DRS_ALPHABLENDENABLE, 1);
            function_0222d0(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
            function_0222d0(D3DRS_DESTBLEND, D3DBLEND_ONE);
            function_0222d0(D3DRS_BLENDOP, D3DBLENDOP_ADD);
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
        D3DDevice_SetRenderState(D3DRS_ALPHATESTENABLE, 0);
        D3DDevice_SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
        _ReadWriteBarrier();
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

PRIVATE inline void texture_surface_dimensions(long index, long *width, long *height)
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

// @retail 0x19530
void function_19530(long mode, byte const *context, short stage, long *width, long *height)
{
    (void)&context; (void)&stage; (void)&width; (void)&height;
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
        function_14f60(stage, (short)g_4858b8);
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
    case 8:
        function_14f60(stage, 10);
        texture_surface_dimensions(10, width, height);
        return;
    case 9: function_143c0(*(long *)(g_485a80 + 4), 0, stage, 0.0f); return;
    case 10:
        if (g_4858c8 && g_4858c4)
        {
            if (*(dword *)g_4858c4 & 0x80000) { mode = 18; goto again; }
            if (*(long *)(g_4858c4 + 0xe0) != NONE && !g_485a77)
                function_14f60(stage, 13);
            else
                function_144f0(*(long *)(g_4858c4 + 0x7c), stage, 1, 0, 0, 0.0f);
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
        else function_144f0(*(long *)(g_4858c4 + 0x7c), stage, 1, 0, 0, 0.0f);
        return;
    case 13:
        function_14f60(stage, 9);
        texture_surface_dimensions(9, width, height);
        return;
    case 14: function_143c0(*(long *)(g_485a80 + 0x14), 0, stage, 0.0f); return;
    case 15: function_143c0(*(long *)(g_485a80 + 0x14), 1, stage, 0.0f); return;
    case 16:
        function_143c0(g_4e6978, (short)g_4e697c, stage, 0.0f);
        if (g_4e6978 != NONE)
        {
            byte *definition = g_4e3b44[g_4e6978 & 0xffff].bytes;
            long clamped = g_4e697c < 0 ? 0 : g_4e697c > *(long *)(definition + 0x44) - 1 ? *(long *)(definition + 0x44) - 1 : g_4e697c;
            if (g_4e697c == clamped)
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
                function_14f60(stage, (short)surface);
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
            long surface = mode == 24 ? g_485a78[0] : g_485a78[1];
            function_14f60(stage, (short)surface);
            if (surface == 25 || surface == 26 || surface == 27)
                function_01dd60(surface, width, height);
            return;
        }
    case 26:
        {
            byte *definition = g_4e3b44[*(long *)(context + 8) & 0xffff].bytes;
            byte *parameters = *(byte **)(definition + 0x24);
            short index = *(short *)(*(byte **)(parameters + 0x60) + 0x10);
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

// @retail 0x18a90
void function_18a90(byte *state, word const *descriptor)
{
    (void)&descriptor;
    byte *binding = *(byte **)(*(byte **)(state + 0x20) + 0xc) + (*descriptor & 0x1ff) * 4;
    long i = 0;
    if ((*descriptor >> 9) > 0)
    {
        do
        {
            long slot = binding[2];
            byte *destination = state + 0x1320 + slot * 0x40;
            if (binding[0] != 0xff)
            {
                byte *source = state + 0x320 + (signed char)binding[0] * 0x40;
                memcpy(destination, source, 0x40);
                if ((binding[3] & 2) && *(long *)source != NONE)
                {
                    byte *definition = g_4e3b44[*(long *)source & 0xffff].bytes;
                    real index = *(real *)(source + 0x18);
                    if ((real)*(long *)(definition + 0x44) > index)
                    {
                        byte *bitmap = *(byte **)(definition + 0x48) + (long)floor(index) * 0x74;
                        if (bitmap[0xe] & 0x10)
                        {
                            *(long *)(source + 8) = *(short *)(bitmap + 4);
                            *(long *)(source + 0xc) = *(short *)(bitmap + 6);
                        }
                    }
                }
                *(dword *)(state + 0x1420) |= 1 << slot;
            }
            else if (binding[1])
            {
                if (binding[1] != 11)
                {
                    *(long *)destination = NONE;
                    function_19530((signed char)binding[1], state, (short)slot,
                        (long *)(destination + 8), (long *)(destination + 0xc));
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
                        long tag = *(long *)(location + 0x1c);
                        if (*(long *)destination != tag || *(real *)(destination + 0x18) != (real)g_485a60)
                        {
                            byte *definition = g_4e3b44[tag & 0xffff].bytes;
                            byte *bitmap = *(byte **)(definition + 0x48) + g_485a60 * 0x74;
                            *(real *)(destination + 0x10) = 3.0f;
                            *(long *)destination = tag;
                            *(real *)(destination + 0x18) = (real)g_485a60;
                            short width = *(short *)(bitmap + 4);
                            short height = *(short *)(bitmap + 6);
                            long maximum = width > height ? width : height;
                            *(real *)(destination + 0x14) = (real)(log((double)maximum) * 0.6931471824645996f);
                        }
                        *(dword *)(state + 0x1420) |= 1 << slot;
                    }
                    else
                        *(long *)destination = *(long *)(g_485a80 + 0x64);
                }
            }
            else
                *(long *)destination = NONE;
            binding += 4;
            ++i;
        } while (i < (*descriptor >> 9));
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
    volatile bool result = false;
    if (tag != NONE)
    {
        byte *definition = g_4e3b44[tag & 0xffff].bytes;
        byte *shader = g_4e3b44[*(long *)(definition + 4) & 0xffff].bytes;
        byte *tables = *(byte **)(shader + 0x5c);
        if (*(long *)tables > 0)
        {
            byte *entry = *(byte **)(tables + 4);
            if (*(dword *)(entry + 2) & (1 << stage))
            {
                word *passes = (word *)*(byte **)(tables + 0xc) + (*(word *)entry & 0x1ff) + stage;
                long pass = 0;
                if ((*passes >> 9) > 0)
                {
                    do
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
                        ++pass;
                    } while (pass < (*passes >> 9));
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
                if (target == NONE) target = (word)g_4858b8;
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
    function_1bbf0(first, tag, second, third, fourth, scale);
}

extern real g_4c19b0, g_4c19b4, g_4b9de4;
extern short g_4b9dd0, g_4b9dd2, g_4b9dd4, g_4b9dd6;
real g_4c19b8, g_4c19bc;
byte g_55e6c5;
struct s_filter_vector { real x, y, z, w; };
static s_filter_vector const s_default_filter_coordinate = { 0.0f, 0.0f, 0.0f, 0.0f };
s_filter_vector const *g_4687c0 = &s_default_filter_coordinate;
void function_01ddd0(long index, dword width, dword height);
long function_21360(long source, real radius, real low, real high, long target,
    long alternate, long passes, long unused, bool configure, bool full_surface, real scale);
long function_211a0(long source, real passes, real distortion, real falloff,
    long target, long alternate, long unused, long configure, long full_surface,
    real scale, real offset);

struct s_filter_coordinates
{
    word flags[4];
    s_filter_vector offsets[4];
};

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
        dimensions[i][0] = surface->data && !surface->flag95 ? surface->width : 0;
        dimensions[i][1] = surface->data && !surface->flag95 ? surface->height : 0;
    }
    real left = (real)g_4b9dd2, right = (real)g_4b9dd6;
    real top = (real)g_4b9dd0, bottom = (real)g_4b9dd4;
    for (long record_index = 0; record_index < *(long const *)(definition + 0x88); ++record_index)
    {
        byte const *record = *(byte **)(definition + 0x8c) + record_index * 0xac;
        long pass = *(short const *)(record + 8);
        if (pass == NONE) pass = record_index;
        long selector = (weights[0] != 0.0f ? 1 : 0) + (weights[1] != 0.0f ? 2 : 0);
        long variant = ((signed char const *)record)[0xc + selector];
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
                    D3DDevice_SetVertexData4f(D3DVSDE_VERTEX, x, y, 16777215.0f, 16777215.0f);
                }
            }
            D3DDevice_End();
        }
        for (long filter = 0; filter < *(long const *)(record + 0xa4); ++filter)
        {
            byte const *entry = *(byte **)(record + 0xa8) + filter * 0x5c;
            long output = filter == 0 ? 25 : 27;
            long alternate = filter == 0 ? 26 : g_485a78[0] == 25 ? 26 : 25;
            word flags = *(word const *)entry;
            if (!(flags & 1) || weights[0] == 1.0f)
            {
                if (!(flags & 2) || weights[1] == 1.0f)
                {
                    long target_width = 0, target_height = 0;
                    function_01dd60(target, &target_width, &target_height);
                    real factor = *(real const *)(entry + 0x58);
                    long scaled_width = (long)(target_width * factor), scaled_height = (long)(target_height * factor);
                    if (scaled_width < 64) scaled_width = 64;
                    if (scaled_height < 64) scaled_height = 64;
                    if (scaled_width * scaled_height > 76800)
                    {
                        double scale = sqrt(76800.0 / (target_width * target_height));
                        scaled_width = (long)(target_width * scale);
                        scaled_height = (long)(target_height * scale);
                        if (scaled_width < 64 || scaled_height < 64)
                        {
                            scaled_width = scaled_height = 64;
                            if (!g_55e6c5) g_55e6c5 = true;
                        }
                    }
                    function_01ddd0(output, scaled_width, scaled_height);
                    function_01ddd0(alternate, scaled_width, scaled_height);
                    if (flags & 4)
                        target = function_21360(target, *(real const *)(entry + 0x48),
                            *(real const *)(entry + 0x50), *(real const *)(entry + 0x54), output, alternate,
                            (long)*(real const *)(entry + 0x44), NONE, true, false, 0.0f);
                    else target = function_211a0(target, *(real const *)(entry + 0x44),
                        *(real const *)(entry + 0x48), *(real const *)(entry + 0x4c),
                        output, alternate, NONE, 1, 0, 0.0f, 0.0f);
                }
            }
            g_485a78[filter] = target;
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

// @retail 0x4b2d0
void function_4b2d0(long tag, long first, long second, long third, long fourth)
{
    (void)&third; (void)&fourth;
    long selected = tag == NONE ? *(long *)(g_485a80 + 0xd0) : tag;
    dword group = ((dword *)g_4e3b44)[(short)tag * 4];
    byte const *parameters;
    if (group == 0x5052544d || group == 0x70727433)
        parameters = (byte const *)function_137bd0(tag)->function_x947334();
    else parameters = *(byte **)(g_4e3b44[tag & 0xffff].bytes + 0x24);
    byte *definition = g_4e3b44[*(long const *)parameters & 0xffff].bytes;
    byte *tables = *(byte **)(definition + 0x5c);
    byte *entry = *(byte **)(tables + 4) + first * 10;
    word range = ((word *)*(byte **)(tables + 0xc))[(*(word *)entry & 0x1ff) + second];
    byte *pass = *(byte **)(tables + 0x14) + ((range & 0x1ff) + fourth) * 10;
    byte *pass_definition = g_4e3b44[*(long *)(pass + 4) & 0xffff].bytes;
    byte *record = *(byte **)(*(byte **)(pass_definition + 0x20) + 4) + third * 0x132;
    long program = *(long *)(record + 0x100);
    function_1bbf0(selected, first, second, fourth, third, 640.0f);
    function_1c590((s_shader_cache *)g_51f0f0, program, 0);
}

#include "geometry_cache.h"
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
vector3f *function_4efb0(vector3f *vector);
point3f *function_3f220(dword first, dword second, dword third, point3f *out);
vector3f *function_11d000(vector3f const *vector, vector3f *perpendicular);
void function_141ce0(real first, real second, real third, transform4x3f *out);
real g_5259b4;
dword g_4b8430;

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
    byte *table = *(byte **)((byte *)g_4e0348 + 0x238);
    byte *record = *(byte **)(table + 0x1c) + ((handle >> 8) & 0x3fffff) * 24;
    byte *block = *(byte **)(table + 0x14) + *(short *)(record + 6) * 0x2c;
    byte *references = *(byte **)((byte *)g_4e0350 + 0x37c);
    long model = *(long *)(references + (signed char)record[0] * 8 + 4);
    byte *definition = g_4e3b44[model & 0xffff].bytes;
    word const *indices = *(word **)(definition + 0x2c);
    s_octree_output *node = &g_4c5700[handle & 0xff];
    if (function_12de70((s_geometry_block_info *)block, 3)
        && function_12de70((s_geometry_block_info *)(definition + 0x38), 3))
    {
        struct { real a, b; } local_opacity_min_opacity_scale_record = { *(real *)(definition + 8), *(real *)(definition + 0xc) };
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
            if (!*(word *)(draw + 6)) continue;
            vector3f packed_vector;
            function_4dd70(&packed_vector, *(dword *)(vertex + 0x10));
            dword coordinates = *(dword *)(vertex + 4);
            point3f position;
            function_3f220(coordinates & 0x3ff, (coordinates >> 10) & 0x3ff, coordinates >> 20, &position);
            real fraction = ((packed >> 16) & 0xf) * (1.0f / 15.0f);
            real alpha = (*(real *)(vertex + 0x14) - local_opacity_min_opacity_scale_record.a) * local_opacity_min_opacity_scale_record.b;
            color3f light, color;
            function_4ca00((s_4ca40_colors *)settings, vertex, &light, &color);
            light.red *= color.red; light.green *= color.green; light.blue *= color.blue;
            position.x += center.x; position.y += center.y; position.z += center.z;
            real size = (1.0f - fraction) * *(real *)(settings + 0xc) + fraction * *(real *)(settings + 0x10);
            real angle = ((packed >> 9) & 0xf) * g_5259b4 * (1.0f / 15.0f);
            vector3f horizontal, vertical, direction;
            if (settings[8] & 1)
            {
                real sine = (real)sin(angle), cosine = (real)cos(angle);
                function_4dcd0(&direction, packed >> 20);
                vector3f perpendicular, cross;
                function_11d000(&direction, &perpendicular);
                cross.i = direction.j * perpendicular.k - direction.k * perpendicular.j;
                cross.j = direction.k * perpendicular.i - direction.i * perpendicular.k;
                cross.k = direction.i * perpendicular.j - direction.j * perpendicular.i;
                function_4efb0(&direction);
                horizontal.i = perpendicular.i * cosine + cross.i * sine;
                horizontal.j = perpendicular.j * cosine + cross.j * sine;
                horizontal.k = perpendicular.k * cosine + cross.k * sine;
                vertical.i = cross.i * cosine - perpendicular.i * sine;
                vertical.j = cross.j * cosine - perpendicular.j * sine;
                vertical.k = cross.k * cosine - perpendicular.k * sine;
            }
            else
            {
                transform4x3f rotation;
                function_141ce0(angle, 0.0f, 0.0f, &rotation);
                horizontal = rotation.forward;
                vertical = rotation.left;
                direction = rotation.up;
            }
            alpha = alpha < 0.0f ? 0.0f : alpha > 1.0f ? 1.0f : alpha;
            D3DDevice_SetVertexData4f(7, packed_vector.i, packed_vector.j, packed_vector.k, alpha);
            D3DDevice_SetVertexData4f(8, horizontal.i * size, vertical.i * size, direction.i * size, position.x);
            D3DDevice_SetVertexData4f(9, horizontal.j * size, vertical.j * size, direction.j * size, position.y);
            D3DDevice_SetVertexData4f(10, horizontal.k * size, vertical.k * size, direction.k * size, position.z);
            D3DDevice_SetVertexData4f(11, light.red, light.green, light.blue, 1.0f);
            long count = *(word *)(draw + 6);
            word const *vertices = indices + *(word *)(draw + 4);
            function_1cf50();
            D3DDevice_DrawIndexedVertices(D3DPT_TRIANGLELIST, count, vertices);
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

// @retail 0x4ebd0
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
    byte *block = *(byte **)(table + 0x14) + *(short *)(record + 6) * 0x2c;
    volatile long record_tag = (*(long **)((byte *)g_4e0350 + 0x37c))[(signed char)record[0] * 2 + 1];
    byte *definition = g_4e3b44[record_tag & 0xffff].bytes;
    byte *part = *(byte **)(definition + 0x14) + record[1] * 20;
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
    memcpy(constants, source + 0x48, 16);
    constants[4] = source[8] ? 1.0f : 0.0f;
    constants[5] = source[8] ? 0.0f : 1.0f;
    constants[6] = source[9] ? 1.0f : 0.0f;
    constants[7] = source[9] ? 0.0f : 1.0f;
    constants[8] = source[0xa] ? 1.0f : 0.0f;
    constants[9] = source[0xa] ? 0.0f : 1.0f;
    { long pair = 0; if (pair < 3) do {
        real *coordinates = *(real **)(source + 0x1c + pair * 4);
        constants[10 + pair * 2] = coordinates ? coordinates[0] : 0.0f;
        constants[11 + pair * 2] = coordinates ? coordinates[1] : 0.0f;
    
++pair;
} while (pair < 3); }
    memcpy(constants + 16, source + 0x28, 24);
    constants[22] = constants[23] = 0.0f;
    D3DDevice_SetVertexShaderConstant(81, transform, 5);
    D3DDevice_SetVertexShaderConstant(86, constants, 6);
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
        D3DDevice_SetTextureStageState(stage, D3DTSS_MAXMIPLEVEL, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MAXANISOTROPY, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_MIPMAPLODBIAS, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_COLORSIGN, 0);
        D3DDevice_SetTextureStageState(stage, D3DTSS_ALPHAKILL, 0);
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
            color3f selected_color = color ? *color : *(color3f *)g_468710;
            colors[i].red = selected_color.red;
            colors[i].green = selected_color.green;
            colors[i].blue = selected_color.blue;
            real *alpha = *(real **)(source + 0x84 + i * 4);
            colors[i].alpha = alpha ? *alpha : 1.0f;
        }
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

byte g_51080c, g_4b62a0;
s_bitmap_view *g_4b62ac;
short g_4670c0;
extern short g_4b9dd0, g_4b9dd2, g_4b9dd4, g_4b9dd6;
extern short_rectangle2d g_4b9dd8;
typedef void (__stdcall *t_1f070_glyph)(long, long, long, long, long,
    real, real, real, real, real, real, real, long, long);
void __stdcall function_1f490(long, long, long, long, long,
    real, real, real, real, real, real, real, long, long);
void __stdcall function_13f0e0(t_1f070_glyph callback, short_rectangle2d const *bounds,
    short *position, short_rectangle2d const *clip, short line_gap, real scale, dword const *text);
void function_36560(void);

// @retail 0x1f070
void function_1f070(short_rectangle2d const *bounds, short_rectangle2d const *clip,
    short *position, long line_gap, dword const *text, real scale,
    void const *color, void const *shadow, bool preserve)
{
    (void)&position; (void)&line_gap; (void)&text; (void)&scale;
    (void)&color; (void)&shadow; (void)&preserve;
    if (g_51080c || g_485602 != 0) return;
    ++g_4670c0;
    s_bitmap_view *bitmap = g_4b62a0 ? g_4b62ac : 0;
    if (!bitmap || !*text) return;
    short_rectangle2d wanted_bounds;
    if (bounds) wanted_bounds = *bounds;
    else
    {
        wanted_bounds = g_4b9dd8;
        wanted_bounds.top -= g_4b9dd0;
        wanted_bounds.left -= g_4b9dd2;
        wanted_bounds.bottom -= g_4b9dd0;
        wanted_bounds.right -= g_4b9dd2;
    }
    short_rectangle2d wanted_clip;
    if (!(clip)) {
        wanted_clip.left = 0;
        wanted_clip.top = 0;
        wanted_clip.right = g_4b9dd6 - g_4b9dd2;
        wanted_clip.bottom = g_4b9dd4 - g_4b9dd0;
    } else {
        long right = g_4b9dd6 - g_4b9dd2;
        long bottom = g_4b9dd4 - g_4b9dd0;
        wanted_clip.left = clip->left < 0 ? 0 : clip->left;
        wanted_clip.top = clip->top < 0 ? 0 : clip->top;
        wanted_clip.right = (short)(right > clip->right ? clip->right : right);
        wanted_clip.bottom = (short)(bottom > clip->bottom ? clip->bottom : bottom);
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
long function_3e380(long model_index, byte lod, transform4x3f const *nodes,
    byte const *permutations, long *node_count, long extra_bytes);
struct s_matrix_workspace
{
    long size;
    byte data[0x27000];
};
extern s_matrix_workspace g_487b18;
void function_151e0(long index);
bool function_13d9f0(long type);
extern dword g_4e6494;

// @retail 0x4c690
void function_4c690(long tag, transform4x3f const *nodes)
{
    byte context[0x78];
    signed char indices[16];
    long node_count;
    byte *volatile definition = g_4e3b44[tag & 0xffff].bytes;
    D3DDevice_SetRenderState(D3DRS_DEPTHCLIPCONTROL, 16);
    function_2c3b0((s_interface_function_context *)context);
    function_3e420(tag, NONE, indices, 4);
    if (function_3e320(tag, (byte const *)indices))
    {
        long handle = function_3e380(tag, 4, nodes, (byte const *)indices, &node_count, 0);
        bool available = handle != NONE;
        if (available)
        {
            s_shader_transform_table *table = 0;
            if (handle != NONE && handle >= 0)
                table = (s_shader_transform_table *)(g_487b18.data + (handle & 0x0fffffff));
            g_485a5c = table;
            function_1bd50(context);
            for (long index = 0; index < *(long *)(definition + 0x1c); ++index)
            {
                long section_index = (byte)indices[index];
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
                    word type = *(word *)part;
                    if (material_index == NONE || type == 0 || type == 5 || type == 1) continue;
                    byte *material = *(byte **)(definition + 0x64) + material_index * 0x20;
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
                            g_4b8448 = 0;
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
    void function_1fa50(short_rectangle2d const *bounds, void const *clip, void const *position,
        long line_gap, real scale, long color, void const *shadow) const;
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
            value = unicode_escape_character_lookup(*next, &recognized);
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
void c_1fa50::function_1fa50(short_rectangle2d const *bounds, void const *clip,
    void const *position, long line_gap, real scale, long color, void const *shadow) const
{
    dword text[2048];
    convert_1fa50_text((word const *)this, text);
    function_1f070(bounds, (short_rectangle2d const *)clip, (short *)position,
        line_gap, text, scale, (void const *)color, shadow, false);
}

// @retail 0x1fa30
void function_1fa30(word const *text, short_rectangle2d *bounds)
{
    ((c_1fa50 const *)text)->function_1fa50(bounds, 0, 0, 0, 1.0f, 0, 0);
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
    g_485ad4.lo = default_lower.value;
    g_485ad4.hi = default_upper.value;
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
extern real g_4857b4;
void function_0224f0(dword stage, D3DTEXTURESTAGESTATETYPE type, dword value);

PRIVATE __forceinline void draw_filter_quad(real left, real right, real top, real bottom)
{
    D3DDevice_SetVertexData2s(3, 0, 0);
    D3DDevice_SetVertexData4f(0, left, top, 16777215.0f, 16777215.0f);
    D3DDevice_SetVertexData2s(3, 1, 0);
    D3DDevice_SetVertexData4f(0, right, top, 16777215.0f, 16777215.0f);
    D3DDevice_SetVertexData2s(3, 1, 1);
    D3DDevice_SetVertexData4f(0, right, bottom, 16777215.0f, 16777215.0f);
    D3DDevice_SetVertexData2s(3, 0, 1);
    D3DDevice_SetVertexData4f(0, left, bottom, 16777215.0f, 16777215.0f);
}

// @retail 0x46ea0
void function_46ea0(long tag, point3f const *position)
{
    union { vector3f direction; real bounds[4]; } workspace;
    vector3f &direction = workspace.direction;
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
    screen[1] = (real)floor(screen[1] + 0.5f);
    real *bounds = workspace.bounds;
    real &left = bounds[0];
    real &right = bounds[1];
    real &top = bounds[2];
    real &bottom = bounds[3];
    left = (real)(screen[0] - 32.0f);
    top = (real)(screen[1] - 32.0f);
    right = (real)(screen[0] + 32.0f);
    bottom = (real)(screen[1] + 32.0f);
    if (!((real)(short)g_48564e > left && (real)(short)g_48564c > top &&
        right > (real)(short)g_48564a && bottom > (real)(short)g_485648)) return;
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
    D3DDevice::Begin(D3DPT_QUADLIST);
    D3DDevice_SetVertexData4f(0, left, top, 16777215.0f, 16777215.0f);
    D3DDevice_SetVertexData4f(0, right, top, 16777215.0f, 16777215.0f);
    D3DDevice_SetVertexData4f(0, right, bottom, 16777215.0f, 16777215.0f);
    D3DDevice_SetVertexData4f(0, left, bottom, 16777215.0f, 16777215.0f);
    D3DDevice::End();
    function_14bc0((short)g_4858b8, 0, true);
    function_1cc30(10);
    function_1c710(g_51f0f0);
    function_143c0(*(long *)(g_485a80 + 0x34), 0, 0, 0.0f);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, 3);
    D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, 3);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER, 2);
    D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER, 2);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPFILTER, 2);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
    D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);
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
    D3DDevice::Begin(D3DPT_QUADLIST);
    draw_filter_quad(left, right, top, bottom);
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
    function_0224f0(0, D3DTSS_MAXMIPLEVEL, 0);
    function_0224f0(0, D3DTSS_MAXANISOTROPY, 0);
    function_0224f0(0, D3DTSS_MIPMAPLODBIAS, 0);
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
        D3DDevice::Begin(D3DPT_QUADLIST);
        ++i;
        D3DDevice_SetVertexData4f(9, 0.0f, 0.0f, 0.0f, strength / i);
        D3DDevice_SetVertexData2s(3, 0, 0);
        D3DDevice_SetVertexData4f(0, left - offset, top - offset, 16777215.0f, 16777215.0f);
        D3DDevice_SetVertexData2s(3, 1, 0);
        D3DDevice_SetVertexData4f(0, right + offset, top - offset, 16777215.0f, 16777215.0f);
        D3DDevice_SetVertexData2s(3, 1, 1);
        D3DDevice_SetVertexData4f(0, right + offset, bottom + offset, 16777215.0f, 16777215.0f);
        D3DDevice_SetVertexData2s(3, 0, 1);
        D3DDevice_SetVertexData4f(0, left - offset, bottom + offset, 16777215.0f, 16777215.0f);
        D3DDevice::End();
    } while (i < 16);
}


extern long g_50943c, g_509440;
struct s_2cb30_state
{
	byte unknown00[0x2a78];
	long active;
};

struct s_2cb30_entry
{
	dword values[9];
};

struct s_2cb30_globals
{
	long current;
	long previous;
	s_2cb30_state *state;
	union
	{
		byte unknown0c[0x9b0 - 0xc];
		struct
		{
			byte weights[850];
			byte material_indices[850];
			long materials[192];
		};
	};
	long count;
	s_2cb30_entry entries[32];
	long entry_count;
};
extern s_2cb30_globals g_4c0b78;
struct s_render_part_context;
void function_41040(short index, long part_index, long group, byte weight,
    s_render_part_context const *context, long material_override, bool force,
    dword and_mask, dword or_mask);
__declspec(noinline) void *function_449e0(short index, bool load, bool instance);
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
        byte *geometry = (byte *)function_449e0((short)index, false,
            (bool)((entry->unknown00 >> 12) & 1));
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
struct s_depth_parameters_13450
{
    long identifier, mode;
    real distance;
};
s_depth_parameters_13450 g_4850c4;
extern bool g_485a74;
extern byte g_485607;
extern dword g_4b8344;
real g_4b893c;

// Disabled retail 0x13450; SDK Clear activation changes matched 0x14ac0.
bool function_13450(s_depth_parameters_13450 const *parameters)
{
    real inverse = 1.0f / g_485ad4.hi;
    g_4850c4 = *parameters;
    real lower = inverse * g_485ad4.lo * 16777215.0f;
    real upper = inverse * g_485ad4.hi * 16777215.0f;
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
dword g_4b8474;
extern byte g_4c19c0;
extern dword g_4c1a04;
struct s_2e9e0_saved { long tag; point3f position; };
extern s_2e9e0_saved g_4c19c4[4];
struct s_2e3f0_record;
long __stdcall function_2e9e0(byte const *context, s_2e3f0_record const *record);
void function_46ea0(long tag, point3f const *position);

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
    g_4b8438 = 0; D3DDevice_SetRenderState(D3DRS_ZENABLE, 0);
    g_4b82e0 = 0x203; D3DDevice_SetRenderState(D3DRS_ZFUNC, 0x203);
    g_4b82fc = 0; D3DDevice_SetRenderState(D3DRS_ZWRITEENABLE, 0);
    g_4b8450 = 0; D3DDevice_SetRenderState(D3DRS_ZBIAS, 0);
    g_4b8474 = 0; D3DDevice_SetVertexShader(0);
    g_51f3c8[1][0] = NULL;
    g_51f3c8[1][1] = NULL;
    g_51f3c8[1][2] = NULL;
    g_51f3c8[1][3] = NULL;
    function_1cf50();
    D3DPIXELSHADERDEF program;
    memset(&program, 0, sizeof(program));
    dword *fields = (dword *)&program;
    fields[0xd8 / 4] = 1;
    fields[0xd4 / 4] = 0x11002;
    fields[0x88 / 4] = 0x8081524;
    fields[0xb4 / 4] = 0xc5;
    fields[0x8c / 4] = 0xc0c0408;
    fields[0xb8 / 4] = 0xd4;
    fields[0x20 / 4] = 0x50f0004;
    fields[0x24 / 4] = 0xc0d1400;
    g_484f68 = program;
    function_1ccf0(&program);
    function_1c590((s_shader_cache *)g_51f0f0, *(long *)(*(byte **)(g_485a80 + 0x5c) + 0x54), 0);
    function_1c710(g_51f0f0);
    g_4c19c0 = false;
    g_4c1a04 = 0;
    function_20bb0(g_4b9ed4, 0xf, NULL, 0, false, (visibility_draw_callback)function_2e9e0);
    for (long i = 0; i < (long)g_4c1a04; ++i)
    {
        point3f position = g_4c19c4[i].position;
        function_46ea0(g_4c19c4[i].tag, &position);
    }
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
extern real g_4b9de4;
extern real g_4b9de0;
extern dword g_4b8474;

// @retail 0x4f7e0
bool __stdcall function_4f7e0(long tag, s_501e0 const *points, long count, short bitmap,
    real cosine, real sine, bool segments, bool perspective, bool filtered)
{
    short const volatile *bitmap_reference = &bitmap;
    short bitmap_index = *bitmap_reference;
    point2f rotations[256];
    s_501e0 midpoints[256];
    if (filtered)
    {
        if (!g_46713c)
        {
            g_4b8474 = 0;
            D3DDevice_SetVertexShader(0);
        }
        g_46713c = true;
    }
    else
        function_4fe20();
    short stage = filtered ? 1 : 0;
    bool skip = tag != NONE ? function_14560(tag, stage, 0, 1, bitmap_index) :
        function_14480(*(long *)(g_485a80 + 0x34), stage, bitmap_index);
    if (!skip)
    {
        function_47ea0(stage, 0);
        if (segments)
        {
            long total = count - 1;
            for (long i = 0; i < total; ++i)
            {
                function_501e0(points + i, perspective, midpoints + i);
                vector3f direction;
                direction.i = points[i + 1].field_0.x - points[i].field_0.x;
                direction.j = points[i + 1].field_0.y - points[i].field_0.y;
                direction.k = points[i + 1].field_0.z - points[i].field_0.z;
                real length = (real)sqrt((double)direction.i * direction.i + (double)direction.k * direction.k + (double)direction.j * direction.j);
                if (!(fabs(length) < 0.0001f))
                {
                    real reciprocal = 1.0f / length;
                    direction.i *= reciprocal;
                    direction.j *= reciprocal;
                    direction.k *= reciprocal;
                }
                point2f screen;
                if (function_30830(&midpoints[i].field_0, NULL, &direction, NULL, true, true, &screen))
                {
                    double angle = atan2(screen.y, screen.x);
                    rotations[i].x = (real)cos(angle);
                    rotations[i].y = (real)sin(angle);
                }
                else
                {
                    rotations[i].x = 1.0f;
                    rotations[i].y = 0.0f;
                }
            }
            s_501e0 const *volatile selected = midpoints;
            points = selected;
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
            real width = points[i].field_c > points[i].field_10 ? points[i].field_c : points[i].field_10;
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
                        constants[8] = 0.0f;
                        constants[9] = 0.0f;
                        constants[10] = 1.0f;
                        constants[11] = 0.0f;
                        constants[12] = 0.0f;
                        constants[13] = 0.0f;
                        constants[14] = 0.0f;
                        constants[15] = 1.0f;
                        constants[16] = 1.0f;
                        constants[17] = 0.0f;
                        constants[18] = 0.0f;
                        constants[19] = 0.0f;
                        constants[20] = 0.0f;
                        constants[21] = 1.0f;
                        constants[22] = 0.0f;
                        constants[23] = 0.0f;
                        constants[24] = 0.0f;
                        constants[25] = 0.0f;
                        constants[26] = 1.0f;
                        constants[27] = 0.0f;
                        constants[28] = 0.0f;
                        constants[29] = 0.0f;
                        constants[30] = 0.0f;
                        constants[31] = 1.0f;
                        D3DDevice_SetVertexShaderConstant(-78, constants, 8);
                        function_484b0(&position, adjusted_depth, width * scale, points[i].field_10 * scale,
                            rotations[i].x, rotations[i].y, points[i].field_14);
                    }
                }
            }
            else
                function_480a0(&points[i].field_0, width, points[i].field_10,
                    rotations[i].x, rotations[i].y, points[i].field_14);
        }
    }
    return !skip;
}

extern short g_485600;
extern word g_485648, g_48564a, g_48564c, g_48564e;
extern real g_485658;
extern dword g_4b82f8, g_4b8324;
bool __stdcall function_3a5c0(byte const *configuration, real low, real high, long mode,
    bool center, bool shift, bool clip, bool skip_state, real *depth_out);
void function_1cf80();

// @retail 0x37a60
bool __stdcall function_37a60(void *context)
{
    long view_span_x = (short)g_48564e - (short)g_48564a;
    long view_span_y = (short)g_48564c - (short)g_485648;
    byte *input = (byte *)context;
    byte *view = input + g_485600 * 0xb8;
    struct s_filter_vertex { real x, y; byte unknown08[0x10]; point2f coordinates[3]; };
    s_filter_vertex vertices[4];
    void (WINAPI *volatile vertex2f)(INT, float, float) = D3DDevice_SetVertexData2f;
    void (WINAPI *volatile vertex4f)(INT, float, float, float, float) = D3DDevice_SetVertexData4f;
    {

        real extent = *(real *)(view + 0x134 + 0 * 4);
        vertices[0].coordinates[0].x = extent * -1.0f;
        vertices[0].coordinates[0].y = extent * -1.0f;
        vertices[1].coordinates[0].x = extent;
        vertices[1].coordinates[0].y = extent * -1.0f;
        vertices[2].coordinates[0].x = extent * -1.0f;
        vertices[2].coordinates[0].y = extent;
        vertices[3].coordinates[0].x = extent;
        vertices[3].coordinates[0].y = extent;
        }
    {

        real extent = *(real *)(view + 0x134 + 1 * 4);
        vertices[0].coordinates[1].x = extent * -1.0f;
        vertices[0].coordinates[1].y = extent * -1.0f;
        vertices[1].coordinates[1].x = extent;
        vertices[1].coordinates[1].y = extent * -1.0f;
        vertices[2].coordinates[1].x = extent * -1.0f;
        vertices[2].coordinates[1].y = extent;
        vertices[3].coordinates[1].x = extent;
        vertices[3].coordinates[1].y = extent;
        }
    {

        real extent = *(real *)(view + 0x134 + 2 * 4);
        vertices[0].coordinates[2].x = extent * -1.0f;
        vertices[0].coordinates[2].y = extent * -1.0f;
        vertices[1].coordinates[2].x = extent;
        vertices[1].coordinates[2].y = extent * -1.0f;
        vertices[2].coordinates[2].x = extent * -1.0f;
        vertices[2].coordinates[2].y = extent;
        vertices[3].coordinates[2].x = extent;
        vertices[3].coordinates[2].y = extent;
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
    byte configuration[0x34] = { 0 };
    configuration[0] = true;
    real gain = *(real *)(view + 0x14c) * *(real *)(view + 0x148);
    real red = *(real *)(view + 0xec) * gain;
    real green = *(real *)(view + 0xf0) * gain;
    real blue = *(real *)(view + 0xf4) * gain;
    *(real *)(configuration + 8) = red < 0.0f ? 0.0f : (red > 1.0f ? 1.0f : red);
    *(real *)(configuration + 0x14) = green < 0.0f ? 0.0f : (green > 1.0f ? 1.0f : green);
    *(real *)(configuration + 0x20) = blue < 0.0f ? 0.0f : (blue > 1.0f ? 1.0f : blue);
    real r = *(real *)(input + 0x98);
    real g = *(real *)(input + 0x9c);
    real b = *(real *)(input + 0xa0);
    *(real *)(configuration + 0x28) = r < 0.0f ? 0.0f : (r > 1.0f ? 1.0f : r);
    *(real *)(configuration + 0x2c) = g < 0.0f ? 0.0f : (g > 1.0f ? 1.0f : g);
    *(real *)(configuration + 0x30) = b < 0.0f ? 0.0f : (b > 1.0f ? 1.0f : b);
    real depth;
    if (function_3a5c0(configuration, *(real *)(input + 0x80), *(real *)(input + 0x84), 3,
        false, false, true, true, &depth))
    {
        real ratio = g_485ad4.hi / (g_485ad4.hi - g_485ad4.lo);
        real z = ((ratio * depth - ratio * g_485ad4.lo) / depth) * 16777215.0f;
        if (z < 0.0f) z = 0.0f;
        else if (z > 16777215.0f) z = 16777215.0f;
        real w = (depth / g_485ad4.hi) * 16777215.0f;
        if (w < 0.0f) w = 0.0f;
        else if (w > 16777215.0f) w = 16777215.0f;
        real x = (real)(short)g_48564a;
        real y = (real)(short)g_485648;
        real width = (real)view_span_x;
        real height = (real)view_span_y;
        g_4670bc = true;
        function_16b10((s_render_reset_state *)g_485b48);
        vertices[0].x = x - 0.5f; vertices[0].y = y - 0.5f;
        vertices[1].x = width + x - 0.5f; vertices[1].y = y - 0.5f;
        vertices[2].x = x - 0.5f; vertices[2].y = height + y - 0.5f;
        vertices[3].x = width + x - 0.5f; vertices[3].y = height + y - 0.5f;
        function_14bc0((short)g_4858b8, 0, true);
        function_14f60(0, 3);
        D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSU, 3);
        D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSV, 3);
        D3DDevice_SetTextureStageState(0, D3DTSS_ADDRESSW, 3);
        D3DDevice_SetTextureStageState(0, D3DTSS_MAGFILTER, 1);
        D3DDevice_SetTextureStageState(0, D3DTSS_MINFILTER, 1);
        D3DDevice_SetTextureStageState(0, D3DTSS_MIPFILTER, 0);
        D3DDevice_SetTextureStageState(0, D3DTSS_MAXANISOTROPY, 0);
        D3DDevice_SetTextureStageState(0, D3DTSS_MIPMAPLODBIAS, 0);
        D3DDevice_SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
        D3DDevice_SetTextureStageState(0, D3DTSS_COLORSIGN, 0);
        D3DDevice_SetTextureStageState(0, D3DTSS_ALPHAKILL, 0);
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
            D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSU, 1);
            D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSV, 1);
            D3DDevice_SetTextureStageState(stage, D3DTSS_ADDRESSW, 1);
            D3DDevice_SetTextureStageState(stage, D3DTSS_MAGFILTER, 2);
            D3DDevice_SetTextureStageState(stage, D3DTSS_MINFILTER, 2);
            D3DDevice_SetTextureStageState(stage, D3DTSS_MIPFILTER, 2);
            D3DDevice_SetTextureStageState(stage, D3DTSS_MAXMIPLEVEL, 0);
            D3DDevice_SetTextureStageState(stage, D3DTSS_MAXANISOTROPY, 0);
            D3DDevice_SetTextureStageState(stage, D3DTSS_MIPMAPLODBIAS, 0);
            D3DDevice_SetTextureStageState(stage, D3DTSS_COLORSIGN, 0);
            D3DDevice_SetTextureStageState(stage, D3DTSS_ALPHAKILL, 0);
        }
        function_1cf80();
        function_1cbe0(0x15, 0, 0);
        function_1c710(g_51f0f0);
        D3DDevice_Begin((D3DPRIMITIVETYPE)9);
        vertex2f(1, vertices[0].x, vertices[0].y);
        vertex2f(2, vertices[0].coordinates[0].x, vertices[0].coordinates[0].y);
        vertex2f(3, vertices[0].coordinates[1].x, vertices[0].coordinates[1].y);
        vertex2f(4, vertices[0].coordinates[2].x, vertices[0].coordinates[2].y);
        vertex4f(0, vertices[0].x, vertices[0].y, z, w);
        vertex2f(1, vertices[1].x, vertices[1].y);
        vertex2f(2, vertices[1].coordinates[0].x, vertices[1].coordinates[0].y);
        vertex2f(3, vertices[1].coordinates[1].x, vertices[1].coordinates[1].y);
        vertex2f(4, vertices[1].coordinates[2].x, vertices[1].coordinates[2].y);
        vertex4f(0, vertices[1].x, vertices[1].y, z, w);
        vertex2f(1, vertices[2].x, vertices[2].y);
        vertex2f(2, vertices[2].coordinates[0].x, vertices[2].coordinates[0].y);
        vertex2f(3, vertices[2].coordinates[1].x, vertices[2].coordinates[1].y);
        vertex2f(4, vertices[2].coordinates[2].x, vertices[2].coordinates[2].y);
        vertex4f(0, vertices[2].x, vertices[2].y, z, w);
        vertex2f(1, vertices[3].x, vertices[3].y);
        vertex2f(2, vertices[3].coordinates[0].x, vertices[3].coordinates[0].y);
        vertex2f(3, vertices[3].coordinates[1].x, vertices[3].coordinates[1].y);
        vertex2f(4, vertices[3].coordinates[2].x, vertices[3].coordinates[2].y);
        vertex4f(0, vertices[3].x, vertices[3].y, z, w);
        D3DDevice_End();
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
s_245c0_state *g_485890;
real g_51ecf0[256];
real g_4b9fd0, g_4b9fd4, g_4b9fd8;
extern short g_485600;
extern long g_485898;
__declspec(noinline) void function_245c0(s_245c0_state *state, real *histogram, short mode);
void __stdcall function_214f0(real passes, real distortion, real strength, real falloff,
    real scale, bool blend, bool preserve);
bool function_022710();
real function_134fc0(long index);

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
        real distortion = 0.9f;
        function_137a0(&strength, &extra);
        if (state)
            distortion = (1.0f - (1.0f - state->range[4])) * 0.9f;
        if (strength > 0.0f)
        {
            strength = (extra + 1.0f) * strength * 0.5f;
            strength = 0.0f > strength ? 0.0f : strength > 1.0f ? 1.0f : strength;
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
        struct { real weights[4]; s_filter_vector grid[4]; } scratch;
        s_filter_vector *grid = scratch.grid;
        real top = (real)g_4b9dd0;
        real left = (real)g_4b9dd2;
        real bottom = (real)g_4b9dd4;
        real right = (real)g_4b9dd6;
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
                    real *weights = scratch.weights;
                    weights[0] = weights[1] = weights[2] = weights[3] = 0.0f;
                    if (*(short *)(entry + 0x20) != NONE)
                        weights[0] = function_134fc0(*(short *)(entry + 0x20));
                    if (*(short *)(entry + 0x22) != NONE)
                        weights[1] = function_134fc0(*(short *)(entry + 0x22));
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

// @retail 0x4f010
void __stdcall function_4f010(void *payload)
{
    s_501e0 points[256];
    s_42850_payload *state = (s_42850_payload *)payload;
    if (state->b == NONE) return;
    byte *definition = g_4e3b44[state->b & 0xffff].bytes;
    if (*(long *)(definition + 8) <= 0) return;
    real fade = 1.0f;
    vector3f direction;
    direction.i = g_4b9da0.x - state->position.x;
    direction.j = g_4b9da0.y - state->position.y;
    direction.k = g_4b9da0.z - state->position.z;
    real distance = function_30bf0(&direction);
    if (*(real *)(definition + 4) > *(real *)definition)
        fade = clamp_callback((*(real *)(definition + 4) - distance) /
            (*(real *)(definition + 4) - *(real *)definition), 0.0f, 1.0f);
    fade *= state->width;
    if (!(fade > 0.0f)) return;
    g_46713c = true;
    g_4670bc = true;
    function_16b10((s_render_reset_state *)g_485b48);
    for (long group = 0; group < *(long *)(definition + 8); ++group)
    {
        byte *entry = *(byte **)(definition + 0xc) + group * 0x98;
        long count = *(long *)(entry + 0xc);
        if (count > 0)
        {
            bool filtered = (bool)((*(dword *)entry >> 3) & 1);
            real facing = (real)fabs((double)direction.k * state->first.k +
                (double)direction.j * state->first.j + (double)direction.i * state->first.i);
            real angle = clamp_callback(1.0f - facing, 0.0f, 1.0f);
            real time = 1.0f - function_13b390(entry + 0x30, angle, 0.0f);
            real step = 1.0f / (real)(count - 1);
            real strength = 0.0f, horizontal = 1.0f, vertical = 0.0f;
            real output[2] = {1.0f, 1.0f};
            real start = 1.0f, multiplier = 1.0f, inverse = 1.0f, offset = 0.0f;
            byte *positions = *(long *)(entry + 0x60) == count ? *(byte **)(entry + 0x64) : NULL;
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
                colour.red = clamp_callback(state->third.i * colour.red, 0.0f, 1.0f);
                colour.green = clamp_callback(state->third.j * colour.green, 0.0f, 1.0f);
                colour.blue = clamp_callback(state->third.k * colour.blue, 0.0f, 1.0f);
                colour.alpha = clamp_callback(opacity * fade, 0.0f, 1.0f);
                points[i].field_0.x = state->first.i * length + state->position.x;
                points[i].field_0.y = state->first.j * length + state->position.y;
                points[i].field_0.z = state->first.k * length + state->position.z;
                points[i].field_c = width * output[0];
                points[i].field_10 = width * output[1];
                points[i].field_14 = pack_color4f(&colour);
            }
            function_4f7e0(*(long *)(entry + 8), points, *(long *)(entry + 0xc), 0,
                horizontal, vertical, false, false, filtered);
        }
    }
    if (!g_46713c)
    {
        g_4b8474 = 0;
        D3DDevice_SetVertexShader(0);
    }
    g_46713c = true;
}


// Disabled: prior callback activation loses matched 0x12b2a0; counter storage is shared.
#if 0
extern s_connection_counter g_485ab0;
extern s_connection_counter g_485ab8;
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
extern s_font_cache_state_1eff0 g_4b62a0;
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



// Disabled: canonical font render storage has 256 entries, while retail traverses 512.
#if 0
extern s_record_pool *g_54d574;

// Retail 0x1f2b0
void function_1f2b0()
{
    if (g_4b62a0.active)
    {
        for (long i = 0; i < 512; ++i)
        {
            long character = g_4b62a0.entries[i].character_index;
            if (character != NONE && i != NONE)
            {
                if (character != NONE)
                    *(long *)(g_54d574->data + (character & 0xffff) * 0x38 + 0x34) = NONE;
                g_4b62a0.entries[i].character_index = NONE;
            }
        }
    }
}
#endif



struct s_record_source;
struct s_record_sources
{
    dword unknown00;
    long count;
    dword flags;
    long first;
    dword unknown10;
    s_record_source *sources;
};
extern s_record_sources g_4c1a48[3];
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



// Disabled: the shared glyph atlas ring currently allocates 256 of retail's 512 slots.
#if 0
struct s_glyph_header_1f5c0
{
    word value00, pixels_size;
    short width, height;
    byte unknown08[8];
};
struct s_glyph_entry_1f5c0
{
    long character;
    short x, y;
};
struct s_glyph_cache_1f5c0
{
    bool active;
    byte unknown01;
    short first, next;
    short x, y, row_height;
    byte *bitmap;
    s_glyph_entry_1f5c0 entries[512];
};
extern s_glyph_cache_1f5c0 g_4b62a0;
extern s_record_pool *g_54d574;
void function_140960(long character, word *pixels);
byte *function_1d5e0(byte *bitmap, long mode, long *pitch);

PRIVATE inline void glyph_remove_first_1f5c0(bool *removed)
{
    long first = g_4b62a0.first;
    *removed = true;
    if (first != NONE)
    {
        long character = g_4b62a0.entries[first].character;
        if (character != NONE)
            *(long *)(g_54d574->data + (character & 0xffff) * 0x38 + 0x34) = NONE;
        g_4b62a0.entries[first].character = NONE;
    }
    g_4b62a0.first = (g_4b62a0.first + 1) & 0x1ff;
}

// Retail 0x1f5c0
void __stdcall function_1f5c0(long character_index)
{
    word pixels[4096];
    if (character_index != NONE
        && *(long *)(g_54d574->data + (character_index & 0xffff) * 0x38 + 0x34) != NONE)
        return;
    byte *character = g_54d574->data + (character_index & 0xffff) * 0x38;
    s_glyph_header_1f5c0 *header = 0;
    if (*(long *)(character + 0x10) == 4)
        header = (s_glyph_header_1f5c0 *)(character + 0x1c);
    bool removed = false;
    if (header->width + g_4b62a0.x > 256)
    {
        g_4b62a0.y += g_4b62a0.row_height;
        g_4b62a0.x = 1;
        g_4b62a0.row_height = 0;
    }
    if (header->height + g_4b62a0.y > 256)
    {
        g_4b62a0.y = 1;
        g_4b62a0.x = 1;
        g_4b62a0.row_height = 0;
        while (g_4b62a0.first != g_4b62a0.next && g_4b62a0.entries[g_4b62a0.first].y > 0)
            glyph_remove_first_1f5c0(&removed);
    }
    if (header->height + 1 > g_4b62a0.row_height)
    {
        long old_end = g_4b62a0.y + g_4b62a0.row_height;
        long new_end = g_4b62a0.y + header->height;
        while (g_4b62a0.first != g_4b62a0.next
            && g_4b62a0.entries[g_4b62a0.first].y >= old_end
            && g_4b62a0.entries[g_4b62a0.first].y < new_end)
            glyph_remove_first_1f5c0(&removed);
        g_4b62a0.row_height = header->height + 1;
    }
    if (((g_4b62a0.next + 1) & 0x1ff) == g_4b62a0.first)
        glyph_remove_first_1f5c0(&removed);
    long slot = g_4b62a0.next;
    if (character_index != NONE)
        *(long *)(g_54d574->data + (character_index & 0xffff) * 0x38 + 0x34) = slot;
    s_glyph_entry_1f5c0 *entry = &g_4b62a0.entries[slot];
    entry->character = character_index;
    entry->x = g_4b62a0.x;
    entry->y = g_4b62a0.y;
    function_140960(character_index, pixels);
    long pitch;
    byte *destination = function_1d5e0(g_4b62a0.bitmap, removed ? 1 : 0, &pitch);
    if (destination)
    {
        short first_x = entry->x - 1;
        if (first_x < 0) first_x = 0;
        short first_y = entry->y - 1;
        if (first_y < 0) first_y = 0;
        short end_x = entry->x + header->width + 1;
        short width = *(short *)(g_4b62a0.bitmap + 4) - 1;
        if (end_x > width) end_x = width;
        short end_y = entry->y + header->height + 1;
        short height = *(short *)(g_4b62a0.bitmap + 6) - 1;
        if (end_y > height) end_y = height;
        for (long y = first_y; y < end_y; y++)
            for (long x = first_x; x < end_x; x++)
                *(word *)(destination + y * pitch + x * 2) = 0x0fff;
        long source = 0;
        for (long y = 0; y < header->height; y++)
        {
            word *row = (word *)(destination + (entry->y + y) * pitch + entry->x * 2);
            for (long x = 0; x < header->width; x++) row[x] = pixels[source + x];
            source += header->width;
        }
        bitmap_predict_inline((s_bitmap_predict_view *)g_4b62a0.bitmap, 7);
    }
    g_4b62a0.x += header->width + 1;
    g_4b62a0.next = (g_4b62a0.next + 1) & 0x1ff;
}
#endif



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
extern byte *g_5093e4;
extern byte g_51f408[0x40c0];
extern long g_485a58;
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
    g_5093e4[0] = g_5093e4[1] = 0;
    for (long i = 4; i <= 0x14; i += 4) *(real *)(g_5093e4 + i) = -1.0f;
    g_5093e4[0x18] = 1;
    g_5093e4[0x19] = 0;
    *(real *)(g_5093e4 + 0x1c) = -1.0f;
    *(real *)(g_5093e4 + 0x20) = -1.0f;
    g_5093e4[0x1a] = 0;
    memset(g_51f408, 0, sizeof(g_51f408));
    for (long i = 0; i < 0x1ff; ++i) function_20e50(i);
    if (effects) *(real *)effects->unknown190 = 0.0f;
    g_485a58 = 0;
}
#endif

// Disabled: retail indexes all 512 glyph atlas entries; the foreign canonical ring still has 256 slots.
#if 0
extern long g_4e28f4[11];
extern s_record_pool *g_54d574;
extern s_glyph_entry_1f5c0 g_4b62b0[512];
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
    s_glyph_entry_1f5c0 *rectangle = &g_4b62b0[entry];
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

// Disabled: the frame copies overlap independently declared shared camera/projection globals.
#if 0
struct s_frame_view_2c560 { byte data[0x298]; };
extern s_frame_view_2c560 g_485600;
extern byte g_48574c[0x120];
extern dword g_5093b4, g_5093b8;
extern vector3f g_4b9dac;
extern byte g_4670cd;
dword __cdecl function_131ed0(void const *color);
void function_25a10(void const *view, point3f *position, vector3f *forward);

// Retail 0x132d0
void __stdcall function_132d0(s_frame_view_2c560 const *frame)
{
    g_485600 = *frame;
    short mode = *(short const *)(frame->data + 2);
    g_4858b8 = mode == 1 ? 18 : 0;
    function_15370(0);
    g_5093b4 = 0x901;
    g_5093b8 = 0x900;
    function_12fa0((real const *)(frame->data + 0x8c), frame->data + 0x18,
        frame->data[7] != 0, mode);
    if (*(short *)(g_485600.data + 8))
    {
        dword color = 0;
        dword flags = 1;
        if (!g_4670cd)
        {
            flags = 0x61;
            color = function_131ed0(g_485600.data + 0xc);
        }
        function_14bc0((short)g_4858b8, 0, true);
        function_14b60(flags, color, 1.0f, 0);
    }
    else function_14bc0((short)g_4858b8, 0, true);
    D3DDevice_SetVertexData4f(7, 0.0f, 0.5f, 1.0f, 42.0f);
    D3DDevice_SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    g_4670bc = 1;
    function_16b10((s_render_reset_state *)g_485b48);
    if (frame->data[0x26c])
    {
        memcpy(g_48574c, frame->data + 0x14c, 0x120);
        function_25a10(g_48574c, &g_4b9da0, &g_4b9dac);
    }
}
#endif

// Disabled: retail publishes the persistent shared render-source address and needs the blocked 14b60 flags interface.
#if 0
struct s_render_mode_44370
{
    long target;
    dword clear_flags, field_8_7;
    real clear_depth;
    byte clear_stencil, unknown11[7];
    short cull_mode, unknown1a;
    long group, unknown20;
};
extern s_render_mode_44370 g_43fa1c[25];
extern byte g_4ba022, g_4ba023, g_4ba024;
extern long g_467130, g_467134, g_4858b4;
extern s_record_sources *g_467138;
extern dword g_4ba014;
void function_15680(short mode);
void function_495e0();
void function_496f0();

// Retail 0x44370
bool __stdcall function_44370(long mode)
{
    s_render_mode_44370 const *settings = &g_43fa1c[mode];
    long group = settings->group;
    s_record_sources *sources = &g_4c1a48[group];
    bool enabled = true;
    if ((1 << mode) & 0xbffdee) enabled = (sources->flags & (1 << mode)) != 0;
    if (enabled)
    {
        if (mode == 2) enabled = g_4ba023 != 0;
        else if (mode == 8) enabled = g_4ba022 != 0;
        else if (mode == 17) enabled = g_4ba024 != 0;
    }
    if (enabled)
    {
        g_467130 = mode;
        g_467134 = group;
        g_467138 = sources;
        long target = settings->target;
        if (target == NONE)
            target = mode == 10 ? ((g_4ba014 & 0x10) && !(g_4ba014 & 0x20) ? 10 : 9) : g_4858b8;
        g_4858b4 = target;
        function_14bc0((short)target, 0, true);
        if (settings->clear_flags)
        {
            if (!g_485a75 && !g_485a76)
                function_14b60(settings->clear_flags, settings->field_8_7, settings->clear_depth, settings->clear_stencil);
        }
        else if (mode == 10)
        {
            if ((g_4ba014 & 0x10) && !(g_4ba014 & 0x20)) function_14b60(1, 0xffffff, 0.0f, 0);
            else function_14b60(0x60, 0, 1.0f, 0);
        }
        function_15680(g_485898 >= 4 && g_485898 <= 7 ? 0 : settings->cull_mode);
        switch (mode)
        {
        case 3: case 16: enabled = true; break;
        case 10: enabled = true; function_495e0(); break;
        case 11: enabled = true; function_496f0(); break;
        case 17: g_485a74 = 1; enabled = true; break;
        }
    }
    else g_467130 = 0;
    function_15370(mode == 3 || mode == 1);
    return enabled;
}
#endif



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
extern dword g_510800_pool_base;
extern long g_510804_pool_size;
bool function_12200();
void global_preferences_initialize();
void function_120a90();
void function_121fc0();
void function_2136c0();
void function_123180();
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
    function_123180();
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
    function_123310();
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
#if 0
// Retail 0x13c20
void function_13c20()
{
    if (g_4ba04c > 1) D3DDevice_Clear(0, 0, 0x80, 0, 1.0f, 0);
    --g_46701c;
    if (g_46701c >= 0)
    {
        byte *frame = (byte *)&g_4850d0 + g_46701c * 0x298;
        function_132d0((s_frame_view_2c560 const *)frame);
        g_4b9ed4 = *(short *)frame;
        memcpy(&g_4b9ef0, frame + 0x14c, 0x120);
        memcpy(&g_4b9da0, frame + 0x18, 0x74);
        real bounds[4];
        function_2f970((s_2f970_view const *)&g_4b9da0, bounds);
        function_2fd90((s_2f800_view const *)&g_4b9da0, (box2f const *)bounds, (byte *)&g_4b9e14);
    }
}
#endif

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



// Disabled: the dual timed-effect pass publishes a new shared visibility-pool address and depends on blocked 2bcd0.
#if 0
// Retail 0x13680
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
    bool world_effects = !skip_effects && g_485898 != 7 && function_22710();
    if (!function_16ac0() && !skip_effects && effect && effect->unknown3c &&
        effect->unknown40 > 0.0f && function_22710() && function_01de20(18))
    {
        function_2bcd0(1, 1, false, world_effects, 1, 1, effect->unknown40);
        g_4c0b78.state = (s_2cb30_state *)&g_547f88;
        function_2bcd0(1, 1, false, world_effects, 2, 2, effect->unknown40);
        function_21a80(effect->unknown44, effect->unknown48, effect->unknown40);
    }
    else function_2bcd0(1, 1, draw_ui, world_effects, 0, 0, 0.0f);
}
#endif

