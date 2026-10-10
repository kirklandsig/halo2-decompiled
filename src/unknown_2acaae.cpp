// @flags /O1 /Oi /arch:SSE /Gr
/* A progress screen driven by a polling callback (vtable 0x45ac80). */

#include "unknown_11c920.h"
#include "screen_widgets.h"
#include "unknown_19b516.h"

typedef long (__stdcall *progress_screen_poll)(void *context, long *description, real *fraction, long *error);
typedef void (__stdcall *progress_screen_cleanup)(void *context);

class c_progress_screen : public c_class_1473c9
{
public:
	c_progress_screen(long a, long b, word user_flags);
	virtual ~c_progress_screen();
	virtual void v3();
	virtual bool v10(s_widget_event *event);
	virtual void v18(void *parameters);
	virtual screen_load_proc get_load_proc();

	void configure(long field_210, progress_screen_poll poll, progress_screen_cleanup cleanup, void *context);

	void *context;
	real fraction;
	bool finished;
	progress_screen_poll poll;
	progress_screen_cleanup cleanup;
	long field_210;
	long description;
};

c_class_1473c9 *__stdcall progress_screen_load(s_screen_parameters *parameters);
long function_1480ff(long screen_id);

// @retail 0x2acaae
screen_load_proc c_progress_screen::get_load_proc()
{
	return progress_screen_load;
}

class c_campaign_options_list;
typedef bool (__stdcall *campaign_progress_poll)(c_campaign_options_list *list, long unused, real *fraction, long *error);

// @retail 0x2acab4
void __stdcall function_2acab4(long a, long user_flags, long string_handle, campaign_progress_poll progress, long b, c_campaign_options_list *list)
{
	s_screen_parameters parameters;
	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, NULL, (word)user_flags, 1, a, (long)progress_screen_load);
	c_progress_screen *screen = (c_progress_screen *)parameters.load(&parameters);
	screen->configure(string_handle, (progress_screen_poll)progress, (progress_screen_cleanup)b, list);
}

// @retail 0x2acaf3
c_progress_screen::c_progress_screen(long a, long b, word user_flags) :
	c_class_1473c9(0xee, a, b, user_flags),
	fraction(-1.0f),
	finished(false),
	poll(NULL),
	cleanup(NULL),
	field_210(0),
	description(0)
{
}

// @retail 0x2acb48 deleting c_progress_screen

// @retail 0x2acb64
c_class_1473c9 *__stdcall progress_screen_load(s_screen_parameters *parameters)
{
	c_progress_screen *screen = new c_progress_screen(parameters->a, parameters->b, parameters->user_flags);
	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x2acba0
void c_progress_screen::configure(long field_210, progress_screen_poll poll, progress_screen_cleanup cleanup, void *context)
{
	this->field_210 = field_210;
	this->poll = poll;
	this->cleanup = cleanup;
	this->context = context;
}

inline bool progress_fraction_visible(real const *fraction)
{
	if (*fraction >= 0.0f)
		return true;
	return false;
}

// @retail 0x2acbc7
void c_progress_screen::v3()
{
	c_class_1a2c81::v3();
	if (!finished && poll)
	{
		long error;
		if (poll(context, &description, &fraction, &error) == 1)
		{
			if (cleanup)
				cleanup(context);
			if (!ANIMATION_FLAG(animation, 1))
				start_animation(3);
			finished = true;
		}
	}
	c_class_2b01eb *bitmap = (c_class_2b01eb *)find_child(8, 0, false);
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)find_child(6, 2, false);
	if (text)
		text->function_253b1a(description);
	if (bitmap)
	{
		real *fraction_reference = &fraction;
		bitmap->value6e = progress_fraction_visible(fraction_reference);
		bitmap->value84 = *fraction_reference;
	}
}

// @retail 0x2acc8c
bool c_progress_screen::v10(s_widget_event *event)
{
	bool handled_event_flag = false;
	if (event->type == 5)
		handled_event_flag = true;
	return handled_event_flag;
}

// @retail 0x2acc9c
c_progress_screen::~c_progress_screen()
{
	if (!finished && cleanup)
		cleanup(context);
}

/* Shared screen-layout setup; identical overrides fold together. */
void c_progress_screen::v18(void *parameters)
{
	volatile long definition_index = function_1480ff(screen_id);
	s_screen_layout layout = { 0, 1, { { 0, 0, 0, 0 } } };
	build(&layout);
	c_class_1a2c81::v1();
}
