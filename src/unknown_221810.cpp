// @flags /O2 /Ob1 /arch:SSE /Gr
/* a sound class of the sound classes tag, in a file of its own that expands only
   inline-marked functions: retail calls it out of line at every site */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_221810.h"

// @retail 0x221810
s_unknown_5c *function_221810(
	short index)
{
	return sound_class_definition_get(index);
}
