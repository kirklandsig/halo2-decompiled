// @flags /O1 /Oi /arch:SSE /Gr
/* UNKNOWN_232D43.CPP: the postgame statistics (the "pcr", the
   postgame carnage report).

   The screen (vtable 0x4596e0) is a tab bar over six tabs. Each tab is a
   screen with one list of the players' statistics: the tabs of the vtables
   0x4593c0, 0x459890, 0x4597b0, 0x459430, 0x459900 and 0x459820 share every
   slot but the one that builds them, which titles their texts, and their
   lists (vtables 0x459750, 0x459680, 0x459620, 0x4595c0, 0x459500 and
   0x4594a0) share every slot of their base (vtable 0x459560) but the one
   that fills a row. */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include "data_array.h"
#include "globals.h"
#include "screen_widgets.h"
#include "unknown_19b510.h"
#include "unknown_19b516.h"
#include "unknown_234c64.h"
#include "unknown_2312b4.h"

bool xuid_equal(XUID const *a, XUID const *b, bool compare_guest_number);
long function_75870(void);
long function_75890(long time);
bool function_592f0(void);
void function_199e2e(bool close);
void function_19a942(void);
bool function_199994(void);
bool function_1999b3(void);
bool function_6c7e0();
bool function_1900a5(long player);
void function_18ff47(long player, dword *out);
long function_1480ff(long screen_id);
void unicode_string_to_ascii(const word *source, char *destination, long maximum_count);
void *__stdcall function_1a47fd(unsigned int size);
long function_19ad9c(XUID const *xuid);
bool voice_port_flag0_only(long port);
bool function_53750(long player_index);
struct s_widget_view_2b0a;
void function_2b0a14(s_widget_view_2b0a *widget, short index);
c_class_1473c9 *__stdcall function_2312af(s_screen_parameters *parameters);
c_class_1473c9 *__stdcall function_23334f(s_screen_parameters *parameters);
/* the item's bitmap hides (unknown_250155.cpp) */
void function_251963(c_class_1a2c81 *item);
word *unicode_string_append(word *destination, const word *source, long maximum_count);
bool network_session_interface_has_user(const XUID *xuid);
long function_19adca(XUID const *xuid);
bool function_19ab77(long player_index);
void function_2b01a2(long value, s_widget_item *item);
void function_22f042(s_widget_item *items, c_class_1a2c81 *widget, long count);

/* what the name lookups take: a player (type 1) */
struct s_name_request
{
	long type;
	s_id_triplet id;
	char name[16];
	byte unknown20[0x78 - 0x20];
};

void __stdcall function_148893(s_name_request *request, long flag);

/* a player of the statistics (0x114 bytes) */
struct s_postgame_player
{
	s_postgame_player();
	word name[0x20];
	word team_name[0x10];
	long value60;
	long value64;
	long value68;
	long value6c;
	short value70;
	short value72;
	bool value74;
	byte unknown75[3];
	long value78;
	long medal_count;
	dword medals;
	long count;
	long value;
	long value2;
	/* the kills of each player */
	long values[0x10];
	dword valued0[4];
	word texte0[0x10];
	s_id_triplet id;
	long value10c;
	long value110;
};

struct s_player_ref
{
	byte unknown00[4];
	long player;
};

/* the players of the statistics, and how many there are */
// @retail 0x232d54
s_postgame_player::s_postgame_player()
{
	name[0] = 0;
	team_name[0] = 0;
	texte0[0] = 0;
}

s_postgame_player g_55caf0[16];
long g_51ec08;
/* the count of the first tab's rows */
long g_51ec0c;

/* the base of the lists (vtable 0x459560) */
class c_postgame_statistics_list : public c_class_1474e8
{
public:
	c_postgame_statistics_list(bool value8a0, long count, long value8a8, word user_flags);

	/* folded with c_widget::v2 (unknown_19b516.cpp) */
	virtual void *get_item_data() { return items; }
	/* folded with c_widget::v3 (unknown_19b516.cpp) */
	virtual long get_item_count() { return 0x10; }
	virtual void v20(c_class_1a2c81 *item, long unused);
	virtual bool v21(c_class_1a2c81 *item);
	/* fills the item with the row (pure in retail; the stand-ins construct
	   the class) */
	virtual void fill_row(c_class_1a2c81 *item, long row) {}
	virtual void handle_item(s_controller_reference **controller, long *item);

	void show_row(long row, c_class_1a2c81 *item);

	c_class_14750b items[0x10];
	c_list_item_handler handler;
	bool value8a0;
	long count;
	long value8a8;
};

class c_postgame_statistics_list_459750 : public c_postgame_statistics_list
{
public:
	c_postgame_statistics_list_459750(word user_flags);

	virtual void fill_row(c_class_1a2c81 *item, long row);
};

class c_postgame_statistics_list_459680 : public c_postgame_statistics_list
{
public:
	c_postgame_statistics_list_459680(bool value8a0, word user_flags);

	virtual void fill_row(c_class_1a2c81 *item, long row);
};

class c_postgame_statistics_list_459620 : public c_postgame_statistics_list
{
public:
	c_postgame_statistics_list_459620(bool value8a0, word user_flags);

	virtual void fill_row(c_class_1a2c81 *item, long row);
};

class c_postgame_statistics_list_4595c0 : public c_postgame_statistics_list
{
public:
	c_postgame_statistics_list_4595c0(bool value8ac, word user_flags);

	virtual void fill_row(c_class_1a2c81 *item, long row);
	virtual void handle_item(s_controller_reference **controller, long *item);

	bool value8ac;
};

class c_postgame_statistics_list_459500 : public c_postgame_statistics_list
{
public:
	c_postgame_statistics_list_459500(bool value8a0, word user_flags);

	virtual void fill_row(c_class_1a2c81 *item, long row);
};

class c_postgame_statistics_list_4594a0 : public c_postgame_statistics_list
{
public:
	c_postgame_statistics_list_4594a0(bool value8a0, word user_flags);

	virtual void fill_row(c_class_1a2c81 *item, long row);
};

/* the tabs: a press is the base widget's (slot 10 is folded), and they cannot
   be created on their own (slot 26 is folded) */
class c_postgame_statistics_screen_4593c0 : public c_class_1473c9
{
public:
	c_postgame_statistics_screen_4593c0(long screen_id, long a, long b, word user_flags);

	virtual bool v10(s_widget_event *event);
	virtual void v18(void *parameters);
	virtual screen_load_proc get_load_proc() { return function_2312af; }

	c_postgame_statistics_list_459750 list;
};

class c_postgame_statistics_screen_459890 : public c_class_1473c9
{
public:
	c_postgame_statistics_screen_459890(long screen_id, long a, long b, word user_flags);

