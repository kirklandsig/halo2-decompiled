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

/* the fraction of the way from start to end that the global time has gone,
   pinned to [0, 1]; equal_value when the two times are equal */
#define TIMED_FRACTION(result, start, end, equal_value) \
	if ((end) != (start)) \
	{ \
		double fraction = (g_4858a0 - (start)) / ((end) - (start)); \
		if (fraction < 0.0) \
			fraction = 0.0; \
		else if (fraction > 1.0) \
			fraction = 1.0; \
		(result) = (real)fraction; \
	} \
	else \
	{ \
		(result) = (equal_value); \
	}

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

// @retail 0x1fd20
s_timed_effect_globals *function_01fd20(s_timed_effect_globals *result)
{
    s_timed_effect_globals *globals = g_5093e0;

    if (globals && globals->unknownac)
    {
        real fraction0;
        real fraction1;
        TIMED_FRACTION(fraction0, globals->unknownb8, globals->unknownbc, 1.0f)
        TIMED_FRACTION(fraction1, globals->unknownd0, globals->unknownd4, 1.0f)
        real value;
        bool volatile finished;
        real fraction2;
        real fraction3;
        real fraction4;
        real fraction5;
        real v1;
        real v2;

        globals->unknown04 = globals->unknownb0 * (1.0f - fraction0) + globals->unknownb4 * fraction0;

        value = globals->unknownc0 * (1.0f - fraction1) + globals->unknownc4 * fraction1;
        if (value < 0.0f)
            value = 0.0f;
        else if (value > 1.0f)
            value = 1.0f;
        globals->unknown10 = value;

        value = globals->unknownc8 * (1.0f - fraction1) + globals->unknowncc * fraction1;
        if (value < 0.0f)
            value = 0.0f;
        else if (value > 1.0f)
            value = 1.0f;
        globals->unknown14 = value;

        if (memcmp(&globals->unknown18, g_468718, sizeof(point3f)) == 0)
            globals->unknown18 = *g_468720;

        if (globals->unknown04 <= 0.0001f || g_4ba04c > 1)
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

        finished = false;
        if (globals->unknowndc > 0.0f)
        {
            TIMED_FRACTION(fraction2, globals->unknowne4, globals->unknowne8, 1.0f)
            TIMED_FRACTION(fraction3, globals->unknownf4, globals->unknownf8, 1.0f)

            v1 = globals->unknowndc * (1.0f - fraction2) + globals->unknowne0 * fraction2;
            v2 = globals->unknownec * (1.0f - fraction3) + globals->unknownf0 * fraction3;
            if (v1 > 0.0f || v2 > 0.0f)
            {
                globals->unknown3c = 1;
                globals->unknown40 = globals->unknownd8;
                globals->unknown44 = v1;
                globals->unknown48 = v2;
            }
        }
        else if (globals->unknowne0 > 0.0f || globals->unknownec > 0.0f || globals->unknownf0 > 0.0f)
        {
            TIMED_FRACTION(fraction2, globals->unknowne4, globals->unknowne8, 1.0f)
            TIMED_FRACTION(fraction3, globals->unknownf4, globals->unknownf8, 1.0f)

            v1 = globals->unknowndc * (1.0f - fraction2) + globals->unknowne0 * fraction2;
            v2 = globals->unknownec * (1.0f - fraction3) + globals->unknownf0 * fraction3;
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

        TIMED_FRACTION(fraction4, globals->unknown124, globals->unknown128, 0.0f)
        globals->unknown4c = globals->unknownfc * (1.0f - fraction4) + globals->unknown100 * fraction4;
        globals->unknown50 = globals->unknown104 * (1.0f - fraction4) + globals->unknown108 * fraction4;
        globals->unknown54 = globals->unknown10c * (1.0f - fraction4) + globals->unknown110 * fraction4;
        globals->unknown58 = globals->unknown114 * (1.0f - fraction4) + globals->unknown118 * fraction4;
        globals->unknown5c = globals->unknown11c * (1.0f - fraction4) + globals->unknown120 * fraction4;

        if (globals->unknown15c)
        {
            globals->unknown60[0][0] = globals->unknown12c[0][0] * globals->unknown50;
            globals->unknown60[0][1] = globals->unknown12c[0][1] * globals->unknown50;
            globals->unknown60[0][2] = globals->unknown12c[0][2] * globals->unknown50;
        }
        else
        {
            globals->unknown60[0][0] = globals->unknown50;
            globals->unknown60[0][1] = globals->unknown50;
            globals->unknown60[0][2] = globals->unknown50;
        }
        if (globals->unknown15d)
        {
            globals->unknown60[1][0] = globals->unknown12c[1][0] * globals->unknown54;
            globals->unknown60[1][1] = globals->unknown12c[1][1] * globals->unknown54;
            globals->unknown60[1][2] = globals->unknown12c[1][2] * globals->unknown54;
        }
        else
        {
            globals->unknown60[1][0] = globals->unknown54;
            globals->unknown60[1][1] = globals->unknown54;
            globals->unknown60[1][2] = globals->unknown54;
        }
        if (globals->unknown15e)
        {
            globals->unknown60[2][0] = globals->unknown12c[2][0] * globals->unknown58;
            globals->unknown60[2][1] = globals->unknown12c[2][1] * globals->unknown58;
            globals->unknown60[2][2] = globals->unknown12c[2][2] * globals->unknown58;
        }
        else
        {
            globals->unknown60[2][0] = globals->unknown58;
            globals->unknown60[2][1] = globals->unknown58;
            globals->unknown60[2][2] = globals->unknown58;
        }
        if (globals->unknown15f)
        {
            globals->unknown60[3][0] = globals->unknown12c[3][0] * globals->unknown5c;
            globals->unknown60[3][1] = globals->unknown12c[3][1] * globals->unknown5c;
            globals->unknown60[3][2] = globals->unknown12c[3][2] * globals->unknown5c;
        }
        else
        {
            globals->unknown60[3][0] = globals->unknown5c;
            globals->unknown60[3][1] = globals->unknown5c;
            globals->unknown60[3][2] = globals->unknown5c;
        }

        globals->unknown90 = globals->unknown160;
        globals->unknown94 = globals->unknown164;
        globals->unknown98 = globals->unknown168;
        globals->unknown9c = globals->unknown16c;

        fraction5 = 0.0f;
        if (globals->unknown17c != globals->unknown178)
        {
            double fraction = (g_4858a0 - globals->unknown178) / (globals->unknown17c - globals->unknown178);

            if (fraction < 0.0)
                fraction = 0.0;
            else if (fraction > 1.0)
                fraction = 1.0;
            fraction5 = 1.0f - (real)fraction;
            if (fraction5 < 0.0f)
                fraction5 = 0.0f;
            else if (fraction5 > 1.0f)
                fraction5 = 1.0f;
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
