// @flags /O1 /Oi /arch:SSE /Gr
/* UNKNOWN_22E27B.CPP: the user interface widget base class (vtable 0x458788)
   and the intrusive lists the widgets keep their delegates in */

#include "unknown_11c920.h"
#include <string.h>
#include <wchar.h>
#include "screen_widgets.h"
#include "unknown_19b516.h"
#include "unknown_234c64.h"
#include "unknown_030290.h"
#include "globals.h"

void unicode_string_copy(word *destination, const word *source, long maximum_count);
void function_22d2ee(word *string, long maximum_length);
void function_13ed90(long font);
void function_2360c3(point3f *point, short_rectangle2d const *bounds);
void function_13e9c0(word const *text, short_rectangle2d const *bounds,
	short_rectangle2d *a, short_rectangle2d *b, real scale);

// @retail 0x22d13d
void function_22d13d(c_class_22cc8e *text, short_rectangle2d const *bounds,
	real depth, short_rectangle2d const *screen, short length,
	short_rectangle2d *ink_bounds, short_rectangle2d *output)
{
	word buffer[0x400];
	word *string = text->function_22f52e();
	if (string && *string)
		unicode_string_copy(buffer, string, 0x400);
	else
		buffer[0] = 0;
	buffer[length] = 0;
	function_22d2ee(buffer, 0x400);
	function_13ed90(text->value04);
	long flags = *(volatile short *)&text->value16;
	long justification = *(volatile long *)&text->value1c;
	long style = *(volatile long *)&text->value18;
	g_4e73a0.flags = flags;
	g_4e73a0.justification = justification;
	g_4e73a0.style = style;
	point3f first, last;
	first.x = (real)bounds->left;
	first.y = (real)bounds->top;
	first.z = depth;
	last.x = (real)bounds->right;
	last.y = (real)bounds->bottom;
	last.z = depth;
	long width = bounds->right - bounds->left;
	function_2360c3(&first, screen);
	function_2360c3(&last, screen);
	short_rectangle2d projected, measured;
	projected.top = (short)first.y;
	long left = (long)first.x;
	projected.left = (short)left;
	projected.right = (short)last.x;
	projected.bottom = (short)last.y;
	function_13e9c0(buffer, &projected, &measured, ink_bounds,
		(last.x - first.x) * text->value20 / (real)width);
	output->top = projected.top;
	output->left = (short)left;
	output->right = measured.right;
	output->bottom = measured.bottom;
}

extern dword g_54d5b8;

struct s_name_buffer;
void function_08cc20(s_name_buffer *buffer, const wchar_t *name);

/* the bounds every screen starts with */
struct s_screen_bounds
{
	short bounds[4];
};

/* unknown_0167a0.cpp: the screen rectangle, then the bounds at 0x485a92 */
struct short_rect
{
	short v0, v1, v2, v3;
};

struct short_rect_pair
{
	short_rect a, b;
};

extern short_rect_pair g_485a8a;

// @retail 0x22e27b
c_class_1a2c81::c_class_1a2c81(long type, word user_flags)
{
	s_type_0cfb31 animation;

	value0a = NONE;
	value0c = NONE;
	this->type = type;
	this->user_flags = user_flags;
	parent = 0;
	child = 0;
	next = 0;
	previous = 0;
	memset(&bounds, 0, sizeof(bounds));
	memset(&color, 0, sizeof(color));
	value68 = NONE;
	value6a = 0;
	m6c = false;
	value6d = true;
	value6e = true;
	memset(&bounds, 0, sizeof(bounds));
	color.red = 1.0f;
	color.blue = 1.0f;
	color.green = 1.0f;

	memset(&animation, 0, sizeof(animation));
	animation.type = NONE;
	animation.direction = 1;
	animation.value14 = 0;
	set_animation(&animation);
}

// @retail 0x2bac9a deleting
c_class_1a2c81::~c_class_1a2c81()
{
	delete_children();
}

// @retail 0x22e391
void c_class_1a2c81::v3()
{
	update(g_54d5b8);
	for (c_class_1a2c81 *widget = child; widget; widget = widget->next)
	{
		widget->v3();
	}
}

void function_148119(c_class_1473c9 *screen);

/* a key of a widget animation: its scale and offset */
struct s_widget_animation_key
{
	long unknown00;
	real scale;
	point3f offset;
};

/* steps the widget's animation to this time: frames advance once per frame
   time and wrap, bounce or stop at the ends by the animation's mode; the
   offset and scale blend between the current frame and the next. A screen
   whose animation ends may be disposed of or restarted. */
static inline void function_x697631(point3f const *point, vector3f const *vector, real t, point3f *result)
{
	result->x = vector->i * t + point->x;
	result->y = vector->j * t + point->y;
	result->z = vector->k * t + point->z;
}

static inline real interpolate_linear(real a, real b, real t)
{
	return (b - a) * t + a;
}