	virtual bool v10(s_widget_event *event);
	virtual void v18(void *parameters);
	virtual screen_load_proc get_load_proc() { return function_2312af; }

	c_postgame_statistics_list_459680 list;
};

class c_postgame_statistics_screen_4597b0 : public c_class_1473c9
{
public:
	c_postgame_statistics_screen_4597b0(long screen_id, long a, long b, word user_flags);

	virtual bool v10(s_widget_event *event);
	virtual void v18(void *parameters);
	virtual screen_load_proc get_load_proc() { return function_2312af; }

	c_postgame_statistics_list_459620 list;
};

class c_postgame_statistics_screen_459430 : public c_class_1473c9
{
public:
	c_postgame_statistics_screen_459430(long screen_id, long a, long b, word user_flags);

	virtual bool v10(s_widget_event *event);
	virtual void v18(void *parameters);
	virtual screen_load_proc get_load_proc() { return function_2312af; }

	c_postgame_statistics_list_4595c0 list;
};

class c_postgame_statistics_screen_459900 : public c_class_1473c9
{
public:
	c_postgame_statistics_screen_459900(long screen_id, long a, long b, word user_flags);

	virtual bool v10(s_widget_event *event);
	virtual void v18(void *parameters);
	virtual screen_load_proc get_load_proc() { return function_2312af; }

	c_postgame_statistics_list_459500 list;
};

class c_postgame_statistics_screen_459820 : public c_class_1473c9
{
public:
	c_postgame_statistics_screen_459820(long screen_id, long a, long b, word user_flags);

	virtual bool v10(s_widget_event *event);
	virtual void v18(void *parameters);
	virtual screen_load_proc get_load_proc() { return function_2312af; }

	c_postgame_statistics_list_4594a0 list;
};

/* the postgame statistics screen (vtable 0x4596e0) */
class c_postgame_statistics_screen : public c_class_1473c9
{
public:
	c_postgame_statistics_screen(long screen_id, long a, long b, word user_flags);

	virtual void v3();
	virtual bool v10(s_widget_event *event);
	virtual void v17();
	virtual void v18(void *parameters);
	virtual void v23(void *window);
	/* folded with c_screen_4596e0's (unknown_23068b.cpp) */
	virtual screen_load_proc get_load_proc() { return function_23334f; }

	bool handle_back(s_widget_event *event);

	bool tab_is_current(c_class_1473c9 *tab)
	{
		return tab && tab == tab_bar.focused;
	}

	c_y_menu_tab_bar tab_bar;
	c_postgame_statistics_screen_4593c0 tab1;
	c_postgame_statistics_screen_459890 tab2;
	c_postgame_statistics_screen_4597b0 tab3;
	c_postgame_statistics_screen_459430 tab4;
	c_postgame_statistics_screen_459900 tab5;
	c_postgame_statistics_screen_459820 tab6;
	/* opened from the saved film (screen 0xb9) */
	bool value5ef0;
	long start_time;
	long value5ef8;
};

/* the game engine of the game the statistics are for */
long g_50224c;

// @retail 0x23381b
void c_postgame_statistics_screen_4593c0::v18(void *parameters)
{
	c_text_widget_45a5e0 *title;
	c_text_widget_45a5e0 *heading1;
	c_text_widget_45a5e0 *heading2;

	c_class_1a2c81::v1();
	title = (c_text_widget_45a5e0 *)find_child(6, 0, false);
	heading1 = (c_text_widget_45a5e0 *)find_child(6, 1, false);
	heading2 = (c_text_widget_45a5e0 *)find_child(6, 2, false);
	if (title)
	{
		title->function_253b1a(0x4000278);
		title->value6e = true;
	}
	if (heading1)
	{
		heading1->function_253b1a(0x5000733);
		heading1->value6e = true;
	}
	if (heading2)
	{
		heading2->function_253b1a(0x5000735);
		heading2->value6e = true;
	}
}

// @retail 0x233b81
void c_postgame_statistics_screen_459430::v18(void *parameters)
{
	c_text_widget_45a5e0 *title;
	c_text_widget_45a5e0 *heading1;
	c_text_widget_45a5e0 *heading2;

	c_class_1a2c81::v1();
	title = (c_text_widget_45a5e0 *)find_child(6, 0, false);
	heading1 = (c_text_widget_45a5e0 *)find_child(6, 1, false);
	heading2 = (c_text_widget_45a5e0 *)find_child(6, 2, false);
	if (title)
	{
		title->function_253b1a(0x6000734);
		title->value6e = true;
	}
	if (heading1)
	{
		heading1->function_253b1a(0xa00073a);
		heading1->value6e = true;
	}
	if (heading2)
	{
		heading2->function_253b1a(0xd00073b);
		heading2->value6e = true;
	}
}

// @retail 0x233c35
void c_postgame_statistics_screen_459900::v18(void *parameters)
{
	c_text_widget_45a5e0 *title;
	c_text_widget_45a5e0 *heading1;
	c_text_widget_45a5e0 *heading2;

	c_class_1a2c81::v1();
	title = (c_text_widget_45a5e0 *)find_child(6, 0, false);
	heading1 = (c_text_widget_45a5e0 *)find_child(6, 1, false);
	heading2 = (c_text_widget_45a5e0 *)find_child(6, 2, false);
	if (title)
	{
		title->function_253b1a(0x6000734);
		title->value6e = true;
	}
	if (heading1)
	{
		heading1->function_253b1a(0xc00073f);
		heading1->value6e = true;
	}
	if (heading2)
	{
		heading2->function_253b1a(0xa000740);
		heading2->value6e = true;
	}
}

// @retail 0x233a7e
void c_postgame_statistics_screen_4597b0::v18(void *parameters)
{
	c_text_widget_45a5e0 *title;
	c_text_widget_45a5e0 *heading1;
	c_text_widget_45a5e0 *heading2;
	c_text_widget_45a5e0 *heading3;
	c_text_widget_45a5e0 *heading4;

	c_class_1a2c81::v1();
	title = (c_text_widget_45a5e0 *)find_child(6, 0, false);
	heading1 = (c_text_widget_45a5e0 *)find_child(6, 1, false);
	heading2 = (c_text_widget_45a5e0 *)find_child(6, 2, false);
	heading3 = (c_text_widget_45a5e0 *)find_child(6, 3, false);
	heading4 = (c_text_widget_45a5e0 *)find_child(6, 4, false);
	if (title)
	{
		title->function_253b1a(0x6000734);
		title->value6e = true;
	}
	if (heading1)
	{
		heading1->function_253b1a(0x5000739);
		heading1->value6e = true;
	}
	if (heading2)
	{
		heading2->function_253b1a(0x7000737);
		heading2->value6e = true;
	}
	if (heading3)
	{
		heading3->function_253b1a(0x6000738);
		heading3->value6e = true;
	}
	if (heading4)
	{
		heading4->function_253b1a(0x800073c);
		heading4->value6e = true;
	}
}

