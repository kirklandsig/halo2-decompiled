// @flags /O2 /Ob1 /Gr /arch:SSE
#include "unknown_11c920.h"
#include <string.h>
#include "crc.h"
#include "unknown_123b30.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include "timed_effect.h"

s_timed_effect_globals *g_5093e0;
double g_4858a0;
dword g_4b5690;
byte g_4b569d;
long g_4ba04c;
point3f *g_468718;
point3f *g_468720;

// @retail 0x1fbb0
void function_01fbb0(void)
{
	long size = sizeof(s_timed_effect_globals);
	byte *result = game_state_globals.base_address + game_state_globals.cpu_allocation_size;
	s_timed_effect_globals *globals;

	game_state_globals.cpu_allocation_size += size;
	function_163ba0(&game_state_globals.allocation_size_checksum, &size, sizeof(size));

	globals = (s_timed_effect_globals *)result;
	g_5093e0 = globals;
	if (globals)
	{
		memset(globals, 0, sizeof(s_timed_effect_globals));
		globals->unknown180[0] = 1.0f;
		globals->unknown180[1] = 1.0f;
		globals->unknown180[2] = 1.0f;
		globals->unknown180[3] = 1.0f;
		globals->unknown3f8 = 1.0f;
	}
}

// @retail 0x1fc30
void function_01fc30(real a, real b, real c, real d, real e, real f, real g)
{
	s_timed_effect_globals *globals = g_5093e0;

	if (globals)
	{
		globals->unknownd8 = (a < 0.0f) ? 0.0f : a;
		if (d >= 0.0f)
		{
			real time = (real)g_4858a0;

			globals->unknowne4 = time;
			globals->unknowndc = b;
			globals->unknowne0 = c;
			globals->unknowne8 = time + d;
		}
		if (g >= 0.0f)
		{
			real time = (real)g_4858a0;

			globals->unknownf4 = time;
			globals->unknownec = e;
			globals->unknownf0 = f;
			globals->unknownf8 = time + g;
		}
	}
}

// @retail 0x1fcd0
void function_01fcd0(void)
{
	if (g_5093e0)
	{
		memset(g_5093e0, 0, sizeof(s_timed_effect_globals));
		if (g_4b5690)
		{
			if (!g_4b569d)
				g_4b569d = 1;
		}
	}
}

// @retail 0x1fd00
void function_01fd00(void)
{
	if (g_5093e0)
		g_5093e0->unknowna0 = 0;
}

// @retail 0x204b0
void function_0204b0(long tag_index)
{
	if (g_5093e0 && tag_index != NONE)
	{
		byte *tag = (byte *)g_4e3b44[tag_index & 0xffff].data;

		if (*(short *)tag == 2 && *(long *)(tag + 0x44) > 0)
		{
			g_5093e0->unknown1ac = 1;
			g_5093e0->unknown1b0 = tag_index;
		}
	}
}

#ifndef PIN
#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))
#endif

PRIVATE inline void timed_effect_interpolate(real a, real b, real t, real *result)
{
    *result = (1.0f - t) * a + b * t;
}

PRIVATE inline void timed_effect_interpolate_clamped(real a, real b, real t, real *result)
{
    *result = PIN((1.0f - t) * a + t * b, 0.0f, 1.0f);
}

PRIVATE inline void timed_effect_point_fallback(point3f *point)
{
    if (memcmp(point, g_468718, sizeof(point3f)) == 0)
        *point = *g_468720;
}

