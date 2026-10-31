// @flags /O1 /arch:SSE /Gr
/* UNKNOWN_19B516.CPP: the methods of the widget class whose vtable is at
   0x4594e0 */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include "globals.h"
#include "unknown_19b516.h"
#include "unknown_234c64.h"

bool xuid_equal(XUID const *a, XUID const *b, bool compare_guest_number);

dword g_54d5b8;


// @retail 0x24bb60
void c_widget::v1()
{
	c_list_view *list = (c_list_view *)this;
	c_class_14750b *items = (c_class_14750b *)list->get_first();
	long count = list->get_count();
	word flags = m8;

	for (long i = 0; i < count; i++)
	{
		c_class_14750b *item = &items[i];

		item->set_user_flags(flags);
		((c_class_1a2c81 *)(void *)this)->add_child(item);
	}
}

// @retail 0x19b516
void *c_widget::v2()
{
	return sub88;
}

// @retail 0x232d3f
long c_widget::v3()
{
	return 0x10;
}

// @retail 0x24bb9c
void c_widget::v9()
{
	function_22e315();
	function_24c0c4(this);
	if (!function_22ed7a() && child)
	{
		v7(child);
		function_24c610(child, this);
	}
}

// @retail 0x24bbd0
void c_widget::v10()
{
	if (m7f)
	{
		s_record_pool *p = m70;
		if (p)
		{
			data_dispose(p);
			m70 = 0;
		}
	}
	function_22e335();
}

// @retail 0x24bd61
void c_widget::v11()
{
	if (function_24c63e(this))
	{
		word value = m74;
		if (value)
		{
			m74 = value - 1;
		}
	}
	if (function_24c676(this))
	{
		word value = m76;
		if (value)
		{
			m76 = value - 1;
		}
	}
	function_22e391();
}

// @retail 0x22e930
void c_widget::v13(s_event *event)
{
	function_22e89c(event);
	for (c_widget *widget = child; widget; widget = widget->next)
	{
		widget->v5(event);
	}
}

// @retail 0x230130
long c_widget::v14()
{
	return m78;
}

// @retail 0x22eb8c
void c_widget::v15(c_widget *widget)
{
	if (widget)
	{
		widget->function_22ecb4(true);
		for (c_widget *item = widget->child; item; item = item->next)
		{
			if (item->function_22e37f())
			{
				widget->v7(item);
				break;
			}
		}
	}
}

// score 2 (was 12): only the loop's shared tail differs: retail copies the next widget to ESI and tests ESI,
// ours tests EAX before the copy (`test eax, eax; mov esi, eax`).
// Before round 9, score 12 (was 20): only the entry differs: retail finds the first widget in EAX/EDX, saves ESI after it
// and joins the loop's `mov esi, eax; test esi, esi` tail; ours finds it straight into ESI (push first).
// Forms with the old candidate copied at the loop head (all 20 unless given): shared widget view; immutable
// current 26; initial/later/both reverse for; current initially empty; direct existing predicate/focus;
// inline/noinline eligibility declarations 44/20; focus declarations; single search cursor 41; proven
// initial candidate do-while 37; eligibility in loop admission 47; named eligibility result; shared success
// exit/current for; role names; separate entry result and entry-for 42/42; failure continue; for plus
// break; initial/both result arms 20/21; entry reverse lifetime; scoped widget 26; scoped result arms 21;
// scoped reverse lifetimes 26; selected-widget success tail 47 (with initial result arms 47); copy before
// the loop and at its tail; for-walks to the first sibling; forceinline next-or-first helper in a for
// loop 42 (out of line 72); helper with the head copy 42; one cursor stepped at the loop top 64.
// The ternary step (`widget = next ? next : first sibling`) keeps the body as retail: 12; the same as a for
// loop, through a next-or-first ternary helper, with an if/do-while entry guard, or `widget = this` first 12;
// a named entry result 35 (entry as retail, but the inverted loop tests the name, so no shared tail).
// From another lane (all with the old candidate copy): candidate-tested while with a body widget 26; one
// stepping loop from this 64; break then focus under if (widget) 47; widget assigned before the loop and at
// its end 20; first-sibling walk local 20.
// Round 9: the base class's next_wrapped (a single-exit `result` set in an if/else) at the entry and the
// step 2 (the result is not merged into ESI at the entry, so the entry joins the shared tail as retail);
// harness screens on the twin 0x22ec1b: the same helper as a ternary or with an early return 12; a result
// from a ternary, a reassigned parameter, an out pointer or a NULL-initialised result 2 each; a `break`
// with the focus after the loop, the search in the loop condition or a do-while under a test 2 to 6.
// @retail 0x22ebc3
void c_widget::v16()
{
	c_widget *widget = next_wrapped();
	while (widget)
	{
		if (widget->function_22e37f())
		{
			function_22ecb4(false);
			widget->function_22ecb4(true);
			return;
		}
		widget = widget->next_wrapped();
	}
}

