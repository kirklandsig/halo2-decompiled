// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_21E330.CPP: the DirectSound driver's channels (the sound
   streams in the driver globals), the impulse buffers and the effects
   processor settings. src/unknown_221490.cpp holds the rest of retail's
   unknown_21e330.cpp. */

#include "unknown_11c920.h"
#include <xtl.h>
#include <math.h>
#include "unknown_0259d0.h"
#include <stddef.h>
#include <string.h>
#include "sound_driver.h"
#include "unknown_2ae170.h"
#include "unknown_21e230.h"

typedef char check_sound_driver_globals_size[sizeof(s_sound_driver_globals) == 0x2ad8 ? 1 : -1];
typedef char check_sound_driver_direct_sound[offsetof(s_sound_driver_globals, direct_sound) == 0x2ab0 ? 1 : -1];
typedef char check_sound_driver_reverbs[offsetof(s_sound_driver_globals, reverbs) == 0x2950 ? 1 : -1];

/* the codecs of the streams: the WMA codec (src/unknown_21ebc0.cpp views it as data)
   and the PCM codec (vtable 0x45715c; c_pcm_codec of src/unknown_2ae170.cpp, viewed
   here as data until that class has a header) */
struct s_47f0d0;
extern s_47f0d0 g_47f0d0;

struct s_47f0f0
{
	void *vtable;
};

s_47f0f0 g_47f0f0;

/* which channels are in which state, and which voices are free (0x60 bytes) */
struct s_sound_driver_channel_usage
{
	dword channels_by_state[4][4];
	long unknown40[3];
	long unknown4c[3];
	dword free_voices[2];
};

/* a curve of the impulse buffers */
struct s_tag_data;
real function_13bb90(s_tag_data const *function, real input, real range);

struct s_sound_impulse_view
{
	short index;
	short mixbin;
	byte unknown04[4];
	s_tag_data const *function;
	long identifier;
};

struct s_looping_impulse_parameters;

void __stdcall sound_driver_stream_callback(LPVOID stream_context, LPVOID arg_3e805a, DWORD status);

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define PIN(n, floor, ceiling) ((n) < (floor) ? (floor) : ((n) > (ceiling) ? (ceiling) : (n)))

static inline s_sound_stream *sound_driver_channel_get(long channel_index)
{
	return &SOUND_DRIVER_GLOBALS->channels[channel_index];
}

/* stops a channel's stream at once, dropping its chunks */
static inline void sound_driver_channel_flush(s_sound_stream *stream)
{
	stream->stream->Flush();
	stream->codec->stop();
	stream->state = 0;
	sound_stream_release_chunks(stream);
	stream->flushing = 0;
}

static inline void sound_driver_channel_halt(s_sound_stream *stream)
{
	DWORD status;

	stream->flushing = 1;
	stream->stream->GetStatus(&status);
	sound_driver_channel_flush(stream);
}

// @retail 0x21e330
long function_21e330(real decibels)
{
	long value = real_truncate((real)((exp(decibels * 0.115129255f) - 1.0f) * 8192.0f));
	if (value >= 0)
	{
		if (value > 0x7fff)
		{
			return 0x7fff;
		}
		return value;
	}
	else
	{
		long negative = 0xc000 - value;

		if (negative < 0xc000)
		{
			return 0xc000;
		}
		return negative > 0xdfff ? 0xdfff : negative;
	}
}

// @retail 0x21e3b0
void function_21e3b0(c_sound_driver_effects *effects)
{
	s_sound_driver_globals *globals = SOUND_DRIVER_GLOBALS;

	if (globals->effects)
	{
		globals->effects->dispose();
		globals = SOUND_DRIVER_GLOBALS;
	}
	globals->effects = effects;
	effects->initialize(globals->effect_levels_a, globals->effect_mixbins, globals->surround);
	SOUND_DRIVER_GLOBALS->effects->set_i3dl2(&g_47005c.room, SOUND_DRIVER_GLOBALS->surround);
}

