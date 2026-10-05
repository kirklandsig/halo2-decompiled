#ifndef UNKNOWN_1FB360_H
#define UNKNOWN_1FB360_H

#include "unknown_11c920.h"

/* one recorded animation playing on an object (0xa0 bytes;
   src/unknown_1fb350.cpp) */
struct s_recorded_animation
{
	short salt;
	byte unknown02[2];
	long object_index;
	word ticks;
	word flags;
	long remaining_ticks;
	byte const *cursor;
	byte control[0x90 - 0x14];
	byte controller[0x9c - 0x90];
	short version;
	byte unknown9e[2];
};

s_recorded_animation *recorded_animation_find(long object_index, long *datum_index);
long recorded_animation_get_frames(long object_index);
/* plays a cutscene recording on a unit (cutscene_play: flags 0;
   cutscene_play_and_free: 8; cutscene_play_and_float: 0x10) */
bool function_1fb360(long unit_index, short recording_index, long flags);

#endif