// score 2 (was 2): only the loop's shared tail differs: retail copies the previous widget to ESI and tests
// ESI, ours tests EAX before the copy (`test eax, eax; mov esi, eax`).
// Before round 9, score 12 (was 20): only the entry differs: retail finds the first widget in EAX/EDX, saves ESI after it
// only the entry differs: retail finds the first widget in EAX/EDX, saves ESI after it
// and joins the loop's `mov esi, eax; test esi, esi` tail; ours finds it straight into ESI (push first).
// Forms with the old candidate copied at the loop head (all 20 unless given): entry/repeat/both link
// cursors; entry/repeat/both last-widget results; link for scans; selected declaration first; transient
// entry scope 42; eligibility condition 44; rejection arm; previous branch 21; one search cursor 21; role
// names; fallback declaration order; native traversal/entry scan 42/repeat scan/next cursors/candidate;
// direct existing methods; native selected lifetime 26; native success tail 47; walk from `widget = this`
// with separate entry/step results 42 (90 bytes); prev link per branch 21; NULL-initialized widget; own
// prev local 21; forceinline previous-or-last helper at entry and step 42 (90), plain inline 72 (55);
// eligibility as bool/byte/short/long 20/20/42/40; base-class candidate with a cast copy; prev read
// before the eligibility call 20; `while (c_widget *widget = candidate)` 20; helper in the loop
// condition from `widget = this` 64 (63); single-variable do/while walk as 0x2b41ea 69 (58).
// The ternary step (`widget = prev ? prev : last sibling`) keeps the body as retail: 12; the same as a for
// loop, through a previous-or-last ternary helper, with an if/do-while entry guard, `widget = this` first
// or a named previous 12; a named entry result 35 (92: entry as retail, but the inverted loop tests the
// name, so no shared tail); ternary into the head-copy candidate 39 (92); last-sibling helper with its own
// next cursor 21; early-return helper 43; eligibility in the loop condition with focus after 44 (92).
// Round 9: the base class's previous_wrapped (a single-exit `result` set in an if/else) at the entry and
// the step 2; a result from a ternary 2; the copy before the loop with ternary steps 35 (92 bytes); the
// helper at the entry and a ternary step 3 (92 bytes, harness); reassigned parameter, out pointer,
// NULL-initialised result, member function, `break` with the focus after the loop, for loop, do-while
// under a test, `continue` arm 2 each (harness); the helper's own walk loop 12 (harness).
// Round 126 (real, 12 unless given): the step as if/else into the widget 12; entry and step both if/else
// 12; entry as if/else 12; the step through a named previous local 12; if-form entry into a head-copy
// candidate with a ternary step 39 (92). Screened (one-link LTCG): ternary head copy in while/for 33 (40
// instructions); head copy as the body's first statement or in an if/do-while: base code; the widget
// passed to a forceinline search helper: base code; an out-of-file previous-or-last function is not
// inlined at /O1 (called). Retail's entry result stays a scratch value copied in the shared loop head.
// Round 51 (LTCG env link of this file, equal to the full listing): the step, the entry or both through
// the base class's helper directly 2; entry and step through two distinct helpers (separate result
// symbols) 2; the NULL-first if/else helper 2; ternary helper at both 17; `result = previous; if
// (!result) result = last_sibling();` at both 20 (90 bytes, entry straight into ESI), written-out walk
// 20; early-return helper at both 17; the if/else step into the widget gives retail's tail but its
// entry its own `test eax, eax; push esi; mov esi, eax; je` (92 bytes); that step with an early-return,
// `if (!result)`, ternary or out-reference entry 17 each (entry computed into ESI and threaded).
// Round 11 (lane 2): retail's head is the widget phi (copies from EAX on every incoming edge, merged);
// the step as an if/else into the widget (last_sibling in the else arm) gives exactly that tail, but the
// entry then either keeps its own rotated test on the scratch value (helper entry) or is computed into
// ESI and threaded into the body (if/else entry). Real: helper entry + if/else step 35 (92 bytes); if/else
// entry and step 12; `widget = previous; if (!widget) widget = last` entry with if/else step 41 (86
// bytes); `break` with the focus after the loop, if/else step 37 (94 bytes); head-copy candidate with an
// if/else step 2 (same code). Harness grid (entry helper/ternary/if-else/inverted if-else/assigned then
// fallback/from a `this` widget x step helper/ternary/if-else/inverted/named previous x while/guarded
// do-while, 60 forms): only helper/helper gives 2; the walk written with its own local in the step, an
// inner do-while in the step, and the search in the loop condition do not keep the entry scratch; a 600 s
// permuter run (55 tries) found none below 2.
// Round 213: native wrapped cursor 2; focus then break 2; focus common exit 2; for with focus/break 2;
// native focus/break 2; widget selected result 16; native selected result 16; focus terminates cursor 2;
// original executable source restored.
// Round 16 (lane 2): `while (widget != NULL)` 2; a free forceinline previous-or-last helper taking the
// widget 2; the loop variable typed as the base class with its methods 2; the c_widget wrapper plain
// inline 72 (55 bytes, called); an early return when the entry finds none, then a do-while 2.
// Round 60: empty-entry guard with a helper-fallback if/else step 35; helper-fallback ternary step 35;
// helper-fallback ternary entry 42; ternary entry and step 12; helper-fallback if/else step 35;
// helper-fallback if/else entry 13; if/else entry and step 45; single-exit goto with if/else step 35.
// Original executable body restored.
// @retail 0x22ec1b
void c_widget::v17()
{
	c_widget *widget = previous_wrapped();
	while (widget)
	{
		if (widget->function_22e37f())
		{
			function_22ecb4(false);
			widget->function_22ecb4(true);
			return;
		}
		widget = widget->previous_wrapped();
	}
}