// @retail 0x233d07
void c_postgame_statistics_screen_459820::v18(void *parameters)
{
	c_text_widget_45a5e0 *title;
	c_text_widget_45a5e0 *heading1;
	c_text_widget_45a5e0 *heading2;
	c_text_widget_45a5e0 *heading3;
	c_text_widget_45a5e0 *heading4;

	c_class_1a2c81::v1();
	title = (c_text_widget_45a5e0 *)find_child(6, 0, false);
	heading1 = (c_text_widget_45a5e0 *)find_child(6, 1, false);
	heading2 = (c_text_widget_45a5e0 *)find_child(6, 2, false);
	heading3 = (c_text_widget_45a5e0 *)find_child(6, 3, false);
	heading4 = (c_text_widget_45a5e0 *)find_child(6, 4, false);
	if (title)
	{
		title->function_253b1a(0x6000734);
		title->value6e = true;
	}
	if (heading1)
	{
		heading1->function_253b1a(0x900075a);
		heading1->value6e = true;
	}
	if (heading2)
	{
		heading2->function_253b1a(0xb00075b);
		heading2->value6e = true;
	}
	if (heading3)
	{
		heading3->function_253b1a(0xe00075c);
		heading3->value6e = true;
	}
	if (heading4)
	{
		heading4->function_253b1a(0xa00075d);
		heading4->value6e = true;
	}
}

// @retail 0x2338cf
void c_postgame_statistics_screen_459890::v18(void *parameters)
{
	c_text_widget_45a5e0 *title;
	c_text_widget_45a5e0 *heading1;
	c_text_widget_45a5e0 *heading2;
	c_text_widget_45a5e0 *column1;
	c_text_widget_45a5e0 *column2;

	c_class_1a2c81::v1();
	title = (c_text_widget_45a5e0 *)find_child(6, 0, false);
	heading1 = (c_text_widget_45a5e0 *)find_child(6, 1, false);
	heading2 = (c_text_widget_45a5e0 *)find_child(6, 4, false);
	column1 = (c_text_widget_45a5e0 *)find_child(6, 2, false);
	column2 = (c_text_widget_45a5e0 *)find_child(6, 3, false);
	if (title)
	{
		title->function_253b1a(0x6000734);
		title->value6e = true;
	}
	if (heading1)
	{
		heading1->function_253b1a(0x5000733);
		heading1->value6e = true;
	}
	if (heading2)
	{
		heading2->function_253b1a(0x5000735);
		heading2->value6e = true;
	}
	if (column1 && column2)
	{
		if (g_50224c == 2)
		{
			column1->function_253b1a(0x8000723);
			column2->function_253b1a(0xa000724);
		}
		else if (g_50224c == 1)
		{
			column1->function_253b1a(0xa000725);
			column2->function_253b1a(0xc000726);
		}
		else if (g_50224c == 3)
		{
			column1->function_253b1a(0xd00072a);
			column2->function_253b1a(0xf00072b);
		}
		else if (g_50224c == 4)
		{
			column1->function_253b1a(0xd000728);
			column2->function_253b1a(0xf000729);
		}
		else if (g_50224c == 7)
		{
			column1->function_253b1a(0xc00072d);
			column2->function_253b1a(0xd00072e);
		}
		else if (g_50224c == 8)
		{
			column1->function_253b1a(0xb00072f);
			column2->function_253b1a(0xa000730);
		}
		else if (g_50224c == 9)
		{
			column1->function_253b1a(0xa000731);
			column2->function_253b1a(0xa000732);
		}
		column1->value6e = true;
		column2->value6e = true;
	}
}

/* a player of the first tab (0x84 bytes) */
struct s_postgame_team
{
	s_postgame_team();
	word name[0x20];
	word score[0x10];
	short unknown60;
	short color_index;
	word place[0x10];
};

// @retail 0x232d67
s_postgame_team::s_postgame_team()
{
	name[0] = 0;
	score[0] = 0;
	place[0] = 0;
}

s_postgame_team g_55dc30[16];

bool function_15dea0();
bool function_15eaf0();
void function_1990b0(long *devices, long *device_count, long *entry_count, long *entries);
long game_results_get_game_type();
void function_19eae0(long team, word *buffer);
void unicode_string_copy(word *destination, word const *source, long maximum_count);
long game_results_get_team_score(long index);
void function_159130(long value, word *text);
long game_results_get_team_standing(long index);
void function_159050(long index, word *buffer);
long game_results_get_player_versus_statistic(long a, long b, long c);
long game_results_get_player_medal_count(long a, long b);
void *game_results_get_player_name(long index);
long game_results_get_player_score(long index);
long function_1969a0(long a, long c, long b);
long game_results_get_player_standing(long index);
long function_196d20(long index);
bool function_19a0c5();
long function_196da0(long index);
void function_196d50(long index, s_input_vector *out);
void *game_results_get_player_identifier(long index);
long game_results_get_player_category_statistic(long a, long b, long c);