// @retail 0x21e410
void function_21e410(long type, XBOXADPCMWAVEFORMAT *format, c_sound_stream_codec **codec)
{
	if (type != 2)
	{
		WORD channels;

		format->wfx.wFormatTag = WAVE_FORMAT_XBOX_ADPCM;
		format->wfx.wBitsPerSample = 4;
		channels = (WORD)(type == 1 ? 2.0f : 1.0f);
		format->wfx.nChannels = channels;
		format->wfx.nSamplesPerSec = 44100;
		format->wfx.cbSize = 2;
		format->wSamplesPerBlock = 64;
		format->wfx.nBlockAlign = channels * 36;
		format->wfx.nAvgBytesPerSec = format->wfx.nBlockAlign * 689;
		*codec = (c_sound_stream_codec *)&g_47f0f0;
	}
	else
	{
		format->wfx.wFormatTag = WAVE_FORMAT_PCM;
		format->wfx.nChannels = 2;
		format->wfx.nSamplesPerSec = 44100;
		format->wfx.nAvgBytesPerSec = 176400;
		format->wfx.nBlockAlign = 4;
		format->wfx.wBitsPerSample = 16;
		format->wfx.cbSize = 0;
		*codec = (c_sound_stream_codec *)&g_47f0d0;
	}
}

// @retail 0x21f4e0
bool function_21f4e0(long channel_index, long type)
{
	s_sound_stream *stream = sound_driver_channel_get(channel_index);
	c_sound_stream_codec *codec = NULL;
	DSSTREAMDESC description;
	XBOXADPCMWAVEFORMAT format;

	memset(&description, 0, sizeof(description));
	function_21e410(type, &format, &codec);
	description.lpwfxFormat = (LPWAVEFORMATEX)&format;
	description.dwFlags = 0;
	description.dwMaxAttachedPackets = 2;
	description.lpfnCallback = sound_driver_stream_callback;
	description.lpvContext = (LPVOID)channel_index;
	return sound_stream_create(stream, codec, &description, (short)type);
}

// @retail 0x220730
void __stdcall sound_driver_stream_callback(LPVOID stream_context, LPVOID arg_3e805a, DWORD status)
{
	if (arg_3e805a)
	{
		long channel_index = (long)stream_context;

		if (PIN(channel_index, 0, SOUND_DRIVER_GLOBALS->channel_count - 1) == channel_index)
		{
			s_sound_stream *stream = sound_driver_channel_get(channel_index);

			stream->codec->chunk_finished(stream, (s_sound_chunk *)arg_3e805a, status);
			if (status == XMEDIAPACKET_STATUS_SUCCESS && !(stream->flushing || stream->unknown03_3))
			{
				sound_stream_update(stream);
			}
		}
	}
}

// @retail 0x21f570
void function_21f570(void)
{
	long i;

	for (i = 0; i < 2; i++)
	{
		IDirectSoundBuffer_Pause(SOUND_DRIVER_GLOBALS->impulse_buffers[i], DSBPAUSE_PAUSE);
	}
}

// @retail 0x21f5a0
void function_21f5a0(void)
{
	long i;

	for (i = 0; i < 2; i++)
	{
		IDirectSoundBuffer_Pause(SOUND_DRIVER_GLOBALS->impulse_buffers[i], DSBPAUSE_RESUME);
	}
}

// @retail 0x21f490
void function_21f490(long mixbin, long headroom)
{
	if (PIN(mixbin, 0, 31) == mixbin)
	{
		IDirectSound_SetMixBinHeadroom(SOUND_DRIVER_GLOBALS->direct_sound, mixbin, PIN(headroom, 0, 7));
	}
}

