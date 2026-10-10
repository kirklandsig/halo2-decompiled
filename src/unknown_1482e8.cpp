// @flags /O1 /Gr
/* UNKNOWN_1482E8.CPP: tag lookup through the tag header globals */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_234c64.h"
#include "unknown_19b516.h"

struct s_tag_indices_1482e8
{
	byte unknown00[0x84];
	long index84;
	byte unknown88[4];
	long index8c;
	byte unknown90[4];
	long index94;
};

struct s_header_globals_1482e8
{
	byte unknown00[0x110];
	long valid;
	s_tag_indices_1482e8 *indices;
};

struct s_selector_globals_1482e8
{
	byte unknown00[0x10];
	short selector;
};

// @retail 0x1482e8
void *function_1482e8(void)
{
	void *result = 0;

	if (g_4e0350 && g_4e034c)
	{
		s_selector_globals_1482e8 *selector_globals = (s_selector_globals_1482e8 *)g_4e0350;
		s_header_globals_1482e8 *header_globals = (s_header_globals_1482e8 *)g_4e034c;
		s_tag_indices_1482e8 *indices = header_globals->valid ? header_globals->indices : 0;

		if (indices)
		{
			long tag_index;
			switch (selector_globals->selector)
			{
			case 0:
				tag_index = indices->index8c;
				break;
			case 1:
				tag_index = indices->index94;
				break;
			case 2:
				tag_index = indices->index84;
				break;
			default:
				tag_index = NONE;
				break;
			}

			if (tag_index != NONE)
			{
				result = g_4e3b44[tag_index & 0xffff].bytes;
			}
		}
	}
	return result;
}

/* the user interface definitions the shared globals tag refers to */
struct s_user_interface_shared_globals
{
	byte unknown00[4];
	long user_interface_globals_tag_index;
};

// @retail 0x148350
s_type_954545 *function_148350(void)
{
	s_type_954545 *ui_globals_tag_bytes = 0;
	s_user_interface_shared_globals *shared = (s_user_interface_shared_globals *)function_1482e8();

	if (shared && shared->user_interface_globals_tag_index != NONE)
	{
		ui_globals_tag_bytes = (s_type_954545 *)g_4e3b44[shared->user_interface_globals_tag_index & 0xffff].bytes;
	}
	return ui_globals_tag_bytes;
}

/* a list skin of the user interface globals */
// @retail 0x14837a
s_sprite_placement *function_14837a(short index)
{
	s_sprite_placement *result = 0;
	s_type_954545 *globals = function_148350();

	if (globals && index >= 0 && index < globals->skin_count)
	{
		s_tag_reference_8 *skin = &globals->skins[index];
		if (skin->tag_index != NONE)
		{
			result = (s_sprite_placement *)g_4e3b44[skin->tag_index & 0xffff].bytes;
		}
	}
	return result;
}

#include "unknown_19d220.h"
long function_216b00(long type);
bool function_19d650(s_game_variant *variant);
void function_2373be(long index, void *base, long value);

struct s_variant_option_1489b5 { long index; long value; };
struct s_variant_defaults_1489b5
{
    byte unknown00[8];
    long count;
    s_variant_option_1489b5 *options;
    long unknown10;
};

// @retail 0x1489b5
void function_1489b5(long type, s_game_variant *variant)
{
    function_19d220(variant, function_216b00(type));
    long tag_index = *(long *)((byte *)function_1482e8() + 0x14);
    if (tag_index != NONE)
    {
        s_variant_defaults_1489b5 *defaults = (s_variant_defaults_1489b5 *)(g_4e3b44[tag_index & 0xffff].bytes + 0x1c) + type;
        for (long i = 0; i < defaults->count; i++)
            function_2373be(defaults->options[i].index, variant, defaults->options[i].value);
    }
    function_19d650(variant);
}
