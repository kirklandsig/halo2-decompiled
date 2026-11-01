// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "unknown_184250.h"
#include <xmmintrin.h>

struct s_type_1e6529;
class c_havok_reference_counted;
extern c_havok_reference_counted *g_4f55b0;
extern byte *g_4ed280;

struct s_184250
{
	long field_0;
	byte field_4[0x30 - 4];
	point3f field_30;
};

struct s_184252
{
	byte field_0[4];
	real field_4;
	byte field_8[0x20 - 8];
	real field_20;
	real field_24;
};

/* a four-float vector with its SSE view */
union s_184253
{
	struct
	{
		real field_0;
		real field_4;
		real field_8;
		real field_c;
	};
	__m128 quad;
};

struct s_184254
{
	long *field_0;
	long field_4;
	dword field_8;
	~s_184254()
	{
		if (!(field_8 & 0x80000000))
			g_480118->allocate((long)field_0, (field_8 & 0x7fffffff) * 4, 0x12);
	}
};

struct s_184257
{
	s_184254 field_0;
	long field_c[0x200];
};

class c_184250
{
public:
	virtual void function_184251() {}
	virtual void function_184252() {}
	virtual void function_184253() {}
	virtual void function_184254() {}
	virtual void function_184255() {}
	virtual void function_184256() {}
	virtual void function_184257() {}
	virtual void function_184258() {}
	virtual void function_184259() {}
	virtual void function_18425a() {}
	virtual void function_18425b(s_184253 const *, s_184254 *) {}
};

struct s_184255
{
	short field_0;
	short field_2;
	long field_4;
	byte field_8[0x20 - 8];
};

/* a block of s_184255 elements */
struct s_184256_block
{
	long count;
	s_184255 *elements;
};

struct s_184256
{
	byte field_0[0x20c];
	s_184256_block field_20c;
};

void voice_fpu_enter(void);
void voice_fpu_leave(void);
long function_184000(long index, long bit);
void function_184060(long arg_0, long arg_1, s_type_1e6529 *arg_2, long arg_3);

// @retail 0x184250
void __stdcall function_184250(s_type_1e6529 const *arg_0)
{
	if (!*g_4ed280)
		return;
	s_184250 const *local_0 = (s_184250 const *)arg_0;
	s_184252 const *local_1 = (s_184252 const *)g_4e3b44[local_0->field_0 & 0xffff].bytes;
	if (local_1->field_20 == 0.0f && local_1->field_24 == 0.0f)
		return;
	real local_2 = local_1->field_4;
	bool local_3 = false;
	if (g_4f55b0)
	{
		s_184253 local_4;
		local_4.field_0 = local_0->field_30.x;
		local_4.field_4 = local_0->field_30.y;
		local_4.field_8 = local_0->field_30.z;
		local_4.field_c = 0.0f;
		{
			s_184257 local_5;
			s_184254 &local_6 = local_5.field_0;
			local_6.field_0 = local_5.field_c;
			local_6.field_4 = 0;
			local_6.field_8 = 0x80000200;
			voice_fpu_enter();
			real radius = local_2 < 0.0f ? 0.0f : local_2;
			{
				s_184253 local_7;
				local_7.quad = local_4.quad;
				local_7.field_c = radius;
				((c_184250 *)g_4f55b0)->function_18425b(&local_7, &local_6);
			}
			voice_fpu_leave();
			if (local_6.field_4 > 0)
			{
				s_184256_block const *block = &((s_184256 *)g_4e0348)->field_20c;
				for (long local_8 = 0; local_8 < local_6.field_4; local_8++)
				{
					s_184255 *local_9 = &block->elements[local_6.field_0[local_8]];
					long local_11 = local_9->field_2;
					long local_10 = local_9->field_0;
					if ((bool)(byte)function_184000(local_10, local_11))
					{
						function_184060(local_10, local_11, (s_type_1e6529 *)arg_0, local_9->field_4);
						local_3 = true;
					}
				}
			}
		}
		if (local_3)
		{
			function_2e90a0(s_184251(false));
		}
	}
}

struct s_184061
{
	long definition_index;
	dword flags;
	byte unknown08[0x54 - 8];
	real scale;
};

