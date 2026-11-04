#ifndef UNKNOWN_07F720_H
#define UNKNOWN_07F720_H
/* UNKNOWN_07F720.H: a player's colours (src/unknown_07f720.cpp) */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

/* the appearance of a player (at +0x84 of a player): four colour indices
   first */
struct s_player_appearance
{
	char colors[4];
	byte unknown04[0x10 - 0x04];
};

color3f function_7f720(short team_index);
void function_7f790(short team_index, bool use_default, s_player_appearance const *appearance, color3f *colors);

#endif
