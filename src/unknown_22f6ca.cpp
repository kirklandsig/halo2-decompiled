// @flags /O1 /Oi /arch:SSE /Gr
/* UNKNOWN_22F6CA.CPP: building a screen from its definition:
   its title and subtitle, its panes (their buttons, list, texts, bitmaps,
   models and widget groups) and the widgets of its widget set */

#include "unknown_11c920.h"
#include <string.h>
#include "screen_widgets.h"
#include "unknown_19b516.h"
#include "unknown_234c64.h"
#include "unknown_2312b4.h"

s_screen_definition *function_22f871(c_class_1473c9 *screen);
void function_08cc20(struct s_name_buffer *buffer, const wchar_t *name);
void function_253765(c_text_widget_45a5e0 *widget, short index, s_text_block const *definition);
void function_2538a6(c_text_widget_45a5e0 *widget, short value04, short font, short flags, color4f const *color, s_widget_bounds const *bounds);
void function_253bc9(c_text_widget_458940 *widget, long subtitle_type);
void function_253cff(c_class_19b8b1 *button);
void function_24bc12(c_class_1474e8 *list, bool remove_extra);
c_widget_45adf0 *function_2baf38(c_class_1a2c81 *parent, s_widget_block_24 *definition);

/* ---- the widgets a pane holds ---- */

// @retail 0x22f9bf
c_text_widget_45a5e0 *function_22f9bf(c_class_1473c9 *screen, long index, s_text_block *definition)
{
	c_text_widget_45a5e0 *text;

	if (definition->flags & 0x10)
	{
		text = new c_text_widget_32(screen->user_flags);
	}
	else
	{
		text = new c_text_widget_458940(screen->user_flags);
	}
	if (text)
	{
		text->m6c = true;
		screen->add_child(text);
		function_253765(text, (short)index, definition);
	}
	return text;
}

// @retail 0x22fa30
c_class_1a2c81 *function_22fa30(c_class_1a2c81 *parent, s_bitmap_block *definition)
{
	c_class_2b01eb *bitmap = new c_class_2b01eb(definition);

	if (bitmap)
	{
		bitmap->m6c = true;
		parent->add_child(bitmap);
	}
	return bitmap;
}

// @retail 0x22fa5b
c_class_1a2c81 *function_22fa5b(c_class_1a2c81 *parent, s_model_block *definition)
{
	c_class_2b0b5e *model = new c_class_2b0b5e(definition);

	if (model)
	{
		model->m6c = true;
		parent->add_child(model);
	}
	return model;
}

// @retail 0x22fa7d
c_widget_45ad18 *function_22fa7d(c_class_1473c9 *screen, long index, s_widget_block_18 *definition)
{
	c_widget_45ad18 *widget = new c_widget_45ad18(index, definition);

	if (widget)
	{
		s_widget_point origin;

		widget->m6c = true;
		screen->add_child(widget);
		memset(&origin, 0, sizeof(origin));
		widget->place(&origin);
	}
	return widget;
}

/* removes and deletes the screen's own widgets (not its title and
   subtitle), before its pane is built again */
// @retail 0x22fba9
void function_22fba9(c_class_1473c9 *screen)
{
	c_class_1a2c81 *widget = screen->child;

	while (widget)
	{
		c_class_1a2c81 *next_widget = widget->next;

		if (widget->type > 1 && widget->type > 5 &&
			(widget->type != 6 || widget != &screen->title && widget != &screen->subtitle))
		{
			screen->remove_child(widget);
			widget->v2();
			if (widget->m6c)
			{
				widget->~c_class_1a2c81();
				function_1a4826(widget);
			}
		}
		widget = next_widget;
	}
}

