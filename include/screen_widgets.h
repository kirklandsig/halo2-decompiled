/* SCREEN_WIDGETS.H: the user interface widget classes of the screens and
   lists in 0x2b0000..0x2cb8c0, numbered by their retail vtable slots.

   Three base layouts recur in the vtables at 0x458788..0x45d920:
   - the widget (17 slots, e.g. 0x458788);
   - the screen (28 slots, e.g. 0x458840): slot 26 returns the screen's load
     procedure (the __stdcall function that allocates and constructs it, which
     the screen history uses to rebuild it), slot 27 reads a flag at +0x5f4;
   - the list (22 slots, e.g. 0x458988): slot 18 returns the list's item
     data, slot 19 its item count, slot 20 fills in one item.
   The slots not decompiled yet have empty bodies here. unknown_19b516.h views
   the 24-slot list at 0x4594a0 as c_widget, with its slots rotated by 8. */

#ifndef SCREEN_WIDGETS_H
#define SCREEN_WIDGETS_H

#include "unknown_11c920.h"
#include "data_array.h"
#include "unknown_0259d0.h"
#include "unknown_19d220.h"

class __single_inheritance c_class_1473c9;
struct s_screen_parameters;
struct s_screen_layout;
struct short_rectangle2d;

/* a player profile's settings (0x1e0 bytes); the settings screens edit a
   copy at 0x54e5d8 */
struct s_player_profile_settings
{
	dword flags;
	dword flags4;
	word name[0x20];
	byte unknown048[0xfc - 0x48];
	struct
	{
		dword invert_look : 1;
		dword vibration : 1;
		dword bit2 : 1;
		dword auto_level : 1;
		dword bits4 : 28;
	} controller_flags;
	byte button_layout;
	byte thumbstick_layout;
	byte look_sensitivity;
	byte unknown103[0x118 - 0x103];
	byte colors[4];
	byte model;
	byte unknown11d[2];
	bool flag;
	byte unknown120[0x148 - 0x120];
	long voice_mask;
	long voice_through_tv;
	byte unknown150;
	byte subtitles;
	byte unknown152[0x1e0 - 0x152];
};

/* the profile being edited (0x54e5d0): its player, its datum and a copy of
   its settings */
struct s_profile_edit
{
	long player;
	long profile_index;
	s_player_profile_settings settings;
};

extern s_profile_edit g_54e5d0;

/* the game variant being edited (or shown when no session holds one) and
   its saved game file index (user_interface_text_parser.cpp) */
extern long g_54e49c;
extern s_game_variant g_54e4a0;

/* unknown_147f6d.cpp */
void function_14800c(long channel, long index);
bool function_148044(long channel, long index, long value);
void profile_edit_begin(long player, s_player_profile_settings *settings, long profile_index);
void profile_edit_save();
void profile_edit_end();

/* the user interface heap (unknown_1a4742.cpp) */
void *__stdcall function_1a47fd(unsigned int size);
void __stdcall function_1a4826(void *pointer);

typedef c_class_1473c9 *(__stdcall *screen_load_proc)(s_screen_parameters *parameters);

/* what a screen is loaded with (0x20 bytes, built by function_149f49, which
   unknown_19b516.h declares with this as an s_message): the controllers it is
   for, the two values its constructor takes, and its load procedure. The
   window channels queue these as requests (unknown_234c64.h). */
struct s_screen_parameters
{
	union
	{
		word type;
		struct
		{
			word type_bit0 : 1;
			word type_bit1 : 1;
			word type_bit2 : 1;
		};
	};
	word user_flags;
	long a;
	long b;
	dword field_c;
	dword id[3];
	screen_load_proc load;
};

/* the focus a screen keeps when it is rebuilt: the focused widget and the
   focused datum of its list */
struct s_screen_focus
{
	long unknown00;
	long widget_id;
	long datum;
};

/* a block of values of a screen definition */
struct s_screen_value_block
{
	byte unknown00[4];
	long count;
	long *values;
};

/* a tag reference of a tag block */
struct s_tag_reference
{
	dword group_tag;
	long tag_index;
};