// @retail 0x21f430
void __stdcall function_21f430(long controller_index)
{
	s_sound_driver_globals *globals = SOUND_DRIVER_GLOBALS;
	bool paused = false;
	long i;

	for (i = 0; i < globals->channel_count; i++)
	{
		if (globals->channels[i].unknown28 == controller_index && globals->channels[i].state)
		{
			IDirectSoundStream_Pause(globals->channels[i].stream, DSSTREAMPAUSE_SYNCHPLAYBACK);
			globals = SOUND_DRIVER_GLOBALS;
			paused = true;
		}
	}
	if (paused)
	{
		IDirectSound_SynchPlayback(globals->direct_sound);
	}
}

static inline real sound_pitch_ratio(long cents)
{
	real octaves = cents / 4096.0f;

	return (real)exp(octaves * 0.693147182f);
}

// @retail 0x21f650
real function_21f650(long channel_index, long mode)
{
	s_sound_stream *stream = sound_driver_channel_get(channel_index);
	real result;

	switch (mode)
	{
	case 0:
		result = sound_pitch_ratio(stream->unknown08 + 0x11f5);
		break;
	case 1:
		result = sound_pitch_ratio(stream->unknown08 + 0x1f5);
		break;
	default:
		result = sound_pitch_ratio(stream->unknown08 + 0x95c);
		break;
	}
	return result;
}

// @retail 0x21f360
void __stdcall function_21f360(s_sound_driver_channel_usage *usage)
{
	s_sound_driver_globals *globals = SOUND_DRIVER_GLOBALS;
	long i;

	memset(usage, 0, sizeof(*usage));
	memset(usage->free_voices, 0xff, ((globals->unknown0008 + 31) >> 5) * sizeof(dword));
	for (i = 0; i < globals->channel_count; i++)
	{
		s_sound_stream *stream = &globals->channels[i];
		dword *by_state = usage->channels_by_state[stream->state];

		by_state[i >> 5] |= 1 << (i & 31);
		if (stream->unknown00 != 0xff && stream->state)
		{
			char voice_index = stream->unknown00;
			dword *free_voices = usage->free_voices;

			free_voices[voice_index >> 5] &= ~(1 << (voice_index & 31));
		}
	}
	i = 0;
	do
	{
		usage->unknown40[i] = globals->unknown1a0c[i];
		usage->unknown4c[i] = globals->unknown1a12[i];
		i++;
	}
	while (i < 3);
}

// @retail 0x21f5d0
void function_21f5d0(long channel_index, long offset)
{
	s_sound_stream *stream = sound_driver_channel_get(channel_index);

	sound_driver_channel_halt(stream);
	stream->unknown0c = 0xc2800000;	/* -64.0f */
	sound_stream_stop(stream);
	stream->offset = offset > 0 ? offset : 0;
	if (stream->unknown28 != NONE)
	{
		IDirectSoundStream_Pause(stream->stream, DSSTREAMPAUSE_PAUSE);
	}
	sound_stream_set_envelope(stream, 0.1f, 0.1f);
}

// @retail 0x21f290
void function_21f290(void)
{
	long i;

	for (i = 0; i < SOUND_DRIVER_GLOBALS->channel_count; i++)
	{
		s_sound_stream *stream = sound_driver_channel_get(i);

		sound_driver_channel_halt(stream);
		stream->unknown0c = 0xc2800000;	/* -64.0f */
		sound_stream_stop(stream);
	}
}

// @retail 0x21f6d0
void __stdcall function_21f6d0(dword const *levels_a, dword const *levels_b, void const *settings)
{
	s_sound_driver_globals *globals = SOUND_DRIVER_GLOBALS;

	globals->effect_levels_a[0] = levels_a[0];
	globals->effect_levels_a[1] = levels_a[1];
	globals->effect_levels_b[0] = levels_b[0];
	globals->effect_levels_b[1] = levels_b[1];
	memcpy(globals->effect_settings, settings, sizeof(globals->effect_settings));
	function_21e3b0(globals->effects);
}

