#include "filter_storage.h"
// @flags /O1 /Oi /arch:SSE /Gr
/* UNKNOWN_234C64.CPP: the window manager's screen channels: each holds the
   current screen, the next one, the previous one and a pending request */

#include "unknown_11c920.h"
#include <xtl.h>
#include <new>
#include <string.h>
#include <stdlib.h>
#include "unknown_234c64.h"
#include "unknown_2b116a.h"
#include "globals.h"
#include "unknown_24b5bc.h"

/* ---- globals ---- */

char g_54d5a8;
extern dword g_54d5b8;
real g_54d5ac;
real g_54d5b0;
real g_54d5b4;

/* ---- externals ---- */

bool function_6c7e0();
long function_147f4f(void);
s_type_954545 *ui_definition_get(void);
long function_199d7c(void);
word function_19022f(void);
c_class_1473c9 *__stdcall function_2b72e6(s_screen_parameters *request);
c_class_1473c9 *__stdcall function_2b7333(s_screen_parameters *request);
bool screen_higher_channel_window_in_use(c_class_1473c9 *screen);
real screen_get_fade(c_class_1473c9 *screen);
__declspec(noinline) void function_235756(real fade);
void function_2359ce(c_window_channel_459a34 *channel);
void function_235abc(c_window_channel_459a34 *channel);
c_class_1473c9 *__stdcall function_2b739a(s_screen_parameters *parameters);

void function_234dd1(c_window_channel *channel);
bool function_235246(c_window_channel *channel);
void function_2353a5(c_class_1473c9 *screen, long window);

#define REAL_TO_LONG(x, result) __asm { fld x } __asm { fistp result }

/* ---- the base channel ---- */

// @retail 0x234e43
c_window_channel::c_window_channel() :
	m4(4),
	current(0),
	next(0),
	previous(0),
	focus(0)
{
}

// @retail 0x234e64
void c_window_channel::clear()
{
	current = 0;
	next = 0;
	memset(&request, 0, sizeof(request));
	previous = 0;
	focus = 0;
}

// @retail 0x234dc7
void c_window_channel::reset()
{
	dispose();
}

// @retail 0x234e81
void c_window_channel::dispose()
{
	if (current)
	{
		function_148148(current);
		current = 0;
	}
	if (next)
	{
		function_148148(next);
		next = 0;
	}
	if (previous)
	{
		function_148148(previous);
		previous = 0;
	}
	focus = 0;
}

// @retail 0x234ebc
void c_window_channel::update()
{
	if (next)
	{
		bool modal = TEST_FIELD_BIT(request.type_bit0);
		next->v17();
		((c_widget *)next)->function_22ecb4(true);
		next->v18(&request);
		next->v19();
		if (modal)
		{
			next->function_22e957(1);
			next->v25((s_screen_focus *)&request.id);
		}
		else
		{
			next->function_22e957(0);
		}
		if (current)
		{
			if (previous)
				function_148148(previous);
			if (current)
			{
				previous = current;
				if (modal)
					current->function_22e957(2);
				else
					current->function_22e957(3);
			}
		}
		current = next;
		next = 0;
		if (!current->is_in_window())
			((c_widget *)current)->function_22ecb4(true);
	}
	if (next)
		next->v3();
	if (current)
		current->v3();
	if (previous)
		previous->v3();
}

// @retail 0x234f8c
void c_window_channel::render(long window)
{
	s_type_954545 *globals = ui_definition_get();
	bool fading = false;
	real fade = 0.f;
	short mode = g_54d5a8;

	if (previous && screen_higher_channel_window_in_use(previous))
	{
		fade = screen_get_fade(previous);
		fading = true;
	}
	if (current && screen_higher_channel_window_in_use(current))
	{
		fade += screen_get_fade(current);
		fading = true;
	}
	if (next && screen_higher_channel_window_in_use(next))
	{
		fade += screen_get_fade(next);
		fading = true;
	}

	bool tint = fading;
	long widened_mode = mode;
	long const *mode_reference = &widened_mode;
	if (*mode_reference >= 0 && *mode_reference <= 1)
	{
		if (!tint && current)
		{
			fade += screen_get_fade(current);
			tint = current->screen_id != 0xd4;
		}
		fading = false;
	}

	if (fade < 0.f)
		fade = 0.f;
	else if (fade > 1.f)
		fade = 1.f;

	dword color;
	if (globals)
	{
		long alpha, red, green, blue;
		real value;
		value = globals->tint.alpha * fade * 255.f;
		REAL_TO_LONG(value, alpha);
		value = globals->tint.red * 255.f;
		REAL_TO_LONG(value, red);
		value = globals->tint.green * 255.f;
		REAL_TO_LONG(value, green);
		value = globals->tint.blue * 255.f;
		REAL_TO_LONG(value, blue);
		color = (((alpha << 8 | red) << 8 | green) << 8) | blue;
	}
	else
	{
		long alpha;
		real value = (real)(fade * 191.25);
		REAL_TO_LONG(value, alpha);
		color = alpha << 24 | 0xf;
	}

	if (tint)
		((c_render_window *)window)->function_147cdb(color);
	if (fading)
		function_235756(fade);
	if (previous)
		function_2353a5(previous, window);
	if (current)
		function_2353a5(current, window);
	if (next)
		function_2353a5(next, window);
}

