// @flags /O1 /Oi /arch:SSE /Gr
/* UNKNOWN_2C8F36.CPP: the screen that records a voice message
   for an xbox live message (vtable 0x45d560). Its five buttons record, stop the recording, play, stop the
   playback and leave; the message send list gives it the buffer to record
   into (unknown_2b7b48.cpp). */

#include "unknown_11c920.h"
#include <string.h>
#include "screen_widgets.h"
#include "unknown_223b60.h"
#include "unknown_234c64.h"

/* the user interface's time (the window manager's, 0x54d5b8) */
#define user_interface_time() ((dword)g_54d598.m20)

long function_1480ff(long screen_id);

/* network_voice.cpp. Its voice_record_voice_mail takes what XHV's
   VoiceMailRecord takes: the port, the longest time, the buffer's size and
   the buffer, then where it reports the size and the time recorded */
void voice_set_port_mode(long port, long mode);
long voice_get_port_mode(long port);
bool __stdcall voice_mail_is_active(long port);
void voice_record_voice_mail(long port, dword maximum_time, dword buffer_size, byte *buffer, dword *size, dword *duration);
void voice_do_work(void);
void voice_mail_stop_if_present(long port);
void voice_play_voice_mail(long port, const long *data, long size);
void voice_mail_stop(long port);

/* unknown_223b60.cpp */
timing_counter *timing_counter_start(timing_counter *c);
__int64 timing_counter_resume(timing_counter *c);
__int64 timing_counter_stop(timing_counter *c);

#define PIN(n, floor, ceiling) ((n) < (floor) ? (floor) : ((n) > (ceiling) ? (ceiling) : (n)))

enum
{
	_voice_record_state_none = 0,
	_voice_record_state_recording,
	_voice_record_state_recorded,
	_voice_record_state_playing
};

/* the controllers after this one (NONE after the last) */
static __forceinline long controller_next(long controller)
{
	long next = NONE;

	if (controller >= 0 && controller < 3)
	{
		next = controller + 1;
	}
	return next;
}

class __single_inheritance c_voice_message_record_screen;

/* a button's handler (vtable 0x45bdb0, folded with the list item
   handlers') */
class c_voice_message_record_handler : public c_list_item_delegate
{
public:
	typedef void (c_voice_message_record_screen::*method_t)(s_controller_reference **controller, long *item);

	c_voice_message_record_handler(c_voice_message_record_screen *owner, method_t method) :
		owner(owner),
		method(method)
	{
	}
	virtual void invoke(s_controller_reference **controller, long *item)
	{
		(owner->*method)(controller, item);
	}

	c_voice_message_record_screen *owner;
	method_t method;
};

/* the buttons of the screen (vtable 0x45d5d0) */
class c_button_widget_45d5d0 : public c_class_19b8b1
{
public:
	c_button_widget_45d5d0(short valuef8, word user_flags);

	/* move the focus to the next (or previous) shown button, wrapping round */
	virtual void v8();
	virtual void v9();
};

/* the voice message record screen (vtable 0x45d560) */
class c_voice_message_record_screen : public c_class_1473c9
{
public:
	c_voice_message_record_screen(long a, long b, word user_flags);
	~c_voice_message_record_screen();

	/* gives the controller its voice port mode back */
	virtual void v2();
	/* notices a recording or a playback that has ended */
	virtual void v3();
	/* a press of B or back stops and leaves */
	virtual bool v10(s_widget_event *event);
	/* builds the screen around its buttons */
	virtual void v18(void *parameters);
	virtual screen_load_proc get_load_proc();

	void update_title();
	void update_progress();
	bool record(s_controller_reference *controller);
	void stop_recording(long controller_index);
	bool finish_recording(s_controller_reference *controller);
	bool play(s_controller_reference *controller);
	bool stop_playing(s_controller_reference *controller);
	bool reset(s_controller_reference *controller);
	bool leave(s_controller_reference *controller);
	void update_buttons();