// @retail 0x21f720
void __stdcall function_21f720(s_looping_impulse_parameters const *parameters_)
{
	s_sound_impulse_view const *parameters = (s_sound_impulse_view const *)parameters_;

	if (PIN(parameters->mixbin, 0, 31) == parameters->mixbin && PIN(parameters->index, 0, 1) == parameters->index)
	{
		long index = parameters->index;
		LPDIRECTSOUNDBUFFER buffer = SOUND_DRIVER_GLOBALS->impulse_buffers[index];

		if (parameters->identifier != SOUND_DRIVER_GLOBALS->impulse_ids[index])
		{
			byte *data;
			DWORD size;
			dword i;

			IDirectSoundBuffer_Pause(buffer, DSBPAUSE_PAUSE);
			IDirectSoundBuffer_Lock(buffer, 0, 1000, (LPVOID *)&data, &size, NULL, NULL, 0);
			for (i = 0; i < size; i++)
			{
				real value = function_13bb90(parameters->function, (long)i * 0.001f, 0.0f);
				long sample;
				byte level;

				value *= 255.0f;
				__asm
				{
					fld value
					fistp sample
				}
				level = (byte)sample;
				data[i] = level > 0xff ? 0xff : level;
			}
			IDirectSoundBuffer_Pause(buffer, DSBPAUSE_RESUME);
			SOUND_DRIVER_GLOBALS->impulse_ids[index] = parameters->identifier;
		}
		IDirectSoundBuffer_SetCurrentPosition(buffer, 0);
		IDirectSoundBuffer_SetVolume(buffer, 0);
	}
}

/* ---- the driver's dispose and the effect data updates ---- */

/* the WMA codec (src/unknown_2ae170.cpp), viewed for the one method used here */
class c_wma_codec
{
public:
	void release_decoder();
};

extern void *g_510c3c;

/* the voice effect settings (include/network_voice.h) */
struct s_voice_effects
{
	LPDSEFFECTIMAGEDESC description;
	short indices[15];
	word changed;
	word previous_changed;
	byte unknown26[2];
	dword effects[4][2];
};

extern s_voice_effects *g_510c90;

// @retail 0x21eae0
void function_21eae0(void)
{
	function_21f290();

	if (SOUND_DRIVER_GLOBALS->channels)
	{
		for (long i = 0; i < SOUND_DRIVER_GLOBALS->channel_count; i++)
		{
			IDirectSoundStream **stream = &SOUND_DRIVER_GLOBALS->channels[i].stream;

			if (*stream)
			{
				(*stream)->Release();
				*stream = NULL;
			}
		}
		SOUND_DRIVER_GLOBALS->channel_count = 0;
	}

	if (SOUND_DRIVER_GLOBALS->voices)
	{
		for (long i = 0; i < SOUND_DRIVER_GLOBALS->voice_count; i++)
		{
			s_sound_driver_voice *voice = &SOUND_DRIVER_GLOBALS->voices[i];

			if (voice->buffer)
			{
				IDirectSoundBuffer_Release(voice->buffer);
				voice->buffer = NULL;
			}
			if (voice->submix)
			{
				IDirectSoundBuffer_Release(voice->submix);
				voice->submix = NULL;
			}
		}
	}

	if (SOUND_DRIVER_GLOBALS->direct_sound)
	{
		IDirectSound_Release(SOUND_DRIVER_GLOBALS->direct_sound);
		SOUND_DRIVER_GLOBALS->direct_sound = NULL;
	}

	((c_wma_codec *)&g_47f0d0)->release_decoder();
	g_510c3c = NULL;
	SOUND_DRIVER_GLOBALS->unknown0000 = false;
}

/* an effect data block: the effects it applies to, then the data written at
   an offset of their state (or the high level description) */
struct s_sound_effect_data
{
	dword effect_mask;
	byte flags;
	byte unknown05[3];
	word offset;
	word size;
	byte data[4];
};

/* iterates the set bits of a mask */
struct s_bit_iterator
{
	dword mask;
	long index;
};

static inline bool bit_iterator_next(s_bit_iterator *iterator)
{
	for (dword index = iterator->index + 1; index < 32; index++)
	{
		dword bit = 1 << index;

		if (iterator->mask < bit)
			break;
		if (iterator->mask & bit)
		{
			iterator->index = index;
			return true;
		}
	}
	return false;
}