// @retail 0x23519d
void c_window_channel::set_next(c_class_1473c9 *screen, s_screen_parameters *new_request)
{
	if (next == screen)
		next = 0;
	if (next)
	{
		function_148148(next);
		next = 0;
	}
	next = screen;
	(s_screen_parameters &)request = *new_request;
}

// @retail 0x2351d4
void c_window_channel::v7()
{
	if (!ANIMATION_FLAG(current->animation, 1))
		current->function_22e957(2);
}

// @retail 0x2351ea
void c_window_channel::remove(c_class_1473c9 *screen)
{
	if (screen == current)
		current = 0;
	if (screen == next)
		next = 0;
	if (screen == previous)
		previous = 0;
	if (focus && (focus == screen || focus->get_screen() == screen))
		focus = 0;
	if (screen->value5f2)
		screen->v2();
	screen->~c_class_1473c9();
	function_1a4826(screen);
}

// @retail 0x2352d3
void c_window_channel::v9()
{
	if (current)
	{
		word users = current->user_flags;
		long load = (long)current->get_load_proc();
		long b = current->v21();
		long a = current->v20();
		function_149f49((s_message *)&request, 0x7fff, 0, users, a, b, load);
	}
	dispose();
}

// score 3 (was 3): after the call to 0x149f49, retail recomputes the
// request's address (lea eax, [ebp - 0x20]); ours uses mov eax, ecx. The
// callee leaves ecx unchanged, and retail's 0x2358c3 in this file reuses it
// after the same call. One tested link-order change left the score unchanged.
// Other locals, address forms and control flow scored 3 or worse; the 600 s
// permutation run found no improvement.
// Round 419: callback from the completed request, derived/base request storage,
// native multi-use request pointer and direct callback-result subsets 4/25/4/25
// and 4/25/4/25. All regress; original callback observation and body restored.
// A twice-used callback snapshot, with both request types and both result
// declaration orders, scored 29 in all eight forms (79/78 bytes).
// Round 26 (lane 10): a header inline method on s_screen_request calling 0x149f49 on `this`,
// used by v9 and v10, matched both; not kept (an inline twin of an existing function used only
// where it helps). Request at function scope 13; `request.type = 0` after the call 12.
// Round 78: type-field callback address 3; shared request pointer/reference 3/3;
// message-cast callback 3; result before builder/at function scope 3/3; type-field
// builder and callback 3; shared message pointer 3. Original body restored.
// Round 7 (m3 lane 2): every other caller of 0x149f49 that uses its request afterwards reuses ECX
// (they call the local request's own load field). The request at function scope 13; type cleared after
// the builder call 12; builder through a forced-inline helper's parameter 3; type cleared before the
// request's declaration 5; `screen` declared first and assigned later 3.
// Round 206: pending-record pointer/reference bound at entry/in branch 3/3/3/3;
// complete pending-record snapshot with live callback 47, with retained callback 47,
// at entry 47, after type clear 43. Snapshots add a 0x40 frame; original restored.
// Round 208: builder mask unsigned 3 (caller loss); native message timeout; native message pointer
// 3; value construction 3; one-element request array 3; one-element message array 3; local names 3;
// native result declared early 3; immutable result 3. All bodies and declarations restored; the
// request-address reload remains.
// Round 321: Native message storage with admission scope/early screen/zero-store boundary subsets: 3/3/3/3/5/5/5/5.
// Round 321: The address reload still differs; original derived request, callback and virtual dispatch restored.
// Round 56: inline definition 3; array-decay id 3; user-mask/channel/window locals 3/3/3;
// explicit pending receiver 3; explicit callback dereference 3; explicit setter receiver 3.
// Round 43 (lane 10): plain parameters local with its own field_c store 3; the builder given the
// first member's address 3; one base pointer for the callback and setter (callback result passed
// directly) 25; early return with the request at function scope 13, declared after the test 3.
// @retail 0x23531c
void c_window_channel::v10()
{
	if (request.type == 0x7fff)
	{
		s_screen_request new_request;
		request.type = 0;
		function_149f49((s_message *)&new_request, 0, (dword *)&request.id, request.user_flags, request.a, request.b, (long)request.load);
		c_class_1473c9 *screen = request.load(&new_request);
		set_next(screen, &new_request);
	}
}