// @retail 0x232d77
void function_232d77()
{
	if (function_15dea0())
	{
		bool teams = function_15eaf0();
		memset(g_55caf0, 0, sizeof(g_55caf0));
		memset(g_55dc30, 0, sizeof(g_55dc30));
		long player_count, team_count;
		long players[16], team_indices[16];
		function_1990b0(players, &player_count, &team_count, team_indices);
		g_51ec08 = player_count;
		g_51ec0c = team_count;
		g_50224c = game_results_get_game_type();
		word name[0x100], score[0x100], place[0x100];
		if (teams)
		{
			for (long i = 0; i < team_count; i++)
			{
				long team = team_indices[i];
				name[0] = 0;
				score[0] = 0;
				place[0] = 0;
				function_19eae0(team, name);
				unicode_string_copy(g_55dc30[i].name, name, 0x20);
				function_159130(game_results_get_team_score(team), score);
				unicode_string_copy(g_55dc30[i].score, score, 0x10);
				g_55dc30[i].unknown60 = (short)game_results_get_team_standing(team);
				g_55dc30[i].color_index = (short)team;
				function_159050(g_55dc30[i].unknown60, place);
				unicode_string_copy(g_55dc30[i].place, place, 0x10);
			}
		}
		for (long row = 0; row < g_51ec08; row++)
		{
			long player = players[row];
			place[0] = 0;
			score[0] = 0;
			for (long column = 0; column < g_51ec08; column++)
				g_55caf0[row].values[column] = game_results_get_player_versus_statistic(player, players[column], 0);
			g_55caf0[row].medal_count = 0;
			g_55caf0[row].medals = 0;
			bool group13 = false, group0 = false;
			for (long medal = 31; medal >= 0; medal--)
			{
				long count = game_results_get_player_medal_count(player, medal);
				if (count > 0)
				{
					if (medal <= 17 && medal >= 13)
					{
						if (!group13)
						{
							group13 = true;
							g_55caf0[row].medals |= 1 << medal;
						}
					}
					else if (medal <= 5 && medal >= 0)
					{
						if (!group0)
						{
							group0 = true;
							g_55caf0[row].medals |= 1 << medal;
						}
					}
					else
						g_55caf0[row].medals |= 1 << medal;
					g_55caf0[row].medal_count += count;
				}
			}
			unicode_string_copy(g_55caf0[row].name, (word *)game_results_get_player_name(player), 0x20);
			function_159130(game_results_get_player_score(player), place);
			unicode_string_copy(g_55caf0[row].team_name, place, 0x10);
			g_55caf0[row].value60 = function_1969a0(player, NONE, 7);
			g_55caf0[row].value64 = function_1969a0(player, NONE, 9);
			g_55caf0[row].value68 = function_1969a0(player, NONE, 8);
			g_55caf0[row].value6c = function_1969a0(player, NONE, 11);
			g_55caf0[row].value70 = (short)game_results_get_player_standing(player);
			g_55caf0[row].value72 = (short)(teams ? function_196d20(player) : NONE);
			g_55caf0[row].value74 = function_196d20(player) == NONE;
			g_55caf0[row].value78 = function_19a0c5() ? function_196da0(player) : NONE;
			function_196d50(player, (s_input_vector *)g_55caf0[row].valued0);
			function_159050(g_55caf0[row].value70, score);
			unicode_string_copy(g_55caf0[row].texte0, score, 0x10);
			g_55caf0[row].id = *(s_id_triplet *)game_results_get_player_identifier(player);
			if (g_50224c == 2)
			{
				long deaths = function_1969a0(player, NONE, 9) + 1;
				long divisor = deaths > 1 ? deaths : 1;
				g_55caf0[row].value10c = function_1969a0(player, NONE, 13) / divisor;
				g_55caf0[row].value110 = function_1969a0(player, NONE, 12);
			}
			else if (g_50224c == 1)
			{
				g_55caf0[row].value10c = function_1969a0(player, NONE, 16);
				g_55caf0[row].value110 = function_1969a0(player, NONE, 15);
			}
			else if (g_50224c == 3)
			{
				g_55caf0[row].value10c = function_1969a0(player, NONE, 26);
				g_55caf0[row].value110 = function_1969a0(player, NONE, 25);
			}
			else if (g_50224c == 4)
			{
				g_55caf0[row].value10c = function_1969a0(player, NONE, 28);
				g_55caf0[row].value110 = function_1969a0(player, NONE, 27);
			}
			else if (g_50224c == 7)
			{
				g_55caf0[row].value10c = function_1969a0(player, NONE, 38);
				g_55caf0[row].value110 = function_1969a0(player, NONE, 39);
			}
			else if (g_50224c == 8)
			{
				g_55caf0[row].value10c = function_1969a0(player, NONE, 43);
				g_55caf0[row].value110 = function_1969a0(player, NONE, 44);
			}
			else if (g_50224c == 9)
			{
				g_55caf0[row].value10c = function_1969a0(player, NONE, 21);
				g_55caf0[row].value110 = function_1969a0(player, NONE, 20);
			}
			g_55caf0[row].count = 0;
			g_55caf0[row].value = 0;
			g_55caf0[row].value2 = 0;
			long *total_count = &g_55caf0[row].count;
			long *total_value = &g_55caf0[row].value;
			long *total_value2 = &g_55caf0[row].value2;
			for (long statistic = 5; statistic < 39; statistic++)
			{
				*total_count += game_results_get_player_category_statistic(player, 4, statistic);
				*total_value += game_results_get_player_category_statistic(player, 5, statistic);
				*total_value2 += game_results_get_player_category_statistic(player, 6, statistic);
			}
		}
	}
}

color3f *function_7f720(color3f *color, short team_index);
hsv3f *function_1318d0(const color3f *rgb, hsv3f *hsv);
color3f *function_131a00(const hsv3f *hsv, color3f *rgb);

/* the row's player: name, emblem and colours, grey when the player has left */
// @retail 0x233f0f
void c_postgame_statistics_list::show_row(long row, c_class_1a2c81 *item)
{
	s_widget_item definition;
	color3f grey;
	bool present;

	definition.flags = 0;
	present = network_session_interface_has_user((XUID const *)&g_55caf0[row].id);
	if (present)
	{
		long player_index = function_19adca((XUID const *)&g_55caf0[row].id);

		if (player_index != NONE)
		{
			function_19ab77(player_index);
		}
	}
	function_2b01a2(g_55caf0[row].value78, &definition);
	memcpy(definition.value48, g_55caf0[row].valued0, sizeof(definition.value48));
	definition.flags |= 2;
	definition.value4 = (long)g_55caf0[row].name;
	definition.flags |= 1;
	definition.value5c = g_55caf0[row].value72;
	definition.flags |= 4;
	definition.value5f = g_55caf0[row].value74;
	definition.flags |= 0x80;
	if (!present)
	{
		grey.red = 0.3f;
		grey.green = 0.3f;
		grey.blue = 0.3f;
		definition.color = grey;
		definition.flags |= 0x200;
	}
	function_22f042(&definition, item, 1);
}

/* the player's voice icon on the item */
// @retail 0x233fd5
void function_233fd5(c_postgame_statistics_list *list, long row, c_class_1a2c81 *item)
{
	/* Retail passes the unused list pointer before the item on the stack. */
	c_postgame_statistics_list **list_reference = &list;
	c_class_1a2c81 *bitmap = item->find_child(8, 0, false);

	if (bitmap)
	{
		long player = function_19ad9c((XUID const *)&g_55caf0[row].id);

		if (player != NONE)
		{
			short index;

			if (voice_port_flag0_only(player))
			{
				index = !function_53750(player);
			}
			else
			{
				index = 2;
			}
			function_2b0a14((s_widget_view_2b0a *)bitmap, index);
			bitmap->value6e = true;
		}
		else
		{
			function_251963(item);
		}
	}
}

// @retail 0x233dcd
c_postgame_statistics_list::c_postgame_statistics_list(bool value8a0, long count, long value8a8, word user_flags) :
	c_class_1474e8(user_flags),
	handler(this, (list_item_method)&c_postgame_statistics_list::handle_item),
	value8a0(value8a0),
	count(count),
	value8a8(value8a8)
{
	if (count > 0)
	{
		data = user_interface_data_new("pcr list", count, 4);
		if (data)
		{
			function_16b790(data);
			for (long i = 0; i < this->count; i++)
			{
				long index = record_pool_allocate(data);

				if (index != NONE)
				{
					((s_list_item_datum *)data->data)[index & 0xffff].item = (short)i;
				}
			}
		}
	}
	delegate_register(&item_handlers, &handler);
}

