// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_0574E0.CPP: the voice observer (lane D): the radio sounds played
   when a player starts or stops talking, and the players' talking times */

#include "unknown_11c920.h"
#include "globals.h"
#include <xtl.h>
#include <xonline.h>
#include <xhv.h>
#include "network_voice.h"
#include "unknown_0662e0.h"

/* a sound to play by label (unknown_189010.cpp) */
struct s_sound_label_play
{
	long label;
	long tag_index;
	real scale;
	char const *variant;
};

long function_189760(s_sound_label_play const *play);
long function_1896c0(real scale, long tag_index);

/* network_voice.cpp: the controller of a session player, or NONE */
long voice_get_player_unknown14(long index);

/* unknown_190001.cpp */
bool function_1906da(long index);

/* the radio effects of the multiplayer globals (8 bytes each) */
struct s_voice_radio_effect
{
	long unknown0;
	long sound_index;
};

struct s_voice_radio_effects
{
	byte unknown00[0x90];
	long count;
	s_voice_radio_effect *effects;
};

class c_voice_observer
{
public:
	void play_sound(long player_index, long tag_index);
	void play_radio_effect(long player_index, long radio_effect_index);

	byte unknown000[0x194];
	long talk_times[16];
};

static inline bool voice_available(void)
{
	return g_4c9878.initialized && g_476fc8.initialized;
}

// @retail 0x57530
void c_voice_observer::play_sound(long player_index, long tag_index)
{
	long controller_index = voice_get_player_unknown14(player_index);

	if (controller_index != NONE)
	{
		long state = 0;
		if (voice_available())
			state = g_4c9878.port_states[controller_index];
		long mask = 0;
		if (voice_available())
			mask = g_4c9878.unknownEE;
		if (state != 3 && function_1906da(controller_index))
		{
			if (state != 1 && (mask & (1 << player_index)))
			{
				long label;
				long index = voice_get_player_unknown14(player_index);
				if (index == NONE)
					return;
				switch (index)
				{
				case 0:
					label = 0x14000140;
					break;
				case 1:
					label = 0x14000141;
					break;
				case 2:
					label = 0x14000142;
					break;
				case 3:
					label = 0x14000143;
					break;
				default:
					__assume(0);
				}
				s_sound_label_play play;
				play.tag_index = tag_index;
				play.scale = 1.0f;
				play.variant = NULL;
				play.label = label;
				function_189760(&play);
			}
			else
			{
				function_1896c0(1.0f, tag_index);
			}
		}
	}
}

// @retail 0x574e0
void c_voice_observer::play_radio_effect(long player_index, long radio_effect_index)
{
	s_voice_radio_effects *effects = *(s_voice_radio_effects **)(g_4e3b44[g_4e034c->index & 0xffff].bytes + 0xc);

	if (radio_effect_index < effects->count)
	{
		long sound_index = effects->effects[radio_effect_index].sound_index;
		if (sound_index != NONE)
			play_sound(player_index, sound_index);
	}
}

/* whether the player talked within the configured time */
// @retail 0x57790
bool voice_observer_talked_recently(c_voice_observer *observer, long player_index)
{
	long time = observer->talk_times[player_index];

	if (time)
	{
		s_game_time_globals *game_time = g_510c54;
		real seconds = (real)(dword)(game_time->game_time - time) * game_time->rate;
		if (g_network_configuration.real16f8 > seconds)
			return true;
	}
	return false;
}
