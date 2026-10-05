// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_276F80.CPP: what the command script being run looks at and aims at */

#include "unknown_11c920.h"
#include "globals.h"
#include "command_scripts.h"
#include "unknown_276f80.h"
#include "squads.h"
#include <math.h>

/* the radii of something the command script being run does, squared */
// @retail 0x276d50
void function_276d50(real a, real b, real c)
{
	long script_index = g_502410;
	if (script_index != NONE)
	{
		s_command_script *script = command_script_get(script_index);
		script->type = 0x14;
		script->flagac = true;
		script->flagd0 = false;
		script->indexb0 = NONE;
		script->valueb4 = a * a;
		script->valueb8 = b * b;
		script->valuebc = c * c;
	}
}
inline void command_script_set_look(bool enable, short type, long index)
{
	long script_index = g_502410;
	s_command_script *script = command_script_get(script_index);
	script->flag52 = enable;
	if (enable)
	{
		script->type54 = type;
		script->index58 = index;
		script->flag51 = false;
	}
}

inline void command_script_set_aim(bool enable, short type, long index)
{
	long script_index = g_502410;
	s_command_script *script = command_script_get(script_index);
	script->flag46 = enable;
	if (enable)
	{
		script->type48 = type;
		script->index4c = index;
	}
}

// @retail 0x276c00
void function_276c00(long object_index, real a, real b, real c)
{
	long script_index = g_502410;
	if (script_index != NONE)
	{
		s_command_script *script = command_script_get(script_index);
		script->type = 0x14;
		script->flagac = true;
		script->flagd0 = false;
		script->indexb0 = object_index;
		script->valueb4 = a * a;
		script->valueb8 = b * b;
		script->valuebc = c * c;
		if (object_index != NONE)
		{
			s_command_script *look_script = command_script_get(script_index);
			look_script->flag52 = true;
			*(volatile bool *)&look_script->flag46 = false;
			*(volatile bool *)&look_script->flag51 = false;
			look_script->type54 = 1;
			look_script->index58 = object_index;
			s_command_script *aim_script = command_script_get(script_index);
			aim_script->flag46 = true;
			aim_script->type48 = 1;
			aim_script->index4c = object_index;
			look_script->flag50 = true;
			look_script->flag51 = true;
		}
	}
}
// @retail 0x276f80
void function_276f80(bool enable, long point_index)
{
	if (g_502410 != NONE)
	{
		command_script_set_look(enable, 2, point_index);
		command_script_set_aim(enable, 2, point_index);
	}
}

inline void command_script_set_look_at_object(bool enable, long object_index)
{
	long script_index = g_502410;
	s_command_script *script = command_script_get(script_index);
	script->flag52 = enable;
	if (enable)
	{
		script->flag46 = false;
		script->type54 = 1;
		script->index58 = object_index;
		script->flag51 = false;
	}
}

long function_276dd0(long actor_index);
extern long g_50240c;

/* a command script of the array read once into a local (retail keeps the
   array in ebp across the call to 276dd0) */
inline s_command_script *function_x157525(s_record_pool *scripts, long index)
{
	return &((s_command_script *)scripts->data)[index & 0xffff];
}

/* points the command script being run (look and aim) at what its actor would
   look at (276dd0) */
// @retail 0x276fd0
void function_276fd0(bool enable)
{
	long script_index = g_502410;
	if (script_index != NONE && g_50240c != NONE)
	{
		s_record_pool *scripts = g_502408;
		s_command_script *script = function_x157525(scripts, script_index);
		bool looking = false;
		if (enable)
		{
			long object_index = function_276dd0(g_50240c);
			if (object_index != NONE)
			{
				script->flag52 = true;
				script->type54 = 1;
				script->index58 = object_index;
				s_command_script *aim_script = function_x157525(scripts, script_index);
				aim_script->flag46 = true;
				aim_script->type48 = 1;
				aim_script->index4c = object_index;
				script->flag51 = looking;
			}
			else
			{
				script->flag52 = looking;
				function_x157525(scripts, script_index)->flag46 = looking;
			}
		}
		else
		{
			script->flag52 = looking;
			function_x157525(scripts, script_index)->flag46 = looking;
		}
	}
}

/* points the aim of the command script being run at what its actor would
   look at (276dd0) */
// @retail 0x2770c0
void function_2770c0(bool enable)
{
	long script_index = g_502410;
	if (script_index != NONE && g_50240c != NONE)
	{
		s_command_script *script = command_script_get(script_index);
		if (enable)
		{
			long object_index = function_276dd0(g_50240c);
			if (object_index != NONE)
			{
				script->flag46 = true;
				script->type48 = 1;
				script->index4c = object_index;
				return;
			}
		}
		script->flag46 = false;
	}
}