// the compiler's thunk for &c_postgame_statistics_list::handle_item, which calls through vtable offset 0x5c
// @retail 0x234c5f vcall 0x5c
// @retail 0x233319 destructor c_postgame_statistics_list
// @retail 0x234509 deleting c_postgame_statistics_list

// @retail 0x233e97
bool c_postgame_statistics_list::v21(c_class_1a2c81 *item)
{
	bool result = false;

	if (value8a0 == result)
	{
		result = c_class_1474e8::v21(item);
	}
	return result;
}

// @retail 0x234169
c_postgame_statistics_list_459750::c_postgame_statistics_list_459750(word user_flags) :
	c_postgame_statistics_list(true, g_51ec0c, 1, user_flags)
{
}

// @retail 0x2342ac
c_postgame_statistics_list_459680::c_postgame_statistics_list_459680(bool value8a0, word user_flags) :
	c_postgame_statistics_list(value8a0 || function_199994() || function_1999b3(), g_51ec08, 0, user_flags)
{
}

// @retail 0x2344cc
c_postgame_statistics_list_459620::c_postgame_statistics_list_459620(bool value8a0, word user_flags) :
	c_postgame_statistics_list(value8a0 || function_199994() || function_1999b3(), g_51ec08, 0, user_flags)
{
}

// @retail 0x23469d
c_postgame_statistics_list_4595c0::c_postgame_statistics_list_4595c0(bool value8ac, word user_flags) :
	c_postgame_statistics_list(false, g_51ec08, 0, user_flags),
	value8ac(value8ac)
{
}

// @retail 0x2347ef
c_postgame_statistics_list_459500::c_postgame_statistics_list_459500(bool value8a0, word user_flags) :
	c_postgame_statistics_list(value8a0 || function_199994() || function_1999b3(), g_51ec08, 0, user_flags)
{
}

// @retail 0x234a5a
c_postgame_statistics_list_4594a0::c_postgame_statistics_list_4594a0(bool value8a0, word user_flags) :
	c_postgame_statistics_list(value8a0 || function_199994() || function_1999b3(), g_51ec08, 0, user_flags)
{
}

/* a player's choice asks for the player's gamertag card */
// @retail 0x23403c
void c_postgame_statistics_list::handle_item(s_controller_reference **controller, long *item)
{
	long handle = *item;
	long index = handle & 0xffff;

	{
		dword local[0x1c];

		function_18ff47((*controller)->controller_index, local);
		if (value8a0 || handle == NONE || !function_6c7e0() || function_199994() || function_1999b3() || function_1900a5((*controller)->controller_index))
		{
			return;
		}
		if (xuid_equal((XUID const *)&g_55caf0[index].id, (XUID const *)local, false))
		{
			return;
		}
	}
	if (index >= 0 && index < g_51ec08 && !(g_55caf0[index].id.c & 3))
	{
		s_name_request request;
		s_message message;

		message.field_c = 0;
		memset(&request, 0, sizeof(request));
		request.type = 1;
		request.id = g_55caf0[index].id;
		unicode_string_to_ascii(g_55caf0[index].name, request.name, 16);
		request.name[15] = 0;
		function_148893(&request, 1);
		if (request.id.b | request.id.a)
		{
			function_149f49(&message, 0, 0, 1 << (*controller)->controller_index, 3, 4, 0x2b7223);
			message.callback(&message);
		}
	}
}

// @retail 0x2346c8
void c_postgame_statistics_list_4595c0::handle_item(s_controller_reference **controller, long *item)
{
	if (!value8ac)
	{
		c_postgame_statistics_list::handle_item(controller, item);
	}
}

/* the first tab's row: the team's name, score and place in its colour */
// @retail 0x23418a
void c_postgame_statistics_list_459750::fill_row(c_class_1a2c81 *item, long row)
{
	c_class_1a2c81 *name = item->find_child(6, 0, false);
	c_class_1a2c81 *place = item->find_child(6, 1, false);
	c_class_1a2c81 *score = item->find_child(6, 2, false);
	c_class_1a2c81 *bitmap = item->find_child(8, 0, false);
	struct
	{
		color3f bitmap_color;
		color3f color;
		hsv3f hsv;
	} colors;

	colors.bitmap_color = *function_7f720(&colors.color, g_55dc30[row].color_index);
	function_1318d0(&colors.bitmap_color, &colors.hsv);
	colors.hsv.saturation = 0.20833333f;
	colors.hsv.value = 0.79166669f;
	function_131a00(&colors.hsv, &colors.color);
	if (name)
	{
		name->function_22f52e()->set_text(g_55dc30[row].name);
		name->color = colors.color;
	}
	if (place)
	{
		place->function_22f52e()->set_text(g_55dc30[row].place);
		place->color = colors.color;
	}
	if (score)
	{
		score->function_22f52e()->set_text(g_55dc30[row].score);
		score->color = colors.color;
	}
	if (bitmap)
	{
		bitmap->color = colors.bitmap_color;
		bitmap->value6e = true;
	}
}

/* a string buffer the screen's strings load into */
struct s_string_buffer
{
	word text[0x100];
	void load(c_widget *screen, long handle) { screen->function_230134(handle, text); }
};

