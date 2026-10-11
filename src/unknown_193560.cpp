// @flags /O2 /Ob1 /Gr
/* UNKNOWN_193560.CPP: the game variant checks (lane H; kept apart with /Ob1
   because retail calls them out of line) */

#include "unknown_11c920.h"

/* a game variant (0x614 bytes; the 16 of them are at 0x551ae8) */
struct s_surface_description
{
	long type;
	long field_4;
	dword field_8;
	byte unknown00c[0x51c - 0xc];
	char field_51c[128];
	byte unknown59c[0x5e4 - 0x59c];
	long field_5e4;
	bool flag_5e8;
	byte unknown5e9[3];
	long width;       // 0x5ec
	long height;      // 0x5f0
	long depth;       // 0x5f4
	long field_5f8;   // 0x5f8
	long field_5fc;
	long field_600;
	bool flag_604;    // 0x604
	byte unknown605[3];
	long field_608;   // 0x608
	long field_60c;   // 0x60c
	byte unknown610[4];
};

// @retail 0x193560
bool function_193560(s_surface_description *p)
{
	bool valid;
	dword index;

	valid = (p->type == 2 || p->type == 1 || p->type == 4 || p->type == 3 || p->type == 5) &&
		p->field_4 >= 0 && p->field_4 < 4 && p->field_8 > 0 && p->field_8 <= 100;
	for (index = 0; index < 128; index++)
	{
		valid = valid && p->field_51c[index] >= (long)index && p->field_51c[index] <= 127;
	}
	return valid;
}

// @retail 0x193610
byte function_193610(s_surface_description *p)
{
	long width = p->width;
	if (width >= 1)
	{
		long height = p->height;
		if (height <= 16 && width <= height)
		{
			return true;
		}
	}
	return false;
}

// @retail 0x193630
byte function_193630(s_surface_description *p)
{
	long width = p->width;
	if (width > 1 && width <= 16)
	{
		long height = p->height;
		if (height > 1)
		{
			long depth = p->depth;
			if (depth <= 16 / width && height <= depth)
			{
				long a = p->field_5fc;
				if (a > 0)
				{
					long b = p->field_600;
					if (b <= depth && a <= b)
					{
						long c = p->field_5f8;
						if (c >= 0 && c <= depth - height)
						{
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

// @retail 0x1936a0
byte function_1936a0(s_surface_description *p)
{
	long width = p->width;
	if (width > 1 && width <= 16)
	{
		long height = p->height;
		if (height >= 1)
		{
			long depth = p->depth;
			if (depth <= 16 / width && height <= depth)
			{
				return true;
			}
		}
	}
	return false;
}


/* the check of type 5: the same code as type 3's, which the linker folded
   into it */
byte function_1936a0_type5(s_surface_description *p)
{
	long width = p->width;
	if (width > 1 && width <= 16)
	{
		long height = p->height;
		if (height >= 1)
		{
			long depth = p->depth;
			if (depth <= 16 / width && height <= depth)
			{
				return true;
			}
		}
	}
	return false;
}


/* the points for each place from 1 to 16: none outside [first, last], none
   for the last place, and a share of the scale falling to zero at the last */
// @retail 0x1939f0
void function_1939f0(long scale, long first, long last, long *points)
{
	for (long place = 1; place <= 16; place++)
	{
		long place_points_share;

		if (place < first || place > last)
			place_points_share = 0x7fffffff;
		else if (place == last)
			place_points_share = 0;
		else
			place_points_share = scale * (last - place) / (last - first);
		points[place - 1] = place_points_share;
	}
}

// @retail 0x193a50
void function_193a50(long *points, long team_size, long last_team, long first_team, long scale)
{
	long last = team_size * last_team;
	long first = team_size * first_team;

	for (long place = 1; place <= 16; place++)
	{
		long value;

		if (place < first || place > last)
			value = 0x7fffffff;
		else if (place == last)
			value = 0;
		else
			value = (last_team - place / team_size) * scale / (last_team - first_team);
		points[place - 1] = value;
	}
}
