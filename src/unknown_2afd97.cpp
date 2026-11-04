// @flags /O1 /Oi /arch:SSE /Gr
/* UNKNOWN_2AFD97.CPP: the widgets a screen's definition describes besides
   its texts: bitmaps, models, and groups of widgets laid out on a grid */

#include "unknown_11c920.h"
#include <string.h>
#include <stdlib.h>
#include "globals.h"
#include "screen_widgets.h"
#include "unknown_07f720.h"

void function_23625d(long tag_index);
c_class_1a2c81 *function_22fa30(c_class_1a2c81 *parent, s_bitmap_block *definition);
void function_253765(c_text_widget_45a5e0 *widget, short index, s_text_block const *definition);

c_widget_45ad18 *function_2baf5d(c_class_1a2c81 *parent, s_widget_point *point, long index, s_widget_block_18 *definition);

/* the views of 0x2b0a14..0x2b0b19 (unknown_2b116a.cpp) */
struct s_widget_view_2b0a;
long function_2b0a68(s_widget_view_2b0a *widget);
bool function_2b0a48(s_widget_view_2b0a *widget);
bool function_2b12ba(s_widget_view_2b0a *widget);

extern dword g_54d5b8;

#define PIN(value, minimum, maximum) ((value) < (minimum) ? (minimum) : ((value) > (maximum) ? (maximum) : (value)))

struct s_type_7ba8e9
{
	byte unknown00[4];
	short width;
	short height;
	byte unknown08[0x74 - 8];
};

struct s_bitmap_group_view
{
	byte unknown00[0x44];
	long bitmap_count;
	s_type_7ba8e9 *bitmaps;
};

// @retail 0x2b01eb
c_class_2b01eb::c_class_2b01eb(s_bitmap_block *definition) :
	c_class_1a2c81(8, 0),
	definition(definition),
	start_time(g_54d5b8),
	value78(0)
{
	memset(&value7c, 0, 2 * sizeof(real));
	value7c = 0.0f;
	value84 = 0.0f;
	sequence = 0;
	bitmap = 0;
	value80 = 0.0f;
	s_type_7ba8e9 *shown = 0;
	s_bitmap_group_view *volatile group = 0;
	s_widget_bounds bounds;

	value68 = this->definition->value04 - 1;
	function_23625d(this->definition->tag_index);
	if (this->definition->tag_index != NONE)
	{
		s_bitmap_group_view *loaded_group = (s_bitmap_group_view *)g_4e3b44[this->definition->tag_index & 0xffff].bytes;
		bool have_group = loaded_group != NULL;
		group = loaded_group;
		if (have_group)
		{
			shown = &loaded_group->bitmaps[sequence];
		}
	}
	memset(&bounds, 0, sizeof(bounds));
	struct { short bottom; } edge;
	struct scale_test
	{
		static __forceinline bool enabled(s_widget_view_2b0a *widget)
		{ return (byte)function_2b0a68(widget) != 0; }
	};
	if (scale_test::enabled((s_widget_view_2b0a *)this))
	{
		bounds.right = this->definition->x;
		bounds.left = this->definition->x;
		bounds.top = this->definition->y;
		edge.bottom = this->definition->height;
		goto set_bottom;
	}
	else if (shown)
	{
		bounds.top = this->definition->y;
		bounds.left = this->definition->x;
		bounds.right = this->definition->x + shown->width;
		edge.bottom = this->definition->y - shown->height;
		goto set_bottom;
	}
	goto bounds_done;
set_bottom:
	bounds.bottom = edge.bottom;
bounds_done:
	sequence = 0;
	this->bounds = bounds;
	struct { s_bitmap_group_view *value; } checked_group = { group };
	if (checked_group.value && this->definition->sequence >= 0 && this->definition->sequence < checked_group.value->bitmap_count)
	{
		sequence = this->definition->sequence;
	}
	value6a = definition->value20;
}