// @retail 0x277060
void function_277060(bool enable, long object_index)
{
	if (g_502410 != NONE)
	{
		command_script_set_look_at_object(enable, object_index);
		command_script_set_aim(enable, 1, object_index);
	}
}
// @retail 0x277120
void function_277120(real a, real b, real c)
{
	long script_index = g_502410;
	if (script_index != NONE)
	{
		s_command_script *script = command_script_get(script_index);
		script->value8 = b;
		script->type = 4;
		script->index_a = NONE;
		script->valuec = a;
		if (fabs(c) <= 45.0f)
			script->value_short = 0;
		else if (c > 0.0f && c < 135.0f)
			script->value_short = 1;
		else if (c < 0.0f && c > -135.0f)
			script->value_short = 2;
		else if (fabs(c) >= 135.0f)
			script->value_short = 3;
	}
}
// @retail 0x2771d0
void function_2771d0(bool enable, long object_index)
{
	long script_index = g_502410;
	if (script_index != NONE)
	{
		s_command_script *script = command_script_get(script_index);
		script->flag45 = enable;
		if (enable && object_index != NONE)
		{
			script->flag52 = true;
			script->type54 = 1;
			script->index58 = object_index;
			script->flag46 = false;
		}
	}
}

/* makes the command script being run wait a number of seconds */
// @retail 0x277250
void function_277250(real seconds)
{
	long script_index = g_502410;
	if (script_index != NONE)
	{
		s_command_script *script = command_script_get(script_index);
		long ticks;

		script->type = 0;
		seconds *= (real)g_510c54->field_2_3;
		__asm
		{
			fld seconds
			fistp ticks
		}
		script->value8 = (real)ticks;
	}
}

// @retail 0x277210
void function_277210(bool enable, long point_index)
{
	long script_index = g_502410;
	if (script_index != NONE)
	{
		s_command_script *script = command_script_get(script_index);
		script->flag45 = enable;
		if (enable && point_index != NONE)
		{
			script->flag52 = true;
			script->type54 = 2;
			script->index58 = point_index;
			script->flag46 = false;
		}
	}
}

/* the names of something the command script being run does */
long const g_46fca8[7] = { 0x06000085, 0x06000084, 0x06000086, 0x04000089, 0x07000039, 0x070000c9, 0x0700002c };

// @retail 0x2775d0
void function_2775d0(short index)
{
	long script_index = g_502410;
	if (script_index != NONE)
	{
		s_command_script *script = command_script_get(script_index);
		if (index >= 0 && index < 7)
			script->name94 = g_46fca8[index];
		else
			script->name94 = NONE;
	}
}

// @retail 0x277620
void function_277620(short value, long index_a, long index_b, long index_c)
{
	long script_index = g_502410;
	if (script_index != NONE && value >= 0 && value < 6)
	{
		s_command_script *script = command_script_get(script_index);
		script->value_short = value;
		*(long *)&script->value8 = index_a;
		script->type = 0x12;
		script->index_a = index_b;
		script->index_b = index_c;
		script->flag5c = true;
	}
}

struct s_actor_277680
{
	byte unknown000[0x266];
	bool active;
	byte unknown267[5];
	long object_index;
	short mode;
	byte unknown272[0x658 - 0x272];
	real value658;
	byte unknown65c[0x674 - 0x65c];
	real value674;
};

struct s_unit_277680
{
	byte unknown000[0x354];
	real speed;
};

struct s_object_header_277680
{
	byte unknown00[8];
	s_unit_277680 *object;
};

void function_10b010(long object_index, real forward_speed, real left_speed, real up_speed);

/* sets the current actor's scripted speed and directional movement */
// @retail 0x277680
void __stdcall function_277680(real value)
{
	if (g_50240c != NONE && g_502410 != NONE)
	{
		s_actor_277680 *actor = (s_actor_277680 *)actor_datum_get(g_50240c);
		s_command_script *script = command_script_get(g_502410);
		if (actor->active)
		{
			long object_index = actor->object_index;
			s_unit_277680 *unit = ((s_object_header_277680 *)g_4e0300->data)[object_index & 0xffff].object;
			function_10b010(object_index, value, 0.0f, 0.0f);
			unit->speed = value;
			script->flag64 = true;
			script->value68 = value;
			real direction;
			if (fabs(value) < 1.5f)
				direction = 0.0f;
			else
				direction = value > 0.0f ? 1.0 : -1.0;
			switch (actor->mode)
			{
			case 3:
				actor->value658 = direction;
				break;
			case 5:
				actor->value674 = direction;
				break;
			}
		}
	}
}
