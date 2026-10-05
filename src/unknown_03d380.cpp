// @flags /O2 /Gr
/* UNKNOWN_03D380.CPP: the game module callbacks of the table at 0x46e320
   (batch 18-4); every entry takes one dword on the stack */

#include "unknown_11c920.h"
#include <xtl.h>
#include <string.h>
#include "unknown_123b30.h"
#include "globals.h"
#include "unknown_03d380.h"
#include "object_iterator.h"

/* ---- types ---- */

struct s_object
{
	byte unknown00[0xcc];
	long unknownCC;
};

struct s_simulation_world
{
	byte unknown00[8];
	long state;
	byte unknown0c[0x22];
	byte flag2e;
};

struct s_4e6380
{
	byte unknown00[0x78];
	byte flag78;
	byte unknown79[2];
	byte flag7b;
	byte unknown7c;
	byte flag7d;
	byte unknown7e[2];
	long unknown80;
	long unknown84;
	byte unknown88[0x1f8 - 0x88];
	long unknown1f8;
	byte unknown1fc[4];
	long unknown200;
};

struct s_element_bc
{
	byte unknown00[2];
	byte kind;
	byte unknown03[9];
	long object_index;
	byte unknown10[0x7c];
	long unknown8c;
	byte unknown90[0x2c];
};

struct s_element_18_flags
{
	word flag0 : 1;
	word flag1 : 1;
	word flag2 : 1;
	word flag3 : 1;
	word flag4 : 1;
	word flag5 : 1;
	word flag6 : 1;
	word flag7 : 1;
	word flag8 : 8;
};

struct s_element_18
{
	byte unknown00[3];
	byte state;
	s_element_18_flags flags;
	byte unknown06[6];
	long value;
	byte unknown10[8];
};

struct s_element_40
{
	byte unknown00[4];
	long table_index;
	byte unknown08[0x30];
	long datum_index;
	byte unknown3c[4];
};

struct s_tag_iterator
{
	long unknown00;
	long unknown04;
	long unknown08;
	long index;
	dword signature;
};


struct s_unknown_5c;
s_unknown_5c *function_221810(short index);

/* ---- globals ---- */

byte g_4cf771;
byte g_4cf772;
byte g_4cf77b;
dword g_453498[1];
s_4e6380 *g_4e6380;
s_record_pool *g_4e637c;
s_looping_sound_globals *g_4ed288;
dword g_4c8798[256];
byte g_4ea934;
byte g_4ea936;
byte g_4e6388;
s_record_pool *g_51ebfc;
s_record_pool *g_51ec00;
long g_47f04c;
byte g_47f059;
s_47f048_object *g_47f048;
void *g_51ecac;

#define GAME_MODE (g_4e6948->mode)
#define ELEMENT(array, type, datum) ((type *)((array)->data + sizeof(type) * ((datum) & 0xffff)))

/* ---- the table ---- */

void __stdcall function_1c3540(dword flags);
void __stdcall function_68090(dword flags);
void __stdcall function_16eff0(dword flags);
void __stdcall function_1e6af0(dword flags);
void __stdcall function_1c3590(dword flags);
void __stdcall function_123b00(dword flags);
void __stdcall function_1889d0(dword flags);
void __stdcall function_155f10(dword flags);
void __stdcall function_67fb0(dword flags);
void __stdcall function_124750(dword flags);
void __stdcall function_1264c0(dword flags);
void __stdcall function_188ac0(dword flags);
void __stdcall function_16f090(dword flags);
void __stdcall function_14b660(dword flags);
void __stdcall function_67fc0(dword flags);
void __stdcall function_43970(dword flags);
void __stdcall function_3d380(dword flags);
void __stdcall function_124620(dword flags);
void __stdcall function_155360(dword flags);
void __stdcall function_1c7f80(dword flags);
void __stdcall function_17d190(dword flags);
void __stdcall function_16f0c0(dword flags);
void __stdcall function_226030(dword flags);
void __stdcall function_24c829(dword flags);

game_module_proc g_46e320[30] =
{
	function_1c3540,
	function_68090,
	function_16eff0,
	function_1e6af0,
	function_1c3590,
	function_1c3540,
	function_123b00,
	function_1889d0,
	function_155f10,
	function_67fb0,
	function_124750,
	function_72c70,
	function_1264c0,
	function_188ac0,
	function_16f090,
	function_14b660,
	function_67fc0,
	function_43970,
	function_3d380,
	function_72c70,
	function_72c70,
	function_72c70,
	function_124620,
	function_155360,
	function_24c829,
	function_1c3590,
	function_1c7f80,
	function_17d190,
	function_16f0c0,
	function_226030,
};

// @retail 0x3d380
void __stdcall function_3d380(dword flags)
{
	s_type_f1af8e iterator;

	function_bae80(&iterator, 0, 0);
	for (s_object *object = function_baeb0(&iterator); object; object = function_baeb0(&iterator))
		object->unknownCC = NONE;
}

// @retail 0x43970
void __stdcall function_43970(dword flags)
{
	g_509448->proc20 = function_43820;
	g_509448->proc24 = function_43850;
}