// @retail 0x22e3cd
void c_class_1a2c81::update(dword time)
{
	bool restart = false;
	bool dispose;

	if (animation.value8 > 0 && animation.value20 > 0 && animation.end_time <= g_54d5b8)
	{
		long elapsed;
		dword next_time;
		long frame_time;

		frame_time = animation.value20 / (animation.value8 - 1);
		elapsed = time - animation.end_time;
		next_time = animation.end_time + frame_time;
		dispose = false;

		while (time >= next_time)
		{
			short frame = animation.valuea + animation.direction;

			dispose = false;
			if (frame < 0)
			{
				dispose = (animation.valuee & 2) && type == 0;
				if (dispose)
				{
					break;
				}
				restart = type == 0 && ((animation.valuee & 1) || (animation.valuee & 2));
				switch (animation.value14)
				{
				case 0:
					frame = animation.valuea;
					break;
				case 1:
					frame = animation.value8 - 1;
					break;
				case 2:
					frame = animation.valuea;
					break;
				default:
					animation.direction = -animation.direction;
					frame = animation.valuea;
					break;
				}
			}
			else if (frame == animation.value8)
			{
				dispose = (animation.valuee & 2) && type == 0;
				if (dispose)
				{
					break;
				}
				restart = type == 0 && ((animation.valuee & 1) || (animation.valuee & 2));
				switch (animation.value14)
				{
				case 0:
					frame = animation.valuea;
					break;
				case 1:
					frame = 0;
					break;
				case 2:
					animation.direction = -animation.direction;
					frame = animation.valuea;
					break;
				default:
					animation.direction = -animation.direction;
					frame = animation.valuea;
					break;
				}
			}
			if (restart)
			{
				break;
			}
			if (!(animation.valuee & 8))
			{
				animation.valuea = frame;
			}
			next_time += frame_time;
			elapsed -= frame_time;
			animation.end_time = time;
		}

		if (!dispose)
		{
			short next = animation.valuea + animation.direction;
			s_widget_animation_key *keys;
			s_widget_animation_key *current;
			s_widget_animation_key *following;
			vector3f delta;
			real t;

			if (next < 0)
			{
				switch (animation.value14)
				{
				case 0:
					next = animation.valuea;
					break;
				case 1:
					animation.valuea = animation.value8 - 1;
					next = animation.valuea - 1;
					break;
				case 2:
					next = animation.valuea;
					break;
				default:
					animation.direction = -animation.direction;
					next = animation.valuea + animation.direction;
					break;
				}
			}
			else if (next == animation.value8)
			{
				switch (animation.value14)
				{
				case 0:
					next = animation.valuea;
					break;
				case 1:
					animation.valuea = 0;
					next = 1;
					break;
				case 2:
					animation.direction = -animation.direction;
					next = animation.valuea + animation.direction;
					break;
				default:
					animation.direction = -animation.direction;
					next = animation.valuea + animation.direction;
					break;
				}
			}
			if (elapsed < 1)
			{
				elapsed = 1;
			}
			t = (real)elapsed / (real)frame_time;
			keys = (s_widget_animation_key *)animation.target;
			vector3d_from_points3d(&keys[animation.valuea].offset, &keys[next].offset, &delta);
			function_x697631(&keys[animation.valuea].offset, &delta, t, &animation.offset);
			keys = (s_widget_animation_key *)animation.target;
			animation.scale = (keys[next].scale - keys[animation.valuea].scale) * t + keys[animation.valuea].scale;
		}
	}
	else
	{
		dispose = (animation.valuee & 2) && type == 0;
		restart = type == 0 && ((animation.valuee & 1) || (animation.valuee & 2));
	}
	if (dispose && get_screen() != this)
	{
		dispose = false;
	}
	if (dispose)
	{
		function_148119((c_class_1473c9 *)this);
	}
	else if (restart)
	{
		start_animation(4);
	}
}

// @retail 0x22e34b
void c_class_1a2c81::delete_children()
{
	c_class_1a2c81 *widget = child;

	child = 0;
	while (widget)
	{
		c_class_1a2c81 *next_widget = widget->next;

		widget->delete_children();
		if (widget->m6c)
		{
			widget->~c_class_1a2c81();
			function_1a4826(widget);
		}
		widget = next_widget;
	}
}

// @retail 0x22e89c
void c_class_1a2c81::set_animation(s_type_0cfb31 *definition)
{
	long time = g_54d598.m20;
	long direction;

	animation.type = definition->type;
	animation.target = definition->target;
	animation.value8 = definition->value8;
	animation.valuea = definition->valuea;
	if (definition->direction)
	{
		direction = definition->direction >= 0 ? 1 : -1;
	}
	else
	{
		direction = 0;
	}
	animation.direction = direction >= 0 ? 1 : -1;
	animation.valuee = definition->valuee;
	animation.duration = definition->duration;
	animation.value14 = definition->value14;
	animation.start_time = time;
	animation.end_time = definition->duration + time;
	animation.value20 = definition->value20;
	animation.offset.z = 0.0f;
	animation.offset.y = 0.0f;
	animation.offset.x = 0.0f;
	animation.scale = 1.0f;
}

__declspec(noinline)
// @retail 0x22edb8
c_class_1a2c81 *c_class_1a2c81::find_child(long type, short index, bool recursive)
{
	c_class_1a2c81 *result = 0;
	c_class_1a2c81 *widget;
	short original_index = index;

	if (index >= 0)
	{
		for (widget = child; widget; widget = widget->next)
		{
			if (widget->type == type && index-- == 0)
			{
				return widget;
			}
		}
	}
	if (recursive)
	{
		for (widget = child; widget; widget = widget->next)
		{
			result = widget->find_child(type, (short)original_index, recursive);
			if (result)
			{
				break;
			}
		}
	}
	return result;
}