// Matches retail: destination references keep each store before the next input loads.
// MiniMax E (11-16): matches retail byte for byte (score 0, was 19): the eight interpolation
// destinations each get their own named pointer local, and this build then stores each
// interpolated value before it loads the next pair of inputs, as retail does.
// score 19 (was 454): same size and instructions; retail stores each interpolated value
// before it loads the next pair of inputs, where ours loads one input ahead of the store.
// MiniMax E (11-16), NOT kept: the eight interpolation outputs through one-use pointer locals (result04 ...
// result50_3) measured the same score; one-use pointers are kept only with a match.
// @retail 0x1fd20
s_timed_effect_globals *function_01fd20(s_timed_effect_globals *result)
{
    s_timed_effect_globals *globals = g_5093e0;

    if (globals && globals->unknownac)
    {
        real fraction0 = globals->unknownbc != globals->unknownb8
            ? (real)PIN((g_4858a0 - globals->unknownb8) / (globals->unknownbc - globals->unknownb8), 0.0, 1.0)
            : 1.0f;
        real fraction1 = globals->unknownd4 != globals->unknownd0
            ? (real)PIN((g_4858a0 - globals->unknownd0) / (globals->unknownd4 - globals->unknownd0), 0.0, 1.0)
            : 1.0f;

        real &output_04 = globals->unknown04;
        timed_effect_interpolate(globals->unknownb0, globals->unknownb4, fraction0, &output_04);
        real &output_10 = globals->unknown10;
        timed_effect_interpolate_clamped(globals->unknownc0, globals->unknownc4, fraction1, &output_10);
        real &output_14 = globals->unknown14;
        timed_effect_interpolate_clamped(globals->unknownc8, globals->unknowncc, fraction1, &output_14);

        timed_effect_point_fallback(&globals->unknown18);

        bool faded = globals->unknown04 <= 0.0001f;
        if (g_4ba04c > 1 || faded)
        {
            globals->unknown04 = 0.0f;
            globals->unknown02 = 0;
            globals->unknown00 = 0;
        }

        if (globals->unknown10 <= 0.0001f && globals->unknown14 <= 0.0001f && fraction1 >= 1.0f)
        {
            globals->unknown10 = 0.0f;
            globals->unknown14 = 0.0f;
        }

        bool finished = false;
        if (globals->unknowndc > 0.0f || globals->unknowne0 > 0.0f || globals->unknownec > 0.0f || globals->unknownf0 > 0.0f)
        {
            real fraction2 = 1.0f;
            real fraction3 = 1.0f;
            if (globals->unknowne8 != globals->unknowne4)
                fraction2 = (real)PIN((g_4858a0 - globals->unknowne4) / (globals->unknowne8 - globals->unknowne4), 0.0, 1.0);
            if (globals->unknownf8 != globals->unknownf4)
                fraction3 = (real)PIN((g_4858a0 - globals->unknownf4) / (globals->unknownf8 - globals->unknownf4), 0.0, 1.0);

            real v1 = (1.0f - fraction2) * globals->unknowndc + globals->unknowne0 * fraction2;
            real v2 = (1.0f - fraction3) * globals->unknownec + globals->unknownf0 * fraction3;
            if (v1 > 0.0f || v2 > 0.0f)
            {
                globals->unknown3c = 1;
                globals->unknown40 = globals->unknownd8;
                globals->unknown44 = v1;
                globals->unknown48 = v2;
            }
        }
        else
        {
            finished = true;
        }

        if ((globals->unknowne0 == 0.0f && globals->unknownf0 == 0.0f && globals->unknowne8 <= g_4858a0 && globals->unknownf8 <= g_4858a0) || finished)
        {
            globals->unknown3c = 0;
            globals->unknown40 = 0.0f;
            globals->unknown44 = 0.0f;
            globals->unknown48 = 0.0f;
            globals->unknownd8 = 0.0f;
            globals->unknowndc = 0.0f;
            globals->unknowne0 = 0.0f;
            globals->unknowne4 = 0.0f;
            globals->unknowne8 = 0.0f;
            globals->unknownec = 0.0f;
            globals->unknownf0 = 0.0f;
            globals->unknownf4 = 0.0f;
            globals->unknownf8 = 0.0f;
        }

        real fraction4 = 0.0f;
        if (globals->unknown128 != globals->unknown124)
            fraction4 = (real)PIN((g_4858a0 - globals->unknown124) / (globals->unknown128 - globals->unknown124), 0.0, 1.0);
        real &output_4c = globals->unknown4c;
        timed_effect_interpolate(globals->unknownfc, globals->unknown100, fraction4, &output_4c);
        real &output_50_0 = globals->unknown50[0];
        timed_effect_interpolate(globals->unknown104, globals->unknown108, fraction4, &output_50_0);
        real &output_50_1 = globals->unknown50[1];
        timed_effect_interpolate(globals->unknown10c, globals->unknown110, fraction4, &output_50_1);
        real &output_50_2 = globals->unknown50[2];
        timed_effect_interpolate(globals->unknown114, globals->unknown118, fraction4, &output_50_2);
        real &output_50_3 = globals->unknown50[3];
        timed_effect_interpolate(globals->unknown11c, globals->unknown120, fraction4, &output_50_3);

        for (long i = 0; i < 4; i++)
        {
            real value = globals->unknown50[i];
            if (globals->unknown15c[i])
            {
                globals->unknown60[i][0] = globals->unknown12c[i][0] * value;
                globals->unknown60[i][1] = globals->unknown12c[i][1] * value;
                globals->unknown60[i][2] = globals->unknown12c[i][2] * value;
            }
            else
            {
                globals->unknown60[i][0] = value;
                globals->unknown60[i][1] = value;
                globals->unknown60[i][2] = value;
            }
        }

        globals->unknown90 = globals->unknown160;
        globals->unknown94 = globals->unknown164;
        globals->unknown98 = globals->unknown168;
        globals->unknown9c = globals->unknown16c;

        real fraction5 = 0.0f;
        if (globals->unknown17c != globals->unknown178)
        {
            real elapsed = (real)PIN((g_4858a0 - globals->unknown178) / (globals->unknown17c - globals->unknown178), 0.0, 1.0);
            fraction5 = PIN(1.0f - elapsed, 0.0f, 1.0f);
            if (fraction5 > 0.0f && globals->unknown170)
            {
                globals->unknowna0 = 1;
                globals->unknown170 = 0;
            }
        }
        globals->unknowna4 = fraction5;
        globals->unknowna8 = globals->unknown174;
        result = globals;
    }
    return result;
}
