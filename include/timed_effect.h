/* TIMED_EFFECT.H: the timed effect globals (g_5093e0; unknown_01fbb0.cpp,
   unknown_020560.cpp) */

#ifndef TIMED_EFFECT_H
#define TIMED_EFFECT_H

#include "unknown_11c920.h"
#include "unknown_0259d0.h"

/* a timed effect state: each timed value has a start time and an end time
   (in seconds against the global time g_4858a0) */
struct s_timed_effect_globals
{
	word unknown00;
	word unknown02;
	real unknown04;
	byte unknown08[8];
	real unknown10;
	real unknown14;
	point3f unknown18;
	byte unknown24[0x18];
	byte unknown3c;
	real unknown40;
	real unknown44;
	real unknown48;
	real unknown4c;
	real unknown50[4];
	real unknown60[4][3];
	real unknown90;
	real unknown94;
	real unknown98;
	real unknown9c;
	byte unknowna0;
	byte unknowna1[3];
	real unknowna4;
	real unknowna8;
	byte unknownac;
	byte unknownad[3];
	real unknownb0;
	real unknownb4;
	real unknownb8;
	real unknownbc;
	real unknownc0;
	real unknownc4;
	real unknownc8;
	real unknowncc;
	real unknownd0;
	real unknownd4;
	real unknownd8;
	real unknowndc;
	real unknowne0;
	real unknowne4;
	real unknowne8;
	real unknownec;
	real unknownf0;
	real unknownf4;
	real unknownf8;
	real unknownfc;
	real unknown100;
	real unknown104;
	real unknown108;
	real unknown10c;
	real unknown110;
	real unknown114;
	real unknown118;
	real unknown11c;
	real unknown120;
	real unknown124;
	real unknown128;
	real unknown12c[4][3];
	byte unknown15c[4];
	real unknown160;
	real unknown164;
	real unknown168;
	real unknown16c;
	byte unknown170;
	byte unknown171[3];
	real unknown174;
	real unknown178;
	real unknown17c;
	real unknown180[4];
	byte unknown190[0x1ac - 0x190];
	byte unknown1ac;
	byte unknown1ad[3];
	dword unknown1b0;
	byte unknown1b4[0x1f8 - 0x1b4];
	real values[32][2];	/* the timed values (020560) */
	real times[32][2];	/* their start and end times */
	real unknown3f8;
};

/* the per-frame parameter block function_12a50 copies in one piece (0x485898..0x4858b0) */
struct s_frame_parameters_12a50
{
    long mode;
    dword identifier;
    double time;
    dword unknown10, unknown14;
};

typedef char assert_frame_parameters_size[(sizeof(s_frame_parameters_12a50) == 0x18) ? 1 : -1];

extern s_frame_parameters_12a50 g_frame_parameters;
#define g_485898 (g_frame_parameters.mode)
#define g_48589c (g_frame_parameters.identifier)
#define g_4858a0 (g_frame_parameters.time)
#define g_4858a8 (g_frame_parameters.unknown10)
#define g_4858ac (g_frame_parameters.unknown14)

extern s_timed_effect_globals *g_5093e0;
extern long g_4ba04c;

#endif