/* the screen at the top of the parent chain (a screen is its own) */
// @retail 0x22eeee
c_class_1473c9 *c_class_1a2c81::get_screen()
{
	c_class_1a2c81 *widget = parent;

	while (widget && widget->parent)
	{
		widget = widget->parent;
	}
	if (!widget && type == 0)
	{
		widget = this;
	}
	return (c_class_1473c9 *)widget;
}

// @retail 0x22ec84
bool c_class_1a2c81::has_screen()
{
	return get_screen() != 0;
}

c_class_1473c9 *function_148d91(long channel, long index);

/* whether this widget is in the screen its window shows */
// @retail 0x22ed7a
bool c_class_1a2c81::is_in_window()
{
	bool result = false;

	if (has_screen())
	{
		for (c_class_1a2c81 *widget = function_148d91(v11(), v12()); widget; widget = widget->parent)
		{
			if (this == widget)
			{
				result = true;
				break;
			}
		}
	}
	return result;
}

void function_148dfc(long channel, long index, c_class_1473c9 *screen);

/* focuses the widget in its window (or gives the focus back to its parent);
   the widgets that were focused and are no longer lose the focus */
// @retail 0x22ecb4
void c_class_1a2c81::function_22ecb4_2(bool focus)
{
	c_class_1a2c81 *previous;

	if (focus)
	{
		previous = function_148d91(v11(), v12());
		function_148dfc(v11(), v12(), (c_class_1473c9 *)this);
		v13();
	}
	else
	{
		if (!is_in_window())
		{
			return;
		}
		previous = function_148d91(v11(), v12());
		if (parent)
		{
			function_148dfc(v11(), v12(), (c_class_1473c9 *)parent);
		}
		else
		{
			function_148dfc(v11(), v12(), 0);
		}
	}
	while (previous)
	{
		if (previous->is_in_window())
		{
			break;
		}
		previous->v14();
		previous = previous->parent;
	}
}

// @retail 0x22e315
void c_class_1a2c81::v1()
{
	value0c = new_widget_id();
	for (c_class_1a2c81 *widget = child; widget; widget = widget->next)
	{
		widget->v1();
	}
}

// @retail 0x22e335
void c_class_1a2c81::v2()
{
	for (c_class_1a2c81 *widget = child; widget; widget = widget->next)
	{
		widget->v2();
	}
}

// @retail 0x22e37f
bool c_class_1a2c81::has_valid_type()
{
	if (type >= 0 && type <= 5)
	{
		return true;
	}
	return false;
}

/* the widget's depth */
// @retail 0x22e9aa
real c_class_1a2c81::get_depth()
{
	point3f offset = animation.offset;

	return offset.z;
}

/* the widget's bounds, moved by its animation */
// @retail 0x22e9c6
void c_class_1a2c81::get_bounds(s_widget_bounds *result)
{
	point3f offset;

	*result = bounds;
	offset = animation.offset;
	short y = (short)offset.y;
	short x = (short)offset.x;
	result->left += x;
	result->right += x;
	result->top += y;
	result->bottom += y;
}

// @retail 0x22e9ff
void c_class_1a2c81::get_real_bounds(box2f *result)
{
	point3f offset;

	result->x0 = bounds.left;
	result->x1 = bounds.right;
	result->y0 = bounds.top;
	result->y1 = bounds.bottom;
	offset = animation.offset;
	result->x0 += offset.x;
	result->y0 += offset.y;
	result->x1 += offset.x;
	result->y1 += offset.y;
}

/* the widget's bounds, moved by its animation, on the screen of a window
   with these bounds */
// @retail 0x22ea81
s_float_rect *function_22ea81(c_class_1a2c81 *widget, s_float_rect *rect, short_rectangle2d const *frame)
{
	point3f offset;

	rect->x0 = widget->bounds.left;
	rect->y0 = widget->bounds.top;
	rect->x1 = widget->bounds.right;
	rect->y1 = widget->bounds.bottom;
	offset = widget->animation.offset;
	rect->x0 += offset.x;
	rect->y0 += offset.y;
	rect->x1 += offset.x;
	rect->y1 += offset.y;
	function_23618e(rect, offset.z, frame);
	return rect;
}

// @retail 0x22eb18
void c_class_1a2c81::add_child(c_class_1a2c81 *widget)
{
	if (widget->parent)
	{
		widget->parent->remove_child(widget);
	}
	if (child)
	{
		c_class_1a2c81 *last = child;
		while (last->next)
		{
			last = last->next;
		}
		last->next = widget;
		widget->previous = last;
	}
	else
	{
		child = widget;
	}
	widget->parent = this;
}

// @retail 0x22eb43
void c_class_1a2c81::remove_child(c_class_1a2c81 *widget)
{
	for (c_class_1a2c81 *other = child; other; other = other->next)
	{
		if (other == widget)
		{
			if (widget->previous)
			{
				widget->previous->next = widget->next;
			}
			if (widget->next)
			{
				widget->next->previous = widget->previous;
			}
			if (widget == child)
			{
				child = widget->next;
			}
			widget->parent = 0;
			widget->next = 0;
			widget->previous = 0;
			widget->value0c = NONE;
			return;
		}
	}
}

// @retail 0x22ec73
bool c_class_1a2c81::v10(s_widget_event *event)
{
	c_class_1a2c81 *widget = parent;
	bool result = false;

	if (widget)
	{
		result = widget->v10(event);
	}
	return result;
}

// @retail 0x22ee17
c_class_1a2c81 *c_class_1a2c81::find_text(long index)
{
	return find_child(6, (short)index, false);
}

