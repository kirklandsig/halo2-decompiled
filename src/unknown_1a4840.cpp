// @flags /O2 /Ob1 /arch:SSE /Gr
/* UNKNOWN_1A4840.CPP: the out-of-line sequence window advance, and a
   two-term falloff */

#include "unknown_11c920.h"

/* a window over a range of sequence numbers (network_streams.cpp, which
   inlines the other window operations) */
struct s_sequence_window
{
	bool valid;
	byte unknown01[3];
	long capacity;
	long newest;
	long oldest;
	long size;
	long head;
	long count;
};

// @retail 0x1a4840
void sequence_window_advance_1a4840(s_sequence_window *window, long sequence)
{
	long advance = sequence - window->oldest;

	if (advance > 0)
	{
		if (window->count >= advance)
		{
			window->head = (window->head + advance) % window->size;
			window->count -= advance;
		}
		window->oldest = sequence;
	}
}

/* 1 up to half of maximum, falling linearly to 0 at maximum */
static inline real falloff(real value, real maximum)
{
	real minimum = maximum * 0.5f;

	if (value >= maximum)
		return 0.0f;
	if (minimum >= value)
		return 1.0f;
	return (maximum - value) / (maximum - minimum);
}

// @retail 0x1a4870
inline real function_1a4870(real distance, real maximum_distance, real angle, real maximum_angle)
{
	real distance_scale = falloff(distance, maximum_distance);

	return falloff(angle, maximum_angle) * distance_scale;
}