	/* the buttons' handlers */
	void handle_record(s_controller_reference **controller, long *item);
	void handle_stop(s_controller_reference **controller, long *item);
	void handle_play(s_controller_reference **controller, long *item);
	/* folded with handle_stop */
	void handle_stop_playing(s_controller_reference **controller, long *item);
	void handle_done(s_controller_reference **controller, long *item);

	long state;
	dword start_time;
	dword stop_time;
	byte *buffer;
	dword buffer_size;
	dword *size;
	dword *duration;
	long port_mode;
	c_button_widget_45d5d0 record_button;
	c_button_widget_45d5d0 stop_button;
	c_button_widget_45d5d0 play_button;
	c_button_widget_45d5d0 stop_playing_button;
	c_button_widget_45d5d0 done_button;
	c_voice_message_record_handler record_handler;
	c_voice_message_record_handler stop_handler;
	c_voice_message_record_handler play_handler;
	c_voice_message_record_handler stop_playing_handler;
	c_voice_message_record_handler done_handler;
	bool recording[4];
};

c_class_1473c9 *__stdcall function_2c9012(s_screen_parameters *parameters);

// @retail 0x2c8f36
c_button_widget_45d5d0::c_button_widget_45d5d0(short valuef8, word user_flags) :
	c_class_19b8b1(valuef8, user_flags)
{
}

// score 2 (was 2): only the loop's shared tail differs: retail copies the widget to ESI and tests ESI, ours
// tests EAX before the copy (`test eax, eax; mov esi, eax`).
// Before round 9, score 12 (was 20): only the entry differs: retail finds the first widget in EAX/EDX, saves ESI after it
// and joins the loop's `mov esi, eax; test esi, esi` tail; ours finds it straight into ESI (push first).
// Forms with the old candidate copied at the loop head (all 20 unless given): entry/repeated/both field
// walks; entry helper only 43; shared edge cursor; earlier entry edge lifetime; widget edge iteration;
// nested eligibility; separate eligibility inline 46, noinline 20; focus inline/noinline; success break;
// entry origin result 45; focus after search 49; complete eligibility; both wrapped helpers 54; helper
// search for 45; helper cursor while 45; edge-field walk 47; common-return edge walk 44; scoped edge
// iterator 45; ordinary inline helper 54; helper selection-first search 42; owner snapshot and tail
// update; step helper shared return 40; same-parameter cursor 28; assigned-result links and predicate 21;
// link for walks; repeated-step helper 49; search for 43; eligibility loop 43; selected result; rejection
// continue; local names; entry scope 43; separate entry/step results with for-walks or cursor walks 44;
// goto-walk wrapped helper for the entry, step in place 42 (92 bytes); one candidate for both scans;
// original head with for-walks 20; out-pointer step helper 20; single-exit forceinline next-or-first helper
// for entry and step 44 (96 bytes), plain inline 74 (61); widget local at the loop top 28; head test
// with a do-while 28; forceinline eligibility helper 20.
// The ternary step (`widget = next ? next : first sibling`) keeps the body as retail: 12; the same through a
// next-or-first ternary helper 12.
// Round 9: the shared next_wrapped (screen_widgets.h, a single-exit `result` set in an if/else) at the
// entry and the step 2 (the same form took 0x22ebc3/0x22ec1b 12 -> 2).
// Round 39: a done flag in the condition (`!done && widget`) 34 (90 bytes, unrotated); the step folded into the
// condition from `this` 66 (69 bytes); the eligibility inverted with the step first 2; `for` with a `continue`
// per failed test 2; the eligibility as a named bool 2. Harness: an early-return next-or-first helper at both 12
// (retail's tail, entry coalesced into ESI); next_wrapped entry with that step 37 (98 bytes).
// Round 62: condition assignment/declaration 2/2; candidate-updated declaration 28 (96 bytes); for assignment 2;
// implicit/reversed assignment 2/2; assignment with shared focus exit 39 (100); declaration with shared focus exit
// 49 (102). Opening body restored.
// Lane 13 r48 (fast, both twins, 2 each, the same code): the search as a guarded do-while with the step
// assigned in its condition (`while ((widget = widget->next_wrapped()) != NULL)`); a `goto found` single exit
// after the while loop; a guarded do-while with the step at the bottom; a `for` loop with the step in its
// header and `goto found`; `widget` declared before a `for` that assigns it. Loop syntax never reaches the copy:
// retail tests ESI after the merged phi copy (copies in the predecessors, merged after scheduling), ours
// schedules the test ahead of the copy inside one block.
// @retail 0x2c8f56
void c_button_widget_45d5d0::v8()
{
	c_class_1a2c81 *widget = next_wrapped();
	while (widget)
	{
		if (widget->has_valid_type() && widget->value6e)
		{
			function_22ecb4_2(false);
			widget->function_22ecb4_2(true);
			return;
		}
		widget = widget->next_wrapped();
	}
}