// @retail 0x22ee27
c_class_1a2c81 *c_class_1a2c81::find_bitmap(short index)
{
	return find_child(8, index, false);
}

// @retail 0x22ee37
c_class_1a2c81 *c_class_1a2c81::find_model(short index)
{
	return find_child(10, index, false);
}

// @retail 0x22ee47
void c_class_1a2c81::set_child_value6e(long type, short index, bool value)
{
	c_class_1a2c81 *widget = find_child(type, index, false);

	if (widget)
	{
		widget->value6e = value;
	}
}

// @retail 0x22ee64
c_class_1a2c81 *c_class_1a2c81::find_by_id(long id)
{
	c_class_1a2c81 *result;

	if (value0c == id)
	{
		return this;
	}
	result = 0;
	for (c_class_1a2c81 *widget = child; widget; widget = widget->next)
	{
		result = widget->find_by_id(id);
		if (result)
		{
			break;
		}
	}
	return result;
}

// @retail 0x22ee92
void c_class_1a2c81::set_user_flags(short flags)
{
	user_flags = flags;
	for (c_class_1a2c81 *widget = child; widget; widget = widget->next)
	{
		widget->set_user_flags(flags);
	}
}

/* the screen of one of this widget's windows that this widget contains */
// @retail 0x22eeb5
c_class_1473c9 *c_class_1a2c81::find_window_screen()
{
	c_class_1473c9 *result = 0;

	for (long index = 0; index < 5; index++)
	{
		c_class_1473c9 *screen = function_148d91(v11(), index);
		if (screen)
		{
			for (c_class_1a2c81 *widget = ((c_class_1a2c81 *)screen)->parent; widget; widget = widget->parent)
			{
				if (this == widget)
				{
					result = screen;
					goto done;
				}
			}
		}
	}
done:
	return result;
}

// @retail 0x22ef0f
long c_class_1a2c81::new_widget_id()
{
	c_class_1473c9 *screen = get_screen();

	screen->next_widget_id++;
	return screen->next_widget_id;
}

/* fills in the animation of this type for the widget: the screen
   transition's for the screen of id 5, a list item's own for type 4, else the
   user interface globals' animation of this index */
// @retail 0x22e65e
void c_class_1a2c81::build_animation(s_type_0cfb31 *animation, short index, long type)
{
	s_type_954545 *globals = function_148350();

	if (globals)
	{
		bool transition = get_screen()->screen_id == 5;

		animation->type = type;
		animation->target = 0;
		animation->value8 = 0;
		animation->valuea = 0;
		animation->direction = 1;
		animation->valuee = 0;
		animation->duration = 0;
		if (type > 1)
		{
			animation->valuee = (type > 3) * 2 + 2;
		}
		else
		{
			animation->valuee = 1;
			animation->duration = v6();
		}
		animation->offset.z = 0.0f;
		animation->offset.y = 0.0f;
		animation->offset.x = 0.0f;
		animation->value14 = 0;
		animation->start_time = 0;
		animation->end_time = 0;
		animation->value20 = 0;
		animation->scale = 1.0f;
		if (transition)
		{
			long value;
			short count;
			short frames;
			void *keys = g_54d598.default_window.get_transition(index, &value, &count, &frames);
			short direction = (count ? (count >= 0 ? 1 : -1) : 0) >= 0 ? 1 : -1;

			animation->direction = direction;
			animation->end_time = g_54d598.m20;
			animation->value20 = value;
			animation->value8 = frames;
			animation->target = (long)keys;
			animation->valuea = direction >= 0 ? 0 : frames - 1;
		}
		else if (this->type == 2 && type == 4)
		{
			c_class_1474e8 *list = (c_class_1474e8 *)parent;
			s_widget_animation_definition *definition =
				(s_widget_animation_definition *)list->get_item_animation(is_in_window() ? 0 : 2);

			animation->target = definition->a.target;
			animation->value8 = definition->a.frames;
			animation->value20 = definition->a.value;
		}
		else if (index >= 0 && index < globals->animation_count)
		{
			s_widget_animation_definition *definition = &globals->animations[index];

			switch (type)
			{
			case 0:
				animation->target = definition->a.target;
				animation->value8 = definition->a.frames;
				animation->direction = 1;
				animation->value20 = definition->a.value;
				break;
			case 1:
				animation->target = definition->b.target;
				animation->value8 = definition->b.frames;
				animation->valuea = animation->value8 > 0 ? animation->value8 - 1 : 0;
				animation->direction = -1;
				animation->value20 = definition->b.value;
				break;
			case 2:
				animation->target = definition->a.target;
				animation->value8 = definition->a.frames;
				animation->valuea = animation->value8 > 0 ? animation->value8 - 1 : 0;
				animation->direction = -1;
				animation->value20 = definition->a.value;
				break;
			case 3:
				animation->target = definition->b.target;
				animation->value8 = definition->b.frames;
				animation->direction = 1;
				animation->value20 = definition->b.value;
				break;
			case 4:
				animation->target = definition->target28;
				animation->value8 = definition->frames24;
				animation->direction = 1;
				switch (definition->mode)
				{
				case 0:
					animation->value14 = 0;
					break;
				case 1:
					animation->value14 = 3;
					break;
				case 2:
					animation->value14 = 1;
					break;
				case 3:
					animation->value14 = 0;
					break;
				}
				animation->value20 = definition->value1c;
				break;
			}
		}
	}
}

/* starts the animation of this type on the widget and its children (a list
   item starts its own) */
