// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_21E230.CPP: DirectSound mix bin and filter helpers */

#include "unknown_11c920.h"
#include <xtl.h>
#include <math.h>
#include "unknown_21e230.h"

// score 8 (was 8): retail stores the mix bin before it loads the decibels for the push and keeps
// the count in eax and the volume in edx after the call; ours loads the decibels first (edx) and uses
// ecx/eax after the call. Tried: the decibels read through a pointer 8; the volume in a local 8;
// the conversion written out 8; a pairs pointer 8; a by-reference conversion helper 8.
// Decibel/bin scheduling and count-volume register roles remain; native pair aggregate/field
// production, with original/sampled index and postincrement publication, six forms 15 each; retained index
// with live count increment 11; reservation before conversion 21. Original field-wise append and body
// restored.
// Provider inline/force-inline at definition and shared declaration: own score 8/8;
// ordinary shared callers tie 200, forced caller graph loses dispatcher at 26. All restored.
// J199: returned index via postincrement/count-minus-one 18/37; retained post-bin/
// entry index 24/12; count from those indices 28/19; signed results 18/24; post-
// conversion index with live/index-based count 24/28; void stdcall/cdecl 8/11;
// entry-index result stdcall/cdecl 12/12. Original void interface and body restored.
// Round 46: volume store before bin store 22; bin-index postincrement and separate increment before
// volume 25 each; preincrement/+=/count+1/1+count 8 each; const scalar input 8. Original body restored.
// Round 53: a DSMIXBINS pointer local 8; a count reference 8; an element pointer for the bin store 8;
// a DWORD index for the bin store 8; a long-cast count 8. Diagnostics (not kept): /Op 8, the decibels'
// address taken 8, the conversion written out or split 8: every form compiles to the same code, the
// decibels load always ahead of the count load.
// @retail 0x21e230
void sound_mixbins_add(s_mixbin_settings *settings, long mixbin, real decibels)
{
	settings->pairs[settings->mixbins.dwMixBinCount].dwMixBin = mixbin;
	settings->pairs[settings->mixbins.dwMixBinCount].lVolume = sound_decibels_to_volume(decibels);
	settings->mixbins.dwMixBinCount++;
}
