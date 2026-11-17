// @flags /O1 /Gr
/* UNKNOWN_2523E3.CPP: the screen where up to four users sign in (vtable
   0x45a490, screen 0x1e); its mode says what it signs them in for */

#include "unknown_11c920.h"
#include <string.h>
#include "screen_widgets.h"
#include "unknown_234c64.h"
#include "unknown_19b510.h"
#include "unknown_19b516.h"
#include "unknown_24b5bc.h"
#include "globals.h"

long function_1480ff(long screen_id);

class c_4way_signin_screen : public c_class_1473c9
{
public:
	c_4way_signin_screen(long a, long b, word user_flags);

	/* A, B and start of the users signed in and not */
	virtual bool v10(s_widget_event *event);
	virtual void v3();
	virtual void v18(void *parameters);
	virtual screen_load_proc get_load_proc();

	long mode;
};

struct s_player_profile { dword data[0x78]; };
struct s_widget_view_2b0a;
class c_class_2b0b5e;
bool controller_is_connected(short index);
void player_slot_get_profile(long index, s_player_profile *profile, long *profile_index);
long function_1249d0(short index);
long player_slot_iterate_next(long index);
void function_2b10a3(dword const *appearance, c_class_2b0b5e *widget, long type);
void function_2b0a14(s_widget_view_2b0a *widget, short index);
void function_22f042(s_widget_item *items, c_class_1a2c81 *widget, long count);

// score 55: the `s_widget_item items[4]` construction differs. Ours inlines the
// `flags = 0` constructor into a zeroing loop; retail calls the array-constructor
// helper 0x20670 with (constructor 0x2b0149, 4, 0x74, &items). Marking the
// constructor `__declspec(noinline)` gives 53, but the compiler still emits a
// per-element loop rather than the helper call. The helper choice is the front
// end's, not reachable from the constructor or array spelling.
// @retail 0x252554
void c_4way_signin_screen::v3()
{
    set_user_flags(0xffff);
    long controller = 0;
    s_widget_item items[4];
    do
    {
        bool connected = controller_is_connected((short)controller);
        bool signed_in = connected && TEST_FIELD_BIT(((s_player_slot_sign_in_view *)g_54e8e0)[controller].signed_in);
        bool live = signed_in && TEST_FIELD_BIT(((s_player_slot_sign_in_view *)g_54e8e0)[controller].live);
        dword const *appearance = 0;
        s_player_profile profile;
        long profile_index;
        if (signed_in)
        {
            player_slot_get_profile(controller, &profile, &profile_index);
            appearance = &profile.data[0x46];
            s_widget_item *item = &items[controller];
            item->flags |= 2;
            for (long word_index = 0; word_index < 4; ++word_index)
                item->value48[word_index] = appearance[word_index];
            item->flags |= 1;
            item->value4 = (long)&profile.data[2];
        }
        long name, connection, warning, live0, live1, live2, model0, model1, active_bitmap, missing_bitmap;
        switch (controller)
        {
        case 0:
            name=2; connection=22; warning=3; live0=10; live1=11; live2=12;
            model0=0; model1=4; active_bitmap=3; missing_bitmap=7;
            break;
        case 1:
            name=4; connection=23; warning=5; live0=13; live1=14; live2=15;
            model0=1; model1=5; active_bitmap=4; missing_bitmap=8;
            break;
        case 2:
            name=6; connection=24; warning=7; live0=16; live1=17; live2=18;
            model0=2; model1=6; active_bitmap=5; missing_bitmap=9;
            break;
        default:
            name=8; connection=25; warning=9; live0=19; live1=20; live2=21;
            model0=3; model1=7; active_bitmap=6; missing_bitmap=10;
            break;
        }
        set_child_value6e(6, (short)name, signed_in);
        c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)find_child(6, (short)connection, false);
        if (text)
        {
            if (connected)
            {
                if (signed_in)
                    text->value6e = false;
                else
                {
                    text->value6e = true;
                    text->function_253b1a(0xf00028a);
                }
            }
            else
            {
                text->value6e = true;
                text->function_253b1a(0x1100028b);
            }
        }
        text = (c_text_widget_45a5e0 *)find_child(6, (short)warning, false);
        if (text)
        {
            if (signed_in)
            {
                text->value6e = true;
                text->function_253b1a(0x1300028c);
            }
            else
                text->value6e = false;
        }
        set_child_value6e(6, (short)live0, signed_in);
        set_child_value6e(6, (short)live1, live);
        set_child_value6e(6, (short)live2, live);
        c_class_1a2c81 *model = find_child(7, (short)model0, false);
        if (model)
            function_2b10a3(appearance, (c_class_2b0b5e *)model, 0);
        model = find_child(7, (short)model1, false);
        if (model)
            function_2b10a3(appearance, (c_class_2b0b5e *)model, 1);
        c_class_1a2c81 *bitmap = find_child(8, (short)active_bitmap, false);
        if (bitmap)
            bitmap->value6e = signed_in;
        bitmap = find_child(8, (short)missing_bitmap, false);
        if (bitmap)
        {
            bitmap->value6e = !signed_in;
            if (!signed_in)
            {
                short frame = function_1249d0((short)controller) > 1 ? (short)(connected != 0) : (short)(connected ? 2 : 0);
                function_2b0a14((s_widget_view_2b0a *)bitmap, frame);
            }
        }
        controller = player_slot_iterate_next(controller);
    } while (controller != NONE);
    function_22f042(items, this, 4);
    c_class_1a2c81::v3();
}