/* a list's definition in its screen's pane */
struct s_list_definition
{
	dword flags;
	short skin_index;
	short item_count;
	short x;
	short y;
	short value0c;
	short value0e;
};

/* a widget's bounds */
struct s_widget_bounds
{
	short top;
	short left;
	short bottom;
	short right;
};

/* a text of a pane (0x2c bytes): flag 0 left-justifies, flag 1 centres
   (else right), flag 3 makes it an editable text, flag 4 gives it a buffer
   of 0x20 characters */
struct s_text_block
{
	dword flags;
	short value04;
	short value06;
	short value08;
	short font;
	real alpha;
	color3f color;
	s_widget_bounds bounds;
	long string_handle;
	short value28;
	byte unknown2a[2];
};

/* a button of a pane (0x3c bytes) */
struct s_button_block
{
	dword flags;
	short value04;
	short value06;
	byte unknown08[2];
	short font;
	byte unknown0c[4];
	color3f color;
	s_widget_bounds bounds;
	byte unknown24[4];
	long bitmap_tag_index;
	byte unknown2c[4];
	long string_handle;
	short value34;
	byte unknown36[2];
	dword text_flags;
};

/* a bitmap, a model and the two other kinds of widget a pane holds */
struct s_bitmap_block;
struct s_model_block;
struct s_widget_block_24;
struct s_widget_block_18
{
	byte unknown00[8];
	/* the definition of the group each widget shows */
	long tag_index;
	short x;
	short y;
	/* 0 fills the grid row by row, else column by column */
	char order;
	char count;
	char rows;
	char columns;
	short y_step;
	short x_step;
};

/* a group of widgets (the definition a group widget shows) */
struct s_widget_group_definition
{
	byte unknown00[0x1c];
	long text_count;
	s_text_block *texts;
	long bitmap_count;
	s_bitmap_block *bitmaps;
	long block_24_count;
	s_widget_block_24 *blocks_24;
	long block_18_count;
	s_widget_block_18 *blocks_18;
};

/* where a group's widgets go */
struct s_widget_point
{
	short x;
	short y;
};

/* a pane of a screen definition (0x4c bytes) */
struct s_screen_pane
{
	byte unknown00[2];
	short value02;
	long button_count;
	s_button_block *buttons;
	long list_count;
	s_list_definition *lists;
	byte unknown14[0x1c - 0x14];
	long text_count;
	s_text_block *texts;
	long bitmap_count;
	s_bitmap_block *bitmaps;
	long model_count;
	s_model_block *models;
	byte unknown34[0x3c - 0x34];
	long block_24_count;
	s_widget_block_24 *blocks_24;
	long block_18_count;
	s_widget_block_18 *blocks_18;
};

/* a screen's definition tag: flag 0 (else 3, else 4) picks the title's size,
   flag 1 builds the first pane of several, flag 2 hides the title */
struct s_screen_definition
{
	union
	{
		dword flags;
		struct
		{
			dword flag0 : 1;
			dword flag1 : 1;
			dword no_title : 1;
			dword flag3 : 1;
			dword flag4 : 1;
			dword flag5 : 1;
		};
	};
	short screen_id;
	short value06;
	color4f subtitle_color;
	byte unknown18[0x1c - 0x18];
	long string_list_index;
	long pane_count;
	s_screen_pane *panes;
	short widget_set;
	byte unknown2a[2];
	long title_string_id;
	long value_block_count;
	s_screen_value_block *value_blocks;
	long bitmap_count;
	s_tag_reference *bitmaps;
};

/* a text buffer of 0x100 characters, empty when constructed (retail's
   out-of-line copy of the constructor is 0x7f8a0) */
struct s_text_256
{
	s_text_256()
	{
		text[0] = 0;
	}

	word text[0x100];
};

/* a reference to a controller (the index at +4) */
struct s_controller_reference
{
	byte unknown00[4];
	long controller_index;
};

/* an input event (type 5 is a button press; param is the button) */
struct s_widget_event
{
	long type;
	long controller_index;
	long param;
};

