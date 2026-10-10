// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_0E6800.CPP: whether an animation channel has stopped playing */

#include "unknown_11c920.h"
#include "unknown_1c62f0.h"

// @retail 0xe6800
bool function_0e6800(c_animation_channel const *channel)
{
	// Retail materializes a full-width 0/1, then tests its low byte.
	long playing = ((channel->flags & 1) && !(channel->unknown11 & 9)) ? 1 : 0;

	return !(byte)playing;
}
