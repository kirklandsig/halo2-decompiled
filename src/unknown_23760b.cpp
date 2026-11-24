// @flags /O1 /arch:SSE /Gr
/* UNKNOWN_23760B.CPP: the virtual keyboard screen (vtable 0x459ba0), which
   edits a string in place: opening it and handing it the string, its keys
   (vtable 0x459b58), moving between them, typing and finishing. What the
   string is (its type) decides the title, the checks the finished string must
   pass and what finishing does with it. */

#include "unknown_11c920.h"
#include <string.h>
#include <ctype.h>
#include <wchar.h>
#include <math.h>
#include "globals.h"
#include "unknown_030290.h"

#pragma intrinsic(memset)
#include <xtl.h>
#include <xonline.h>
#include "screen_widgets.h"
#include "unknown_19b516.h"
#include "unknown_19b510.h"
#include "unknown_19d220.h"
#include "unknown_24b5bc.h"
#include "saved_game_storage.h"

void unicode_string_copy(word *destination, const word *source, long maximum_count);
c_class_1473c9 *__stdcall function_23784f(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_23764f(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_237713(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall xbox_live_message_send_screen_load_type1_to_gamertag(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall xbox_live_message_send_screen_load_type2_to_gamertag(s_screen_parameters *parameters);
long function_1480ff(long screen_id);
s_screen_definition *function_22f871(c_class_1473c9 *screen);
void function_1a0180(long tag_index, long string_handle, word *buffer);
void parse_text(word *string);
bool function_13ee20(word const *string, long font);
bool unicode_character_is_private_use(word c);
void function_236299(long sound);
void __stdcall function_215e60(long index);
bool function_216120(word *string, long type);
void function_148a58();
void function_120df0(long index, wchar_t const *name);
void function_120e20(long *profile_index, long controller_index);
long __stdcall player_profile_make(long controller_index, word *name);
bool player_profile_load(long profile_index, s_player_profile_settings *settings);
void function_19060a(long controller_index, long profile_index);
long online_team_create(long controller_index, XONLINE_TEAM_PROPERTIES const *properties);
void function_1487c3(long controller_index, long task_index, long callback, long value, long context);
class c_online_task_screen;
void __stdcall function_1a2cb7(c_online_task_screen *screen);
struct s_friend_request;
bool friend_request_get(s_friend_request *request);
void function_148ca8(long error, short controller_flags);
bool gamertag_valid(word *string);

/* a key of the virtual keyboard (vtable 0x459b58) */
class c_keyboard_key_widget : public c_class_19b8b1
{
public:
	c_keyboard_key_widget();

	/* a press of A types the key */
	virtual bool v10(s_widget_event *event);
	/* the four keys show their second frame while the keyboard's flags say
	   so */
	virtual long v17();
};

enum
{
	k_keyboard_key_count = 0x2f,

	/* the keys that do something other than type their text */
	_keyboard_key_backspace = 0x24,
	_keyboard_key_space,
	_keyboard_key_left,
	_keyboard_key_right,
	_keyboard_key_done,
	_keyboard_key_shift,
	_keyboard_key_accents,
	_keyboard_key_symbols,
	_keyboard_key_extra
};

/* the keyboard screen */
class c_virtual_keyboard_screen : public c_class_1473c9
{
public:
	c_virtual_keyboard_screen(long a, long b, word user_flags);

	virtual void v3();
	virtual void v4(long screen_rect_address);
	virtual bool v10(s_widget_event *event);
	/* builds the screen around its keys */
	virtual void v18(void *parameters);
	virtual screen_load_proc get_load_proc();

	void update_title();
	char move_focus(long direction);
	long check_string();
	void press_key(short key);
	void finish();
	bool insert(word character);
	void clear_if_selected();
	void backspace();
	void move_cursor(long direction);
	void toggle_shift();
	void toggle_accents();
	void toggle_symbols();
	void toggle_extra();
	void update_keys();
	void update_string();
	void create_team();

	/* what the string is: a gamertag, a variant's name, ... */
	long type;
	long value614;
	/* the string as it was */
	word original[0x100];
	/* the string being edited, the cursor and its maximum length */
	word *string;
	word *end;
	short maximum_length;
	/* the focused key's row and column */
	short row;
	short column;
	/* the four keys of 0x29..0x2c that change the keys' texts */
	short key_flags;
	/* the string is selected: the first key replaces it */
	bool editing;
	c_keyboard_key_widget keys[k_keyboard_key_count];
	/* the team task (a team's name) */
	long team_task;
	/* the team the clan screens create (its name, description, motto and
	   url are typed here) */
	XONLINE_TEAM_PROPERTIES team;
};

/* what each type needs of its string, and a check of its own */
struct s_keyboard_type
{
	dword flags;
	bool (__stdcall *check)(c_virtual_keyboard_screen *keyboard);
};

bool __stdcall function_2380a6(c_virtual_keyboard_screen *keyboard);
bool __stdcall function_2380c0(c_virtual_keyboard_screen *keyboard);
bool __stdcall function_2380d6(c_virtual_keyboard_screen *keyboard);

const s_keyboard_type g_44a9f8[] =
{
	{ 2, function_2380c0 },
	{ 2, function_2380c0 },
	{ 2, function_2380c0 },
	{ 2, function_2380c0 },
	{ 2, function_2380c0 },
	{ 2, function_2380d6 },
	{ 2, function_2380d6 },
	{ 2, function_2380a6 },
	{ 1, 0 },
	{ 1, 0 },
	{ 1, 0 },
	{ 1, 0 },
	{ 8, 0 },
	{ 8, 0 },
	{ 4, 0 },
	{ 4, 0 },
	{ 2, 0 },
	{ 4, 0 },
	{ 4, 0 },
	{ 4, 0 },
	{ 4, 0 }
};

/* the keys' texts: accented letters, symbols, symbols with shift, extra
   symbols, capitals and small letters */
const long g_44a540[0x30] =
{
	0xb000463, 0xb000464, 0xd000465, 0xc000466, 0xc000467, 0xb000468, 0xd000469, 0xd00046a, 0xc00046b, 0xc00046c, 0xb000491, 0xb000492,
	0xb000493, 0xb000494, 0xb000495, 0xb000496, 0xb000497, 0xb000498, 0xb000499, 0xb00049a, 0xb00049b, 0xb00049c, 0xb00049d, 0xb00049e,
	0xb00049f, 0xb0004a0, 0xb0004a1, 0xb0004a2, 0xb0004a3, 0xb0004a4, 0xb0004a5, 0xb0004a6, 0xb0004a7, 0xb0004a8, 0xb0004a9, 0xb0004aa,
	0x900051d, 0x500051c, 0xa00051e, 0xb00051f, 0x4000517, 0x5000518, 0x9000519, 0x700051a, 0x700051b, 0x700051a, 0x700051b, 0x0
};
const long g_44a600[0x30] =
{
	0xa0004ab, 0xa0004ac, 0xc0004ad, 0xb0004ae, 0xb0004af, 0xa0004b0, 0xc0004b1, 0xc0004b2, 0xb0004b3, 0xb0004b4, 0x80004b5, 0x80004b6,
	0x80004b7, 0x80004b8, 0x80004b9, 0x80004ba, 0x80004bb, 0x80004bc, 0x80004bd, 0x80004be, 0x80004bf, 0x80004c0, 0x80004c1, 0x80004c2,
	0x80004c3, 0x80004c4, 0x80004c5, 0x80004c6, 0x80004c7, 0x80004c8, 0x80004c9, 0x80004ca, 0x80004cb, 0x80004cc, 0x80004cd, 0x80004ce,
	0x900051d, 0x500051c, 0xa00051e, 0xb00051f, 0x4000517, 0x5000518, 0x9000519, 0x700051a, 0x700051b, 0x700051a, 0x700051b, 0x0
};
const long g_44a6c0[0x30] =
{
	0x100004cf, 0x100004d0, 0x120004d1, 0x110004d2, 0x110004d3, 0x100004d4, 0x120004d5, 0x120004d6, 0x110004d7, 0x110004d8, 0xe0004d9, 0xe0004da,
	0xe0004db, 0xe0004dc, 0xe0004dd, 0xe0004de, 0xe0004df, 0xe0004e0, 0xe0004e1, 0xe0004e2, 0xe0004e3, 0xe0004e4, 0xe0004e5, 0xe0004e6,
	0xe0004e7, 0xe0004e8, 0xe0004e9, 0xe0004ea, 0xe0004eb, 0xe0004ec, 0xe0004ed, 0xe0004ee, 0xe0004ef, 0xe0004f0, 0xe0004f1, 0xe0004f2,
	0x900051d, 0x500051c, 0xa00051e, 0xb00051f, 0x4000517, 0x5000518, 0x9000519, 0x700051a, 0x700051b, 0x700051a, 0x700051b, 0x0
};
const long g_44a780[0x30] =
{
	0x120004f3, 0x120004f4, 0x140004f5, 0x130004f6, 0x130004f7, 0x120004f8, 0x140004f9, 0x140004fa, 0x130004fb, 0x130004fc, 0x80004fd, 0x80004fe,
	0x80004ff, 0x8000500, 0x8000501, 0x8000502, 0x8000503, 0x8000504, 0x8000505, 0x8000506, 0x8000507, 0x8000508, 0x8000509, 0x800050a,
	0x800050b, 0x800050c, 0x800050d, 0x800050e, 0x800050f, 0x8000510, 0x8000511, 0x8000512, 0x8000513, 0x8000514, 0x8000515, 0x8000516,
	0x900051d, 0x500051c, 0xa00051e, 0xb00051f, 0x4000517, 0x5000518, 0x9000519, 0x700051a, 0x700051b, 0x700051a, 0x700051b, 0x0
};
const long g_44a840[0x30] =
{
	0x9000487, 0x9000488, 0xb000489, 0xa00048a, 0xa00048b, 0x900048c, 0xb00048d, 0xb00048e, 0xa00048f, 0xa000490, 0xb000491, 0xb000492,
	0xb000493, 0xb000494, 0xb000495, 0xb000496, 0xb000497, 0xb000498, 0xb000499, 0xb00049a, 0xb00049b, 0xb00049c, 0xb00049d, 0xb00049e,
	0xb00049f, 0xb0004a0, 0xb0004a1, 0xb0004a2, 0xb0004a3, 0xb0004a4, 0xb0004a5, 0xb0004a6, 0xb0004a7, 0xb0004a8, 0xb0004a9, 0xb0004aa,
	0x900051d, 0x500051c, 0xa00051e, 0xb00051f, 0x4000517, 0x5000518, 0x9000519, 0x700051a, 0x700051b, 0x700051a, 0x700051b, 0x0
};
const long g_44a900[0x2f] =
{
	0xb000463, 0xb000464, 0xd000465, 0xc000466, 0xc000467, 0xb000468, 0xd000469, 0xd00046a, 0xc00046b, 0xc00046c, 0xb00046d, 0xb00046e,
	0xb00046f, 0xb000470, 0xb000471, 0xb000472, 0xb000473, 0xb000474, 0xb000475, 0xb000476, 0xb000477, 0xb000478, 0xb000479, 0xb00047a,
	0xb00047b, 0xb00047c, 0xb00047d, 0xb00047e, 0xb00047f, 0xb000480, 0xb000481, 0xb000482, 0xb000483, 0xb000484, 0xb000485, 0xb000486,
	0x900051d, 0x500051c, 0xa00051e, 0xb00051f, 0x4000517, 0x5000518, 0x9000519, 0x700051a, 0x700051b, 0x700051a, 0x700051b
};

/* the keys by row and column */
const char g_44a9bc[5][11] =
{
	{ 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x2c },
	{ 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10, 0x11, 0x12, 0x13, 0x29 },
	{ 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x2a },
	{ 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x24, 0x24, 0x24, 0x2b },
	{ 0x25, 0x25, 0x25, 0x25, 0x25, 0x25, 0x26, 0x26, 0x27, 0x27, 0x28 }
};

/* the characters a gamertag may hold */
const wchar_t g_459c80[] = L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const wchar_t g_459ad0[] = L"0123456789";

/* hands the keyboard the string to edit */
// @retail 0x2381de
void virtual_keyboard_set_string(c_virtual_keyboard_screen *keyboard, word *string, short maximum_length)
{
	long length;

	if (keyboard->type == 16 && !string)
	{
		string = keyboard->team.wszTeamName;
		maximum_length = 16;
	}
	unicode_string_copy(keyboard->original, string, 0x100);
	keyboard->string = string;
	length = wcslen((wchar_t *)string);
	if (keyboard->type == 7 && length > 2)
	{
		keyboard->string[length - 2] = 0;
	}
	if (keyboard->type >= 0 && (keyboard->type <= 11 || keyboard->type > 14 && keyboard->type <= 16))
	{
		keyboard->maximum_length = maximum_length > 17 ? 17 : maximum_length;
		if (maximum_length > 17)
		{
			keyboard->string[keyboard->maximum_length - 1] = 0;
		}
	}
	else
	{
		keyboard->maximum_length = maximum_length;
	}
	keyboard->maximum_length = keyboard->maximum_length > 150 ? 150 : keyboard->maximum_length;
	keyboard->end = keyboard->string + wcslen((wchar_t *)keyboard->string);
	keyboard->editing = true;
}

/* opens the keyboard on a string */
// @retail 0x238c21
void function_238c21(long controller, long type, word *name, long maximum_count)
{
	s_screen_parameters parameters;
	c_virtual_keyboard_screen *keyboard;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, 1 << controller, 2, 4, (long)function_23784f);
	keyboard = (c_virtual_keyboard_screen *)parameters.load(&parameters);
	keyboard->type = type;
	virtual_keyboard_set_string(keyboard, name, (short)maximum_count);
}

/* the same, with a second value */
// @retail 0x238c69
void function_238c69(long mode, long type, word *name, long maximum_count, long controller)
{
	s_screen_parameters parameters;
	c_virtual_keyboard_screen *keyboard;

	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, 1 << controller, 2, 4, (long)function_23784f);
	keyboard = (c_virtual_keyboard_screen *)parameters.load(&parameters);
	keyboard->type = mode;
	virtual_keyboard_set_string(keyboard, name, (short)maximum_count);
	keyboard->value614 = type;
}

bool function_199bef(const word *machine_name, const word *session_name);

/* the keyboard's string names a session to join */
// @retail 0x238922
bool function_238922(c_virtual_keyboard_screen *keyboard)
{
	function_199bef(0, keyboard->string);
	return true;
}

// @retail 0x23760b
c_keyboard_key_widget::c_keyboard_key_widget() :
	c_class_19b8b1(NONE, 0)
{
}

// @retail 0x23763f destructor c_keyboard_key_widget

// @retail 0x237797
bool c_keyboard_key_widget::v10(s_widget_event *event)
{
	if (event->type == 5)
	{
		c_virtual_keyboard_screen *keyboard = (c_virtual_keyboard_screen *)get_screen();

		if (event->param == 0)
		{
			keyboard->press_key(valuef8);
			return true;
		}
		if (event->param == 1 && (keyboard->type == 7 || keyboard->type == 5) && g_54e49c != NONE)
		{
			function_215e60(g_54e49c);
		}
	}
	return c_class_1a2c81::v10(event);
}

// @retail 0x2377f6
long c_keyboard_key_widget::v17()
{
	c_virtual_keyboard_screen *keyboard = (c_virtual_keyboard_screen *)get_screen();

	switch (valuef8)
	{
	case 0x29:
		if ((bool)(keyboard->key_flags & 1))
		{
			goto active;
		}
		return c_class_19b8b1::v17();
	case 0x2a:
		if ((bool)(((dword)(char)keyboard->key_flags >> 1) & 1))
		{
			goto active;
		}
		return c_class_19b8b1::v17();
	case 0x2b:
		if ((bool)(((dword)(char)keyboard->key_flags >> 2) & 1))
		{
			goto active;
		}
		return c_class_19b8b1::v17();
	case 0x2c:
		if ((bool)(((dword)(char)keyboard->key_flags >> 3) & 1))
		{
			goto active;
		}
		return c_class_19b8b1::v17();
	}
	return c_class_19b8b1::v17();
active:
	return 1;
}

// @retail 0x23784f
c_class_1473c9 *__stdcall function_23784f(s_screen_parameters *parameters)
{
	c_virtual_keyboard_screen *screen = new c_virtual_keyboard_screen(parameters->a, parameters->b, parameters->user_flags);

	screen->m6c = true;
	screen->function_147f6d(parameters);
	return screen;
}

// @retail 0x23788f
c_virtual_keyboard_screen::c_virtual_keyboard_screen(long a, long b, word user_flags) :
	c_class_1473c9(0xa7, a, b, user_flags),
	type(NONE),
	value614(NONE),
	string(0),
	end(0),
	maximum_length(0),
	row(4),
	column(10),
	key_flags(0),
	editing(false),
	team_task(NONE)
{
	memset(&team, 0, sizeof(team));
	for (word key = 0; key < k_keyboard_key_count; key++)
	{
		keys[key].valuef8 = key;
		keys[key].user_flags = user_flags;
	}
	memset(&team, 0, sizeof(team));
}

// @retail 0x237958 deleting c_virtual_keyboard_screen
// @retail 0x237976 destructor c_virtual_keyboard_screen

// @retail 0x237649
screen_load_proc c_virtual_keyboard_screen::get_load_proc()
{
	return function_23784f;
}

void function_238de7(c_virtual_keyboard_screen *keyboard, long const *string_ids, long count);

// @retail 0x237998
void c_virtual_keyboard_screen::v18(void *parameters)
{
	if (function_1480ff(screen_id) != NONE)
	{
		c_class_1a2c81 *buttons[k_keyboard_key_count];
		s_screen_layout layout =
		{
			0,
			1,
			{
				{ k_keyboard_key_count, buttons, 0, 0 }
			}
		};

		for (dword key = 0; key < k_keyboard_key_count; key++)
		{
			buttons[key] = &keys[key];
		}
		build(&layout);
	}
	c_class_1a2c81::v1();
	function_238de7(this, g_44a900, k_keyboard_key_count);
	function_238de7(this, g_44a540, k_keyboard_key_count);
	function_238de7(this, g_44a780, k_keyboard_key_count);
	function_238de7(this, g_44a600, k_keyboard_key_count);
	update_title();
	if (!(g_44a9f8[type].flags & 0xa))
	{
		keys[0x2e].value6e = false;
		keys[0x2d].value6e = false;
	}
	else
	{
		keys[_keyboard_key_extra].value6e = false;
		keys[_keyboard_key_symbols].value6e = false;
	}
}

/* the title and the description say what the string is */
// @retail 0x237a74
void c_virtual_keyboard_screen::update_title()
{
	c_text_widget_45a5e0 *description = (c_text_widget_45a5e0 *)find_child(6, 3, false);
	long title_id = 0;
	long description_id = 0;

	switch (type)
	{
	case 0:
	case 1:
	case 2:
	case 4:
		title_id = 0x20000449;
		description_id = 0x1d00044a;
		break;
	case 3:
		title_id = 0x20000449;
		description_id = 0x2500044b;
		break;
	case 5:
	case 6:
		title_id = 0x1200044c;
		description_id = 0x1600044d;
		break;
	case 7:
		title_id = 0x1200044c;
		description_id = 0x2500044e;
		break;
	case 8:
	case 9:
		title_id = 0xd00044f;
		description_id = 0x12000450;
		break;
	case 10:
	case 11:
		title_id = 0xf000451;
		description_id = 0x14000452;
		break;
	case 12:
		title_id = 0xe000453;
		description_id = 0x12000454;
		break;
	case 13:
		title_id = 0x17000455;
		description_id = 0x1b000456;
		break;
	case 14:
		title_id = 0xd000457;
		description_id = 0x11000458;
		break;
	case 15:
		title_id = 0x10000459;
		description_id = 0x1400045a;
		break;
	case 16:
		title_id = 0x1100045b;
		description_id = 0x1500045c;
		virtual_keyboard_set_string(this, team.wszTeamName, 0x10);
		break;
	case 17:
		title_id = 0x1600045d;
		description_id = 0x1a00045e;
		virtual_keyboard_set_string(this, team.wszDescription, 0x100);
		break;
	case 18:
		title_id = 0x1000045f;
		description_id = 0x14000460;
		virtual_keyboard_set_string(this, team.wszMotto, 0x100);
		break;
	case 19:
		title_id = 0xe000461;
		description_id = 0x12000462;
		virtual_keyboard_set_string(this, team.wszURL, 0x100);
		break;
	}
	title.function_253b1a(title_id);
	if (description)
	{
		description->function_253b1a(description_id);
	}
	v7(&keys[_keyboard_key_done]);
}

// @retail 0x237bff
void c_virtual_keyboard_screen::v3()
{
	c_class_1473c9::v3();
}

/* moves the focus a step (1 up, 2 left, 3 down, 4 right) to the next
   different key and returns it */
// @retail 0x237e06
char c_virtual_keyboard_screen::move_focus(long direction)
{
	char key = g_44a9bc[row][column];

	switch (direction)
	{
	case 1:
	{
		short *value = &row;

		do
		{
			if (--*value < 0)
			{
				*value = 4;
			}
		}
		while (g_44a9bc[row][column] == key);
		break;
	}
	case 2:
	{
		short *value = &column;

		do
		{
			if (--*value < 0)
			{
				*value = 10;
			}
		}
		while (g_44a9bc[row][column] == key);
		break;
	}
	case 3:
	{
		short *value = &row;

		do
		{
			if (++*value == 5)
			{
				*value = 0;
			}
		}
		while (g_44a9bc[row][column] == key);
		break;
	}
	case 4:
	{
		short *value = &column;

		do
		{
			if (++*value == 11)
			{
				*value = 0;
			}
		}
		while (g_44a9bc[row][column] == key);
		break;
	}
	}
	return g_44a9bc[row][column];
}

// @retail 0x237f2a
bool c_virtual_keyboard_screen::v10(s_widget_event *event)
{
	bool result = false;
	if (event->type == 1 || event->type == 2 || event->type == 3 || event->type == 4)
	{
		char key = move_focus(event->type);
		long const *string_ids;

		switch (key)
		{
		case 0x29:
			string_ids = g_44a840;
			if (key_flags & 1)
				string_ids = g_44a900;
			break;
		case 0x2a:
			if (key_flags & 2)
				goto shifted_labels;
			string_ids = g_44a540;
			break;
		case 0x2b:
			if (key_flags & 4)
				goto shifted_labels;
			string_ids = g_44a600;
			break;
		case 0x2c:
			if (key_flags & 8)
				goto shifted_labels;
			string_ids = g_44a780;
			break;
		default:
			goto labels_done;
		}
		goto replace_labels;
shifted_labels:
		string_ids = g_44a900;
replace_labels:
		function_238de7(this, string_ids, k_keyboard_key_count);
labels_done:
		if (g_44a9f8[type].flags & 0xa && (key == 0x2b || key == 0x2c))
		{
			key = move_focus(event->type);
		}
		v7(&keys[key]);
		result = true;
	}
	else if (event->type == 5)
	{
		if (event->param == 1 || event->param == 13)
		{
			if (string)
			{
				unicode_string_copy(string, original, maximum_length);
			}
		}
		else if (event->param == 6)
		{
			move_cursor(-1);
			result = true;
		}
		else if (event->param == 7)
		{
			move_cursor(1);
			result = true;
		}
		else if (event->param == 2)
		{
			backspace();
			result = true;
		}
		else if (event->param == 14 || event->param == 15)
		{
			toggle_shift();
			result = true;
		}
		else if (event->param == 5)
		{
			insert(' ');
			result = true;
		}
		else if (event->param == 12)
		{
			press_key(_keyboard_key_done);
			result = true;
		}
	}
	if (!result)
		result = c_class_1473c9::v10(event);
	return result;
}

/* the checks of the type's string: 0 when it passes, 1 when it fails the
   type's flags, 2 when it fails the type's own check */
// @retail 0x238114
long c_virtual_keyboard_screen::check_string()
{
	bool empty = string[0] == 0;
	s_keyboard_type const *keyboard_type = &g_44a9f8[type];
	bool has_alphanumeric = false;
	bool characters_valid = true;
	long result = 0;

	for (long i = 0; string[i]; i++)
	{
		if (iswalnum(string[i]))
		{
			has_alphanumeric = true;
		}
		else if (!iswspace(string[i]))
		{
			characters_valid = false;
		}
	}
	bool valid = (!(keyboard_type->flags & 1) || has_alphanumeric) &&
		(!(keyboard_type->flags & 2) || characters_valid) &&
		((keyboard_type->flags & 4) || !empty) &&
		(!(keyboard_type->flags & 8) || gamertag_valid(string));
	if (valid)
	{
		if (keyboard_type->check && !keyboard_type->check(this))
			result = 2;
	}
	else
		result = 1;
	return result;
}

/* a key is pressed: types its text or does what it does */
// @retail 0x2382d6
void c_virtual_keyboard_screen::press_key(short key)
{
	if (type == 14 && team_task != NONE)
	{
		return;
	}
	switch (key)
	{
	case _keyboard_key_backspace:
		backspace();
		break;
	case _keyboard_key_space:
		insert(' ');
		break;
	case _keyboard_key_left:
		move_cursor(-1);
		break;
	case _keyboard_key_right:
		move_cursor(1);
		break;
	case _keyboard_key_done:
		if (type != NONE)
		{
			long dialog_id = 0;
			long error = check_string();

			if (error)
			{
				if (type > 4)
				{
					if (type > 7)
					{
						if (type > 9)
						{
							if (type > 11)
							{
								dialog_id = type > 13 ? 0x8e : 0xa7;
							}
							else if (error == 1)
								dialog_id = 0x5b;
							else if (error == 2)
								dialog_id = 0x49;
						}
						else if (error == 1)
							dialog_id = 0x5a;
						else if (error == 2)
							dialog_id = 0x48;
					}
					else if (error == 1)
						dialog_id = 0x59;
					else if (error == 2)
						dialog_id = 0x47;
				}
				else if (error == 1)
					dialog_id = 0x58;
				else if (error == 2)
					dialog_id = 0x46;
				dialog_ok_show(1, dialog_id, 4, user_flags, 0, 0);
				break;
			}
		}
		finish();
		break;
	case _keyboard_key_shift:
		toggle_shift();
		break;
	case _keyboard_key_accents:
		toggle_accents();
		break;
	case _keyboard_key_symbols:
		toggle_symbols();
		break;
	case _keyboard_key_extra:
		toggle_extra();
		break;
	case 0x0:
	case 0x1:
	case 0x2:
	case 0x3:
	case 0x4:
	case 0x5:
	case 0x6:
	case 0x7:
	case 0x8:
	case 0x9:
	case 0xa:
	case 0xb:
	case 0xc:
	case 0xd:
	case 0xe:
	case 0xf:
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
	case 0x14:
	case 0x15:
	case 0x16:
	case 0x17:
	case 0x18:
	case 0x19:
	case 0x1a:
	case 0x1b:
	case 0x1c:
	case 0x1d:
	case 0x1e:
	case 0x1f:
	case 0x20:
	case 0x21:
	case 0x22:
	case 0x23:
		{
			word const *text = keys[key].function_22f52e()->function_22f52e();

			for (long index = 0; text[index]; index++)
			{
				word character = text[index];

				if (unicode_character_is_private_use(character))
				{
					character = 0x25a1;
				}
				if (!insert(character))
				{
					break;
				}
			}
		}
		break;
	default:
		__assume(0);
	}
}

bool finish_profile_create(c_virtual_keyboard_screen *keyboard);
bool finish_profile_create_1(c_virtual_keyboard_screen *keyboard);
bool finish_profile_create_2(c_virtual_keyboard_screen *keyboard);
bool finish_profile_create_3(c_virtual_keyboard_screen *keyboard);
bool finish_profile_rename(c_virtual_keyboard_screen *keyboard);
bool finish_variant_name(c_virtual_keyboard_screen *keyboard);
bool finish_variant_save(c_virtual_keyboard_screen *keyboard);
bool finish_message_gamertag(c_virtual_keyboard_screen *keyboard);
bool finish_friend_gamertag(c_virtual_keyboard_screen *keyboard);

/* the keyboard types with no finish step of their own; retail's jump table
   still lists them, so they keep their own (empty) case */
PRIVATE inline void finish_nothing(c_virtual_keyboard_screen *keyboard)
{
}

/* the string is done: the type's finish, then the screen closes */
// @retail 0x238459
void c_virtual_keyboard_screen::finish()
{
	bool done = true;

	switch (type)
	{
	case 0:
		done = finish_profile_create(this);
		break;
	case 1:
		done = finish_profile_create_1(this);
		break;
	case 2:
		done = finish_profile_create_2(this);
		break;
	case 3:
		done = finish_profile_create_3(this);
		break;
	case 4:
		done = finish_profile_rename(this);
		break;
	case 5:
	case 7:
		done = finish_variant_name(this);
		break;
	case 6:
		done = finish_variant_save(this);
		break;
	case 12:
		done = finish_message_gamertag(this);
		break;
	case 13:
		done = finish_friend_gamertag(this);
		break;
	case 15:
		done = function_238922(this);
		break;
	case 16:
		create_team();
		break;
	case 14:
		done = team_task == NONE;
		break;
	case 8:
	case 9:
	case 10:
	case 11:
	case 17:
	case 18:
	case 19:
	case 20:
		finish_nothing(this);
		break;
	}
	if (done)
	{
		start_animation(3);
	}
}

/* types a character at the cursor */
// @retail 0x238932
bool c_virtual_keyboard_screen::insert(word character)
{
	bool result = false;

	clear_if_selected();
	if (string)
	{
		long length = wcslen((wchar_t *)string) + 1;

		if (length < maximum_length)
		{
			long size = (length - (end - string)) * sizeof(word);

			if (size > 0)
			{
				memcpy(end + 1, end, size);
			}
			*end = character;
			end++;
			if (key_flags & 1)
			{
				toggle_shift();
			}
			function_236299(7);
			result = true;
		}
		else
		{
			function_236299(2);
		}
	}
	return result;
}

/* the first key replaces the selected string */
// @retail 0x2389c7
void c_virtual_keyboard_screen::clear_if_selected()
{
	if (editing)
	{
		if (string)
		{
			unicode_string_copy(string, (word *)L"", maximum_length);
		}
		end = string;
		editing = false;
	}
}

/* deletes the character before the cursor */
// @retail 0x238a05
void c_virtual_keyboard_screen::backspace()
{
	clear_if_selected();
	if (end > string)
	{
		long size = (maximum_length - (end - string)) * sizeof(word);

		if (size > 0)
		{
			memcpy(end - 1, end, size);
		}
		string[maximum_length - 1] = 0;
		end--;
	}
	function_236299(6);
}

/* moves the cursor a character left or right */
// @retail 0x238a63
void c_virtual_keyboard_screen::move_cursor(long direction)
{
	editing = false;
	if (direction > 0)
	{
		if (*end)
		{
			end++;
		}
	}
	else if (direction < 0)
	{
		if (end > string)
		{
			end--;
		}
	}
	function_236299(6);
}

// @retail 0x238aa6
void c_virtual_keyboard_screen::toggle_shift()
{
	if (key_flags & 1)
		key_flags &= ~1;
	else
		key_flags |= 1;
	key_flags &= ~(2 | 8);
	update_keys();
}

// @retail 0x238acf
void c_virtual_keyboard_screen::toggle_accents()
{
	if (key_flags & 2)
		key_flags &= ~2;
	else
		key_flags |= 2;
	key_flags &= ~(1 | 4 | 8);
	update_keys();
}

// @retail 0x238af8
void c_virtual_keyboard_screen::toggle_symbols()
{
	if (key_flags & 4)
		key_flags &= ~4;
	else
		key_flags |= 4;
	key_flags &= ~(2 | 8);
	update_keys();
}

// @retail 0x238b21
void c_virtual_keyboard_screen::toggle_extra()
{
	if (key_flags & 8)
		key_flags &= ~8;
	else
		key_flags |= 8;
	key_flags &= ~(1 | 2 | 4);
	update_keys();
}

/* gives the keys the texts of the keyboard's flags */
// @retail 0x238b4a
void c_virtual_keyboard_screen::update_keys()
{
	bool accents = (bool)(((dword)key_flags >> 1) & 1);
	bool symbols = (bool)(((dword)key_flags >> 2) & 1);
	bool extra = (bool)(((dword)key_flags >> 3) & 1);
	bool shift = (bool)(key_flags & 1);
	c_keyboard_key_widget *key = keys;

	for (long i = 0; i < k_keyboard_key_count; i++, key++)
	{
		long string_handle;

		if (accents)
			string_handle = g_44a540[i];
		else if (symbols)
			string_handle = shift ? g_44a6c0[i] : g_44a600[i];
		else if (extra)
			string_handle = g_44a780[i];
		else if (shift)
			string_handle = g_44a840[i];
		else
			string_handle = g_44a900[i];
		key->function_253b1a(string_handle);
	}
}

/* creates the team */
// @retail 0x238bf1
void c_virtual_keyboard_screen::create_team()
{
	long controller_index = get_controller_index();
	long task_index = online_team_create(controller_index, &team);

	if (task_index != NONE)
	{
		function_1487c3(controller_index, task_index, (long)function_1a2cb7, 0, 0);
	}
}

/* shows the string in the screen's text */
// @retail 0x238cba
void c_virtual_keyboard_screen::update_string()
{
	c_class_1a2c81 *text = find_child(6, 2, false);

	if (text)
	{
		word *s = string;

		if (s)
		{
			text->function_22f52e()->set_text(s);
		}
	}
}

/* a gamertag starts with a letter, holds letters, digits and single spaces
   and doesn't end with a space */
// @retail 0x238ce9
bool gamertag_valid(word *string)
{
	bool result = true;
	long i = 0;

	if (!string[0] || string[0] == ' ' || !wcschr(g_459c80, string[0]))
	{
		result = false;
	}
	else
	{
		for (; (short)i < 0x10; i++)
		{
			word *character = &string[(short)i];

			if (!*character)
			{
				break;
			}
			if (!result)
			{
				goto done;
			}
			if (*character != ' ' && !wcschr(g_459c80, *character) && !wcschr(g_459ad0, *character))
			{
				result = false;
			}
			else
			{
				if (iswspace(*character))
				{
					if ((short)(i + 1) >= 0x10)
						goto next_character;
					if (iswspace(string[(short)(i + 1)]))
					{
						result = false;
						goto next_character;
					}
				}
				short next = i + 1;

				if (next < 0x10)
				{
					word current = *character;
					if (iswspace(current) && !string[next])
						result = false;
				}
			}
next_character:
			;
		}
		if (result)
		{
			result = (short)i < 0x10;
		}
	}
done:
	return result;
}

/* loads the keys' strings (precaching their characters) */
// @retail 0x238de7
void function_238de7(c_virtual_keyboard_screen *keyboard, long const *string_ids, long count)
{
	word buffer[0x100];

	buffer[0] = 0;
	for (long i = 0; i < count; i++)
	{
		function_1a0180(function_22f871(keyboard)->string_list_index, string_ids[i], buffer);
		parse_text(buffer);
		function_13ee20(buffer, 1);
	}
}

/* ---- the checks ---- */

// @retail 0x2380a6
bool __stdcall function_2380a6(c_virtual_keyboard_screen *keyboard)
{
	return function_216120(keyboard->string, keyboard->value614);
}

// @retail 0x2380c0
bool __stdcall function_2380c0(c_virtual_keyboard_screen *keyboard)
{
	return function_216120(keyboard->string, 0);
}

// @retail 0x2380d6
bool __stdcall function_2380d6(c_virtual_keyboard_screen *keyboard)
{
	long result;
	wchar_t *value = (wchar_t *)keyboard->string;
	wchar_t **value_reference = &value;

	if (wcscmp(*value_reference, (wchar_t *)keyboard->original) == 0 || function_216120(keyboard->string, keyboard->value614))
	{
		result = true;
	}
	else
	{
		result = false;
	}
	return *(bool *)&result;
}

/* ---- finishing ---- */

/* a name request (0x78 bytes) */
struct s_keyboard_name_request
{
	long type;
	char name[0x10];
	byte unknown14[0x78 - 0x14];
};

// @retail 0x238537
bool finish_profile_create(c_virtual_keyboard_screen *keyboard)
{
	s_screen_parameters parameters;
	s_player_profile_settings settings;
	long profile_index;

	parameters.field_c = 0;
	profile_index = player_profile_make(keyboard->get_controller_index(), keyboard->string);
	if (profile_index != NONE)
	{
		player_profile_load(profile_index, &settings);
		profile_edit_begin(keyboard->get_controller_index(), &settings, profile_index);
		player_slot_profile_get(keyboard->get_controller_index())->set_profile(profile_index);
		function_149f49((s_message *)&parameters, 0, 0, keyboard->user_flags, 5, 4, (long)function_237713);
		parameters.load(&parameters);
	}
	else
	{
		function_148ca8(g_55c154, keyboard->user_flags);
	}
	return true;
}

// @retail 0x2385d6
bool finish_profile_create_1(c_virtual_keyboard_screen *keyboard)
{
	s_player_profile_settings settings;
	long profile_index = player_profile_make(keyboard->get_controller_index(), keyboard->string);

	if (profile_index != NONE)
	{
		player_profile_load(profile_index, &settings);
		profile_edit_begin(keyboard->get_controller_index(), &settings, profile_index);
		function_19060a(keyboard->get_controller_index(), profile_index);
	}
	else
	{
		function_148ca8(g_55c154, keyboard->user_flags);
	}
	return true;
}

// @retail 0x238642
bool finish_profile_create_2(c_virtual_keyboard_screen *keyboard)
{
	s_player_profile_settings settings;
	long profile_index = player_profile_make(keyboard->get_controller_index(), keyboard->string);

	if (profile_index != NONE)
	{
		s_screen_parameters parameters;

		player_profile_load(profile_index, &settings);
		profile_edit_begin(keyboard->get_controller_index(), &settings, profile_index);
		parameters.field_c = 0;
		function_149f49((s_message *)&parameters, 0, 0, keyboard->user_flags, 5, 4, (long)function_237713);
		parameters.load(&parameters);
	}
	else
	{
		function_148ca8(g_55c154, keyboard->user_flags);
	}
	return true;
}

// @retail 0x2386c5
bool finish_profile_create_3(c_virtual_keyboard_screen *keyboard)
{
	s_player_profile_settings settings;
	long profile_index = player_profile_make(keyboard->get_controller_index(), keyboard->string);

	if (profile_index != NONE)
	{
		player_profile_load(profile_index, &settings);
		profile_edit_begin(keyboard->get_controller_index(), &settings, profile_index);
		player_slot_profile_get(keyboard->get_controller_index())->set_profile(profile_index);
		profile_edit_end();
	}
	else
	{
		function_148ca8(g_55c154, keyboard->user_flags);
	}
	return true;
}

// @retail 0x238742
bool finish_profile_rename(c_virtual_keyboard_screen *keyboard)
{
	long profile_index = g_54e5d0.profile_index;
	long current;
	volatile long saved_profile_index = profile_index;
	long controller_index = keyboard->get_controller_index();

	if (!(bool)(((dword)profile_index >> 21) & 1))
	{
		unicode_string_copy(g_54e5d0.settings.name, keyboard->string, 0x20);
	}
	if (controller_index != NONE)
	{

		function_120e20(&current, controller_index);
		long saved_value = saved_profile_index;
		if (current == saved_value)
		{
			function_120df0(controller_index, (wchar_t const *)g_54e5d0.settings.name);
		}
	}
	profile_edit_end();
	return true;
}

// @retail 0x2387a5
bool finish_variant_name(c_virtual_keyboard_screen *keyboard)
{
	s_screen_parameters parameters;

	parameters.field_c = 0;
	if (!(bool)(((dword)g_54e49c >> 21) & 1))
	{
		unicode_string_copy((word *)g_54e4a0.name, keyboard->string, 0x20);
	}
	function_149f49((s_message *)&parameters, 0, 0, keyboard->user_flags, 5, 4, (long)function_23764f);
	if (parameters.load)
	{
		parameters.load(&parameters);
	}
	return true;
}

// @retail 0x2387fe
bool finish_variant_save(c_virtual_keyboard_screen *keyboard)
{
	if (!(bool)(((dword)g_54e49c >> 21) & 1))
	{
		unicode_string_copy((word *)g_54e4a0.name, keyboard->string, 0x20);
		function_148a58();
	}
	return true;
}

// @retail 0x23882f
bool finish_message_gamertag(c_virtual_keyboard_screen *keyboard)
{
	s_keyboard_name_request request;
	char name[0x10];
	s_screen_parameters parameters;

	unicode_string_to_ascii(keyboard->string, name, 0x10);
	request.type = 3;
	strncpy(request.name, name, 0x10);
	request.name[0xf] = 0;
	function_148893((s_name_request *)&request, 1);
	parameters.field_c = 0;
	function_149f49((s_message *)&parameters, 0, 0, keyboard->user_flags, 3, 4, (long)xbox_live_message_send_screen_load_type1_to_gamertag);
	parameters.load(&parameters);
	return true;
}

// @retail 0x2388a1
bool finish_friend_gamertag(c_virtual_keyboard_screen *keyboard)
{
	byte friend_request[0x6a4];

	if (friend_request_get((s_friend_request *)friend_request))
	{
		s_keyboard_name_request request;
		char name[0x10];
		s_screen_parameters parameters;

		unicode_string_to_ascii(keyboard->string, name, 0x10);
		request.type = 3;
		strncpy(request.name, name, 0x10);
		request.name[0xf] = 0;
		function_148893((s_name_request *)&request, 1);
		parameters.field_c = 0;
		function_149f49((s_message *)&parameters, 0, 0, keyboard->user_flags, 3, 4, (long)xbox_live_message_send_screen_load_type2_to_gamertag);
		parameters.load(&parameters);
	}
	return true;
}

struct s_type_7ba8e9;
s_type_7ba8e9 *function_236235(short frame_index, short sequence_index);
extern dword g_54d5b8;

/* Draws the text selection and the keyboard cursor. */
// @retail 0x237c04
void c_virtual_keyboard_screen::v4(long screen_rect_address)
{
	short_rectangle2d const *screen = (short_rectangle2d const *)screen_rect_address;
	c_text_widget_45a5e0 *text = (c_text_widget_45a5e0 *)find_child(6, 2, false);
	update_string();
	if (text && string && (dword)text->animation.end_time <= g_54d5b8 && !ANIMATION_FLAG(text->animation, 1))
	{
		if (editing && string[0])
		{
			byte *settings = *(long *)((byte *)g_4e034c + 0x108) ? *(byte **)((byte *)g_4e034c + 0x10c) : 0;
			long tag_index = *(long *)(settings + 0x64);
			s_sprite_element *sprite = *(s_sprite_element **)(g_4e3b44[tag_index & 0xffff].bytes + 0x48);
			sprite = (s_sprite_element *)((byte *)sprite + 0x74);
			short_rectangle2d selected;
			text->function_253ab4(screen, &selected);
			s_float_rect from, to;
			from.y0 = 0.0f;
			from.x0 = 0.0f;
			from.x1 = sprite->width;
			from.y1 = sprite->height;
			to.x0 = selected.left;
			to.x1 = selected.right;
			to.y0 = selected.top;
			to.y1 = selected.bottom;
			function_235e5e(sprite, &from, &to, 0xffffffff, 1, 0);
		}
		s_sprite_element *cursor = (s_sprite_element *)function_236235(0, 0);
		if (cursor)
		{
			// Reading the timer through a const pointer here keeps the comparison above in retail's operand order.
			real fade = (real)((cos(*(dword const *)&g_54d5b8 * 0.003f) + 1.0f) * 0.5f);
			fade *= animation.scale;
			fade *= 255.0f;
			long alpha;
			__asm
			{
				fld fade
				fistp alpha
			}
			s_float_rect from, to;
			from.y0 = 0.0f;
			from.x0 = 0.0f;
			from.x1 = cursor->width;
			from.y1 = cursor->height;
			short_rectangle2d position;
			text->function_253a73(screen, (end - string), &position);
			to.x0 = position.left;
			to.x1 = position.right;
			to.y0 = position.top;
			to.y1 = position.bottom;
			function_235e5e(cursor, &from, &to, ((dword)alpha << 24) | 0xffffff, 0, 0);
		}
	}
}
