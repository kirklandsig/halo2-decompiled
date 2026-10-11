/* UNKNOWN_19B516.H: the widget class whose 24-slot vtable is at 0x4594e0
   (slots 22 and 23 are library/other code), and the types its methods use */

#ifndef UNKNOWN_19B516_H
#define UNKNOWN_19B516_H

#include "unknown_11c920.h"
#include "data_array.h"
#include "screen_widgets.h"

struct s_id_triplet
{
	dword a;
	dword b;
	dword c;
};

struct s_message
{
	byte unknown00[0xc];
	dword field_c;
	byte unknown10[0xc];
	void (__stdcall *callback)(s_message *message);
};

void function_149f49(s_message *message, word a, dword *id, short b, long c, long d, long e);

struct s_event
{
	long type;
	long unknown04;
	long param;
	byte unknown0c[0x64];
	long index;

};


struct s_text_interface
{
	virtual void v0() {}
	virtual void set_text(word *text) {}
};

struct c_type_5003a0
{
	virtual void v0() {}
	virtual void v1() {}
	virtual void v2() {}
	virtual void v3() {}
	virtual void v4() {}
	virtual void v5() {}
	virtual void v6() {}
	virtual void v7() {}
	virtual void v8() {}
	virtual void v9() {}
	virtual void v10() {}
	virtual void v11() {}
	virtual void v12() {}
	virtual void v13() {}
	virtual void v14() {}
	virtual s_text_interface *function_22f52e() { return 0; }
};

/* slots 18 and 19 as 24bb60 calls them */
struct c_list_view
{
	virtual void s0() {}
	virtual void s1() {}
	virtual void s2() {}
	virtual void s3() {}
	virtual void s4() {}
	virtual void s5() {}
	virtual void s6() {}
	virtual void s7() {}
	virtual void s8() {}
	virtual void s9() {}
	virtual void s10() {}
	virtual void s11() {}
	virtual void s12() {}
	virtual void s13() {}
	virtual void s14() {}
	virtual void s15() {}
	virtual void s16() {}
	virtual void s17() {}
	virtual void *get_first() { return 0; }
	virtual long get_count() { return 0; }
};

/* This is the list whose real 24-slot vtable is at 0x4594a0, viewed with its
   slots rotated by 8: vN here is real slot N+16 for N <= 7 and N-8 otherwise.
   screen_widgets.h's c_class_1474e8 is the same list family numbered by the
   real slots (its lists call v9, v10 and v11 here as their base methods).
   They are not merged yet: real slot 22 here (v6, 0x234a97) clashes with the
   get_items slot of the 23- and 26-slot lists, several slots differ in
   signature, and the members and virtual calls here would need retyping. */
class c_widget
{
public:
	/* the base class's slot 16 (unknown_22e27b.cpp) */
	virtual bool v0() { return ((c_class_1a2c81 *)(void *)this)->c_class_1a2c81::v16(); }
	virtual void v1();
	virtual void *v2();
	virtual long v3();
	/* the postgame statistics lists' slots 20 to 22 (unknown_232d43.cpp) */
	virtual void v4(s_event *event, long unused) {}
	virtual bool v5(s_event *event) { return false; }
	virtual void v6(c_widget *window, long row) {}
	virtual void v7(c_widget *child) {}
	virtual ~c_widget() {}
	virtual void v9();
	virtual void v10();
	virtual void v11();
	virtual void v12(long a);
	virtual void v13(s_event *event);
	virtual long v14();
	virtual void v15(c_widget *widget);
	virtual void v16();
	virtual void v17();
	virtual bool v18(s_event *event);
	virtual long v19();
	virtual long v20();
	virtual void v21();
	virtual void v22(s_event *event, long index) {}
	virtual void v23() {}