/* an animation of a widget (0x34 bytes): the definition is copied into the
   widget's state by c_class_1a2c81::set_animation */
/* a widget animation flag tested by shifting the flags word, widened to a
   dword, rather than as a bitfield: this gives retail's zero-extended byte
   load, shr and test of the low byte (xor ecx, ecx; mov cl, [x + 0x42];
   shr ecx, 1; test cl, 1) */
#define ANIMATION_FLAG(animation, bit) ((bool)(((dword)(animation).valuee >> (bit)) & 1))

struct s_type_0cfb31
{
	long type;
	long target;
	short value8;
	short valuea;
	short direction;
	union
	{
		short valuee;
		/* the window channels test these at widget +0x42 */
		struct
		{
			word flag0 : 1;
			word flag1 : 1;
			word flag2 : 1;
			word flag3 : 1;
		} flags;
	};
	long duration;
	long value14;
	long start_time;
	long end_time;
	long value20;
	/* x and y move the widget; z is its depth */
	point3f offset;
	real scale;
};

/* an intrusive doubly linked list: a node, and the list's head (a node
   leaves its list when it dies; a dying list empties itself) */
struct s_list_node;
struct s_list_head;

void list_node_detach(s_list_node *node);
void list_remove(s_list_head *list, s_list_node *node);
void list_remove_all(s_list_head *list);
void list_append(s_list_head *list, s_list_node *node);

struct s_list_node
{
	s_list_node()
	{
		next = 0;
		previous = 0;
		list = 0;
	}
	~s_list_node()
	{
		list_node_detach(this);
	}

	s_list_node *previous;
	s_list_node *next;
	s_list_head *list;
};

struct s_list_head
{
	s_list_head()
	{
		first = 0;
	}
	~s_list_head()
	{
		list_remove_all(this);
	}

	s_list_node *first;
};

struct s_controller_reference;

/* a list's item handler (vtable 0x45bdb0 for every instance: the one slot
   calls a method of the owner). The list node follows the vtable pointer. */
class c_list_item_delegate : public s_list_node
{
public:
	virtual void invoke(s_controller_reference **controller, long *item) = 0;
};

void delegate_register(s_list_head *list, c_list_item_delegate *delegate);

/* a widget's text (vtable 0x4576d0): slot 1 sets the string, slot 2 returns
   it */
class c_class_22cc8e
{
public:
	c_class_22cc8e();
	virtual ~c_class_22cc8e() {}
	/* pure in retail (the stand-ins cannot construct an abstract class) */
	virtual void set_text(word *text) {}
	virtual word *function_22f52e() { return 0; }

	void update_length();
	/* sets the text and how it shows (unknown_22e27b.cpp) */
	void setup(word *text, long value04, color3f const *color, short value14, long value18, long value1c, long value24);

	long value04;
	color3f color;
	short value14;
	short value16;
	long value18;
	long value1c;
	real value20;
	long value24;
	byte unknown28[0x38 - 0x28];
	long value38;
	short cursor;
	short length;
	long value40;
};

/* a text with its own buffer (vtable 0x458930) */
class c_user_interface_text_buffer : public c_class_22cc8e
{
public:
	c_user_interface_text_buffer();
	virtual void set_text(word *text);
	virtual word *function_22f52e();

	s_text_256 buffer;
};

/* a text with a buffer of 0x20 characters (vtable 0x4588b0) */
class c_user_interface_text_buffer_32 : public c_class_22cc8e
{
public:
	c_user_interface_text_buffer_32();
	virtual void set_text(word *text);
	/* folded with c_user_interface_text_buffer's */
	virtual word *function_22f52e() { return buffer.text; }

	struct s_text_32
	{
		s_text_32()
		{
			text[0] = 0;
		}

		word text[0x20];
	} buffer;
};

