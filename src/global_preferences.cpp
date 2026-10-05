// @flags /O2 /arch:SSE /Gr
/* GLOBAL_PREFERENCES.CPP: the console's preferences, kept in
   z:\preferences.dat: a signed block of 0x1e8 bytes loaded at startup and
   written back asynchronously when it changes. The setters at 0x120df0,
   0x120e40, 0x121040 and 0x121060 moved here from unknown_11fc80.cpp. */

#include "unknown_11c920.h"
#include "async.h"
#include "global_preferences.h"
#include <xtl.h>
#include <string.h>

bool signature_calculate_end(XCALCSIG_SIGNATURE *signature);

extern HANDLE g_470024;

/* the preferences data that the signature covers */
#define PREFERENCES_DATA(preferences) (&(preferences)->version)
#define k_preferences_data_size (sizeof(s_global_preferences) - sizeof(XCALCSIG_SIGNATURE))
#define k_preferences_version 8

char const *g_4687ec = "z:\\preferences.dat";

s_global_preferences_globals global_preferences_globals;

long function_xf5684f(void const *a, void const *b, long size);

void function_121120(void);

// @retail 0x120d80
void global_preferences_flush(void)
{
	if (global_preferences_globals.file.handle != INVALID_HANDLE_VALUE)
	{
		function_120d50(&global_preferences_globals.done, false);
		if (global_preferences_globals.dirty)
			function_121120();
		function_120d50(&global_preferences_globals.done, false);
		async_flush_file_blocking(global_preferences_globals.file, 8);
	}
}

// @retail 0x120df0
void function_120df0(long index, wchar_t const *name)
{
	wchar_t *profile_name = global_preferences_globals.current.names[index];

	wcsncpy(profile_name, name, 31);
	profile_name[31] = 0;
	global_preferences_globals.dirty = true;
}

// @retail 0x120e40
void function_120e40(wchar_t const *name)
{
	wcsncpy(global_preferences_globals.current.names[4], name, 31);
	global_preferences_globals.current.names[4][31] = 0;
	global_preferences_globals.dirty = true;
}

// @retail 0x121040
void function_121040(long value)
{
	if (value != NONE)
	{
		global_preferences_globals.current.unknown170 = value;
		global_preferences_globals.dirty = true;
	}
}

// @retail 0x121060
void function_121060(long value)
{
	if (value < 0)
		global_preferences_globals.current.unknown174 = 0;
	else
	{
		global_preferences_globals.current.unknown174 = 3;
		if (!(value > 3))
			global_preferences_globals.current.unknown174 = value;
	}
	global_preferences_globals.dirty = true;
}

// @retail 0x1210a0
void function_1210a0(long *value)
{
	*value = global_preferences_globals.current.unknown174 < 0 ? 0 :
		(global_preferences_globals.current.unknown174 > 3 ? 3 : global_preferences_globals.current.unknown174);
}

// @retail 0x1210c0
void function_1210c0(long value)
{
	if (value < 0)
		global_preferences_globals.current.unknown178 = 0;
	else
	{
		global_preferences_globals.current.unknown178 = 3;
		if (!(value > 3))
			global_preferences_globals.current.unknown178 = value;
	}
	global_preferences_globals.dirty = true;
}

// @retail 0x121100
void function_121100(long *value)
{
	*value = global_preferences_globals.current.unknown178 < 0 ? 0 :
		(global_preferences_globals.current.unknown178 > 3 ? 3 : global_preferences_globals.current.unknown178);
}

// @retail 0x121120
void function_121120(void)
{
	if (global_preferences_globals.dirty && global_preferences_globals.done)
	{
		if (function_xf5684f(PREFERENCES_DATA(&global_preferences_globals.current), PREFERENCES_DATA(&global_preferences_globals.saved), k_preferences_data_size) != 0)
		{
			XCALCSIG_SIGNATURE signature;
			bool signed_data;

			memcpy(PREFERENCES_DATA(&global_preferences_globals.saved), PREFERENCES_DATA(&global_preferences_globals.current), k_preferences_data_size);
			signed_data = false;
			g_470024 = XCalculateSignatureBegin(XCALCSIG_FLAG_NON_ROAMABLE);
			if (g_470024 != INVALID_HANDLE_VALUE &&
				XCalculateSignatureUpdate(g_470024, (BYTE const *)PREFERENCES_DATA(&global_preferences_globals.saved), k_preferences_data_size) == ERROR_SUCCESS)
			{
				signed_data = signature_calculate_end(&signature);
			}
			if (global_preferences_globals.file.handle != INVALID_HANDLE_VALUE && signed_data)
			{
				global_preferences_globals.saved.signature = signature;
				global_preferences_globals.write_task = function_1a1050(global_preferences_globals.file, &global_preferences_globals.saved, sizeof(global_preferences_globals.saved), 0, 1, 8, 2, NULL, &global_preferences_globals.done);
			}
		}
		global_preferences_globals.dirty = false;
	}
}