// @retail 0x22e957
void c_class_1a2c81::start_animation(long type)
{
	s_type_0cfb31 animation;

	build_animation(&animation, value68, type);
	if (this->type == 2)
	{
		v5((s_widget_event *)&animation);
	}
	else
	{
		set_animation(&animation);
		for (c_class_1a2c81 *widget = child; widget; widget = widget->next)
		{
			widget->start_animation(type);
		}
	}
}

// @retail 0x22e3b4
bool c_class_1a2c81::v16()
{
	return value6d && value6e && animation.end_time <= g_54d5b8;
}

void function_2afeae(s_widget_item *item, c_class_1a2c81 *widget);

/* builds the first count children of the widget from the items and hides
   the rest of its 16 */
// @retail 0x22f042
void function_22f042(s_widget_item *items, c_class_1a2c81 *widget, long count)
{
	/* retail keeps the count on the stack (its address taken): that matched
	   the callers 0x233f0f, 0x251703 and 0x251778 */
	long const *count_reference = &count;
	long i;

	for (i = 0; i < count; i++)
	{
		c_class_1a2c81 *child = widget->find_model((short)i);

		if (child)
		{
			function_2afeae(&items[i], child);
			child->value6e = true;
		}
	}
	for (i = *count_reference; i < 0x10; i++)
	{
		widget->set_child_value6e(10, (short)i, false);
	}
}

// @retail 0x22f092
void list_node_detach(s_list_node *node)
{
	if (node->list)
	{
		list_remove(node->list, node);
		node->list = 0;
	}
}

// @retail 0x22f0a5
void list_remove_all(s_list_head *list)
{
	while (list->first)
	{
		list_remove(list, list->first);
	}
}

// @retail 0x22f0b7
void list_append(s_list_head *list, s_list_node *node)
{
	node->list = list;
	if (!list->first)
	{
		list->first = node;
	}
	else
	{
		s_list_node *last = list->first;
		while (last->next)
		{
			last = last->next;
		}
		last->next = node;
		node->previous = last;
	}
}

// @retail 0x22f0d2
void list_remove(s_list_head *list, s_list_node *node)
{
	s_list_node *next = node->next;

	if (node->previous)
	{
		node->previous->next = node->next;
	}
	if (node->next)
	{
		node->next->previous = node->previous;
	}
	node->next = 0;
	node->previous = 0;
	node->list = 0;
	if (list->first == node)
	{
		list->first = next;
	}
}

/* whether the widget's animation is under way */
// @retail 0x22f0ff
bool function_22f0ff(c_widget *widget)
{
	c_class_1a2c81 *base = (c_class_1a2c81 *)(void *)widget;

	return ANIMATION_FLAG(base->animation, 1) || TEST_FIELD_BIT(base->animation.flags.flag0);
}

// @retail 0x22f4cd
void delegate_register(s_list_head *list, c_list_item_delegate *delegate)
{
	list_append(list, delegate);
}

// @retail 0x22cc8e
c_class_22cc8e::c_class_22cc8e()
{
	value04 = 0;
	memset(&color, 0, sizeof(color));
	value18 = NONE;
	value24 = NONE;
	cursor = NONE;
	value14 = 0;
	value38 = 0;
	length = 0;
	value40 = 0;
	value16 = 1;
	value1c = 2;
	value20 = 1.0f;
	color.blue = 1.0f;
	color.green = 1.0f;
	color.red = 1.0f;
}

// @retail 0x22cd0a
void c_class_22cc8e::setup(word *text, long value04, color3f const *color, short value14, long value18, long value1c, long value24)
{
	set_text(text);
	this->value04 = value04;
	this->color = *color;
	this->value14 = value14;
	this->value18 = value18;
	this->value1c = value1c;
	this->value24 = value24;
}

// @retail 0x22f4db
void c_class_22cc8e::update_length()
{
	length = (short)wcslen(function_22f52e());
	if (cursor >= 0)
	{
		cursor = 0;
	}
}

// @retail 0x22f52e
word *c_user_interface_text_buffer::function_22f52e()
{
	return buffer.text;
}

// @retail 0x22f532
c_user_interface_text_buffer::c_user_interface_text_buffer()
{
}

// @retail 0x22f545
void c_user_interface_text_buffer::set_text(word *string)
{
	function_08cc20((s_name_buffer *)buffer.text, string);
	update_length();
}

// @retail 0x253746
c_text_widget_45a5e0::c_text_widget_45a5e0(word user_flags) :
	c_class_1a2c81(6, user_flags)
{
	value70 = 0;
}

/* the constructors for one controller's user (any user when NONE) */
// @retail 0x25371e
c_text_widget_45a5e0::c_text_widget_45a5e0(long controller_index) :
	c_class_1a2c81(6, controller_index != NONE ? 1 << controller_index : 0)
{
	value70 = 0;
}

// @retail 0x2bac52
c_text_widget_32::c_text_widget_32(long controller_index) :
	c_text_widget_45a5e0(controller_index)
{
}

// @retail 0x2bac6a
c_text_widget_458940::c_text_widget_458940(long controller_index) :
	c_text_widget_45a5e0(controller_index)
{
}

// @retail 0x253aee
long c_text_widget_45a5e0::v6()
{
	return value70;
}

// @retail 0x22f57f
c_class_22cc8e *c_text_widget_458940::function_22f52e()
{
	return &text;
}

void unicode_string_copy(word *destination, const word *source, long maximum_count);

