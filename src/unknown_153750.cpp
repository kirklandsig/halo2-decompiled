// @flags /O2 /Gr
/* UNKNOWN_153750.CPP: the player appearance checks, the address printer and
   the profile record reset */

#include "unknown_11c920.h"
#include <stdio.h>
#include <string.h>

/* the appearance fields a profile carries (colors, model, emblem) */
struct s_player_appearance
{
	char colors[4];
	char model;
	byte emblem_foreground;
	byte emblem_background;
	byte emblem_flags;
};

char g_55eb88[0x30];

// @retail 0x153750
bool function_153750(s_player_appearance *appearance)
{
	bool valid = true;
	long i = 0;

	do
	{
		if (valid && appearance->colors[i] >= -1 && appearance->colors[i] < 18)
		{
			valid = true;
		}
		else
		{
			valid = false;
		}
		i++;
	}
	while (i < 4);
	if (valid && appearance->model >= -1 && appearance->model < 4 &&
		appearance->emblem_foreground < 0x40 && appearance->emblem_background < 0x20 && !(appearance->emblem_flags & 0xf0))
	{
		return true;
	}
	return false;
}

// @retail 0x1537a0
char *__stdcall function_1537a0(byte const *address)
{
	long i = 0;
	long remaining = 12;
	do
	{
		sprintf(&g_55eb88[i * 3], "%02x%c", address[i], (char)(i == 11 ? 0 : ':'));
		i++;
	}
	while (--remaining);
	return g_55eb88;
}

/* a profile's record (0x90 bytes) */
struct s_profile_record
{
	byte unknown00[0x40];
	dword value40[4];
	byte unknown50[0x70 - 0x50];
	dword value70[3];
	bool flag7c;
	bool flag7d;
	char value7e;
	char value7f;
	byte unknown80[4];
	long value84;
	short value88;
	short value8a;
	long value8c;
};

// @retail 0x1537f0
void function_1537f0(s_profile_record *record)
{
	memset(record, 0, sizeof(*record));
	memset(record->value40, 0, sizeof(record->value40));
	memset(record->value70, 0, sizeof(record->value70));
	record->flag7c = false;
	record->value7e = NONE;
	record->value7f = NONE;
	record->value84 = NONE;
	record->value8c = NONE;
	record->value88 = NONE;
	record->value8a = NONE;
	record->flag7d = false;
}

// @retail 0x153850
bool function_153850(byte *model)
{
	char signed_model_code = (char)*model;

	return signed_model_code >= -1 && signed_model_code < 4;
}