// @retail 0x67fb0
void __stdcall function_67fb0(dword flags)
{
	g_4cf77b = 1;
}

// @retail 0x67fc0
void __stdcall function_67fc0(dword flags)
{
	if (flags & 4)
	{
		function_593e0();
		s_simulation_world *world = g_4cf77c;
		if (GAME_MODE == 1 && world->state == 1)
		{
			function_6b040((c_class_6a600 *)world);
			g_4cf772 = 0;
			g_4cf771 = 0;
		}
		else
		{
			function_67f60();
			if (GAME_MODE == 4)
			{
				GAME_MODE = 5;
				void *proc = g_55e4d0[g_4e9ae8->engine_index];
				if (proc)
					function_162060(proc);
			}
			if (GAME_MODE >= 4 && GAME_MODE <= 5)
			{
				function_162420();
				function_bb7f0();
				function_183f10();
			}
			GAME_MODE = 1;
			function_67ee0();
		}
	}
	((c_class_6a600 *)g_4cf77c)->function_6a600();
	function_83370(g_4cf780, flags);
	function_6a770(g_4cf77c);
	g_4cf77b = 0;
}

// @retail 0x68090
void __stdcall function_68090(dword flags)
{
	if (g_4cf770 && !(flags & 1))
	{
		long state = g_4cf77c->state;
		if (state != 3 && state != 5)
			g_4cf77c->flag2e = 1;
	}
}

// @retail 0x123b00
void __stdcall function_123b00(dword flags)
{
	byte *base = game_state_globals.base_address;

	XPhysicalProtect(base, 0x3be000, PAGE_READWRITE);
	XPhysicalProtect(base + 0x3be000, 0x40000, PAGE_READWRITE | PAGE_WRITECOMBINE);
}

// @retail 0x124620
void __stdcall function_124620(dword flags)
{
	game_state_globals.game_time = g_510c54->game_time;
	g_510c54->unknown01 = 0;
}

// @retail 0x124750
void __stdcall function_124750(dword flags)
{
	if (g_510c2c)
		*(dword *)g_510c2c = (dword)g_453498;
}

// @retail 0x1264c0
void __stdcall function_1264c0(dword flags)
{
	bool a = (flags >> 6) & 1;
	bool b = (flags >> 5) & 1;
	s_4e6380 *globals = g_4e6380;

	if (!b && globals->flag7b)
	{
		globals->flag7d = 1;
		globals->flag7b = 0;
		globals->flag7d = 0;
	}

	if (globals->flag78)
	{
		long datum;

		function_21d4d0();
		s_record_pool *array = g_4e637c;
		datum = data_datum_index(array, function_16bc00(array, 0));
		while (datum != NONE)
		{
			s_element_bc *element = ELEMENT(array, s_element_bc, datum);
			bool remove = true;

			if (b)
			{
				short type = *((char *)g_4e3b44[element->object_index & 0xffff].flags + 2);

				if (type == 0x20 || type == 0x26)
				{
					remove = (type == 0x26 && !a);
				}
				else if ((1 << element->kind) & 0x1e)
				{
					remove = (((byte *)function_221810(type))[0x4c] != 1);
				}
			}

			if (remove)
			{
				function_127320(datum, 10);
				array = g_4e637c;
			}
			else
				element->unknown8c = NONE;

			datum = data_datum_index(array, function_16bc00(array, datum == NONE ? 0 : (datum & 0xffff) + 1));
		}

		if (a)
			function_125d60();
		else
			function_21f290();
	}

	globals = g_4e6380;
	globals->unknown80 = GetTickCount();
	globals->unknown1f8 = NONE;
	globals->unknown200 = NONE;
	globals->unknown84 = 0;
	g_4ed288->value20 = 0;
	g_4ed288->value24 = 0;
	if (!b)
		function_21a1e0();
}

// @retail 0x14b660
void __stdcall function_14b660(dword flags)
{
	struct
	{
		long a[2];
		long b;
		byte c[0xe40];
	} locals;

	if (flags & 8)
	{
		function_23654b(locals.c, locals.a, &locals.b);
		function_152f80(locals.a, locals.c);
	}
}

// @retail 0x155360
void __stdcall function_155360(dword flags)
{
	function_155380();
	function_155a30(*g_4e8c34);
}

// @retail 0x155f10
void __stdcall function_155f10(dword flags)
{
	if (g_4e9188.initialized)
	{
		if (g_4e9188.movie)
		{
			function_3e2ff0(g_4e9188.movie);
			g_4e9188.movie = 0;
		}
		function_1565e0();
		if (g_4e9188.flag1)
		{
			g_4e9188.flag1 = 0;
			g_4e6388 = 0;
		}
	}
}

// @retail 0x16eff0
void __stdcall function_16eff0(dword flags)
{
	if (!((flags >> 4) & 1))
	{
		long *out = g_510c70;
		long *entry = g_4e8c20->entries;
		long i;

		*out++ = g_4686c4;
		for (i = 0; i < 4; i++)
		{
			*out = NONE;
			if (i != NONE && *entry != NONE)
			{
				struct
				{
					byte unknown[4];
					short value;
				} result;

				function_11bed0((s_location *)&result, (point3f const *)&g_4e9bd4[i].state);
				*out = result.value;
			}
			entry++;
			out++;
		}
	}
}