// @retail 0x22f4fa
c_user_interface_text_buffer_32::c_user_interface_text_buffer_32()
{
}

// @retail 0x22f50d
void c_user_interface_text_buffer_32::set_text(word *string)
{
	unicode_string_copy(buffer.text, string, 0x20);
	update_length();
}

// @retail 0x22f561
c_text_widget_32::c_text_widget_32(word user_flags) :
	c_text_widget_45a5e0(user_flags)
{
}

// @retail 0x22cced deleting c_class_22cc8e

// @retail 0x22f583
c_text_widget_458940::c_text_widget_458940(word user_flags) :
	c_text_widget_45a5e0(user_flags)
{
}

// @retail 0x22f5a1 deleting c_text_widget_458940

// @retail 0x22f5ca
c_class_1473c9::c_class_1473c9(long screen_id, long a, long b, word user_flags) :
	c_class_1a2c81(0, user_flags),
	screen_id(screen_id),
	a(a),
	b(b),
	next_widget_id(NONE),
	title((word)0),
	subtitle((word)0),
	value5f0(NONE),
	value5f2(false),
	value5f3(0),
	value5f4(false),
	delegate(this, &c_class_1473c9::function_230427)
{
	type = 0;
	value0c = ++next_widget_id;
	value6d = true;
	*(s_screen_bounds *)&bounds = *(s_screen_bounds *)&g_485a8a.b;
}

// @retail 0x2c883c deleting c_class_1473c9
// @retail 0x1473d0 destructor c_class_1473c9

// @retail 0x24bb00
c_class_1474e8::c_class_1474e8(word user_flags) :
	c_class_1a2c81(1, user_flags),
	data(0),
	value74(0),
	value76(0),
	value78(0),
	value7c(false),
	wraps(false),
	notify_screen(false),
	value7f(true)
{
}

// @retail 0x24bb44 deleting c_class_1474e8
// @retail 0x1474e8 destructor c_class_1474e8

// @retail 0x2bac82
c_widget_45c4d0::c_widget_45c4d0(long type, word user_flags) :
	c_class_1a2c81(type, user_flags)
{
}

// @retail 0x24abae
c_class_14750b::c_class_14750b() :
	c_widget_45c4d0(2, 0),
	value70(NONE),
	value74(0)
{
	type = 2;
	value6d = true;
}

// @retail 0x231e40 deleting c_class_14750b

void function_236299(long sound);
struct s_item_list;
void function_24c7c1(s_item_list *list, long a);

// @retail 0x24ac05
bool c_class_14750b::v10(s_widget_event *event)
{
	if (parent->type == 1 && event->type == 5 && (event->param == 0 || event->param == 12))
	{
		function_24c7c1((s_item_list *)&head7c, (long)&event);
		function_236299(1);
		parent->v10(event);
		return true;
	}
	return c_class_1a2c81::v10(event);
}

// @retail 0x24ac57
bool c_class_14750b::v16()
{
	if (v17() && c_class_1a2c81::v16())
	{
		return true;
	}
	return false;
}

// @retail 0x24ac76
bool c_class_14750b::v17()
{
	return value70 != NONE;
}

/* an item animation of a list's skin (16 bytes) */
struct s_list_item_animation
{
	long unknown00;
	long value20;
	short value8;
	short unknown0a;
	long target;
};

/* a widget delegate that takes no argument (the list node follows the
   vtable pointer) */
class c_widget_delegate : public s_list_node
{
public:
	virtual void invoke() = 0;
};

struct s_widget_view_2b0a;
bool function_2b0a57(s_widget_view_2b0a *widget);
void function_2b0a14(s_widget_view_2b0a *widget, short index);
bool function_24c40b(c_class_1474e8 *list, c_class_1a2c81 *item);
bool function_22f0ff(c_widget *widget);

/* shows the item's focus on its bitmaps: the focused item, an item of the
   focused group and the others each use their own frame */
// @retail 0x24add8
PRIVATE void list_item_update_bitmaps(c_class_14750b *item)
{
	c_class_1474e8 *list = (c_class_1474e8 *)item->parent;

	if (list && list->type == 1)
	{
		short frame;

		if (list->v21(item))
		{
			frame = 0;
		}
		else
		{
			frame = function_24c40b(list, item) ? 1 : 2;
		}
		for (c_class_1a2c81 *child = item->child; child; child = child->next)
		{
			if (child->type == 8 && function_2b0a57((s_widget_view_2b0a *)child))
			{
				function_2b0a14((s_widget_view_2b0a *)child, frame);
			}
		}
	}
}

// @retail 0x24abe0
void c_class_14750b::v3()
{
	c_class_1474e8 *list = (c_class_1474e8 *)parent;
	long skin_index = list->get_skin_index();

	list->v20(this, skin_index);
	list_item_update_bitmaps(this);
	c_class_1a2c81::v3();
}

/* tells the delegates */
// @retail 0x24ae28
PRIVATE void widget_delegates_invoke(s_list_head *list)
{
	for (s_list_node *node = list->first; node; node = node->next)
	{
		static_cast<c_widget_delegate *>(node)->invoke();
	}
}