#define EFFECT_DATA(i) ((s_sound_effect_data const *)&buffer[(i) + 1])

// @retail 0x21f960
void function_21f960(dword size, dword const *buffer)
{
	if (buffer && size)
	{
		long count = (size - 4) >> 2;

		for (long i = 0; i < count; i += (EFFECT_DATA(i)->size + 0xf) >> 2)
		{
			if (!(EFFECT_DATA(i)->flags & 2))
			{
				s_bit_iterator iterator;

				iterator.mask = EFFECT_DATA(i)->effect_mask;
				iterator.index = NONE;
				while (bit_iterator_next(&iterator))
				{
					long effect_index = iterator.index;

					if (EFFECT_DATA(i)->flags & 1)
					{
						XAudioSetEffectData(effect_index, (LPCDSFX_HIGH_LEVEL_EFFECT_DESCRIPTION)EFFECT_DATA(i)->data, NULL);
					}
					else
					{
						LPDSEFFECTIMAGEDESC description = g_510c90->description;
						DSEFFECTMAP *map = &description->aEffectMaps[effect_index];

						if ((dword)(EFFECT_DATA(i)->offset + EFFECT_DATA(i)->size) <= map->dwStateSize * 4 &&
							memcmp(EFFECT_DATA(i)->data, (byte *)map->lpvStateSegment + EFFECT_DATA(i)->offset, EFFECT_DATA(i)->size) != 0)
						{
							IDirectSound_SetEffectData(SOUND_DRIVER_GLOBALS->direct_sound, effect_index, EFFECT_DATA(i)->offset,
								EFFECT_DATA(i)->data, EFFECT_DATA(i)->size, DSFX_IMMEDIATE);
						}
					}
				}
				g_510c90->changed |= (word)EFFECT_DATA(i)->effect_mask;
			}
		}
	}
}

long function_2197f0(real gain);
real function_12aff0(real lower, real upper, real value, bool clamp);

struct s_sound_random_parameter
{
	real lower;
	real upper;
	real offset;
	real variation;
};

struct s_sound_filter_parameters
{
	long mode;
	long quality;
	s_sound_random_parameter coefficients[4];
};

struct s_sound_lfo_parameters
{
	s_sound_random_parameter values[4];
};

struct s_sound_channel_parameters
{
	dword seed;
	real interpolation;
	byte unknown08[8];
	s_sound_filter_parameters const *filter;
	s_sound_lfo_parameters const *pitch_lfo;
	s_sound_lfo_parameters const *combined_lfo;
	dword effect_words[0x100];
	dword effect_data_size;
};

typedef char check_channel_effect_size_offset[offsetof(s_sound_channel_parameters, effect_data_size) == 0x41c ? 1 : -1];

real function_219880(s_sound_random_parameter const *parameter, dword *seed, real interpolation);