// score 2 (was 2): only the loop's shared tail differs: retail copies the widget to ESI and tests ESI, ours
// tests EAX before the copy (`test eax, eax; mov esi, eax`).
// Before round 9, score 12 (was 20): only the entry differs: retail finds the first widget in EAX/EDX, saves ESI after it
// and joins the loop's `mov esi, eax; test esi, esi` tail; ours finds it straight into ESI (push first).
// Forms with the old candidate copied at the loop head (all 20 unless given): entry/repeated/both field
// walks; entry helper only 43; shared edge cursor; earlier entry edge lifetime; widget edge iteration;
// nested eligibility; separate eligibility inline 46, noinline 20; focus inline/noinline; success break;
// entry origin result 45; focus after search 49; complete eligibility; both wrapped helpers 54; helper
// search for 45; helper cursor while 45; edge-field walk 47; common-return edge walk 44; scoped edge
// iterator 45; ordinary inline helper 54; helper selection-first search 42; owner snapshot and tail
// update; step helper shared return 40; same-parameter cursor 28; assigned-result links and predicate 21;
// link for walks; repeated-step helper 49; search for 43; eligibility loop 43; selected result; rejection
// continue; local names; entry scope 43; separate entry/step results with for-walks or cursor walks 44;
// goto-walk wrapped helper for the entry, step in place 42 (92 bytes); one candidate for both scans;
// original head with the step in its own result copied to the candidate 44; out-pointer step helper 20; single-exit forceinline previous-or-last helper
// for entry and step 44 (96 bytes), plain inline 74 (61); widget local at the loop top 28; head test
// with a do-while 28; forceinline eligibility helper 20.
// The ternary step (`widget = previous ? previous : last sibling`) keeps the body as retail: 12; the same through
// a previous-or-last ternary helper 12.
// Round 9: the shared previous_wrapped (screen_widgets.h, a single-exit `result` set in an if/else) at the
// entry and the step 2 (the same form took 0x22ebc3/0x22ec1b 12 -> 2).
// Round 39: a done flag in the condition (`!done && widget`) 34 (90 bytes, unrotated); the step folded into the
// condition from `this` 66 (69 bytes); the eligibility inverted with the step first 2; `for` with a `continue`
// per failed test 2; the eligibility as a named bool 2. Harness: an early-return previous-or-last helper at both 12
// (retail's tail, entry coalesced into ESI); previous_wrapped entry with that step 37 (98 bytes).
// Lane 13 r48 (fast, both twins, 2 each, the same code): the search as a guarded do-while with the step
// assigned in its condition (`while ((widget = widget->previous_wrapped()) != NULL)`); a `goto found` single exit
// after the while loop; a guarded do-while with the step at the bottom; a `for` loop with the step in its
// header and `goto found`; `widget` declared before a `for` that assigns it. Loop syntax never reaches the copy:
// retail tests ESI after the merged phi copy (copies in the predecessors, merged after scheduling), ours
// schedules the test ahead of the copy inside one block.
// @retail 0x2c8fb4
void c_button_widget_45d5d0::v9()
{
	c_class_1a2c81 *widget = previous_wrapped();
	while (widget)
	{
		if (widget->has_valid_type() && widget->value6e)
		{
			function_22ecb4_2(false);
			widget->function_22ecb4_2(true);
			return;
		}
		widget = widget->previous_wrapped();
	}
}