// @retail 0x24ac80
void c_class_14750b::v13()
{
	if (!function_22f0ff((c_widget *)this))
	{
		s_list_item_animation *item_animation = (s_list_item_animation *)((c_class_1474e8 *)parent)->get_item_animation(0);
		s_type_0cfb31 animation;

		animation.type = 4;
		animation.target = item_animation->target;
		animation.value8 = item_animation->value8;
		animation.valuea = 0;
		animation.direction = 1;
		animation.valuee = 0;
		animation.duration = 0;
		animation.value14 = 0;
		animation.start_time = 0;
		animation.end_time = 0;
		animation.value20 = item_animation->value20;
		animation.offset.z = 0.0f;
		animation.offset.y = 0.0f;
		animation.offset.x = 0.0f;
		animation.scale = 1.0f;
		v5((s_widget_event *)&animation);
	}
	widget_delegates_invoke(&head78);
}

// @retail 0x24ad0b
void c_class_14750b::v14()
{
	if (!function_22f0ff((c_widget *)this))
	{
		c_class_1474e8 *list = (c_class_1474e8 *)parent;

		if (list)
		{
			s_type_0cfb31 animation;

			if (!previous && g_54d5b8 - this->animation.end_time <= 1)
			{
				memset(&animation, 0, sizeof(animation));
				animation.type = 4;
				animation.direction = 1;
				animation.value14 = 0;
			}
			else
			{
				s_list_item_animation *item_animation = (s_list_item_animation *)list->get_item_animation(1);

				memset(&animation, 0, sizeof(animation));
				animation.type = 4;
				animation.target = item_animation->target;
				animation.value8 = item_animation->value8;
				animation.valuea = 0;
				animation.direction = 1;
				animation.valuee = 0;
				animation.duration = 0;
				animation.value14 = 0;
				animation.start_time = 0;
				animation.end_time = 0;
				animation.value20 = item_animation->value20;
				animation.offset.z = 0.0f;
				animation.offset.y = 0.0f;
				animation.offset.x = 0.0f;
				animation.scale = 1.0f;
			}
			v5((s_widget_event *)&animation);
		}
	}
}
// @retail 0x253b1a
void c_text_widget_45a5e0::function_253b1a(long string_handle)
{
	if (string_handle != NONE)
	{
		c_class_1473c9 *screen = get_screen();
		if (screen)
		{
			word buffer[0x100];

			buffer[0] = 0;
			((c_widget *)screen)->function_230134(string_handle, buffer);
			function_22f52e()->set_text(buffer);
		}
	}
}

// @retail 0x22d0e3
void function_22d0e3(c_class_22cc8e *text, short_rectangle2d const *bounds,
    real depth, short_rectangle2d const *screen, short length, short_rectangle2d *ink_bounds)
{
    short_rectangle2d output;
    function_22d13d(text, bounds, depth, screen, length, ink_bounds, &output);
}

// @retail 0x22d108
void function_22d108(c_class_22cc8e *text, short_rectangle2d const *bounds,
    real depth, short_rectangle2d const *screen, short_rectangle2d *output)
{
    short_rectangle2d ink_bounds;
    short length = (short)wcslen((wchar_t *)text->function_22f52e());
    function_22d13d(text, bounds, depth, screen, length, &ink_bounds, output);
}

// @retail 0x253a73
void c_text_widget_45a5e0::function_253a73(short_rectangle2d const *screen, long character_index, short_rectangle2d *output)
{
    real depth = get_depth();
    short_rectangle2d bounds;
    get_bounds((s_widget_bounds *)&bounds);
    function_22d0e3(function_22f52e(), &bounds, depth, screen, (short)character_index, output);
}

// @retail 0x253ab4
void c_text_widget_45a5e0::function_253ab4(short_rectangle2d const *screen, short_rectangle2d *output)
{
    real depth = get_depth();
    short_rectangle2d bounds;
    get_bounds((s_widget_bounds *)&bounds);
    function_22d108(function_22f52e(), &bounds, depth, screen, output);
}

struct s_text_cursor_settings
{
	byte unknown00[0x48];
	short delay;
	word character;
};

struct s_type_954545;
s_type_954545 *function_148350(void);
word *unicode_string_upper(word *string, long maximum_count);
void function_13eb20(short const *tab_stops, short count);
void function_13ec70(color4f const *color);
void function_13ed50(color4f const *color);
bool function_13ee20(word const *string, long font);
void function_235d69(short_rectangle2d const *rectangle, real depth,
	short_rectangle2d const *screen, color4f const *color);

class c_1fa50
{
public:
	void function_1fa50(short_rectangle2d const *bounds, void const *clip,
		void const *option, long flags, real scale, long count, void const *data) const;
};

byte g_51ec04;