// @retail 0x2b0b5e
c_class_2b0b5e::c_class_2b0b5e(s_model_block *definition) :
	c_class_1a2c81(7, 0),
	definition(definition)
{
	value68 = definition->value04 - 1;
	bounds = definition->bounds;
	value6a = definition->value08;
}

// @retail 0x2b1111
c_widget_45adf0::c_widget_45adf0(s_widget_block_24 *definition) :
	c_class_1a2c81(9, 0),
	value84(NONE)
{
	this->definition = definition;
	memset(value74, 0, sizeof(value74));
	bounds = this->definition->bounds;
	value68 = definition->value04;
	value6a = definition->value08;
	if (this->definition->tag_index != NONE)
	{
		function_23625d(this->definition->tag_index);
	}
}

// @retail 0x2afd97
c_widget_45ad18::c_widget_45ad18(long index, s_widget_block_18 *definition) :
	c_widget_45c4d0(10, 0),
	index(index),
	definition(definition)
{
}

/* the bounds of the widget's children that show */
// @retail 0x2bafa4
void function_2bafa4(c_class_1a2c81 *widget, s_widget_bounds *bounds)
{
	c_class_1a2c81 *child;

	bounds->left = 0x7fff;
	bounds->right = -0x8000;
	bounds->top = -0x8000;
	bounds->bottom = 0x7fff;
	for (child = widget->child; child; child = child->next)
	{
		bool hidden;

		if (child->type == 8)
		{
			hidden = function_2b0a48((s_widget_view_2b0a *)child);
		}
		else if (child->type == 9)
		{
			hidden = function_2b12ba((s_widget_view_2b0a *)child);
		}
		else
		{
			hidden = false;
		}
		if (!hidden)
		{
			s_widget_bounds child_bounds;

			child->get_bounds(&child_bounds);
			if (child_bounds.left < bounds->left)
			{
				bounds->left = child_bounds.left;
			}
			if (child_bounds.right > bounds->right)
			{
				bounds->right = child_bounds.right;
			}
			if (child_bounds.top > bounds->top)
			{
				bounds->top = child_bounds.top;
			}
			if (child_bounds.bottom < bounds->bottom)
			{
				bounds->bottom = child_bounds.bottom;
			}
		}
	}
	if ((short)(bounds->right - bounds->left) < 0 || (short)(bounds->top - bounds->bottom) < 0)
	{
		memset(bounds, 0, 4);
	}
}

/* a text of a group, for the parent's controller */
// @retail 0x2baeb1
c_text_widget_45a5e0 *function_2baeb1(c_class_1a2c81 *parent, long index, s_text_block *definition)
{
	c_text_widget_45a5e0 *text;

	if (definition->flags & 0x10)
	{
		text = new c_text_widget_32(parent->get_controller_index());
	}
	else
	{
		text = new c_text_widget_458940(parent->get_controller_index());
	}
	if (text)
	{
		text->m6c = true;
		parent->add_child(text);
		function_253765(text, (short)index, definition);
		text->function_22f52e()->set_text((word *)L"");
	}
	return text;
}

// @retail 0x2baf38
c_widget_45adf0 *function_2baf38(c_class_1a2c81 *parent, s_widget_block_24 *definition)
{
	c_widget_45adf0 *widget = new c_widget_45adf0(definition);

	if (widget)
	{
		widget->m6c = true;
		parent->add_child(widget);
	}
	return widget;
}

long function_2b014f(s_widget_item *item);
long function_2b015b(s_widget_item *item);
long function_2b016d(s_widget_item *item);
bool function_2b0180(s_widget_item *item);
bool function_2b0191(s_widget_item *item);
long function_2b01c7(s_widget_item *item);
long function_2b01da(s_widget_item *item);
void function_2b0a14(s_widget_view_2b0a *widget, short index);
void function_2b12ca(s_widget_view_2b0a *widget, short b, short a, void const *bounds);
hsv3f *function_1318d0(color3f const *rgb, hsv3f *hsv);
color3f *function_131a00(hsv3f const *hsv, color3f *rgb);