class c_class_1a2c81
{
public:
	c_class_1a2c81(long type, word user_flags);
	virtual ~c_class_1a2c81();
	/* gives the widget and its children new ids */
	virtual void v1();
	virtual void v2();
	/* updates the widget and its children */
	virtual void v3();
	virtual void v4(long) {}
	virtual bool v5(s_widget_event *) { return false; }
	virtual long v6() { return 0; }
	virtual void v7(c_class_1a2c81 *) {}
	virtual void v8() {}
	virtual void v9() {}
	/* passes the event up to the parent */
	virtual bool v10(s_widget_event *event);
	virtual long v11() { return 0; }
	virtual long v12() { return 0; }
	virtual void v13() {}
	virtual void v14() {}
	virtual c_class_22cc8e *function_22f52e() { return 0; }
	/* whether the widget shows and its animation has ended */
	virtual bool v16();

	/* the widgets are allocated from the user interface heap */
	static void *operator new(unsigned int size)
	{
		return function_1a47fd(size);
	}

	/* unknown_22e27b.cpp */
	void delete_children();
	/* steps the widget's animation */
	void update(dword time);
	void set_animation(s_type_0cfb31 *animation);
	c_class_1a2c81 *find_child(long type, short index, bool recursive);
	c_class_1473c9 *get_screen();
	bool has_screen();
	bool is_in_window();
	/* focuses the widget in its window (or gives the focus back to its
	   parent), telling the widgets that lose the focus */
	void function_22ecb4_2(bool focus);
	bool has_valid_type();
	real get_depth();
	void get_bounds(s_widget_bounds *bounds);
	void get_real_bounds(box2f *bounds);
	void add_child(c_class_1a2c81 *widget);
	void remove_child(c_class_1a2c81 *widget);
	c_class_1a2c81 *find_text(long index);
	c_class_1a2c81 *find_bitmap(short index);
	c_class_1a2c81 *find_model(short index);
	void set_child_value6e(long type, short index, bool value);
	c_class_1a2c81 *find_by_id(long id);
	void set_user_flags(short user_flags);
	c_class_1473c9 *find_window_screen();
	long new_widget_id();
	/* the first controller of the widget's user flags (unknown_1a2c81.cpp) */
	long get_controller_index();
	void build_animation(s_type_0cfb31 *animation, short index, long type);
	void start_animation(long type);

	/* unknown_24c177.cpp */
	long child_count();
	c_class_1a2c81 *get_child(long index);

	/* the old name of start_animation */
	void function_22e957(long type) { start_animation(type); }

	long type;
	word user_flags;
	short value0a;
	long value0c;
	c_class_1a2c81 *parent;
	c_class_1a2c81 *child;
	c_class_1a2c81 *next;
	c_class_1a2c81 *previous;
	s_widget_bounds bounds;
	color3f color;
	s_type_0cfb31 animation;
	short value68;
	short value6a;
	bool m6c;
	bool value6d;
	bool value6e;
	byte unknown6f;
};

/* a text widget (type 6, vtable 0x45a5e0); slot 15 returns its text */
class c_text_widget_45a5e0 : public c_class_1a2c81
{
public:
	c_text_widget_45a5e0(word user_flags);
	/* for one controller's user (any user when NONE) */
	c_text_widget_45a5e0(long controller_index);

	virtual void v4(long screen_rect_address);
	virtual long v6();

	/* shows the string with this id from the screen's string list */
	void function_253b1a(long string_handle);
	void function_253a73(short_rectangle2d const *screen, long character_index, short_rectangle2d *bounds);
	void function_253ab4(short_rectangle2d const *screen, short_rectangle2d *bounds);

	long value70;
};

/* a text widget with its own text buffer (vtable 0x458940); a screen has two */
class c_text_widget_458940 : public c_text_widget_45a5e0
{
public:
	c_text_widget_458940(word user_flags);
	c_text_widget_458940(long controller_index);
	virtual c_class_22cc8e *function_22f52e();

	c_user_interface_text_buffer text;
};

/* a text widget with a buffer of 0x20 characters (retail folded its vtable
   with 0x458940's) */
class c_text_widget_32 : public c_text_widget_45a5e0
{
public:
	c_text_widget_32(word user_flags);
	c_text_widget_32(long controller_index);
	/* folded with c_text_widget_458940's */
	virtual c_class_22cc8e *function_22f52e() { return &text; }

