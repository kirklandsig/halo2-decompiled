/* UNKNOWN_234C64.H: the window manager's screen channels (the vtables at
   0x45997c, 0x4599a8, 0x4599dc, 0x459a08 and 0x459a34). The window manager
   at 0x54d598 (constructed by 0x147645) holds them: see unknown_147f6d.cpp. */

#ifndef UNKNOWN_234C64_H
#define UNKNOWN_234C64_H

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "screen_widgets.h"
#include "unknown_19b516.h"
#include "online_message_entries.h"

/* a screen request as the channels keep and build them: the parameters,
   with field_c cleared on construction */
struct s_screen_request : s_screen_parameters
{
	s_screen_request()
	{
		field_c = 0;
	}
};

class c_window_channel
{
public:
	c_window_channel();

	virtual void clear();
	virtual void reset();
	virtual void v2() {}
	virtual void dispose();
	virtual void update();
	virtual void render(long window);
	virtual void set_next(c_class_1473c9 *screen, s_screen_parameters *request);
	virtual void v7();
	virtual void remove(c_class_1473c9 *screen);
	virtual void v9();
	virtual void v10();

	/* whether the focused screen takes a user's (or controller's) input */
	bool function_235276(long user_index);
	bool function_235294(long controller);

	long m4;
	c_class_1473c9 *current;
	c_class_1473c9 *next;
	s_screen_request request;
	c_class_1473c9 *previous;
	c_class_1473c9 *focus;
};

/* whether a window has a screen or one coming (unknown_1473b6.cpp) */
bool function_1473b6(c_window_channel *window);

/* the channel with a transition state (0x45997c) */
class c_window_channel_45997c : public c_window_channel
{
public:
	c_window_channel_45997c();

	virtual void clear();
	virtual void v2();
	virtual void dispose();
	virtual void update();
	virtual void v10();

	void *get_transition(short index, long *value, short *count, short *frames);

	long m38;
	real m3c;
	byte unknown40[0x10];
	real m50;
	byte unknown54[0xc];
	long m60;
	long state;
};

/* a queued request (0x28 bytes, from the user interface allocator) */
struct s_queued_request
{
	s_queued_request *next;
	s_screen_request request;
	long window;
};

/* the channel with a queue of requests (0x4599a8) */
class c_window_channel_4599a8 : public c_window_channel
{
public:
	c_window_channel_4599a8();

	virtual void clear();
	virtual void dispose();
	virtual void set_next(c_class_1473c9 *screen, s_screen_parameters *request);
	virtual void v7();
	virtual void v9();
	virtual void v11(short count);
	virtual short v12(long window);

	s_queued_request *queue;
	bool m3c;
	bool m3d;
};

/* a channel derived from the queued one without overrides (its vtable is
   identical to 0x4599a8): the window manager's five at 0x54d62c */
class c_window_channel_234e33 : public c_window_channel_4599a8
{
public:
	c_window_channel_234e33();
};

/* 0x459a08 */
class c_window_channel_459a08 : public c_window_channel
{
public:
	c_window_channel_459a08();

	virtual void update();

	long m38;
	long m3c;
};

/* a message the notification channel shows (0x50 bytes; 0x6d080 reads one):
   its type picks the bitmap, its entry gives the sender's name */
struct s_channel_message
{
	long type;
	long id;
	s_entry entry;
	bool flags[4];
	byte unknown4c[4];
};

struct s_channel_slot
{
	dword time;
	byte unknown04[4];
	s_channel_message message;
	bool shown;
	byte unknown59[7];
};

/* 0x459a34 */
class c_window_channel_459a34 : public c_window_channel
{
public:
	c_window_channel_459a34();

	virtual void dispose();
	virtual void update();
	virtual void render(long window);

	c_class_1473c9 *m38;
	c_class_1473c9 *m3c;
	s_channel_slot slots[4];
	dword m1c0;
};

/* a channel with the base's vtable (0x4599dc, folded with the base's): the
   window manager's five windows of channel 1 */
class c_window_channel_2357c9 : public c_window_channel
{
public:
	c_window_channel_2357c9();
};

/* the screen settings copied out by function_14887e (0x78 bytes) */
struct s_screen_settings_54dc6c
{
	dword data[0x1e];
};

struct s_window_manager_754
{
	word data[0x6a2 / 2];
};

struct s_window_manager_df6
{
	word data[0x92 / 2];
};

struct s_window_manager_e94
{
	dword data[0x1c];
};

struct s_window_manager_1248
{
	s_window_manager_1248()
	{
		m10 = NONE;
		m0 = 0;
		m4 = 0;
		mc = 0;
		m8 = 0;
	}

	long m0;
	long m4;
	long m8;
	long mc;
	long m10;
};

/* the window manager (0x54d598, constructed by 0x147645; unknown_147f6d.cpp):
   the windows of each channel, three per-controller arrays of five (the
   fifth, index 4, is shared by all controllers) and three single windows
   that only take index 4. function_148262 maps a channel and an index to one
   of them. The profile being edited (g_54e5d0, at +0x1038) is defined on its
   own, as are 0x54d5a0 (value08), 0x54e49c (mf04), 0x54e7cc (m1234) and
   0x54e7fc (m1264), and the other files read them by those names; 0x1476c7
   clears them through these members, since retail keeps those stores in order
   with the stores to the rest of the object. */
