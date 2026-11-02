/* UNKNOWN_21E230.H: DirectSound mix bin and filter helpers
   (src/unknown_21e230.cpp) */

#ifndef UNKNOWN_21E230_H
#define UNKNOWN_21E230_H

#include "unknown_11c920.h"
#include <xtl.h>

/* mix bin volume pairs and the DSMIXBINS naming them (also used by
   unknown_221490.cpp) */
struct s_mixbin_settings
{
	DSMIXBINVOLUMEPAIR pairs[8];
	DSMIXBINS mixbins;
};

/* a level in decibels, passed and returned by value; constant levels are
   built from their bits (s_decibels(0ul), or 0xc2800000ul for -64) */
struct s_decibels
{
	s_decibels() {}
	s_decibels(real decibels) : value(decibels) {}
	explicit s_decibels(dword bits) { *(dword *)&value = bits; }

	s_decibels operator+(s_decibels other) const
	{
		return other += *this;
	}

	s_decibels &operator+=(s_decibels const &other)
	{
		value += other.value;
		return *this;
	}

	bool operator>(s_decibels const &other) const
	{
		return value > other.value;
	}

	real value;
};

extern s_decibels const g_44f70c;
extern s_decibels const g_44f710;

real function_12aff0(real a, real b, real c, bool flag);

/* decibels (-64 to 0) to a DirectSound volume (-6400 to 0) */
inline long sound_decibels_to_volume(s_decibels decibels)
{
	return (long)(function_12aff0(-64.0f, 0.0f, decibels.value, true) * 6400.0f - 6400.0f);
}

inline void sound_mixbins_initialize(s_mixbin_settings *settings)
{
	settings->mixbins.dwMixBinCount = 0;
	settings->mixbins.lpMixBinVolumePairs = settings->pairs;
}

void sound_mixbins_add(s_mixbin_settings *settings, long mixbin, s_decibels decibels);
long sound_filter_frequency_coefficient(real frequency);
dword sound_filter_gain_coefficient(real decibels);

#endif
