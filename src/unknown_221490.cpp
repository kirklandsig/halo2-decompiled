// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include <xtl.h>
#include <string.h>
#include "unknown_123b30.h"
#include "crc.h"
#include "globals.h"
#include "unknown_21e230.h"
#include "unknown_221810.h"
#include "sound_driver.h"
#include <math.h>

enum
{
	k_sound_class_count = 54,
	k_silence_buffer_size = 1000
};

/* the wave format with natural alignment (the SDK header packs it) */
struct s_wave_format
{
	word format_tag;
	word channels;
	dword samples_per_second;
	dword average_bytes_per_second;
	word block_align;
	word bits_per_sample;
	word extra_size;
};

/* a sound class volume fade, 16 bytes; target and current are real values
   handled as raw bits */
struct s_sound_class_fade
{
	dword target;
	dword current;
	real time;
	byte flags;
	byte unknownd[3];
};

/* a mix bin table: a count followed by that many pairs */
struct s_mixbin_list
{
	long count;
	DSMIXBINVOLUMEPAIR pairs[8];
};


/* the volumes the game sets */
struct s_sound_driver_volumes
{
	real volume_a;
	real volume_b;
	real volume_c;
	real volume_d;
	long unknown10;
	real unknown14;
};

s_sound_effect_parameters g_47005c =
{
	true, {0, 0, 0},
	0, 0, 0, 0,
	0.0f, 0, 0.0f, 0, 0.25f, 8000, 0.0f
};

#define PIN(n, floor, ceiling) ((n) < (floor) ? (floor) : ((n) > (ceiling) ? (ceiling) : (n)))

/* a gain in [0, 1] as a direct sound volume in hundredths of decibels */
PRIVATE inline long sound_gain_to_volume(real gain)
{
	long volume;

	if (gain == 0.0f)
	{
		volume = -6400;
	}
	else
	{
		volume = (long)(log10(gain) * 2000.0f);
		if (volume < -6400)
			volume = -6400;
		else if (volume > 0)
			volume = 0;
	}
	return volume;
}

dword g_510800_pool_base;
long g_510804_pool_size;
dword g_510808_pool_checksum;

s_sound_class_fade *g_502118;
bool g_50211c;
char const *g_470090[k_sound_class_count];

PRIVATE __forceinline void function_221491(long arg_0, long arg_1, LPDIRECTSOUNDBUFFER *arg_2)
{
	s_wave_format format;
	DSBUFFERDESC description;
	DSMIXBINVOLUMEPAIR pair;
	DSMIXBINS mixbins;
	long aligned_size;
	byte *memory;
	byte *top;

	memset(&format, 0, 0x12);
	memset(&description, 0, sizeof(description));

	LPDIRECTSOUNDBUFFER buffer = (LPDIRECTSOUNDBUFFER)arg_1;

	format.format_tag = (word)arg_0;
	format.channels = (word)arg_0;
	format.samples_per_second = k_silence_buffer_size;
	format.average_bytes_per_second = k_silence_buffer_size;
	format.block_align = (word)arg_0;
	format.bits_per_sample = 8;
	format.extra_size = (word)arg_1;

	description.dwSize = sizeof(description);
	description.dwBufferBytes = (DWORD)arg_1;
	description.lpwfxFormat = (LPWAVEFORMATEX)&format;
	description.lpMixBins = &mixbins;

	mixbins.dwMixBinCount = (DWORD)arg_0;
	mixbins.lpMixBinVolumePairs = &pair;
	pair.dwMixBin = 0xe;
	pair.lVolume = arg_1;


	DirectSoundCreateBuffer(&description, &buffer);

	top = (byte *)g_510800_pool_base + g_510804_pool_size;
	memory = (byte *)(((dword)top + 3) & ~3);
	aligned_size = (memory - top) + k_silence_buffer_size;
	g_510804_pool_size += aligned_size;
	function_163ba0(&g_510808_pool_checksum, &aligned_size, sizeof(aligned_size));

	memset(memory, 0, k_silence_buffer_size);
	IDirectSoundBuffer_SetBufferData(buffer, memory, k_silence_buffer_size);
	IDirectSoundBuffer_SetHeadroom(buffer, (DWORD)arg_1);
	IDirectSoundBuffer_SetVolume(buffer, -10000);
	IDirectSoundBuffer_Play(buffer, (DWORD)arg_1, (DWORD)arg_1, (DWORD)arg_0);

	*arg_2 = buffer;
}

// @retail 0x221490
void function_221490(
	LPDIRECTSOUNDBUFFER *buffer_reference)
{
    function_221491(1, 0, buffer_reference);
}