class c_window_manager
{
public:
	c_window_manager();

	byte unknown00[4];
	bool active;
	byte unknown05[0x08 - 0x05];
	/* the game variant screens leave out two engines while this is set */
	bool value08;
	byte unknown09[0xc - 0x09];
	/* the kind of join to start for the first signed in user (set by 0x2395dc,
	   sent by 0x14a08f) */
	long pending_join_type;
	/* the e_shell_mode whose event handler 0x14954a runs */
	char shell_mode;
	byte unknown11[0x14 - 0x11];
	color3f color14;
	long m20;
	byte unknown24[0x28 - 0x24];
	long m28;
	c_window_channel_45997c default_window;
	c_window_channel_234e33 windows_5[5];
	c_window_channel windows_3[5];
	c_window_channel_2357c9 windows_1[5];
	c_window_channel_459a08 window_0;
	byte unknown444[4];
	c_window_channel_459a34 window_4;
	byte unknown60c[4];
	c_window_channel window_2;
	c_class_1473c9 *screens[0x23];
	s_screen_settings_54dc6c settings;
	/* the online tasks that look up the selected player's clans (0x148893) */
	long team_task;
	long team_members_task;
	s_window_manager_754 team_details;
	s_window_manager_df6 team_member_details;
	byte unknowne88[0xe94 - 0xe88];
	s_window_manager_e94 me94;
	long mf04;
	byte mf08[0x130];
	/* the profile being edited (g_54e5d0) */
	s_profile_edit profile_edit;
	bool m1220;
	byte unknown1221[3];
	long m1224;
	long m1228;
	long m122c;
	long m1230;
	/* g_54e7cc */
	bool m1234;
	byte unknown1235;
	byte m1236[0x10];
	byte unknown1246[0x1248 - 0x1246];
	s_window_manager_1248 main_menu_music;
	byte unknown125c[0x1264 - 0x125c];
	/* g_54e7fc */
	dword m1264;
};

extern c_window_manager g_54d598;
/* the session menu values at 0x54e7b8..0x54e7c8 are these window manager
   fields: stores to them may alias other stores, as retail orders them
   (0x2c4ea0) */
#define g_54e7b8 (g_54d598.m1220)
#define g_54e7bc (g_54d598.m1224)
#define g_54e7c0 (g_54d598.m1228)
#define g_54e7c4 (g_54d598.m122c)
#define g_54e7c8 (g_54d598.m1230)

struct s_screen_sort_entry
{
	c_class_1a2c81 *screen;
	real depth;

	short layer;
};

/* the user interface globals (0x148350): the screen tint */
struct s_tag_reference_8
{
	dword group_tag;
	long tag_index;
};

/* a widget animation of the user interface globals (0x2c bytes): the
   keyframes of each animation type */
struct s_widget_animation_keys
{
	long value;
	short frames;
	short pad;
	long target;
};

struct s_widget_animation_definition
{
	byte unknown00[4];
	s_widget_animation_keys a;
	s_widget_animation_keys b;
	long value1c;
	short mode;
	short pad22;
	short frames24;
	short pad26;
	long target28;
};

/* the widgets every screen of a set shows (0x18 bytes) */
struct s_widget_set
{
	byte unknown00[8];
	long model_count;
	s_model_block *models;
	long bitmap_count;
	s_bitmap_block *bitmaps;
};

struct s_type_954545
{
	byte unknown00[0x48];
	short value48;
	short value4a;
	real value4c;
	byte unknown50[0x60 - 0x50];
	color3f value60;
	color4f tint;
	byte unknown7c[0x120 - 0x7c];
	long animation_count;
	s_widget_animation_definition *animations;
	long widget_set_count;
	s_widget_set *widget_sets;
	byte unknown130[0x138 - 0x130];
	long skin_count;
	s_tag_reference_8 *skins;
	byte unknown140[0x144 - 0x140];
	/* the subtitles' unicode string list (0x1496f6) */
	long subtitle_string_list_index;
	byte unknown148[0x158 - 0x148];
	long range_count;
	struct s_user_interface_globals_range
	{
		short lower;
		short upper;
	} *ranges;
	/* the screens' titles: four sizes (picked by the definition's flags) */
	short title_fonts[4];
	color4f title_color;
	struct
	{
		s_widget_bounds title;
		s_widget_bounds subtitle;
	} title_bounds[4];
	/* the main menu's music, a looping sound (main_menu_music.cpp) */
	s_tag_reference_8 main_menu_music;
	/* how long the music stays silent before the menu moves on
	   (milliseconds) */
	long main_menu_music_silence_time;
};

/* the user interface globals tag (ui_definition.cpp) */
s_type_954545 *ui_definition_get(void);

/* the render window passed down to the screens */
class c_render_window
{
public:
	void function_147cdb(dword color);
};

#endif