/* ---- helpers the window manager calls ---- */

static __forceinline c_class_1a2c81 *widget_parent_for_user(c_class_1a2c81 *screen, long user)
{
	long users = 1 << user;
	do
	{
		if (users & (short)screen->user_flags)
			break;
		screen = screen->parent;
	}
	while (screen);
	return screen;
}

// @retail 0x23515d
bool function_23515d(c_window_channel *channel, s_event *event)
{
	if (function_235246(channel))
	{
		c_class_1a2c81 *screen = channel->focus;
		if (screen)
		{
			screen = widget_parent_for_user(screen, event->unknown04);
		}
		if (screen)
			return screen->v10((s_widget_event *)event);
		return false;
	}
	return true;
}

// @retail 0x235246
bool function_235246(c_window_channel *channel)
{
	bool result = false;
	if (channel->focus)
	{
		c_class_1473c9 *root = channel->focus->get_screen();
		if (root && root == channel->current && !ANIMATION_FLAG(root->animation, 1) && !TEST_FIELD_BIT(root->animation.flags.flag0))
			result = true;
	}
	return result;
}

// @retail 0x2352c0
void function_2352c0(c_window_channel *channel)
{
	if (channel->previous)
	{
		function_148148(channel->previous);
		channel->previous = 0;
	}
}

// @retail 0x23536a
void function_23536a(c_window_channel *channel, c_class_1473c9 *screen)
{
	if (screen)
	{
		c_class_1473c9 *root = screen->get_screen();
		if ((!root || root != channel->current && root != channel->next) && screen->type)
			return;
	}
	channel->focus = screen;
}

// @retail 0x23538b
void function_23538b(c_window_channel *channel)
{
	c_class_1473c9 *screen = channel->current;
	if (screen && !ANIMATION_FLAG(screen->animation, 1))
		screen->function_22e957(3);
}

/* ---- the transition channel ---- */

// @retail 0x234c64
c_window_channel_45997c::c_window_channel_45997c()
{
	state = NONE;
	m60 = 250;
	memset(&m38, 0, 0x28);
}

// @retail 0x234c8b
void c_window_channel_45997c::clear()
{
	memset(&m38, 0, 0x28);
	state = NONE;
	c_window_channel::clear();
}

// @retail 0x234ca4
void c_window_channel_45997c::v2()
{
	function_234dd1(this);
	state = NONE;
}

// @retail 0x234cb3
void c_window_channel_45997c::dispose()
{
	c_window_channel::dispose();
	memset(&m38, 0, 0x28);
	state = NONE;
}

// @retail 0x234ccd
void c_window_channel_45997c::update()
{
	if (g_54d5a8 == 2 && !current)
		function_234dd1(this);

	c_class_1473c9 *screen = current;
	if (screen)
	{
		long new_state;
		long id = function_147f4f();
		switch (id)
		{
		case NONE:
		case 9: new_state = 0; break;
		case 6: new_state = 1; break;
		case 11:
		case 19:
		case 30:
		case 186: new_state = 2; break;
		default: new_state = 3; break;
		}

		if (state != new_state)
		{
			real value;
			switch (new_state)
			{
			case 0:
				m3c = 0.f;
				value = 0.f;
				break;
			case 1:
				if (state >= 1)
				{
					m3c = 1.f;
					value = 0.f;
				}
				else
				{
					m3c = 0.f;
					value = 0.f;
				}
				break;
			case 2:
				if (!(state >= 2))
				{
					m3c = 0.f;
					value = 1.f;
				}
				else
				{
					m3c = 1.f;
					value = 1.f;
				}
				break;
			case 3:
				m3c = 1.f;
				value = 1.f;
				break;
			default:
				m3c = 0.f;
				value = 0.f;
				break;
			}
			m50 = value;
			screen->function_22e957(4);
			state = new_state;
		}
	}
	c_window_channel::update();
}