/* Applies an item's optional text, appearance and bitmap fields to the
   children of its group widget. */
// @retail 0x2afeae
void function_2afeae(s_widget_item *item, c_class_1a2c81 *widget)
{
	long tag_index = ((c_widget_45ad18 *)widget)->definition->tag_index;
	if (tag_index != NONE)
	{
		s_widget_group_definition *definition = (s_widget_group_definition *)g_4e3b44[tag_index & 0xffff].bytes;
		long team_index = function_2b016d(item);
		word *text_address = (word *)function_2b014f(item);
		s_player_appearance const *appearance = TEST_FIELD_BIT(item->unknown1) ? (s_player_appearance const *)item->value48 : NULL;
		long text_id = function_2b015b(item);
		word *text = TEST_FIELD_BIT(item->unknown4) ? (word *)item->unknown08 : NULL;
		byte shown = function_2b0191(item);
		long bitmap_index = function_2b01c7(item);
		long frame_count = function_2b01da(item);
		color3f colors[4];

		if (appearance)
		{
			function_7f790((short)team_index, function_2b0180(item), appearance, colors);
		}
		else
		{
			color3f *color = colors;
			long count = 4;
			do
			{
				*color = *(color3f const *)g_468710;
				color++;
			} while (--count);
			if ((short)team_index != NONE)
			{
				colors[0] = function_7f720((short)team_index);
			}
		}

		for (widget = widget->child; widget; widget = widget->next)
		{
			switch (widget->type)
			{
			case 9:
				if (appearance)
					function_2b12ca((s_widget_view_2b0a *)widget, (short)team_index, (short)bitmap_index, appearance);
				else
					widget->value6e = false;
				break;

			case 8:
				if (widget->value0a != NONE)
				{
					long id = *(long *)definition->bitmaps[widget->value0a].unknown2c;
					switch (id)
					{
					case 0x1400069a:
						if (appearance || (short)team_index != NONE)
						{
							long frame = 0;
							if (frame_count > 1)
								frame = frame_count - 1;
							widget->color = colors[0];
							widget->value6e = true;
							function_2b0a14((s_widget_view_2b0a *)widget, (short)frame);
						}
						else
							widget->value6e = false;
						break;
					case 0x0900069c:
						if ((short)bitmap_index != NONE)
						{
							function_2b0a14((s_widget_view_2b0a *)widget, (short)bitmap_index);
							widget->value6e = true;
						}
						else
							widget->value6e = false;
						break;
					case 0x0e00069b:
						widget->value6e = shown != 0;
						break;
					}
				}
				break;

			case 6:
				if (widget->value0a != NONE)
				{
					long id = definition->texts[widget->value0a].string_handle;
					switch (id)
					{
					case 0x0e00069b:
						widget->value6e = shown != 0;
						break;
					case 0x0b000698:
					{
						if (text_id == NONE && !text && !text_address)
						{
							widget->value6e = false;
							break;
						}
						if (text_id != NONE)
							((c_text_widget_45a5e0 *)widget)->function_253b1a(text_id);
						else if (text)
							widget->function_22f52e()->set_text(text);
						else
							widget->function_22f52e()->set_text((word *)text_address);

						color3f color;
						if (TEST_FIELD_BIT(item->has_color))
							color = item->color;
						else
						{
							hsv3f hsv;
							color = colors[0];
							function_1318d0(&color, &hsv);
							hsv.saturation = 0.20833333f;
							hsv.value = 0.7916667f;
							function_131a00(&hsv, &color);
						}
						widget->color = color;
						widget->value6e = true;
						break;
					}
					}
				}
				break;
			}
		}
	}
}

