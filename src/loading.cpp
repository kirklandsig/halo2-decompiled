// @flags /O2 /arch:SSE /Gr
/* LOADING.CPP: the map the game wants next and the loading screen that waits
   for a map to be copied to the utility drive (0x163680..0x163b60; 0x163610,
   the multiplayer map named by the game options, is in unknown_163110.cpp).
   The map file states are unknown_213d20.cpp's. */

#include "unknown_11c920.h"
#include "globals.h"
#include "main_globals.h"
#include <xtl.h>

/* unknown_213d20.cpp */
long map_location_get(char const *map_name);
long cache_copy_current_priority(char const *map_name);
long cache_copy_queued_priority(char const *map_name);
long cache_copy_priority(char const *map_name);
bool map_copy_request(char const *map_name, long priority);
bool map_names_equal(char const *map_name, char const *other_map_name);
bool function_2141f0(void);
bool function_2148b0(real *progress);

extern char g_55bd21[0x103];
extern bool g_55bd0c;

enum
{
	_map_location_none = 0,
	_map_location_queued,
	_map_location_copying,
	_map_location_open,
	_map_location_copied
};

/* the map to load next (163680) */
struct s_map_load_request
{
	char const *map_name;
	long priority;
	bool copy_only;
};

/* the scenario, as read here */
struct s_loading_scenario_view
{
	byte unknown00[0x10];
	short type;
};

/* the network session (unknown_059ad0.h), as read here */
struct s_597d0_object
{
	byte unknown00[0x741c];
	long field_741c;
};

class c_class_58d20
{
public:
	bool get_values_4d08(long *a, long *b, byte **c);
};

bool function_597d0(s_597d0_object **out);
long function_1910d9(void);
char const *function_191117(void);
char *function_163610();
bool function_155f60(void);
void function_155f80(bool synchronous);
void function_156090(char const *name, dword flags);
bool attract_mode_movie_path(char *path, char const *name);
void function_124950(void);
void input_update_device_changes(void);
void main_time_wait_for_vblank(void);
class c_class_93590;
void network_link_receive(c_class_93590 *link);
void function_8df50(void);
void function_8dfc0(void);
void function_2232a0(void);
void function_13f10(long a, long b);
void function_12b6f0(real progress);

/* the maps the game keeps on the utility drive */
char const *g_4687fc = "scenarios\\shared\\shared\\shared";
char const *g_468800 = "scenarios\\shared\\shared\\single_player_shared";
char const *g_468804 = "scenarios\\ui\\mainmenu\\mainmenu";
char const *g_468808 = "scenarios\\solo\\00a_introduction\\00a_introduction";

char const *g_4e9bb8;

struct s_input_globals
{
	bool initialized;
};

extern s_input_globals g_4e61b8;
extern bool g_4d8ba0;
extern c_class_93590 *g_510560;
extern dword g_51ebc8;

// @retail 0x163680
bool map_load_request_get(s_map_load_request *request)
{
	s_loading_scenario_view *scenario = (s_loading_scenario_view *)g_4e0350;
	bool local_b608f1 = function_1910d9() == 1;
	bool has_scenario = scenario != NULL;
	bool multiplayer = false;
	bool campaign = false;
	long location = map_location_get(g_468808);
	s_597d0_object *session = NULL;
	s_map_load_request result;

	if (function_597d0(&session) && session->field_741c > 2 && session->field_741c <= 8)
	{
		long a;
		long b;
		byte *c;

		if (((c_class_58d20 *)session)->get_values_4d08(&a, &b, &c))
		{
			local_b608f1 = false;
		}
	}
	if (has_scenario)
	{
		short type = scenario->type;

		multiplayer = type == 2;
		campaign = type == 0;
	}

	result.copy_only = true;
	result.map_name = NULL;
	result.priority = 0;
	if (map_location_get(g_468804) == _map_location_none)
	{
		result.map_name = g_468804;
		result.priority = 6;
	}
	else if (map_location_get(g_4687fc) == _map_location_none)
	{
		result.map_name = g_4687fc;
		result.priority = 5;
	}
	else
	{
		if ((multiplayer || !has_scenario) && local_b608f1 &&
			(location == _map_location_none || location == _map_location_queued))
		{
			result.map_name = g_468808;
			result.priority = 4;
		}
		else if (multiplayer && !local_b608f1 && cache_copy_priority(g_468808) == 4)
		{
			result.map_name = g_468808;
			result.copy_only = local_b608f1;
			result.priority = 0;
		}
		else if (map_location_get(g_468800) == _map_location_none)
		{
			result.map_name = g_468800;
			result.priority = 3;
		}
		else
		{
			char const *name = NULL;

			if (campaign)
			{
				name = function_163610();
			}
			else if (multiplayer && !function_155f60())
			{
				name = function_191117();
			}
			if (name)
			{
				result.map_name = name;
				result.priority = 0;
			}
		}
	}

	if (request)
	{
		*request = result;
	}

	return result.map_name != NULL;
}