// @retail 0x2201f0
void function_2201f0(long channel_index, s_sound_channel_parameters const *parameters, bool force)
{
	if (parameters)
	{
		s_sound_stream *stream = sound_driver_channel_get(channel_index);
		real interpolation = parameters->interpolation;
		dword seed;
		long channel_count;
		bool filter_set = false;
		bool pitch_set = false;
		bool combined_set = false;
		seed = parameters->seed;
		channel_count = stream->channel_count;
		s_sound_filter_parameters const *filter = parameters->filter;
		if (filter)
		{
			if (!force && fabs(parameters->interpolation - *(real *)stream->unknown04) < 0.0001f)
				goto finished;
			DSFILTERDESC description = {0};
			real a = function_219880(&filter->coefficients[0], &seed, interpolation);
			real b = function_219880(&filter->coefficients[1], &seed, interpolation);
			real c = function_219880(&filter->coefficients[2], &seed, interpolation);
			real d = function_219880(&filter->coefficients[3], &seed, interpolation);
			switch (filter->mode)
			{
			case 0:
				*(volatile DWORD *)&description.dwMode = DSFILTER_MODE_PARAMEQ;
				{
					long quality = *(long volatile const *)&filter->quality;
					description.dwQCoefficient = PIN(quality, 0, 7);
				}
				description.adwCoefficients[0] = sound_filter_frequency_coefficient(a);
				description.adwCoefficients[1] = function_21e330(b);
				description.adwCoefficients[2] = sound_filter_frequency_coefficient(c);
				description.adwCoefficients[3] = function_21e330(d);
				break;
			case 1:
				description.dwMode = DSFILTER_MODE_DLS2;
				description.adwCoefficients[0] = sound_filter_frequency_coefficient(a);
				description.adwCoefficients[1] = sound_filter_gain_coefficient(b);
				description.adwCoefficients[2] = sound_filter_frequency_coefficient(c);
				description.adwCoefficients[3] = sound_filter_gain_coefficient(d);
				break;
			case 2:
				if (channel_count != 0)
					goto filter_finished;
				*(volatile DWORD *)&description.dwMode = DSFILTER_MODE_MULTI;
				{
					long quality = *(long volatile const *)&filter->quality;
					description.dwQCoefficient = PIN(quality, 0, 7);
				}
				description.adwCoefficients[0] = sound_filter_frequency_coefficient(a);
				description.adwCoefficients[1] = sound_filter_gain_coefficient(b);
				description.adwCoefficients[2] = sound_filter_frequency_coefficient(c);
				description.adwCoefficients[3] = function_21e330(d);
				break;
			default:
				goto filter_finished;
			}
			filter_set = true;
			stream->stream->SetFilter(&description);
		}
	filter_finished:
		if (force)
		{
			s_sound_lfo_parameters const *pitch = parameters->pitch_lfo;
			if (pitch)
			{
				DSLFODESC description = {0};
				description.dwLFO = 1;
				real delay = function_219880(&pitch->values[0], &seed, interpolation);
				real frequency = function_219880(&pitch->values[1], &seed, interpolation);
				real amount = function_219880(&pitch->values[2], &seed, interpolation);
				long value = real_truncate(delay * 1500.0f);
				description.dwDelay = PIN(value, 0, 0xffff);
				value = real_truncate(frequency * 43.69066619873047f);
				description.dwDelta = PIN(value, 0, 0x3ff);
				value = real_truncate(amount * 128.0f);
				description.lPitchModulation = PIN(value, -128, 127);
				stream->stream->SetLFO(&description);
				pitch_set = true;
			}
			s_sound_lfo_parameters const *combined = parameters->combined_lfo;
			if (combined)
			{
				DSLFODESC description = {0};
				description.dwLFO = 0;
				real delay = function_219880(&combined->values[0], &seed, interpolation);
				real frequency = function_219880(&combined->values[1], &seed, interpolation);
				real cutoff = function_219880(&combined->values[2], &seed, interpolation);
				real amplitude = function_219880(&combined->values[3], &seed, interpolation);
				long value = real_truncate(delay * 1500.0f);
				description.dwDelay = PIN(value, 0, 0xffff);
				value = real_truncate(frequency * 43.69066619873047f);
				description.dwDelta = PIN(value, 0, 0x3ff);
				value = real_truncate(cutoff * 16.0f);
				description.lFilterCutOffRange = PIN(value, -128, 127);
				value = real_truncate(amplitude * 16.0f);
				description.lAmplitudeModulation = PIN(value, -128, 128);
				stream->stream->SetLFO(&description);
				combined_set = true;
			}
			if (!filter_set)
			{
				DSFILTERDESC description;
				memset(&description.dwQCoefficient, 0, sizeof(description) - sizeof(description.dwMode));
				description.dwMode = DSFILTER_MODE_BYPASS;
				stream->stream->SetFilter(&description);
			}
			if (!pitch_set)
			{
				DSLFODESC description = {0};
				description.dwLFO = 1;
				stream->stream->SetLFO(&description);
			}
			if (!combined_set)
			{
				DSLFODESC description;
				memset(&description.dwDelay, 0, sizeof(description) - sizeof(description.dwLFO));
				description.dwLFO = 0;
				stream->stream->SetLFO(&description);
			}
		}
	finished:
		*(real *)stream->unknown04 = interpolation;
		function_21f960(parameters->effect_data_size, parameters->effect_words);
	}
}

