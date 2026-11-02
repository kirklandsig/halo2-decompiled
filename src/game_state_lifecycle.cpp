// @flags /O2 /arch:SSE /Gr
/* GAME_STATE_LIFECYCLE.CPP: the game state's new-map and new-structure-bsp
   callbacks (its entry in the subsystem table at 0x158530), which fill in the
   header at the start of the game state, and the count of reverts made soon
   after a save, which falls back to the older of the two saves. */

#include "unknown_11c920.h"
#include "unknown_123b30.h"
#include "globals.h"
#include <xtl.h>
#include <string.h>
#include <stddef.h>

#define ARENA (game_state_globals.arena)
/* the map being played: a value the game state header keeps (and compares
   when it reads a saved core back), and the map's path */
struct s_current_map
{
	dword unknown00; /* copied to the header's unknown128 */
	byte unknown04[0x74];
	char map_name[0x100];
};
typedef char assert_current_map_size[(sizeof(s_current_map) == 0x178) ? 1 : -1];
typedef char assert_current_map_map_name[(offsetof(s_current_map, map_name) == 0x78) ? 1 : -1];

/* retail's sits inside the loaded map's cache header (cache_file_globals,
   which cache_files_load_map fills), so it keeps its address name: its own
   storage would stay empty, every saved header would have no map name, and
   function_124520 would refuse every revert */
s_current_map g_547844;
/* the build's version string, kept in the game state header */
char g_450698[0x20];

PRIVATE void csstrncpy(char *destination, char const *source, long size)
{
	strncpy(destination, source, size);
	destination[size - 1] = 0;
}

/* makes the game state's memory writable, its last 0x40000 bytes
   write-combined */
static __forceinline void game_state_protect_memory(byte *base_address)
{
	XPhysicalProtect(base_address, 0x3be000, PAGE_READWRITE);
	XPhysicalProtect(base_address + 0x3be000, 0x40000, PAGE_READWRITE | PAGE_WRITECOMBINE);
}

/* clears the game state's saves and fills in its header for the new map */
// @retail 0x123c20
void game_state_map_begin(void)
{
	game_state_protect_memory(game_state_globals.base_address);

	game_state_globals.flag1 = true;
	game_state_globals.flag2 = false;
	game_state_globals.flag3 = false;
	memset(game_state_globals.saved_headers, 0, sizeof(game_state_globals.saved_headers));

	game_state_globals.game_time = NONE;
	memset(ARENA, 0, 0x1288);
	ARENA->checksum = game_state_globals.allocation_size_checksum;
	ARENA->base_address = (long)game_state_globals.base_address;
	csstrncpy(ARENA->map_name, g_547844.map_name, sizeof(ARENA->map_name));
	csstrncpy(ARENA->version, g_450698, sizeof(ARENA->version));
	ARENA->unknown128 = g_547844.unknown00;
	/* the session options at +8 of the game options */
	memcpy(ARENA->field_130_2, (byte *)g_4e6948 + 8, sizeof(ARENA->field_130_2));
	game_state_globals.arena_flag = false;
}

/* keeps g_4686c4 in the game state header for the new structure bsp */
// @retail 0x123d20
void game_state_bsp_begin(void)
{
	ARENA->unknown1248 = g_4686c4;
}

/* called as a revert starts: counts reverts made within 8 seconds of the
   last save while an older save exists; at the fifth it falls back to the
   older save */
// @retail 0x123ed0
void game_state_rollback_done(void)
{
	if (game_state_globals.flag2 && game_state_globals.flag3)
	{
		dword elapsed = g_510c54->game_time - game_state_globals.time;

		if ((real)elapsed * g_510c54->rate < 8.0f)
		{
			if (++game_state_globals.counter >= 5)
			{
				game_state_globals.slot = (game_state_globals.slot + 1) % 2;
				game_state_globals.flag2 = true;
				game_state_globals.flag3 = false;
			}
			return;
		}
	}
	game_state_globals.counter = 0;
}