// @retail 0x16f090
void __stdcall function_16f090(dword flags)
{
	g_4ea934 = 1;
	for (long i = 0; i < 4; i++)
		function_16f4b0(&g_4e9bd4[i]);
}

// @retail 0x16f0c0
void __stdcall function_16f0c0(dword flags)
{
	long mode = g_4e6948->mode;

	if (mode < 2 || mode > 5)
		g_4ea936 = 1;
}

// @retail 0x17d190
void __stdcall function_17d190(dword flags)
{
	if (g_4ea950->valid)
	{
		s_record_pool *array = g_4ea950;
		long index = NONE;
		long datum;

		for (;;)
		{
			index = data_find_index(array, index + 1);
			if (index == NONE)
				break;
			datum = data_datum_index(array, index);
			s_element_40 *element = ELEMENT(array, s_element_40, datum);
			if (element->datum_index == datum)
				g_4c8798[element->table_index & 0xffff] = datum;
		}
	}
}

// @retail 0x1889d0
void __stdcall function_1889d0(dword flags)
{
	s_record_pool *array = g_4ed28c;
	long datum = data_datum_index(array, function_16bc00(array, 0));

	while (datum != NONE)
	{
		s_element_18 *element = ELEMENT(array, s_element_18, datum);

		if (function_18d1c0(element->value) == datum)
		{
			long value = element->value;
			long other = function_18d1c0(value);
			if (other != NONE)
			{
				s_element_18 *other_element = ELEMENT(array, s_element_18, other);
				if (other_element->value == value)
					other_element->flags.flag5 = 0;
			}
		}

		datum = data_datum_index(array, data_find_index(array, datum == NONE ? 0 : (datum & 0xffff) + 1));
	}
}

// @retail 0x188ac0
void __stdcall function_188ac0(dword flags)
{
	bool a = (flags >> 6) & 1;
	s_record_pool *array = g_4ed28c;
	long datum = data_datum_index(array, function_16bc00(array, 0));
	s_tag_iterator iterator;
	long tag_datum;
	s_looping_sound_globals *globals;

	while (datum != NONE)
	{
		s_element_18 *element = ELEMENT(array, s_element_18, datum);

		s_element_18_flags flags5 = element->flags;

		if (element->state == 2)
			record_pool_release(array, datum);
		else if (flags5.flag5)
		{
			long value = element->value;

			if (!(function_18d360(value) && a))
			{
				if (flags5.flag6 || (*(byte *)g_4e3b44[value & 0xffff].flags & 2))
					record_pool_release(array, datum);
				else
				{
					function_18d290(datum, value);
					array = g_4ed28c;
				}
			}
		}

		datum = data_datum_index(array, data_find_index(array, datum == NONE ? 0 : (datum & 0xffff) + 1));
	}

	iterator.index = 0;
	iterator.signature = 0x736e6421;
	while ((tag_datum = function_122c70(&iterator)) != NONE)
	{
		byte index = *((byte *)g_4e3b44[tag_datum & 0xffff].flags + 0xc);

		if (index != 0xff)
		{
			long *entry = (long *)(g_51ebd4->entries + (char)index * 0x1c);

			if (entry)
			{
				entry[4] = NONE;
				entry[5] = 0;
				entry[6] = 0;
			}
		}
	}

	globals = g_4ed288;
	function_220fd0((s_sound_driver_volumes const *)globals->gains);
	memset(globals->slots, 0xff, 0x100);
	function_225ab0();
	if (flags & 0x20)
	{
		if (!g_4ed288->flag241)
			function_18bb80(3.4028235e38f);
		g_4ed288->flag241 = 0;
	}
	else
		g_4ed288->flag241 = 0;
}

// @retail 0x1c3540
void __stdcall function_1c3540(dword flags)
{
	if (!(flags & 1))
	{
		function_1c2b10();
		record_pool_release_all(g_51ebfc);
		record_pool_release_all(g_51ec00);
		g_47f058 = false;
	}
	if (flags & 2)
	{
		function_1c29d0();
		function_1c2890();
		g_47f059 = 0;
	}
	g_47f04c++;
}

// @retail 0x1c3590
void __stdcall function_1c3590(dword flags)
{
	g_47f04c--;
	if (flags & 2)
	{
		g_47f059 = 1;
		function_1c2910();
		function_1c39c0(g_47f048, 0);
		g_47f048->function_30be40((long)g_51ecac);
	}
	if (!(flags & 1))
	{
		g_47f058 = true;
		function_1c2a10();
	}
}

// @retail 0x1c7f80
void __stdcall function_1c7f80(dword flags)
{
	g_557c6c->proc2c = function_25dd20;
	g_557c6c->proc30 = function_25dd30;
}

// @retail 0x1e6af0
void __stdcall function_1e6af0(dword flags)
{
	function_1e75d0(0);
}

// @retail 0x226030
void __stdcall function_226030(dword flags)
{
	if (!(flags & 0x80))
		g_4701ec.stage = 0;
}