/* the player's name, team and the game engine's two scores */
// score 13 (was 13): discrete 7/8 tests and the final 9 test after RET differ;
// an earlier comparison at 65 (from 50) had the arguments pushed in the opposite order and the two
// virtual calls made in the other order;
// low/high partitions 59/56, conditional membership 57/53, classified result
// 57/53, upper/lower range result 95/53 (plain scores).
// Range in a bool local 13; both values under a repeated test 104; conditional
// values 140; forced-inline mode predicate 50; one flag for both values 50 (fast).
// r105 (fast): the plain six-way chain 65 (480 bytes, 9 in line); the chain without 9
// and an `else if (== 9)` arm with the same loads 65 (the arms merge); the chain in a
// bool, a long, or a flag set by an if 65; the score1 format call in both arms with the
// chain 100 (483 bytes) or the range 104.
// Independent final-mode if 63 (501 bytes); nested output guards with the range 13,
// with the chain 50; score-widget declarations split 50; one value declaration 50;
// upper-bound-first range 14; 7/9/8 chain 51; grouped final equalities 50 (plain).
// Round 41 (lane 10, fast): retail's `cmp 9; jne <score1 format>; jmp <loads>` after RET
// looks like a second copy of the loads cross-jumped into the first after layout. Six
// separate else-if arms with the same loads 50 (merged, 9 in line); the 1/3/4/7/8 chain,
// an `== 9` arm and a final else arm, each with its own score1 format call 95 (484 bytes,
// the format string goes to EDI); the same as seven arms 95; the 1/3/4/7/8 chain with an
// `== 9` arm whose loads are in the other order 50, the chain's loads reversed instead 50;
// a multi-label switch (diagnostic) 100 (range compares).
// Round 48 (fast here gives 34 at 475 bytes on the unchanged source: the reduced image passes
// show_row's row on the stack; aligned counts against that): a 1/3/4/7/8 switch with case 9 as its
// own identical arm, after or before, 101 (range compares, aligned 24); `>= 7 && <= 8` plus `== 9`
// 65 (aligned 10); the 1/3/4/7/8 chain with the `== 9` test nested in its else 65 (aligned 9: only
// the 9 test is in line); the chain and a separate `if (== 9)` 76, with `>= 7 && <= 8` 77 (500 bytes,
// not threaded).
// @retail 0x2342e9
void c_postgame_statistics_list_459680::fill_row(c_class_1a2c81 *item, long row)
{
	c_class_1a2c81 *header = item->find_child(10, 0, false);
	c_class_1a2c81 *name = header->find_child(6, 0, false);
	c_class_1a2c81 *title = header->find_child(6, 1, false);
	c_class_1a2c81 *team = header->find_child(6, 4, false);
	c_class_1a2c81 *score1_text = header->find_child(6, 2, false);
	c_class_1a2c81 *score2_text = header->find_child(6, 3, false);

	show_row(row, item);
	if (name)
	{
		name->function_22f52e()->set_text(g_55caf0[row].name);
	}
	if (title)
	{
		title->function_22f52e()->set_text(g_55caf0[row].texte0);
	}
	if (team)
	{
		team->function_22f52e()->set_text(g_55caf0[row].team_name);
	}
	if (score1_text && score2_text)
	{
		s_string_buffer format;
		word score1[0x100];
		word score2[0x100];
		long value1 = 0;
		long value2 = 0;

		format.text[0] = 0;
		score1[0] = 0;
		score2[0] = 0;
		if (g_50224c == 2)
		{
			value1 = g_55caf0[row].value10c;
			value2 = g_55caf0[row].value110;
			format.load((c_widget *)get_screen(), 0xb000736);
			function_1630e0(score1, format.text, value1 / 60, value1 % 60);
		}
		else
		{
			if (g_50224c == 1 || g_50224c == 3 || g_50224c == 4 || (g_50224c >= 7 && g_50224c <= 9))
			{
				value1 = g_55caf0[row].value10c;
				value2 = g_55caf0[row].value110;
			}
			function_1630e0(score1, (const word *)L"%d", value1);
		}
		function_1630e0(score2, (const word *)L"%d", value2);
		score1_text->function_22f52e()->set_text(score1);
		score2_text->function_22f52e()->set_text(score2);
	}
}

/* the player's name and four counts */
// @retail 0x234527
void c_postgame_statistics_list_459620::fill_row(c_class_1a2c81 *item, long row)
{
	word text[0x100];
	c_class_1a2c81 *header;
	c_class_1a2c81 *name;
	c_class_1a2c81 *text1;
	c_class_1a2c81 *text2;
	c_class_1a2c81 *text3;
	c_class_1a2c81 *text4;

	text[0] = 0;
	header = item->find_child(10, 0, false);
	name = header->find_child(6, 0, false);
	text1 = header->find_child(6, 1, false);
	text2 = header->find_child(6, 2, false);
	text3 = header->find_child(6, 3, false);
	text4 = header->find_child(6, 4, false);
	show_row(row, item);
	if (name)
	{
		name->function_22f52e()->set_text(g_55caf0[row].name);
	}
	if (text1)
	{
		function_1630e0(text, (const word *)L"%d", g_55caf0[row].value60);
		text1->function_22f52e()->set_text(text);
	}
	if (text2)
	{
		function_1630e0(text, (const word *)L"%d", g_55caf0[row].value68);
		text2->function_22f52e()->set_text(text);
	}
	if (text3)
	{
		function_1630e0(text, (const word *)L"%d", g_55caf0[row].value64);
		text3->function_22f52e()->set_text(text);
	}
	if (text4)
	{
		function_1630e0(text, (const word *)L"%d", g_55caf0[row].value6c);
		text4->function_22f52e()->set_text(text);
	}
}

/* the kills between the player of the row and the focused player */
// @retail 0x2346d9
void c_postgame_statistics_list_4595c0::fill_row(c_class_1a2c81 *item, long row)
{
	word text[0x100];
	c_class_1a2c81 *header;
	c_class_1a2c81 *name;
	c_class_1a2c81 *killed;
	c_class_1a2c81 *killed_by;
	long column;

	text[0] = 0;
	header = item->find_child(10, 0, false);
	name = header->find_child(6, 0, false);
	killed = header->find_child(6, 1, false);
	killed_by = header->find_child(6, 2, false);
	if (get_focused_datum() != NONE)
	{
		column = get_focused_datum() & 0xffff;
	}
	else
	{
		column = 0;
	}
	show_row(row, item);
	if (name)
	{
		name->function_22f52e()->set_text(g_55caf0[row].name);
	}
	if (killed)
	{
		function_1630e0(text, (const word *)L"%d", g_55caf0[column].values[row]);
		killed->function_22f52e()->set_text(text);
	}
	if (killed_by)
	{
		function_1630e0(text, (const word *)L"%d", g_55caf0[row].values[column]);
		killed_by->function_22f52e()->set_text(text);
	}
}

/* the player's name, medal count and up to eight of the medals' names */
// @retail 0x23482c
void c_postgame_statistics_list_459500::fill_row(c_class_1a2c81 *item, long row)
{
	c_class_1a2c81 *header = item->find_child(10, 0, false);
	c_class_1a2c81 *name = header->find_child(6, 0, false);
	c_class_1a2c81 *count_text = header->find_child(6, 1, false);
	c_class_1a2c81 *medals_text = header->find_child(6, 2, false);
	word text[0x100];

	show_row(row, item);
	if (name)
	{
		name->function_22f52e()->set_text(g_55caf0[row].name);
	}
	if (count_text)
	{
		text[0] = 0;
		function_1630e0(text, (const word *)L"%d", g_55caf0[row].medal_count);
		count_text->function_22f52e()->set_text(text);
	}
	if (medals_text)
	{
		if (g_55caf0[row].medals)
		{
			s_string_buffer medal_name;
			long shown;
			long i;

			medal_name.text[0] = 0;
			text[0] = 0;
			shown = 0;
			long string_ids[0x18] =
			{
				0xf000741, 0xf000742, 0xf000743, 0xf000744, 0xf000745, 0xf000746,
				0xb000747, 0xe000748, 0x9000749, 0xc00074a, 0xe00074b, 0xf00074c,
				0xb00074d, 0x1000074e, 0x1100074f, 0x11000750, 0x11000751, 0x11000752,
				0x9000753, 0x11000754, 0xb000755, 0xa000756, 0x11000757, 0xd000758
			};
			for (i = 0x17; i >= 0 && shown < 8; i--)
			{
				if (g_55caf0[row].medals & (1 << i))
				{
					if (shown)
					{
						unicode_string_append(text, (const word *)L" ", 0x100);
						text[0xff] = 0;
					}
					medal_name.load((c_widget *)get_screen(), string_ids[i]);
					unicode_string_append(text, medal_name.text, 0x100);
					shown++;
					text[0xff] = 0;
				}
			}
			if (shown > 0)
			{
				medals_text->function_22f52e()->set_text(text);
				medals_text->value6e = true;
				return;
			}
		}
		medals_text->value6e = false;
	}
}

