// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"

struct s_timer_player
{
    byte unknown00[0x2c];
    long unit_index;
    byte unknown30[0x198 - 0x30];
    short effect_timers[2];
    byte unknown19c[0x21c - 0x19c];
};

void function_d0e00(long unit_index, real rate);

static inline s_timer_player *timer_player_get(long player_index)
{
	return (s_timer_player *)(g_4e8c24->data + (player_index & 0xffff) * sizeof(s_timer_player));
}

// @retail 0x1521d0
void function_1521d0(long player_index)
{
	s_timer_player *player = timer_player_get(player_index);
	long effect = 0;

	for (long timer_index = 0; timer_index < 2; timer_index++)
	{
		if (player->effect_timers[timer_index] > 0)
		{
			player->effect_timers[timer_index]--;
			if (player->effect_timers[timer_index] == 0 && effect == 0)
			{
				function_d0e00(timer_player_get(player_index)->unit_index, 1.0f);
			}
		}
		effect++;
	}
}
