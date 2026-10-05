// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include <xtl.h>
#include <string.h>
#include "unknown_058ee0.h"

/* the texture stages: [0] the textures set on the device (0x51f3c8), [1] the
   textures wanted (0x51f3d8) */
IDirect3DBaseTexture8 *g_51f3c8[2][4];
long g_5093d8;
dword g_487288[17][32];
D3DSurface *g_51f3f4;
D3DSurface *g_51f3f8;
byte g_51f3fc;

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
	dword *mask = g_487288[0];
	for (long count = 0; count <= 16; count++, mask += 32)
	{
		for (long x = 0; x < 32; x += 4)
		{
			long base = x;
			for (long rows = 8; rows > 0; rows--, base += 128)
			{
				for (long i = 0; i < count; i++)
				{
					long bit = g_43f188[i].y * 32 + base + g_43f188[i].x;
					mask[bit / 32] |= 1 << (bit % 32);
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