/* the row's name, kills, deaths, accuracy and assists */
// @retail 0x234a97
void c_postgame_statistics_list_4594a0::fill_row(c_class_1a2c81 *item, long row)
{
	s_string_buffer text;
	word percent_text[256];

	text.text[0] = 0;
	c_class_1a2c81 *header = item->find_child(10, 0, false);
	c_class_1a2c81 *name = header->find_child(6, 0, false);
	c_class_1a2c81 *score = header->find_child(6, 1, false);
	c_class_1a2c81 *total = header->find_child(6, 2, false);
	c_class_1a2c81 *percent = header->find_child(6, 3, false);
	c_class_1a2c81 *extra = header->find_child(6, 4, false);
	show_row(row, item);
	if (name)
	{
		name->function_22f52e()->set_text(g_55caf0[row].name);
	}
	if (score)
	{
		function_1630e0(text.text, (const word *)L"%d", g_55caf0[row].value);
		score->function_22f52e()->set_text(text.text);
	}
	if (total)
	{
		function_1630e0(text.text, (const word *)L"%d", g_55caf0[row].count);
		total->function_22f52e()->set_text(text.text);
	}
	if (percent)
	{
		long percentage;

		percent_text[0] = 0;
		percentage = 0;
		if (g_55caf0[row].count > 0)
		{
			percentage = (long)((real)g_55caf0[row].value * 100.0f / (real)g_55caf0[row].count);
		}
		text.load((c_widget *)get_screen(), 0x11000759);
		function_1630e0(percent_text, text.text, percentage);
		percent->function_22f52e()->set_text(percent_text);
	}
	if (extra)
	{
		function_1630e0(text.text, (const word *)L"%d", g_55caf0[row].value2);
		extra->function_22f52e()->set_text(text.text);
	}
}

/* ---- the tabs ---- */

// @retail 0x2337e9
c_postgame_statistics_screen_4593c0::c_postgame_statistics_screen_4593c0(long screen_id, long a, long b, word user_flags) :
	c_class_1473c9(screen_id, a, b, user_flags),
	list(user_flags)
{
}

// @retail 0x233892
c_postgame_statistics_screen_459890::c_postgame_statistics_screen_459890(long screen_id, long a, long b, word user_flags) :
	c_class_1473c9(screen_id, a, b, user_flags),
	list(screen_id == 0xb9, user_flags)
{
}

// @retail 0x233a41
c_postgame_statistics_screen_4597b0::c_postgame_statistics_screen_4597b0(long screen_id, long a, long b, word user_flags) :
	c_class_1473c9(screen_id, a, b, user_flags),
	list(screen_id == 0xb9, user_flags)
{
}

// @retail 0x233b44
c_postgame_statistics_screen_459430::c_postgame_statistics_screen_459430(long screen_id, long a, long b, word user_flags) :
	c_class_1473c9(screen_id, a, b, user_flags),
	list(screen_id == 0xb9, user_flags)
{
}

// @retail 0x233bf8
c_postgame_statistics_screen_459900::c_postgame_statistics_screen_459900(long screen_id, long a, long b, word user_flags) :
	c_class_1473c9(screen_id, a, b, user_flags),
	list(screen_id == 0xb9, user_flags)
{
}

// @retail 0x233cac
c_postgame_statistics_screen_459820::c_postgame_statistics_screen_459820(long screen_id, long a, long b, word user_flags) :
	c_class_1473c9(screen_id, a, b, user_flags),
	list(screen_id == 0xb9, user_flags)
{
}

// @retail 0x2332a7 destructor c_postgame_statistics_screen_4593c0
// @retail 0x233ce9 deleting c_postgame_statistics_screen_4593c0

// @retail 0x232d43
bool c_postgame_statistics_screen_4593c0::v10(s_widget_event *event)
{
	return c_class_1a2c81::v10(event);
}

bool c_postgame_statistics_screen_459890::v10(s_widget_event *event)
{
	return c_class_1a2c81::v10(event);
}

bool c_postgame_statistics_screen_4597b0::v10(s_widget_event *event)
{
	return c_class_1a2c81::v10(event);
}

bool c_postgame_statistics_screen_459430::v10(s_widget_event *event)
{
	return c_class_1a2c81::v10(event);
}

bool c_postgame_statistics_screen_459900::v10(s_widget_event *event)
{
	return c_class_1a2c81::v10(event);
}

bool c_postgame_statistics_screen_459820::v10(s_widget_event *event)
{
	return c_class_1a2c81::v10(event);
}

/* the item of the row the list shows (the list's slot 20): filled, and its
   player's voice icon shown */
// @retail 0x233ea9
void c_postgame_statistics_list::v20(c_class_1a2c81 *item, long unused)
{
	c_class_1473c9 *tab = (c_class_1473c9 *)parent;
	c_postgame_statistics_screen *screen = (c_postgame_statistics_screen *)parent->parent->parent;

	{
		if (tab != 0 && tab == screen->tab_bar.focused)
		{
			long row = widget_item(item)->value70 & 0xffff;

			if (row >= 0 && row < count)
			{
				fill_row(item, row);
				if (!value8a8)
				{
					if (screen->value5ef0)
					{
						function_251963(item);
					}
					else
					{
						function_233fd5(this, row, item);
					}
				}
			}
		}
	}
}

/* ---- the screen ---- */

/* a random value in [lower, upper) from the second seed */
__forceinline short postgame_random_range(short lower, short upper)
{
	dword *seed = &g_4e7408->seed;
	*seed = *seed * 0x19660d + 0x3c6ef35f;
	return lower + (short)(((upper - lower) * (*seed >> 16)) >> 16);
}