// @retail 0x234dcc
void c_window_channel_45997c::v10()
{
	v2();
}

// @retail 0x234dd1
void function_234dd1(c_window_channel *channel)
{
	s_screen_request request;

	request.type = 0;
	request.user_flags = 0;
	request.a = 6;
	request.b = 4;
	memset(&request.id, NONE, sizeof(request.id));
	request.load = 0;
	if (g_54d5a8 == 2)
	{
		request.load = function_2b72e6;
		if (request.load)
		{
			channel->current = request.load(&request);
			channel->current->v18(&request);
		}
	}
}

/* ---- the queued channel ---- */

// @retail 0x23546b
c_window_channel_4599a8::c_window_channel_4599a8()
{
	queue = 0;
	m3c = false;
	m3d = false;
}

// @retail 0x234e33
c_window_channel_234e33::c_window_channel_234e33()
{
}

// @retail 0x235486
void c_window_channel_4599a8::clear()
{
	queue = 0;
	c_window_channel::clear();
}

// @retail 0x235626
void function_235626(c_window_channel_4599a8 *channel, s_screen_parameters *request)
{
	s_queued_request *queued = channel->queue;
	*request = queued->request;
	channel->queue = queued->next;
	function_1a4826(queued);
}

// @retail 0x235647
void function_235647(c_window_channel_4599a8 *channel)
{
	s_screen_request request;
	while (channel->queue)
		function_235626(channel, &request);
}

// @retail 0x2355ed
void function_2355ed(c_window_channel_4599a8 *channel, s_screen_parameters *request, long window)
{
	s_queued_request *queued = new (function_1a47fd(sizeof(s_queued_request))) s_queued_request;
	if (queued)
	{
		(s_screen_parameters &)queued->request = *request;
		queued->next = channel->queue;
		queued->window = window;
		channel->queue = queued;
	}
}

// @retail 0x23548f
void c_window_channel_4599a8::dispose()
{
	if (!m3d)
		function_235647(this);
	c_window_channel::dispose();
	m3d = false;
}

// @retail 0x2354aa
void c_window_channel_4599a8::set_next(c_class_1473c9 *screen, s_screen_parameters *new_request)
{
	if (current && !(new_request->type & 6))
	{
		c_class_1473c9 *active = current;
		{
			s_screen_request previous_request;
			word flags = active->user_flags;
			screen_load_proc load = active->get_load_proc();
			long window = active->v21();
			long channel = active->v20();
			function_149f49((s_message *)&previous_request, 2, 0, flags, channel, window, (long)load);
			active->v24((s_screen_focus *)&previous_request.id);
			function_2355ed(this, &previous_request, active->screen_id);
		}
	}
	if (new_request->type & 4)
		function_235647(this);
	c_window_channel::set_next(screen, new_request);
}

// @retail 0x235537
void c_window_channel_4599a8::v7()
{
	v11(1);
}

// @retail 0x23553f
void c_window_channel_4599a8::v11(short count)
{
	s_screen_request request;

	if (count > 1)
	{
		long i = (word)(count - 1);
		do
		{
			if (queue)
				function_235626(this, &request);
		} while (--i);
	}
	if (queue)
	{
		function_235626(this, &request);
		request.type |= 3;
		request.load(&request);
	}
	else if (m3c && current)
	{
		if (previous)
		{
			function_148148(previous);
			previous = 0;
		}
		if (current)
		{
			previous = current;
			current->function_22e957(2);
		}
	}
}

// @retail 0x2355bd
short c_window_channel_4599a8::v12(long window)
{
	short index = 1;
	s_queued_request *queued;
	for (queued = queue; queued; queued = queued->next, index++)
	{
		if (queued->window == window)
			break;
	}
	if (queued)
		v11(index);
	else
		index = 0;
	return index;
}

// @retail 0x235661
void c_window_channel_4599a8::v9()
{
	m3d = true;
	c_window_channel::v9();
}

/* ---- 0x459a08 ---- */

// @retail 0x235795
c_window_channel_459a08::c_window_channel_459a08()
{
	memset(&m38, 0, 8);
	memset(&m38, 0, 8);
}

// @retail 0x2357b6
void c_window_channel_459a08::update()
{
	if (m38)
		memset(&m38, 0, 8);
	c_window_channel::update();
}

/* ---- 0x459a34 ---- */