/* the widgets of the current pane: bitmaps, groups, texts and models */
// score 23 (was 66): the text loop's offset reset is mov where retail has and, the model loop's
// the other way, and the model loop copies the screen to eax before reading its animation type
// (retail uses esi, the call's register argument, directly; both copy it in the text loop).
// Tried: the text definition as pointer arithmetic, the text loop as a while, a model
// definition local, a named animation type, the model helper taking the screen type: all 23.
// Round 45: own counters for the text loop or the model loop, the text result typed as the
// helper's return type, the text test nested, the inner counter declared with i: all 23.
// Round 11 (lane 18): the four alike loop bodies (bitmaps, block-24, block-18, models) through one
// shared inline helper 187 (kept out of line under /O1), as __forceinline 23 (identical code); the
// block-18 element read at each use (no block pointer) 61; `long i = 0;` declared before the pane
// lookup 25 (the text loop's reset becomes retail's `and`, but the zero moves above the pane call),
// with the model loop's own counter as well 25; the text result declared once before its loop 23;
// `animating` declared before the pane test 23.
// @retail 0x22fc08
void __stdcall function_22fc08(c_class_1473c9 *screen)
{
	s_screen_pane *pane = screen->get_current_pane();
	long i = 0;

	if (pane)
	{
		bool animating = function_22f0ff((c_widget *)screen);

		for (; i < pane->bitmap_count; i++)
		{
			c_class_1a2c81 *bitmap = function_22fa30(screen, &pane->bitmaps[i]);

			if (bitmap)
			{
				if (animating)
				{
					bitmap->start_animation(screen->animation.type);
				}
				bitmap->value0a = (short)i;
			}
		}
		for (i = 0; i < pane->block_24_count; i++)
		{
			c_class_1a2c81 *widget = function_2baf38(screen, &pane->blocks_24[i]);

			if (widget)
			{
				if (animating)
				{
					widget->start_animation(screen->animation.type);
				}
				widget->value0a = (short)i;
			}
		}
		for (i = 0; i < pane->block_18_count; i++)
		{
			s_widget_block_18 *block = &pane->blocks_18[i];
			long j;

			for (j = 0; j < block->count; j++)
			{
				c_class_1a2c81 *widget = function_22fa7d(screen, j, block);

				if (widget)
				{
					if (animating)
					{
						widget->start_animation(screen->animation.type);
					}
					widget->value0a = (short)i;
				}
			}
		}
		for (i = 0; i < pane->text_count; i++)
		{
			c_class_1a2c81 *text = function_22f9bf(screen, i, &pane->texts[i]);

			if (text && animating)
			{
				text->start_animation(screen->animation.type);
			}
		}
		for (i = 0; i < pane->model_count; i++)
		{
			c_class_1a2c81 *model = function_22fa5b(screen, &pane->models[i]);

			if (model)
			{
				if (animating)
				{
					model->start_animation(screen->animation.type);
				}
				model->value0a = (short)i;
			}
		}
	}
}

/* the models and bitmaps of a widget set of the user interface globals */
// @retail 0x22facd
void function_22facd(c_class_1473c9 *screen, short set_index)
{
	bool animating = function_22f0ff((c_widget *)screen);

	if (set_index >= 0 && set_index < 0x20)
	{
		s_type_954545 *globals = ui_definition_get();

		if (globals && set_index < globals->widget_set_count)
		{
			s_widget_set *set = &globals->widget_sets[set_index];
			long i;

			for (i = 0; i < set->model_count; i++)
			{
				c_class_1a2c81 *model = function_22fa5b(screen, &set->models[i]);

				if (model && animating)
				{
					model->start_animation(screen->animation.type);
				}
			}
			for (i = 0; i < set->bitmap_count; i++)
			{
				c_class_1a2c81 *bitmap = function_22fa30(screen, &set->bitmaps[i]);

				if (bitmap)
				{
					if (animating)
					{
						bitmap->start_animation(screen->animation.type);
					}
					bitmap->value0a = (short)-i;
				}
			}
		}
	}
}

/* ---- the panes ---- */

/* builds the screen's current pane: its buttons or its list, then the rest
   of its widgets */
// @retail 0x22f6ca
void function_22f6ca(c_class_1473c9 *screen, s_screen_layout *layout, bool rebuild)
{
	if (rebuild || (screen->value5f0 >= 0 && screen->value5f0 < layout->count))
	{
		long i;
		s_screen_definition *definition = function_22f871(screen);
		s_screen_pane *pane = screen->get_current_pane();

		if (definition && pane)
		{
			screen->value68 = pane->value02 - 1;
			if (rebuild)
			{
				for (i = 0; i < pane->button_count; i++)
				{
					c_class_1a2c81 *button = layout->lists[screen->value5f0].widget[i];

					if (!button)
					{
						break;
					}
					screen->add_child(button);
					function_253cff((c_class_19b8b1 *)layout->lists[screen->value5f0].widget[i]);
				}
				if (pane->list_count > 0)
				{
					screen->add_child(layout->lists[screen->value5f0].list);
					function_24bc12(layout->lists[screen->value5f0].list, rebuild);
					goto configure_list;
				}
			}
			else if (layout->lists[screen->value5f0].type > 0)
			{
				long count = layout->lists[screen->value5f0].type;

				if (pane->button_count <= count)
				{
					count = pane->button_count;
				}
				for (i = 0; i < count; i++)
				{
					screen->add_child(layout->lists[screen->value5f0].widget[i]);
					function_253cff((c_class_19b8b1 *)layout->lists[screen->value5f0].widget[i]);
				}
			}
			else if (layout->lists[screen->value5f0].list)
			{
				screen->add_child(layout->lists[screen->value5f0].list);
				function_24bc12(layout->lists[screen->value5f0].list, false);
configure_list:
				if (definition->flag1)
				{
					layout->lists[screen->value5f0].list->notify_screen = true;
					s_list_head *head = &layout->lists[screen->value5f0].list->head80;
					s_list_head *const *head_reference = &head;
					delegate_register(*head_reference, (c_list_item_delegate *)&screen->delegate);
				}
			}
			function_22fc08(screen);
		}
	}
}