// @retail 0x2c8f50
screen_load_proc c_voice_message_record_screen::get_load_proc()
{
	return function_2c9012;
}

// @retail 0x2c9012
c_class_1473c9 *__stdcall function_2c9012(s_screen_parameters *parameters)
{
	c_voice_message_record_screen *screen = new c_voice_message_record_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2c9054
c_voice_message_record_screen::c_voice_message_record_screen(long a, long b, word user_flags) :
	c_class_1473c9(0x24, a, b, user_flags),
	state(_voice_record_state_none),
	start_time(0),
	stop_time(0),
	buffer(0),
	buffer_size(0),
	size(0),
	duration(0),
	port_mode(0),
	record_button(0, user_flags),
	stop_button(1, user_flags),
	play_button(2, user_flags),
	stop_playing_button(3, user_flags),
	done_button(4, user_flags),
	record_handler(this, &c_voice_message_record_screen::handle_record),
	stop_handler(this, &c_voice_message_record_screen::handle_stop),
	play_handler(this, &c_voice_message_record_screen::handle_play),
	stop_playing_handler(this, &c_voice_message_record_screen::handle_stop_playing),
	done_handler(this, &c_voice_message_record_screen::handle_done)
{
	memset(recording, 0, sizeof(recording));
}

// @retail 0x2c918f deleting

/* stops the recordings still under way */
// @retail 0x2c91ad
c_voice_message_record_screen::~c_voice_message_record_screen()
{
	for (long controller = 0; controller != NONE; controller = controller_next(controller))
	{
		if (recording[controller])
		{
			stop_recording(controller);
		}
	}
}

// @retail 0x2c9285
void c_voice_message_record_screen::v18(void *parameters)
{
	volatile long definition_index = function_1480ff(screen_id);
	c_class_1a2c81 *buttons[5] = { &record_button, &stop_button, &play_button, &stop_playing_button, &done_button };
	s_screen_layout layout =
	{
		0,
		1,
		{
			{ 5, buttons, 0, 0 }
		}
	};

	build(&layout);
	c_class_1a2c81::v1();
	if (record_button.parent == this)
	{
		v7(&record_button);
		port_mode = voice_get_port_mode(get_controller_index());
		voice_set_port_mode(get_controller_index(), 3);
		delegate_register(&record_button.handlers, &record_handler);
		delegate_register(&stop_button.handlers, &stop_handler);
		delegate_register(&play_button.handlers, &play_handler);
		delegate_register(&stop_playing_button.handlers, &stop_playing_handler);
		delegate_register(&done_button.handlers, &done_handler);
		update_buttons();
	}
	else
	{
		start_animation(3);
	}
}

// @retail 0x2c93a7
void c_voice_message_record_screen::v2()
{
	voice_set_port_mode(get_controller_index(), port_mode);
	c_class_1a2c81::v2();
}

// @retail 0x2c93ca
void c_voice_message_record_screen::v3()
{
	long current = state;

	if (current == _voice_record_state_playing && !voice_mail_is_active(get_controller_index()))
	{
		goto stopped;
	}
	if (current == _voice_record_state_recording && !voice_mail_is_active(get_controller_index()))
	{
		for (long controller = 0; controller < 4; controller++)
			recording[controller] = false;
	stopped:
		state = _voice_record_state_recorded;
		stop_time = user_interface_time();
		update_buttons();
	}
	update_title();
	update_progress();
	c_class_1a2c81::v3();
}

/* the text says what the screen is doing */
// @retail 0x2c9438
void c_voice_message_record_screen::update_title()
{
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)find_text(1);

	if (text)
	{
		long string_handle;

		switch (state)
		{
		case _voice_record_state_none:
			string_handle = 0x13000603;
			break;
		case _voice_record_state_recording:
			string_handle = 0x19000604;
			break;
		case _voice_record_state_recorded:
			string_handle = 0x16000605;
			break;
		case _voice_record_state_playing:
			string_handle = 0x18000606;
			break;
		default:
			return;
		}
		text->function_253b1a(string_handle);
	}
}

