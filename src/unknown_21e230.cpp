// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_21E230.CPP: DirectSound mix bin and filter helpers */

#include "unknown_11c920.h"
#include <xtl.h>
#include <math.h>
#include "unknown_21e230.h"

// @retail 0x21e230
void sound_mixbins_add(s_mixbin_settings *settings, long mixbin, s_decibels decibels)
{
	settings->pairs[settings->mixbins.dwMixBinCount].dwMixBin = mixbin;
	settings->pairs[settings->mixbins.dwMixBinCount].lVolume = sound_decibels_to_volume(decibels);
	settings->mixbins.dwMixBinCount++;
}
