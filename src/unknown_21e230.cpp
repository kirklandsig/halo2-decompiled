// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_21E230.CPP: DirectSound mix bin and filter helpers */

#include "unknown_11c920.h"
#include <xtl.h>
#include <math.h>
#include "unknown_21e230.h"

/* appends a mix-bin number and its decibel volume (as DirectSound's
   hundredths-of-decibel lVolume) to a DSMIXBINS array; 0x21e230 is near:
   retail loads decibels from the stack after the dwMixBin store, our build
   hoists the load to function entry, and every differing instruction
   follows from that scheduling choice. The callee function_12aff0
   (0x12aff0) is still unmatched; revisit after it lands. */
// @retail 0x21e230
void sound_mixbins_add(s_mixbin_settings *settings, long mixbin, real decibels)
{
	settings->pairs[settings->mixbins.dwMixBinCount].dwMixBin = mixbin;
	settings->pairs[settings->mixbins.dwMixBinCount].lVolume = sound_decibels_to_volume(decibels);
	settings->mixbins.dwMixBinCount++;
}
