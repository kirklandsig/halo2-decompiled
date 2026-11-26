// @flags /O2 /arch:SSE /Gr
#include "shader_storage.h"
#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "unknown_224240.h"
#include "effects.h"
#include "vertex_inputs.h"

struct s_particle_system_datum;
struct s_particle_location_datum;
struct s_particle_175610;
struct s_particle_value_cache;
struct s_draw_sprite_seed;
struct s_draw_value_cache;
struct s_draw_sprite_context;

typedef void (__stdcall *particle_callback_175610)(s_particle_175610 *, s_particle_value_cache *,
	point3f const *, vector3f const *, real, color4f const *, void *);

struct s_interface_function_context
{
	s_particle_system_datum const *field_0;
	real (__stdcall *field_4)(s_particle_system_datum const *, long);
	byte field_8[0x70];
};

struct s_2243c0
{
	s_interface_definition const *field_0;
	byte const *field_4;
	byte const *field_8;
	real field_c;
};

struct s_2243c1
{
	real field_0[17];
	dword field_44;
	void *field_48;
	void *field_4c;
	void const *field_50;
};

s_interface_function_context g_55edd8;
s_interface_function_context g_55ed60;
extern byte g_470a2c;
extern transform4x3f *g_4687d0;
void function_2c3b0(s_interface_function_context *);
void function_1bd50(void *);
real __stdcall function_173c30(s_particle_system_datum const *, long);
bool function_23ea80(long tag, transform4x3f const *matrix, long variant, long stage, long pass);
void __stdcall function_173ba0(dword, void *, void *, void const *, real *);
void function_175610(s_particle_value_cache *, s_particle_system_datum *, s_particle_location_datum *,
	particle_callback_175610, void *);
void __stdcall function_224550(s_draw_sprite_seed const *, s_particle_value_cache *,
	point3f *, point3f *, real, color4f const *, s_draw_sprite_context *);

// @retail 0x2243c0
void __stdcall function_2243c0(s_interface_definition *arg_0, long arg_1, long arg_2,
	long arg_3, long arg_4, long arg_5)
{
	s_2243c0 local_0;
	s_2243c1 local_1;
	byte const *local_2 = (byte const *)arg_0;
	if (*(long const *)(local_2 + 0x8c) > 0)
	{
		function_2c3b0(&g_55edd8);
		g_55edd8.field_0 = (s_particle_system_datum const *)arg_1;
		g_55edd8.field_4 = function_173c30;
		function_1bd50(&g_55edd8);
		if (function_23ea80(arg_3, g_4687d0, *(word const *)(local_2 + 4), arg_4, arg_5))
		{
			byte const *local_3 = *(byte const *const *)(local_2 + 0x90);
			local_1.field_44 = 0;
			local_1.field_48 = 0;
			local_1.field_4c = 0;
			local_1.field_50 = 0;
			local_0.field_0 = arg_0;
			local_0.field_c = 1.0f;
			local_0.field_4 = 0;
			local_0.field_8 = 0;
			long local_4 = **(short const *const *)(local_3 + 0x60);
			if (local_4 != NONE)
			{
				long local_5 = *(long const *)(*(byte const *const *)(local_3 + 8) + local_4 * 12);
				if (local_5 != NONE)
				{
					local_0.field_4 = g_4e3b44[local_5 & 0xffff].bytes;
					long local_6 = *(long const *)(local_0.field_4 + 0x3c);
					if (local_6 > 0)
						local_0.field_8 = *(byte const *const *)(local_0.field_4 + 0x40) +
							(*(short const *)(local_2 + 8) % local_6) * 0x3c;
					if (*(long const *)(local_0.field_4 + 0x44) <= 0)
						goto local_7;
					byte const *local_8 = *(byte const *const *)(local_0.field_4 + 0x48);
					short local_9 = *(short const *)(local_8 + 4);
					short local_10 = *(short const *)(local_8 + 6);
					if (local_9 != local_10)
						local_0.field_c = (real)local_9 / (real)local_10;
				}
			}
			{
			if (arg_1)
			{
				local_1.field_48 = (void *)arg_1;
				local_1.field_44 = 0;
			}
			if (arg_2)
			{
				local_1.field_4c = (void *)arg_2;
				local_1.field_44 = 0;
			}
			dword local_11 = *(dword const *)(local_2 + 0x98) & 0x107f0;
			function_173ba0(local_11, local_1.field_48, local_1.field_4c,
				local_1.field_50, local_1.field_0);
			local_1.field_44 |= local_11;
			function_175610((s_particle_value_cache *)&local_1,
				(s_particle_system_datum *)arg_1, (s_particle_location_datum *)arg_2,
				(particle_callback_175610)function_224550, &local_0);
			}
		local_7:
			g_470a2c = false;
		}
	}
}

struct s_224db0
{
	s_interface_definition const *field_0;
	long field_4;
};

struct s_shader_cache;
struct s_geometry_block_info;
struct s_draw_mesh_seed;
struct s_draw_mesh_context;
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
c_type_4e7709 *function_137bd0(long);
bool function_12de70(s_geometry_block_info *, dword);
void function_1c590(s_shader_cache *, long, long);
void function_1c620(s_shader_cache *, long, long, long, byte const *);
void __stdcall function_1c710(void *);
void function_1cdd0(real const *, byte);
void function_1bbf0(long, long, long, long, long, real);
void __stdcall function_224fd0(s_draw_mesh_seed const *, s_draw_value_cache *,
	point3f const *, vector3f const *, real, color4f const *, s_draw_mesh_context const *);

/* the address of element index of the tag block at block (count, then address) */
static __forceinline void const *block_element_224db0(byte const *block, long index, long size)
{
	return *(byte const *const *)(block + 4) + index * size;
}

