// @flags /O1 /Oi /arch:SSE /Gr
/* UNKNOWN_2CB350.CPP: refreshing the player color screen (built with SSE,
   apart from its neighbours in unknown_2c4e9c.cpp) */

#include <string.h>
#include "unknown_11c920.h"
#include "globals.h"
#include <xtl.h>
#include <xonline.h>
#include "screen_widgets.h"
#include "unknown_19b510.h"
#include "user_interface_lists.h"
#include "unknown_19b516.h"
#include "unknown_234c64.h"

void function_2b10a3(c_class_2b0b5e *widget, dword const *appearance, long type);

/* a screen that edits the profile settings, keeping a copy to restore */
class c_screen_45d2b8 : public c_screen_with_menu
{
public:
	c_screen_45d2b8(long screen_id, long a, long b, word user_flags, void *list);

	virtual bool v10(s_widget_event *event);

	bool changed;
	byte unknown615[3];
	dword settings[0x78];
};

/* the player color screen (vtable 0x45d398): one screen for each of the
   four colors */
class c_screen_45d398 : public c_screen_45d2b8
{
public:
	c_screen_45d398(long a, long b, word user_flags);
	virtual void v3();

	/* focuses the color the profile has */
	virtual void v19();
	virtual screen_load_proc get_load_proc();

	c_choose_player_color_list list;
	long index;
};

// score MATCH (was 7): the old 7 was that the two find_child results were allocated differently. Retail
// copies each into ebx before testing it; ours tests eax and copies into ebx
// inside the taken branch (function_2b10a3 takes the model in ebx). Separate
// const locals for the two results measured the same 7. Permuter: 128 tries in
// 300 s (seed 999), no form below 7.
// b14I (11-17) walk against retail, fast build on r847: the 7 was not in this function's own statements. Retail copies
// each find_child result into ebx right after the call (0x2cb3a6, 0x2cb3c7: `mov ebx, eax; test ebx, ebx`) and loads
// the colors into eax only after the test (0x2cb3b3, 0x2cb3cf), i.e. the widget is the first declared parameter of
// function_2b10a3 and the colors the second. Declared that way (widget, appearance, type; callee bytes unchanged,
// still MATCH) this function matches. Forms that stayed 7 on the old order: volatile model, one local per model,
// `!= NULL`, `else` branch, a base-type local with the cast at the call, a bool, a ternary, an assignment in the
// condition, an inline helper (74 unless __forceinline, then 7), the address-taken local of 0x2c8d7c (7). Calls and
// declarations updated in unknown_2b116a.cpp (definition and 0x2c8d7c), unknown_24a2e7.cpp (0x24a7ec) and
// unknown_2523e3.cpp (0x252554, 55 -> 49).
// @retail 0x2cb350
void c_screen_45d398::v3()
{
	if (!ANIMATION_FLAG(animation, 1))
	{
		short color = (short)list.get_focused_datum();
		long index = this->index;
		long previous_color = (char)g_54e5d0.settings.colors[index];
		long valid_color = color < -1 ? -1 : (color > 17 ? 17 : color);
		if (valid_color == color)
			previous_color = color;
		g_54e5d0.settings.colors[index] = (byte)previous_color;
	}
	c_class_2b0b5e *model = (c_class_2b0b5e *)find_child(7, 0, false);
	if (model)
		function_2b10a3(model, (dword const *)g_54e5d0.settings.colors, 0);
	model = (c_class_2b0b5e *)find_child(7, 1, false);
	if (model)
		function_2b10a3(model, (dword const *)g_54e5d0.settings.colors, 1);
	c_class_1a2c81::v3();
}
