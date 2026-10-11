#include "unknown_11c920.h"
#include "globals.h"

// @flags /O2 /Gr

/* Two state queries of the multiplayer globals (g_4e9ae8). Decompiled by
   lane O because the engine at 0x459d18 (unknown_2420a0.cpp) inlines them. */

struct s_engine_state_globals
{
	byte unknown00[0x6c];
	short w6c;
	byte unknown6e[0xc04 - 0x6e];
	long lc04;
	byte unknownc08[0xc14 - 0xc08];
	long engine_index;
};

/* true while the engine runs the game (always on a client) */
// @retail 0x15b2f0
bool function_15b2f0()
{
	s_engine_state_globals *g = (s_engine_state_globals *)g_4e9ae8;
	bool mode_gate_open = false;

	if (g_55e4d0[g->engine_index] && g->w6c == 1 && (g_4e6948->mode == 4 || g->lc04 == 1))
		mode_gate_open = true;

	return mode_gate_open;
}

/* true when the game has teams */
// @retail 0x15eaf0
bool function_15eaf0()
{
	bool result = false;

	if (g_55e4d0[g_4e9ae8->engine_index])
		result = g_4e6948->flags184.bit0;

	return result;
}

struct s_score_display
{
    long players[16];
    short teams[8];
    char player_ranks[16];
    char team_ranks[8];
    short player_count;
    short team_count;
};

void function_23f3e0(s_score_display *display, long mode, bool fallback);

PRIVATE __forceinline byte sweep_score_teams()
{
    byte result = 0;
    if (g_55e4d0[g_4e9ae8->engine_index])
    {
        result = ((byte *)g_4e6948)[0x184] & 1;
        volatile byte observed_teams = result;
    }
    return result;
}

// @retail 0x15b330
long function_15b330(bool teams)
{
    s_score_display display;
    function_23f3e0(&display, 0, false);
    if (sweep_score_teams())
    {
        if (display.team_ranks[0] == 0)
            return display.teams[0];
    }
    else if (display.player_ranks[0] == 0)
        return display.players[0];
    return NONE;
}