bool window_manager_channel_window_in_use(long channel, long index);

/* whether a window over the widget's screen's window is in use */
// @retail 0x22ef1b
bool c_widget::function_22ef1b()
{
	bool result;
	c_class_1473c9 *screen = (c_class_1473c9 *)function_22eeee();
	long index = screen->v21();

	switch (screen->v20())
	{
	case 3:
		result = window_manager_channel_window_in_use(1, index) || g_54d598.window_0.current || g_54d598.window_0.next ||
			window_manager_channel_window_in_use(2, 4);
		break;
	case 5:
	case 6:
		result = window_manager_channel_window_in_use(1, index) || g_54d598.window_0.current || g_54d598.window_0.next ||
			window_manager_channel_window_in_use(3, index) || window_manager_channel_window_in_use(4, index) ||
			window_manager_channel_window_in_use(2, 4);
		break;
	case 4:
		result = window_manager_channel_window_in_use(1, index) || g_54d598.window_0.current || g_54d598.window_0.next ||
			window_manager_channel_window_in_use(3, index) || window_manager_channel_window_in_use(2, 4);
		break;
	case 2:
		result = window_manager_channel_window_in_use(1, index) || g_54d598.window_0.current || g_54d598.window_0.next;
		break;
	case 1:
		result = window_manager_channel_window_in_use(0, index);
		break;
	case 0:
		result = false;
		break;
	default:
		result = false;
		break;
	}
	return result;
}