// @retail 0x2215d0
void function_2215d0(
	s_mixbin_list *list,
	s_mixbin_settings *settings,
	LPDIRECTSOUNDSTREAM stream)
{
	DSMIXBINS out_mixbins;
	DSMIXBINVOLUMEPAIR out_pairs[8];
	long values[32];
	long out_count = 0;
	dword changed_mask = 0;
	dword seen_mask = 0;
	bool changed;
	long i;

	out_mixbins.dwMixBinCount = 0;
	out_mixbins.lpMixBinVolumePairs = out_pairs;

	if (list && list->count == (long)settings->mixbins.dwMixBinCount)
	{
		long count = list->count;
		changed = false;

		for (i = 0; i < count; i++)
		{
			long mixbin = settings->pairs[i].dwMixBin;
			long volume = settings->pairs[i].lVolume;
			dword bit;

			if (list->pairs[i].dwMixBin != (dword)mixbin)
				goto set_all;

			if (list->pairs[i].lVolume != volume)
			{
				bit = 1 << mixbin;
				if (seen_mask & bit)
				{
					if (volume != values[i])
						goto set_all;
					if (changed_mask & bit)
						goto set_all;
				}
				changed_mask |= bit;
				out_count = out_mixbins.dwMixBinCount;
				changed = true;
			}

			bit = 1 << mixbin;
			if (!(seen_mask & bit))
			{
				out_pairs[out_count].lVolume = volume;
				seen_mask |= bit;
				out_pairs[out_count].dwMixBin = mixbin;
				out_count++;
				values[mixbin] = volume;
				out_mixbins.dwMixBinCount = out_count;
			}
		}

		if (changed)
			IDirectSoundStream_SetMixBinVolumes(stream, &out_mixbins);
		return;
	}

set_all:
	IDirectSoundStream_SetMixBins(stream, &settings->mixbins);
}

// @retail 0x2216f0
void function_2216f0(
	s_mixbin_list *list,
	s_mixbin_settings *settings,
	LPDIRECTSOUNDBUFFER buffer)
{
	DSMIXBINS out_mixbins;
	DSMIXBINVOLUMEPAIR out_pairs[8];
	long values[32];
	long out_count = 0;
	dword changed_mask = 0;
	dword seen_mask = 0;
	bool changed;
	long i;

	out_mixbins.dwMixBinCount = 0;
	out_mixbins.lpMixBinVolumePairs = out_pairs;

	if (list && list->count == (long)settings->mixbins.dwMixBinCount)
	{
		long count = list->count;
		changed = false;

		for (i = 0; i < count; i++)
		{
			long mixbin = settings->pairs[i].dwMixBin;
			long volume = settings->pairs[i].lVolume;
			dword bit;

			if (list->pairs[i].dwMixBin != (dword)mixbin)
				goto set_all;

			if (list->pairs[i].lVolume != volume)
			{
				bit = 1 << mixbin;
				if (seen_mask & bit)
				{
					if (volume != values[i])
						goto set_all;
					if (changed_mask & bit)
						goto set_all;
				}
				changed_mask |= bit;
				out_count = out_mixbins.dwMixBinCount;
				changed = true;
			}

			bit = 1 << mixbin;
			if (!(seen_mask & bit))
			{
				out_pairs[out_count].lVolume = volume;
				seen_mask |= bit;
				out_pairs[out_count].dwMixBin = mixbin;
				out_count++;
				values[mixbin] = volume;
				out_mixbins.dwMixBinCount = out_count;
			}
		}

		if (changed)
			IDirectSoundBuffer_SetMixBinVolumes(buffer, &out_mixbins);
		return;
	}

set_all:
	IDirectSoundBuffer_SetMixBins(buffer, &settings->mixbins);
}

/* a sound class of the sound classes tag. Retail calls it out of line
   everywhere, but moving it into a file of its own built /Ob1 (which matched
   0x218d30) changed 0x189fe0's convention and lost 0x189650 and 0x189760 */
// @retail 0x221810
s_unknown_5c *function_221810(
	short index)
{
	return sound_class_definition_get(index);
}

// @retail 0x221850
void function_221850(void)
{
	g_502118 = (s_sound_class_fade *)function_123d40("", "", k_sound_class_count * sizeof(s_sound_class_fade));
	memset(g_502118, 0, k_sound_class_count * sizeof(s_sound_class_fade));
}

// @retail 0x2218a0
void function_2218a0(void)
{
	long i;

	g_50211c = true;
	for (i = 0; i < k_sound_class_count; i++)
	{
		g_502118[i].current = 0;
		g_502118[i].target = 0;
		g_502118[i].time = 0.0f;
	}
}