// @retail 0x2524cc
c_4way_signin_screen::c_4way_signin_screen(long a, long b, word user_flags) :
	c_class_1473c9(0x1e, a, b, user_flags),
	mode(1)
{
}

// @retail 0x2523e3
c_4way_signin_screen *signin_screen_new(s_screen_parameters *parameters)
{
	c_4way_signin_screen *screen = new c_4way_signin_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	return screen;
}

// @retail 0x2524a8
c_class_1473c9 *__stdcall function_2524a8(s_screen_parameters *parameters)
{
	c_4way_signin_screen *screen = signin_screen_new(parameters);

	screen->mode = 0;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x25240c
c_class_1473c9 *__stdcall function_25240c(s_screen_parameters *parameters)
{
	c_4way_signin_screen *screen = signin_screen_new(parameters);

	screen->mode = 1;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x252433
c_class_1473c9 *__stdcall function_252433(s_screen_parameters *parameters)
{
	c_4way_signin_screen *screen = signin_screen_new(parameters);

	screen->mode = 2;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x25245a
c_class_1473c9 *__stdcall function_25245a(s_screen_parameters *parameters)
{
	c_4way_signin_screen *screen = signin_screen_new(parameters);

	screen->mode = 3;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x252481
c_class_1473c9 *__stdcall function_252481(s_screen_parameters *parameters)
{
	c_4way_signin_screen *screen = signin_screen_new(parameters);

	screen->mode = 4;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x252b2c
screen_load_proc c_4way_signin_screen::get_load_proc()
{
	switch (mode)
	{
	case 0:
		return function_2524a8;
	case 1:
		return function_25240c;
	case 2:
		return function_252433;
	case 3:
		return function_25245a;
	}
	return function_252481;
}

/* builds the screen; no user is signing in yet */
// @retail 0x2524f8
void c_4way_signin_screen::v18(void *parameters)
{
	volatile long definition_index = function_1480ff(screen_id);
	s_screen_layout layout =
	{
		0,
		1,
		{
			{ 0, 0, 0, 0 }
		}
	};

	build(&layout);
	c_class_1a2c81::v1();
	g_54d598.m1224 = NONE;
	g_54d598.m1220 = false;
}

c_class_1473c9 *__stdcall press_start_screen_open(s_screen_parameters *parameters);
short player_slot_count_active(void);
bool function_6c7e0();
void function_1905bf(long controller, bool flag);

/* back to the press start screen */
// @retail 0x252a32
bool __stdcall function_252a32(long controller)
{
	s_screen_parameters parameters;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, (word)(1 << controller), 5, 4, (long)press_start_screen_open);
	parameters.load(&parameters);
	return true;
}

/* A signs the controller in; B asks whether to go back when no one is
   signed in */
// @retail 0x252a67
bool function_252a67(c_4way_signin_screen *screen, s_widget_event *event)
{
	long param = event->param;

	if (param == 0 || param == 12)
	{
		bool live = function_6c7e0() || screen->mode == 4;

		function_1905bf(event->controller_index, live);
	}
	else if ((param == 1 || param == 13) && player_slot_count_active() == 0)
	{
		dialog_choice_show(3, 0x7e, 4, (1 << event->controller_index), function_252a32, 0, 0);
	}
	return true;
}

bool window_manager_channel_in_use(long channel);
bool function_148044(long channel, long index, long value);
void __stdcall function_1483c3(long reason);
short function_1900ff(long controller);
bool function_199df9(bool offline, bool system_link);
void function_199a57(void);
void function_199a03(long mode);
void function_121100(long *value);
void function_148d42(long value);
bool function_19a76d(short index);
void __stdcall function_238f3f(long controller_index, void *message, unsigned __int64 value);
c_class_1473c9 *__stdcall function_2310b7(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_253185(s_screen_parameters *parameters);
extern bool g_54e7cc;

/* A goes on to what the screen signs the users in for; B leaves, or signs
   the user out */
// @retail 0x25286d
bool function_25286d(c_4way_signin_screen *screen, s_widget_event *event)
{
	bool result = true;
	long param = event->param;

	if (param == 0 || param == 12)
	{
		s_screen_parameters parameters;

		parameters.field_c = 0;
		function_149f49((s_message *)&parameters, 0, 0, (word)(1 << event->controller_index), 5, 4, 0);
		switch (screen->mode)
		{
		case 0:
		{
			bool live = function_6c7e0();

			if (function_199df9(true, live))
			{
				long value;

				function_199a57();
				function_199a03(0);
				function_121100(&value);
				function_148d42(value);
				g_54e7cc = false;
				function_19a76d((short)value);
			}
			break;
		}
		case 1:
			if (function_199df9(true, false))
			{
				function_199a03(2);
			}
			break;
		case 2:
			parameters.load = function_253185;
			break;
		case 3:
			parameters.load = function_2310b7;
			break;
		case 4:
			if (function_6c7e0())
			{
				function_238f3f(event->controller_index, NULL, 0);
				screen->mode = 3;
			}
			else
			{
				dialog_ok_show(3, 0x7c, 4, (word)(1 << event->controller_index), 0, 0);
			}
			break;
		}
		if (parameters.load)
		{
			parameters.load(&parameters);
		}
	}
	else if (param == 1 || param == 13)
	{
		if (player_slot_count_active() == 1)
		{
			if (!function_148044(5, 4, 0xba) && !function_148044(5, 4, 6))
			{
				function_1483c3(1);
			}
		}
		else
		{
			long controller = event->controller_index;

			if (function_1900ff(controller) > 0)
			{
				dialog_ok_show(3, 0x88, 4, (word)(1 << controller), 0, 0);
			}
			else
			{
				s_player_slot_profile *profile = player_slot_profile_get(controller);

				if (screen->mode == 4)
				{
					profile->callback = NULL;
					profile->sign_out();
				}
				else
				{
					profile->show_dialog(NULL, 0x30);
				}
			}
			screen->user_flags |= 1 << event->controller_index;
		}
	}
	else
	{
		result = false;
	}
	return result;
}

// @retail 0x252ad7
bool c_4way_signin_screen::v10(s_widget_event *event)
{
	if (!window_manager_channel_in_use(1))
	{
		bool handled;

		if (TEST_FIELD_BIT(((s_player_slot_sign_in_view *)g_54e8e0)[event->controller_index].signed_in))
		{
			if (event->type != 5)
			{
				return c_class_1473c9::v10(event);
			}
			handled = function_25286d(this, event);
		}
		else
		{
			if (event->type != 5)
			{
				return c_class_1473c9::v10(event);
			}
			handled = function_252a67(this, event);
		}
		if (handled)
		{
			return handled;
		}
		return c_class_1473c9::v10(event);
	}
	return true;
}