real g_468834 = 1.0f;

PRIVATE inline real reverb_decibels_add(long first, long second)
{
	return (real)((double)*(real *)&first + *(real *)&second);
}

// @retail 0x21ece0
void function_21ece0(long effect_index, s_sound_driver_reverb const *reverb)
{
	DSFX_HIGH_LEVEL_EFFECT_DESCRIPTION description;
	long gain = function_2197f0(g_468834);

	description.effectType = DSFX_EFFECT_TYPE_I3DL2REVERB;
	description.I3DL2Reverb.lRoom = (long)(function_12aff0(-64.0f, 0.0f,
		reverb_decibels_add(*(long const *)&reverb->room, gain), true) * 6400.0f - 6400.0f);
	description.I3DL2Reverb.lRoomHF = (long)(function_12aff0(-64.0f, 0.0f,
		reverb_decibels_add(*(long const *)&reverb->room_hf, gain), true) * 6400.0f - 6400.0f);
	description.I3DL2Reverb.flRoomRolloffFactor = reverb->room_rolloff;
	description.I3DL2Reverb.flDecayTime = reverb->decay_time;
	description.I3DL2Reverb.flDecayHFRatio = reverb->decay_hf_ratio;
	description.I3DL2Reverb.lReflections = (long)(function_12aff0(-64.0f, 10.0f,
		reverb_decibels_add(*(long const *)&reverb->reflections, gain), true) * 7400.0f - 6400.0f);
	description.I3DL2Reverb.flReflectionsDelay = reverb->reflections_delay;
	description.I3DL2Reverb.lReverb = (long)(function_12aff0(-64.0f, 20.0f,
		reverb_decibels_add(*(long const *)&reverb->reverb, gain), true) * 8400.0f - 6400.0f);
	description.I3DL2Reverb.flReverbDelay = reverb->reverb_delay;
	description.I3DL2Reverb.flDiffusion = reverb->diffusion * 100.0f;
	description.I3DL2Reverb.flDensity = reverb->density * 100.0f;
	description.I3DL2Reverb.flHFReference = reverb->hf_reference;
	XAudioSetEffectData(effect_index, &description, NULL);
}

/* ---- the listener ---- */

/* the reverb of no environment */
const s_sound_driver_reverb g_44a2a0 =
{
	{ 0, 0, 0, 0, 0, 0x80, 0, 0 },
	-100.0f, -100.0f, 0.0f, 1.0f, 1.0f, -100.0f, 0.0f, -100.0f, 0.0f, 1.0f, 1.0f, 5000.0f
};

/* an environment the listener hears (0x18 bytes) */
struct s_sound_listener_environment
{
	s_sound_driver_reverb const *reverb;
	s_sound_driver_occlusion occlusion;
	real scale;
};

/* the listener, as the driver reads it */
struct s_sound_listener
{
	point3f position;
	vector3f forward;
	vector3f up;
	byte unknown24[0xc];
	long environment_count;
	s_sound_listener_environment const *environments;
};

static __forceinline bool sound_real_equal(real a, real b, real epsilon)
{
	return fabs(a - b) < epsilon;
}

