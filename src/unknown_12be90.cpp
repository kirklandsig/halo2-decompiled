// @flags /O2 /Gr
/* UNKNOWN_12BE90.CPP: the main loop's checks of whether something is busy
   (loading, saving, a menu or a movie up) and its reset of the game time
   when the game loses focus */

#include "unknown_11c920.h"
#include "main_globals.h"
#include "unknown_058ee0.h"
#include "async.h"
#include "globals.h"
#include <xtl.h>
#include "main_messages.h"

/* unknown_12de70.cpp: the geometry cache's pending loads */
extern long g_4e64a0;

s_main_globals main_globals;

bool g_4ed39d;
bool g_4ed39e;
dword g_4ed3a0;
long g_4e6470;

long g_55bd04;
long g_55bd08;
bool g_55c14c;

void function_593e0(void);

// @retail 0x12be90
bool function_12be90(void)
{
	bool result = false;

	if (g_4ed39d)
	{
		result = true;
	}
	if (main_globals.switch_structure_bsp || main_globals.reset_map || main_globals.unknown6f || main_globals.unknown72 || main_globals.save_map || main_globals.quit_game || main_globals.unknown76 || g_4e6470 > 0 || g_4e64a0 > 0)
	{
		result = true;
	}
	return result;
}

/* the game's state flags (unknown_13bf00.cpp), as these functions read them */
struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;

struct s_510c50_view
{
	byte unknown00[6];
	bool revert_requested;
	byte unknown07[0x22 - 7];
	bool revert_checked;
};

bool function_163b60(void);
void function_18e700(void);

// @retail 0x12ba90
void function_12ba90(void)
{
	if (g_4e6948 && g_4e6948->flag1120 && (!g_510c54->active || !g_510c54->unknown01))
	{
		function_18e700();
		main_globals.reset_map = false;
	}
}

// @retail 0x12bad0
void function_12bad0(void)
{
	s_510c50_view *state = (s_510c50_view *)g_510c50;
	bool revert = state->revert_requested;

	if (revert && state->revert_checked)
	{
		revert = !function_163b60();
	}
	main_globals.unknown72 = false;
	if (revert)
	{
		main_globals.unknown75 = false;
		main_globals.unknown6f = true;
		main_globals.unknown74 = true;
	}
}

// @retail 0x12bf00
void function_12bf00(void)
{
	main_globals.unknown29 = true;
	g_4ed39e = true;
	g_4ed39d = true;
	g_4ed3a0 = GetTickCount();
	if (g_527330.initialized && (g_527330.state == 3 || g_527330.state == 8))
	{
		function_593e0();
	}
}

// @retail 0x12bf40
void function_12bf40(void)
{
	if (g_55bd04 && g_55bd04 < 0x11)
	{
		g_55bd08 = 2;
		g_55bd04 = 0x11;
	}
	function_120d50(&g_55c14c, true);
}
/* ---- the main loop's messages to the local players (loading, saving,
   switching structure bsps) ---- */

bool function_14ddc0(long local_player_index);
void function_123e20(void);
long function_1896c0(real scale, long tag_index);
void function_190e37(void);

/* the hud globals' sound played when the game is saved */
struct s_hud_globals_save_view
{
	byte unknown00[0x394];
	long save_sound;
};

/* the scenario's structure bsps */
struct s_scenario_structure_bsps_view
{
	byte unknown00[0x210];
	long structure_bsp_count;
};

// @retail 0x12b790
void function_12b790(void)
{
	main_globals.save_map = true;
	if (g_4e6948->state == 1)
	{
		function_24cdaf();
		if (((s_hud_globals_save_view *)g_510c94)->save_sound != NONE)
			function_1896c0(1.0f, ((s_hud_globals_save_view *)g_510c94)->save_sound);
		for (long i = 0; i < 4; i++)
		{
			if (function_14ddc0(i))
				main_print_message(i, 0x120006a0);
		}
	}
}

// @retail 0x12b850
void function_12b850(short field_0_3)
{
	if (field_0_3 >= 0 && field_0_3 < ((s_scenario_structure_bsps_view *)g_4e0350)->structure_bsp_count)
	{
		if (field_0_3 == g_4686c4)
		{
			if (main_globals.switch_structure_bsp)
			{
				main_globals.field_0_3 = field_0_3;
				main_globals.switch_structure_bsp = false;
				if (g_4e6948->state == 1)
				{
					function_24cdaf();
					main_print_message(local_player_first_index(), 0xf0006a3);
				}
			}
		}
		else
		{
			main_globals.field_0_3 = field_0_3;
			main_globals.switch_structure_bsp = true;
			if (g_4e6948->state == 1)
			{
				function_24cdaf();
				main_print_message(local_player_first_index(), 0x110006a2);
			}
		}
	}
}

// @retail 0x12b980
void function_12b980(void)
{
	if (!main_globals.unknown76)
	{
		main_globals.unknown76 = true;
		if (g_4e6948 && g_4e6948->flag1120 && g_4e6948->state == 1)
		{
			function_24cdaf();
			main_print_message(local_player_first_index(), 0x100006a4);
		}
	}
}

// @retail 0x12bb20
void function_12bb20(void)
{
	if (g_4e6948 && g_4e6948->flag1120)
		function_123e20();
	if (g_4e6948->state == 1)
	{
		function_24cdaf();
		for (long i = 0; i < 4; i++)
		{
			if (function_14ddc0(i))
				main_print_message(i, 0x100006a1);
		}
	}
	main_globals.save_map = false;
}

// @retail 0x12bdf0
void function_12bdf0(void)
{
	function_190e37();
	main_globals.unknown76 = false;
	if (g_4e6948 && g_4e6948->flag1120 && g_4e6948->state == 1)
	{
		function_24cdaf();
		main_print_message(local_player_first_index(), 0xe0006a5);
	}
}

int function_11c920(char const *s1, char const *s2);
bool attract_mode_movie_path(char *path, char const *name);
void function_156090(char const *name, dword flags);
extern dword g_51ebc8;

/* the name of the map being loaded (0x509344) */
char g_509344[256];

static inline bool main_bink_playback_active(void)
{
	return g_4e9188.movie && g_4e9188.initialized;
}

/* plays the intro movie when the main loop asked for it (except in the demo
   map); true while it plays */
// @retail 0x12bf90
bool main_play_intro_movie(void)
{
	bool result = false;
	char path[256];

	if (main_globals.unknown28)
	{
		if (function_11c920(g_509344, "xdemo") != 0)
		{
			if (!main_bink_playback_active() && main_globals.unknown28)
			{
				path[0] = 0;
				if (attract_mode_movie_path(path, "intro"))
					function_156090(path, 0x1c6);
				main_globals.unknown28 = 0;
			}
			result = main_bink_playback_active();
		}
		g_51ebc8 = GetTickCount();
		main_globals.unknown28 = 0;
	}
	return result;
}
