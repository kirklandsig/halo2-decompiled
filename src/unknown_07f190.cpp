// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_07F190.CPP: the trimmed mean of a set of samples (lane D: called
   by the session's member latency summary and the observer) */

#include "unknown_11c920.h"
#include "globals.h"
#include <xtl.h>
#include <string.h>

/* the fraction of the samples dropped at each end */
real *g_4cf8e8;

struct s_network_observer;
void network_observer_reset_bandwidth(s_network_observer *observer);

bool g_4cf8e0;
s_network_observer *g_4cf8e4;
bool g_4cf8ec;
long g_4cf8f0[27];
extern bool g_4cf95c;
bool g_4cf95d;
long g_4cf960;
bool g_4cf964;
extern long g_4cf968;
extern long g_4cf96c;

/* sorts the samples, drops the given fraction of them at each end and returns
   the mean of the rest */
// @retail 0x7f190
long samples_trimmed_mean(const long *samples, long count)
{
	long sorted[128];
	long trim = (long)(count * *g_4cf8e8);
	long used;
	long total;
	long i;

	if (trim > 0)
	{
		for (i = 0; i < count; i++)
		{
			for (long j = i; j >= 0; j--)
			{
				if (j <= 0 || sorted[j - 1] <= samples[i])
				{
					sorted[j] = samples[i];
					break;
				}
				sorted[j] = sorted[j - 1];
			}
		}
		used = i - 2 * trim;
		memcpy(sorted, &sorted[trim], used * sizeof(long));
	}
	else
	{
		for (i = 0; i < count; i++)
			sorted[i] = samples[i];
		used = count;
	}
	total = 0;
	for (i = 0; i < used; i++)
		total += sorted[i];
	return total / used;
}

// @retail 0x7f020
void function_7f020(s_network_observer *observer, real *trim)
{
	g_4cf8e4 = observer;
	g_4cf95c = false;
	g_4cf964 = false;
	g_4cf8ec = false;
	g_4cf8e8 = trim;
	g_4cf968 = NONE;
	g_4cf96c = 16;
	memset(g_4cf8f0, 0, sizeof(g_4cf8f0));
	g_4cf8e0 = true;
}

// @retail 0x7f260
void function_7f260(void)
{
	if (g_4cf8e0)
	{
		network_observer_reset_bandwidth(g_4cf8e4);
		g_4cf95d = true;
		if (g_510548)
			g_4cf960 = g_51054c;
		else
			g_4cf960 = GetTickCount();
	}
}
