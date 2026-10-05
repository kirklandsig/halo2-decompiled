/* UNKNOWN_120D80.H: the console's preferences in z:\preferences.dat
   (unknown_120d80.cpp) */
#ifndef UNKNOWN_120D80_H
#define UNKNOWN_120D80_H

#include "unknown_11c920.h"
#include "job_queue.h"
#include <xtl.h>

struct s_global_preferences
{
	XCALCSIG_SIGNATURE signature;
	long version;
	long unknown18;
	long unknown1c;
	long unknown20;
	byte unknown24[8];
	wchar_t names[5][32];
	long unknown16c;
	long unknown170;
	long unknown174;
	long unknown178;
	long unknown17c[9];
	long unknown1a0[18];
	long unknown1e8;
	byte unknown1ec[0x10];
};

/* 0x510818 */
struct s_global_preferences_globals
{
	bool initialized;
	bool dirty;
	s_file_handle file;
	s_global_preferences current;
	s_global_preferences saved;
	long write_task;
	bool volatile done;
};

extern s_global_preferences_globals global_preferences_globals;

#endif