struct s_184062
{
	byte unknown00[6];
	short material_index;
};

struct s_184063
{
	byte unknown00[0x2c];
	s_184062 *references;
};

struct s_184064
{
	byte unknown00[0x70];
	s_184063 surfaces;
	byte unknowna0[0xc8 - 0xa0];
};

struct s_184065
{
	byte unknown00[0x34];
	short owner_index;
	byte unknown36[0x58 - 0x36];
};

struct s_184066
{
	byte unknown00[8];
	word material_index;
	byte unknown0a[0x14 - 0xa];
};

struct s_184067
{
	byte unknown00[0x10];
	s_184066 *materials;
	byte unknown14[4];
	s_184063 *surfaces;
	byte unknown1c[0x13c - 0x1c];
	s_184064 *owners;
	byte unknown140[4];
	s_184065 *references;
	byte unknown148[0x20c - 0x148];
	long bounds_count;
	s_184255 *bounds;
};

struct s_184068
{
	byte unknown00[0x10];
	long group0;
	long group1;
	byte unknown18[0x34 - 0x18];
	long definition_index;
};

struct s_184069_block
{
	byte unknown00[0xc];
	real base_scale;
	real minimum;
	real maximum;
	byte unknown18[0x40 - 0x18];
	long group0;
	long group1;
};
struct s_184069
{
	byte unknown00[0x10];
	s_184069_block block;
};

struct s_globals_element;
struct s_18444a;
s_globals_element *function_188690(short index);
byte *function_183fc0(long index);
void __stdcall function_184440(long index, long bit_index, s_18444a const *input, long surface_index);
real function_259a0(dword *seed);
real function_d5bc0(long group0, long group1, long group2, long group3);
void function_a87a0(long first, long second, long third, byte const *input);
void function_a8ee0(long first, long second);
void function_1c55e0(real const *bounds);

// @retail 0x184060
void function_184060(long index, long bit, s_type_1e6529 *input_data, long surface_index)
{
	long bit_index = bit;
	s_184061 const *input = (s_184061 const *)input_data;
	if (!*g_4ed280 || input->definition_index == NONE || !((bool)(byte)function_184000(index, bit_index)))
		return;

	s_184067 *globals = (s_184067 *)g_4e0348;
	s_184063 *surfaces = index != NONE ? &globals->owners[globals->references[index].owner_index].surfaces : globals->surfaces;
	short material_index = surfaces->references[surface_index].material_index;
	s_184068 *material = (s_184068 *)function_188690(globals->materials[material_index].material_index);
	if (material->definition_index == NONE)
		return;
	real const *resistance = (real const *)g_4e3b44[material->definition_index & 0xffff].bytes;
	if (!(*resistance > 0.0f))
		return;

	s_184069_block const *definition = &((s_184069 const *)g_4e3b44[input->definition_index & 0xffff].bytes)->block;
	real scale = (function_259d0((dword *)g_4e7408, NULL, 0, definition->minimum, definition->maximum) - definition->base_scale) * input->scale + definition->base_scale;
	real probability = function_d5bc0(definition->group0, definition->group1, material->group0, material->group1);
	probability *= scale;
	probability /= *resistance;
	if (TEST_FIELD_BIT((input->flags >> 8) & 1) ? 10.0f >= *resistance : probability >= function_259a0((dword *)g_4e7408))
	{
		dword *bits = (dword *)function_183fc0(index);
		bits[bit_index >> 5] &= ~(1 << (bit_index & 31));
		function_184440(index, bit_index, (s_18444a const *)input_data, surface_index);
		function_2e90a0(s_184251(false));
		function_a87a0(index, bit_index, surface_index, (byte const *)input_data);
		function_a8ee0(bit_index, index);

		for (long i = 0; i < ((s_184067 *)g_4e0348)->bounds_count; ++i)
		{
			s_184255 *bounds = &((s_184067 *)g_4e0348)->bounds[i];
			if (bounds->field_0 == index && bounds->field_2 == bit_index)
			{
				function_1c55e0((real const *)((byte *)bounds + 8));
				break;
			}
		}
	}
}
