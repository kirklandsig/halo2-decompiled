#include <string.h>
#include "material_records_storage.h"
#include "visibility_storage.h"
#include "unknown_11c920.h"
#include <xtl.h>
#include "globals.h"
#include "screen_widgets.h"
#include "unknown_19b510.h"
#include "user_interface_lists.h"
#include "unknown_19b516.h"
#include "unknown_2b116a.h"
#include "unknown_18f576.h"
#include "unknown_2b6106.h"
#include "network_qos.h"
#include "loop_allocator.h"
#include "unknown_24b5bc.h"

// @flags /O1 /Oi /arch:SSE /Gr

/* UNKNOWN_2B116A.CPP: the small virtual methods of the screens and lists
   built in 0x2b0000..0x2bbfff (their load procedures and constructors sit next
   to them). Each class is named by its list's debug name where its
   constructor gives one, else by its retail vtable. */

long function_1480ff(long screen_id);
void function_236299(long sound);

/* the load procedures (some not decompiled yet: stubs in src/stubs/lane_e.cpp) */
c_class_1473c9 *__stdcall function_2b130a(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b136d(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b1467(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b19dc(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b1b01(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b1b43(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b1b85(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b1bc9(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b2801(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b288c(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b2917(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b4274(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b4397(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b4485(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b4565(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b46a6(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b47a7(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b5406(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b71f0(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b7152(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b7201(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b7212(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b7223(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b72e6(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b7333(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b739a(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b7714(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b77a8(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b7887(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b8536(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b8acd(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b8add(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b8aed(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b8b2d(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ba45b(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2baa9b(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2baae7(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2bb2db(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2bb31d(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2bb3ed(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2bb432(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2bbacb(s_screen_parameters *parameters);

/* ---- the item flags of the widget definitions (read by 0x2afeae) ---- */

// @retail 0x2b014f
long function_2b014f(s_widget_item *item)
{
	return item->has_value4 ? item->value4 : 0;
}

// @retail 0x2b015b
long function_2b015b(s_widget_item *item)
{
	return TEST_FIELD_BIT(item->has_value58) ? item->value58 : NONE;
}

// @retail 0x2b016d
long function_2b016d(s_widget_item *item)
{
	return TEST_FIELD_BIT(item->has_value5c) ? item->value5c : NONE;
}

// @retail 0x2b0180
bool function_2b0180(s_widget_item *item)
{
	return TEST_FIELD_BIT(item->has_value5f) ? item->value5f : false;
}

// @retail 0x2b0191
bool function_2b0191(s_widget_item *item)
{
	return TEST_FIELD_BIT(item->has_value5e) ? item->value5e : false;
}

// @retail 0x2b01b5
void function_2b01b5(s_widget_item *item, short value)
{
	item->value60 = value;
	if (value != NONE)
		item->flags |= 0x40;
	else
		item->flags &= ~0x40;
}


// @retail 0x2b01c7
long function_2b01c7(s_widget_item *item)
{
	return TEST_FIELD_BIT(item->has_value60) ? item->value60 : NONE;
}

// @retail 0x2b01da
long function_2b01da(s_widget_item *item)
{
	return TEST_FIELD_BIT(item->has_value64) ? item->value64 : 0;
}

/* ---- helpers of the widgets whose definition (an s_widget_item) is at
   +0x70 ---- */

/* a bitmap of a bitmap tag (0x74 bytes) */
struct s_type_7ba8e9
{
	byte unknown00[4];
	short width;
	short height;
	byte unknown08[0x74 - 8];
};

struct s_widget_view_2b0a
{
	byte unknown00[0x20];
	s_widget_bounds widget_bounds;
	byte unknown28[0x6e - 0x28];
	bool enabled;
	byte unknown6f;
	s_widget_item *definition;
	byte bounds[0x10];
	short value84;
	short value86;
	short value88;
	byte unknown8a[2];
	s_type_7ba8e9 *bitmap;
};

struct s_bitmap_tag_2b0a
{
	byte unknown00[0x44];
	long count;
	s_type_7ba8e9 *bitmaps;
};

// @retail 0x2b0a14
void function_2b0a14(s_widget_view_2b0a *widget, short index)
{
	long tag_index = *(long *)((byte *)widget->definition + 0x1c);

	if (tag_index != NONE && index >= 0)
	{
		s_bitmap_tag_2b0a *tag = (s_bitmap_tag_2b0a *)g_4e3b44[tag_index & 0xffff].bytes;

		if (index < tag->count)
			widget->value88 = index;
	}
}

// @retail 0x2b0a48
bool function_2b0a48(s_widget_view_2b0a *widget)
{
	s_widget_item *definition = widget->definition;

	return definition ? TEST_FIELD_BIT(definition->has_value4) : false;
}

// @retail 0x2b0a57
bool function_2b0a57(s_widget_view_2b0a *widget)
{
	s_widget_item *definition = widget->definition;

	return definition ? TEST_FIELD_BIT(definition->unknown1) : false;
}

// @retail 0x2b0a68
long function_2b0a68(s_widget_view_2b0a *widget)
{
	s_widget_item *definition = widget->definition;

	return definition && (definition->flags & 4);
}

void __stdcall function_2b0565(c_class_2b01eb *widget, long frame);
void __stdcall function_2b07e2(c_class_2b01eb *widget, long frame);

#include "unknown_234c64.h"
struct s_13c051;
void function_147cbc(s_13c051 const *window, dword color);
void unicode_string_snprintf(word *buffer, long maximum_count, word const *format, ...);
void function_235d69(short_rectangle2d const *rectangle, real depth,
    short_rectangle2d const *screen, color4f const *color);
s_float_rect *function_22ea81(c_class_1a2c81 *widget, s_float_rect *rect, short_rectangle2d const *frame);
void function_22cd48(c_class_22cc8e *text, short_rectangle2d const *bounds,
    short_rectangle2d const *clip, real depth, real alpha, short_rectangle2d const *screen);

bool g_54d59f;
color3f const *g_46873c;
color4f const *g_4686f8;

// @retail 0x2b07e2
void __stdcall function_2b07e2(c_class_2b01eb *widget, long frame)
{
    s_type_954545 *globals = ui_definition_get();
    dword alpha = (long)(globals ? *(real *)((byte *)globals + 0x50) * 255.0f : 255.0f);
    dword red = (long)(globals ? *(real *)((byte *)globals + 0x54) * 255.0f : 255.0f);
    dword green = (long)(globals ? *(real *)((byte *)globals + 0x58) * 255.0f : 255.0f);
    dword blue = (long)(globals ? *(real *)((byte *)globals + 0x5c) * 255.0f : 255.0f);
    dword color = (((alpha << 8) | red) << 8 | green) << 8 | blue;
    short y, x;
    function_2363d4((short_rectangle2d const *)frame, &x, &y);
    bool show_label = g_54d59f && widget->has_screen() && widget->v11() != 6;
    s_float_rect rectangle;
    function_22ea81(widget, &rectangle, (short_rectangle2d const *)frame);
    s_widget_bounds bounds;
    s_widget_bounds outline;
    bounds.left = (short)rectangle.x0;
    bounds.right = (short)rectangle.x1;
    bounds.top = (short)rectangle.y0;
    bounds.bottom = (short)rectangle.y1;
    bounds.left += x;
    bounds.right += x;
    bounds.top += y;
    bounds.bottom += y;
    function_147cbc((s_13c051 const *)&bounds, color);
    if (show_label)
    {
        c_user_interface_text_buffer_32 text;
        real depth = widget->get_depth();
        word label[16];
        unicode_string_snprintf(label, 16, (word const *)L"%d", widget->value0a);
        text.setup(label, 0, g_46873c, 0, NONE, 0, NONE);
        outline.left = widget->bounds.left;
        outline.right = widget->bounds.right;
        outline.top = widget->bounds.top;
        outline.bottom = widget->bounds.bottom;
        if (outline.bottom - outline.top > 0 && outline.right - outline.left > 0)
        {
            outline.left += x;
            outline.right += x;
            outline.top += y;
            outline.bottom += y;
            function_22cd48(&text, (short_rectangle2d const *)&outline, (short_rectangle2d const *)&outline,
                depth, widget->animation.scale, (short_rectangle2d const *)frame);
            function_235d69((short_rectangle2d const *)&outline, depth, (short_rectangle2d const *)frame, g_4686f8);
        }
    }
}

// @retail 0x2b0540
void c_class_2b01eb::v4(long frame)
{
	if (definition)
	{
		if ((byte)function_2b0a68((s_widget_view_2b0a *)this))
			function_2b07e2(this, frame);
		else
			function_2b0565(this, frame);
	}
}

void function_235f31(s_sprite_element *element, s_float_rect const *from, point2f const *split, dword color, short mode);
s_type_7ba8e9 *function_2b0b19(s_widget_view_2b0a *widget);

PRIVATE __forceinline long bitmap_channel_round_2b(real value)
{
    long result;
    __asm
    {
        fld value
        fistp result
    }
    return result;
}

// @retail 0x2b0565
void __stdcall function_2b0565(c_class_2b01eb *widget, long frame)
{
    real alpha = widget->animation.scale;
    short x, y;
    function_2363d4((short_rectangle2d const *)frame, &x, &y);
    bool show_label = g_54d59f && widget->has_screen() && widget->v11() != 6;
    if (*(long *)((byte *)widget->definition + 0x1c) != NONE)
    {
        long mode = 0;
        dword color = bitmap_channel_round_2b(alpha * 255.0f);
        color = (color << 8) | bitmap_channel_round_2b(widget->color.red * 255.0f);
        color = (color << 8) | bitmap_channel_round_2b(widget->color.green * 255.0f);
        color = (color << 8) | bitmap_channel_round_2b(widget->color.blue * 255.0f);
        s_type_7ba8e9 *bitmap = function_2b0b19((s_widget_view_2b0a *)widget);
        s_float_rect source;
        source.x0 = source.y0 = 0.0f;
        source.x1 = (real)bitmap->width;
        source.y1 = (real)bitmap->height;
        s_float_rect rectangle;
        function_22ea81(widget, &rectangle, (short_rectangle2d const *)frame);
        real offset_y = (real)-y;
        real offset_x = (real)x;
        rectangle.x0 += offset_x;
        rectangle.y0 += offset_y;
        rectangle.x1 += offset_x;
        rectangle.y1 += offset_y;
        /* a switch, not an if: retail sign-extends the field and decrements it (an if scores 109) */
        switch (*(short *)((byte *)widget->definition + 8))
        {
        case 1:
            mode = 1;
            break;
        }
        if (*(real *)((byte *)widget->definition + 0x10) == 0.0f &&
            *(real *)((byte *)widget->definition + 0x14) == 0.0f)
        {
            function_235e5e((s_sprite_element *)bitmap, &source, &rectangle, color, 1, mode);
        }
        else
        {
            function_235f31((s_sprite_element *)bitmap, &rectangle, (point2f const *)((byte *)widget + 0x7c), color, (short)mode);
        }
    }
    if (show_label)
    {
        c_user_interface_text_buffer_32 text;
        real depth = widget->get_depth();
        word label[16];
        unicode_string_snprintf(label, 16, (word const *)L"%d", widget->value0a);
        c_class_22cc8e *label_text = &text;
        label_text->setup(label, 0, g_46873c, 0, NONE, 0, NONE);
        s_widget_bounds outline;
        outline.left = widget->bounds.left;
        outline.right = widget->bounds.right;
        outline.top = widget->bounds.top;
        outline.bottom = widget->bounds.bottom;
        function_22cd48(label_text, (short_rectangle2d const *)&outline, (short_rectangle2d const *)&outline,
            depth, alpha, (short_rectangle2d const *)frame);
        outline.left += x;
        outline.right += x;
        outline.top += y;
        outline.bottom += y;
        function_235d69((short_rectangle2d const *)&outline, depth, (short_rectangle2d const *)frame, g_4686f8);
    }
}

/* the widget's bitmap: the one set, else the definition's */
// @retail 0x2b0b19
s_type_7ba8e9 *function_2b0b19(s_widget_view_2b0a *widget)
{
    s_type_7ba8e9 *result = 0;
    if (widget->bitmap)
        result = widget->bitmap;
    else
    {
        s_bitmap_tag_2b0a *tag = (s_bitmap_tag_2b0a *)g_4e3b44[*(long *)((byte *)widget->definition + 0x1c) & 0xffff].bytes;
        short index = widget->value88;
        if (index >= 0 && index < tag->count)
            result = &tag->bitmaps[index];
        else
            result = tag->bitmaps;
    }
    return result;
}

/* sets the widget's bitmap and sizes the widget to it */
// @retail 0x2b0a7b
void function_2b0a7b(s_widget_view_2b0a *widget, s_type_7ba8e9 *bitmap)
{
	s_widget_bounds bounds;
	s_type_7ba8e9 *shown;

	memset(&bounds, 0, sizeof(bounds));

	widget->bitmap = bitmap;
	shown = function_2b0b19(widget);
	if (shown)
	{
		short x = widget->definition->x;
		short y = widget->definition->y;
		short right = x + shown->width;
		bounds.top = y;
		bounds.left = x;
		bounds.right = right;
		bounds.bottom = y - shown->height;
	}
	widget->widget_bounds = bounds;
}

s_screen_definition *function_22f871(c_class_1473c9 *screen);
s_type_7ba8e9 *function_137550(long group_index, short bitmap_index);

/* shows a bitmap of the screen definition's bitmap tag in the widget */
// @retail 0x2b0ad3
void function_2b0ad3(long index, s_widget_view_2b0a *widget, long bitmap_index)
{
	c_class_1473c9 *screen = ((c_class_1a2c81 *)widget)->get_screen();

	if (screen)
	{
		s_screen_definition *definition = function_22f871(screen);

		if (definition && index >= 0 && index < definition->bitmap_count)
		{
			s_tag_reference *reference = &definition->bitmaps[index];

			if (reference)
			{
				function_2b0a7b(widget, function_137550(reference->tag_index, (short)bitmap_index));
			}
		}
	}
}

// @retail 0x2b12ba
bool function_2b12ba(s_widget_view_2b0a *widget)
{
	s_widget_item *definition = widget->definition;

	return definition ? (definition->flags & 1) : false;
}

// @retail 0x2b12ca
void function_2b12ca(s_widget_view_2b0a *widget, short b, short a, void const *bounds)
{
	bool disabled;

	memcpy(widget->bounds, bounds, sizeof(widget->bounds));
	widget->value84 = b;
	widget->value86 = a;
	if (a == NONE && widget->definition && (widget->definition->flags & 2))
		disabled = true;
	else
		disabled = false;
	widget->enabled = !disabled;
}

// @retail 0x2b2327
bool function_2b2327(s_list_item_iterator *iterator)
{
	iterator->item = record_pool_iterator_step(&iterator->iterator);
	return iterator->item != 0;
}

/* whether a screen's current item (at +0x684) is the one it holds at the
   given offset */
struct s_screen_view_2b2d
{
	byte unknown00[0x684];
	void *current;
	byte item[4];
};

// @retail 0x2b2d81
long function_2b2d81(s_screen_view_2b2d *screen)
{
	void *item = screen->item;

	return item && item == screen->current;
}

struct s_screen_view_2b39
{
	byte unknown00[0x684];
	void *current;
	byte unknown688[0x29a8 - 0x688];
	byte item[4];
};

inline __declspec(noinline) long function_2b3923(s_screen_view_2b39 *screen);

// @retail 0x2b3923
inline long function_2b3923(s_screen_view_2b39 *screen)
{
	void *item = screen->item;

	return item && item == screen->current;
}

struct s_screen_view_2b3e
{
	byte unknown00[0x684];
	void *current;
	byte unknown688[0x1818 - 0x688];
	byte item[4];
};

// @retail 0x2b3efc
long function_2b3efc(s_screen_view_2b3e *screen)
{
	void *item = screen->item;

	return item && item == screen->current;
}

/* ---- opening screens: function_149f49 builds the new screen's parameters,
   whose load procedure then builds the screen ---- */

/* load procedures outside the region (stubs in src/stubs/lane_e.cpp) */
c_class_1473c9 *__stdcall function_230c8d(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_230d6b(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2325fb(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2b54b2(s_screen_parameters *parameters);

/* the screen 0x2b54b2 loads */
struct s_screen_view_2b61
{
	byte unknown00[0xdb8];
	long value;
};

// @retail 0x2b6068
void function_2b6068(s_controller_reference **controller)
{
	s_screen_parameters parameters;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)function_2b5406);
	parameters.load(&parameters);
}

// @retail 0x2b61ce
c_class_1473c9 *function_2b61ce(long user_flags, long value)
{
	s_screen_parameters parameters;
	s_screen_view_2b61 *screen;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, user_flags, 3, 4, (long)function_2b54b2);
	screen = (s_screen_view_2b61 *)parameters.load(&parameters);
	if (screen)
		screen->value = value;
	return (c_class_1473c9 *)screen;
}

// @retail 0x2bb8ac
void function_2bb8ac(s_controller_reference **controller)
{
	s_screen_parameters parameters;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)function_2bb2db);
	parameters.load(&parameters);
}

// @retail 0x2bb8df
void function_2bb8df(s_controller_reference **controller)
{
	s_screen_parameters parameters;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)function_2b1b85);
	parameters.load(&parameters);
}

// @retail 0x2bb912
void function_2bb912(s_controller_reference **controller)
{
	s_screen_parameters parameters;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)function_2b136d);
	parameters.load(&parameters);
}

// @retail 0x2bb945
void function_2bb945(s_controller_reference **controller)
{
	s_screen_parameters parameters;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)function_2bb432);
	parameters.load(&parameters);
}

// @retail 0x2bb9ce
void function_2bb9ce(s_controller_reference **controller)
{
	s_screen_parameters parameters;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)function_2325fb);
	parameters.load(&parameters);
}

// @retail 0x2bbf06
void function_2bbf06(long controller_index, bool alternate)
{
	s_screen_parameters parameters;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, 1 << controller_index, 3, 4, (long)(alternate ? function_230c8d : function_230d6b));
	parameters.load(&parameters);
}

/* ---- widgets ---- */

/* the widgets at 0x45ad60, 0x45ada8 and 0x45adf0 share slot 6 */
class c_widget_45ad60 : public c_class_1a2c81
{
public:
	virtual long v6();

	void *data;
};

struct s_widget_data_45ad60
{
	byte unknown00[6];
	short value;
};

// @retail 0x2b116a
long c_widget_45ad60::v6()
{
	s_widget_data_45ad60 *widget_data = (s_widget_data_45ad60 *)data;

	if (widget_data)
		return widget_data->value;
	return 0;
}

/* a player's profile (player_slots.cpp) */
struct s_player_profile
{
	dword data[0x78];
};

void player_slot_get_profile(long index, s_player_profile *profile, long *profile_index);
c_class_1473c9 *__stdcall function_231db5(s_screen_parameters *parameters);

/* ---- screens ---- */

__forceinline c_level_select_screen::c_level_select_screen(long a, long b, word user_flags) :
 c_screen_with_menu(0xb, a, b, user_flags, &list),
 list(user_flags, false)
{
}

// @retail 0x2b130a
c_class_1473c9 *__stdcall function_2b130a(s_screen_parameters *parameters)
{
	c_level_select_screen *screen = new c_level_select_screen(parameters->a, parameters->b, parameters->user_flags);

	if (screen)
	{
		screen->m6c = true;
		screen->function_147f6d(parameters);
	}
	return screen;
}

// @retail 0x2b136d
c_class_1473c9 *__stdcall function_2b136d(s_screen_parameters *parameters)
{
	c_level_select_screen *screen = new c_level_select_screen(parameters->a, parameters->b, parameters->user_flags, true);

	if (screen)
	{
		screen->m6c = true;
		screen->function_147f6d(parameters);
	}
	return screen;
}

// @retail 0x2b13b3
c_level_select_screen::c_level_select_screen(long a, long b, word user_flags, bool alternate) :
	c_screen_with_menu(0xb, a, b, user_flags, &list),
	list(user_flags, alternate)
{
}

// @retail 0x2b13fe deleting c_level_select_screen
// @retail 0x2b1452 destructor c_level_select_screen
// @retail 0x2b141c destructor c_campaign_level_handles_list

// @retail 0x2b13ea
screen_load_proc c_level_select_screen::get_load_proc()
{
	return list.alternate ? function_2b136d : function_2b130a;
}

class c_campaign_options_screen : public c_class_1473c9
{
public:
	c_campaign_options_screen(long a, long b, word user_flags);

	virtual screen_load_proc get_load_proc();

	long value610;
	bool value614;
};

// @retail 0x2b14a3
c_campaign_options_screen::c_campaign_options_screen(long a, long b, word user_flags) :
	c_class_1473c9(0xd1, a, b, user_flags),
	value610(0),
	value614(false)
{
}

// @retail 0x2b1467
c_class_1473c9 *__stdcall function_2b1467(s_screen_parameters *parameters)
{
	c_campaign_options_screen *screen = new c_campaign_options_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b14d6
screen_load_proc c_campaign_options_screen::get_load_proc()
{
	return function_2b1467;
}

/* ---- the campaign options dialog's list ---- */

long player_slot_get_single_profile(void);
long player_slot_get_single_profile_index(void);
bool function_1904cb(long index);
void function_19052c(long index, long *best_key, long *best_index);
void function_148d42(long value);
bool function_148c3e(long controller, long type);
c_class_1473c9 *__stdcall function_2524a8(s_screen_parameters *parameters);
bool function_124360(long index, s_saved_game_header *header, s_saved_game_read *read);
void __stdcall function_2acab4(long a, long user_flags, long string_handle, bool (__stdcall *progress)(c_campaign_options_list *list, long unused, real *fraction, long *error), long b, c_campaign_options_list *list);
void function_120d50(bool volatile *done, bool idle);
long __stdcall function_2b186e(c_campaign_options_list *list, long unused, real *fraction, long *error);

extern bool g_54e7cc;

// @retail 0x2b14dc
c_campaign_options_list::c_campaign_options_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_campaign_options_list::handle_item),
	reading(false)
{
	long key;
	long index;
	bool has_saved_game;

	data = user_interface_data_new("campaign options list", 3, 4);
	function_16b790(data);
	index = player_slot_get_single_profile();
	has_saved_game = index != NONE && function_1904cb(index);
	function_19052c(index, &index, &key);
	if (has_saved_game)
	{
		list_item_add(this, 0);
		list_item_add(this, 1);
		list_item_add(this, 2);
	}
	else if (index != NONE)
	{
		list_item_add(this, 1);
		list_item_add(this, 2);
	}
	else
	{
		list_item_add(this, 3);
		list_item_add(this, 4);
	}
	delegate_register(&item_handlers, &handler);
	read.done = false;
}

// @retail 0x2b166d
void c_campaign_options_list::handle_item(s_controller_reference **controller, long *item)
{
	short *datum = (short *)record_pool_lookup(data, *item);

	if (datum)
	{
		s_screen_parameters parameters;

		parameters.field_c = 0;
		switch (datum[1])
		{
		case 0:
			if (!reading)
			{
				long profile = player_slot_get_single_profile_index();
				if (profile != NONE)
				{
					reading = function_124360(profile, &header, &read);
					if (reading)
					{
						function_2acab4(4, 1 << (*controller)->controller_index, 0x1b000719, (bool (__stdcall *)(c_campaign_options_list *, long, real *, long *))function_2b186e, 0, this);
					}
				}
				if (!reading)
				{
					dialog_ok_show(1, 0x81, 4, (word)(1 << (*controller)->controller_index), 0, 0);
				}
			}
			return;
		case 1:
		case 3:
			if (player_slot_get_single_profile() == NONE)
			{
				goto animate;
			}
			if ((bool)(((dword)player_slot_get_single_profile_index() >> 21) & 1))
			{
				function_148c3e((*controller)->controller_index, 3);
				return;
			}
			function_149f49((s_message *)&parameters, 0, 0, (word)(1 << (*controller)->controller_index), 5, 4, (long)function_2b130a);
			parameters.load(&parameters);
			goto animate;
		case 2:
		case 4:
			function_149f49((s_message *)&parameters, 0, 0, (word)(1 << (*controller)->controller_index), 5, 4, (long)function_2524a8);
			parameters.load(&parameters);
			goto animate;
		}
	}
animate:
	get_screen()->start_animation(3);
}

// @retail 0x2b1845
void c_campaign_options_list::v2()
{
	((c_widget *)this)->c_widget::v10();
	if (reading)
	{
		read.cancel = true;
		function_120d50(&read.done, true);
	}
}

// @retail 0x2b178e
void c_campaign_options_list::v3()
{
	((c_widget *)this)->c_widget::v11();
	if (reading && read.done)
	{
		reading = false;
		if (read.success)
		{
			s_screen_parameters parameters;
			c_campaign_options_screen *screen;

			long difficulty = *(short volatile *)&header.difficulty;
			long level = header.level;
			g_54e7c0 = 1;
			g_54e7c4 = level;
			function_148d42(difficulty);
			/* retail stores this flag before it reads user_flags, and fills field_c just before the call */
			*(bool volatile *)&g_54e7cc = true;
			parameters.field_c = 0;
			function_149f49((s_message *)&parameters, 0, 0, user_flags, 5, 4, (long)function_2b1467);
			screen = (c_campaign_options_screen *)parameters.load(&parameters);
			screen->value610 = 0;
			get_screen()->start_animation(3);
		}
		else
		{
			dialog_ok_show(1, 0x81, 4, user_flags, 0, 0);
		}
	}
}

/* the progress of the saved game read, for the progress dialog */
// @retail 0x2b186e
long __stdcall function_2b186e(c_campaign_options_list *list, long unused, real *fraction, long *error)
{
	long done = list->read.done != 0;

	*fraction = list->read.progress;
	if (list->read.done)
	{
		if (list->read.success)
		{
			*error = 0;
		}
		else
		{
			*error = list->read.error;
		}
	}
	return done;
}


// @retail 0x2b1ab3
c_variant_game_engine_type_screen::c_variant_game_engine_type_screen(long a, long b, word user_flags, long screen_id, bool edit_settings, bool create, bool edit_alternate) :
	c_screen_with_menu(screen_id, a, b, user_flags, &list),
	list(user_flags)
{
	this->edit_settings = edit_settings;
	this->create = create;
	this->edit_alternate = edit_alternate;
}

// @retail 0x2b1b01
c_class_1473c9 *__stdcall function_2b1b01(s_screen_parameters *parameters)
{
	c_variant_game_engine_type_screen *screen = new c_variant_game_engine_type_screen(parameters->a, parameters->b, parameters->user_flags, 0x3c, true, false, false);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b1b43
c_class_1473c9 *__stdcall function_2b1b43(s_screen_parameters *parameters)
{
	c_variant_game_engine_type_screen *screen = new c_variant_game_engine_type_screen(parameters->a, parameters->b, parameters->user_flags, 0x3c, false, true, false);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b1b85
c_class_1473c9 *__stdcall function_2b1b85(s_screen_parameters *parameters)
{
	c_variant_game_engine_type_screen *screen = new c_variant_game_engine_type_screen(parameters->a, parameters->b, parameters->user_flags, 0xcf, false, false, false);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b1bc9
c_class_1473c9 *__stdcall function_2b1bc9(s_screen_parameters *parameters)
{
	c_variant_game_engine_type_screen *screen = new c_variant_game_engine_type_screen(parameters->a, parameters->b, parameters->user_flags, 0x3c, false, false, true);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b1c61 deleting c_variant_game_engine_type_screen
// @retail 0x2b1cb5 destructor c_variant_game_engine_type_screen
// @retail 0x2b1c7f destructor c_game_engine_variant_category_list

// @retail 0x2b1c0b
void c_variant_game_engine_type_screen::v17()
{
	list.edit_settings = edit_settings;
	list.create = create;
	list.edit_alternate = edit_alternate;
}

// @retail 0x2b1c30
screen_load_proc c_variant_game_engine_type_screen::get_load_proc()
{
	if (create)
		return function_2b1b43;
	if (edit_settings)
		return function_2b1b01;
	return edit_alternate ? function_2b1bc9 : function_2b1b85;
}

/* the xbox live appear offline screen (vtable 0x45b220) */
class c_xbox_live_appear_offline_screen : public c_screen_with_menu
{
public:
	c_xbox_live_appear_offline_screen(long a, long b, word user_flags);

	virtual screen_load_proc get_load_proc();

	c_xbox_live_appear_offline_list list;
};

// @retail 0x2b2801
c_class_1473c9 *__stdcall function_2b2801(s_screen_parameters *parameters)
{
	c_xbox_live_appear_offline_screen *screen = new c_xbox_live_appear_offline_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b283f
c_xbox_live_appear_offline_screen::c_xbox_live_appear_offline_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0x3a, a, b, user_flags, &list),
	list(user_flags)
{
}

// @retail 0x2b2871
screen_load_proc c_xbox_live_appear_offline_screen::get_load_proc()
{
	return function_2b2801;
}

// @retail 0x147494 deleting c_xbox_live_appear_offline_screen
// @retail 0x2b2877 destructor c_xbox_live_appear_offline_screen

/* the voice mask screen (vtable 0x45b290) */
class c_voice_mask_screen : public c_screen_with_menu
{
public:
	c_voice_mask_screen(long a, long b, word user_flags);

	virtual screen_load_proc get_load_proc();

	c_voice_mask_list list;
};

// @retail 0x2b288c
c_class_1473c9 *__stdcall function_2b288c(s_screen_parameters *parameters)
{
	c_voice_mask_screen *screen = new c_voice_mask_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b4ed2
c_class_1473c9 *__stdcall function_2b4ed2(s_screen_parameters *parameters)
{
	c_voice_mask_screen *screen = new c_voice_mask_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->list.value188 = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b28ca
c_voice_mask_screen::c_voice_mask_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0x36, a, b, user_flags, &list),
	list(user_flags)
{
}

// @retail 0x2b28fc
screen_load_proc c_voice_mask_screen::get_load_proc()
{
	return function_2b288c;
}

/* the voice through tv screen (vtable 0x45b300) */
class c_voice_through_tv_screen : public c_screen_with_menu
{
public:
	c_voice_through_tv_screen(long a, long b, word user_flags);

	virtual screen_load_proc get_load_proc();

	c_voice_through_tv_list list;
};

// @retail 0x2b2917
c_class_1473c9 *__stdcall function_2b2917(s_screen_parameters *parameters)
{
	c_voice_through_tv_screen *screen = new c_voice_through_tv_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b4f17
c_class_1473c9 *__stdcall function_2b4f17(s_screen_parameters *parameters)
{
	c_voice_through_tv_screen *screen = new c_voice_through_tv_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->list.value288 = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b2955
c_voice_through_tv_screen::c_voice_through_tv_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0x37, a, b, user_flags, &list),
	list(user_flags)
{
}

// @retail 0x2b2987
screen_load_proc c_voice_through_tv_screen::get_load_proc()
{
	return function_2b2917;
}

/* the thumbstick settings screen (vtable 0x45b628) */
class c_thumbstick_settings_screen : public c_screen_with_menu
{
public:
	c_thumbstick_settings_screen(long a, long b, word user_flags);

	/* shows the pane of the controller's kind */
	virtual void v3();
	virtual void v19();
	virtual screen_load_proc get_load_proc();

	c_thumbstick_settings_edit_list list;
	long value8b8;
};

// @retail 0x2b4274
c_class_1473c9 *__stdcall function_2b4274(s_screen_parameters *parameters)
{
	c_thumbstick_settings_screen *screen = new c_thumbstick_settings_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b42b6
c_class_1473c9 *__stdcall function_2b42b6(s_screen_parameters *parameters)
{
	c_thumbstick_settings_screen *screen = new c_thumbstick_settings_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->list.value288 = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b42ff
c_thumbstick_settings_screen::c_thumbstick_settings_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0x29, a, b, user_flags, &list),
	list(user_flags),
	value8b8(NONE)
{
	if (b != 4)
	{
		set_screen_id(0xeb);
	}
}

// @retail 0x2b434c
void c_thumbstick_settings_screen::v19()
{
	switch (g_54e5d0.settings.thumbstick_layout)
	{
	case 0:
		list.select_item(0);
		break;
	case 1:
		list.select_item(1);
		break;
	case 2:
		list.select_item(2);
		break;
	case 3:
		list.select_item(3);
		break;
	default:
		list.select_item(0);
		break;
	}
	c_class_1473c9::v19();
}

// @retail 0x2b4391
screen_load_proc c_thumbstick_settings_screen::get_load_proc()
{
	return function_2b4274;
}

long function_1249d0(short gamepad_index);

// @retail 0x2c6170
void c_thumbstick_settings_screen::v3()
{
	long controller_index;

	c_class_1a2c81::v3();
	controller_index = get_controller_index();
	if (controller_index != NONE)
	{
		long type = function_1249d0((short)controller_index);

		if (type != value8b8)
		{
			long datum;

			switch (type)
			{
			case 1:
				value5f3 = 4;
				break;
			default:
				value5f3 = 0;
				break;
			}
			datum = list.get_focused_datum();
			select_pane((short *)&datum);
			value8b8 = type;
		}
	}
}

// @retail 0x2b4688 deleting c_thumbstick_settings_screen
// @retail 0x2b8bf0 destructor c_thumbstick_settings_screen
// @retail 0x2b83a5 destructor c_thumbstick_settings_edit_list

/* the look sensitivity settings screen (vtable 0x45b7d0) */
class c_look_sensitivity_settings_screen : public c_screen_with_menu
{
public:
	c_look_sensitivity_settings_screen(long a, long b, word user_flags);

	virtual void v19();
	virtual screen_load_proc get_load_proc();

	c_look_sensitivity_settings_edit_list list;
};

// @retail 0x2b4397
c_class_1473c9 *__stdcall function_2b4397(s_screen_parameters *parameters)
{
	c_look_sensitivity_settings_screen *screen = new c_look_sensitivity_settings_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b43d5
c_class_1473c9 *__stdcall function_2b43d5(s_screen_parameters *parameters)
{
	c_look_sensitivity_settings_screen *screen = new c_look_sensitivity_settings_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->list.value288 = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b441a
c_look_sensitivity_settings_screen::c_look_sensitivity_settings_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0x2a, a, b, user_flags, &list),
	list(user_flags)
{
}

// @retail 0x2b444c
void c_look_sensitivity_settings_screen::v19()
{
	byte sensitivity = g_54e5d0.settings.look_sensitivity;
	short item = sensitivity < 1 ? 1 : sensitivity > 10 ? 10 : sensitivity;

	list.select_item(item - 1);
	c_class_1473c9::v19();
}

// @retail 0x2b447f
screen_load_proc c_look_sensitivity_settings_screen::get_load_proc()
{
	return function_2b4397;
}

/* the invert look settings screen (vtable 0x45b708) */
class c_invert_look_settings_screen : public c_screen_with_menu
{
public:
	c_invert_look_settings_screen(long a, long b, word user_flags);

	virtual void v19();
	virtual screen_load_proc get_load_proc();

	c_invert_look_settings_edit_list list;
};

// @retail 0x2b4485
c_class_1473c9 *__stdcall function_2b4485(s_screen_parameters *parameters)
{
	c_invert_look_settings_screen *screen = new c_invert_look_settings_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b44c3
c_class_1473c9 *__stdcall function_2b44c3(s_screen_parameters *parameters)
{
	c_invert_look_settings_screen *screen = new c_invert_look_settings_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->list.value188 = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b4508
c_invert_look_settings_screen::c_invert_look_settings_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0x2b, a, b, user_flags, &list),
	list(user_flags)
{
}

// @retail 0x2b453a
void c_invert_look_settings_screen::v19()
{
	if (TEST_FIELD_BIT(g_54e5d0.settings.controller_flags.invert_look))
	{
		list.select_item(0);
	}
	else
	{
		list.select_item(1);
	}
	c_class_1473c9::v19();
}

// @retail 0x2b455f
screen_load_proc c_invert_look_settings_screen::get_load_proc()
{
	return function_2b4485;
}

// @retail 0x2b4789 deleting c_invert_look_settings_screen
// @retail 0x2b2902 destructor c_invert_look_settings_screen
// @retail 0x2b488c destructor c_invert_look_settings_edit_list

/* the button settings screen (vtable 0x45b840) */
class c_button_settings_screen : public c_screen_with_menu
{
public:
	c_button_settings_screen(long a, long b, word user_flags);

	/* shows the pane of the controller's kind */
	virtual void v3();
	virtual void v19();
	virtual screen_load_proc get_load_proc();

	c_button_settings_edit_list list;
	long value8b8;
};

// @retail 0x2c65bf
void c_button_settings_screen::v3()
{
	long controller_index = get_controller_index();

	if (controller_index != NONE)
	{
		long type = function_1249d0((short)controller_index);

		if (type != value8b8)
		{
			long datum;

			switch (type)
			{
			case 1:
				value5f3 = 4;
				break;
			default:
				value5f3 = 0;
				break;
			}
			datum = list.get_focused_datum();
			select_pane((short *)&datum);
			value8b8 = type;
		}
	}
	c_class_1a2c81::v3();
}

// @retail 0x2b4565
c_class_1473c9 *__stdcall function_2b4565(s_screen_parameters *parameters)
{
	c_button_settings_screen *screen = new c_button_settings_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b45a7
c_class_1473c9 *__stdcall function_2b45a7(s_screen_parameters *parameters)
{
	c_button_settings_screen *screen = new c_button_settings_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->list.value288 = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b45f0
c_button_settings_screen::c_button_settings_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0x28, a, b, user_flags, &list),
	list(user_flags),
	value8b8(NONE)
{
	if (b != 4)
	{
		set_screen_id(0xea);
	}
}

// @retail 0x2b463d
void c_button_settings_screen::v19()
{
	switch (g_54e5d0.settings.button_layout)
	{
	case 0:
		list.select_item(0);
		break;
	case 1:
		list.select_item(1);
		break;
	case 2:
		list.select_item(2);
		break;
	case 3:
		list.select_item(3);
		break;
	default:
		list.select_item(0);
		break;
	}
	c_class_1473c9::v19();
}

// @retail 0x2b4682
screen_load_proc c_button_settings_screen::get_load_proc()
{
	return function_2b4565;
}

/* the auto level settings screen (vtable 0x45b5b8) */
class c_auto_level_settings_screen : public c_screen_with_menu
{
public:
	c_auto_level_settings_screen(long a, long b, word user_flags);

	virtual void v19();
	virtual screen_load_proc get_load_proc();

	c_auto_level_settings_edit_list list;
};

// @retail 0x2b46a6
c_class_1473c9 *__stdcall function_2b46a6(s_screen_parameters *parameters)
{
	c_auto_level_settings_screen *screen = new c_auto_level_settings_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b46e4
c_class_1473c9 *__stdcall function_2b46e4(s_screen_parameters *parameters)
{
	c_auto_level_settings_screen *screen = new c_auto_level_settings_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->list.value188 = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b4729
c_auto_level_settings_screen::c_auto_level_settings_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0x2c, a, b, user_flags, &list),
	list(user_flags)
{
}

// @retail 0x2b475b
void c_auto_level_settings_screen::v19()
{
	if (TEST_FIELD_BIT(g_54e5d0.settings.controller_flags.auto_level))
	{
		list.select_item(0);
	}
	else
	{
		list.select_item(1);
	}
	c_class_1473c9::v19();
}

// @retail 0x2b4783
screen_load_proc c_auto_level_settings_screen::get_load_proc()
{
	return function_2b46a6;
}

/* the vibration settings screen (vtable 0x45b698) */
class c_vibration_settings_screen : public c_screen_with_menu
{
public:
	c_vibration_settings_screen(long a, long b, word user_flags);

	virtual void v19();
	virtual screen_load_proc get_load_proc();

	c_vibration_settings_edit_list list;
};

// @retail 0x2b47a7
c_class_1473c9 *__stdcall function_2b47a7(s_screen_parameters *parameters)
{
	c_vibration_settings_screen *screen = new c_vibration_settings_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b47e5
c_class_1473c9 *__stdcall function_2b47e5(s_screen_parameters *parameters)
{
	c_vibration_settings_screen *screen = new c_vibration_settings_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->list.value188 = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b482a
c_vibration_settings_screen::c_vibration_settings_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0xb7, a, b, user_flags, &list),
	list(user_flags)
{
}

// @retail 0x2b485f
void c_vibration_settings_screen::v19()
{
	if (!TEST_FIELD_BIT(g_54e5d0.settings.controller_flags.vibration))
	{
		list.select_item(0);
	}
	else
	{
		list.select_item(1);
	}
	c_class_1473c9::v19();
}

// @retail 0x2b4886
screen_load_proc c_vibration_settings_screen::get_load_proc()
{
	return function_2b47a7;
}

// @retail 0x2b5406
c_class_1473c9 *__stdcall function_2b5406(s_screen_parameters *parameters)
{
	c_clan_member_privileges_screen *screen = new c_clan_member_privileges_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b5444
c_clan_member_privileges_screen::c_clan_member_privileges_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0xec, a, b, user_flags, &list),
	list(user_flags)
{
}

// @retail 0x2b547f deleting c_clan_member_privileges_screen
// @retail 0x2b549d destructor c_clan_member_privileges_screen
// @retail 0x2b53ea deleting c_clan_member_privileges_list

// @retail 0x2b5479
screen_load_proc c_clan_member_privileges_screen::get_load_proc()
{
	return function_2b5406;
}

class c_screen_45bc60 : public c_class_1473c9
{
public:
	c_screen_45bc60(long a, long b);

	virtual void v18(void *parameters);
	virtual screen_load_proc get_load_proc();
};

/* marks the widget and its children as animated in (bit 3 of the
   animation's flags) */
// @retail 0x2b730e
void __stdcall function_2b730e(c_class_1a2c81 *widget)
{
	widget->animation.flags.flag3 = true;
	for (c_class_1a2c81 *child = widget->child; child; child = child->next)
	{
		function_2b730e(child);
	}
}

// @retail 0x2b7289
void c_screen_45bc60::v18(void *parameters)
{
	volatile long definition_index = function_1480ff(screen_id);
	s_screen_layout layout =
	{
		0,
		1,
		{
			{ 0, 0, 0, 0 }
		}
	};

	build(&layout);
	c_class_1a2c81::v1();
	start_animation(4);
	function_2b730e(this);
}

// @retail 0x2b7263
screen_load_proc c_screen_45bc60::get_load_proc()
{
	return function_2b72e6;
}

// @retail 0x2b7269
c_screen_45bc60::c_screen_45bc60(long a, long b) :
	c_class_1473c9(5, a, b, 0)
{
}

// @retail 0x2b72e6
c_class_1473c9 *__stdcall function_2b72e6(s_screen_parameters *parameters)
{
	c_screen_45bc60 *screen = new c_screen_45bc60(parameters->a, parameters->b);

	screen->m6c = true;
	return screen;
}

class c_screen_45bcd0 : public c_class_1473c9
{
public:
	c_screen_45bcd0(long a, long b, word user_flags);

	virtual screen_load_proc get_load_proc();
};

// @retail 0x2b7372
c_screen_45bcd0::c_screen_45bcd0(long a, long b, word user_flags) :
	c_class_1473c9(0x1b, a, b, user_flags)
{
}

/* builds the screen at once */
// @retail 0x2b7333
c_class_1473c9 *__stdcall function_2b7333(s_screen_parameters *parameters)
{
	c_screen_45bcd0 *screen = new c_screen_45bcd0(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->v18(parameters);
	return screen;
}

// @retail 0x2b732d
screen_load_proc c_screen_45bcd0::get_load_proc()
{
	return function_2b7333;
}

void ascii_string_to_unicode(const char *source, word *destination, long maximum_count);

// @retail 0x2b739a
c_class_1473c9 *__stdcall function_2b739a(s_screen_parameters *parameters)
{
	c_screen_45bd40 *screen = new c_screen_45bd40(parameters->a, parameters->b, parameters->user_flags);

	if (screen)
	{
		screen->m6c = true;
		screen->v18(parameters);
	}
	return screen;
}

// @retail 0x2b73dd
c_screen_45bd40::c_screen_45bd40(long a, long b, word user_flags) :
	c_class_1473c9(0xc0, a, b, user_flags)
{
	text[0] = 0;
}

// @retail 0x2b7460
void c_screen_45bd40::set_text(const char *string)
{
	ascii_string_to_unicode(string, text, 0x10);
}

// @retail 0x2b746e
void c_screen_45bd40::set_bitmap(short index)
{
	s_widget_view_2b0a *bitmap = (s_widget_view_2b0a *)find_child(8, 0, false);

	if (bitmap)
	{
		function_2b0a14(bitmap, index);
	}
}

// @retail 0x2b748b
void c_screen_45bd40::v3()
{
	c_class_1a2c81::v3();
	if (text[0])
	{
		c_class_1a2c81 *widget = find_child(6, 1, false);

		if (widget)
		{
			widget->function_22f52e()->set_text(text);
		}
	}
}

// @retail 0x2b740a
void c_screen_45bd40::v18(void *parameters)
{
	volatile long definition_index = function_1480ff(screen_id);
	s_screen_layout layout =
	{
		0,
		1,
		{
			{ 0, 0, 0, 0 }
		}
	};

	build(&layout);
	c_class_1a2c81::v1();
	function_236299(8);
}

// @retail 0x2b7394
screen_load_proc c_screen_45bd40::get_load_proc()
{
	return function_2b739a;
}

/* the controller settings screen (vtable 0x45be30) */
class c_controller_settings_screen : public c_screen_with_menu
{
public:
	c_controller_settings_screen(long a, long b, word user_flags);

	virtual screen_load_proc get_load_proc();

	c_controller_settings_edit_list list;
};

// @retail 0x2b7714
c_class_1473c9 *__stdcall function_2b7714(s_screen_parameters *parameters)
{
	c_controller_settings_screen *screen = new c_controller_settings_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b7752
c_controller_settings_screen::c_controller_settings_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0x27, a, b, user_flags, &list),
	list(user_flags)
{
}

// @retail 0x2b7784
screen_load_proc c_controller_settings_screen::get_load_proc()
{
	return function_2b7714;
}

/* the multiplayer settings screen (vtable 0x45bef8) */
class c_multiplayer_settings_screen : public c_screen_with_menu
{
public:
	c_multiplayer_settings_screen(long a, long b, word user_flags);
	virtual void v3();

	virtual screen_load_proc get_load_proc();

	c_multiplayer_settings_edit_list list;
};

// @retail 0x2b77a8
c_class_1473c9 *__stdcall function_2b77a8(s_screen_parameters *parameters)
{
	c_multiplayer_settings_screen *screen = new c_multiplayer_settings_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b77e6
c_multiplayer_settings_screen::c_multiplayer_settings_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0x2f, a, b, user_flags, &list),
	list(user_flags)
{
}

// @retail 0x2b7818
screen_load_proc c_multiplayer_settings_screen::get_load_proc()
{
	return function_2b77a8;
}

// @retail 0x2b781e deleting c_multiplayer_settings_screen
// @retail 0x2b7872 destructor c_multiplayer_settings_screen

/* the subtitle setting screen (vtable 0x45bf68; its deleting destructor is
   folded with c_settings_screen's) */
class c_subtitle_setting_screen : public c_screen_with_menu
{
public:
	c_subtitle_setting_screen(long a, long b, word user_flags);

	virtual screen_load_proc get_load_proc();

	c_subtitle_setting_list list;
};

// @retail 0x2b7887
c_class_1473c9 *__stdcall function_2b7887(s_screen_parameters *parameters)
{
	c_subtitle_setting_screen *screen = new c_subtitle_setting_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b78c5
c_subtitle_setting_screen::c_subtitle_setting_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0xf2, a, b, user_flags, &list),
	list(user_flags)
{
}

// @retail 0x2b78fa
screen_load_proc c_subtitle_setting_screen::get_load_proc()
{
	return function_2b7887;
}

bool function_19a951(long player_index);
bool function_19ab77(long player_index);

/* an item of a list whose data holds a value per item */
struct s_list_item_value
{
	short salt;
	short item;
	long value;
};

/* "potential squad leader player list" (vtable 0x45c150) */
class c_potential_squad_leader_player_list : public c_class_1474e8
{
public:
	c_potential_squad_leader_player_list(word user_flags);

	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);
	void fill();

	c_class_14750b items[4];
	bool value288;
	c_list_item_handler handler;
};

/* the squad leader screen (vtable 0x45c0e0) */
class c_screen_45c0e0 : public c_class_1473c9
{
public:
	c_screen_45c0e0(long a, long b, word user_flags);

	virtual void v18(void *parameters);
	virtual screen_load_proc get_load_proc();

	c_potential_squad_leader_player_list list;
};

// @retail 0x2b85d9
void c_screen_45c0e0::v18(void *parameters)
{
	volatile long definition_index = function_1480ff(screen_id);
	s_screen_layout layout =
	{
		0,
		1,
		{
			{ 0, 0, &list, 0 }
		}
	};

	build(&layout);
	c_class_1a2c81::v1();
}

// @retail 0x2b8536
c_class_1473c9 *__stdcall function_2b8536(s_screen_parameters *parameters)
{
	c_screen_45c0e0 *screen = new c_screen_45c0e0(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b8572
c_screen_45c0e0::c_screen_45c0e0(long a, long b, word user_flags) :
	c_class_1473c9(0xb1, a, b, user_flags),
	list(user_flags)
{
}

// @retail 0x2b831d
screen_load_proc c_screen_45c0e0::get_load_proc()
{
	return function_2b8536;
}

// @retail 0x2b85a6 deleting c_screen_45c0e0
// @retail 0x2b85c4 destructor c_screen_45c0e0

// @retail 0x2b8323
c_potential_squad_leader_player_list::c_potential_squad_leader_player_list(word user_flags) :
	c_class_1474e8(user_flags),
	value288(false),
	handler(this, (list_item_method)&c_potential_squad_leader_player_list::handle_item)
{
	data = user_interface_data_new("potential squad leader player list", 16, 8);
	function_16b790(data);
	delegate_register(&item_handlers, &handler);
	fill();
}

// @retail 0x2b8446
void c_potential_squad_leader_player_list::fill()
{
	long players[16];
	long count = 0;

	for (long i = 0; i < 16; i++)
	{
		if (function_19a951(i) && !function_19ab77(i))
		{
			players[count++] = i;
		}
	}
	while (--count >= 0)
	{
		((s_list_item_value *)data->data)[record_pool_allocate(data) & 0xffff].value = players[count];
	}
}

/* ---- the live feedback dialog: feedback on a player (mode 1) or a clan
   (mode 2) ---- */

/* "feedback list" (vtable 0x45c1d0) */
class c_feedback_list : public c_class_1474e8
{
public:
	c_feedback_list(word user_flags, long mode);
	~c_feedback_list();

	/* folded with c_widget's v2 */
	virtual void *get_item_data() { return items; }
	virtual long get_item_count() { return 4; }
	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	c_class_14750b items[4];
	c_list_item_handler handler;
	long kind;
	long mode;
};

/* the live feedback dialog (vtable 0x45c228) */
class c_live_feedback_dialog_screen : public c_screen_with_menu
{
public:
	c_live_feedback_dialog_screen(long a, long b, word user_flags, long mode);

	virtual screen_load_proc get_load_proc();

	c_feedback_list list;
	long mode;
};

/* the player or clan the feedback is about (0x78 bytes, from
   function_14887e) */
#pragma pack(push, 4)
struct s_feedback_target
{
	long type;
	union
	{
		unsigned __int64 field_4_6;
		unsigned __int64 clan_id;
	};
	byte unknown0c[0x78 - 0x0c];
};
#pragma pack(pop)

/* the target's player or clan, when it is one */
static __forceinline unsigned __int64 *feedback_target_xuid(s_feedback_target *target)
{
	unsigned __int64 *xuid = 0;

	switch (target->type)
	{
	case 1:
		xuid = &target->field_4_6;
		break;
	case 2:
		xuid = &target->clan_id;
		break;
	}
	return xuid;
}

struct s_screen_settings_54dc6c;
void function_14887e(s_screen_settings_54dc6c *settings);
bool __stdcall function_8c150(void *cache, long a, unsigned __int64 *xuid, void *data, unsigned __int64 *clan_id);
struct _XUID;
void online_feedback_send(const _XUID *xuid, unsigned long controller_index, long kind);
void function_14800c(long channel, long index);

extern byte g_4771c8[0x2580];

/* the list waiting for the dialog's answer */
c_feedback_list *g_51ecbc;

// @retail 0x2b86fa
c_feedback_list::c_feedback_list(word user_flags, long mode) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_feedback_list::handle_item)
{
	
#pragma pack(push, 4)
	struct s_feedback_scratch
	{
		byte buffer[0x20];
		s_feedback_target target;
		unsigned __int64 clan_id;
		dword unknown_tail;
	};
#pragma pack(pop)
	s_feedback_scratch scratch;
	bool in_clan;
	unsigned __int64 *xuid;

	this->mode = mode;
	function_14887e((s_screen_settings_54dc6c *)&scratch.target);
	xuid = feedback_target_xuid(&scratch.target);
	if (function_8c150(g_4771c8, 0, xuid, scratch.buffer, &scratch.clan_id) && scratch.clan_id)
	{
		in_clan = true;
	}
	else
	{
		in_clan = false;
	}
	data = user_interface_data_new("feedback list", 10, 4);
	function_16b790(data);
	if (this->mode == 1)
	{
		list_item_add(this, 0);
		list_item_add(this, 1);
		list_item_add(this, 2);
		list_item_add(this, 3);
		list_item_add(this, 4);
		list_item_add(this, 5);
		list_item_add(this, 6);
		if (in_clan)
		{
			list_item_add(this, 7);
		}
	}
	else if (this->mode == 2)
	{
		list_item_add(this, 8);
		list_item_add(this, 9);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2b862d
c_feedback_list::~c_feedback_list()
{
	g_51ecbc = 0;
}

// @retail 0x2b8670 deleting c_feedback_list

// @retail 0x2b88d1
void c_feedback_list::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);
	s_list_item_datum *datum = (s_list_item_datum *)record_pool_lookup(data, widget_item(widget)->value70);

	if (text && datum)
	{
		long string_handle;

		switch (datum->item)
		{
		case 0:
			string_handle = 0xd000291;
			break;
		case 1:
			string_handle = 0xb000292;
			break;
		case 2:
			string_handle = 0xc000293;
			break;
		case 3:
			string_handle = 0x8000294;
			break;
		case 4:
			string_handle = 0x10000295;
			break;
		case 5:
			string_handle = 0xa000296;
			break;
		case 6:
			string_handle = 0x8000297;
			break;
		case 7:
			string_handle = 0xd000298;
			break;
		case 9:
			string_handle = 0x1500029a;
			break;
		case 8:
			string_handle = 0xc000299;
			break;
		default:
			string_handle = NONE;
			break;
		}
		text->function_253b1a(string_handle);
	}
}

/* the dialog's answer: sends the feedback */
// @retail 0x2b8989
bool __stdcall function_2b8989(long controller)
{
	if (g_51ecbc)
	{
		s_feedback_target target;
		unsigned __int64 *xuid;

		function_14887e((s_screen_settings_54dc6c *)&target);
		xuid = feedback_target_xuid(&target);
		if (xuid && *xuid)
		{
			online_feedback_send((const _XUID *)xuid, controller, g_51ecbc->kind);
		}
		function_14800c(g_51ecbc->v11(), g_51ecbc->v12());
	}
	return true;
}

// @retail 0x2b89ee
void c_feedback_list::handle_item(s_controller_reference **controller, long *item)
{
	s_list_item_datum *datum = (s_list_item_datum *)record_pool_lookup(data, *item);

	if (datum)
	{
		s_feedback_target target;
		unsigned __int64 *xuid;
		long feedback = datum->item;

		kind = feedback;
		function_14887e((s_screen_settings_54dc6c *)&target);
		xuid = feedback_target_xuid(&target);
		if (xuid && *xuid)
		{
			if (feedback > 1)
			{
				g_51ecbc = this;
				dialog_choice_show(1, 0xa5, 4, 1 << (*controller)->controller_index, function_2b8989, 0, 0);
				return;
			}
			online_feedback_send((const _XUID *)xuid, (*controller)->controller_index, feedback);
		}
		function_14800c(v11(), v12());
	}
}

// @retail 0x2b868e
c_live_feedback_dialog_screen::c_live_feedback_dialog_screen(long a, long b, word user_flags, long mode) :
	c_screen_with_menu(0x22, a, b, user_flags, &list),
	list(user_flags, mode)
{
	this->mode = mode;
}

// @retail 0x2b86c7 deleting c_live_feedback_dialog_screen
// @retail 0x2b86e5 destructor c_live_feedback_dialog_screen

// @retail 0x2b8a89
c_class_1473c9 *live_feedback_dialog_load(s_screen_parameters *parameters, long mode)
{
	c_live_feedback_dialog_screen *screen = new c_live_feedback_dialog_screen(parameters->a, parameters->b, parameters->user_flags, mode);

	if (screen)
	{
		screen->m6c = true;
		screen->function_147f6d(parameters);
	}
	return screen;
}

// @retail 0x2b8acd
c_class_1473c9 *__stdcall function_2b8acd(s_screen_parameters *parameters)
{
	return live_feedback_dialog_load(parameters, 0);
}

// @retail 0x2b8add
c_class_1473c9 *__stdcall function_2b8add(s_screen_parameters *parameters)
{
	return live_feedback_dialog_load(parameters, 1);
}

// @retail 0x2b8aed
c_class_1473c9 *__stdcall function_2b8aed(s_screen_parameters *parameters)
{
	return live_feedback_dialog_load(parameters, 2);
}

// @retail 0x2b8afd
screen_load_proc c_live_feedback_dialog_screen::get_load_proc()
{
	screen_load_proc result = function_2b8acd;

	switch (mode)
	{
	case 1:
		result = function_2b8add;
		break;
	case 2:
		result = function_2b8aed;
		break;
	}
	return result;
}

/* a list of up to four items that calls back with the chosen item
   (vtable 0x45c318) */
typedef bool (__stdcall *multiple_choice_callback)(long controller_index, long item);

class c_list_45c318 : public c_class_1474e8
{
public:
	c_list_45c318(word user_flags);

	virtual long get_item_count();
	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);
	/* one item per string */
	void set_choices(long count, long *string_ids);

	c_class_14750b items[4];
	multiple_choice_callback callback;
	c_list_item_handler handler;
};

/* the screen of the list of 0x45c318 (vtable 0x45c2a8; its deleting
   destructor is folded with c_thumbstick_settings_screen's) */
class c_screen_45c2a8 : public c_screen_with_menu
{
public:
	c_screen_45c2a8(long a, long b, word user_flags);

	virtual void v19();
	virtual screen_load_proc get_load_proc();

	c_list_45c318 list;
	/* the string of the title text */
	long value8b8;
};

// @retail 0x2b8b2d
c_class_1473c9 *__stdcall function_2b8b2d(s_screen_parameters *parameters)
{
	c_screen_45c2a8 *screen = new c_screen_45c2a8(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2b8b71
c_screen_45c2a8::c_screen_45c2a8(long a, long b, word user_flags) :
	c_screen_with_menu(0xe6, a, b, user_flags, &list),
	list(user_flags),
	value8b8(0)
{
}

// @retail 0x2b8bad
void c_screen_45c2a8::v19()
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)find_child(6, 2, false);

	if (text)
	{
		text->function_253b1a(value8b8);
	}
	c_class_1473c9::v19();
}

// @retail 0x2b8b6b
screen_load_proc c_screen_45c2a8::get_load_proc()
{
	return function_2b8b2d;
}

// @retail 0x2b8b1a
long c_list_45c318::get_item_count()
{
	if (data)
	{
		dword count = data->actual_count;

		if (count <= 4)
			return count;
	}
	return 4;
}

// @retail 0x2b8c51
c_list_45c318::c_list_45c318(word user_flags) :
	c_class_1474e8(user_flags),
	callback(0),
	handler(this, (list_item_method)&c_list_45c318::handle_item)
{
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2b8d07
void c_list_45c318::handle_item(s_controller_reference **controller, long *item)
{
	if (callback)
	{
		s_list_item_value *entry = &((s_list_item_value *)data->data)[*item & 0xffff];

		if (callback((*controller)->controller_index, entry->item))
		{
			get_screen()->start_animation(3);
		}
	}
}

// @retail 0x2b8d50
void c_list_45c318::set_choices(long count, long *string_ids)
{
	data = user_interface_data_new("multiple choice list", count, 8);
	function_16b790(data);
	for (long i = 0; i < count; i++)
	{
		long datum_index = record_pool_allocate(data);
		s_list_item_value *entry = &((s_list_item_value *)data->data)[datum_index & 0xffff];

		entry->item = (short)i;
		entry->value = string_ids[i];
	}
}

// @retail 0x2b8bd6
void function_2b8bd6(c_screen_45c2a8 *screen, long count, long *string_ids)
{
	screen->list.set_choices(count, string_ids);
}

/* opens a dialog that offers the strings and calls back with the choice */
// @retail 0x2b8c05
void function_2b8c05(long a, long b, word user_flags, multiple_choice_callback callback, long title, long count, long *string_ids)
{
	s_screen_parameters parameters;
	c_screen_45c2a8 *screen;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, user_flags, a, b, (long)function_2b8b2d);
	screen = (c_screen_45c2a8 *)parameters.load(&parameters);
	screen->value8b8 = title;
	screen->list.callback = callback;
	function_2b8bd6(screen, count, string_ids);
}

class c_screen_45c388 : public c_class_1473c9
{
public:
	c_screen_45c388(long a, long b, word user_flags);

	virtual bool v10(s_widget_event *event);
	virtual void v18(void *parameters);
	virtual screen_load_proc get_load_proc();

	byte unknown610[4];
	long value614;
	c_class_1a2c81 *bitmaps[4];
};

byte function_6c850(byte button);

/* the passcode screen: each of four button presses shows a bitmap and adds
   the button to the code; the fourth signs the player slot in when the code
   is its user's passcode */
// @retail 0x2ba531
bool c_screen_45c388::v10(s_widget_event *event)
{
	switch (event->param)
	{
	case 1:
	case 13:
		start_animation(3);
		break;
	case 2:
	case 3:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
		if ((dword)value614 < 4)
		{
			byte buttons[18] = { 0xff, 0xff, 2, 3, 0xff, 0xff, 6, 7, 8, 9, 10, 11, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
			byte button = buttons[event->param];

			if (button != 0xff)
			{
				if (bitmaps[value614])
				{
					function_2b0a14((s_widget_view_2b0a *)bitmaps[value614], 1);
				}
				unknown610[value614] = function_6c850(button);
				value614++;
				if (value614 == 4)
				{
					long player = event->controller_index;
					s_player_slot_profile *profile = player_slot_profile_get(player);

					if (*(dword *)profile->user.passcode == *(dword *)unknown610)
					{
						profile->sign_in(0);
						function_14800c(v20(), v21());
					}
					else
					{
						dialog_ok_show(1, 0x3c, 4, 1 << player, 0, 0);
						function_14800c(v20(), v21());
					}
				}
			}
		}
		break;
	}
	return c_class_1473c9::v10(event);
}

/* builds the screen and finds its four bitmaps */
// @retail 0x2ba4c0
void c_screen_45c388::v18(void *parameters)
{
	volatile long definition_index = function_1480ff(screen_id);
	s_screen_layout layout =
	{
		0,
		1,
		{
			{ 0, 0, 0, 0 }
		}
	};
	word i;

	build(&layout);
	for (i = 0; i < 4; i++)
	{
		bitmaps[i] = find_child(8, i + 4, false);
	}
	c_class_1a2c81::v1();
}

// @retail 0x2ba497
c_screen_45c388::c_screen_45c388(long a, long b, word user_flags) :
	c_class_1473c9(0x15, a, b, user_flags)
{
	value614 = 0;
}

// @retail 0x2ba45b
c_class_1473c9 *__stdcall function_2ba45b(s_screen_parameters *parameters)
{
	c_screen_45c388 *screen = new c_screen_45c388(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2ba455
screen_load_proc c_screen_45c388::get_load_proc()
{
	return function_2ba45b;
}

void function_148a8d();
void function_148c21();

/* the screen transition state (cleared by 0x148bff and 0x148c21; defined by
   unknown_249fa3.cpp) */

/* "actions list" (vtable 0x45c468): copy, edit or delete the player profile
   (mode 0) or the game variant (mode 1) being edited */
class c_actions_list : public c_class_1474e8
{
public:
	c_actions_list(word user_flags);

	virtual void *get_item_data();
	/* folded with c_squad_privacy_setting_list's */
	virtual long get_item_count() { return 3; }
	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);

	long mode;
	c_class_14750b items[3];
	c_list_item_handler handler;
};

/* the actions screen (vtable 0x45c3f8) */
class c_actions_screen : public c_screen_with_menu
{
public:
	c_actions_screen(long a, long b, word user_flags);

	virtual bool v10(s_widget_event *event);
	/* the texts say what the actions act on */
	virtual void v19();
	virtual screen_load_proc get_load_proc();

	long mode;
	c_actions_list list;
};

// @retail 0x2baa9b
c_class_1473c9 *__stdcall function_2baa9b(s_screen_parameters *parameters)
{
	c_actions_screen *screen = new c_actions_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->mode = 0;
	screen->list.mode = 0;
	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2baae7
c_class_1473c9 *__stdcall function_2baae7(s_screen_parameters *parameters)
{
	c_actions_screen *screen = new c_actions_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->mode = 1;
	screen->list.mode = 1;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2baba2
void c_actions_screen::v19()
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)find_child(6, 2, false);

	if (text)
	{
		long string_handle;
		long title_id = 0x6000233;

		switch (mode)
		{
		case 0:
			string_handle = 0x1e000199;
			title_id = 0xe00019d;
			break;
		case 1:
			string_handle = 0x2300019a;
			title_id = 0xe00019c;
			break;
		default:
			string_handle = NONE;
			break;
		}
		text->function_253b1a(string_handle);
		title.function_253b1a(title_id);
	}
	c_class_1473c9::v19();
}

// @retail 0x2bab33
c_actions_screen::c_actions_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0xc2, a, b, user_flags, &list),
	mode(0),
	list(user_flags)
{
}

// @retail 0x2bab6f deleting c_actions_screen
// @retail 0x2bab8d destructor c_actions_screen

// @retail 0x2bac18
bool c_actions_screen::v10(s_widget_event *event)
{
	if (event->type == 5)
	{
		switch (event->param)
		{
		case 1:
		case 13:
			switch (mode)
			{
			case 0:
				function_148c21();
				break;
			case 1:
				function_148a8d();
				break;
			default:
				__assume(0);
			}
			break;
		}
	}
	return c_class_1473c9::v10(event);
}

// @retail 0x2bac02
screen_load_proc c_actions_screen::get_load_proc()
{
	screen_load_proc result;

	switch (mode)
	{
	case 0:
		result = function_2baa9b;
		break;
	case 1:
		result = function_2baae7;
		break;
	default:
		__assume(0);
	}
	return result;
}

/* the difficulty screen (vtable 0x45c588; its deleting destructor is folded
   with c_handicap_settings_screen's) */
class c_difficulty_screen : public c_screen_with_menu
{
public:
	c_difficulty_screen(long a, long b, word user_flags);

	virtual screen_load_proc get_load_proc();

	c_difficulty_list list;
};

// @retail 0x2bb3ed
c_class_1473c9 *__stdcall function_2bb3ed(s_screen_parameters *parameters)
{
	c_difficulty_screen *screen = new c_difficulty_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->list.alternate = false;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2bb432
c_class_1473c9 *__stdcall function_2bb432(s_screen_parameters *parameters)
{
	c_difficulty_screen *screen = new c_difficulty_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->list.alternate = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2bb477
c_difficulty_screen::c_difficulty_screen(long a, long b, word user_flags) :
	c_screen_with_menu(0xc, a, b, user_flags, &list),
	list(user_flags)
{
}

// @retail 0x2bb4a9
screen_load_proc c_difficulty_screen::get_load_proc()
{
	return list.alternate ? function_2bb432 : function_2bb3ed;
}

/* the custom game maps screen (vtable 0x45c650): a custom game's maps, or a
   coop game's (alternate) */
class c_custom_game_maps_screen : public c_screen_with_menu
{
public:
	c_custom_game_maps_screen(long a, long b, word user_flags, long screen_id, bool alternate);

	virtual void v3();
	virtual void v19();
	virtual screen_load_proc get_load_proc();

	c_custom_game_maps_list list;
	bool alternate;
};

// @retail 0x2bb2db
c_class_1473c9 *__stdcall function_2bb2db(s_screen_parameters *parameters)
{
	c_custom_game_maps_screen *screen = new c_custom_game_maps_screen(parameters->a, parameters->b, parameters->user_flags, 0xce, false);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2bb31d
c_class_1473c9 *__stdcall function_2bb31d(s_screen_parameters *parameters)
{
	c_custom_game_maps_screen *screen = new c_custom_game_maps_screen(parameters->a, parameters->b, parameters->user_flags, 0x11, true);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2bb29f
c_custom_game_maps_screen::c_custom_game_maps_screen(long a, long b, word user_flags, long screen_id, bool alternate) :
	c_screen_with_menu(screen_id, a, b, user_flags, &list),
	list(user_flags)
{
	this->alternate = alternate;
}

// @retail 0x2bb384 deleting c_custom_game_maps_screen
// @retail 0x2bb3d8 destructor c_custom_game_maps_screen

// @retail 0x2bb373
void c_custom_game_maps_screen::v19()
{
	list.coop = alternate;
	c_class_1473c9::v19();
}

long function_11cae0(void);
struct s_entry_c;
s_entry_c *function_19c5f0(long key);

/* a map list item's datum */
struct s_map_item_2c9
{
	short salt;
	bool downloaded;
	byte unknown03;
	long map_id;
};

/* a map's definition: its bitmap and its description in each language */
struct s_map_definition_2c9
{
	long map_id;
	byte unknown04[4];
	long bitmap_tag_index;
	byte unknown0c[0x24c - 0xc];
	wchar_t descriptions[8][0x80];
};

/* shows the focused map's bitmap and description */
// @retail 0x2c9b7c
void c_custom_game_maps_screen::v3()
{
	c_class_1a2c81 *bitmap = find_child(8, 1, false);
	c_class_1a2c81 *text = find_child(6, 2, false);
	s_record_pool *data = list.data;
	s_map_item_2c9 *datum = (s_map_item_2c9 *)record_pool_lookup(data, list.get_focused_datum());

	if (datum)
	{
		s_map_definition_2c9 *map = (s_map_definition_2c9 *)function_19c5f0(datum->map_id);

		if (map)
		{
			if (bitmap)
			{
				function_2b0a7b((s_widget_view_2b0a *)bitmap, function_137550(map->bitmap_tag_index, 0));
			}
			if (text)
			{
				wchar_t *description = map->descriptions[function_11cae0()];

				text->function_22f52e()->set_text((word *)description);
			}
		}
	}
	c_class_1a2c81::v3();
}

// @retail 0x2bb35f
screen_load_proc c_custom_game_maps_screen::get_load_proc()
{
	return alternate ? function_2bb31d : function_2bb2db;
}

/* ---- lists ---- */

// @retail 0x2b160c
long c_campaign_options_list::get_item_count()
{
	return data->actual_count;
}

/* the 26-slot list at 0x45af88; its item count (6) is shared by the lists at
   0x45b370, 0x45bb78, 0x45bdb8, 0x45bea0, 0x45d020 and 0x45d240 */
class c_list_45af88 : public c_list_24a64d
{
public:
	c_list_45af88(word user_flags);
	virtual long get_item_count();
	virtual void *get_items(long *count);

	c_class_14750b entries[6];
	s_profile_list_entry items[6];
};

// @retail 0x2b198a
long c_list_45af88::get_item_count()
{
	return 6;
}

// @retail 0x2b198e
void *c_list_45af88::get_items(long *count)
{
	*count = 6;
	return items;
}

// @retail 0x2b27f9
void c_list_item_handler::invoke(s_controller_reference **controller, long *item)
{
	(owner->*method)(controller, item);
}

// @retail 0x2b1cca
c_class_232671::c_class_232671(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_class_232671::handle_item)
{
	extended = false;
	data = user_interface_data_new("settings list", 2, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
}

/* the item handler the list's constructor (0x2b1cca) registers */
// @retail 0x2b1da9
void c_class_232671::handle_item(s_controller_reference **controller, long *item)
{
	s_record_pool *items = data;

	if (record_pool_lookup(items, *item))
	{
		s_screen_parameters parameters;

		parameters.field_c = 0;
		switch (*(short *)item)
		{
		case 0:
			function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 5, 4, (long)function_2b19dc);
			parameters.load(&parameters);
			break;
		case 1:
			function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 5, 4, (long)function_2b1b01);
			parameters.load(&parameters);
			break;
		}
	}
}

// @retail 0x2b1d60
long c_class_232671::get_item_count()
{
	return extended ? 3 : 2;
}

/* a playlist the list knows: its saved game file type and index */
struct s_playlist_entry
{
	s_playlist_entry();

	byte type;
	byte unknown01[3];
	long index;
	/* the playlist itself: its name follows a long */
	long unknown08;
	word name[(0x2d9c - 0x0c) / 2];
};

/* an item of the list: a playlist, or a game variant (variant), or the
   item that makes a new one (create) */
struct s_playlist_item
{
	dword salt : 16;
	dword variant : 1;
	dword create : 1;
	dword unused : 14;
	long index;
};

/* "playlist saved game file list" (vtable 0x45b138) */
class c_playlist_saved_game_file_list : public c_class_1474e8
{
public:
	c_playlist_saved_game_file_list(word user_flags);

	/* fills the list and focuses the last chosen playlist */
	virtual void v1();
	virtual void v3();
	virtual void v20(c_class_1a2c81 *widget, long index);

	void handle_item(s_controller_reference **controller, long *item);
	void fill();
	long *find_playlist(byte type, long index);

	c_class_14750b items[16];
	s_playlist_entry playlists[16];
	bool value2e248;
	bool value2e249;
	bool value2e24a;
	long value2e24c;
	c_list_item_handler handler;
};

/* the playlist listing screen (vtable 0x458c10) */
class c_playlist_listing_screen : public c_screen_with_menu
{
public:
	c_playlist_listing_screen(long a, long b, word user_flags, long mode);

	virtual void v3();
	virtual screen_load_proc get_load_proc();

	c_playlist_saved_game_file_list list;
	byte unknown2e87c[0x2e97c - 0x2e87c];
	long mode;
};

// @retail 0x2b1ece
s_playlist_entry::s_playlist_entry()
{
	index = NONE;
}

// @retail 0x2b1e08
c_playlist_saved_game_file_list::c_playlist_saved_game_file_list(word user_flags) :
	c_class_1474e8(user_flags),
	value2e248(false),
	value2e249(false),
	value2e24a(false),
	value2e24c(NONE),
	handler(this, (list_item_method)&c_playlist_saved_game_file_list::handle_item)
{
	data = user_interface_data_new("playlist saved game file list", 0x1001, 8);
	function_16b790(data);
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2b1eb0 deleting c_playlist_saved_game_file_list
// @retail 0x230e4d destructor c_playlist_saved_game_file_list

/* the list item as its flags word: bit 0 is the variant flag, bit 1 the
   create flag */
struct s_playlist_item_view
{
	word salt;
	short flags;
	long index;
};

/* the playlist index the list focuses when it is built */
long g_510990;

void playlist_list_focus_playlist(c_playlist_saved_game_file_list *list);
void record_pool_release_all(s_record_pool *data);
bool function_192db0(long index);
long function_1945c0(long index);
bool function_194610(long index);
struct s_game_variant_block;
bool game_variant_block_read(long index, s_game_variant_block *block);
void __stdcall function_215900(long controller_index, long type, word *count, long *files, bool a);

/* a player slot's value at +0x204, as fill reads it */
struct s_player_slot_2b1f
{
	byte unknown000[0x204];
	long value204;
	byte unknown208[0xc70 - 0x208];
};

/* the list's items: the item that makes a new playlist, the variants of the
   variants file that can be played, and the saved playlists */
// @retail 0x2b1ff4
void c_playlist_saved_game_file_list::fill()
{
	long i;

	record_pool_release_all(data);
	for (i = 0; i < 16; i++)
	{
		playlists[i].index = NONE;
	}
	if (value2e24a)
	{
		s_playlist_item_view *item = &((s_playlist_item_view *)data->data)[record_pool_allocate(data) & 0xffff];

		item->flags = (item->flags & ~1) | 2;
	}
	if (value2e248)
	{
		for (i = 0; i < 16; i++)
		{
			if (function_192db0(i) &&
				(function_194610(i) || ((s_player_slot_2b1f *)g_54e8e0)[get_controller_index()].value204 >= function_1945c0(i)))
			{
				byte block[0x15cb8];

				if (game_variant_block_read(i, (s_game_variant_block *)block))
				{
					long datum = record_pool_allocate(data);
					s_playlist_item_view *item;

					if (datum == NONE)
					{
						break;
					}
					item = &((s_playlist_item_view *)data->data)[datum & 0xffff];
					item->index = i;
					item->flags = (item->flags & ~2) | 1;
				}
			}
		}
	}
	if (value2e249)
	{
		long files[0x1000];
		long count = 0x1000;

		function_215900(get_controller_index(), 10, (word *)&count, files, 0);
		for (i = 0; i < (word)count; i++)
		{
			long datum = record_pool_allocate(data);
			s_playlist_item_view *item;

			if (datum == NONE)
			{
				break;
			}
			item = &((s_playlist_item_view *)data->data)[datum & 0xffff];
			item->flags &= ~3;
			item->index = files[i];
		}
	}
}

// @retail 0x2b1ed5
void c_playlist_saved_game_file_list::v1()
{
	fill();
	((c_widget *)this)->c_widget::v9();
	playlist_list_focus_playlist(this);
}
// @retail 0x2b1eef
void playlist_list_focus_playlist(c_playlist_saved_game_file_list *list)
{
    if (list->value2e24a || g_510990 == NONE) return;
    long index = g_510990;
    s_list_item_iterator iterator;
    iterator.iterator.index = NONE;
    iterator.iterator.datum_index = NONE;
    iterator.iterator.data = list->data;
    while (function_2b2327(&iterator))
    {
        s_playlist_item *item = (s_playlist_item *)iterator.item;
        if (!(bool)(((dword)((s_playlist_item_view *)item)->flags >> 1) & 1) && list->value2e248 && TEST_FIELD_BIT(item->variant) && item->index == index)
        {
            list->select_datum(iterator.iterator.datum_index);
            break;
        }
    }
}
// score 9: the body is retail's byte for byte with ECX and EDX exchanged —
// retail keeps the loop counter in ECX and the entry cursor in EDX, ours the
// reverse (the xor, lea, inc, add, cmp, jb, imul and both loads). Declaration
// order, a scoped counter, a do/while loop, commutative conditions, a named
// index local, an explicit cursor pointer, a reference to the entry and a
// direct return all measured 9 to 30 (the pointer and reference forms 30, 49
// bytes). Permuter: 178 tries (seed 999) and 173 (seed 7), 300 s each, no form
// below 9.
// @retail 0x2b221d
long *c_playlist_saved_game_file_list::find_playlist(byte type, long index)
{
	dword i;
	long *result = 0;

	for (i = 0; i < 16; i++)
	{
		if (playlists[i].type == type && playlists[i].index == index)
		{
			result = &playlists[i].unknown08;
			break;
		}
	}
	return result;
}

bool game_variant_get_name(long index, word *name);

/* shows the playlist's or the variant's name */
// @retail 0x2b1f5f
void c_playlist_saved_game_file_list::v20(c_class_1a2c81 *widget, long index)
{
	long datum = widget_item(widget)->value70;

	if (datum != NONE)
	{
		c_class_1a2c81 *text = widget->find_child(6, 0, false);

		if (text)
		{
			s_playlist_item *item = &((s_playlist_item *)data->data)[datum & 0xffff];

			if ((bool)(((dword)((s_playlist_item_view *)item)->flags >> 1) & 1))
			{
				((c_text_widget_45a5e0 *)text)->function_253b1a(0x130001a1);
			}
			else
			{
				word *name = (word *)L"";

				if (item->variant)
				{
					word local_dbe893[0x10];

					if (game_variant_get_name(item->index, local_dbe893))
					{
						name = local_dbe893;
					}
				}
				else
				{
					long *playlist = find_playlist(0, item->index);

					if (playlist)
					{
						name = (word *)(playlist + 1);
					}
				}
				text->function_22f52e()->set_text(name);
			}
		}
	}
}

bool function_592f0(void);
void function_199e2e(bool close);
bool __stdcall network_session_manager_host_session(long mode, const XNKID *kid, const XNKEY *key);
void function_199a57(void);
void function_199a03(long mode);
bool network_session_interface_set_value49a4(long value);
void function_148cfc(long value);
void function_19a0af(long value);
void function_1902fc(long index, long value);
void __stdcall function_148b27(long index);

/* the profile the dialog below releases when it is chosen */
long g_50493c;

// @retail 0x2b2310
bool __stdcall function_2b2310(long controller_index)
{
	function_148b27(g_50493c);
	g_50493c = NONE;
	return true;
}

/* starts a game of the chosen variant, hosting a session if there is none */
// @retail 0x2b2181
void function_2b2181(c_playlist_saved_game_file_list *list, long controller_index, long index)
{
	byte block[0x15cb8];

	if (game_variant_block_read(index, (s_game_variant_block *)block))
	{
		if (function_592f0() || (function_199e2e(true), network_session_manager_host_session(2, NULL, NULL)))
		{
			function_199a57();
			function_199a03(3);
			network_session_interface_set_value49a4(2);
			function_148cfc(index);
			function_19a0af(index);
			function_1902fc(controller_index, index);
			list->get_screen()->start_animation(3);
		}
	}
	else
	{
		dialog_choice_show(1, 0x7b, 4, 1 << controller_index, function_2b2310, 0, 0);
	}
}

// @retail 0x2b2149
void c_playlist_saved_game_file_list::handle_item(s_controller_reference **controller, long *item)
{
	if (*item != NONE)
	{
		s_playlist_item *datum = (s_playlist_item *)record_pool_lookup(data, *item);

		if (datum && datum->variant)
		{
			function_2b2181(this, (*controller)->controller_index, datum->index);
		}
	}
}

// @retail 0x230dce
c_playlist_listing_screen::c_playlist_listing_screen(long a, long b, word user_flags, long mode) :
	c_screen_with_menu(NONE, a, b, user_flags, &list),
	list(user_flags)
{
	this->mode = mode;
}

// @retail 0x230e2f deleting c_playlist_listing_screen
// @retail 0x230e83 destructor c_playlist_listing_screen

bool game_variant_get_description(long index, word *description);

/* shows the focused variant's name and description */
// @retail 0x2b225a
void c_playlist_listing_screen::v3()
{
	long datum = list.get_focused_datum();

	if (datum != NONE)
	{
		s_playlist_item *item = (s_playlist_item *)list.function_24c5f2(datum);

		if (item)
		{
			long candidate = item->index;
			bool valid = candidate != NONE;
			volatile long index = candidate;

			if (valid)
			{
				c_class_1a2c81 *name_text = find_child(6, 3, false);
				c_class_1a2c81 *description_text = find_child(6, 4, false);

				if (name_text)
				{
					word name[0x10];

					if (game_variant_get_name(index, name))
					{
						name_text->function_22f52e()->set_text(name);
					}
				}
				if (description_text)
				{
					word description[0x80];

					if (game_variant_get_description(index, description))
					{
						description_text->function_22f52e()->set_text(description);
					}
				}
			}
		}
	}
	c_class_1a2c81::v3();
}

// @retail 0x230c8d
c_class_1473c9 *__stdcall function_230c8d(s_screen_parameters *parameters)
{
	long screen_id = parameters->a == 3 ? 0xed : 0xdb;
	c_playlist_listing_screen *screen = new c_playlist_listing_screen(parameters->a, parameters->b, parameters->user_flags, 0);

	screen->m6c = true;
	screen->list.value2e248 = true;
	screen->list.value2e249 = false;
	screen->list.value2e24a = false;
	screen->set_screen_id(screen_id);
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x230d08
c_class_1473c9 *__stdcall function_230d08(s_screen_parameters *parameters)
{
	c_playlist_listing_screen *screen = new c_playlist_listing_screen(parameters->a, parameters->b, parameters->user_flags, 1);

	screen->m6c = true;
	screen->list.value2e248 = false;
	screen->list.value2e249 = true;
	screen->list.value2e24a = true;
	screen->set_screen_id(0xb3);
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x230d6b
c_class_1473c9 *__stdcall function_230d6b(s_screen_parameters *parameters)
{
	c_playlist_listing_screen *screen = new c_playlist_listing_screen(parameters->a, parameters->b, parameters->user_flags, 2);

	screen->m6c = true;
	screen->list.value2e248 = false;
	screen->list.value2e249 = true;
	screen->list.value2e24a = false;
	screen->set_screen_id(0xdc);
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x230e09
screen_load_proc c_playlist_listing_screen::get_load_proc()
{
	screen_load_proc result;

	switch (mode)
	{
	case 0:
		result = function_230c8d;
		break;
	case 1:
		result = function_230d08;
		break;
	case 2:
		result = function_230d6b;
		break;
	default:
		result = 0;
		break;
	}
	return result;
}

// @retail 0x2b1f5a
void c_playlist_saved_game_file_list::v3()
{
	((c_widget *)this)->c_widget::v11();
}

// @retail 0x2b5575
void c_y_menu_player_selected_list::v1()
{
	((c_widget *)this)->c_widget::v9();
}

// @retail 0x2b5c16
void c_y_menu_player_selected_list::v20(c_class_1a2c81 *item, long unused)
{
	s_list_item_text table[20];

	table[0].item = 0;
	table[0].string_handle = 0xc0002a7;
	table[1].item = 1;
	table[1].string_handle = 0x150002b5;
	table[2].item = 2;
	table[2].string_handle = 0x160002b6;
	table[3].item = 3;
	table[3].string_handle = 0xc0002a6;
	table[4].item = 4;
	table[4].string_handle = 0xa0002a8;
	table[5].item = 5;
	table[5].string_handle = 0x90002a9;
	table[6].item = 6;
	table[6].string_handle = 0x1300042d;
	table[7].item = 7;
	table[7].string_handle = 0xd0002ac;
	table[8].item = 8;
	table[8].string_handle = 0x100002ae;
	table[9].item = 9;
	table[9].string_handle = 0x120002af;
	table[10].item = 10;
	table[10].string_handle = 0x130002b4;
	table[11].item = 11;
	table[11].string_handle = 0x150002b8;
	table[12].item = 12;
	table[12].string_handle = 0x110002ba;
	table[13].item = 13;
	table[13].string_handle = 0x160002c0;
	table[14].item = 14;
	table[14].string_handle = 0x40002c1;
	table[15].item = 15;
	table[15].string_handle = 0x60002c2;
	table[16].item = 16;
	table[16].string_handle = 0xf0002c3;
	table[17].item = 17;
	table[17].string_handle = 0x180002b9;
	table[18].item = 18;
	table[18].string_handle = 0x100002bf;
	table[19].item = 19;
	table[19].string_handle = 0x130002be;
	function_24c75c(this, item, table, 0, 20);
}

bool function_6c7e0();
bool function_19a148(long privacy);

// @retail 0x2b51ee
c_squad_privacy_setting_list::c_squad_privacy_setting_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_squad_privacy_setting_list::handle_item)
{
	data = user_interface_data_new("squad privacy setting list", 3, 4);
	function_16b790(data);
	list_item_add(this, 0);
	if (function_6c7e0())
	{
		list_item_add(this, 1);
	}
	list_item_add(this, 2);
	delegate_register(&item_handlers, &handler);
}

long function_19a161(void);

/* focuses the item of the squad's privacy setting */
// @retail 0x2b5361
void function_2b5361(c_squad_privacy_setting_list *list)
{
	long privacy = function_19a161();
	s_list_item_iterator iterator;

	iterator.iterator.index = NONE;
	iterator.iterator.datum_index = NONE;
	iterator.iterator.data = list->data;
	while (function_2b2327(&iterator))
	{
		if (((s_list_item_datum *)iterator.item)->item == privacy)
		{
			list->select_datum(iterator.iterator.datum_index);
			break;
		}
	}
}

// @retail 0x2b52d5
void c_squad_privacy_setting_list::v1()
{
	((c_widget *)this)->c_widget::v9();
	function_2b5361(this);
}

/* opens the screen that edits the chosen setting of the controller's
   profile */
// @retail 0x2b50d9
void c_mp_player_settings_game_list::handle_item(s_controller_reference **controller, long *item)
{
	if (*item != NONE)
	{
		s_list_item_datum *datum = &((s_list_item_datum *)data->data)[*item & 0xffff];
		s_screen_parameters parameters;

		long window;
		long channel;

		parameters.field_c = 0;
		window = v12();
		channel = v11();
		function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, channel, window, 0);
		switch (datum->item)
		{
		case 0:
			parameters.load = function_231db5;
			break;
		case 1:
			parameters.load = function_2b4f17;
			break;
		case 2:
			parameters.load = function_2b4ed2;
			break;
		case 3:
			parameters.load = function_2b2801;
			break;
		}
		if (parameters.load)
		{
			s_player_profile profile;
			long profile_index;

			if (g_54e5d0.profile_index != NONE)
			{
				profile_edit_end();
			}
			player_slot_get_profile(get_controller_index(), &profile, &profile_index);
			profile_edit_begin(get_controller_index(), (s_player_profile_settings *)&profile, profile_index);
			parameters.load(&parameters);
		}
	}
}

// @retail 0x2b5324
void c_squad_privacy_setting_list::handle_item(s_controller_reference **controller, long *item)
{
	s_list_item_datum *datum = (s_list_item_datum *)record_pool_lookup(data, *item);

	if (datum)
	{
		function_19a148(datum->item);
		function_14800c(v11(), v12());
	}
}

// @retail 0x2b51ea
long c_squad_privacy_setting_list::get_item_count()
{
	return 3;
}

// @retail 0x2ba65f
void *c_actions_list::get_item_data()
{
	return items;
}

// @retail 0x2ba6ae
c_actions_list::c_actions_list(word user_flags) :
	c_class_1474e8(user_flags),
	mode(0),
	handler(this, (list_item_method)&c_actions_list::handle_item)
{
	data = user_interface_data_new("actions list", 3, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2ba744 deleting c_actions_list
// @retail 0x2ba762 destructor c_actions_list

// @retail 0x2ba798
void c_actions_list::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);

	if (text)
	{
		long string_handle;

		switch ((short)widget_item(widget)->value70)
		{
		case 0:
			string_handle = 0x400019e;
			break;
		case 1:
			string_handle = 0x600019f;
			break;
		case 2:
			string_handle = 0x60001a0;
			break;
		default:
			string_handle = NONE;
			break;
		}
		text->function_253b1a(string_handle);
	}
}

/* whether a saved game file is one of the built-in ones (bit 21 of its
   index) */
static inline bool saved_game_file_is_builtin(long file_index)
{
	return (bool)(((dword)file_index >> 21) & 1);
}

void function_236299(long sound);
void function_148a2c();
void function_148afb();
bool player_slot_profile_in_use(long profile_index);
bool saved_game_storage_has_free_blocks(long blocks);
long saved_game_file_type_size_in_blocks(long type);
long saved_game_file_type_from_variant(s_game_variant *variant);
void function_238c21(long controller, long type, word *name, long maximum_count);
void function_238c69(long mode, long type, word *name, long maximum_count, long controller);
c_class_1473c9 *__stdcall function_23764f(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_2ba666(s_screen_parameters *parameters);
bool __stdcall function_2523b7(long controller);
bool __stdcall dialog_callback_dashboard_memory_for_profile(long controller);
bool __stdcall dialog_callback_dashboard_memory_for_game_variant(long controller);

// @retail 0x2ba6a4
bool __stdcall function_2ba6a4(long controller)
{
	function_148a2c();
	return true;
}

// @retail 0x2ba949
bool __stdcall function_2ba949(long controller)
{
	function_148afb();
	return true;
}

/* edits the player profile */
// @retail 0x2ba8a0
void function_2ba8a0(long controller)
{
	if (saved_game_file_is_builtin(g_54e5d0.profile_index))
	{
		function_236299(2);
	}
	else
	{
		s_screen_parameters parameters;

		parameters.field_c = 0;
		function_149f49((s_message *)&parameters, 0, 0, 1 << controller, 5, 4, (long)function_2ba666);
		parameters.load(&parameters);
	}
}

/* copies the player profile */
// @retail 0x2ba8e9
void function_2ba8e9(long controller)
{
	long profile_index = g_54e5d0.profile_index;
	long blocks = saved_game_file_type_size_in_blocks(0);

	if (saved_game_file_is_builtin(profile_index))
	{
		function_236299(2);
	}
	else if (saved_game_storage_has_free_blocks(blocks))
	{
		function_238c21(controller, 4, g_54e5d0.settings.name, 0x20);
	}
	else
	{
		dialog_choice_show(3, 0x4e, 4, 1 << controller, dialog_callback_dashboard_memory_for_profile, function_2523b7, 0);
	}
}

/* deletes the player profile */
// @retail 0x2ba953
void function_2ba953(long controller)
{
	long profile_index = g_54e5d0.profile_index;

	if (saved_game_file_is_builtin(profile_index))
	{
		dialog_ok_show(1, 0x53, 4, 1 << controller, 0, 0);
	}
	else if (player_slot_profile_in_use(profile_index))
	{
		dialog_ok_show(1, 0x57, 4, 1 << controller, 0, 0);
	}
	else
	{
		dialog_choice_show(1, 0x70, 4, 1 << controller, function_2ba949, 0, 0);
	}
}

/* edits the game variant */
// @retail 0x2ba9ab
void function_2ba9ab(long controller)
{
	if (saved_game_file_is_builtin(g_54e49c))
	{
		function_236299(2);
	}
	else
	{
		s_screen_parameters parameters;

		parameters.field_c = 0;
		function_149f49((s_message *)&parameters, 0, 0, 1 << controller, 5, 4, (long)function_23764f);
		parameters.load(&parameters);
	}
}

/* copies the game variant */
// @retail 0x2ba9f4
void function_2ba9f4(long controller)
{
	long blocks = saved_game_file_type_size_in_blocks(1);

	if (saved_game_file_is_builtin(g_54e49c))
	{
		function_236299(2);
	}
	else if (saved_game_storage_has_free_blocks(blocks))
	{
		function_238c69(6, saved_game_file_type_from_variant(&g_54e4a0), (word *)g_54e4a0.name, 0x20, controller);
	}
	else
	{
		dialog_choice_show(3, 0x4f, 4, 1 << controller, dialog_callback_dashboard_memory_for_game_variant, function_2523b7, 0);
	}
}

/* deletes the game variant */
// @retail 0x2baa60
void function_2baa60(c_actions_list *list, long controller)
{
	if (saved_game_file_is_builtin(g_54e49c))
	{
		function_236299(2);
	}
	else
	{
		dialog_choice_show_default(3, list->v12(), 1 << controller, function_2ba6a4, 0xb);
	}
}

// @retail 0x2ba7e3
void c_actions_list::handle_item(s_controller_reference **controller, long *item)
{
	if (*item != NONE)
	{
		switch ((short)*item)
		{
		case 0:
			switch (mode)
			{
			case 0:
				function_2ba8a0((*controller)->controller_index);
				break;
			case 1:
				function_2ba9ab((*controller)->controller_index);
				break;
			}
			break;
		case 1:
			switch (mode)
			{
			case 0:
				function_2ba8e9((*controller)->controller_index);
				break;
			case 1:
				function_2ba9f4((*controller)->controller_index);
				break;
			}
			break;
		case 2:
			switch (mode)
			{
			case 0:
				function_2ba953((*controller)->controller_index);
				break;
			case 1:
				function_2baa60(this, (*controller)->controller_index);
				break;
			}
			break;
		}
	}
	get_screen()->start_animation(3);
}

// @retail 0x2b48c2
c_mp_controller_settings_game_list::c_mp_controller_settings_game_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_mp_controller_settings_game_list::handle_item)
{
	data = user_interface_data_new("mp controller settings game list", 6, 4);
	function_16b790(data);
	list_item_add(this, 0);
	list_item_add(this, 1);
	list_item_add(this, 2);
	list_item_add(this, 3);
	list_item_add(this, 4);
	list_item_add(this, 5);
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2b49e6
void c_mp_controller_settings_game_list::v20(c_class_1a2c81 *item, long unused)
{
	s_list_item_text table[6];

	table[0].item = 0;
	table[0].string_handle = 0x130003dd;
	table[1].item = 1;
	table[1].string_handle = 0xf0003de;
	table[2].item = 2;
	table[2].string_handle = 0x100003df;
	table[3].item = 3;
	table[3].string_handle = 0xb0003e0;
	table[4].item = 4;
	table[4].string_handle = 0xb0003e1;
	table[5].item = 5;
	table[5].string_handle = 0x90003e2;
	function_24c75c(this, item, table, 0, 6);
}

// @retail 0x2b4b43
c_handicap_settings_edit_list::c_handicap_settings_edit_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_handicap_settings_edit_list::handle_item)
{
	data = user_interface_data_new("handicap settings edit list", 4, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2b4bf0
void c_handicap_settings_edit_list::v20(c_class_1a2c81 *item, long unused)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)item->find_child(6, 0, false);

	if (text)
	{
		long string_handle;

		switch ((short)((c_class_14750b *)item)->value70)
		{
		case 0:
			string_handle = 0xa0003d9;
			break;
		case 1:
			string_handle = 0xa0003da;
			break;
		case 2:
			string_handle = 0xa0003db;
			break;
		case 3:
			string_handle = 0xa0003dc;
			break;
		default:
			string_handle = NONE;
			break;
		}
		text->function_253b1a(string_handle);
	}
}

/* a player slot's handicap (+0x200) */
struct s_player_slot_handicap_view
{
	byte unknown000[0x200];
	long handicap;
	byte unknown204[0xc70 - 0x204];
};

void function_18fe9e(long index);

/* sets the controller's player's handicap and leaves the screen */
// @retail 0x2b4c45
void c_handicap_settings_edit_list::handle_item(s_controller_reference **controller, long *item)
{
	switch ((short)*item)
	{
	case 0:
		((s_player_slot_handicap_view *)g_54e8e0)[(*controller)->controller_index].handicap = 0;
		break;
	case 1:
		((s_player_slot_handicap_view *)g_54e8e0)[(*controller)->controller_index].handicap = 1;
		break;
	case 2:
		((s_player_slot_handicap_view *)g_54e8e0)[(*controller)->controller_index].handicap = 2;
		break;
	case 3:
		((s_player_slot_handicap_view *)g_54e8e0)[(*controller)->controller_index].handicap = 3;
		break;
	}
	function_18fe9e((*controller)->controller_index);
	function_14800c(v11(), v12());
}

/* opens the screen that edits the chosen setting of the controller's
   profile */
// @retail 0x2b4a4d
void c_mp_controller_settings_game_list::handle_item(s_controller_reference **controller, long *item)
{
	if (*item != NONE)
	{
		s_list_item_datum *datum = &((s_list_item_datum *)data->data)[*item & 0xffff];
		s_screen_parameters parameters;

		long window;
		long channel;

		parameters.field_c = 0;
		window = v12();
		channel = v11();
		function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, channel, window, 0);
		switch (datum->item)
		{
		case 0:
			parameters.load = function_2b42b6;
			break;
		case 1:
			parameters.load = function_2b45a7;
			break;
		case 2:
			parameters.load = function_2b43d5;
			break;
		case 3:
			parameters.load = function_2b44c3;
			break;
		case 4:
			parameters.load = function_2b46e4;
			break;
		case 5:
			parameters.load = function_2b47e5;
			break;
		}
		if (parameters.load)
		{
			s_player_profile profile;
			long profile_index;

			if (g_54e5d0.profile_index != NONE)
			{
				profile_edit_end();
			}
			player_slot_get_profile(get_controller_index(), &profile, &profile_index);
			profile_edit_begin(get_controller_index(), (s_player_profile_settings *)&profile, profile_index);
			parameters.load(&parameters);
		}
	}
}

// @retail 0x2b49c8 deleting c_mp_controller_settings_game_list
// @retail 0x2324e3 destructor c_mp_controller_settings_game_list

// @retail 0x2b74c3
c_variant_editing_options_list::c_variant_editing_options_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_variant_editing_options_list::handle_item)
{
	data = user_interface_data_new("variant editing options list", 6, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		((s_list_item_datum *)data->data)[record_pool_allocate(data) & 0xffff].item = (short)i;
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2b7581
void c_variant_editing_options_list::v20(c_class_1a2c81 *item, long unused)
{
	s_list_item_text table[6];

	table[0].item = 0;
	table[0].string_handle = 0xd000434;
	table[1].item = 1;
	table[1].string_handle = 0xe000430;
	table[2].item = 2;
	table[2].string_handle = 0xc000435;
	table[3].item = 3;
	table[3].string_handle = 0x1100042e;
	table[4].item = 4;
	table[4].string_handle = 0xf000433;
	table[5].item = 5;
	table[5].string_handle = 0x11000436;
	function_24c75c(this, item, table, 0, 6);
}

void __stdcall function_2c83e6(long type, long a, long b, word user_flags);

/* opens the editor of the chosen category of the variant's settings; the
   first three depend on the variant's game engine */
// @retail 0x2b75e8
void c_variant_editing_options_list::handle_item(s_controller_reference **controller, long *item)
{
	s_list_item_datum *datum = (s_list_item_datum *)record_pool_lookup(data, *item);

	if (datum)
	{
		long type = NONE;

		switch (datum->item)
		{
		case 0:
			switch (g_54e4a0.field_xcb8724)
			{
			case 1:
				type = 0;
				break;
			case 2:
				type = 1;
				break;
			case 3:
				type = 2;
				break;
			case 4:
				type = 3;
				break;
			case 7:
				type = 6;
				break;
			case 8:
				type = 7;
				break;
			case 9:
				type = 8;
				break;
			default:
				break;
			}
			break;
		case 1:
			type = 9;
			break;
		case 2:
			switch (g_54e4a0.field_xcb8724)
			{
			case 1:
				type = 0x1f;
				break;
			case 2:
				type = 0x20;
				break;
			case 3:
				type = 0x21;
				break;
			case 4:
				type = 0x22;
				break;
			case 7:
				type = 0x25;
				break;
			case 8:
				type = 0x26;
				break;
			case 9:
				type = 0x27;
				break;
			default:
				break;
			}
			break;
		case 3:
			switch (g_54e4a0.field_xcb8724)
			{
			case 1:
				type = 0xd;
				break;
			case 2:
				type = 0xe;
				break;
			case 3:
				type = 0xf;
				break;
			case 4:
				type = 0x10;
				break;
			case 7:
				type = 0x13;
				break;
			case 8:
				type = 0x14;
				break;
			case 9:
				type = 0x15;
				break;
			default:
				break;
			}
			break;
		case 4:
			type = 0xb;
			break;
		case 5:
			type = 0xc;
			break;
		default:
			break;
		}
		if (type != NONE)
			function_2c83e6(type, 5, 4, 1 << (*controller)->controller_index);
	}
}

// @retail 0x2b7563 deleting c_variant_editing_options_list

// @retail 0x2b7900
c_player_profile_edit_list::c_player_profile_edit_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_player_profile_edit_list::handle_item)
{
	data = user_interface_data_new("player profile edit list", 6, 4);
	function_16b790(data);
	for (long i = 0; i < data->maximum_count; i++)
	{
		record_pool_allocate(data);
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2b7990
void c_player_profile_edit_list::v20(c_class_1a2c81 *item, long unused)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)item->find_child(6, 0, false);

	if (text)
	{
		long string_handle;

		switch ((short)((c_class_14750b *)item)->value70)
		{
		case 0:
			string_handle = 0x130002ef;
			break;
		case 1:
			string_handle = 0x130002f0;
			break;
		case 2:
			string_handle = 0xc000302;
			break;
		case 3:
			string_handle = 0xd000301;
			break;
		case 4:
			string_handle = 0x120002f2;
			break;
		case 5:
			string_handle = 0x120002f1;
			break;
		default:
			string_handle = NONE;
			break;
		}
		text->function_253b1a(string_handle);
	}
}

c_class_1473c9 *window_manager_open_channel_5_screen(short user_flags, long load);

/* opens the chosen part of the profile editor */
// @retail 0x2b79f9
void c_player_profile_edit_list::handle_item(s_controller_reference **controller, long *item)
{
	if (record_pool_lookup(data, *item))
	{
		s_screen_parameters parameters;
		long window;

		switch ((short)*item)
		{
		case 0:
			window_manager_open_channel_5_screen(1 << (*controller)->controller_index, (long)function_2b7714);
			break;
		case 1:
			window_manager_open_channel_5_screen(1 << (*controller)->controller_index, (long)function_2b77a8);
			break;
		case 2:
			parameters.field_c = 0;
			window = v12();
			function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, window, (long)function_2b2917);
			parameters.load(&parameters);
			break;
		case 3:
			parameters.field_c = 0;
			window = v12();
			function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, window, (long)function_2b288c);
			parameters.load(&parameters);
			break;
		case 4:
			parameters.field_c = 0;
			function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)function_2b2801);
			parameters.load(&parameters);
			break;
		case 5:
			parameters.field_c = 0;
			window = v12();
			function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, window, (long)function_2b7887);
			parameters.load(&parameters);
			break;
		}
	}
}

bool function_19a935(void);

// @retail 0x2b298d
c_friends_options_list::c_friends_options_list(word user_flags) :
	c_class_1474e8(user_flags),
	name(0),
	entries(0),
	entry_count(0),
	source(0),
	handler(this, (list_item_method)&c_friends_options_list::handle_item)
{
	bool online = function_19a935();

	data = user_interface_data_new("friends options list", 6, 4);
	function_16b790(data);
	list_item_add(this, 0);
	list_item_add(this, 1);
	if (online)
	{
		list_item_add(this, 2);
	}
	list_item_add(this, 3);
	list_item_add(this, 4);
	list_item_add(this, 5);
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2b2ad9
void c_friends_options_list::v20(c_class_1a2c81 *item, long unused)
{
	s_list_item_text table[6];

	table[0].item = 0;
	table[0].string_handle = 0x1600029e;
	table[1].item = 1;
	table[1].string_handle = 0x180002a0;
	table[2].item = 2;
	table[2].string_handle = 0x170002a1;
	table[3].item = 3;
	table[3].string_handle = 0xc000302;
	table[4].item = 4;
	table[4].string_handle = 0xd000301;
	table[5].item = 5;
	table[5].string_handle = 0x120002f2;
	function_24c75c(this, item, table, 0, 6);
}

// @retail 0x2b2abb deleting c_friends_options_list

bool function_1900a5(long index);
bool function_1906da(long index);

// @retail 0x2b4f5c
c_mp_player_settings_game_list::c_mp_player_settings_game_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_mp_player_settings_game_list::handle_item)
{
	data = user_interface_data_new("mp player settings game list", 4, 4);
	function_16b790(data);
	list_item_add(this, 0);
	if (!function_1900a5(get_controller_index()))
	{
		if (function_1906da(get_controller_index()))
		{
			list_item_add(this, 1);
			list_item_add(this, 2);
		}
		if (function_6c7e0())
		{
			list_item_add(this, 3);
		}
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2b508c
void c_mp_player_settings_game_list::v20(c_class_1a2c81 *item, long unused)
{
	s_list_item_text table[4];

	table[0].item = 0;
	table[0].string_handle = 0x130002ef;
	table[1].item = 1;
	table[1].string_handle = 0xc000302;
	table[2].item = 2;
	table[2].string_handle = 0xd000301;
	table[3].item = 3;
	table[3].string_handle = 0x120002f2;
	function_24c75c(this, item, table, 0, 4);
}

void function_190728(long index);

/* a player's clan membership as function_18ffc3 fills it in */
struct s_clan_membership
{
	byte unknown00[0x1c];
	long rank;
	byte unknown20[0x94 - 0x20];
};

// @retail 0x2b233d
c_clan_options_list::c_clan_options_list(word user_flags) :
	c_class_1474e8(user_flags),
	name(0),
	entries(0),
	entry_count(0),
	source(0),
	handler(this, (list_item_method)&c_clan_options_list::handle_item)
{
	long controller = get_controller_index();
	bool online = function_19a935();
	s_clan_membership membership;

	data = user_interface_data_new("clan options list", 4, 4);
	function_16b790(data);
	if (function_18ffc3(controller, (s_player_slot_blockb82 *)&membership))
	{
		bool member = membership.rank >= 1;
		bool ranked = membership.rank >= 1;

		bool online_member = online && membership.rank >= 1;
		if (member)
		{
			list_item_add(this, 0);
		}
		if (ranked)
		{
			list_item_add(this, 1);
		}
		if (online_member)
		{
			list_item_add(this, 2);
		}
	}
	list_item_add(this, 3);
	delegate_register(&item_handlers, &handler);
	function_190728(get_controller_index());
}

// @retail 0x2b24b2
void c_clan_options_list::v20(c_class_1a2c81 *item, long unused)
{
	s_list_item_text table[4];

	table[0].item = 0;
	table[0].string_handle = 0x180002a2;
	table[1].item = 1;
	table[1].string_handle = 0x1a0002a3;
	table[2].item = 2;
	table[2].string_handle = 0x190002a4;
	table[3].item = 3;
	table[3].string_handle = 0xa0002a5;
	function_24c75c(this, item, table, 0, 4);
}

// @retail 0x2b2494 deleting c_clan_options_list

#pragma pack(push, 2)
struct s_player_identity
{
	unsigned __int64 id;
	byte data[0x69e - 8];
	dword type;
};
#pragma pack(pop)

bool player_slot_get_identity(long index, s_player_identity *identity);
bool function_239abe(long controller_index);
class c_online_task_screen;
struct _XUID;
bool function_1a334a(long index, _XUID const *xuid);
bool function_19acc6(_XUID const *xuid);
void function_18ff47(long player, dword *out);
long online_team_member_remove(long controller_index, _XUID const *team, _XUID const *member);
long online_team_delete(_XUID const *team, long controller_index);
void function_1487c3(long controller_index, long task_index, long callback, long value, long context);
void __stdcall function_1a2cb7(c_online_task_screen *screen);
c_class_1473c9 *__stdcall xbox_live_message_send_screen_load_type3(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall xbox_live_message_send_screen_load_type4(s_screen_parameters *parameters);

/* a recipient of a message: the player's id and what follows it */
#pragma pack(push, 4)
struct s_message_recipient
{
	unsigned __int64 xuid;
	long unknown8;
};

/* a friend or clan member of the lists the online Y menu fills in */
struct s_online_member
{
	byte unknown00[4];
	s_message_recipient recipient;
	byte unknown10[0x10];
	dword flags;
};
#pragma pack(pop)

/* the message send screen as the clan and friends options fill it in */
struct s_message_send_screen_view
{
	byte unknown000[0xa24];
	s_message_recipient *recipients;
	long recipient_count;
	byte unknowna2c[0xa50 - 0xa2c];
	long valuea50;
};

/* leaves the clan, once the user confirms */
// @retail 0x2b278e
bool __stdcall function_2b278e(long controller_index)
{
	dword user[0x1c];
	s_player_identity identity;

	function_18ff47(controller_index, user);
	if (player_slot_get_identity(controller_index, &identity))
	{
		long task_index;

		if (identity.type > 1)
		{
			task_index = online_team_member_remove(controller_index, (_XUID const *)&identity, (_XUID const *)user);
		}
		else
		{
			task_index = online_team_delete((_XUID const *)&identity, controller_index);
		}
		if (task_index != NONE)
		{
			function_1487c3(controller_index, task_index, (long)function_1a2cb7, 0, 0);
		}
	}
	return true;
}

// score 54 (was 56): only case 3 differs. Retail keeps controller in esi, spills controller_index
// to [ebp+8] and reads (*controller)->controller_index again for the dialog masks after
// function_1a334a. Our build reuses the index loaded before player_slot_get_identity: LTCG here
// treats function_1a334a as writing no memory (an impure call placed after it gives retail's
// registers exactly), so the cause is outside this function. Tried: bool member (199: 8 bytes
// shorter, though closer by align.py), no controller_index local (199), long volatile
// controller_index (188). Case 2's count = 0 first: 56 -> 54.
// @retail 0x2b24ff
void c_clan_options_list::handle_item(s_controller_reference **controller, long *item)
{
	s_list_item_datum *datum = (s_list_item_datum *)record_pool_lookup(data, *item);

	if (datum)
	{

		switch (datum->item)
		{
		case 0:
			if (name)
			{
				name[0] = 0;
				function_238c21((*controller)->controller_index, 0xd, name, 0x10);
			}
			break;
		case 2:
		{
			s_screen_parameters parameters;
			s_list_item_iterator iterator;
			long count;
			parameters.field_c = 0;
			if (source && source->actual_count > 0)
			{
				if (!function_239abe((*controller)->controller_index))
				{
					break;
				}
				count = 0;
				iterator.iterator.data = source;
				iterator.iterator.index = NONE;
				iterator.iterator.datum_index = NONE;
				while (count < entry_count)
				{
					s_online_member *member;

					if (!function_2b2327(&iterator))
					{
						break;
					}
					member = (s_online_member *)iterator.item;

					if (member->recipient.xuid && !function_19acc6((_XUID const *)&member->recipient.xuid) && !(member->flags & 0xc00) && (member->flags & 1))
					{
						s_message_recipient *recipient = &((s_message_recipient *)entries)[count];
						++count;
						*recipient = member->recipient;
					}
				}
				if (count > 0)
				{
					s_message_send_screen_view *screen;

					function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)xbox_live_message_send_screen_load_type3);
					screen = (s_message_send_screen_view *)parameters.load(&parameters);
					if (screen)
					{
						s_message_recipient *recipients = (s_message_recipient *)entries;

						screen->valuea50 = 2;
						screen->recipients = recipients;
						screen->recipient_count = count;
					}
					break;
				}
			}
			function_236299(2);
			break;
		}
		case 1:
		{
			s_screen_parameters parameters;
			s_list_item_iterator iterator;
			long count;
			parameters.field_c = 0;
			if (source && source->actual_count > 0)
			{
				iterator.iterator.data = source;
				iterator.iterator.index = NONE;
				iterator.iterator.datum_index = NONE;
				count = 0;
				while (count < entry_count)
				{
					s_online_member *member;

					if (!function_2b2327(&iterator))
					{
						break;
					}
					member = (s_online_member *)iterator.item;

					if (member->recipient.xuid && !(member->flags & 0xc00))
					{
						s_message_recipient *recipient = &((s_message_recipient *)entries)[count];
						++count;
						*recipient = member->recipient;
					}
				}
				if (count > 0)
				{
					s_message_send_screen_view *screen;

					function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)xbox_live_message_send_screen_load_type4);
					screen = (s_message_send_screen_view *)parameters.load(&parameters);
					if (screen)
					{
						s_message_recipient *recipients = (s_message_recipient *)entries;

						screen->valuea50 = 2;
						screen->recipients = recipients;
						screen->recipient_count = count;
					}
					break;
				}
			}
			function_236299(2);
			break;
		}
		case 3:
		{
			long controller_index = (*controller)->controller_index;
			s_player_identity identity;

			if (player_slot_get_identity(controller_index, &identity))
			{
				/* volatile: stored to the stack slot where retail spills
				   controller_index, which keeps the function within a byte of retail's length (see the score note) */
				long volatile member = function_1a334a(controller_index, (_XUID const *)&identity);

				if (identity.type != 1 && member)
				{
					dialog_ok_show(v11(), 0x96, v12(), 1 << (*controller)->controller_index, 0, 0);
				}
				else
				{
					dialog_choice_show_default(v11(), v12(), 1 << (*controller)->controller_index, function_2b278e, 0x7a);
				}
			}
			break;
		}
		}
	}
	get_screen()->start_animation(3);
}

struct s_name_request;
void __stdcall function_148893(s_name_request *request, long flag);

// @retail 0x2b2b40
void c_friends_options_list::handle_item(s_controller_reference **controller, long *item)
{
	s_list_item_datum *datum = (s_list_item_datum *)record_pool_lookup(data, *item);

	if (datum)
	{
		switch (datum->item)
		{
		case 0:
			if (name)
			{
				function_148893(0, 1);
				name[0] = 0;
				function_238c21((*controller)->controller_index, 0xc, name, 0x10);
			}
			break;
		case 2:
		{
			s_screen_parameters parameters;
			s_list_item_iterator iterator;
			long count;
			parameters.field_c = 0;
			if (source && source->actual_count > 0)
			{
				if (!function_239abe((*controller)->controller_index))
				{
					break;
				}
				iterator.iterator.data = source;
				iterator.iterator.index = NONE;
				iterator.iterator.datum_index = NONE;
				count = 0;
				while (count < entry_count)
				{
					s_online_member *member;

					if (!function_2b2327(&iterator))
					{
						break;
					}
					member = (s_online_member *)iterator.item;
					if (member->recipient.xuid && !function_19acc6((_XUID const *)&member->recipient.xuid) && !(member->flags & 0x30) && (member->flags & 1))
					{
						s_message_recipient *recipient = &((s_message_recipient *)entries)[count];
						++count;
						*recipient = member->recipient;
					}
				}
				if (count > 0)
				{
					s_message_send_screen_view *screen;

					function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)xbox_live_message_send_screen_load_type3);
					screen = (s_message_send_screen_view *)parameters.load(&parameters);
					if (screen)
					{
						s_message_recipient *recipients = (s_message_recipient *)entries;

						screen->valuea50 = 1;
						screen->recipients = recipients;
						screen->recipient_count = count;
					}
				}
				else
				{
					function_236299(2);
				}
			}
			else
			{
				function_236299(2);
			}
			break;
		}
		case 1:
		{
			s_screen_parameters parameters;
			s_list_item_iterator iterator;
			long count;
			parameters.field_c = 0;
			if (source && source->actual_count > 0)
			{
				iterator.iterator.index = NONE;
				iterator.iterator.datum_index = NONE;
				count = 0;
				iterator.iterator.data = source;
				while (count < entry_count)
				{
					s_online_member *member;

					if (!function_2b2327(&iterator))
					{
						break;
					}
					member = (s_online_member *)iterator.item;
					if (member->recipient.xuid && !(member->flags & 0x30))
					{
						s_message_recipient *recipient = &((s_message_recipient *)entries)[count];
						++count;
						*recipient = member->recipient;
					}
				}
				if (count > 0)
				{
					s_message_send_screen_view *screen;

					function_149f49((s_message *)&parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)xbox_live_message_send_screen_load_type4);
					screen = (s_message_send_screen_view *)parameters.load(&parameters);
					if (screen)
					{
						s_message_recipient *recipients = (s_message_recipient *)entries;

						screen->valuea50 = 1;
						screen->recipients = recipients;
						screen->recipient_count = count;
					}
				}
			}
			else
			{
				function_236299(2);
			}
			break;
		}
		case 3:
		{
			s_screen_parameters screen_parameters;

			screen_parameters.field_c = 0;
			function_149f49((s_message *)&screen_parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)function_2b2917);
			screen_parameters.load(&screen_parameters);
			break;
		}
		case 4:
		{
			s_screen_parameters screen_parameters;

			screen_parameters.field_c = 0;
			function_149f49((s_message *)&screen_parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)function_2b288c);
			screen_parameters.load(&screen_parameters);
			break;
		}
		case 5:
		{
			s_screen_parameters screen_parameters;

			screen_parameters.field_c = 0;
			function_149f49((s_message *)&screen_parameters, 0, 0, 1 << (*controller)->controller_index, 3, 4, (long)function_2b2801);
			screen_parameters.load(&screen_parameters);
			break;
		}
		}
	}
	get_screen()->start_animation(3);
}

struct s_network_session_interface_view
{
	byte unknown00[0x48];
	dword flags;
};

byte *network_session_interface_get_data_4db0(void);
word function_157a40(void);

// @retail 0x2b4ce1
c_mp_change_teams_list::c_mp_change_teams_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_mp_change_teams_list::handle_item)
{
	s_network_session_interface_view *session = (s_network_session_interface_view *)network_session_interface_get_data_4db0();

	data = user_interface_data_new("mp change teams list", 9, 4);
	function_16b790(data);
	if (session->flags & 1)
	{
		dword teams = function_157a40();
		for (long team = 0; team < 8; team++)
		{
			if (teams & (1 << team))
			{
				long datum = record_pool_allocate(data);
				if (datum != NONE)
				{
					((s_list_item_datum *)data->data)[datum & 0xffff].item = (short)team;
				}
			}
		}
	}
	if (TEST_FIELD_BIT((session->flags >> 5) & 1))
	{
		long datum = record_pool_allocate(data);
		if (datum != NONE)
		{
			((s_list_item_datum *)data->data)[datum & 0xffff].item = NONE;
		}
	}
	delegate_register(&item_handlers, &handler);
}

// @retail 0x2b4dcd
void c_mp_change_teams_list::v20(c_class_1a2c81 *item, long unused)
{
	s_list_item_text table[9];

	table[0].item = 0;
	table[0].string_handle = 0x30001be;
	table[1].item = 1;
	table[1].string_handle = 0x40001bf;
	table[2].item = 2;
	table[2].string_handle = 0x50001c0;
	table[3].item = 3;
	table[3].string_handle = 0x60001c1;
	table[4].item = 4;
	table[4].string_handle = 0x60001c2;
	table[5].item = 5;
	table[5].string_handle = 0x60001c3;
	table[6].item = 6;
	table[6].string_handle = 0x50001c4;
	table[7].item = 7;
	table[7].string_handle = 0x40001c5;
	table[8].item = NONE;
	table[8].string_handle = 0x90001c6;
	function_24c75c(this, item, table, 0, 9);
}

long function_19aa17(long value);
byte *function_19aaa5(long player_index);

/* a player slot's team (+0x1fc) */
struct s_player_slot_team_view
{
	byte unknown000[0x1fc];
	long team;
	byte unknown200[0xc70 - 0x200];
};

/* moves the controller's player to the chosen team */
// @retail 0x2b4e5a
void c_mp_change_teams_list::handle_item(s_controller_reference **controller, long *item)
{
	if (*item != NONE)
	{
		s_list_item_datum *datum = &((s_list_item_datum *)data->data)[*item & 0xffff];
		long player_index = function_19aa17((*controller)->controller_index);

		if (player_index != NONE)
		{
			byte *player = function_19aaa5(player_index);
			short team = (char)player[0x7c];

			if (team != datum->item)
			{
				((s_player_slot_team_view *)g_54e8e0)[(*controller)->controller_index].team = datum->item;
				function_18fe9e((*controller)->controller_index);
			}
		}
	}
	get_screen()->start_animation(3);
}

/* ---- the lists' item texts ---- */

// @retail 0x2b1d6e
void c_class_232671::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);

	if (text)
	{
		switch ((short)widget_item(widget)->value70)
		{
		case 0:
			text->function_253b1a(0x17000196);
			break;
		case 1:
			text->function_253b1a(0x10000197);
			break;
		}
	}
}

// @retail 0x2b52e4
void c_squad_privacy_setting_list::v20(c_class_1a2c81 *widget, long index)
{
	s_list_item_text table[3];

	table[0].item = 0;
	table[0].string_handle = 0x4000201;
	table[1].item = 1;
	table[1].string_handle = 0xf000202;
	table[2].item = 2;
	table[2].string_handle = 0x6000203;
	function_24c75c(this, widget, table, 0, 3);
}

/* an item of the list of 0x45c318: its string */
struct s_list_45c318_datum
{
	short salt;
	short item;
	long string_handle;
};

// @retail 0x2b8ccf
void c_list_45c318::v20(c_class_1a2c81 *widget, long index)
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)widget->find_child(6, 0, false);

	if (text)
	{
		text->function_253b1a(((s_list_45c318_datum *)data->data)[widget_item(widget)->value70 & 0xffff].string_handle);
	}
}

// @retail 0x2b1613
void c_campaign_options_list::v20(c_class_1a2c81 *widget, long index)
{
	s_list_item_text table[5];

	table[0].item = 0;
	table[0].string_handle = 0x14000425;
	table[1].item = 1;
	table[1].string_handle = 0x15000426;
	table[2].item = 2;
	table[2].string_handle = 0x15000427;
	table[3].item = 3;
	table[3].string_handle = 0x4000424;
	table[4].item = 4;
	table[4].string_handle = 0x4000423;
	function_24c75c(this, widget, table, 0, 5);
}

byte *function_19aaa5(long player_index);

/* an item of the potential squad leader list: its player */
struct s_squad_leader_datum
{
	short salt;
	short unknown02;
	long player_index;
};

/* shows the player's name */
// @retail 0x2b83db
void c_potential_squad_leader_player_list::v20(c_class_1a2c81 *widget, long index)
{
	c_class_1a2c81 *text = widget->find_child(6, 0, false);

	if (text && widget_item(widget)->value70 != NONE)
	{
		s_squad_leader_datum *datum = &((s_squad_leader_datum *)data->data)[widget_item(widget)->value70 & 0xffff];

		if (function_19a951(*(long volatile *)&datum->player_index))
		{
			word *name = (word *)function_19aaa5(*(long volatile *)&datum->player_index);

			text->function_22f52e()->set_text(name);
		}
		else
		{
			text->function_22f52e()->set_text((word *)L"");
		}
	}
}


/* ---- the actions of the online Y menu's player selected list ---- */

void function_238e42(long controller_index, long type);
void function_238ee3(long controller_index);
void function_2390f8(long controller_index);
void function_23914b(long controller_index);
void function_239197(long controller_index);
void function_23922e(long controller_index);
void function_239277(long controller_index);
void __stdcall function_2393ae(long controller, long privilege);
void __stdcall function_238ea7(long user_index);
long function_19adca(_XUID const *xuid);
bool function_19a179(long player_index);
bool function_19a1d4(long player_index);
void online_mutelist_add(long controller_index, const _XUID *xuid);
void online_mutelist_remove(long controller_index, const _XUID *xuid);

/* the selected player as the list reads it: a user or a friend, each
   starting with the player's id */
struct s_player_selection_xuid
{
	dword data[3];
};

struct s_player_selection
{
	long type;
	union
	{
		s_player_selection_xuid user_xuid;
		s_player_selection_xuid friend_xuid;
	};
	byte unknown10[0x78 - 0x10];
};

/* the selected player's id */
static inline _XUID const *player_selection_get_xuid(s_player_selection *selection)
{
	_XUID const *result = 0;

	switch (selection->type)
	{
	case 1:
		result = (_XUID const *)&selection->user_xuid;
		break;
	case 2:
		result = (_XUID const *)&selection->friend_xuid;
		break;
	}
	return result;
}

/* an id as the mute list takes it (16 bytes) */
struct s_player_xuid_16
{
	unsigned __int64 id;
	dword flags;
	dword unknown0c;
};

/* the screen that holds the list */
struct s_player_selected_screen_view
{
	byte unknown0000[0x10e8];
	long value10e8;
};

// @retail 0x2b5f9b
bool __stdcall function_2b5f9b(long controller_index)
{
	function_2393ae(controller_index, NONE);
	return true;
}

// @retail 0x2b5eeb
bool __stdcall function_2b5eeb(long controller_index)
{
	function_238ea7(controller_index);
	return true;
}

// @retail 0x2b5ecb
void function_2b5ecb(c_class_1a2c81 *list, s_controller_reference **controller)
{
	function_2b61ce(1 << (*controller)->controller_index, ((s_player_selected_screen_view *)list->parent)->value10e8);
}

// @retail 0x2b5ef9
void function_2b5ef9(s_controller_reference **controller)
{
	if (g_4e6948->state == 1)
	{
		dialog_choice_show(1, 9, 4, 1 << (*controller)->controller_index, function_2b5eeb, 0, 0);
	}
	else
	{
		function_238ea7((*controller)->controller_index);
	}
}

// @retail 0x2b5f2e
void function_2b5f2e(bool *close)
{
	s_player_selection selection;

	_XUID const *xuid;

	function_14887e((s_screen_settings_54dc6c *)&selection);
	xuid = player_selection_get_xuid(&selection);
	if (xuid && function_19acc6(xuid))
	{
		function_19a179(function_19adca(xuid));
	}
	*close = true;
}

// @retail 0x2b5f6a
void function_2b5f6a(s_controller_reference **controller, bool *close)
{
	function_238ee3((*controller)->controller_index);
	*close = false;
}

// @retail 0x2b5f7e
void function_2b5f7e(s_controller_reference **controller)
{
	if (function_239abe((*controller)->controller_index))
	{
		function_238e42((*controller)->controller_index, 3);
	}
}

// @retail 0x2b5fab
void function_2b5fab(s_controller_reference **controller)
{
	dialog_choice_show(3, 0x2f, 4, 1 << (*controller)->controller_index, function_2b5f9b, 0, 0);
}

// @retail 0x2b5fcb
void __stdcall function_2b5fcb(s_controller_reference **controller, bool *close)
{
	s_player_selection selection;
	s_player_xuid_16 xuid = { 0 };

	function_14887e((s_screen_settings_54dc6c *)&selection);
	switch (selection.type)
	{
	case 1:
		*(s_player_selection_xuid *)&xuid = selection.user_xuid;
		break;
	case 2:
		*(s_player_selection_xuid *)&xuid = selection.friend_xuid;
		break;
	default:
		__assume(0);
	}
	online_mutelist_add((*controller)->controller_index, (_XUID const *)&xuid);
	*close = true;
}

// @retail 0x2b6021
void __stdcall function_2b6021(s_controller_reference **controller, bool *close)
{
	s_player_selection selection;
	s_player_xuid_16 xuid = { 0 };

	function_14887e((s_screen_settings_54dc6c *)&selection);
	*(s_player_selection_xuid *)&xuid = selection.user_xuid;
	online_mutelist_remove((*controller)->controller_index, (_XUID const *)&xuid);
	*close = true;
}

// @retail 0x2b609b
bool __stdcall function_2b609b(long controller_index)
{
	s_player_selection selection;

	_XUID const *xuid;

	function_14887e((s_screen_settings_54dc6c *)&selection);
	xuid = player_selection_get_xuid(&selection);
	if (xuid && function_19acc6(xuid) && !function_19a1d4(function_19adca(xuid)))
	{
		function_236299(2);
	}
	return true;
}

// @retail 0x2b60e3
void function_2b60e3(s_controller_reference **controller)
{
	dialog_choice_show(3, 0x97, 4, 1 << (*controller)->controller_index, function_2b609b, 0, 0);
}

// @retail 0x2b54f4
c_y_menu_player_selected_list::c_y_menu_player_selected_list(word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_y_menu_player_selected_list::handle_item),
	value3a0(0),
	value3a4(false)
{
	data = user_interface_data_new("Y-menu player selected list", 0x14, 4);
	function_16b790(data);
	delegate_register(&item_handlers, &handler);
}

/* does the chosen action to the selected player; most close the screen */
// @retail 0x2b5d3b
void c_y_menu_player_selected_list::handle_item(s_controller_reference **controller, long *item)
{
	bool close = true;

	if (*item != NONE)
	{
		s_list_item_datum *datum = &((s_list_item_datum *)data->data)[*item & 0xffff];

		switch (datum->item)
		{
		case 0:
			function_2b5ecb(this, controller);
			break;
		case 1:
			function_23914b((*controller)->controller_index);
			break;
		case 2:
			function_239197((*controller)->controller_index);
			break;
		case 3:
			function_238e42((*controller)->controller_index, 4);
			break;
		case 4:
			function_2b5ef9(controller);
			break;
		case 5:
			function_2b5ef9(controller);
			break;
		case 6:
			function_2b5f2e(&close);
			break;
		case 7:
			function_2b5f6a(controller, &close);
			break;
		case 8:
			function_2b5f7e(controller);
			break;
		case 9:
			function_2390f8((*controller)->controller_index);
			break;
		case 10:
			function_238e42((*controller)->controller_index, 1);
			break;
		case 11:
			function_23922e((*controller)->controller_index);
			break;
		case 12:
			function_238e42((*controller)->controller_index, 2);
			break;
		case 13:
			function_2b6068(controller);
			break;
		case 14:
			function_2b5fcb(controller, &close);
			break;
		case 15:
			function_2b6021(controller, &close);
			break;
		case 16:
			function_2b60e3(controller);
			break;
		case 17:
			function_239277((*controller)->controller_index);
			break;
		case 18:
			function_2b5fab(controller);
			break;
		case 19:
			function_2393ae((*controller)->controller_index, NONE);
			break;
		default:
			__assume(0);
		}
	}
	if (close)
	{
		get_screen()->start_animation(3);
	}
}

void qos_release(long handle);
long online_get_nat_type(void);
void function_19adf6(const byte *data, long value);

/* the Live sign in's qos probe (unknown_24b5bc.cpp) */
extern long g_475338;

/* passes the sign in's qos result on with the NAT type, and releases the
   probe */
// @retail 0x2bb0e9
void function_2bb0e9(void)
{
	if (g_475338 != NONE)
	{
		s_qos_result result;

		if (qos_target_result(g_475338, 0, &result))
		{
			function_19adf6((const byte *)&result, online_get_nat_type());
		}
		qos_release(g_475338);
		g_475338 = NONE;
	}
}

/* the value of the list's focused datum */
// @retail 0x2b18b7
short function_2b18b7(c_class_1474e8 *list)
{
	long datum_index = list->get_focused_datum();
	short *datum = (short *)record_pool_lookup(list->data, datum_index);
	short result;

	if (datum)
	{
		result = datum[1];
	}
	else
	{
		result = NONE;
	}
	return result;
}

/* shows the description of the focused option */
// @retail 0x2b18d5
void c_screen_458a00::v3()
{
	short item = function_2b18b7(&list);
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)find_child(6, 2, false);
	long string_handle;

	switch (item)
	{
	case 0:
		string_handle = 0x10000779;
		break;
	case 1:
		string_handle = 0x1100077a;
		break;
	case 2:
		string_handle = 0x900077b;
		break;
	case 3:
		string_handle = 0x900077c;
		break;
	case 4:
		string_handle = 0x900077b;
		break;
	default:
		string_handle = NONE;
		break;
	}
	if (text && string_handle != NONE)
	{
		text->function_253b1a(string_handle);
	}
	c_class_1a2c81::v3();
}

// @retail 0x2b19bd destructor c_list_45af88

struct s_model_names_2b
{
    byte unknown00[12];
    long count;
    char (*names)[32];
};
struct s_object_header_2b
{
    short salt;
    byte flags;
    byte type;
    long unused;
    byte *object;
};
struct s_name_table_11c0b0;
short function_11c0b0(char const *name, s_name_table_11c0b0 *table);
long function_bb760(short index);
bool function_10f5c0(long unit_index, real blend, long flags, long mode, long set);
bool function_bacc0(long object_index, long index, point3f const *point);

// @retail 0x2b102f
void function_2b102f(c_class_2b0b5e *widget, dword const *value)
{
    s_name_table_11c0b0 *table = (s_name_table_11c0b0 *)g_4e0350;
    for (long i = 0; i < ((s_model_names_2b *)widget->definition)->count; ++i)
    {
        char const *name = ((s_model_names_2b *)widget->definition)->names[i];
        if (name[0])
        {
            short index = function_11c0b0(name, table);
            if (index != NONE)
            {
                long object_index = function_bb760(index);
                if (object_index != NONE)
                {
                    s_object_header_2b *header = &((s_object_header_2b *)g_4e0300->data)[object_index & 0xffff];
                    if (header->type == 0)
                        *(dword *)(header->object + 0x24) = *value;
                }
            }
        }
    }
}

// @retail 0x2b0eeb
void function_2b0eeb(c_class_2b0b5e *widget, long mode)
{
    s_model_names_2b *definition = (s_model_names_2b *)widget->definition;
    if (definition && definition->count > 0)
    {
        s_name_table_11c0b0 *table = (s_name_table_11c0b0 *)g_4e0350;
        for (long i = 0; i < definition->count; ++i)
        {
            char const *name = definition->names[i];
            if (name[0])
            {
                short index = function_11c0b0(name, table);
                if (index != NONE)
                {
                    long object_index = function_bb760(index);
                    if (object_index != NONE && ((s_object_header_2b *)g_4e0300->data)[object_index & 0xffff].type == 0)
                        function_10f5c0(object_index, 0.0f, 2, mode, 0x7000101);
                }
            }
            /* the calls above can change the widget's definition */
            definition = (s_model_names_2b *)widget->definition;
        }
    }
}

// @retail 0x2b0f76
void function_2b0f76(c_class_2b0b5e *widget, long index, point3f const *point)
{
    for (long i = 0; i < ((s_model_names_2b *)widget->definition)->count; ++i)
    {
        char const *name = ((s_model_names_2b *)widget->definition)->names[i];
        if (name[0])
        {
            short named_index = function_11c0b0(name, (s_name_table_11c0b0 *)g_4e0350);
            if (named_index != NONE)
            {
                long object_index = function_bb760(named_index);
                if (object_index != NONE && ((s_object_header_2b *)g_4e0300->data)[object_index & 0xffff].type == 0)
                {
                    function_bacc0(object_index, index, point);
                    long tag = *(long *)((s_object_header_2b *)g_4e0300->data)[object_index & 0xffff].object;
                    *(dword *)(g_4e3b44[tag & 0xffff].bytes + 0x1c) |= 1;
                }
            }
        }
    }
}

#include "online_tasks.h"
long qos_service_lookup();
bool qos_is_complete(long handle);
void function_1486b8(long error, bool keep);

// @retail 0x2bb04e
bool function_2bb04e(long task_index, bool *completion)
{
    bool result;
    *completion = false;
    if (g_475338 != NONE)
    {
        result = true;
        if (qos_is_complete(g_475338))
            *completion = true;
    }
    else
    {
        result = false;
        if (task_index != NONE)
        {
            s_type_9df9da *task = function_6b910(task_index);
            if (task)
            {
                long status = online_task_get_logon_status(task_index);
                long type = online_task_get_type(task_index);
                if (status != 0)
                {
                    if (status == 1)
                    {
                        g_475338 = qos_service_lookup();
                        result = true;
                    }
                    else if (type == 1)
                        function_1486b8(status, true);
                }
                else if (!(task->flags & 4) && (type == 0 || type == 1))
                    result = true;
            }
        }
    }
    if (!result || *completion)
        function_2bb0e9();
    return result;
}

bool function_19a902();
long function_199f34();
bool function_19a7e9(long controller, long value);
void __stdcall function_2395dc(XONLINE_FRIEND *friend_, long controller_index, long mode);

// @retail 0x2b8497
void c_potential_squad_leader_player_list::handle_item(s_controller_reference **controller, long *item)
{
    s_list_item_value volatile *entry = (s_list_item_value *)record_pool_lookup(data, *item);
    if (entry && function_19a951(entry->value))
    {
        if (!function_19a179(entry->value))
        {
            if (!value288)
            {
                goto animate;
            }
            if (!function_19a179(NONE))
                goto finish;
        }
        if (function_19a902())
        {
            function_199f34();
            function_19a7e9((*controller)->controller_index, 10);
        }
    }
finish:
    if (value288)
    {
        s_feedback_target selection;
        function_14887e((s_screen_settings_54dc6c *)&selection);
        function_2395dc((XONLINE_FRIEND *)((byte *)&selection + 4), (*controller)->controller_index, 4);
    }
animate:
    get_screen()->start_animation(3);
}

real g_509330;
real g_509334;
dword g_509338;
void function_28980(long index);

struct s_main_menu_music;
real main_menu_music_silence_fraction(s_main_menu_music const *music);
dword main_menu_music_silence_time();
real __stdcall function_122dd0(byte *map_name, long mode, long type);

// @retail 0x2c52e7
void __stdcall function_2c52e7(char const *map_name)
{
	real progress = function_122dd0((byte *)map_name, 0, 0);
	real step = 1.0f;
	real duration = (real)(long)main_menu_music_silence_time();
	dword now = GetTickCount();
	real elapsed = (real)(now - g_509338);
	g_509338 = now;
	if (duration > 0.0f)
	{
		real clamped = elapsed > 0.0 ? elapsed : 0.0;
		step = clamped / duration;
	}
	if (0.0f > g_509330)
		g_509330 = progress;
	if (!(0.001f >= 1.0f - g_509330))
	{
		if (g_509330 > progress) progress = g_509330;
		else if (progress > 1.0f) progress = 1.0f;
		progress = (progress - g_509330) / (1.0f - g_509330);
		if (0.0f > progress) progress = 0.0f;
		else if (progress > 1.0f) progress = 1.0f;
	}
	else progress = 1.0f;
	// Read music through the existing canonical member const view at its later use.
	real target = main_menu_music_silence_fraction((s_main_menu_music const *)&g_54d598.main_menu_music) * 0.3f + progress * 0.7f;
	if (target >= g_509334)
	{
		g_509334 += step;
		if (!(target > g_509334)) g_509334 = target;
	}
}


// @retail 0x2c5224
void c_level_select_screen::v18(void *parameters)
{
    volatile long definition_index = function_1480ff(screen_id);
    s_screen_layout layout = { 0, 1, { { 0, 0, 0, 0 } } };
    build(&layout);
    c_class_1a2c81::v2();
    delete_children();
    g_509330 = -1.0f;
    g_509334 = 0.0f;
    g_509338 = GetTickCount();
    function_28980((long)&g_509334);
    c_class_1a2c81::v1();
}

// @retail 0x2b1942
c_list_45af88::c_list_45af88(word user_flags) : c_list_24a64d(user_flags)
{
}

// @retail 0x2b19a1 deleting c_list_45af88

/* s_profile_list_entry as this screen holds it: retail builds it with the
   entry constructor written inline */
struct s_profile_list_entry_inline
{
    __forceinline s_profile_list_entry_inline()
    {
        profile_index = NONE;
        memset(&settings, 0, sizeof(settings));
    }
    long profile_index;
    s_player_profile_settings settings;
    byte unknown1e4[0x248 - 0x1e4];
};

class c_screen_45b0b8 : public c_screen_with_menu
{
public:
    c_screen_45b0b8(long a, long b, word user_flags);
    virtual screen_load_proc get_load_proc();
    c_list_45af88 profile_choices;
    s_profile_list_entry_inline selected;
    bool value19b4;
};

// @retail 0x2b1a7c
screen_load_proc c_screen_45b0b8::get_load_proc()
{
    return function_2b19dc;
}

// @retail 0x2b1a1c
c_screen_45b0b8::c_screen_45b0b8(long a, long b, word user_flags) :
    c_screen_with_menu(0xc1, a, b, user_flags, &profile_choices),
    profile_choices(user_flags)
{
    value19b4 = false;
    profile_choices.field_89 = true;
    memset(&selected, 0, sizeof(selected));
}

// @retail 0x2b1a9e destructor c_screen_45b0b8

// @retail 0x2b0b96
void c_class_2b0b5e::v1()
{
    c_class_1a2c81::v1();
    function_2b0eeb(this, 0x20001b3);
}

#include "unknown_07f720.h"

// @retail 0x2b10a3
void function_2b10a3(dword const *appearance, c_class_2b0b5e *widget, long type)
{
    bool visible = false;
    if (appearance)
        visible = type == ((char const *)appearance)[4];
    widget->value6e = visible;
    if (visible)
    {
        color3f colors[4];
        function_7f790(NONE, false, (s_player_appearance const *)appearance, colors);
        function_2b102f(widget, (dword const *)((byte const *)appearance + 5));
        function_2b0f76(widget, 0, (point3f *)&colors[0]);
        function_2b0f76(widget, 1, (point3f *)&colors[1]);
        function_2b0f76(widget, 2, (point3f *)&colors[2]);
        function_2b0f76(widget, 3, (point3f *)&colors[3]);
    }
}

struct s_friend_request { byte data[0x6a2]; };
bool friend_request_get(s_friend_request *request);
bool friends_list_task_running();
bool screen_lower_channel_window_in_use(c_class_1473c9 *screen);
void friends_lists_get_user(XUID const *xuid, bool *listed_friend, bool *is_player, dword *flags, DWORD *title_id, bool *in_session, XONLINE_FRIEND *details);
bool xuid_equal(XUID const *a, XUID const *b, bool compare_guest_number);
bool network_session_interface_has_user(XUID const *xuid);
bool online_title_is_this_title(DWORD title_id);
bool online_messages_find_from(XUID const *sender, DWORD controller_index, long kind);
bool online_messages_find_flagged_from(XUID const *sender, DWORD controller_index);
bool online_mutelist_contains(long controller_index, XUID const *xuid);
bool function_19a935();
bool function_592f0();
bool function_19ab77(long player_index);
bool function_19a250();
bool function_19a203();

/* Retail tail-calls this separate update body from the list tick. */
PRIVATE __declspec(noinline) void update_player_actions_2b55ae(c_y_menu_player_selected_list *list);

// @retail 0x2b55ae
PRIVATE void update_player_actions_2b55ae(c_y_menu_player_selected_list *list)
{
    long controller = list->get_controller_index();
    s_player_selection selection;
    dword identity[28];
    s_player_slot_blockb82 local_status;
    function_14887e((s_screen_settings_54dc6c *)&selection);
    function_18ff47(controller, identity);
    function_18ffc3(controller, &local_status);
    XUID const *xuid = player_selection_get_xuid(&selection);
    if (xuid && xuid->qwUserID)
    {
        record_pool_release_all(list->data);
        bool listed_friend, is_player, same_title;
        dword flags;
        DWORD title;
        friends_lists_get_user(xuid, &listed_friend, &is_player, &flags, &title, &same_title, 0);
        bool from_friends = selection.type == 2;
        bool self = xuid_equal(xuid, (XUID *)identity, false);
        bool online = (bool)(flags & 1);
        bool in_session = network_session_interface_has_user(xuid);
        long player = function_19adca(xuid);
        bool has_player = function_19acc6(xuid);
        bool joinable = ((flags & 8) != 0 || in_session);
        bool can_join = online && same_title && joinable && !self && !has_player && from_friends;
        bool can_join_title = online_title_is_this_title(title) && can_join;
        long mode = list->value3a0;
        bool message = online_messages_find_from(xuid, controller, mode);
        bool muted = online_mutelist_contains(controller, xuid);
        bool host = function_19a935();
        bool allow_remove = function_592f0();
        bool protected_player = in_session && player != NONE && function_19ab77(player);
        s_friend_request request;
        bool request_valid = friend_request_get(&request);
        bool regular_friend = listed_friend && !(flags & 0x30);
        bool regular_player = is_player && !(flags & 0xc00);
        bool allow_join = function_19a250();
        bool pending = list->value3a0 == 1 && !online_messages_find_flagged_from(xuid, controller) && (flags & 0x10);
        bool invite = (regular_friend || regular_player) && !self;
        can_join_title = can_join_title && !self && allow_join;
        bool join_other = !can_join_title && can_join && !self && allow_join;
        bool remove_player = allow_remove && has_player && !protected_player;
        bool feedback = !self && (list->value3a0 == 3 || list->value3a0 == 4);
        bool offer = !has_player && !self && ((list->value3a0 == 4 && in_session) || (list->value3a0 != 3 && invite));
        bool promote = host && !self && (flags & 0x100);
        bool add_friend = !listed_friend && !self;
        bool cancel_friend = listed_friend && (flags & 0x20) && (list->value3a0 == 1 || list->value3a0 == 3);
        bool remove_friend = listed_friend && !(flags & 0x30) && list->value3a0 == 1;
        long rank = *(long *)(local_status.data + 0x1c);
        bool add_member = request_valid && !is_player && !self && rank >= 1 && list->value3a0 != 4 && list->value3a0 != 2;
        bool pending_member;
        bool change_member;
        if (!add_member && is_player && !self && (flags & 0x800) && list->value3a0 == 2)
        {
            pending_member = true;
            change_member = false;
        }
        else
        {
            pending_member = false;
            change_member = regular_player && rank >= 2 && !self && list->value3a0 == 2;
        }
        bool remove_member = regular_player && rank >= 2 && !self && list->value3a0 == 2;
        bool mute = !muted && !self;
        bool unmute = !mute && muted && !self;
        bool kick = allow_remove && has_player && !protected_player && function_19a203();
        if (message) list_item_add(list, 0);
        if (pending) { list_item_add(list, 1); list_item_add(list, 2); }
        if (invite) list_item_add(list, 3);
        if (can_join_title) list_item_add(list, 4);
        if (join_other) list_item_add(list, 5);
        if (remove_player) list_item_add(list, 6);
        if (offer) list_item_add(list, 8);
        if (promote) list_item_add(list, 9);
        if (add_friend) list_item_add(list, 10);
        if (cancel_friend) list_item_add(list, 11);
        if (add_member) list_item_add(list, 12);
        if (remove_member) list_item_add(list, 13);
        if (mute) list_item_add(list, 14);
        if (unmute) list_item_add(list, 15);
        if (kick) list_item_add(list, 16);
        if (remove_friend) list_item_add(list, 17);
        if (change_member) list_item_add(list, 18);
        if (pending_member) list_item_add(list, 19);
        if (feedback) list_item_add(list, 7);
        function_24c0c4((c_widget *)list);
    }
    list->value3a4 = true;
}

// @retail 0x2b557a
void c_y_menu_player_selected_list::v3()
{
    ((c_widget *)this)->c_widget::v11();
    if (!value3a4 && !screen_lower_channel_window_in_use(get_screen()) && friends_list_task_running())
        update_player_actions_2b55ae(this);
}

// @retail 0x2b1a82 deleting c_screen_45b0b8

// @retail 0x2b19dc
c_class_1473c9 *__stdcall function_2b19dc(s_screen_parameters *parameters)
{
    c_screen_45b0b8 *screen = new c_screen_45b0b8(parameters->a, parameters->b, parameters->user_flags);
    screen->m6c = true;
    screen->function_147f6d(parameters);
    return screen;
}

struct s_bitmap_view;
struct s_predicted_resource_block;
D3DTexture *function_12360(s_bitmap_view *bitmap, real priority);
bool function_16e5e0(s_predicted_resource_block const *block, short mode);

// @retail 0x2b1179
void c_widget_45adf0::v3()
{
    c_class_1a2c81::v3();
    s_type_7ba8e9 *bitmap = function_137550(definition->tag_index, *(short *)((byte *)definition + 0xa));
    if (bitmap)
        function_12360((s_bitmap_view *)bitmap, 0.0f);
    long tag = *(long *)((byte *)definition + 0x18);
    if (tag != NONE)
        function_16e5e0((s_predicted_resource_block const *)(g_4e3b44[tag & 0xffff].bytes + 0x2c), 0);
}

/* The screen waiting for a map to finish loading. */
class c_load_progress_screen : public c_class_1473c9
{
public:
    virtual void v3();
    long state;
    bool dialog_shown;
};

bool window_manager_channel_has_dialog(long channel);
word function_1901fc();
bool __stdcall function_2c55c4(long unused);
bool __stdcall function_163890(char const *map_name, long mode);
long map_location_progress_get(char const *map_name);
struct s_entry_a;
s_entry_a *function_148d61();
bool window_manager_main_menu_music_silence_done();
void function_23643f();
void __stdcall function_2c52e7(char const *map_name);
extern real g_509334;

// @retail 0x2c542b
void c_load_progress_screen::v3()
{
    c_class_1a2c81::v3();
    if (!ANIMATION_FLAG(animation, 1))
    {
        if (window_manager_channel_has_dialog(0) ||
            window_manager_channel_has_dialog(1) ||
            window_manager_channel_has_dialog(3))
        {
            if (!dialog_shown)
            {
                function_14800c(v20(), v21());
                dialog_shown = true;
            }
        }
        else if (state == 0)
        {
            byte *entry = (byte *)function_148d61();
            char *map_name = 0;
            long progress = 3;
            if (entry)
            {
                map_name = (char *)(entry + 8);
                if (map_name)
                    progress = map_location_progress_get(map_name);
            }
            switch (progress)
            {
            case 2:
                if (window_manager_main_menu_music_silence_done() && fabs(g_509334 - 1.0f) < 0.0001f)
                {
                    function_23643f();
                    start_animation(3);
                }
                break;
            case 3:
                if (!dialog_shown)
                {
                    function_14800c(v20(), v21());
                    dialog_ok_show(1, 0x22, 4, function_1901fc(), function_2c55c4, 0);
                    dialog_shown = true;
                }
                break;
            default:
                function_163890(map_name, 1);
                break;
            }
            if (map_name)
                function_2c52e7(map_name);
            short width = bounds.right - bounds.left;
            c_class_1a2c81 *bitmap = find_child(8, 1, false);
            if (bitmap)
            {
                s_widget_bounds rectangle;
                bitmap->get_bounds(&rectangle);
                short local_62a74a = rectangle.right - rectangle.left;
                rectangle.left = (short)(width * g_509334) - width / 2;
                rectangle.right = rectangle.left + local_62a74a;
                bitmap->bounds = rectangle;
            }
        }
    }
}

#include "unknown_07f720.h"

struct s_510c4c;
extern s_510c4c *g_510c4c;

struct s_preview_state_2b11
{
    byte unknown00[0x1b4];
    long player_index;
    byte unknown1b8[0x1d0 - 0x1b8];
    short sequence;
};
struct s_widget_quad_2b11
{
    real points[8];
};

struct s_4e6950
{
    byte unknown00[0x1c];
    real value1c;
    byte unknown20[0x80 - 0x20];
};
extern s_4e6950 g_4e6950;
void function_1bbd0();
void function_1392f8(s_player_appearance const *appearance);
s_float_rect *function_22ea81(c_class_1a2c81 *widget, s_float_rect *rect, short_rectangle2d const *frame);
void function_22a664(s_widget_quad_2b11 const *quad, s_float_rect const *coordinates,
    long tag_index, long sequence, long resource_index);

// @retail 0x2b11c9
void c_widget_45adf0::v4(long frame)
{
    if (definition && (!(*(dword *)definition & 2) || *(short *)unknown86 != NONE))
    {
        real scale = animation.scale;
        s_float_rect coordinates;
        coordinates.x0 = coordinates.y0 = 0.0f;
        coordinates.x1 = coordinates.y1 = 1.0f;
        s_float_rect rectangle;
        function_22ea81(this, &rectangle, (short_rectangle2d const *)frame);
        function_1bbd0();
        s_preview_state_2b11 *state = (s_preview_state_2b11 *)g_510c4c;
        state->player_index = NONE;
        function_1392f8((s_player_appearance const *)value74);
        s_widget_quad_2b11 quad;
        quad.points[0] = rectangle.x0;
        quad.points[1] = rectangle.y0;
        quad.points[2] = rectangle.x1;
        quad.points[3] = rectangle.y0;
        quad.points[4] = rectangle.x1;
        quad.points[5] = rectangle.y1;
        quad.points[6] = rectangle.x0;
        quad.points[7] = rectangle.y1;
        state->sequence = *(short *)unknown86;
        g_4e6950.value1c = scale;
        function_22a664(&quad, &coordinates, definition->tag_index,
            *(short *)((byte *)definition + 0xa), *(long *)((byte *)definition + 0x18));
    }
}


extern dword g_54d5b8;

// @retail 0x2b0328
void c_class_2b01eb::v3()
{
 dword now = g_54d5b8;
 c_class_1a2c81::v3();
 if (definition)
 {
  s_bitmap_block *block = definition;
  real rate = *(real *)((byte *)block + 0x24);
  real period;
  if (rate >= 0.001f)
   period = 1000.0f / *(real *)((byte *)block + 0x24);
  else
   period = 0.0f;
  dword frame_duration = (long)period;
  dword elapsed = now - start_time;
  real seconds = (dword)value78 > 0 ? (real)(dword)(now - value78) * 0.001f : 0.0f;
  if ((byte)function_2b0a68((s_widget_view_2b0a *)this))
  {
   real progress = value84 < 0.0f ? 0.0f : value84 > 1.0f ? 1.0f : value84;
   real width = (real)(*(short *)((byte *)block + 0x28) - block->x) * progress;
   long rounded_width;
   __asm {
    fld width
    fistp rounded_width
   }
   bounds.right = bounds.left + (short)rounded_width;
  }
  else if (block->tag_index != NONE)
  {
   if (frame_duration > 0)
   {
    s_bitmap_tag_2b0a *tag = (s_bitmap_tag_2b0a *)g_4e3b44[block->tag_index & 0xffff].bytes;
    sequence = (elapsed / frame_duration) % tag->count;
   }
   s_type_7ba8e9 *data = function_2b0b19((s_widget_view_2b0a *)this);
   bounds.right = bounds.left + data->width;
   bounds.bottom = bounds.top - data->height;
   function_12360((s_bitmap_view *)data, 0.0f);
  }
  value7c += *(real *)((byte *)definition + 0x10) * seconds;
  value80 += *(real *)((byte *)definition + 0x14) * seconds;
  while (value7c > 1.0f)
   value7c -= 1.0f;
  while (value80 > 1.0f)
   value80 -= 1.0f;
  while (value7c < 0.0f)
   value7c += 1.0f;
  while (value80 < 0.0f)
   value80 += 1.0f;
 }
 value78 = now;
}

struct s_model_render_definition_2b
{
    byte field_0[0xc];
    long object_count;
    char (*objects)[32];
    long extra_count;
    char (*extras)[32];
    real distance[3];
    point3f position;
    real angle;
    s_widget_bounds bounds;
    byte field_40[0xc];
};

struct s_visibility_object_lists;
extern color3f *g_46871c;
extern color4f *g_4686d8;
static s_visibility_object_lists *const model_visibility_lists = (s_visibility_object_lists *)((byte *)g_547f88.lists - offsetof(s_bit_vector_pool, lists));
void visibility_object_lists_add(long const *, long, s_visibility_object_lists *, long const *, long, bool);
void function_bbf40(long object_index, bool flag);
long function_c38e0(short index);
real function_30bf0(vector3f *v);
void __stdcall function_2cbc0(bool enabled);
void function_44720();
void function_2c560(short left, short top, short right, short bottom,
    vector3f const *up, real angle, vector3f const *forward, point3f const *position);

// @retail 0x2b0baa
void c_class_2b0b5e::v4(long frame)
{
    if (*(long *)g_54d598.unknown00 != 4)
        return;
    long object_indices[32];
    long extra_indices[8];
    long object_count = 0;
    long extra_count = 0;
    for (long i = 0; i < ((s_model_render_definition_2b *)definition)->object_count; ++i)
    {
        char const *name = ((s_model_render_definition_2b *)definition)->objects[i];
        if (*name)
        {
            short index = function_11c0b0(name, (s_name_table_11c0b0 *)g_4e0350);
            if (index != NONE)
            {
                long object_index = function_bb760(index);
                if (object_index != NONE)
                {
                    object_indices[object_count++] = object_index;
                    function_bbf40(object_index, true);
                }
            }
        }
    }
    for (long j = 0; j < ((s_model_render_definition_2b *)definition)->extra_count; ++j)
    {
        char const *name = ((s_model_render_definition_2b *)definition)->extras[j];
        if (*name)
        {
            short index = function_11c0b0(name, (s_name_table_11c0b0 *)g_4e0350);
            if (index != NONE)
            {
                long object_index = function_c38e0(index);
                if (object_index != NONE)
                    extra_indices[extra_count++] = object_index;
            }
        }
    }
    if (object_count > 0)
    {
        s_widget_bounds const *local_00ace5 = (s_widget_bounds const *)frame;
        short_rectangle2d const *screen = (short_rectangle2d const *)frame;
        long width = local_00ace5->right - local_00ace5->left;
        long height = local_00ace5->bottom - local_00ace5->top;
        long left = (width >> 1) + ((s_model_render_definition_2b const *)definition)->bounds.left;
        long top = (height >> 1) - ((s_model_render_definition_2b const *)definition)->bounds.top;
        long right = (width >> 1) + ((s_model_render_definition_2b const *)definition)->bounds.right;
        long bottom = (height >> 1) - ((s_model_render_definition_2b const *)definition)->bounds.bottom;
        real degrees = 1.0f > ((s_model_render_definition_2b const *)definition)->angle ? 1.0f :
            (((s_model_render_definition_2b const *)definition)->angle > 359.0f ? 359.0f : ((s_model_render_definition_2b const *)definition)->angle);
        real angle = degrees * 0.008726646192371845f;
        bool show_label = g_54d59f && has_screen() && v11() != 6;
        short y, x;
        function_2363d4(screen, &x, &y);
        vector3f animation_translation = *(vector3f const *)((byte const *)this + 0x58);
        point3f position;
        position.x = ((s_model_render_definition_2b const *)definition)->position.x - ((s_model_render_definition_2b const *)definition)->distance[0] * animation_translation.i;
        position.y = ((s_model_render_definition_2b const *)definition)->position.y - ((s_model_render_definition_2b const *)definition)->distance[1] * animation_translation.k;
        position.z = ((s_model_render_definition_2b const *)definition)->position.z - ((s_model_render_definition_2b const *)definition)->distance[2] * animation_translation.j;
        vector3f up = { 0.0f, 1.0f, 0.0f };
        vector3f forward = { 0.0f, 0.0f, 1.0f };
        function_30bf0(&up);
        function_30bf0(&forward);
        g_4c0b78.state = (s_2cb30_state *)model_visibility_lists;
        // The second name lookup fills the first list in the pass storage.
        visibility_object_lists_add(extra_indices, extra_count, model_visibility_lists, object_indices, object_count, false);
        function_2cbc0(true);
        function_44720();
        function_2c560((short)(left + x), (short)(top + y), (short)(right + x),
            (short)(bottom + y), &up, angle, &forward, &position);
        if (show_label)
        {
            c_user_interface_text_buffer_32 text;
            real depth = get_depth();
            real alpha = animation.scale;
            word label[16];
            unicode_string_snprintf(label, 16, (word const *)L"%d", value0a);
            c_class_22cc8e *label_text = &text;
            label_text->setup(label, 0, g_46871c, 0, NONE, 0, NONE);
            s_widget_bounds outline;
            outline.left = bounds.left;
            outline.right = bounds.right;
            outline.top = bounds.top;
            outline.bottom = bounds.bottom;
            function_22cd48(label_text, (short_rectangle2d const *)&outline, (short_rectangle2d const *)&outline,
                depth, alpha, screen);
            outline.left += x;
            outline.right += x;
            outline.top += y;
            outline.bottom += y;
            function_235d69((short_rectangle2d const *)&outline, depth, screen, g_4686d8);
        }
    }
}



void function_22f042(s_widget_item *items, c_class_1a2c81 *widget, long count);
// @retail 0x2c8d7c
void c_multiplayer_settings_screen::v3()
{
	c_class_1a2c81::v3();
	s_widget_item item;
	memcpy(item.value48, g_54e5d0.settings.colors, sizeof(item.value48));
	item.value4 = (long)g_54e5d0.settings.name;
	item.flags = 3;
	function_22f042(&item, this, 1);
	c_class_2b0b5e *model = (c_class_2b0b5e *)find_child(7, 0, false);
	// Keep the child pointer addressable across both preview updates.
	c_class_2b0b5e **model_reference = &model;
	function_2b10a3((dword const *)g_54e5d0.settings.colors, *model_reference, 0);
	model = (c_class_2b0b5e *)find_child(7, 1, false);
	function_2b10a3((dword const *)g_54e5d0.settings.colors, *model_reference, 1);
}