// @retail 0x2218e0
void function_2218e0(void)
{
	g_50211c = false;
}

// @retail 0x2218f0
void function_2218f0(void)
{
	g_502118 = NULL;
}

// @retail 0x221900
void function_221900(
	real delta)
{
	s_sound_class_fade *fade;
	long i;

	if (delta > 0.0f)
	{
		fade = g_502118;
		real *local_0 = &fade->time;
		for (i = 0; i < k_sound_class_count; i++, local_0 += 4)
		{
			if (*local_0 > delta)
			{
				dword target_bits = ((dword *)local_0)[-2];
				dword current_bits = ((dword *)local_0)[-1];
				real target = *(real *)&target_bits;
				real current = *(real *)&current_bits;
				real local_1 = delta / *local_0;
				real result = (target - current) * local_1 + current;

				((dword *)local_0)[-1] = *(dword *)&result;
				*local_0 = *local_0 - delta;
			}
			else
			{
				((dword *)local_0)[-1] = ((dword *)local_0)[-2];
				*local_0 = 0.0f;
			}
		}
	}
}

// @retail 0x221980
void __stdcall function_221980(
	char const *name,
	long value_bits,
	real time)
{
	char const *local_0 = name;
	long i;

	for (i = 0; i < k_sound_class_count; i++)
	{
		if (*g_470090[i] && strstr(g_470090[i], local_0))
		{
			s_sound_class_fade *fade = &g_502118[i];
			*(long volatile *)&name = value_bits;
			real value = *(real *)&name;
			dword target;

			if (value < -64.0f)
				target = 0xc2800000;
			else if (value > 0.0f)
				target = 0;
			else
				target = value_bits;

			fade->target = target;
			fade->time = time < 0.0f ? 0.0f : time;
			if (time == 0.0f)
				fade->current = target;
		}
	}
}

struct s_221a20
{
	word field_0 : 1;
	word field_1 : 15;
};

PRIVATE inline void set_fade_flag(
	s_221a20 &arg_0,
	bool arg_1)
{
	if (arg_1)
		arg_0.field_0 = true;
	else
		arg_0.field_0 = false;
}

// @retail 0x221a20
void function_221a20(
	char const *name,
	bool set)
{
	short i;

	for (i = 0; i < k_sound_class_count; ++i)
	{
		if (*g_470090[i] && strstr(g_470090[i], name))
			set_fade_flag(*(s_221a20 *)&g_502118[i].flags, !set);
	}
}

void (__stdcall *const g_221980_callbacks[])(char const *, long, real) =
{
	function_221980
};

// @retail 0x220a70
bool sound_driver_voice_create_buffer(
	long voice_index)
{
	s_sound_driver_voice *voice = &SOUND_DRIVER_GLOBALS->voices[voice_index];
	DSBUFFERDESC description = {0};
	bool result;

	description.dwSize = sizeof(description);
	description.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_MIXIN;
	if (SUCCEEDED(IDirectSound_CreateSoundBuffer(SOUND_DRIVER_GLOBALS->direct_sound, &description, &voice->buffer, NULL)))
	{
		DSMIXBINVOLUMEPAIR pairs[5];
		DSMIXBINS mixbins;

		pairs[0].dwMixBin = 6;
		pairs[0].lVolume = 0;
		pairs[1].dwMixBin = 8;
		pairs[1].lVolume = 0;
		pairs[2].dwMixBin = 7;
		pairs[2].lVolume = 0;
		pairs[3].dwMixBin = 9;
		pairs[3].lVolume = 0;
		pairs[4].dwMixBin = 10;
		pairs[4].lVolume = 0;
		mixbins.dwMixBinCount = 5;
		mixbins.lpMixBinVolumePairs = pairs;
		IDirectSoundBuffer_SetMixBins(voice->buffer, &mixbins);
		voice->unknown00 = 0x1f;
		IDirectSoundBuffer_SetMaxDistance(voice->buffer, FLT_MAX, DS3D_DEFERRED);
		IDirectSoundBuffer_SetMinDistance(voice->buffer, FLT_MAX, DS3D_DEFERRED);
		IDirectSoundBuffer_SetRolloffFactor(voice->buffer, 0.0f, DS3D_DEFERRED);
		IDirectSoundBuffer_SetDopplerFactor(voice->buffer, 0.0f, DS3D_DEFERRED);
		IDirectSoundBuffer_SetConeOutsideVolume(voice->buffer, 0, DS3D_DEFERRED);
		result = true;
	}
	else
	{
		result = false;
	}
	voice->submix = NULL;
	return result;
}