	c_user_interface_text_buffer_32 text;
};

/* a button (type 3, vtable 0x45a628; unknown_253c8b.cpp): a text of 0x20
   characters, and the handlers its press runs */
class c_class_19b8b1 : public c_class_1a2c81
{
public:
	c_class_19b8b1(short valuef8, word user_flags);

	/* colours the text: focused or as the definition says */
	virtual void v3();
	virtual void v4(long screen_rect_address);
	virtual long v6();
	/* a press of A or start runs the handlers; the directions move the
	   focus unless the definition's flags stop them */
	virtual bool v10(s_widget_event *event);
	virtual c_class_22cc8e *function_22f52e();
	/* whether the focused button's bitmap has more than one frame */
	virtual long v17();

	/* shows the string with this id from the screen's string list
	   (unknown_253c8b.cpp) */
	void function_253b1a(long string_handle);

	c_user_interface_text_buffer_32 text;
	long valuef4;
	short valuef8;
	s_list_head handlers;
};

/* the screen's delegate (vtable 0x45bdb0: retail folded its one slot with
   the list item delegates') */
class c_screen_delegate : public s_list_node
{
public:
	c_screen_delegate(c_class_1473c9 *owner, void (c_class_1473c9::*method)(short *delta)) :
		owner(owner),
		method(method)
	{
	}
	virtual void invoke(short *delta)
	{
		(owner->*method)(delta);
	}

	c_class_1473c9 *owner;
	void (c_class_1473c9::*method)(short *delta);
};

/* the screen widget (vtable 0x458840); the screens override slots 10, 17,
   18, 19 and 26 */
class c_class_1473c9 : public c_class_1a2c81
{
public:
	c_class_1473c9(long screen_id, long a, long b, word user_flags);

	/* 0x2300ea: a press of B or back leaves the screen (unknown_2300cf.cpp) */
	virtual bool v10(s_widget_event *event);
	virtual bool v16();

	virtual void v17() {}
	virtual void v18(void *parameters) {}
	/* loads the bitmaps of the screen's definition */
	virtual void v19();
	/* the screen's window: its channel and index */
	virtual long v20();
	virtual long v21() { return b; }
	virtual void v22(void *window) {}
	virtual void v23(void *window) {}
	/* remembers the focused widget and the list's focused datum */
	virtual void v24(s_screen_focus *focus);
	/* focuses the list's datum or the widget the focus names */
	virtual void v25(s_screen_focus *focus);
	virtual screen_load_proc get_load_proc() { return 0; }
	virtual bool v27();

	/* unknown_2300cf.cpp */
	s_screen_pane *get_current_pane();
	s_screen_pane *get_first_pane();
	short get_first_pane_value();
	bool set_screen_id(long id);
	long get_definition_value(long block, long index);

	/* builds the screen from its definition (unknown_22f6ca.cpp) */
	void build(s_screen_layout *layout);

	/* places the newly loaded screen in its window (unknown_147f6d.cpp) */
	void function_147f6d(s_screen_parameters *parameters);

	/* the delegate's method (unknown_2300cf.cpp) */
	void function_230427(short *delta);

	long screen_id;
	long a;
	long b;
	long next_widget_id;
	c_text_widget_458940 title;
	c_text_widget_458940 subtitle;
	short value5f0;
	bool value5f2;
	char value5f3;
	bool value5f4;
	byte unknown5f5[3];
	c_screen_delegate delegate;
};

/* the screen with a list (vtable 0x4588c0, constructed by 0x230451) */
class c_screen_with_menu : public c_class_1473c9
{
public:
	c_screen_with_menu(long screen_id, long a, long b, word user_flags, void *list);

	/* builds the screen around its list */
	virtual void v18(void *parameters);

	void *list;
};

class c_class_1474e8;

/* what a screen's panes are built from (function_22f8df): the widget that
   holds the panes when there are several, and each pane's buttons or list */