/* the title: its text and size come from the definition */
// @retail 0x22fda6
void __stdcall function_22fda6(c_class_1473c9 *screen)
{
	s_type_954545 *globals = ui_definition_get();
	s_screen_definition *definition = function_22f871(screen);
	word name[0x100];
	short pane_value;
	color4f color;
	long font;
	s_widget_bounds bounds;

	name[0] = 0;
	pane_value = screen->get_first_pane_value();
	color.alpha = 1.0f;
	color.red = 0.7f;
	color.green = 0.7f;
	color.blue = 0.7f;
	font = 1;
	if (definition ? (bool)(definition->no_title == 0) : true)
	{
		bounds.left = -100;
		bounds.right = 100;
		bounds.top = 100;
		bounds.bottom = 80;
		screen->add_child(&screen->title);
		if (globals)
		{
			color = globals->title_color;
			if (definition)
			{
				if (definition->flags & 1)
				{
					font = globals->title_fonts[3];
					bounds = globals->title_bounds[3].title;
				}
				else if (definition->flags & 8)
				{
					font = globals->title_fonts[2];
					bounds = globals->title_bounds[2].title;
				}
				else if (definition->flags & 0x10)
				{
					font = globals->title_fonts[1];
					bounds = globals->title_bounds[1].title;
				}
				else
				{
					font = globals->title_fonts[0];
					bounds = globals->title_bounds[0].title;
				}
				screen->title.function_253b1a(definition->title_string_id);
				function_08cc20((s_name_buffer *)name, (const wchar_t *)screen->title.function_22f52e()->function_22f52e());
			}
		}
		function_2538a6(&screen->title, pane_value, (short)font, 1, &color, &bounds);
		c_class_22cc8e *text = screen->title.function_22f52e();
		c_class_22cc8e *const *text_reference = &text;
		(*text_reference)->value14 |= 2;
		screen->title.function_22f52e()->set_text(name);
		screen->title.value6a = 0x7fff;
	}
}

/* the subtitle: the user's gamertag */
// @retail 0x22ff53
void __stdcall function_22ff53(c_class_1473c9 *screen)
{
	s_type_954545 *globals = ui_definition_get();
	s_screen_definition *definition = function_22f871(screen);
	short pane_value = screen->get_first_pane_value();
	word name[0x100];
	color4f color;
	s_widget_bounds bounds;

	name[0] = 0;
	color.alpha = 1.0f;
	color.red = 0.7f;
	color.green = 0.7f;
	color.blue = 0.7f;
	wcsncpy((wchar_t *)name, L"", 0xff);
	name[0xff] = 0;
	bounds.left = -100;
	bounds.right = 100;
	bounds.top = -100;
	bounds.bottom = -120;
	screen->add_child(&screen->subtitle);
	if (globals)
	{
		color = globals->title_color;
		if (definition)
		{
			long subtitle_type = definition->value06;
			color = definition->subtitle_color;
			if (definition->flags & 1)
			{
				bounds = globals->title_bounds[3].subtitle;
			}
			else if (definition->flags & 8)
			{
				bounds = globals->title_bounds[2].subtitle;
			}
			else if (definition->flags & 0x10)
			{
				bounds = globals->title_bounds[1].subtitle;
			}
			else
			{
				bounds = globals->title_bounds[0].subtitle;
			}
			function_253bc9(&screen->subtitle, subtitle_type);
			function_08cc20((s_name_buffer *)name, (const wchar_t *)screen->subtitle.function_22f52e()->function_22f52e());
		}
	}
	function_2538a6(&screen->subtitle, pane_value, 1, 2, &color, &bounds);
	screen->subtitle.function_22f52e()->set_text(name);
	screen->subtitle.value6a = 0x7fff;
}

/* builds the screen from its definition: its title and subtitle, its panes
   (each pane a child of the layout's container when there are several) and
   its widget set */
// @retail 0x22f8df
void c_class_1473c9::build(s_screen_layout *layout)
{
	s_screen_definition *definition = function_22f871(this);

	function_22fda6(this);
	function_22ff53(this);
	if (definition)
	{
		bool rebuild = layout->count == NONE;

		value5f4 = TEST_FIELD_BIT(definition->flag5);
		if (definition->pane_count > 0)
		{
			s_screen_pane *first_pane = definition->panes;

			if (definition->pane_count > 1 && layout->container)
			{
				c_class_1473c9 *pane = (c_class_1473c9 *)layout->container->child;
				short i;

				value5f0 = 0;
				value68 = first_pane->value02 - 1;
				add_child(layout->container);
				for (i = 0; i < definition->pane_count; i++)
				{
					pane->value5f0 = i;
					function_22f6ca(pane, layout, rebuild);
					pane = (c_class_1473c9 *)pane->next;
				}
			}
			else if (definition->pane_count == 1 || definition->flag1)
			{
				value5f0 = 0;
				function_22f6ca(this, layout, rebuild);
			}
			function_22facd(this, definition->widget_set - 1);
		}
	}
}