// @retail 0x220b90
short sound_driver_voice_new(
	dword flags,
	dword input_mixbin)
{
	s_sound_driver_globals *globals = SOUND_DRIVER_GLOBALS;
	short voice_index = globals->voice_count;

	if (voice_index < 0x40)
	{
		bool three_d = flags & 1;
		bool play = (flags >> 1) & 1;
		s_sound_driver_voice *voice;
		DSBUFFERDESC description = {0};

		globals->voice_count = voice_index + 1;
		voice = &globals->voices[voice_index];
		description.dwSize = sizeof(description);
		description.dwFlags = DSBCAPS_FXIN2;
		description.dwInputMixBin = input_mixbin;
		if (SUCCEEDED(IDirectSound_CreateSoundBuffer(globals->direct_sound, &description, &voice->submix, NULL)))
		{
			DSMIXBINVOLUMEPAIR pairs[6];
			DSMIXBINS mixbins;

			IDirectSoundBuffer_SetVolume(voice->submix, 0);
			pairs[0].dwMixBin = 0;
			pairs[0].lVolume = 0;
			pairs[1].dwMixBin = 1;
			pairs[1].lVolume = 0;
			pairs[2].dwMixBin = 2;
			pairs[2].lVolume = 0;
			pairs[3].dwMixBin = 3;
			pairs[3].lVolume = 0;
			pairs[4].dwMixBin = 4;
			pairs[4].lVolume = 0;
			pairs[5].dwMixBin = 5;
			pairs[5].lVolume = 0;
			mixbins.dwMixBinCount = 6;
			mixbins.lpMixBinVolumePairs = pairs;
			IDirectSoundBuffer_SetMixBins(voice->submix, &mixbins);
			IDirectSoundBuffer_Play(voice->submix, 0, 0, 0);
		}
		if (three_d)
		{
			DSBUFFERDESC buffer_description = {0};

			buffer_description.dwSize = sizeof(buffer_description);
			buffer_description.dwFlags = (play ? DSBCAPS_FXIN2 : 0) + DSBCAPS_FXIN | DSBCAPS_CTRL3D;
			buffer_description.dwInputMixBin = input_mixbin;
			if (FAILED(IDirectSound_CreateSoundBuffer(SOUND_DRIVER_GLOBALS->direct_sound, &buffer_description, &voice->buffer, NULL)))
			{
				SOUND_DRIVER_GLOBALS->voice_count--;
				return NONE;
			}
			else
			{
				DSMIXBINVOLUMEPAIR pairs[5];
				DSMIXBINS mixbins;

				pairs[0].dwMixBin = 6;
				pairs[0].lVolume = 0;
				pairs[1].dwMixBin = 8;
				pairs[1].lVolume = 0;
				pairs[2].dwMixBin = 7;
				pairs[2].lVolume = 0;
				pairs[3].dwMixBin = 9;
				pairs[3].lVolume = 0;
				pairs[4].dwMixBin = 10;
				pairs[4].lVolume = 0;
				mixbins.dwMixBinCount = 5;
				mixbins.lpMixBinVolumePairs = pairs;
				IDirectSoundBuffer_SetMixBins(voice->buffer, &mixbins);
				voice->unknown00 = input_mixbin;
				IDirectSoundBuffer_SetMaxDistance(voice->buffer, FLT_MAX, DS3D_DEFERRED);
				IDirectSoundBuffer_SetMinDistance(voice->buffer, FLT_MAX, DS3D_DEFERRED);
				if (play)
					IDirectSoundBuffer_Play(voice->buffer, 0, 0, 0);
			}
		}
		else
		{
			voice->buffer = NULL;
		}
	}
	else
	{
		voice_index = NONE;
	}
	return voice_index;
}

// @retail 0x220fd0
void function_220fd0(
	s_sound_driver_volumes const *volumes)
{
	s_sound_driver_globals *globals;
	long volume;
	long i;

	(globals = SOUND_DRIVER_GLOBALS)->volume_a = PIN(volumes->volume_a, 0.0f, 1.0f);
	globals->volume_b = PIN(volumes->volume_b, 0.0f, 1.0f);
	globals->volume_c = PIN(volumes->volume_c, 0.0f, 1.0f);
	globals->volume_d = PIN(volumes->volume_d, 0.0f, 1.0f);
	globals->unknown2acc = volumes->unknown10;
	globals->unknown2ad0 = volumes->unknown14;
	g_47005c.room = sound_gain_to_volume(globals->volume_a);
	g_47005c.room_hf = sound_gain_to_volume(globals->volume_b);
	g_47005c.unknown70 = 0.0f;
	g_47005c.unknown78 = 0.0f;
	g_47005c.unknown80 = 0.25f;
	volume = sound_gain_to_volume(globals->volume_c);
	volume = PIN(volume, -10000, 0);
	g_47005c.direct = volume;
	g_47005c.direct_hf = volume;
	g_47005c.unknown84 = globals->unknown2acc;
	g_47005c.unknown88 = globals->unknown2ad0;
	for (i = 0; i < globals->voice_count; i++)
		globals->voices[i].flags &= ~2;
	g_47005c.dirty = true;
}