/* Draws widget text with cursor progression, projection and tab stops. */
// @retail 0x22cd48
void function_22cd48(c_class_22cc8e *text, short_rectangle2d const *bounds,
	short_rectangle2d const *clip, real depth, real alpha, short_rectangle2d const *screen)
{
	(void)&bounds;
	(void)&clip;
	(void)&depth;
	(void)&alpha;
	(void)&screen;
	word *string = text->function_22f52e();
	if (string && *string && bounds->right - bounds->left && bounds->bottom - bounds->top)
	{
		word buffer[0x400];
		{
		s_text_cursor_settings *settings = (s_text_cursor_settings *)function_148350();
		word cursor_character = settings ? settings->character : 0xdb;
		unicode_string_copy(buffer, string, 0x400);
		short const cursor_position = text->cursor;
		if (cursor_position >= 0)
		{
			dword time = g_54d5b8;
			long cursor = 0x3fe;
			if (cursor_position <= (short)cursor)
				cursor = cursor_position;
			word &glyph = buffer[(short)cursor];
			word &terminator = buffer[(short)cursor + 1];
			glyph = cursor_character;
			terminator = 0;
			text->cursor = (short)cursor;
			if (!text->value40)
			{
				text->value40 = time;
			}
			else
			{
				long delay = settings ? settings->delay : 50;
				if (time - text->value40 >= (dword)delay)
				{
					text->value40 = time;
					text->cursor = cursor + 1;
					if (text->cursor == text->length)
						text->cursor = NONE;
				}
			}
		}
		}
		function_22d2ee(buffer, 0x400);
		if (text->value14 & 1)
			alpha *= (real)((cos(g_54d5b8 * 0.003f) + 1.0f) * 0.5f);
		if (text->value14 & 2)
			unicode_string_upper(buffer, 0x400);
		function_13ed90(text->value04);
		long flags = *(volatile short *)&text->value16;
		long justification = *(volatile long *)&text->value1c;
		long style = *(volatile long *)&text->value18;
		g_4e73a0.style = style;
		g_4e73a0.justification = justification;
		g_4e73a0.flags = flags;
		{
			color4f color;
			color4f shadow;
			color.alpha = alpha;
			*(color3f *)&color.red = text->color;
			shadow.alpha = text->value14 & 4 ? 0.0f : alpha;
			shadow.blue = 0.0f;
			shadow.green = 0.0f;
			shadow.red = 0.0f;
			function_13ec70(&color);
			function_13ed50(&shadow);
		}
		point3f first, last;
		first.x = bounds->left;
		first.y = bounds->top;
		first.z = depth;
		last.x = bounds->right;
		last.y = bounds->bottom;
		last.z = depth;
		function_2360c3(&first, screen);
		function_2360c3(&last, screen);
		short_rectangle2d projected;
		projected.left = (short)first.x;
		projected.top = (short)first.y;
		projected.right = (short)last.x;
		projected.bottom = (short)last.y;
		short_rectangle2d projected_clip;
		if (clip)
		{
			first.x = clip->left;
			first.y = clip->top;
			first.z = depth;
			last.x = clip->right;
			last.y = clip->bottom;
			last.z = depth;
			function_2360c3(&first, screen);
			function_2360c3(&last, screen);
			projected_clip.left = (short)first.x;
			projected_clip.top = (short)first.y;
			projected_clip.right = (short)last.x;
			projected_clip.bottom = (short)last.y;
		}
		else
		{
			projected_clip = projected;
		}
		short tab_stops[8];
		if (text->value38 > 0)
		{
			for (long i = 0; i < text->value38; i++)
				tab_stops[i] = ((short *)text->unknown28)[i] + projected.left;
			function_13eb20(tab_stops, (short)text->value38);
		}
		else
		{
			function_13eb20(0, 0);
		}
		real scale = (last.x - first.x) * text->value20 / (real)(bounds->right - bounds->left);
		if (function_13ee20(buffer, g_4e73a0.font))
			((c_1fa50 const *)buffer)->function_1fa50(&projected, &projected_clip, 0, 0, scale, 0, 0);
	}
	if (g_51ec04)
	{
		color4f outline = { alpha, 1.0f, 1.0f, 1.0f };
		function_235d69(bounds, depth, screen, &outline);
	}
	function_13eb20(0, 0);
}

// @retail 0x253b65
void __stdcall function_253b65(c_class_1a2c81 *widget, long string_handle)
{
    if (string_handle != NONE)
    {
        c_class_1473c9 *screen = widget->get_screen();
        if (screen)
        {
            word buffer[0x100];
            buffer[0] = 0;
            ((c_widget *)screen)->function_230134(string_handle, buffer);
            if (function_13ee20(buffer, widget->function_22f52e()->value04))
                widget->function_22f52e()->set_text(buffer);
        }
    }
}

extern point3f *g_468720;
extern color4f *g_4686dc;
void unicode_string_snprintf(word *buffer, long maximum_count, word const *format, ...);

// @retail 0x2538f5
void c_text_widget_45a5e0::v4(long screen_rect_address)
{
    short_rectangle2d const *screen = (short_rectangle2d const *)screen_rect_address;
    real alpha = animation.scale;
    real depth = get_depth();
    byte debug = g_54d598.unknown05[2];
    short x, y;
    function_2363d4(screen, &x, &y);
    short_rectangle2d rectangle;
    get_bounds((s_widget_bounds *)&rectangle);
    color3f saved = function_22f52e()->color;
    color3f widget_color = color;
    color3f tinted;
    tinted.red = widget_color.red * saved.red;
    tinted.green = widget_color.green * saved.green;
    tinted.blue = widget_color.blue * saved.blue;
    c_class_22cc8e *text = function_22f52e();
    rectangle.left += x;
    rectangle.right += x;
    rectangle.top += y;
    rectangle.bottom += y;
    text->color = tinted;
    if (!debug)
        function_22cd48(function_22f52e(), &rectangle, &rectangle, depth, alpha, screen);
    function_22f52e()->color = saved;
    if (debug)
    {
        c_user_interface_text_buffer_32 label;
        word buffer[16];
        unicode_string_snprintf(buffer, 16, (word const *)L"%d", value0a);
        label.setup(buffer, 0, (color3f const *)g_468720, 0, NONE, 0, NONE);
        function_22cd48(&label, &rectangle, &rectangle, depth, alpha, screen);
        function_235d69(&rectangle, depth, screen, g_4686dc);
    }
}