struct s_screen_layout
{
	c_class_1a2c81 *container;
	long count;
	struct
	{
		long type;
		c_class_1a2c81 **widget;
		c_class_1474e8 *list;
		long unknownc;
	} lists[6];
};


/* the window manager disposes of a screen (not decompiled yet) */
void function_148148(c_class_1473c9 *screen);

/* unknown_19b516.h's c_widget is a list of this family (vtable 0x4594a0)
   with its slots rotated by 8; its v9, v10 and v11 are slots 1, 2 and 3 here.
   See the note there on why the two views are still separate. */
class c_class_1474e8 : public c_class_1a2c81
{
public:
	c_class_1474e8(word user_flags);

	virtual void v17() {}
	virtual void *get_item_data() { return 0; }
	virtual long get_item_count() { return 0; }
	virtual void v20(c_class_1a2c81 *, long) {}
	/* whether the item is the focused one (unknown_24c177.cpp) */
	virtual bool v21(c_class_1a2c81 *item);

	/* unknown_24c177.cpp */
	s_list_definition *function_1751d0();
	long get_skin_index();
	void *get_item_animation(long index);
	c_class_1a2c81 *find_item(long datum);
	c_class_1a2c81 *get_focused_item();
	long get_focused_datum();
	void assign_items(long datum);
	void select_datum(long datum);
	void select_item(short item);
	long count_filled_items();
	void *function_24c5f2(long datum);

	s_record_pool *data;
	short value74;
	short value76;
	long value78;
	bool value7c;
	bool wraps;
	bool notify_screen;
	bool value7f;
	s_list_head head80;
	s_list_head item_handlers;
};

/* the widget base of the list items (vtable 0x45c4d0) */
class c_widget_45c4d0 : public c_class_1a2c81
{
public:
	c_widget_45c4d0(long type, word user_flags);
};

/* a bitmap of a pane (0x38 bytes): flag 2 sizes the widget from the
   definition rather than from the bitmap */
struct s_bitmap_block
{
	dword flags;
	short value04;
	byte unknown06[4];
	short sequence;
	short x;
	short y;
	byte unknown10[0x1c - 0x10];
	long tag_index;
	short value20;
	byte unknown22[0x2a - 0x22];
	short height;
	byte unknown2c[0x38 - 0x2c];
};

struct s_type_7ba8e9;

/* a bitmap widget (type 8, vtable 0x45ad60, 0x90 bytes; the helpers that
   view it as s_widget_view_2b0a are in unknown_2b116a.cpp) */
class c_class_2b01eb : public c_class_1a2c81
{
public:
	c_class_2b01eb(s_bitmap_block *definition);
	virtual void v3();
	virtual void v4(long frame);

	s_bitmap_block *definition;
	long start_time;
	long value78;
	real value7c;
	real value80;
	real value84;
	short sequence;
	byte unknown8a[2];
	s_type_7ba8e9 *bitmap;
};

/* a model of a pane (0x4c bytes) */
struct s_model_block
{
	byte unknown00[4];
	short value04;
	byte unknown06[2];
	short value08;
	byte unknown0a[0x38 - 0x0a];
	s_widget_bounds bounds;
	byte unknown40[0x4c - 0x40];
};

/* a model widget (type 7, vtable 0x45ada8, 0x74 bytes) */
class c_class_2b0b5e : public c_class_1a2c81
{
public:
	c_class_2b0b5e(s_model_block *definition);
	virtual void v1();

	s_model_block *definition;
};

/* the third kind of widget of a pane (0x24 bytes) */
struct s_widget_block_24
{
	byte unknown00[4];
	short value04;
	byte unknown06[2];
	short value08;
	byte unknown0a[0x10 - 0x0a];
	long tag_index;
	byte unknown14[0x1c - 0x14];
	s_widget_bounds bounds;
};

/* its widget (type 9, vtable 0x45adf0, 0x88 bytes) */
class c_widget_45adf0 : public c_class_1a2c81
{
public:
	c_widget_45adf0(s_widget_block_24 *definition);
	virtual void v3();
	virtual void v4(long frame);

