// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_23EF80.CPP: the sounds of game events: an event's sound plays
   once while the same event repeats (remembered by the event's hash), and
   the sounds queued with a delay (moved out of unknown_23d970.cpp: retail
   calls its helpers out of line) */

#include "unknown_11c920.h"
#include <string.h>
#include "globals.h"

struct s_recent_entry
{
	long id;
	dword unknown04;
	byte flag08;
	byte unknown09;
	short value;
};

struct s_hash_entry
{
	dword hash;
	long values[4];
	long time;
};

struct s_recent_globals
{
	s_recent_entry recent[5];
	s_hash_entry hashes[4];
	long count;
};

struct s_hash_key
{
	dword v0;
	dword v1;
	dword unknown08;
	dword v3;
	dword v4;
	dword v5;
	dword v6;
};

void __stdcall function_23f120(long a, long b, long c);

s_recent_globals g_502350;

/* 23f120 is slot 4 of the sound source table g_444b7c (unknown_18c810.cpp) */

struct s_event;
long function_1896c0(real scale, long tag_index);
void function_23f140(long id, short value);
s_hash_entry *function_23f180(s_hash_key *key);

/* plays the event's sound unless the same event played it lately: a sound
   is remembered under the event's hash (four per hash); a new one, or an
   event of kind 0x14 or 0x15, plays (queued with its delay when asked) */
// @retail 0x23ef80
void function_23ef80(long sound_index, long delay, s_event *event, bool flag)
{
	s_hash_key *key = (s_hash_key *)event;
	s_hash_entry *entry = function_23f180(key);
	bool added = false;

	for (long i = 0; i < 4; i++)
	{
		if (entry->values[i] == sound_index)
		{
			break;
		}
		if (entry->values[i] == NONE)
		{
			entry->values[i] = sound_index;
			added = true;
			break;
		}
	}
	if ((key->v0 == 0 && (key->v1 == 0x14 || key->v1 == 0x15) || added) && sound_index != NONE)
	{
		if (flag)
		{
			function_23f140(sound_index, (short)delay);
		}
		else
		{
			function_1896c0(1.0f, sound_index);
		}
	}
}

// @retail 0x23f0a0
void function_23f0a0(void)
{
	memset(&g_502350, 0, sizeof(g_502350));
	g_502350.recent[0].flag08 = 0;
	g_502350.count = 1;
	g_502350.recent[0].id = NONE;

	real seconds = g_510c54->field_2_3 * 2.f;
	long ticks;
	__asm
	{
		fld seconds
		fistp ticks
	}
	g_502350.recent[0].value = (short)ticks;

	for (long i = 0; i < 4; i++)
		g_502350.hashes[i].time = -1000;
}

// @retail 0x23f120
void __stdcall function_23f120(long a, long b, long c)
{
	if (g_502350.recent[0].unknown04 == b)
		g_502350.recent[0].flag08 = 0;
}

// @retail 0x23f140
void function_23f140(long id, short recent_entry_value)
{
	if (g_502350.count < 5)
	{
		g_502350.recent[g_502350.count].id = id;
		g_502350.recent[g_502350.count].value = recent_entry_value;
		g_502350.count++;
	}
}

// @retail 0x23f180
s_hash_entry *function_23f180(s_hash_key *key)
{
	s_game_time_globals *game_time = g_510c54;
	real seconds = game_time->field_2_3 * 0.9f;
	long threshold;
	__asm
	{
		fld seconds
		fistp threshold
	}

	dword hash = ((((key->v1 << 12) ^ key->v4) << 4) ^ ~(key->v6 << 8)) ^ ~key->v5 ^ key->v3 ^ key->v0;
	long now = game_time->game_time;
	long best = NONE;
	long best_age = NONE;
	bool is_new = true;

	for (long i = 0; i < 4; i++)
	{
		s_hash_entry *entry = &g_502350.hashes[i];
		if (entry->hash == hash && now - entry->time < threshold)
		{
			best = i;
			is_new = false;
			break;
		}
		long age = now - entry->time;
		if (age > best_age)
		{
			best_age = age;
			best = i;
		}
	}

	g_502350.hashes[best].time = now;
	if (is_new)
	{
		g_502350.hashes[best].hash = hash;
		memset(g_502350.hashes[best].values, 0xff, sizeof(g_502350.hashes[best].values));
	}
	return &g_502350.hashes[best];
}

long function_189710(real scale, long tag_index);

/* counts down the first queued sound's delay and plays it; once it has
   played (and finished), the next one moves up */
// @retail 0x23f000
void function_23f000(void)
{
	if (g_502350.count)
	{
		if (g_502350.recent[0].value >= 0)
		{
			if (--g_502350.recent[0].value == NONE)
			{
				long id = g_502350.recent[0].id;
				long handle = NONE;

				if (id != NONE)
				{
					handle = function_189710(1.0f, id);
				}
				g_502350.recent[0].unknown04 = handle;
				if (handle != NONE)
				{
					g_502350.recent[0].flag08 = true;
				}
			}
		}
		else if (!g_502350.recent[0].flag08)
		{
			for (long i = 1; i < g_502350.count; i++)
			{
				g_502350.recent[i - 1] = g_502350.recent[i];
			}
			g_502350.count--;
		}
	}
}