/* places a group's widgets at the point */
// @retail 0x2bacbc
void function_2bacbc(c_class_1a2c81 *widget, s_widget_group_definition *group, s_widget_point *point)
{
	long i;

	for (i = 0; i < group->bitmap_count; i++)
	{
		c_class_1a2c81 *bitmap = function_22fa30(widget, &group->bitmaps[i]);

		if (bitmap)
		{
			short height;

			s_widget_bounds bounds;
   bitmap->get_bounds(&bounds);
			height = (short)(bounds.bottom - bounds.top >= 0 ? bounds.bottom - bounds.top : bounds.top - bounds.bottom);
			bounds.top += height;
			bounds.bottom += height;
			bounds.top += point->y;
			bounds.left += point->x;
			bounds.right += point->x;
			bounds.bottom += point->y;
			bitmap->bounds = bounds;
			bitmap->value0a = (short)i;
		}
	}
	for (i = 0; i < group->block_24_count; i++)
	{
		c_class_1a2c81 *child = function_2baf38(widget, &group->blocks_24[i]);

		if (child)
		{
			short height;

			s_widget_bounds bounds;
   child->get_bounds(&bounds);
			height = (short)(bounds.bottom - bounds.top >= 0 ? bounds.bottom - bounds.top : bounds.top - bounds.bottom);
			bounds.top += height;
			bounds.bottom += height;
			bounds.top += point->y;
			bounds.left += point->x;
			bounds.right += point->x;
			bounds.bottom += point->y;
			child->bounds = bounds;
			child->value0a = (short)i;
		}
	}
	for (i = 0; i < group->block_18_count; i++)
	{
		s_widget_block_18 *block = &group->blocks_18[i];
		long j;

		for (j = 0; j < block->count; j++)
		{
			c_class_1a2c81 *child = function_2baf5d(widget, point, j, block);

			if (child)
			{
				s_widget_bounds child_bounds;
				short height;

				child->get_bounds(&child_bounds);
				height = (short)(child_bounds.bottom - child_bounds.top >= 0 ? child_bounds.bottom - child_bounds.top : child_bounds.top - child_bounds.bottom);
				child_bounds.top += height;
				child_bounds.bottom += height;
				child->value0a = (short)i;
			}
		}
	}
	for (i = 0; i < group->text_count; i++)
	{
		c_text_widget_45a5e0 *text = function_2baeb1(widget, i, &group->texts[i]);

		if (text)
		{
			s_widget_bounds bounds;
   text->get_bounds(&bounds);
			bounds.top += point->y;
			bounds.left += point->x;
			bounds.right += point->x;
			bounds.bottom += point->y;
			text->bounds = bounds;
		}
	}
}

/* places the widget's group in its cell of the grid */
// @retail 0x2afdbc
void c_widget_45ad18::place(s_widget_point *origin)
{
	if (definition->tag_index != NONE)
	{
		s_widget_group_definition *group = (s_widget_group_definition *)g_4e3b44[definition->tag_index & 0xffff].bytes;
		long columns;
		long rows;

		if (definition->rows > 1)
			rows = definition->rows;
		else
			rows = 1;
		if (definition->columns > 1)
			columns = definition->columns;
		else
			columns = 1;
		long column;
		long row;

		switch (definition->order)
		{
		case 0:
			row = index / columns;
			column = index % columns;
			break;
		case 1:
			column = index / rows;
			row = index % rows;
			break;
		default:
			__assume(0);
		}
		if (PIN(column, 0, columns - 1) == column && PIN(row, 0, rows - 1) == row)
		{
			s_widget_point point;
			s_widget_bounds group_bounds;

			point = *(s_widget_point *)((byte *)definition + 0xc);
			point.x += definition->x_step * (short)column + origin->x;
			point.y += definition->y_step * (short)row + origin->y;
			function_2bacbc(this, group, &point);
			function_2bafa4(this, &group_bounds);
			bounds = group_bounds;
		}
	}
}

// @retail 0x2baf5d
c_widget_45ad18 *function_2baf5d(c_class_1a2c81 *parent, s_widget_point *point, long index, s_widget_block_18 *definition)
{
	c_widget_45ad18 *widget = new c_widget_45ad18(index, definition);

	if (widget)
	{
		widget->m6c = true;
		parent->add_child(widget);
		widget->place(point);
	}
	return widget;
}