// @retail 0x24c024
bool c_widget::v18(s_event *event)
{
	bool handled = false;
	switch (event->type)
	{
	case 1:
		function_24c1c5(this, -1);
		handled = true;
		break;
	case 3:
		function_24c1c5(this, 1);
		handled = true;
		break;
	case 5:
		{
			long button = event->param;
			if ((button == 0 || button == 12) && !function_22ef1b())
			{
				c_widget *item = function_24bae6(this);
				if (item)
				{
					long type = *(long *)((byte *)item + 4);
					long datum = *(long *)((byte *)item + 0x70);
					volatile long key = type;
					key = datum;
					function_24c7e4(sub84, &event, (long *)&key);
				}
				handled = true;
			}
			break;
		}
	}
	if (!handled)
		handled = function_22ec73(event);
	return handled;
}

// @retail 0x22ec90
long c_widget::v19()
{
	return function_22eeee()->v20();
}

// @retail 0x22ec9e
long c_widget::v20()
{
	c_class_1473c9 *screen = (c_class_1473c9 *)function_22eeee();
	if (screen)
	{
		return screen->v21();
	}
	return 4;
}

// @retail 0x24c09d
void c_widget::v21()
{
	v7(child);
	function_24c610(child, this);
}

#include "slot_handler.h"
static __forceinline long real_to_long(real value);

static __forceinline void draw_element_24bda2(s_sprite_element *element, long left, long top,
	short x, short y, real scale, real opacity, short_rectangle2d const *screen)
{
	real offset_x, offset_y;
	s_float_rect from, to;
	from.x0 = from.y0 = 0.0f;
	from.x1 = (real)element->width;
	from.y1 = (real)element->height;
	to.x0 = (real)left;
	to.y0 = (real)top;
	to.y1 = to.y0 - element->height;
	to.x1 = to.x0 + element->width;
	offset_y = (real)y;
	offset_x = (real)x;
	to.x0 += offset_x;
	to.y0 += offset_y;
	to.x1 += offset_x;
	to.y1 += offset_y;
	function_23618e(&to, scale, screen);
	opacity *= 255.0f;
	long alpha = real_to_long(opacity);
	function_235e5e(element, &from, &to, (alpha << 24) | 0xffffff, 0, 0);
}

// @retail 0x24bda2
void c_widget::v12(long a)
{
	short x;
	short y;
	s_bounds bounds;

	function_2363d4((short_rectangle2d const *)a, &x, &y);
	if (function_22f0ff(this))
	{
		return;
	}
	s_sprite_placement *placement = ui_definition_get_skin(function_24c0b3(this));
	if (!placement)
	{
		return;
	}
	long tag_index = placement->tag_index;
	s_sprite_tag *tag = (s_sprite_tag *)g_4e3b44[tag_index & 0xffff].flags;
	real scale = function_22e9aa(this);
	function_22e9c6(&bounds.a);
	if (tag_index == NONE)
	{
		return;
	}
	if (function_24c63e(this))
	{
		real opacity = 1.0f;
		if (m74 == 0)
		{
			opacity = 0.5f;
		}
		s_sprite_element *element = &tag->sprite->main;
		if (tag->sprite)
		{
			draw_element_24bda2(element, placement->main_x + bounds.b, placement->main_y + bounds.c, x, y, scale, opacity,
				(short_rectangle2d const *)a);
		}
	}
	if (function_24c676(this))
	{
		real opacity = 1.0f;
		if (m76 == 0)
		{
			opacity = 0.5f;
		}
		s_sprite_element *element = &tag->sprite->second;
		if (element)
		{
			draw_element_24bda2(element, placement->second_x + bounds.b, placement->second_y + bounds.c, x, y, scale, opacity,
				(short_rectangle2d const *)a);
		}
	}
}