/* the screen transition's animation keys (none for NONE) */
// @retail 0x234d9e
void *c_window_channel_45997c::get_transition(short index, long *value, short *count, short *frames)
{
	void *result = 0;

	if (index == NONE)
	{
		*value = 0;
		*count = 0;
		*frames = 0;
	}
	else
	{
		*value = m60;
		*count = 1;
		*frames = 2;
		result = &m38;
	}
	return result;
}

// @retail 0x2357c9
c_window_channel_2357c9::c_window_channel_2357c9()
{
}

// @retail 0x2357d9
c_window_channel_459a34::c_window_channel_459a34()
{
	m38 = 0;
	m3c = 0;
	m1c0 = 0;
	memset(slots, 0, sizeof(slots));
}

// @retail 0x235906
void function_235906(c_window_channel_459a34 *channel)
{
	if (channel->m38)
	{
		channel->remove(channel->m38);
		channel->m38 = 0;
	}
}

// @retail 0x2359a9
void function_2359a9(c_window_channel_459a34 *channel)
{
	if (channel->m3c)
	{
		channel->remove(channel->m3c);
		memset(channel->slots, 0, sizeof(channel->slots));
		channel->m3c = 0;
		channel->m1c0 = 0;
	}
}

/* the slot after this one, or NONE after the last */
static inline long channel_slot_next(long index)
{
	long result = NONE;

	if (index >= 0 && index < 3)
		result = index + 1;
	return result;
}

/* the sender's name of a message entry ("" for none) */
inline char const *message_entry_get_name(s_entry const *entry)
{
	char const *name = "";

	if (entry)
	{
		name = entry->name;
	}
	return name;
}

/* opens the notification screen on a slot's message */
// @retail 0x23591a
void function_23591a(c_window_channel_459a34 *channel, long index)
{
	if (!channel->m3c)
	{
		s_screen_request request;

		request.type = 0;
		request.user_flags = 0;
		request.a = 4;
		request.b = 4;
		memset(request.id, 0xff, sizeof(request.id));
		request.load = function_2b739a;
		channel->m3c = function_2b739a(&request);
		if (channel->m3c)
		{
			s_entry *entry = &channel->slots[index].message.entry;
			short bitmap;

			channel->m3c->start_animation(0);
			((c_screen_45bd40 *)channel->m3c)->set_text(message_entry_get_name(entry));
			bitmap = 6;
			switch (channel->slots[index].message.type)
			{
			case 1:
				bitmap = 1;
				break;
			case 2:
				bitmap = 0x12;
				break;
			}
			((c_screen_45bd40 *)channel->m3c)->set_bitmap(bitmap);
		}
	}
}

/* keeps the notification screen up while a slot's time lasts, else opens
   it on the first slot with a message */
// @retail 0x235abc
void function_235abc(c_window_channel_459a34 *channel)
{
	if (channel->m3c)
	{
		dword time = g_54d5b8;
		long index = 0;

		do
		{
			s_channel_slot *slot = &channel->slots[index];

			if (slot->time)
			{
				if (slot->time >= time)
				{
					channel->m3c->v3();
					return;
				}
				slot->time = 0;
			}
			index = channel_slot_next(index);
		} while (index != NONE);
		function_2359a9(channel);
	}
	else
	{
		long index = 0;

		do
		{
			if (channel->slots[index].time)
			{
				function_23591a(channel, index);
				if (channel->m3c)
					channel->m3c->v3();
				return;
			}
			index = channel_slot_next(index);
		} while (index != NONE);
	}
}

// @retail 0x235801
void c_window_channel_459a34::dispose()
{
	function_235906(this);
	function_2359a9(this);
	c_window_channel::dispose();
}

// @retail 0x2358c3
void function_2358c3(c_window_channel_459a34 *channel)
{
	s_screen_request request;
	function_149f49((s_message *)&request, 0, 0, function_19022f(), 4, 4, (long)function_2b7333);
	c_class_1473c9 *screen = request.load(&request);
	channel->m38 = screen;
	if (screen)
		screen->function_22e957(0);
}

// @retail 0x23586f
void function_23586f(c_window_channel_459a34 *channel)
{
	if (!function_1473b6(channel) && function_6c7e0() && !g_4670cd &&
		function_199d7c() != 6 && function_199d7c() != 7)
	{
		if (!channel->m38)
		{
			function_2358c3(channel);
			if (!channel->m38)
				return;
		}
		channel->m38->v3();
		return;
	}
	function_235906(channel);
}