/* the bar shows how much of the fifteen seconds the message takes */
// @retail 0x2c9482
void c_voice_message_record_screen::update_progress()
{
	c_class_2b01eb *bar = (c_class_2b01eb *)find_bitmap(5);

	if (bar)
	{
		dword elapsed = 0;
		real fraction;

		switch (state)
		{
		case _voice_record_state_recording:
		case _voice_record_state_playing:
			elapsed = user_interface_time() - start_time;
			break;
		case _voice_record_state_recorded:
			elapsed = stop_time - start_time;
			break;
		}
		fraction = (real)elapsed / 15000.0f;
		if (fraction < 0.0f)
		{
			fraction = 0.0f;
		}
		else if (fraction > 1.0f)
		{
			fraction = 1.0f;
		}
		bar->value84 = fraction;
	}
}

// @retail 0x2c9501
bool c_voice_message_record_screen::v10(s_widget_event *event)
{
	bool result = false;

	if (event->type == 5)
	{
		switch (event->param)
		{
		case 1:
		case 13:
			result = leave((s_controller_reference *)event);
			break;
		}
	}
	if (!result)
	{
		result = c_class_1473c9::v10(event);
	}
	return result;
}

/* records into the message's buffer */
// @retail 0x2c9532
bool c_voice_message_record_screen::record(s_controller_reference *controller)
{
	if (state == _voice_record_state_none)
	{
		if (size)
		{
			*size = 0;
		}
		if (duration)
		{
			*duration = 0;
		}
		voice_record_voice_mail(controller->controller_index, 15000, buffer_size, buffer, size, duration);
		state = _voice_record_state_recording;
		start_time = user_interface_time();
		stop_time = 0;
		recording[controller->controller_index] = true;
	}
	update_buttons();
	return true;
}

/* stops a controller's recording and waits for the voice code to finish */
// @retail 0x2c95ac
void c_voice_message_record_screen::stop_recording(long controller_index)
{
	if (state == _voice_record_state_recording)
	{
		timing_counter counter;

		voice_mail_stop_if_present(controller_index);
		timing_counter_start(&counter);
		timing_counter_resume(&counter);
		while (voice_mail_is_active(controller_index))
		{
			voice_do_work();
		}
		timing_counter_stop(&counter);
		state = _voice_record_state_recorded;
		stop_time = user_interface_time();
		recording[controller_index] = false;
	}
}

// @retail 0x2c9609
bool c_voice_message_record_screen::finish_recording(s_controller_reference *controller)
{
	stop_recording(controller->controller_index);
	update_buttons();
	return true;
}

// @retail 0x2c961d
bool c_voice_message_record_screen::play(s_controller_reference *controller)
{
	s_controller_reference *const *controller_reference = &controller;

	if (state == _voice_record_state_recorded)
	{
		voice_play_voice_mail((*controller_reference)->controller_index, (const long *)buffer, size ? *size : buffer_size);
		state = _voice_record_state_playing;
		start_time = user_interface_time();
		stop_time = 0;
	}
	update_buttons();
	return true;
}

// @retail 0x2c9677
bool c_voice_message_record_screen::stop_playing(s_controller_reference *controller)
{
	if (state == _voice_record_state_playing)
	{
		voice_mail_stop(controller->controller_index);
		state = _voice_record_state_recorded;
		stop_time = user_interface_time();
	}
	update_buttons();
	return true;
}

