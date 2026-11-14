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

// score 117 (was 117): earlier first line 117 (was 117). The tag index masked as a 32-bit value inside the
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