	s_widget_block_24 *definition;
	dword value74[4];
	short value84;
	byte unknown86[2];
};

/* a widget of a group of a pane's fourth kind (type 10, vtable 0x45ad18,
   0x78 bytes) */
class c_widget_45ad18 : public c_widget_45c4d0
{
public:
	c_widget_45ad18(long index, s_widget_block_18 *definition);
	virtual void v1();

	void place(s_widget_point *origin);

	long index;
	s_widget_block_18 *definition;
};

/* a list's item widget (vtable 0x459f10, 0x80 bytes); every list keeps an
   array of them at +0x88 */
class c_class_14750b : public c_widget_45c4d0
{
public:
	c_class_14750b();

	/* lets the list fill the item, then shows its focus */
	virtual void v3();
	/* a press of A or start chooses the item */
	virtual bool v10(s_widget_event *event);
	/* starts the item's animations of the list's skin */
	virtual void v13();
	virtual void v14();
	virtual bool v16();
	/* whether the item shows a datum */
	virtual bool v17();

	long value70;
	long value74;
	s_list_head head78;
	s_list_head head7c;
};

/* an item's text, chosen by the item from a table (function_24c75c) */
struct s_list_item_text
{
	short item;
	long string_handle;
};

bool function_24c75c(c_class_1474e8 *list, c_class_1a2c81 *item, s_list_item_text *table, long text_index, long count);

typedef void (c_class_1474e8::*list_item_method)(s_controller_reference **controller, long *item);

/* the item handler a list's constructor registers (vtable 0x45bdb0; retail
   folded every list's copy of its one slot into 0x2b27f9) */
class c_list_item_handler : public c_list_item_delegate
{
public:
	c_list_item_handler(c_class_1474e8 *owner, list_item_method method) :
		owner(owner),
		method(method)
	{
	}
	virtual void invoke(s_controller_reference **controller, long *item);

	c_class_1474e8 *owner;
	list_item_method method;
};

/* a list's item widget (a list's children are its items) */
inline c_class_14750b *widget_item(c_class_1a2c81 *widget)
{
	return (c_class_14750b *)widget;
}

/* a list's datum: the item it shows */
struct s_list_item_datum
{
	short salt;
	short item;
};

/* adds a datum showing this item to a list's data */
__forceinline void list_item_add(c_class_1474e8 *list, short item)
{
	((s_list_item_datum *)list->data->data)[record_pool_allocate(list->data) & 0xffff].item = item;
}

/* creates a data array in the user interface heap */
s_record_pool *user_interface_data_new(const char *name, long maximum_count, long size);

/* the lists with a 23rd slot (0x45b3e0, 0x45b510) and the 26-slot lists of
   the vtable at 0x459e88 (0x45af88): slot 22 returns the items and their
   count */
class c_list_widget_with_items : public c_class_1474e8
{
public:
	c_list_widget_with_items(word user_flags) :
		c_class_1474e8(user_flags)
	{
	}

	virtual void *get_items(long *count) { return 0; }
};

class c_list_widget_26 : public c_list_widget_with_items
{
public:
	virtual void v23() {}
	virtual void v24() {}
	virtual void v25() {}
};

/* a widget definition item (0x74 bytes; read by 0x2afeae): its optional
   fields are present when their flag is set */
struct s_widget_item
{
	union
	{
		dword flags;
		struct
		{
			dword has_value4 : 1;
			dword unknown1 : 1;
			dword has_value5c : 1;
			dword has_value58 : 1;
			dword unknown4 : 1;
			dword has_value5e : 1;
			dword has_value60 : 1;
			dword has_value5f : 1;
			dword has_value64 : 1;
			dword has_color : 1;
			dword unknown_bits : 22;
		};
	};
	long value4;
	byte unknown08[4];
	short x;
	short y;
	byte unknown10[0x48 - 0x10];
	dword value48[4];
	long value58;
	short value5c;
	bool value5e;
	bool value5f;
	short value60;
	byte unknown62[2];
	long value64;
	color3f color;

	s_widget_item();
};

#endif