// score 117 (was 117): m3 lane 16 r179 and lane 5 r233 unchanged (their lines just above the marker). The fallback masks the tag in place in retail (`and edx, 0xffff`, ours `movzx eax, dx`), the stage
// and pass element loads are unfused in retail, and the definitions take other scratch registers; +0xd8 is read late.
// MiniMax D (11-20, round 132): no new form, no lower score (117): the body is unchanged. Masking the fallback tag index
// in place gives retail's `and edx, 0xffff` but moves the tag table from EDI into ECX and hoists its load above the frame
// (152), while the word cast keeps the table in EDI and gives `movzx eax, dx` (117); the stage and pass homes (ESI and EBP
// against ours' EBP and EDX) and the unfused element loads still differ, and the tag table's own address moves between
// the reduced and the full image.
// MiniMax C (11-20): no new form, no lower score (117): read the earlier rounds' forms; the only lower one known is the
// score-8 chain, and it rests on one-use element bindings and moved stores, so it is kept only with an exact match. The
// stage and pass homes (ESI and EBP against ours' EBP and EDX) and the unfused element loads are unchanged.
// MiniMax B (11-20): no new form, no lower score (117): the fallback tag index is still zero-extended through
// EAX where retail masks the 32-bit value in place (`and edx, 0xffff`); the in-place mask moves the tag table
// from EDI into ECX and hoists its load above the frame (152), while the `(word)` cast keeps the table in EDI
// but gives `movzx eax, dx` (117), and masking the index inside the subscript reproduces retail's `and` only
// with that same table move. The stage and pass homes (ESI and EBP against ours' EBP and EDX) and the unfused
// element loads still differ. No form found this round.
// MiniMax C (11-15): no new form kept, no lower score (117, was 117): the fallback path still masks the tag index with
// a word load (`movzx eax, dx`) where retail masks the 32-bit value in place (`and edx, 0xffff`); masking the index
// inside the subscript reproduces retail's `and` but moves the tag table from EDI into ECX and hoists its load above
// the frame (152), so the original is kept, and casting the index to a word keeps the same code. The stage and pass
// homes (ESI and EBP against ours' EBP and EDX) and the unfused element loads are unchanged. The score-8 chain needs
// the head, stage and pass elements bound by reference and is kept only with an exact match.
// MiniMax I (11-17): no new form, no lower score (117): the else path's in-place `and edx, 0xffff` needs the `& 0xffff`
// spelling, which hoists the tag table global into ECX before `sub esp` (152), while the `(word)` cast keeps the table in
// EDI but gives `movzx eax, dx` (117); the mask assigned back to the tag, a masked-tag local and a named element address
// leave the code unchanged (117) or worse (152). The block chain's argument registers and unfused element loads stand.
// Earlier at 117 (from 117): Retail masks the fallback tag in EDX and materializes its stage and pass element addresses separately.
// Earlier: The 0x5c frame agrees. The fallback tag index is zero-extended through EAX, and the stage/pass and later tag-data loads use different registers.
// MiniMax H (11-18), at 117 (from 117): no new form, no lower score: the permuter (301 s, 36 tries on the reduced
// image, then the full image) found nothing below 117, and the full image confirms the same 117. The fallback tag index
// is still zero-extended through EAX where retail masks the 32-bit value in place (and edx, 0xffff), and the stage and
// pass elements still land in other registers than retail's ESI and EBP with the element loads unfused. The score-8 chain
// needs the head, stage and pass elements bound by reference and is kept only with an exact match, so the plain-pointer
// body stands.
// Earlier at 117 (from 117): The 0x5c frame agrees. The fallback tag index is zero-extended through EAX, and the stage/pass and later tag-data loads use different registers.
// Earlier 117 (was 117): The 0x5c frame agrees. The fallback tag index is zero-extended through EAX, and the stage/pass and later tag-data loads use different registers.
// M3 lane 8 r170 (fast, match-only screens on lane 13 r59's saved score-8 chain, main's body unchanged):
// the chain rebuilt on this main 8 (first +0x9e); on it the fallback reference unbound 10, the definition reference unbound
// 10 (both move only the head: the pass definition stays EDX, the material tag ECX and the definition EDX, so the late
// registers are not a rotation carried from the head); the pass element passed straight into a by-value inline accessor
// for its +4 tag, no binding, 121 (no dead lea, pass definition still EDX); diagnostic: the element passed as an unused
// first argument of the definition lookup, evaluated after the tag load, 121 (the unused argument is removed with its lea).
// Earlier first line 117 (was 117). The tag index masked as a 32-bit value inside the
// subscript reproduces retail's `and edx, 0xffff` but moves the base global from EDI into ECX and raises the score
// to 152, so the original is kept; the tag lookup still masks the index with a word load where
// retail masks the 32-bit value, and the fallback EDX, the stage/pass homes and the unfused element
// loads remain.
// MiniMax G (11-14): no new form, no lower score (117, was 117): masking the tag index in place (`local_tag &= 0xffff`), masking a copy of it, or masking it inside the subscript each reproduce retail's `and edx, 0xffff` but move the tag table from EDI into ECX and hoist its load above the frame (152); casting the index to a word keeps the `movzx eax, dx` form (117), and the stage and pass homes and the unfused element loads are unchanged.
// MiniMax B (11-12): no new form, no lower score (117, was 117): the fallback path still masks the tag with a word
// load where retail masks the 32-bit value (`and edx, 0xffff`), the stage and pass elements still land in other
// registers than retail's ESI and EBP, and the element loads are still unfused; the score-8 chain needs the
// elements bound by reference, which the rules allow only with an exact match.
// MiniMax D (11-11): no new form, no lower score (117); the fallback masked in place (`local_tag & 0xffff`) does reproduce retail's `and edx, 0xffff` but moves the tag table from EDI into ECX (152 alone, 149 with the four cache zeros stored together), and the stage and pass arguments still land in other registers than retail's ESI and EBP; the four cache zeros stored together do give retail's four consecutive stores but score 120.
// M3 lane 1 r96 (fast, 0x2243c0 and 0x224d90 matched each time): block_element_224db0 defined out of line in another
// file so LTCG inlines it at link time 117 (identical code: link-time inlining leaves no unfused load or dead `lea`);
// the tag re-read from the definition at each of its three uses 135 (first difference +0x0, the group index read as a
// word); a const instance pointer 117; a const byte view of the definition 117; the tag-data result declared after the
// instance 117.
// MiniMax D (11-10): no new form, no lower score (117); retail masks the fallback tag index in place in
// EDX (`and edx, 0xffff` then `shl edx, 4`), ours zero-extends the index into EAX (`movzx eax, dx`) and
// keeps the scaled index in EAX.
// retail masks the fallback tag index in place in EDX where ours zero-extends it into EAX;
// the stage/pass homes and the unfused element loads still differ (main's body unchanged in round 168).
// Earlier: the tag index masked as a 32-bit value inside the subscript
// reproduces retail's `and edx, 0xffff` but moves the base global from EDI into ECX and raises the
// score to 152, so the original is kept; the tag lookup still masks the index with a word load where
// fallback EDX masking, stage/pass homes and the unfused element loads remain; older notes below.
// Earlier 117 (was 117), MiniMax: the tag index masked as a 32-bit value inside the subscript
// reproduces retail's `and edx, 0xffff` but moves the base global from EDI into ECX and raises the score
// MiniMax I (11-09): no new form, no lower score (117); the fallback index masked in place (`local_tag & 0xffff`)
// gives retail's `and edx, 0xffff` but moves the tag table from EDI into ECX (152). The round 59 chain rebuilt
// (by-value tag accessor, head/stage/pass elements bound, grouped zeros, material index bound) measures 86 with
// arg_0 on the stack; with the flags read early 75 (544 bytes, arg_0 back in EBX); the group test re-reading the
// tag is forwarded by the compiler (no change); with no instance local 155; with the tag read at each use 88.
// The chain is a score-picked form (one-use element bindings, moved stores) and is not kept without a byte match.
// MiniMax D (11-09): no new form, no lower score (117); the tag declared unsigned long gives the same code as the unsigned-short cast, and the tag masked inside the subscript gives 152 with the base global in ecx.
// MiniMax R (11-09): the masked tag held in a local or in a short local gives 152 because the base global moves from
// edi into ecx, and the index masked inside the subscript or cast to an unsigned short gives 117 with the same code.
// MiniMax R (11-08): no new form, no lower score (117); the fallback lookup's masked tag index still takes a
// fresh register where retail masks it in place in edx, and the element loads and the fallback path's call order
// still differ.
// MiniMax D (11-06): no new form, no lower score; the fallback lookup still zero-extends the tag index where retail masks it in place in edx, the element reads still use fused addressing with arg_3 in ebp instead of esi, and the four cache zeros still land before the material lookup.
// MiniMax H (11-06): no new form, no lower score; the fallback lookup's masked tag index still takes a fresh
// register where retail masks it in place in edx, and the element loads and the fallback path's call order still
// differ; the only lower form needs by-value tag accessors with the head, stage and pass elements bound, which
// is a score-picked form.
// MiniMax A (11-05): no new form, no lower score; the fallback lookup's tag index and the element loads still sit in different registers than retail's.
// MiniMax H (11-03): no new form, no lower score; the fallback lookup masks the tag into a fresh register where retail masks it in place in edx, and the in-place form scores 152 against 117, so it was dropped.
// Earlier (117, was 117): retail keeps the masked index in EDX where ours reuses EAX for the table
// Earlier 117 (was 117): retail keeps the masked index in EDX where ours reuses EAX for the table
// entry, and the fallback path's call order and the element loads still differ; the file handle
// comes in through a declaration outside this file.
// Frame 0x5c and first thirteen instructions agree; fallback EDX masking, stage/pass homes and unfused element loads differ.
// Earlier: frame 0x5c agrees; saved EDI table ownership, fused lookups and scratch allocation remain.
// Earlier: ECX table hoist versus saved EDI and lookup-register ownership still differ.
// Unsigned root/leaf handles 149/149; signed masked indices 149; const input 149;
// cache-tail memset 150; callback seed initializer 160; mapping element 149;
// native block-helper return type 149. Early mask read and matched wrapper retained.
// Earlier: ECX table hoist, lookup roles and word-load materialization remain.
// Native root entry value/binding 194/149; native input field 149, plus entry value 194;
// unmasked first/second/both words and native-input combination 149/149/149/149.
// table stays in ECX; saved-register and lookup order still differ.
// Cache initialization in success/entry 150/164; early callback input 162; both lifetimes 162;
// cached marker count, geometry result and missing-mask result 149; delayed cache/count 150.
// Earlier: table ownership and lookup schedule still differ.
// stride-first helper 149f; ordinal-first helper 149f.
// element-address helper 149f; stride template 150f.
// byte helper stride 149f; root handle at use 140f.
// direct pass value 149f; nested ordinals 149f.
// root/direct pass 140f; root/nested ordinals 140f.
// tag table stays in ECX; saved-register and lookup order still differ.
// Existing input/render fields and both 149; ordinary block inline 149; group snapshot 163;
// shared pointer zeros 148 (dropped); callback search field 150; validation failure return 149.
// Earlier: table still enters ECX before the frame rather than saved EDI.
// Existing element helper out of line 189; type query, geometry test, value callback,
// value update and context reset noinline declarations 140 each; unsigned extent 140;
// native int ordinal 140. All temporary helper contracts and declarations restored.
// Earlier: retained table register and unfused lookups still differ.
// Typed count/address helper pointer/reference, unsigned ordinal/extent, byte result,
// ordinary inline and two typed formal orders all 140; original helper restored.
// Earlier: saved table register and unfused lookup addresses still differ;
// group reads at use 156; cache/mask/pointer zeros before final lookup 141/156/159;
// callback input before lookup 141; group reads with cache/mask/pointer zeros 157/136/136.
// Earlier: saved table ownership and unfused element addresses remain.
// Byte/word element-reference results 136/136; element-type pointer/reference templates 187/187;
// each with full root snapshot 144/144/197/197. Original helper and group/mask lifetimes retained.
// saved table ownership and native lookup addresses remain.
// Forced stride template 167; with root/word ordinals/root and words 171/167/171;
// with const input/direct ordinal/late mask/root and late mask 167/167/165/165.
// Earlier: unfused element addresses, arg/temporary registers and the zero stores remain.
// Tag index read once (table moves to retail's EDI, first difference +0xe) 135; plus flags read at
// the call 162; plus lookups in the record_material shape 135; plus all four zero stores after the
// lookups 163; plus a tag entry pointer for the group tests 152; pointer and late zeros 149.
// frame 0x5c and EAX input agree; saved EDI table ownership and unfused lookups differ.
// Five native byte-block formal orders 136 each; accessor result lifetime 136;
// extent-first product 136; target inline contract 136. Original helper retained.
// Round 403: native cache 136; existing cache method 136; method root value 144; method mask at use
// 164; method chained zeros 156; method zero boundary 156; method native context 136; method search
// output 159. Opening body restored.
// Earlier: the else path's tag index is zero-extended into a fresh register instead of
// masked in place, and the loaded tag pointer is defined in ECX instead of EDX; the group test's
// instance address is materialized and the first 13 instructions now match.
// Round 216: native tag-value join, signed/unsigned and early/producer declarations 133 each;
// conditional native value initialization signed/unsigned 133/133; value join with final cache zeros
// signed/unsigned 143/143. Original pointer join restored.
// Round 102 (fast): the else path's index as `local_tag & 0xffff` 152 (retail's in-place `and edx,
// 0xffff`, but the tag table moves from EDI to ECX, loaded before the frame); the flags field read just
// before function_173ba0 149 (arg_0 becomes a stack parameter), both 156; the second and third block
// elements through named element pointers 117 each (identical code).
// Round 31: flags read just before function_173ba0 149 (arg_0 then lives in EBP and goes on the stack);
// the first cache zero store moved down with the other three 120 (retail's grouping); that with the
// fallback index masked 149; flags late with the grouped zeros 151; flags late with no byte alias 149.
// Round 233: packed mapping element/native directory/both/marker/element-marker/directory-marker/
// all schemas/all plus low-word fallback mask: 117/117/117/117/117/117/117/152. Original data views and
// accessor retained; fallback EDX masking and stage/pass/unfused-load homes remain.
// Round 321: Actual search ordinal as found/exhausted result with twice-used stage/pass values and entry/producer declaration subsets: all eight 141 at 544/544 bytes.
// Round 321: Original separate candidate result, parameter uses, lookup and mask lifetimes, helpers and body restored; fallback/register homes remain. Stacked notes merged.
// Round 100 (lane 8, fast): the second and third element pointers bound to `void const *const &` locals 127 (the second
// element is then unfused as retail), the third alone 128, the second alone 128, the second plus the third's field through
// a `long const &` 127, the third's field through a `long const &` alone 135.
// Round 115: retail's chain lookup is `lea eax,[eax+edx*2]; movzx eax,word [eax]` and the pass element's
// address is computed and left unused; the matched 0x17420 gets that load from a packed-range struct pointer
// read through its inline index()/count(). Chain/entry through that struct's index() 117 (same code); flags
// read after the search loop 132; the three pointer fields zeroed in one chained assignment 117; the head in
// 0x16b90's direct-indexing shape 117; search result written straight into the context 136. Reference
// locals for the lookups compile identically (compile-only, not counted).
// Round 10 (lane 17): an unused read of the pass element's first dword (diagnostic for the dead `lea`) 117 (the
// read is dropped before allocation, no `lea`); the four zero stores grouped with the fallback masked `& 0xffff`
// 149; the group test through the existing TAG_GROUP macro and the fallback through TAG_GET 135 (retail's in-place
// `and edx, 0xffff` and EDI table, but the group read folds into `[eax+edi]` where retail adds then loads), the
// TAG_GROUP test with the original word fallback 165; the index sums written parameter first 117 (same code);
// the source-table pointer chosen by one ternary 117 (same code); the first cache zero store before the pass
// element read 139.
// Round 116 (lane 6, fast): the 0x1c620 call's argument setup agrees with retail. Stage/pass sums written
// `arg_3 + local_7` / `arg_4 + local_8` 117 (same code); that with the first cache zero moved down 120; the
// two sums named where computed 117 (same code); the stage offset folded into the first ordinal 117, and the
// pass offset into the second as well 117. Retail loads arg_3/arg_4 into ESI/EBP before the chain and
// leaves a dead `lea eax, [eax + edx*2]` after the pass element read; ours loads them late into EBP/EDX.
// Round 61 (fast): the three blocks read through the existing s_tag_data (size, address) view 117 (same
// code); the stage/pass parameters as named const copies at the top 117; the first ordinal as `word`
// 117; the pass element's field as `[1]` of a long pointer 117; the group test through
// `g_4e3b44[(short)local_tag]` with no instance local 165.
// Round 26 (m3 lane 2, fast): the head is the body of the existing inline material_parameters (unknown_0167a0.cpp,
// whose matched 0x16b90 walks the same chain): that form 135 (in-place fallback mask, folded group read), with the
// grouped cache zeros 163; the original head with a `& 0xffff` fallback and grouped zeros 149; the second +0x100
// read named for the 0x1c590 call 117 (same code); the geometry test as an early return 117. The grouped zeros
// alone re-measure 120: the chain is then two instructions short (retail's unfused ordinal load and dead `lea`).
// Round 57 (lane 16, fast): the group tag read once into a named dword tested twice 165 (first difference +0xe);
// the first ordinal read straight from block 0's address 117; the geometry test as an early return 117; the
// cached pointers compared field first 119; no byte alias of the definition (casts of arg_0) 117 (same code).
// Lane 18 r14 (one-link LTCG env, exact for the first 82 instructions; counts over those, control 51): fallback
// `local_tag & 0xffff` 67; the group as a dword local 73, with the masked fallback 73; the tag read at each use
// 70; the two arms as one ternary 51, swapped arms with the negated test 51, `(dword)(word)` fallback 51 (same code).
// Round 230: In-place fallback normalization with original/root/directory/nested/before-cache
// lifetimes 152/152/152/152/152; original handle with root/directory/nested scopes 117/117/117.
// Lane 9 round 67 (fast, which reproduces 117): the stage and pass elements bound to `void const *const &` locals
// with all four cache zero stores after the material lookup 118: retail's unfused ordinal load, its dead pass-element
// `lea` and its zero grouping all appear, but arg_3 stays in EBP (retail ESI, arg_4 EBP), block 2's address is
// loaded early and the material index is not copied through ESI; with the flags read just before 0x173ba0 the check
// hung in Wine (unmeasured).
// Round 48 (m3 lane 11, fast; control 117): the fallback through the existing TAG_GET with the original group test
// 152 (the same code as `local_tag & 0xffff`: in-place mask as retail, but the tag table moves into ECX ahead of the
// frame); every tag lookup through TAG_GET 152; TAG_GET fallback with the stage element through a `word const *`
// 152; the pass element read through a 10-byte element struct pointer (`tag_index` at +4) 117 (same code), with
// the stage element pointer too 117 (same code): a pointer-valued element alone does not leave retail's dead `lea`.
// Round 230: In-place fallback normalization with original/root/directory/nested/before-cache
// lifetimes 152/152/152/152/152; original handle with root/directory/nested scopes 117/117/117.
// Lane 18 round 16 (fast): the fallback through the existing TAG_GET 152 (table leaves EDI); the four zero stores
// grouped after the third lookup 120; that with the TAG_GET fallback 149; the group test as a two-case switch on the
// instance's group with the fallback as default 165; the fallback read through the same instance pointer 168.
// Env only (first 82 instructions, control 53): the switch with a `& 0xffff` fallback 75, through TAG_GROUP/TAG_GET
// 75, a const group local with a `& 0xffff` fallback 75.
// Round 230: In-place fallback normalization with original/root/directory/nested/before-cache
// lifetimes 152/152/152/152/152; original handle with root/directory/nested scopes 117/117/117.
// Round 128 (m3 lane 6, fast; control 117): the fallback masked `& 0xffff`, the four cache zeros grouped and
// the flags read just before 0x173ba0 together 154 (first difference +0x0, arg_0 leaves EAX); the pass
// element's field through a named `long const *` 117 (same code); the pass block's address named right
// before its read (`local_passes`, retail reads it after the second ordinal) 117 (same code), with the grouped
// zeros 120, with the masked fallback 152.
// Round 48 (m3 lane 11, fast; control 117): the fallback through the existing TAG_GET with the original group test
// 152 (the same code as `local_tag & 0xffff`: in-place mask as retail, but the tag table moves into ECX ahead of the
// frame); every tag lookup through TAG_GET 152; TAG_GET fallback with the stage element through a `word const *`
// 152; the pass element read through a 10-byte element struct pointer (`tag_index` at +4) 117 (same code), with
// the stage element pointer too 117 (same code): a pointer-valued element alone does not leave retail's dead `lea`.
// Round 40 (lane 13, fast): dword tag with a `& 0xffff` fallback 152; the `& 0xffff` fallback with the four
// cache zeros grouped 149 (head and stage in ESI as retail, but the tag table goes to ECX above the frame
// where retail keeps it in EDI); the grouped zeros with the second ordinal read through a named element
// pointer 120 (still fused); on the 149 form the sums written parameter first 149, the root handle named 149.
// Round 495: Native standard predicate search with signed/unsigned result, cache-zero
// boundary and fallback-mask combinations 199/199/203/203/211/211/210/210. Original body
// restored; fallback normalization and stage/pass homes remain.
// Round 231: On the saved-EDI entry graph, repeated shader input with grouped zeros/late
// mask/reused stage-pass subsets 146/168/162/164/146/168/162/164. Late-mask forms lose the
// matched wrapper; all restored. Fallback masking and stage/pass homes remain.
// Round 277: standard find with original/final zeros and word/masked fallback 132/155/136/153;
// standard equality-predicate search on the same four graphs 199/211/203/210. Original body restored.
// Round 193 (lane 3, fast; control 117): the flags read at each use (in the call's mask and the cache
// update after it) 166 (re-read after the call; arg_0 leaves EAX); TAG_GROUP test, TAG_GET fallback and
// the four cache zeros grouped 163; the late flags local through the stored definition (local_0.field_0)
// 149, declared first and assigned late 149, as long 149: with the read late our allocator gives arg_0
// EBP (arg_4 takes EBX) and LTCG then passes it on the stack, where retail keeps it in EBX from EAX.
// Retail's group read (add then load), unfused stage element (lea then movzx) and dead pass-element lea
// look like tag/block accessors inlined at link time; no out-of-line accessor exists here to call.
// Round 495: Native standard predicate search with signed/unsigned result, cache-zero
// boundary and fallback-mask combinations 199/199/203/203/211/211/210/210. Original body
// restored; fallback normalization and stage/pass homes remain.
// Round 336: both word ordinals loaded before mask/remainder, long/dword,
// with original/final-handle normalization 117/117/152/117/117/117/152/117.
// Original complete body restored; fallback EDX masking, stage/pass ownership,
// unfused element loads and the early mask lifetime remain.
// Round 338: reference-value identity with tag/stage/pass scalar lexical priorities,
// including independent mapping declarations, 117 each in eight forms. Original
// complete body restored; fallback EDX normalization and stage/pass/unfused-load homes remain.
// Round 59 (m3 lane 1, fast; all restored, one-use reference forms stay match-only). Chain, each on the last: head, stage and
// pass elements each bound to a `void const *const &` temporary from block_element_224db0, the four cache zeros grouped after
// the material lookup 102 (the stage and pass alone 118); fallback masked and the fallback and tables lookups through a
// by-value forceinline tag accessor bound the same way 99; flags read just before 0x173ba0 89 (551 bytes, arg_0 goes on the
// stack through EBP); the tag read a second time for the group test (arg_0 back in EBX from EAX) 106 (544 bytes, aligned 10);
// the material index for 0x1c590 bound to `long const &` from a by-value accessor 8. Left, 7 lines: retail overwrites the index
// base with the loaded definition pointer (EAX at the pass definition, EDI at the material definition, the zero's xor just
// before the shift), ours takes a fresh EDX. On 8: pass definition bound by reference 36, material definition 92 (552 bytes),
// both 86; material tag bound first 36; pass records bound 8, reused local 8, nested block elements 8, bound 36; zeros between
// the material index and its lookup 41, before the lookup statement 13, as a memset 97; through a local pointer 8, an inline
// clearing helper 8; a const-reference tag parameter 8; argument copies 36, 92, 86; bound ordinal sums 131, 110, 110, 111;
// random combinations of these forms (with reads of the 0x9c field at each use) 10 at best.
// Round 340: existing cache-update method on the root-local/EDI-entry graph,
// early/late mask, original/final zero boundary and native callback-context combinations
// 117/149/120/151/117/149/120/151. Late masks lose the thunk; every body/type restored.
// Fallback EDX normalization and stage/pass/unfused-load homes remain.
// Round 343: Group-classification result bool/byte/word/long, positive and complementary:
// 122/122/122/122/122/122/122/122. Original dispatch, lookups, mask observation and call tuples
// restored.
// Round 350: Initially empty first/second/both receiver predicates 134/131/135; grouped initialization
// 138; first/second/both receivers constructed bound 159/159/177, late construction 181. Fallback
// masking and stage/pass homes remain.
// Round 352: In-place unsigned fallback mask/remainder with twice-used stage/pass/both/reverse
// entry captures: 152 each in eight forms. Original full-handle dispatch, parameter uses, early
// mask observation, search, helper and all calls restored; fallback/stage/pass and unfused-load
// differences remain.
// Round 355: actual two-byte mapping records with nine-bit ordinals, first/second/both
// 117/117/117; each with final grouped cache zeros 120/120/120; both with masked fallback
// 152, with both boundaries 149. Original data views and body restored; fallback/stage/pass homes remain.
// Round 40 (m3 lane 2, fast; all on the round 59 score-8 chain, restored; match-only forms). Left at 8: pass definition EDX (retail EAX), material index
// ECX (EDX), material definition EDX (EDI), the zero's `xor` after the +8 read. Zeros before the material lookup 13, before the pass record read 60, before
// the pass tag 18. The geometry pointer as a named local at five places 8 (forwarded), at entry 68 (551 bytes); bound to a const reference from a by-value
// accessor before the pass tag or record read 33/33, before the material lookup 90 (552: index EDX as retail, material definition EAX), before or after the
// zeros 8/8; with earlier zeros 97/97/38. Material definition named or bound, then the zeros, then its +8 read 95/95 (552: definition in EDI and the `xor` at
// retail's place, index still ECX, the +8 read after the stores, the 0x1c620 setup flips). With the geometry bound first or after 68/68/82/82.
// Grid 1 (geometry none/before material/before zeros/before pass; pass definition, material definition, records, pass tag each plain/bound):
// 8/59/36/104/92/119/86/103/36/105/76/107/86/104/60/105
// 90/118/112/104/62/117/88/103/112/105/76/107/88/104/60/102
// 8/119/86/104/79/119/86/103/86/105/76/107/86/104/60/105
// 33/111/87/112/107/110/107/111/87/113/107/114/107/112/105/109
// Grid 2 (pass chain plain/definition bound/records bound/definition named/one reused local; geometry none/before pass/before material/before zeros/after
// zeros; material inline/definition named across zeros/bound across zeros/+8 read named across zeros/+8 read bound across zeros):
// 8/95/95/110/110/33/111/111/128/128/90/68/68/105/105/8/82/82/110/110/8/82/82/110/110
// 36/90/90/110/110/87/113/113/114/114/112/92/92/113/113/86/90/90/106/106/86/90/90/106/106
// 36/90/90/110/110/87/113/113/114/114/112/92/92/113/113/86/90/90/106/106/86/90/90/106/106
// 8/95/95/110/110/33/111/111/128/128/90/68/68/105/105/8/82/82/110/110/8/82/82/110/110
// 8/95/95/110/110/33/111/111/128/128/90/68/68/105/105/8/82/82/110/110/8/82/82/110/110
// Then on the chain: the pass tag not named, const or dword 8/8/8; the material tag named 8, with the zeros after it 41.
// DL238: Complete actual directory mutable/immutable/memcpy/memmove values 195/195/195/202; with
// final grouped cache zeros 196/196/196/203. Original whole body, directory observations, helper,
// cache/mask lifetime, includes and all calls restored; fallback EDX masking, stage/pass and
// unfused-load homes remain.
// DH310: Existing two-user cache constructor, member/body/chained/mixed defaults at entry/final lookup,
// 172/172/172/172/120/120/120/120. Original record, object lifetimes and executable body restored;
// all eight demote the matched sibling and are rejected; fallback EDX masking, stage/pass and unfused loads remain.
// Lane 17 r36 (fast): all four cache zeros at the top 172; all four right after the pass element read 148;
// the first zero before the first ordinal 151; the pass tag as dword 117 (same code); local_6 const 117 (same code).
// Round 33 (m3 lane 18, fast; 0x224d90 and 0x2243c0 matched each time): the element helper's product
// as `size * index` 117 (same code); the stage and pass ordinals as short 117 (same code); the tables
// lookup as one ternary 117 (same code); the group tag in a named dword for both tests 165; the
// search result stored straight into the record 136. All restored.
// Round 390: Complete callback context after search, field/aggregate construction,
// crossed with grouped cache zeros and in-place fallback: 147/149/158/154/147/149/158/154.
// Original complete body restored; fallback EDX, stage/pass and unfused loads remain.
// Round 82 (m3 lane 12, fast; 0x224d90 and 0x2243c0 matched each time): the context's definition stored
// after the search loop 147, at the top of the draw block 117; the root tag local passed to 0x1bbf0 130
// (first difference +0x2); first cache zero after the material record lookup 117; the search count read
// once before the loop 117. Original body restored.
// Round 129 (m3 lane 8, fast; 0x2243c0 and 0x224d90 matched each time): round 59's chain rebuilt with a by-value
// forceinline tag-data accessor (fallback and tables bound by reference), head/stage/pass elements bound, grouped
// zeros, flags read before 0x173ba0, the group test re-reading the tag and the 0x1c590 material index bound from a
// by-value accessor: 8 (same seven lines: pass definition EDX where retail overwrites EAX, material index ECX/EDX,
// definition EDX/EDI, late zero xor). On it: pass and material lookups through the accessor 8; material lookup
// through both accessors 8; pass/material chains through block_element_224db0 (element 0) 8; local_10/local_11
// declared at the top 8; first zero before the material lookup 23; the material index reference used for the
// lookup as well 76. Match-only chain; original body restored.
// Round 390: Complete callback context after search, field/aggregate construction,
// crossed with grouped cache zeros and in-place fallback: 147/149/158/154/147/149/158/154.
// Original complete body restored; fallback EDX, stage/pass and unfused loads remain.
// Round 131 (m3 lane 8): the type-14 search through the shared vertex_input_index (include/vertex_inputs.h, the
// search 0x116b00 makes four times) in place of the hand-written loop: identical code, 117.
// Round 59 (m3 lane 13, fast; 0x2243c0 and 0x224d90 checked each time): m3 lane 1 round 59's score-8 chain rebuilt (8; the four
// cache zeros must stay in field order, 12 otherwise); a permuter run on it found nothing below 8 in 49 tries. On the
// chain: the record pointer formed after its +0x232 read (named pointer advanced) 132; the cache zeros before the
// material lookup 13 (the stores then precede the loads); the material record as element 1 of a 0x1c-byte block 8
// (same code); the material definition bound from the by-value tag accessor, with that record 92 and inside its own
// block 92 (552 bytes: the definition takes EDI as retail, but the 0x1c620 and 0x1bbf0 argument registers swap and a
// loop pad appears); that with the 0x1c590 index unbound 90 (0x224d90 lost), the head unbound 38 (544 bytes: material
// and pass definitions as retail, the head block's registers differ), the fallback unbound 47, the head bound in its own
// block 92, head and stage unbound 135, the tag definition unbound 94, the stage unbound 124. Original body restored.
// Round 50 (m3 lane 15, fast; 0x224d90 and 0x2243c0 matched each time): the first two cache zeros together
// ahead of the material lookups 150; the group constants tested 'prt3' first 119; the search loop as
// `!= 14` continue 117 (same code); the cache pointers compared field first 119; the context's definition
// stored before 0x1bbf0 117 (same code). (A chained zero of the three pointers does not compile: field_50 is const.)
// B417: Completed stream-input region with mask-phase ending, callback-context producer storage
// and native cache/context role-name subsets: 117/117/117/117/117/117/117/117 at 544/544.
// Original full body restored; fallback EDX masking and stage/pass/unfused-load homes remain.
// N355: Actual head-address receiver with stage/pass/material-definition receiver subsets:
// 117/117/117/117/117/117/117/117. NOT kept: no exact match from the tested structural forms.
// Complete original executable source restored; Fallback EDX normalization and stage/pass/unfused-load homes remain.
// Round 147 (m3 lane 6, fast). Round 59's score-8 chain rebuilt 8 (first +0x9e; group test variants on the way 134, 89, 112, 106, 67, 91).
// On it, with 0x2243c0 and 0x224d90 matching: the element helper with a named base and offset 8; pass and material definitions named
// 8; the material index named 8; the pass tag through `long const &` 8; the material definition bound 92 (552 bytes). Those two
// not checked: the pass element bound in its own block 8; the four zeros in reverse field order 12. With the callee files the harness reproduces the
// first 60 lines (the same 8, 88 overall): helper with a named base, offset, both in either order, a named result, base and result 8
// each; pass tag through a member struct 8; the records pointer named before the +0x132 132 (22 in those lines); the material
// definition bound with the head unbound, a pointer or direct 107 (index EDX, the `xor` and EDI as retail, but the registers above
// them rotate). Ours rotates EDX, EAX, ECX, EDX through the pass and material definitions; retail reuses EAX over its dead `lea`.
// Match-only chain; original body restored. A 600 s permuter run on the chain: 59 tries (24 failed to build), none below 8.
// B428: Ordinary inline caller declarations for 0x137bd0/12de70/1c590/1c620/
// 1c710/1cdd0/2c3b0/1bd50: 117/117/117/117/117/117/117/117. NOT kept: no gain.
// Every declaration and whole body restored; fallback EDX, stage/pass ownership,
// unfused element loads and mask lifetime remain.
// M3 lane 13 r63 (fast, restored; 0x2243c0 and 0x224d90 checked): chain-8 rebuilt from the recipe; tag read before the instance is what gets 8 from 91 (552 bytes); instance from local_tag 67 (0x224d90 lost). On 8: pass definition bound 36, material bound 92 (552),
// both 86 (0x224d90 lost); named copies of either or both 8 (same code); the 0x20 block pointer named 8; the pass field read twice through a by-value element pointer 121, through a bound one with no local 8; lookups through the by-value accessor 8. Same 7 lines left.
// B441: Complete cache-phase mask/system/location scalar carriers, subsets 1/2/4/3/5/6/7,
// then signed mask: 127/120/120/128/120/123/129/129. NOT kept: all regress from 117 at 544/544;
// original field observations/publications and complete body restored. Fallback and stage/pass homes remain.
// B443: Geometry admission in short/word/int/dword, positive then complementary
// result domains at the original query: 117/117/117/117/117/117/117/117. NOT kept: no byte equality
// from the one-use result; original gate/full body restored. Fallback EDX masking,
// stage/pass homes and unfused element loads remain.
// B445: Original root-tag carrier destructively normalized after full-handle dispatch,
// signed/unsigned mask/word forms at producer/entry declarations: 152/117/152/117/152/117/152/117.
// NOT kept: no equality or gain; whole body restored. Fallback EDX, stage/pass and unfused homes remain.
// Round 130 (m3 lane 7, fast; 0x2243c0 and 0x224d90 checked each time): round 59's chain rebuilt 8 (aligned 7, masked 1); tag
// read after the instance 91 (552 B), instance from the tag 67 (0x224d90 lost). Material definition bound 92 (552 B), with head unbound
// 38. Scratch dead reads ahead of the pass lookup: two of them put every later register at retail's (index EDX, the `xor` early,
// EDI, the 0x1c620 and 0x1bbf0 setups, no loop pad), though the head then shifts, so ours spends one scratch step too many there.
// On the bound form: pass tag inline 92, unbound pass 118 (544 B, no pad), stage unbound 124, pointer copy of the pass, double
// reference, reference tag, its copy, staged tables 92 each; bound tables 86 (0x224d90 lost), on the chain 36. Cache zeros before the
// material lookup 13 (`xor` as retail, but the stores stay ahead of the loads; retail stores them after), before the pass lookup 60,
// with the bound definition 97; the same 13 with the cache copied for 0x175610 (scratch; its stores did not move).
// Round 59 (m3 lane 15, fast): lane 1 r59's score-8 form reproduced on the current body (8 at +0x9e: the pass definition loads into EDX
// where retail reuses EAX, the dead element address, and the material definition into EDX where retail overwrites EDI, the table, with
// the zero `xor` before the shift). On 8: all four tag lookups through the by-value accessor 8; the head/stage/pass bindings in their own
// block 8; the pass definition named 8; the material accessor also inside the material lookup 8; the material index bound once and used for
// the lookup and 0x1c590 76; main's split zeros (field_44 after the pass element) 59; the four zeros at entry 165; the element helper
// returning a named result with no caller bindings 153 (only a reference bound to the pointer temporary gives the unfused loads).
// Round 171 (m3 lane 5): the stream setup (`function_1c620(g_51f0f0, ...)` from the record at +0x9c) is the body of
// the existing matched 0x1cda0 (function_1cda0, src/unknown_01cf50.cpp); calling it 188 (plain, 508 B: called, its
// file is /Ob1); with `inline` on its definition 117 plain, identical code (0x1cda0 and its other caller 0x3c3c0
// unchanged; not kept: no code change, and it needs `inline` on another file's function). Same code, 117: local_6, local_10
// and local_11 read as element 0 through block_element_224db0; local_7 and local_8 written into the subscripts; the
// fallback through the instance pointer 168 (listed, re-measured).
// Round 168 (m3 lane 6, fast with 0x224d90 and 0x2243c0, both matched each time; match-only screens on the round 59
// chain, main's body unchanged): the chain rebuilt 8, and on it the pass and material lookups through the accessor 8,
// the pass definition bound 36, the material index named for both uses 76 (all re-measured). New on the chain: the
// pass element's +4 value bound as `long const &` 8 (same code); that value as a reference straight into the element
// with no element binding 121; the material definition, the pass definition or both as plain named pointers 8 each
// (forwarded); the material index bound and used for the lookup as well 36; the pass element unbound or as a plain
// pointer 121 each (no dead lea, yet the pass definition still takes EDX, so the dead lea's temporary is not what
// keeps EAX busy). Diagnostic dead loads (reverted at once) before the pass-definition lookup, before the pass element
// and before the tables lookup: 27, 45 and 116 (552 bytes), each moving the arg_3/arg_4 homes.
// M3 lane 13 r83 (fast, 0x2243c0 and 0x224d90 matched, control 117, fully masked aligned 15): no untried natural form found.
// The tag-data lookup as one ternary initializer re-measured 117 (same code, listed above). The tag-data and record-group
// chains here are the bodies of private helpers of other files (unknown_0167a0.cpp, unknown_03bcb0.cpp); no out-of-line
// accessor exists to call, so the unfused element loads (retail's `lea` then load, and its dead `lea`) still need the
// match-only reference bindings of the score-8 chain.
// M3 lane 5 r177 (fast, 0x2243c0 and 0x224d90 matched each time): the head through the tables lookups written as matched
// 0x23ea80 writes them (group read as `((dword *)g_4e3b44)[(short)tag * 4]`, `& 0xffff` fallback, ordinals read straight from
// the block addresses) 135 (first +0xe: the group load fuses to `[eax + edi]`; the in-place `and edx` then keeps the table in
// EDI); the material index named for the lookup and the 0x1c590 call 146 (524 B); the flags local after the type-14 search
// 132; the flags read at entry 132 (546 B); the callback context as an aggregate initializer `{ arg_0, search }` after the
// flags 120. Re-measured: the group as a named dword with the `& 0xffff` fallback 135; named element pointers with the
// ordinals masked at their uses 117 (same code).
// M3 lane 9 r118 (fast, 0x2243c0 and 0x224d90 matched each time; match-only screens on lane 6 r147's score-8 chain, main's
// body unchanged): the chain rebuilt 8 (first +0x9e). Same-code spellings on it (not counted as trials): the pass element's tag read through the by-value long
// accessor 8; the material tag for the definition lookup through that accessor 8; both 8 (combination, same code); one
// forceinline pointer-at-offset tag helper for the record (+0x20) and material (+8) definitions 8; the pass element bound
// as `byte const *const &` 8 (VC folds every accessor shape to the same tuples). New: the head and stage ordinals bound
// as `word const &` straight into their elements 152 (first +0x0). No untried natural form is left (lane 13 r83 above). The pass definition (EDX, retail
// EAX), material index (ECX, retail EDX) and material definition (EDX, retail EDI) do not move with accessor shapes.
// m3 lane 1 r102 (fast, 0x2243c0 and 0x224d90 matched each time; masked = aligned count with registers and offsets masked,
// base 30): the two block indices written `arg_3 + local_7` / `arg_4 + local_8` 117 (same code); the 0x1ff masks moved into
// the index expressions (raw ordinals in the locals) 117 (same code); the four cache zeros together after local_11 120
// (masked 27); local_7/local_8 as `short` 117 (same code); the flags local ahead of the context's arg_0 store 120 (masked 32).
// Re-measured: the tag-data ternary 117, the material tag named for both uses 146 (524 B).
// P487: Standard accessor/stage/pass iterator advancement, seven subsets, then
// all with an external instance reference: 233/169/158/254/244/188/266/266.
// NOT kept: no eligible gain; original accessor, views, cache zeros and body restored.
// Fallback EDX normalization, unfused elements and stage/pass homes remain.
// M3 lane 16 r118 (fast, 0x2243c0 and 0x224d90 matched each time): the round 59/129 reference chain rebuilt on this main 8 (the
// same eight lines: pass definition EDX where retail reuses EAX, material tag ECX/EDX, material definition EDX where retail reuses
// EDI, the zero's xor); on it: 1..7 unused-local placeholders 8 each (no phase effect); the pass element as a plain pointer 121
// (the load fuses again); the pass element's reference and tag read in their own block 8; local_10 or local_11 bound to a
// `byte const *const &` 8/8; the pass definition's tag entry as an `s_tag_instance const &` 8 (all the same code).
// M3 lane 7 r162 (fast, 0x2243c0 and 0x224d90 matched): the four cache zeros together right after the pass value 148; the
// fallback indexed by `(word)(short)local_tag` 117 (same code); no byte view of the definition, the parameter cast at each use
// 117 (same code); the two cache tests written field first 119; the vertex input lookup stored before the flags local 132.
// M3 lane 3 r245 (fast, 0x2243c0 and 0x224d90 matched unless noted): a non-const tag with the fallback masked `& 0xffff`
// 152 (re-measure); the instance group read once into a dword 165; that mask plus the four cache zeros together after
// local_11 149; the zeros together after local_11 alone 120 (re-measure); the flags local declared just before the
// 0x173ba0 call (retail reads +0xd8 after the two cache tests) 149, and 0x224d90 lost (36). Diagnostic: dead-local
// phase scan n = 0..3, 117 each.
// M3 lane 15 r90 (fast with 0x2243c0 and 0x224d90, both matched each time; control 117): the function as a member of a renderer
// type with the definition as `this` (0x224d90's call through a cast; LTCG still passes it in EAX) 117 as `__stdcall` and 117 as
// thiscall (same code each); that member with the fallback masked `& 0xffff` 152; block_element_224db0 plain `inline` 117 (same
// code); the fallback re-reading the tag field masked `& 0xffff` 152 (the field load CSEs with the tag, in-place `and`, table to
// ECX ahead of the frame); the material definition named before its +8 read 117 (same code).
// M3 lane 15 r93 (fast with 0x2243c0 and 0x224d90, both matched each time; control 117): the fallback indexed by the tag
// field's low word read from memory 117 (same code: the load CSEs with the tag); the 0x173ba0 mask written `local_16 &
// ~field_44` 117; the flags local as `long` 117; the tag-data pointer typed `dword const *` 117; the flags local declared at
// the top of the draw block and assigned in place 117 (all the same code). Diagnostic re-measure of the in-place mask (152):
// the table then takes ECX and is loaded ahead of `sub esp`, where retail keeps it in EDI after the pushes.
// P576: Actual scalar acquisitions through memcpy, head/stage/pass subsets 1/2/4/3/5/6/7: 142/141/117/172/142/141/172;
// all three through memmove 255. NOT kept: no byte equality; word acquisitions expand the native frame, pass copy ties.
// Original full body/helper/includes restored. Fallback EDX normalization, stage/pass homes and unfused loads remain.
// b9P (11-17) walk against retail, fast build on r840: every statement, offset, immediate (compared as a multiset
// on the side-by-side, relocations aside) and jcc sense agrees: group test 0x224dd1/0x224dd8 (je to the shared
// path), 0x1ff and 0xffff masks, strides 2/2/10 with the pass tag at +4 (0x224e3d), 0x132/0x232/0x100, 10000.0f
// (0x224f15), arg_1..arg_4 at esp+0x70..0x7c, ret 0x10. Callee arguments agree: 0x1bbf0 (0x224f12-0x224f20: tag
// re-read from +0x1c in EDI, 0, arg_3, arg_4, 1, 10000.0f), 0x175610 (0x224faf-0x224fc1: arg_1, arg_2, callback
// 0x224fd0, &local_0, cache in EDI), 0x173ba0, 0x1c590/0x1c620 registers; the cache tests are the compare-and-
// clear of 0x175610's own head (0x175628-0x175657). No behaviour difference found. Evaluation order: retail reads
// arg_0+0xd8 at 0x224f85, after both cache tests (matched sibling 0x2243c0 reads it just before 0x173ba0); ours
// reads it before the search loop. The late spelling (flag local declared just before 0x173ba0) 149 with arg_0 in
// EBP, so stack-passed (ret 0x14), 0x224d90 36; with the context's `field_0 = arg_0` store also moved down (after
// the loop, or right before the flags) 143, 0x224d90 kept, arg_0 in EBX: neither is kept (retail stores the
// context at 0x224f25). Ours reads its in-file copy of g_43f408 (identical to unknown_01cf50.cpp's); retail reads
// the one at 0x43f408 (0x224ec6): no code effect.
// m3 lane 11 r110 (fast, 0x2243c0 and 0x224d90 matched each time), on lane 8 r170's score-8 chain rebuilt on this
// main (8 at +0x9e, 544 B; left only the pass-tag and material definition lookups: retail puts the first tag data in EAX,
// the material tag in EDX and its tag data in EDI over the dying table base, with the cache zero scheduled before the
// last shift; ours EDX, ECX, EDX): the material lookup through the chain's tag-data and record accessors 8 (same code);
// the pass-tag lookup through the tag-data accessor too 8 (same code); the pass tag's data bound by reference 36; the pass
// reference in its own block 8 (same code); the pass tag read through a by-value inline accessor 8 (same code), the pass
// element passed straight to a by-reference accessor parameter 121 (no dead lea, like by value). Main body unchanged.
// Typed render-tag reads 117.
// Typed fallback shader reference 117.
// Shared tag-group accessor 165.
// Typed render-tag reads and typed fallback shader reference 117.
// Typed render-tag reads and shared tag-group accessor 165.
// Typed fallback shader reference and shared tag-group accessor 165.
// Typed render-tag reads and typed fallback shader reference and shared tag-group accessor 165.
// Typed render-tag reads and shared accessors for both group and fallback data 135.
// NOT kept: the typed field reads tied and the group-accessor forms regressed; the original code was restored.
// m3 lane 10 r115 (fast with 0x2243c0 and 0x224d90, both matched unless noted; control 117; main's body unchanged). Same
// code as the body (not trials): one pointer local reused down the head, stage and pass elements, alone or on through the
// pass and material definitions (five placements; VC splits the reassigned local, so a reused variable is not what gives
// retail's unfused loads or its EAX reuse); the block accessor with its index, its block or all three parameters taken
// by const reference; the accessor writing the element through an out parameter. New: the accessor returning a
// one-member handle object by value (`.address` read at each use, unnamed or as named head/stage/pass locals) 123 (544
// B, other code: the stage load is then retail's `lea` plus `movzx eax, word ptr [eax]`, the pass element still reads
// through the `lea`). On the round 59 chain (rebuilt 8): those handle objects in place of the three `void const *const &`
// element bindings 8, the same code, so the chain's reference bindings have a by-value handle spelling (still a
// one-member wrapper, match-only). Tag lookups returning the same handle, re-measured in that spelling: the pass definition
// 36, the material definition 92 (552 B), both 86 (545 B, 0x224d90 36), a second handle type for the tag lookups 86 (same
// code). New on the chain with handles: the pass record read as element 0 of the definition's +0x1c block through the
// accessor, on the pass handle 76 (546 B, 0x224d90 36), on the pointer tag accessor 36 (544 B, other code than the listed
// 36); both definitions through handles with both their blocks through the accessor 81 (551 B); the pass through a
// handle and the material through the pointer accessor, both blocks through the accessor 81 (551 B, other code); both
// through the pointer accessor with both blocks 110; the pass handle with its block and the material handle 60 (551 B,
// 0x224d90 36). A grid on the three handle leads (pass handle, pass pointer with its record block, material handle)
// over the four group-test forms and three material-index forms (reference, accessor reference, plain), 33 builds: best
// 79 (546 B), every other 86-135, none under the chain's 8. Left on the chain, as before: the pass definition takes EDX
// where retail reuses EAX, the material tag ECX (EDX), the material definition EDX (EDI), the zero's `xor` late.
// M3 lane 7 r175 (fast, 0x2243c0 and 0x224d90 matched each time; main's body unchanged at 117): the reference chain rebuilt
// on this main (the type-14 search through vertex_input_index) 8, the same eight lines. On it: the pass tag bound as
// `long const &` straight into the pass element 8; the pass definition's tag data named before its +0x20 read 8;
// local_11 declared ahead of local_10 and assigned after it 8; local_11 through the tag-data and material-index accessors
// 8 (all the same code); the four cache zeros grouped ahead of local_10 60 (first +0x50).
// M3 lane 16 r135 (fast, 0x2243c0 and 0x224d90 matched each time; base 117): the group test read through the table
// at each compare (no instance pointer) 165 (first +0xe); the two arms swapped with the tests negated 117 (same code);
// the first cache zero moved between local_10 and local_11 117 (same code); the other three zeros moved there instead
// 150; the tag data named before its +0x5c read 117 (same code).
// Typed root/stage/pass arrays singly, in pairs, and together 117 each.
// All typed arrays with full-width fallback mask 152. NOT kept: element addressing
// still fuses, and the full-width fallback moves the tag table out of EDI.
// The fallback still zero-extends DX into EAX, and the stage/pass reads remain fused.
// Group/fallback table receiver 117, with full-width mask 152; shader/pass/material table
// receiver 117; separate receivers 117, with full-width fallback mask 152.
// Shader index selected in the arms, selected absolute index, typed fields, and selected index
// with a shared shader table each 133.
// M3 lane 3 r261 (fast with 0x2243c0, 0x224d90 and 0x173ba0, all MATCH unless noted; control 117): a named tag-table
// pointer (`s_tag_instance *tags = g_4e3b44;`) for the group-test instance and a fallback masked `& 0xffff` 152 (the
// table then loads into ECX ahead of `sub esp`); the same pointer for every lookup 152 (first difference +0x34: the head
// is retail's up to the fallback, `and edx, 0xffff` in place with the table in EDI, but the fallback's load takes EAX
// where retail reuses EDX, and the table is not re-read after the 0x137bd0 path as retail does); that with the +0xd8
// flags read just before 0x173ba0 154 (0x224d90 36); that pointer for every lookup with the four cache zeros together
// after the material lookup 150; the pointer for the head and the stage-table lookup only 151.
// Existing model and fallback render-tag types singly or together with the
// short ordinal 117 each. Typed shared tag lookup 152, with both model types
// 152, with the model field used directly 152, with typed tag arrays 152,
// and with both shader lookups shared 152. The full-width fallback moves
// the table load ahead of the frame. The stage and pass addressing still fuses.
// M3 lane 11 r119 (fast with 0x2243c0 and 0x224d90, both MATCH unless noted; main body 117 unchanged). Round 59's score-8
// chain (reference-bound head/stage/pass elements and fallback/definition data, the material index as `long const &`
// from a by-value accessor, the zeros grouped, the +0xd8 flags read just before 0x173ba0) rebuilt on this main 8, the
// same lines: retail loads the pass definition into EAX over the dead pass-element `lea`, the material tag into EDX and
// the material definition into EDI over the dying table base, where ours takes EDX, ECX and EDX in turn. On it: the pass
// tag through a by-value accessor bound to `long const &` 59; the two cache pointers zeroed as one chained assignment 10;
// both definition lookups through a tag-bytes accessor returning `byte *const &` 8 (same lines); the stage-block base
// bound as a const reference 8; the material index reference declared before the material lookup 113 (0x224d90 36).
// M3 lane 15 r104 (fast with 0x2243c0 and 0x224d90, both matched each time; control 117, all restored): the tag local as
// `dword` with the `& 0xffff` fallback 152 (the table hoists into ECX above `sub esp`, as the long form); the group test,
// fallback and groups read in a forceinline lookup helper shaped as other files' private ones 117 (same code); lane 1
// r59's score-8 chain rebuilt on this body 8 (first +0x9e: pass definition EDX, material index ECX, material definition
// EDX, the zero after the definition's +8 read; re-measure, not a trial); main's body with the group test re-reading the
// tag and the flags read just before 0x173ba0 (the chain's two non-reference parts) 145; the element helper as plain
// `inline` 117 (still inlined, same code); the two nine-bit ordinals as `word` locals 117 (same code).
// Existing tag-array header in the accessor 117; typed block pointer 117; block lvalue 117;
// three named block fields by pointer or lvalue 117/117; pass and material block headers 117;
// all tag-array headers 117, with full-width fallback mask 152.
// The full-width mask still hoists the tag table, and the stage/pass addresses still fuse.
// Fallback entry lvalue with word or dword ordinal 117/152; masked entry pointer 152.
// Word fallback lvalue with shader/pass/material lvalues singly or all 117/117/117/117;
// all with masked fallback 152. NOT kept: The dword mask hoists the table, and the word
// forms leave the EAX zero-extension and fused element loads.
// M3 lane 2 r109 (fast with 0x2243c0 and 0x224d90; control 117): the +0xd8 flags read just before 0x173ba0 on main's body
// 149 (0x224d90 36: the convention changes); the group dword named once for both compares 165; the stage and pass indices
// written argument first (`arg_3 + local_7`, `arg_4 + local_8`) 117 (same code); the pass element bound to a named byte pointer
// before its +4 read 117 (same code); the fallback re-reading the tag word from the object (`*(word const *)(local_2 + 0x1c)`)
// 117 (same code: the read is forwarded to the local, still `movzx eax, dx`).
// M3 lane 4 r219 (fast with 0x2243c0 and 0x224d90, both MATCH each time; control 117). On lane 13 r59's score-8 chain
// rebuilt here (8, the same eight lines): the material definition named through the tag-data accessor 8, the pass
// definition named 8, the pass record (the +0x20 element) named 8, the material tag named before its lookup 8 (all the
// same code: the named pointers fold and no scratch register moves). Main's body with the head of matched 0x173cb0
// (`dword group = TAG_GROUP(tag)` and the fallback `tag & 0xffff`) 135: the fallback is then retail's in-place
// `and edx, 0xffff` with the table still in EDI, but the group read fuses (`mov eax, [eax+edi]` where retail forms the
// address, `add eax, edi; mov eax, [eax]`) and `shl` moves after `push edi`; the instance pointer with a group dword and
// `& 0xffff` 135; TAG_GROUP at each compare with `& 0xffff` 135; main's instance pointer read twice with `& 0xffff` 152
// (re-measure). Reading: the unfused group read needs the instance pointer read twice, the table stays in EDI only when
// the group is read once; no form gives both.
// M3 lane 12 r148 (fast with 0x2243c0/0x224d90/0x173ba0, all MATCH each time; main 117): lane 1 r59's reference chain
// rebuilt on this main (the type-14 search through vertex_input_index) 8 at +0x9e, the same eight lines (pass definition
// EDX where retail reuses EAX, material tag ECX for EDX, material definition EDX for EDI, the zero's `xor` after the
// definition's +8 read where retail has it between the tag's `and` and `shl`). On the chain: the first cache zero between
// the pass and material definitions 23; all four zeros ahead of the pass definition 60; all four between the two
// definitions 13; the material definition named in a plain local before its +8 read 8 (same code); that definition bound
// as `void const *const &` from the tag-data accessor 92 (552 B). Body unchanged.
// M3 lane 14 r121 (fast with 0x2243c0/0x224d90/0x173ba0, all MATCH each time; main 117): lane 1 r59's reference chain
// rebuilt on this main 8 at +0x9e (the same lines). Reading by the scratch next-fit order (EAX, ECX, EDX): ours takes
// EDX for the pass definition because the dead pass-element `lea` moved the pointer past EAX, retail takes EAX there,
// then EDX for the material tag, puts the zero in EAX between that tag's `and` and `shl`, and gives the material
// definition EDI, the dying table base, so in retail every scratch register is busy at that load. On the chain: the
// material lookup through an accessor that takes the instance table as a parameter 8, the pass lookup through it 8,
// both 8, the material lookup as the tag-data accessor of the record-tag accessor 8 (all the same code); the material
// tag bound as `long const &` just ahead of its lookup 36; the material tag read into a long ahead of the four zeros
// with the lookup after them 41, masked at that read 41, with that long also passed to 0x1c590 81, the reference form
// of that 81; the four zeros between the two definitions re-measured 13. Body unchanged.
// AC597: Tag table refreshed after virtual dispatch, with direct word/mask, named word/mask, masked
// handle in place, typed word/mask and a shared signed/unsigned slot: 165/152/165/152/152/165/152/155.
// M3 lane 16 r152 (fast with 0x2243c0, 0x224d90 and 0x173ba0, all MATCH unless noted; main's body unchanged at 117;
// match-only screens on lane 1 r59's reference chain, rebuilt on this main 8 at +0x9e with the same lines). On the chain:
// one pointer local walked from the pass element through its tag data and the +0x20 block (`cursor = element; cursor =
// tag data; cursor = *(cursor + 0x20)`) 8 (same code: the reassigned local splits); the pass definition named and then
// read through a `byte const *const &` alias of that local 36 (the definition then loads into EAX as retail, but the
// +0x20 read takes EDX where retail stays in EAX, and arg_3 moves to EBP); the material definition named and read
// through such an alias 92 (552 B: it takes EDI as retail, the index stays ECX and the zero's `xor` late); the pass tag
// read through a `long const &` alias of its local 59; both definition aliases 86 (545 B, 0x224d90 36). A named local
// aliased by a const reference gives the same code as binding the reference to the accessor's temporary, so the
// listed 36/92/86/59 forms have no second spelling. Diagnostic (a scratch global, reverted at once): the pass tag's
// table offset stored after the material lookup 143 (553 B: the pass element's load unfuses instead of ECX staying
// busy). Reading by next-fit over EAX, ECX, EDX: retail's choices at the last two lookups (pass definition EAX twice,
// material tag EDX, zero EAX, material definition EDI) are what next-fit gives if ECX stays busy from the pass tag's
// load to the material definition's and EDX until the +0x20 read; nothing in this body uses either that late.
// M3 lane 12 r156 (fast with 0x2243c0 and 0x224d90, both MATCH; control 117): the tag local not const 117 (same
// code); the group tests reading the instance at each use with no instance local 165; the group test negated with the
// fallback arm first 117 (same code); the material tag named once for 0x1c590 and its lookup 146 (524 B); the stage and
// pass indices named where they are summed 117 (same code).
// M3 lane 3 r280 (fast with 0x2243c0, 0x224d90 and 0x173ba0, all MATCH each time; control 117, aligned rows 56 of 165;
// main's body unchanged). The four cache fields cleared by one memset of the record's tail (16 bytes) where retail has
// its four stores, after the material lookup, 162 (the head's prologue changes, aligned 63); that memset ahead of the
// pass definition lookup 119 (aligned 74), between the two definition lookups 163; the four stores as statements
// together after the material lookup 120 (re-measure, aligned 51), together ahead of the pass definition 148 (aligned
// 49). The head, stage and pass block headers copied whole into (count, address) aggregate locals and indexed from
// them 163 (561 B: the three addresses are loaded up front), with the late memset 166 (563 B).
// Unshifted pass-record base 164; shared material tag field address 152; native pass array header 117.
// With grouped cache defaults those three give 161/153/120. Native pass array with unshifted
// base or material field address 164/152. NOT kept: ties or regressions. The fallback still
// zero-extends into EAX instead of masking EDX and the stage/pass loads still fuse.
// M3 lane 4 r237 (fast with 0x2243c0 and 0x224d90, both MATCH; control 117): the tag bound as a `long const &` to the
// definition's +0x1c field with the word fallback 135 (first +0x0), with the `& 0xffff` fallback 156; the group read in place
// at both compares (no instance pointer) 165 (first +0xe), with the masked fallback 135; the instance pointer const 117 (same
// code).
// M3 lane 5 r219 (H2M_FAST with 0x2243c0, 0x224d90 and 0x173ba0, all MATCH each run; main 117): lane 1 r59's reference
// chain rebuilt on this main 8 at +0x9e (the same eight lines). Permuter on the chain (--fast --anneal --jobs 1, 600 s,
// the full check here took 4 minutes): 50 tries, best 8. On the chain, each the same code (8): the three pointer zeros
// as one chained assignment; the pass definition bound as `byte const *const &` with local_10 formed from it; the pass
// tag bound as `long const &` to the element's field; the pass tag masked at its definition; one to seven unused
// initialised locals at the top (flat: the last two lookups' registers are not a symbol-count phase). Body unchanged.
// M3 lane 12 r166 (fast with 0x2243c0 and 0x224d90; control 117; the score line moved to the top of this note). On main's
// body: the kind, definition, format, element and shader-chain pointers each named and read through a const-reference
// binding (the shape of the record helpers private to another file) 120 (first +0x50: the fallback's in-place `and edx,
// 0xffff` then appears with the table kept in EDI, but arg_3/arg_4 take EBP/EDX for retail's ESI/EBP and the pass load is
// unfused). On round 59's chain rebuilt here 8 (with vertex_input_index for the search and the +0xd8 read just before
// 0x173ba0; 47 with it read before the search): the shader definition and its 0x20 block named and bound 76 (546 B,
// 0x224d90 lost: arg_0 leaves EAX); only the shader definition named and bound 36, only the block 36 (first +0x50: the
// head's argument homes rotate, the shader/material registers do not reach retail's); the material index reference bound
// ahead of the material lookup 113 (551 B, 0x224d90 lost). Match-only chain; body unchanged.
// M3 lane 2 r126 (H2M_FAST with 0x2243c0, 0x224d90 and 0x173ba0, all MATCH; main's body unchanged at 117): lane 16 r152's
// saved reference chain rebuilt on this file 8 at +0x9e (left: the pass definition EDX where retail EAX, the material tag
// ECX where retail EDX, the material definition EDX where retail EDI, the zero after the material lookup). On the chain:
// the four cache zeros between the two definition lookups 13 (the stores move, the registers stay), ahead of both
// lookups 60; both definition lookups through the chain's tag-data helper (and the material through its record helper)
// 8, the material lookup alone 8 (same code); the geometry-block pointer named ahead of the pass tag read, between the
// two lookups or ahead of the pass element 8 each (same code); the pass tag read without the bound pass element, the
// element bound after the pass lookup, after both lookups or not at all 121 each (the bound element's dead `lea`, which
// retail has, goes). Lane 16's next-fit reading still stands: retail needs ECX held from the pass tag's load to the
// material definition's and EDX until the +0x20 read, and no spelling here holds either.
// M3 lane 6 r219 (H2M_FAST with 0x2243c0, 0x224d90 and 0x173ba0, all MATCH each run; main 117): lane 1 r59's reference
// chain rebuilt on this main 8 at +0x9e (the same eight lines). On the chain, match-only: the pass tag re-read through a
// `long const &` to its local at the pass lookup 59 (first +0x50); the pass definition through the tag-data accessor 8
// (same code); that definition bound as `void const *const &` 36 (first +0x50: the pass definition then takes EAX as
// retail, but the stage index moves to EBP and the +0x20 read to EDX); the material lookup through the tag-data and
// record-tag accessors 8 (same code); both accessor forms 8 (same code); the material definition bound as `void const
// *const &` 92 (552 B); both bindings 86 (545 B). Body unchanged.
// Fallback payload member and fallback plus shader payload members, mask/word
// 152/117 and 152/117. Native slot lvalues 152/117.
// Bound element addresses 124/123. Grouped cache defaults 103,
// then late definition mask 93. NOT kept: 103 needs bound one-use
// element addresses. The late mask loses caller 0x224d90. The EDX mask
// and EDI table can coexist. The fallback payload still takes EAX.
// M3 lane 6 r225 (H2M_FAST with 0x2243c0, 0x224d90 and 0x173ba0, all MATCH each run; main 117): lane 1 r59's reference
// chain rebuilt on this main 8 at +0x9e (the same eight lines). On the chain, match-only: a redundant `local_1.field_44 =
// 0;` after the pass tag read (for the zero's `xor` placement) 141, after the record pointer 137, ahead of the pass tag
// read 138 (the extra store is kept and shapes the code); the pass definition through a by-reference element helper
// returning the tag data of the element's tag (the dead `lea` as an element passed by reference) 8 (same code); the
// material definition through it from the record 8 (same code). Body unchanged.
// M3 lane 7 r210 (h2m fast with 0x2243c0, 0x224d90 and 0x173ba0, all MATCH unless given; main 117): lane 1 r59's
// reference chain rebuilt on this main 8 at +0x9e (the same eight lines: pass definition EDX where retail reuses EAX after
// the dead `lea`, material tag ECX for EDX, material definition EDX for EDI, the zero's `xor` after the definition). On the
// chain, match-only: the pass element's reference scoped in its own block ending after the +4 read 8 (same code); the
// head, stage and pass references each in their own block 8 (same code); the geometry-info pointer for 0x12de70 named
// right after the pass tag (to hold ECX across the last two lookups) 8 (same code: it folds), named just after local_9 8,
// named at the function's top 68 (551 B, 0x224d90 36). Body unchanged.
// Fallback payload member and fallback plus shader payload members, mask/word
// 152/117 and 152/117. Native slot lvalues 152/117.
// Bound element addresses 124/123. Grouped cache defaults 103,
// then late definition mask 93. NOT kept: 103 needs bound one-use
// element addresses. The late mask loses caller 0x224d90. The EDX mask
// and EDI table can coexist. The fallback payload still takes EAX.
// M3 lane 8 r222 (H2M_FAST with 0x2243c0 and 0x224d90, MATCH each run; control 117). Lane 16 r152's saved chain (two
// file-local accessors, the elements bound by const reference; match-only) rebuilt at 8, first +0x9e: left the pass
// definition EDX (retail EAX), the material index ECX (EDX), the material definition EDX (retail reuses the table's EDI)
// and the zero's `xor` after the material read. On the chain: the pass definition read through the tag-data accessor 8,
// the material definition through it 8, both 8, the material tag through the record accessor 8 (same code each); the
// pass definition bound by const reference to the accessor 36 (first +0x50), the material definition bound 92 (552 B),
// both 86 (545 B). Diagnostic volatile loads through `local_6` ahead of the pass tag, the pass definition or the material
// read move the whole head (81-141), so no single later temporary is isolated here. Body unchanged.
// M3 lane 2 r134 (H2M_FAST with 0x2243c0, 0x224d90 and 0x173ba0, all MATCH unless given; main 117): lane 16 r152's
// reference chain rebuilt on this main 33 with the +0xd8 read where main has it, 8 at +0x9e with it read just before
// 0x173ba0. On the chain, match-only: the material definition through the tag-data accessor with `local_11 += 0x1c` as its
// own step 8 (same code); the pass definition through it with `local_10 += 0x132` as its own step 8 (same code); the two
// aggregate locals declared in the other order 8 (same code); the material lookup and local_11 moved into the if body
// beside their one use 145 (546 B, 0x224d90 36); local_11 bound as `byte const *const &` 8 (same code); local_10 bound so
// 8 (same code). Retail's zero is live in EAX before the material definition loads (so that load takes EDI); no
// spelling here starts the zero earlier. Body unchanged.
// M3 lane 5 r233 (H2M_FAST with 0x2243c0, 0x224d90 and 0x173ba0, all MATCH; main 117; lane 1 r59's reference chain
// rebuilt from its saved body 8 at +0x9e, aligned 12 with the image's address rows, 7 real rows: pass definition EDX
// where retail reuses EAX, material tag ECX for EDX, material definition EDX for EDI, the zero's `xor` after the
// definition's +8 read). A link-time lab (this file's unit with the objects of its reduced link, 3.5 s a variant,
// main's code exactly; build/r233/lab.py, s224.py) scored 517 random combinations of the chain's binding choices
// against retail's window: the pass tag (plain local, const-reference local, or read in place), the pass definition
// (in place, plain local, tag-data accessor in place, const-reference from the accessor, plain local from the
// accessor), the +0x20 block (in place or named), `+ 0x132` and `+ 0x1c` (in the initialiser or as their own steps),
// the material tag (in place, plain local, record accessor in place, const-reference from it), the material definition
// (four forms as the pass definition's), the four cache zeros (after both lookups, the first ahead of the pass
// definition, all ahead of it, all between the two lookups, the first between them) and the 0x1c590 argument (the
// bound reference or the field read in place): 16 combinations tie the chain at 7 rows, 51 give 8, none fewer. Also
// in the lab: the value cache as a derived local type whose inline constructor zeroes the four members, declared at the
// top, ahead of the pass element, after the pass tag, after local_10 or after local_11: 42, 39, 30, 8 and 7 rows (a
// constructor is the four stores at its declaration, nothing more); diagnostics (wrong code) that keep the pass tag
// alive past the material lookup 26 rows (the pass element's load unfuses instead), so the busy-register reading above
// is not what gives retail's EAX/EDX/EDI picks; the pass tag masked once into a named index 7 (same code as the chain).
// Body unchanged.
// M3 lane 16 r179 (H2M_FAST with 0x2243c0 and 0x224d90, both MATCH; control 117). Lane 1 r59's match-only score-8 chain
// rebuilt at 8 (first +0x9e: pass definition EDX where retail overwrites EAX, material index ECX for EDX, material
// definition EDX for retail's EDI, and retail's cache-zero xor scheduled ahead of the material index shift). On it:
// local_11's tag data through the by-value tag-data and record-tag accessors 8; local_10's through the tag-data accessor
// 8; both 8; local_10 and local_11 bound as const references 8 (all the same code); the material tag data bound to a
// const-reference temporary before local_11's read 92 (552 B). Body unchanged.
// M3 lane 6 r234 (H2M_FAST with 0x2243c0 and 0x224d90, both MATCH; control 117). The chain rebuilt on this main at 8
// needs the +0xd8 flags read moved after the two cache tests as well; with main's early read the chain head alone gives
// 47 (the flags load is hoisted above the search). On the 8 chain: the pass tag `long const` 8; the field_44 zero back
// between the pass tag and the pass definition 59 (first +0x50); the instance pointer to const 8; the root tag not
// const 8; the cache local declared ahead of the context local 8 (same code each). Body unchanged.
// M3 lane 7 r221 (H2M_FAST with 0x2243c0, 0x224d90 and 0x173ba0, all MATCH; control 117; lane 16 r152's chain
// rebuilt at 8, first +0x9e: pass definition EDX for retail's EAX, material tag ECX for EDX, material definition EDX
// for retail's EDI, the cache zero's xor after the material lookup). On the chain, the material lookup split into a
// named definition pointer and its +8 read: with the four zeros after both 8 (same code), the zeros between the two
// 95 (552 B), only the first zero ahead of the split 23 (first +0x50); the definition bound to a const reference from
// the tag-data accessor with the zeros between 95, after 92 (552 B). Body unchanged.
// M3 lane 8 r231 (h2m fast with 0x2243c0, 0x224d90 and 0x173ba0, all MATCH; control 117; the reference chain rebuilt at 8,
// first +0x9e, the same eight lines). On the chain, match-only: the material index masked into a named long with the four
// zeros between it and the lookup 41 (re-measure: the stores stay ahead of the lookup's loads), with only the first zero
// between 40; the material instance named as a pointer into the table with the zeros between 135 (552 B), after 8; the
// definition read without its `+ 0x1c` and the offset added after the zeros 8; the lookup through an accessor taking an
// already masked index 8; through an accessor with a NULL-initialised result and a named masked index 8; the four zeros
// through a file-local reset helper taking the cache pointer after the lookup 8 (same code each), ahead of it 13.
// Body unchanged.
// M3 lane 8 r233 (h2m fast with 0x2243c0, 0x224d90 and 0x173ba0, all MATCH; control 117; the reference chain rebuilt
// at 8, same eight lines). Allocator reading: on the chain the pass definition is the scratch pick that sits one cursor
// step from retail's (ours EDX, retail EAX), so the material tag and definition follow (ours ECX/EDX, retail EDX and the
// dying table's EDI); on main the in-place mask splits the head's table piece from the body's (ECX, hoisted above the
// frame) where retail keeps one EDI web; no single forced colouring decision lowers main. On main: the tag as `dword`
// with a `& 0xffff` fallback 152 (re-measure of round 40's form); the fallback indexed by `(unsigned short)` 117 (same
// code); the instance read from the tag field as a short 117 (same code); the tag local passed to 0x1bbf0 130 (listed);
// the four cache zeros between the definition's table read and the head element 121. Body unchanged.
// Typed render index or directory group with signed-slot delta 148/148, unsigned remainder 152/152,
// scaled byte displacement 113/113, signed-slot correction 172/172, or mask before displacement 152/152.
// NOT kept: the four-point displacement gain is below threshold and shifts before masking; retail masks first.
// Group selection carried separately before tag lookup, word index and full mask respectively: positive
// 122/163, negative 122/163, staged 154/157, both comparisons evaluated 131/131. The masked forms
// still lose the retail table load after the saves. The original frame, fallback, and body are restored.
// The dispatch span still needs the in-place EDX fallback with the directory in EDI. Signed group slot from the unsigned
// ordinal, word/masked fallback: sign-bit bias 157/135, sign contribution 160/136, signed-half selection 131/136,
// full-width sign extension 117/152. The original group and fallback expressions are restored.
// The fallback mask and unfused stage/pass loads still differ. Low-half projection into dword/word:
// subtract masked high half 132/117, xor high half 152/117, shift pair 152/117, complemented mask 141/117.
// NOT kept: the narrow forms tie, and the wide forms add operations or displace the directory.
// M3 lane 13 r155 (H2M_FAST with 0x2243c0 and 0x224d90, both MATCH; control 117; lane 16 r152's reference chain rebuilt
// at 8, first +0x9e, the same eight lines). Reading of the eight lines as scratch picks: after the pass tag's load ours
// takes EDX for the pass definition, EAX for its +0x20 read, ECX for the material tag and EDX for the material
// definition; retail takes EAX, EAX, EDX and gives the material definition the table's EDI, which needs EDX still
// counted busy at the pass definition's load and ECX at the material tag's. On the chain, match-only: the pass and
// material lookups through a file-local accessor taking the tag index by `long const &` (to keep the index in its own
// temporary) 8; the pass lookup alone 8; the material lookup alone 8; the pass lookup given the element's +4 field in
// place, no pass-tag local, 8; all four lookups (fallback, definition, pass, material) through it 8 (the same code each:
// a by-reference index is forwarded and frees its register at the load as a by-value one does). Body unchanged.
// Two-group table ordinal walk and mismatch scan, word/mask 174/172 each;
// standard linear search 163/161; sorted search 191/193. NOT kept: all grow
// the body and regress. Fallback EDX masking and split element addresses remain.
// Packed group memcpy word/mask 165/135, memmove 185/188, byte comparisons 180/183;
// packed handle memcpy word/mask 117/152.
// Fallback EDX masking and the split element addresses remain. NOT kept: the handle copy ties and the other packed acquisition forms regress.
// M3 lane 14 r155 (H2M_FAST with 0x2243c0, 0x224d90 and 0x173ba0, all MATCH each run; control 117; body unchanged). With
// the fallback masked in place (152, re-measured) the tag table takes ECX for the whole body, free in that fallback, and
// the block pointer falls to EDX; retail keeps the table in EDI. A forced-ranking search on that form is best at 109 (the
// pass tag in ECX: in retail EAX still holds the pass element's address, the dead `lea`, when the tag loads). On main:
// the pass tag bound as `long const &` to the element's +4 field 135 (arg_3 then takes ESI as retail), with the in-place
// fallback mask 146; the stage and pass indices as `word` locals 117 and the two element indices named where computed
// 117 (same code each).
// Packed render-handle views through native shorts, bitfields, signed or unsigned low members.
// Word/full mask scores 135/156, 156/156, 135/156 and 135/156. NOT kept: all eight
// move the first difference to entry and raise the score. The EDX mask and split element loads remain.
// The fallback EDX mask with the EDI directory and separate element addresses still differ.
// Reused instance with the word fallback 117. Full mask, then continued by shader,
// pass, material, shader/pass or all three 152 each. Word fallback continued
// through all three 117. NOT kept: the word forms tie, and the full mask hoists
// the directory load and raises the score.
// M3 lane 11 r169 (h2m fast with 0x2243c0, 0x224d90, 0x173ba0, 0x173c10, 0x224550 and 0x175610, all MATCH unless given;
// control 117). The tail is the value cache's existing method 0x173c10 written out (the masked 0x173ba0 call and the
// mask OR), as matched 0x224550 calls it: local_1 typed as the particle value cache (this file's forward declaration
// completed with 0x224550's layout) and `local_1.function_173c10(flags & 0x107f0)` in place of the call and the OR:
// on main 149 (first +0x4, 0x224d90 36); on lane 1 r59's reference chain rebuilt here 8 (the same eight lines; the
// method's argument now gives the chain's late +0xd8 read). On that chain with the method: the material tag read in
// place 106, the fallback unbound 35, the head and stage unbound 152, the definition unbound 10. A forced-ranking
// search on the chain (70 single decisions, scratch picks included) finds nothing under 8 (best 9). Body unchanged.
// The dispatch span and the fallback EDX mask with the EDI directory still differ.
// Existing typed group entry and direct indexed group reads, word/full mask
// 117/152 and 165/135. Typed directory through fallback 117, through all
// lookups with word/full mask 165/152, with typed fallback shader field 152.
// NOT kept: the word entry ties; the full mask still moves the directory.
// @retail 0x224db0
void __stdcall function_224db0(s_interface_definition *arg_0, long arg_1, long arg_2,
	long arg_3, long arg_4)
{
	s_224db0 local_0;
	s_2243c1 local_1;
	byte *local_2 = (byte *)arg_0;
	long const *local_5;
	long const local_tag = *(long const *)(local_2 + 0x1c);
	s_tag_instance *local_instance = &g_4e3b44[(short)local_tag];
	if (*(dword const *)local_instance->unknown00 == 0x5052544d ||
		*(dword const *)local_instance->unknown00 == 0x70727433)
		local_5 = function_137bd0(local_tag)->function_x947334();
	else
		local_5 = *(long const *const *)(g_4e3b44[(word)local_tag].bytes + 0x24);
	byte const *local_6 = *(byte const *const *)(g_4e3b44[*local_5 & 0xffff].bytes + 0x5c);
	long local_7 = *(word const *)block_element_224db0(local_6, 0, 2) & 0x1ff;
	long local_8 = *(word const *)block_element_224db0(local_6 + 8, local_7 + arg_3, 2) & 0x1ff;
	long local_9 = *(long const *)((byte const *)block_element_224db0(local_6 + 0x10, local_8 + arg_4, 10) + 4);
	local_1.field_44 = 0;
	byte const *local_10 = *(byte const *const *)(*(byte const *const *)(g_4e3b44[local_9 & 0xffff].bytes + 0x20) + 4) + 0x132;
	byte const *local_11 = *(byte const *const *)(g_4e3b44[*(long const *)(local_10 + 0x100) & 0xffff].bytes + 8) + 0x1c;
	local_1.field_48 = 0;
	local_1.field_4c = 0;
	local_1.field_50 = 0;
	if (function_12de70((s_geometry_block_info *)(local_2 + 0xa0), 3))
	{
		function_1c590((s_shader_cache *)g_51f0f0, *(long const *)(local_10 + 0x100), 1);
		byte const *local_12 = *(byte const *const *)(local_2 + 0x9c);
		function_1c620((s_shader_cache *)g_51f0f0, *(long const *)(local_12 + 0x10),
			local_12[1], *(long const *)(local_12 + 8), g_43f408[local_12[0]]);
		function_1c710(g_51f0f0);
		function_1cdd0(0, 0);
		function_2c3b0(&g_55ed60);
		g_55ed60.field_4 = function_173c30;
		g_55ed60.field_0 = (s_particle_system_datum const *)arg_1;
		function_1bd50(&g_55ed60);
		function_1bbf0(*(long const *)(local_2 + 0x1c), 0, arg_3, arg_4, 1, 10000.0f);
		local_0.field_0 = arg_0;
		dword local_16 = *(dword const *)(local_2 + 0xd8) & 0x107f0;
		local_0.field_4 = vertex_input_index(local_11, 14);
		if ((void *)arg_1 != local_1.field_48)
		{
			local_1.field_48 = (void *)arg_1;
			local_1.field_44 &= 0xfffff98f;
		}
		if ((void *)arg_2 != local_1.field_4c)
		{
			local_1.field_4c = (void *)arg_2;
			local_1.field_44 &= 0xfffecf7f;
		}
		function_173ba0(~local_1.field_44 & local_16, local_1.field_48, local_1.field_4c,
			local_1.field_50, local_1.field_0);
		local_1.field_44 |= local_16;
		function_175610((s_particle_value_cache *)&local_1,
			(s_particle_system_datum *)arg_1, (s_particle_location_datum *)arg_2,
			(particle_callback_175610)function_224fd0, &local_0);
	}
}