/* stops whatever is under way and forgets the recording */
// @retail 0x2c96ab
bool c_voice_message_record_screen::reset(s_controller_reference *controller)
{
	if (state == _voice_record_state_playing)
	{
		stop_playing(controller);
	}
	else if (state == _voice_record_state_recording)
	{
		finish_recording(controller);
	}
	state = _voice_record_state_none;
	start_time = 0;
	stop_time = 0;
	if (size)
	{
		*size = 0;
	}
	if (duration)
	{
		*duration = 0;
	}
	if (buffer && buffer_size > 0)
	{
		memset(buffer, 0, buffer_size);
	}
	update_buttons();
	return true;
}

// @retail 0x2c9731
bool c_voice_message_record_screen::leave(s_controller_reference *controller)
{
	reset(controller);
	start_animation(3);
	return true;
}

/* shows the buttons the state allows, moving the focus off the hidden ones */
// @retail 0x2c974c
void c_voice_message_record_screen::update_buttons()
{
	switch (state)
	{
	case _voice_record_state_none:
	case _voice_record_state_recorded:
		record_button.value6e = true;
		stop_button.value6e = false;
		if (stop_button.is_in_window())
		{
			record_button.function_22ecb4_2(true);
		}
		play_button.value6e = true;
		stop_playing_button.value6e = false;
		if (stop_playing_button.is_in_window())
		{
			play_button.function_22ecb4_2(true);
		}
		break;
	case _voice_record_state_recording:
		record_button.value6e = false;
		if (record_button.is_in_window())
		{
			stop_button.function_22ecb4_2(true);
		}
		stop_button.value6e = true;
		play_button.value6e = true;
		stop_playing_button.value6e = false;
		if (stop_playing_button.is_in_window())
		{
			play_button.function_22ecb4_2(true);
		}
		break;
	case _voice_record_state_playing:
		record_button.value6e = true;
		stop_button.value6e = false;
		if (stop_button.is_in_window())
		{
			record_button.value6e = true;
		}
		play_button.value6e = false;
		if (play_button.is_in_window())
		{
			stop_playing_button.function_22ecb4_2(true);
		}
		stop_playing_button.value6e = true;
		break;
	}
}

/* the record button: records, stops, or records again */
// @retail 0x2c983f
void c_voice_message_record_screen::handle_record(s_controller_reference **controller, long *item)
{
	switch (state)
	{
	case _voice_record_state_none:
		record(*controller);
		break;
	case _voice_record_state_recording:
		finish_recording(*controller);
		break;
	case _voice_record_state_recorded:
		reset(*controller);
		record(*controller);
		break;
	}
}

/* the stop button */
// @retail 0x2c988f
void c_voice_message_record_screen::handle_stop(s_controller_reference **controller, long *item)
{
	switch (state)
	{
	case _voice_record_state_recording:
		finish_recording(*controller);
		break;
	case _voice_record_state_playing:
		stop_playing(*controller);
		break;
	}
}

/* the stop playing button (retail folded it with handle_stop) */
void c_voice_message_record_screen::handle_stop_playing(s_controller_reference **controller, long *item)
{
	switch (state)
	{
	case _voice_record_state_recording:
		finish_recording(*controller);
		break;
	case _voice_record_state_playing:
		stop_playing(*controller);
		break;
	}
}

// @retail 0x2c98b7
void c_voice_message_record_screen::handle_play(s_controller_reference **controller, long *item)
{
	switch (state)
	{
	case _voice_record_state_recorded:
		play(*controller);
		break;
	case _voice_record_state_playing:
		stop_playing(*controller);
		break;
	}
}

/* the last button: leaves (forgetting the recording when nothing was
   recorded) */
// @retail 0x2c98e1
void c_voice_message_record_screen::handle_done(s_controller_reference **controller, long *item)
{
	switch (state)
	{
	case _voice_record_state_none:
		leave(*controller);
		break;
	case _voice_record_state_recording:
		finish_recording(*controller);
		break;
	case _voice_record_state_recorded:
		start_animation(3);
		break;
	case _voice_record_state_playing:
		stop_playing(*controller);
		break;
	}
}
