#include "unknown_11c920.h"
#include <string.h>
#include "unknown_19b516.h"

// @flags /O1 /Oi /Ob1 /Gr

/* the fields of s_message written by the constructor-like setup below
   (unknown_19b516.h declares only the callback and field_c) */
struct s_message_view
{
	word type;
	word flags;
	long value;
	long data;
	dword field_c;
	s_id_triplet id;
	long callback;
};

// @retail 0x149f49
void function_149f49(s_message *message, word a, dword *id, short b, long c, long d, long e)
{
	s_message_view *view = (s_message_view *)message;

	view->type = a;
	view->flags = b;
	view->value = c;
	view->data = d;

	if (id)
	{
		memcpy(id, &view->id, sizeof(view->id));
	}
	else
	{
		memset(&view->id, 0xff, sizeof(view->id));
	}

	view->callback = e;
}

#include "unknown_19d220.h"
#include <wchar.h>
long function_216ab0(long index);
void function_1489b5(long type, s_game_variant *variant);
void function_1a0180(long tag_index, long string_handle, word *buffer);
void function_2373be(long index, void *base, long value);
bool function_19d650(s_game_variant *variant);
void __stdcall function_217080(s_game_variant const *variant);

struct s_preset_option_149f88 { long index; long value; };
struct s_preset_entry_149f88
{
    long name;
    long type;
    long option_count;
    s_preset_option_149f88 *options;
    char percentage;
    byte unknown11[3];
};
struct s_preset_list_149f88
{
    byte unknown00[0x24];
    long string_tag;
    long count;
    s_preset_entry_149f88 *entries;
};

// @retail 0x149f88
void __stdcall function_149f88(void *screen)
{
    s_preset_list_149f88 *list = (s_preset_list_149f88 *)screen;
    for (long index = 0; index < list->count; ++index)
    {
        s_preset_entry_149f88 *entry = &list->entries[index];
        if (entry->type >= 0 && entry->type < 7)
        {
            s_game_variant variant;
            long type = function_216ab0(entry->type);
            function_1489b5(type, &variant);
            word name[0x100];
            name[0] = 0;
            function_1a0180(list->string_tag, entry->name, name);
            wcsncpy(variant.name, (wchar_t const *)name, 0x1f);
            variant.name[0x1f] = 0;
            long percentage = entry->percentage < 0 ? 0 : (entry->percentage > 100 ? 100 : entry->percentage);
            if (percentage == entry->percentage)
                variant.unknown03 = entry->percentage;
            for (long option = 0; option < entry->option_count; ++option)
                function_2373be(entry->options[option].index, &variant, entry->options[option].value);
            if (function_19d650(&variant))
                function_217080(&variant);
        }
    }
}