bool function_6c7e0();
bool function_6d080(long controller_index, s_channel_message *message);

/* a player slot: set when the user's messages changed */
struct s_player_slot_messages_changed_view
{
	dword flags0 : 5;
	dword live : 1;
	dword : 26;
	byte unknown004[0x46d - 4];
	bool messages_changed;
	byte unknown46e[0xc70 - 0x46e];
};

/* once a second, takes each live user's newest message: a newer one replaces
   the slot's; a slot whose message went away is let go five seconds later */
// @retail 0x2359ce
void function_2359ce(c_window_channel_459a34 *channel)
{
	if (function_6c7e0())
	{
		dword time = g_54d5b8;

		if (time - channel->m1c0 >= 1000)
		{
			long index = 0;

			do
			{
				if (TEST_FIELD_BIT(((s_player_slot_messages_changed_view *)g_54e8e0)[index].live))
				{
					s_channel_message message;

					if (function_6d080(index, &message))
					{
						if (CompareFileTime((FILETIME const *)&message.entry.unknown30, (FILETIME const *)&channel->slots[index].message.entry.unknown30) == 1)
						{
							channel->slots[index].message = message;
						}
						channel->slots[index].shown = true;
						((s_player_slot_messages_changed_view *)g_54e8e0)[index].messages_changed = true;
					}
					else if (channel->slots[index].shown && !channel->slots[index].time)
					{
						channel->slots[index].time = g_54d5b8 + 5000;
						channel->slots[index].shown = false;
					}
				}
				index = next_controller_index(index);
			}
			while (index != NONE);
			channel->m1c0 = time;
		}
	}
}
// @retail 0x235816
void c_window_channel_459a34::update()
{
	if (g_54d5a8 == 2)
		function_23586f(this);
	function_2359ce(this);
	function_235abc(this);
	c_window_channel::update();
}

// @retail 0x23583e
void c_window_channel_459a34::render(long window)
{
	c_window_channel::render(window);
	if (m38)
		function_2353a5(m38, window);
	if (m3c)
		function_2353a5(m3c, window);
}

/* ---- drawing the screens back to front ---- */

// @retail 0x23566a
void __stdcall function_23566a(c_class_1a2c81 *screen, s_screen_sort_entry *entries, long *count)
{
	if (screen->v16())
	{
		for (c_class_1a2c81 *child = screen->child; child; child = child->next)
			function_23566a(child, entries, count);
		if (*count < 256)
		{
			entries[*count].screen = screen;
			entries[*count].depth = function_22e9aa((c_widget *)screen);
			entries[*count].layer = screen->value6a;
			(*count)++;
		}
	}
}

// @retail 0x2356d8
int __cdecl function_2356d8(void const *a, void const *b)
{
	s_screen_sort_entry const *x = (s_screen_sort_entry const *)a;
	s_screen_sort_entry const *y = (s_screen_sort_entry const *)b;
	int result;
	real difference = x->depth - y->depth;
	real distance = difference >= 0.f ? difference : 0.f - difference;

	if (distance <= 0.001f)
	{
		if (x->layer > y->layer)
			result = 1;
		else if (x->layer < y->layer)
			result = -1;
		else if (x->screen->type < y->screen->type)
			result = 1;
		else
			result = x->screen->type > y->screen->type ? -1 : 0;
	}
	else if (y->depth > x->depth)
		result = 1;
	else if (x->depth > y->depth)
		result = -1;
	else
		result = 0;
	return result;
}

// @retail 0x2353a5
void function_2353a5(c_class_1473c9 *screen, long window)
{
	if (!g_4670cd)
	{
		s_screen_sort_entry entries[256];
		long count = 0;

		function_23566a(screen, entries, &count);
		if (count > 0)
			qsort(entries, count, sizeof(s_screen_sort_entry), function_2356d8);
		screen->v22((void *)window);

		real nearest = g_54d5ac - g_54d5b0;
		real farthest = g_54d5b4 - g_54d5b0;
		for (long i = 0; i < count; i++)
		{
			if (entries[i].depth >= nearest && farthest >= entries[i].depth)
				entries[i].screen->v4(window);
		}
		screen->v23((void *)window);
	}
}

long function_211a0(long value, real alpha, real first, real second, long mode, long index, long flags, long option, long unused, real scale, real offset);

// @retail 0x235756
void function_235756(real fade)
{
	function_211a0(0, fade * 3.75f, 1.f, 1.f,
		1, NONE, 0, 1, 0, 0.25f, 0.f);
}