/* retail calls this out of line from both of its callers (0x12b450, 0x163890) */
__declspec(noinline) void map_load_request_update(void);

// @retail 0x163820
void map_load_request_update(void)
{
	s_map_load_request request;

	if (map_load_request_get(&request))
	{
		if (request.copy_only)
		{
			if (map_location_get(request.map_name) == _map_location_none)
			{
				map_copy_request(request.map_name, request.priority);
			}
		}
		else
		{
			map_copy_request(request.map_name, request.priority);
		}
	}
}

// @retail 0x163870
bool function_163870(void)
{
	bool result = true;

	if (function_2148b0(NULL))
	{
		result = false;
	}
	return result;
}

static inline bool loading_bink_playing(void)
{
	return g_4e9188.movie && g_4e9188.initialized;
}

/* keeps the network and the screen alive while the loading screen waits */
static __forceinline void loading_screen_idle(void)
{
	if (g_4d8ba0)
	{
		g_510548 = 1;
		g_51054c = GetTickCount();
		network_link_receive(g_510560);
		g_510548 = 0;
		g_51054c = GetTickCount();
	}
	function_8df50();
	function_8dfc0();
	function_2232a0();
	main_time_wait_for_vblank();
	function_13f10(0, 0);
}

/* cache_copy_priority (unknown_213d20.cpp), which retail inlines here but
   calls from map_load_request_get */
static inline long cache_copy_priority_inlined(char const *map_name)
{
	long result = cache_copy_current_priority(map_name);

	if (result == NONE)
	{
		result = cache_copy_queued_priority(map_name);
	}
	return result;
}

// @retail 0x163890
bool __stdcall function_163890(char const *map_name, long mode)
{
	volatile bool result = true;
	long location = map_location_get(map_name);

	if (location != _map_location_open && location != _map_location_copied)
	{
		bool wait = false;
		long priority;

		switch (mode)
		{
		case 0:
			priority = 0;
			break;
		case 1:
			priority = 1;
			break;
		default:
			priority = 2;
			wait = true;
			break;
		}
		if (cache_copy_priority_inlined(map_name) < priority)
		{
			map_copy_request(map_name, priority);
		}
		if (wait)
		{
			g_4e9bb8 = map_name;
			if (main_globals.unknown28)
			{
				char path[256];

				if (g_4e61b8.initialized)
				{
					input_update_device_changes();
				}
				path[0] = 0;
				if (attract_mode_movie_path(path, "intro"))
				{
					function_156090(path, 0x3c6);
				}
				main_globals.unknown28 = false;
			}
			do
			{
				real progress = 0.0f;
				long current = map_location_get(map_name);

				if (current == _map_location_open)
				{
					progress = 1.0f;
				}
				else if (current == _map_location_copying)
				{
					if (map_names_equal(g_55bd21, map_name))
					{
						function_2148b0(&progress);
					}
				}
				else
				{
					progress = 0.0f;
				}
				map_load_request_update();
				if (map_location_get(map_name) == _map_location_none)
				{
					map_copy_request(map_name, priority);
				}
				if (g_55bd0c)
				{
					function_2141f0();
				}
				SwitchToThread();
				if (loading_bink_playing())
				{
					loading_screen_idle();
				}
				else
				{
					function_12b6f0(progress);
				}
				location = map_location_get(map_name);
			}
			while (location == _map_location_copying || location == _map_location_queued || location == _map_location_none);

			g_51ebc8 = GetTickCount();
			function_124950();
			g_4e9188.unknown03 = 1;
			while (loading_bink_playing())
			{
				function_124950();
				function_155f80(false);
				loading_screen_idle();
			}
			g_4e9bb8 = NULL;
			location = map_location_get(map_name);
		}
	}
	if (mode == 2 && location == _map_location_copied)
	{
		result = false;
	}

	return result;
}

// @retail 0x163b20
long map_location_progress_get(char const *map_name)
{
	long result;

	switch (map_location_get(map_name))
	{
	case _map_location_none:
		result = 0;
		break;
	case _map_location_queued:
		result = 1;
		break;
	case _map_location_copying:
		result = 1;
		break;
	case _map_location_open:
		result = 2;
		break;
	case _map_location_copied:
		result = 3;
		break;
	default:
		__assume(0);
	}
	return result;
}

// @retail 0x163b60
bool function_163b60(void)
{
	bool result = false;
	char const *map_name = function_163610();

	if (map_name)
	{
		switch (map_location_get(map_name))
		{
		case _map_location_open:
			result = false;
			break;
		case _map_location_copied:
			result = false;
			break;
		case _map_location_none:
			result = true;
			break;
		case _map_location_queued:
			result = true;
			break;
		case _map_location_copying:
			result = true;
			break;
		default:
			__assume(0);
		}
	}
	return result;
}