// @retail 0x220790
void function_220790(s_sound_listener const *listener)
{
	s_sound_driver_globals *globals = SOUND_DRIVER_GLOBALS;
	long environment_count;
	long i;

	if (!globals->unknown0000)
	{
		/* the driver is y up */
		IDirectSound_SetPosition(globals->direct_sound, listener->position.x, listener->position.z, listener->position.y, DS3D_DEFERRED);
		SOUND_DRIVER_GLOBALS->listener_position = listener->position;
	}

	globals = SOUND_DRIVER_GLOBALS;
	if (!(sound_real_equal(listener->forward.i, globals->listener_forward.i, 0.05f) &&
		sound_real_equal(listener->forward.j, globals->listener_forward.j, 0.05f) &&
		sound_real_equal(listener->forward.k, globals->listener_forward.k, 0.05f) &&
		sound_real_equal(listener->up.i, globals->listener_up.i, 0.05f) &&
		sound_real_equal(listener->up.j, globals->listener_up.j, 0.05f) &&
		sound_real_equal(listener->up.k, globals->listener_up.k, 0.05f)) || !globals->unknown0000)
	{
		IDirectSound_SetOrientation(globals->direct_sound,
			listener->forward.i, listener->forward.k, listener->forward.j,
			listener->up.i, listener->up.k, listener->up.j, DS3D_DEFERRED);
		globals = SOUND_DRIVER_GLOBALS;
		globals->listener_forward = listener->forward;
		globals->listener_up = listener->up;
	}

	environment_count = MIN(listener->environment_count, k_sound_driver_reverb_count);

	for (i = 0; i < environment_count; i++)
	{
		s_sound_listener_environment const *environment = &listener->environments[i];

		if (memcmp(environment->reverb, &globals->reverbs[i], sizeof(s_sound_driver_reverb)) != 0 || !globals->unknown0000)
		{
			globals->reverbs[i] = *environment->reverb;
			globals->reverb_dirty[i] = true;
		}
		if (memcmp(&globals->occlusions[i], &environment->occlusion, sizeof(s_sound_driver_occlusion)) != 0 || !globals->unknown0000)
		{
			globals->occlusions[i] = environment->occlusion;
			globals->occlusion_dirty[i] = true;
		}
		if (!sound_real_equal(globals->reverb_scales[i], environment->scale, 0.0001f) || !globals->unknown0000)
		{
			globals->reverb_scales[i] = environment->scale;
			globals->occlusion_dirty[i] = true;
		}
	}

	for (i = environment_count; i < k_sound_driver_reverb_count; i++)
	{
		if (memcmp(&g_44a2a0, &globals->reverbs[i], sizeof(s_sound_driver_reverb)) != 0 || !globals->unknown0000)
		{
			globals->reverbs[i] = g_44a2a0;
			globals->reverb_dirty[i] = true;
		}
		if (!(fabs(globals->reverb_scales[i]) < 0.0001f) || !globals->unknown0000)
		{
			globals->reverb_scales[i] = 0.0f;
			globals->occlusion_dirty[i] = true;
		}
	}
}

/* ---- the per frame update ---- */

/* counts the resets of the audio processor (dsound.lib) */
extern "C" DWORD g_dwDirectSoundDeltaPanicCount;

void function_1915f0(void);

// @retail 0x21ec00
void function_21ec00(void)
{
	if (SOUND_DRIVER_GLOBALS->unknown2ad4 != g_dwDirectSoundDeltaPanicCount)
	{
		/* the processor was reset: download its image and the reverbs again */
		function_1915f0();
		memset(SOUND_DRIVER_GLOBALS->reverb_dirty, true, sizeof(SOUND_DRIVER_GLOBALS->reverb_dirty));
		SOUND_DRIVER_GLOBALS->unknown2ad4 = g_dwDirectSoundDeltaPanicCount;
	}

	DirectSoundDoWork();

	for (long i = 0; i < SOUND_DRIVER_GLOBALS->channel_count; i++)
	{
		s_sound_stream *stream = sound_driver_channel_get(i);

		if (stream->state == 1)
		{
			DWORD status;

			stream->stream->GetStatus(&status);
			volatile bool stopped = !((status >> 16) & 1);
			if (stopped)
			{
				sound_driver_channel_flush(stream);
			}
		}

		/* retail keeps a jump table here whose four cases are all empty;
		   no source shape for it found yet */
		switch (stream->state)
		{
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		}
	}
}
