// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_065340.CPP: publishing the current session's map status (built with
   SSE, apart from its neighbours in unknown_054fe0.cpp) */

#include "unknown_4cf790.h"
#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include <string.h>
#include "globals.h"
#include "unknown_059ad0.h"
#include "unknown_058ee0.h"

/* one local user's state (0xd0 bytes) */
#pragma pack(push, 1)
struct s_session_interface_user
{
	bool valid;
	XUID xuid;
	byte unknown0d[3];
	long unknown10;
	byte properties[0x90];
	long unknowna4;
	long unknowna8[3];
	long unknownb4[3];
	long unknownc0[3];
	byte unknowncc[4];
};
#pragma pack(pop)

struct s_session_interface_globals
{
	bool initialized;
	byte unknown01;
	wchar_t machine_name[16];
	wchar_t session_name[32];
	bool unknown62;
	byte unknown63;
	union
	{
		byte unknown64[32];
		struct
		{
			byte unknown64_00[0x14];
			long unknown78;
		};
	};
	long unknown84;
	long unknown88;
	long unknown8c;
	long unknown90;
	long unknown94;
	long unknown98;
	s_session_interface_user users[4];
	long update3dc[3];
	long unknown3e8[3];
	long value3f4[3];
	byte data400[3][0x130];
	byte unknown790[0x7d4 - 0x790];
	void *session_manager;
};

extern s_session_interface_globals g_4cd868;

#define SESSION_STATE_IS_LIVE(state) ((state) > 2 && (state) <= 8)

void __stdcall function_65530(long user_index);
bool game_loop_running(void);
long map_location_progress_get(char const *map_name);
real __stdcall function_122dd0(byte *map_name, long mode, long type);
bool __stdcall function_163890(char const *map_name, long mode);

static __forceinline bool function_65341(c_class_58d20 **out)
{
 long state = 0;
 if (g_527330.initialized)
  state = g_527330.state;
 switch (state)
 {
 case 1: case 2: case 3: case 4:
  {
   bool result = false;
   if (g_527330.initialized)
   {
    c_class_58d20 *current = (c_class_58d20 *)g_527330.session_a;
    if (current->state) { *out = current; result = true; }
   }
   return result;
  }
 case 6: case 7: case 8: case 9:
  {
   bool result = false;
   if (g_527330.initialized)
   {
    c_class_58d20 *current = (c_class_58d20 *)g_527330.session_b;
    if (current->state) { *out = current; result = true; }
   }
   return result;
  }
 }
 return false;
}

/* publishes the current session's map status, load progress and variant to the
   session interface globals, then refreshes each valid local user */
// @retail 0x65340
void function_065340(void)
{
	c_class_58d20 *session = 0;
	long status = 0;
	real progress = 0.0f;
	long variant_index = NONE;
	bool found = function_65341(&session);
	if (found && session && SESSION_STATE_IS_LIVE(session->state))
	{
		long first_id;
		long second_id;
		byte *name;
		if (session->get_values_4d08(&first_id, &second_id, &name) &&
			second_id != NONE && name[0])
		{
			if (game_loop_running() && g_4e6948->position_a == first_id &&
				g_4e6948->position_b == second_id &&
				!strncmp((char const *)name, g_4e6948->name, 0x104))
			{
				status = 4;
				progress = 1.0f;
			}
			else
			{
				long location = map_location_progress_get((char const *)name);
				progress = function_122dd0(name, 0, first_id == NONE);
				switch (location)
				{
				case 1: status = 2; break;
				case 2: status = 3; break;
				case 3: status = 1; break;
				default:
					function_163890((char const *)name, 1);
					break;
				}
			}
		}
		long session_variant = NONE;
		if (SESSION_STATE_IS_LIVE(session->state))
			session_variant = session->value49c8;
		variant_index = session_variant;
	}
	g_4cd868.unknown90 = status;
	g_4cd868.unknown94 = (long)(progress * 100.0f);
	g_4cd868.unknown98 = variant_index;
	for (long i = 0; i < 4; i++)
	{
		if (g_4cd868.users[i].valid)
			function_65530(i);
	}
}