// @retail 0x2331a2
c_postgame_statistics_screen::c_postgame_statistics_screen(long screen_id, long a, long b, word user_flags) :
	c_class_1473c9(screen_id, a, b, user_flags),
	tab_bar(user_flags),
	tab1(screen_id, a, b, user_flags),
	tab2(screen_id, a, b, user_flags),
	tab3(screen_id, a, b, user_flags),
	tab4(screen_id, a, b, user_flags),
	tab5(screen_id, a, b, user_flags),
	tab6(screen_id, a, b, user_flags),
	value5ef0(false),
	start_time(0),
	value5ef8(postgame_random_range(0, 15000))
{
	value5ef0 = screen_id == 0xb9;
}

// @retail 0x2332bc destructor c_postgame_statistics_screen
// @retail 0x233289 deleting c_postgame_statistics_screen

// @retail 0x23334f
c_class_1473c9 *__stdcall function_23334f(s_screen_parameters *parameters)
{
	c_postgame_statistics_screen *screen = new c_postgame_statistics_screen(0x10, parameters->a, parameters->b, parameters->user_flags);

	if (screen)
	{
		screen->m6c = true;
		screen->function_147f6d(parameters);
	}
	return screen;
}

// @retail 0x233395
c_class_1473c9 *__stdcall function_233395(s_screen_parameters *parameters)
{
	c_postgame_statistics_screen *screen = new c_postgame_statistics_screen(0xb9, parameters->a, parameters->b, parameters->user_flags);

	if (screen)
	{
		screen->m6c = true;
		screen->function_147f6d(parameters);
	}
	return screen;
}

// @retail 0x2333dd
void c_postgame_statistics_screen::v17()
{
	c_class_1473c9 *tab;

	tab = &tab1;
	tab->v17();
	tab = &tab2;
	tab->v17();
	tab = &tab3;
	tab->v17();
	tab = &tab4;
	tab->v17();
	tab = &tab5;
	tab->v17();
	tab = &tab6;
	tab->v17();
}

/* the tabs and their lists; the first tab shows unless it has no rows */
// @retail 0x233423
void c_postgame_statistics_screen::v18(void *parameters)
{
	volatile long definition_index = function_1480ff(screen_id);
	s_screen_layout layout =
	{
		&tab_bar,
		6,
		{
			{ 0, 0, &tab1.list, 0 },
			{ 0, 0, &tab2.list, 0 },
			{ 0, 0, &tab3.list, 0 },
			{ 0, 0, &tab4.list, 0 },
			{ 0, 0, &tab5.list, 0 },
			{ 0, 0, &tab6.list, 0 }
		}
	};
	c_class_1473c9 *tab;

	tab_bar.add_child(&tab1);
	tab_bar.add_child(&tab2);
	tab_bar.add_child(&tab3);
	tab_bar.add_child(&tab4);
	tab_bar.add_child(&tab5);
	tab_bar.add_child(&tab6);
	build(&layout);
	c_class_1a2c81::v1();
	tab = &tab1;
	tab->v18(0);
	tab = &tab2;
	tab->v18(0);
	tab = &tab3;
	tab->v18(0);
	tab = &tab4;
	tab->v18(0);
	tab = &tab5;
	tab->v18(0);
	tab = &tab6;
	tab->v18(0);
	start_time = function_75870();
	if (tab1.parent)
	{
		if (g_51ec0c > 0)
		{
			tab = &tab1;
			tab->v7(&tab1.list);
			tab_bar.focused = tab;
		}
		else
		{
			tab = &tab2;
			tab->v7(&tab2.list);
			tab_bar.focused = tab;
			tab1.value6e = false;
		}
	}
	else
	{
		start_animation(3);
	}
}

/* the subtitle tells how to leave and whether a player's card can be shown */
// @retail 0x2335b7
void c_postgame_statistics_screen::v3()
{
	if (tab_bar.focused)
	{
		bool can_leave = !function_592f0() && !value5ef0;
		long index = 0;
		c_class_1a2c81 *text = 0;
		long string_ids[4] = { 0x1d000764, 0x16000763, 0xd000766, 0x6000765 };

		if (tab_is_current(&tab1))
		{
			index = can_leave ? 2 : 3;
			text = tab1.find_child(6, 4, false);
		}
		else if (tab_is_current(&tab2))
		{
			index = !can_leave;
			text = tab2.find_child(6, 6, false);
		}
		else if (tab_is_current(&tab3))
		{
			index = !can_leave;
			text = tab3.find_child(6, 6, false);
		}
		else if (tab_is_current(&tab4))
		{
			index = !can_leave;
			text = tab4.find_child(6, 4, false);
		}
		else if (tab_is_current(&tab5))
		{
			index = !can_leave;
			text = tab5.find_child(6, 4, false);
		}
		else if (tab_is_current(&tab6))
		{
			index = !can_leave;
			text = tab6.find_child(6, 6, false);
		}
		if (!function_6c7e0() || function_199994() || function_1999b3())
		{
			if (index < 2)
			{
				index += 2;
			}
		}
		subtitle.function_253b1a(string_ids[index]);
		if (text)
		{
			text->value6e = g_51ec08 <= 0;
		}
	}
	c_class_1a2c81::v3();
}

/* the dialog's choice leaves the game */
// @retail 0x233786
bool __stdcall function_233786(long controller_index)
{
	function_199e2e(false);
	return true;
}

/* B or back: the saved film's statistics close, a host's are left by the
   dialog */
// @retail 0x233792
bool c_postgame_statistics_screen::handle_back(s_widget_event *event)
{
	bool result = false;
	bool leaving = function_592f0();

	switch (event->param)
	{
	case 1:
	case 13:
		if (value5ef0)
		{
			start_animation(3);
		}
		else if (leaving)
		{
			function_19a942();
		}
		else
		{
			dialog_choice_show(3, 0x98, 4, 1 << event->controller_index, function_233786, 0, 0);
		}
		result = true;
		break;
	}
	return result;
}

// @retail 0x2336ef
bool c_postgame_statistics_screen::v10(s_widget_event *event)
{
	if (event->type == 5)
	{
		handle_back(event);
	}
	return c_class_1473c9::v10(event);
}

/* the screen fades in over the game's last view two seconds after it opens */
// @retail 0x23370e
void c_postgame_statistics_screen::v23(void *window)
{
	if (!value5ef0)
	{
		long elapsed = function_75890(start_time);
		dword color;

		if (elapsed <= 2000)
		{
			color = 0xff000000;
		}
		else
		{
			long fade = elapsed - 2000;
			real alpha;
			long value;

			if (fade < 0)
			{
				fade = 0;
			}
			else if (fade > 2000)
			{
				fade = 2000;
			}
			alpha = (1.0f - (real)fade * 0.0005f) * 255.0f;
			__asm
			{
				fld alpha
				fistp value
			}
			color = value << 24;
		}
		((c_render_window *)window)->function_147cdb(color);
	}
}