// @retail 0x121220
void global_preferences_set_defaults(void)
{
	memset(&global_preferences_globals.current, 0, sizeof(global_preferences_globals.current));
	global_preferences_globals.current.unknown18 = NONE;
	global_preferences_globals.current.unknown1c = NONE;
	global_preferences_globals.current.version = k_preferences_version;
	global_preferences_globals.current.unknown1e8 = 0;
	global_preferences_globals.current.unknown174 = 1;
	global_preferences_globals.current.unknown178 = 1;
	memset(PREFERENCES_DATA(&global_preferences_globals.saved), 0, k_preferences_data_size);
	global_preferences_globals.dirty = true;
}

// @retail 0x121280
void global_preferences_verify(s_global_preferences *preferences)
{
	preferences->names[4][31] = 0;
	for (long i = 0; i < 4; i++)
		global_preferences_globals.current.names[i][31] = 0;

	if (preferences->unknown18 < 0 || preferences->unknown18 >= 9)
		preferences->unknown18 = NONE;
	if (preferences->unknown1c < 0 || preferences->unknown1c >= 9)
		preferences->unknown1c = NONE;
	preferences->unknown170 = preferences->unknown170 < 0 ? 0 : (preferences->unknown170 > 15 ? 15 : preferences->unknown170);
	preferences->unknown174 = preferences->unknown174 < 0 ? 0 : (preferences->unknown174 > 3 ? 3 : preferences->unknown174);
	preferences->unknown178 = preferences->unknown178 < 0 ? 0 : (preferences->unknown178 > 3 ? 3 : preferences->unknown178);
	if (preferences->unknown17c[0] < 0 || (dword)preferences->unknown17c[0] > 8 ||
		preferences->unknown1a0[0] < 0 || (dword)preferences->unknown1a0[0] > 8)
	{
		memset(preferences->unknown17c, 0, sizeof(preferences->unknown17c) + sizeof(preferences->unknown1a0));
	}
}

// @retail 0x121350
void global_preferences_initialize(void)
{
	if (!global_preferences_globals.initialized)
	{
		global_preferences_globals.initialized = true;
		function_1a0b40(g_4687ec, 3, 3, 4, 8, 6, &global_preferences_globals.file, &global_preferences_globals.done);
		function_120d50(&global_preferences_globals.done, false);
		if (global_preferences_globals.file.handle != INVALID_HANDLE_VALUE)
		{
			s_global_preferences preferences;
			XCALCSIG_SIGNATURE signature;
			dword bytes_read;
			bool signed_data;
			bool valid;

			function_1a0f10(global_preferences_globals.file, &preferences, sizeof(preferences), 0, 8, 6, &bytes_read, &global_preferences_globals.done);
			function_120d50(&global_preferences_globals.done, false);

			signed_data = false;
			g_470024 = XCalculateSignatureBegin(XCALCSIG_FLAG_NON_ROAMABLE);
			if (g_470024 != INVALID_HANDLE_VALUE &&
				XCalculateSignatureUpdate(g_470024, (BYTE const *)PREFERENCES_DATA(&preferences), k_preferences_data_size) == ERROR_SUCCESS)
			{
				signed_data = signature_calculate_end(&signature);
			}
			valid = true;
			valid &= preferences.version == k_preferences_version;
			valid &= signed_data && memcmp(&preferences.signature, &signature, sizeof(signature)) == 0;
			valid &= bytes_read == sizeof(preferences);
			if (valid)
			{
				global_preferences_verify(&preferences);
				global_preferences_globals.done = true;
				global_preferences_globals.current = preferences;
				memcpy(PREFERENCES_DATA(&global_preferences_globals.saved), PREFERENCES_DATA(&global_preferences_globals.current), k_preferences_data_size);
				return;
			}
		}
		global_preferences_set_defaults();
		if (global_preferences_globals.file.handle != INVALID_HANDLE_VALUE)
		{
			global_preferences_flush();
			global_preferences_globals.write_task = function_1a1310(global_preferences_globals.file, sizeof(s_global_preferences), 8, 2, NULL, &global_preferences_globals.done);
		}
	}
}
