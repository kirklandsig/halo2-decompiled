/* UNKNOWN_13927E.H: the 0x90 byte marker list the multiplayer engines build for
   a player or a point (162550, 243ed0, 2440a0) and pass to 24e59f */

#ifndef UNKNOWN_13927E_H
#define UNKNOWN_13927E_H

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

/* a color copied as three dwords */
struct s_color_bits
{
	dword v[3];
};

struct s_list_item
{
	long kind;
	s_color_bits a;
	s_color_bits b;
	real r;
	long index;
};

struct s_marker_list
{
	byte b0;
	byte b1;
	byte unknown02[2];
	long l4;
	point3f position;
	real r14;
	real r18;
	real r1c;
	long l20;
	s_color_bits color24;
	s_color_bits color30;
	real r3c;
	real r40;
	long count;
	s_list_item items[2];
};

/* the two marker colors, alpha then red, green and blue (unknown_2420a0.cpp) */
extern color4f g_468c80[2];

/* the color of a player's markers (unknown_13927e.cpp) */
s_color_bits *function_13927e(long player_index);

/* the marker list over a player (unknown_162550.cpp) */
bool function_162550(long player_index, s_marker_list *list);

/* not decompiled yet (src/stubs/unknown_1523c0.cpp) */
void __stdcall function_24e59f(s_marker_list *list);

#endif
