/* UNKNOWN_033A0B.H: the view whose vectors 0x33980 copies out and 0x336f0 reads as two colours */

#ifndef UNKNOWN_033A0B_H
#define UNKNOWN_033A0B_H

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

struct s_33a0b_view
{
	byte unknown00[0x10];
	vector3f vector10;
	byte unknown1c[0x14];
	vector3f vector30;
	vector3f vector3c;
	vector3f vector48;
	vector3f vector54;
};

extern s_33a0b_view *g_485a58;

#endif