// @retail 0x2211e0
void sound_driver_voice_environment_set(
	long voice_index,
	real decibels)
{
	s_sound_driver_globals *globals = SOUND_DRIVER_GLOBALS;
	s_sound_driver_voice *voice = &globals->voices[voice_index];
	DSI3DL2BUFFER parameters = {0};
	DSFILTERDESC filter;
	real attenuation;

	parameters.lDirect = sound_gain_to_volume(globals->volume_a);
	parameters.lDirectHF = sound_gain_to_volume(globals->volume_b);
	parameters.flRoomRolloffFactor = 0.0f;
	parameters.Obstruction.flLFRatio = 0.0f;
	parameters.Occlusion.flLFRatio = 0.25f;
	attenuation = function_12aff0(-64.0f, 0.0f, decibels, true);
	parameters.lRoom = PIN(sound_gain_to_volume(globals->volume_c) - (long)(6400.0f - attenuation * 6400.0f), -10000, 0);
	parameters.lRoomHF = parameters.lRoom;
	parameters.Obstruction.lHFLevel = sound_gain_to_volume(1.0f - voice->unknown30);
	parameters.Occlusion.lHFLevel = sound_gain_to_volume(1.0f - voice->unknown2c);
	IDirectSoundBuffer_SetI3DL2Source(voice->buffer, &parameters, DS3D_DEFERRED);
	filter.dwMode = DSFILTER_MODE_DLS2;
	filter.dwQCoefficient = 0;
	filter.adwCoefficients[0] = sound_filter_frequency_coefficient((real)g_47005c.unknown84);
	filter.adwCoefficients[1] = sound_filter_gain_coefficient(g_47005c.unknown88);
	filter.adwCoefficients[2] = 0;
	filter.adwCoefficients[3] = 0;
	IDirectSoundBuffer_SetFilter(voice->buffer, &filter);
}

/* what the game asks of a voice each update */
struct s_sound_driver_voice_parameters
{
	byte flags;
	byte unknown01[3];
	point3f position;
	real obstruction;
	real occlusion;
	real decibels;
	real occlusion_rate;
	real obstruction_rate;
};

// @retail 0x220e00
void sound_driver_voice_update(
	long voice_index,
	s_sound_driver_voice_parameters const *parameters)
{
	s_sound_driver_globals *globals = SOUND_DRIVER_GLOBALS;
	s_sound_driver_voice *voice = &globals->voices[voice_index];
	bool force = !(voice->flags & 2) || !globals->unknown0000;
	byte flag = parameters->flags & 1;
	real occlusion = parameters->obstruction;
	real obstruction = parameters->occlusion;

	if (force ||
		!(fabs(parameters->position.x - voice->position.x) < 0.05f) ||
		!(fabs(parameters->position.y - voice->position.y) < 0.05f) ||
		!(fabs(parameters->position.z - voice->position.z) < 0.05f))
	{
		IDirectSoundBuffer_SetPosition(voice->buffer, parameters->position.x, parameters->position.y, parameters->position.z, DS3D_DEFERRED);
		voice->position = parameters->position;
	}
	if (!force && fabs(occlusion - voice->unknown2c) < 0.001f &&
		fabs(obstruction - voice->unknown30) < 0.001f &&
		(voice->flags & 1) == flag)
	{
		goto local_0;
	}
	if (!force && (voice->flags & 1) == flag && parameters->obstruction_rate > 0.0f)
	{
		voice->unknown30 += PIN(obstruction - voice->unknown30, -parameters->obstruction_rate, parameters->obstruction_rate);
	}
	else
	{
		voice->unknown30 = obstruction;
	}
	if (!force && (voice->flags & 1) == flag && parameters->occlusion_rate > 0.0f)
	{
		voice->unknown2c += PIN(occlusion - voice->unknown2c, -parameters->occlusion_rate, parameters->occlusion_rate);
	}
	else
	{
		voice->unknown2c = occlusion;
	}
	voice->flags ^= (voice->flags ^ flag) & 1;
	sound_driver_voice_environment_set(voice_index, parameters->decibels);
local_0:
	voice->flags |= 2;
}