	/* the base class's has_valid_type (unknown_22e27b.cpp) */
	bool function_22e37f() { return ((c_class_1a2c81 *)(void *)this)->has_valid_type(); }
	/* the base class's function_22ecb4_2 (unknown_22e27b.cpp) */
	void function_22ecb4(bool focus) { ((c_class_1a2c81 *)(void *)this)->function_22ecb4_2(focus); }
	/* the base class's is_in_window (unknown_22e27b.cpp) */
	bool function_22ed7a() { return ((c_class_1a2c81 *)(void *)this)->is_in_window(); }
	/* the base class's get_screen (unknown_22e27b.cpp) */
	c_widget *function_22eeee() { return (c_widget *)((c_class_1a2c81 *)(void *)this)->get_screen(); }
	bool function_22ef1b();
	/* the base class's slots 1 and 2 (unknown_22e27b.cpp) */
	void function_22e335() { ((c_class_1a2c81 *)(void *)this)->c_class_1a2c81::v2(); }
	/* the base class's slot 3 (unknown_22e27b.cpp) */
	void function_22e391() { ((c_class_1a2c81 *)(void *)this)->c_class_1a2c81::v3(); }
	void function_22e315() { ((c_class_1a2c81 *)(void *)this)->c_class_1a2c81::v1(); }
	/* the base class's set_animation (unknown_22e27b.cpp) */
	void function_22e89c(s_event *event) { ((c_class_1a2c81 *)(void *)this)->set_animation((s_type_0cfb31 *)event); }
	/* the base class's slot 10 (unknown_22e27b.cpp) */
	bool function_22ec73(s_event *event) { return ((c_class_1a2c81 *)(void *)this)->c_class_1a2c81::v10((s_widget_event *)event); }
	/* the list's slot 21 (unknown_24c177.cpp) */
	bool function_24c3f8(s_event *event) { return ((c_class_1474e8 *)(void *)this)->c_class_1474e8::v21((c_class_1a2c81 *)event); }
	void function_230134(long id, word *buffer);
	/* the base class's find_child (unknown_22e27b.cpp) */
	c_type_5003a0 *function_22edb8(long type, long index, long flag) { return (c_type_5003a0 *)((c_class_1a2c81 *)(void *)this)->find_child(type, (short)index, flag != 0); }
	/* the base class's get_bounds (unknown_22e27b.cpp) */
	void function_22e9c6(short *bounds) { ((c_class_1a2c81 *)(void *)this)->get_bounds((s_widget_bounds *)bounds); }

	byte unknown04[4];
	word m8;
	byte unknown0a[6];
	c_widget *parent;
	c_widget *child;
	c_widget *next;
	c_widget *prev;
	byte unknown20[0x30];
	dword m50;
	byte unknown54[0x19];
	byte m6d;
	byte m6e;
	byte unknown6f;
	s_record_pool *m70;
	word m74;
	word m76;
	long m78;
	byte unknown7c[3];
	byte m7f;
	byte unknown80[4];
	byte sub84[4];
	byte sub88[0x818];
	byte m8a0;
	byte unknown8a1[3];
	long m8a4;
	long m8a8;
};

word *function_1630e0(word *buffer, const word *format, ...);
void function_24c0c4(c_widget *widget);
void function_24c610(void *item, c_widget *widget);
bool function_24c63e(c_widget *widget);
bool function_24c676(c_widget *widget);
/* the list's get_focused_item (unknown_24c177.cpp) */
inline c_widget *function_24bae6(c_widget *widget) { return (c_widget *)((c_class_1474e8 *)(void *)widget)->get_focused_item(); }
void function_24c7e4(void *list, s_event **event, long *key);
void function_24c1c5(c_widget *widget, char direction);



/* the sprite pair drawn by 24bda2: the two elements are drawn from the tag's
   element at +0x48 */
struct s_sprite_element
{
	byte unknown00[4];
	short width;
	short height;
};

struct s_sprite
{
	s_sprite_element main;
	byte unknown08[0x74 - sizeof(s_sprite_element)];
	s_sprite_element second;
};

struct s_sprite_tag
{
	byte unknown00[0x48];
	s_sprite *sprite;
};

struct s_sprite_placement
{
	byte unknown00[8];
	long tag_index;
	short main_x;
	short main_y;
	short second_x;
	short second_y;
	byte unknown14[4];
	/* the list items' animations (16 bytes each) */
	byte *item_animations;
};

struct s_float_rect
{
	real x0;
	real x1;
	real y0;
	real y1;
};

struct s_bounds
{
	short a;
	short b;
	short c;
	short d;
};


struct s_name_request;
void function_18ff47(long player, dword *out);
bool function_6c7e0();
bool function_199994();
bool function_1999b3();
bool function_1900a5(long player);
void unicode_string_to_ascii(const word *source, char *destination, long maximum_count);
void __stdcall function_148893(s_name_request *request, long flag);
struct short_rectangle2d;
void function_2363d4(short_rectangle2d const *bounds, short *x, short *y);
bool function_22f0ff(c_widget *widget);
/* the list's get_skin_index (unknown_24c177.cpp) */
inline long function_24c0b3(c_widget *widget) { return ((c_class_1474e8 *)(void *)widget)->get_skin_index(); }
s_sprite_placement *function_14837a(short index);
/* the base class's get_depth (unknown_22e27b.cpp) */
inline real function_22e9aa(c_widget *widget) { return ((c_class_1a2c81 *)(void *)widget)->get_depth(); }
struct short_rectangle2d;
s_float_rect *function_23618e(s_float_rect *rect, real depth, short_rectangle2d const *bounds);
void function_235e5e(s_sprite_element *element, s_float_rect *from, s_float_rect *to, dword color, long a, long b);

#endif
