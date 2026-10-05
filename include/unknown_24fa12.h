#ifndef UNKNOWN_24FA12_H
#define UNKNOWN_24FA12_H

#include "screen_widgets.h"

class c_session_user_state
{
public:
	c_session_user_state();
	union { bool active; bool valid; };
	byte unknown01;
	union { short index; short team; };
};

typedef void (c_class_1473c9::*session_choice_method)(s_controller_reference **controller, long *item);

class c_session_choice_handler : public c_list_item_delegate
{
public:
	c_session_choice_handler(c_class_1473c9 *owner, session_choice_method method) :
		owner(owner), method(method) {}
	virtual void invoke(s_controller_reference **controller, long *item);
	c_class_1473c9 *owner;
	session_choice_method method;
};

class c_screen_24fd74 : public c_class_1473c9
{
public:
	c_screen_24fd74(long a, long b, word user_flags);
	virtual screen_load_proc get_load_proc();
	void function_250155();
	void function_250eb7();
	void function_250cda(long index, bool update);
	void change_team(long index, long delta);
	void show_session_state();
	void update_countdown(bool signed_in_needed);
	void function_250f3a(byte *data);
	void function_2508a8();
	void handle_lobby_choice(s_controller_reference **controller, long *item);
	virtual bool v10(s_widget_event *event);

	c_class_19b8b1 button0;
	c_class_19b8b1 button1;
	bool value810;
	bool value811;
	byte value812;
	bool value813;
	c_session_user_state teams[4];
	s_text_256 text[6];
	long countdown_stage;
	dword countdown_time;
	short mode;
	bool value142e;
	c_session_choice_handler choice0;
	c_session_choice_handler choice1;
	bool value1460;
	long value1464;
	bool value1468;
	long value146c;
	dword state_shown_time;
};

inline void c_session_choice_handler::invoke(s_controller_reference **controller, long *item)
{
	(owner->*method)(controller, item);
}

#endif
