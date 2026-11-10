// @flags /O2 /arch:SSE /Gr
#include "pool_storage.h"
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

/* updates a stream's mix bin volumes in place when only the volumes changed,
   otherwise sets its mix bins afresh */
// @retail 0x2215d0
void function_2215d0(
	s_mixbin_list *list,
	s_mixbin_settings *settings,
	LPDIRECTSOUNDSTREAM stream)
{
	DSMIXBINS out_mixbins;
	DSMIXBINVOLUMEPAIR out_pairs[8];
	long values[32];
	dword changed_mask;
	dword seen_mask;
	bool changed;
	long i;

	out_mixbins.dwMixBinCount = 0;
	out_mixbins.lpMixBinVolumePairs = out_pairs;
	changed_mask = 0;
	seen_mask = 0;

	if (!list)
		goto set_all;
	if (list->count != (long)settings->mixbins.dwMixBinCount)
		goto set_all;
	{
		long count = list->count;
		DSMIXBINVOLUMEPAIR *list_pairs = list->pairs;
		changed = false;

		for (i = 0; i < count; i++)
		{
			long mixbin = settings->pairs[i].dwMixBin;
			dword bit;

			if (list_pairs[i].dwMixBin != (dword)mixbin)
				goto set_all;

			long volume = settings->pairs[i].lVolume;
			if (list_pairs[i].lVolume != volume)
			{
				bit = 1 << mixbin;
				if (seen_mask & bit)
				{
					if (settings->pairs[i].lVolume != values[i])
						goto set_all;
					if (changed_mask & bit)
						goto set_all;
				}
				changed_mask |= bit;
				changed = true;
			}

			bit = 1 << mixbin;
			if (!(seen_mask & bit))
			{
				out_pairs[out_mixbins.dwMixBinCount].lVolume = volume;
				seen_mask |= bit;
				out_pairs[out_mixbins.dwMixBinCount].dwMixBin = mixbin;
				values[mixbin] = volume;
				out_mixbins.dwMixBinCount++;
			}
		}

		if (changed)
			IDirectSoundStream_SetMixBinVolumes(stream, &out_mixbins);
		return;
	}

set_all:
	IDirectSoundStream_SetMixBins(stream, &settings->mixbins);
}

/* the same for a buffer; a mismatch sets set_all instead of jumping to a
   shared call, as in 0x2aeb20 where this is inlined with no list */
// @retail 0x2216f0
void function_2216f0(
	s_mixbin_list *list,
	s_mixbin_settings *settings,
	LPDIRECTSOUNDBUFFER buffer)
{
	DSMIXBINS out_mixbins;
	DSMIXBINVOLUMEPAIR out_pairs[8];
	long values[32];
	dword changed_mask;
	dword seen_mask;
	bool changed;
	bool set_all = true;
	long i;

	out_mixbins.dwMixBinCount = 0;
	out_mixbins.lpMixBinVolumePairs = out_pairs;
	changed_mask = 0;
	seen_mask = 0;

	if (list && list->count == (long)settings->mixbins.dwMixBinCount)
	{
		set_all = false;
		long count = list->count;
		DSMIXBINVOLUMEPAIR *list_pairs = list->pairs;
		changed = false;

		for (i = 0; i < count; i++)
		{
			long mixbin = settings->pairs[i].dwMixBin;
			dword bit;

			if (list_pairs[i].dwMixBin != (dword)mixbin)
			{
				set_all = true;
				break;
			}

			long volume = settings->pairs[i].lVolume;
			if (list_pairs[i].lVolume != volume)
			{
				bit = 1 << mixbin;
				if (seen_mask & bit)
				{
					if (settings->pairs[i].lVolume != values[i] || (changed_mask & bit))
					{
						set_all = true;
						break;
					}
				}
				changed_mask |= bit;
				changed = true;
			}

			bit = 1 << mixbin;
			if (!(seen_mask & bit))
			{
				out_pairs[out_mixbins.dwMixBinCount].lVolume = volume;
				seen_mask |= bit;
				out_pairs[out_mixbins.dwMixBinCount].dwMixBin = mixbin;
				values[mixbin] = volume;
				out_mixbins.dwMixBinCount++;
			}
		}

	}

	if (set_all)
		IDirectSoundBuffer_SetMixBins(buffer, &settings->mixbins);
	else if (changed)
		IDirectSoundBuffer_SetMixBinVolumes(buffer, &out_mixbins);
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
	if (delta > 0.0f)
	{
		real volatile *class_fade_time = &g_502118->time;
		for (long i = 0; i < k_sound_class_count; i++, class_fade_time += 4)
		{
			real time_left = *class_fade_time;
			if (time_left > delta)
			{
				dword target_bits = ((dword *)class_fade_time)[-2];
				dword current_bits = ((dword *)class_fade_time)[-1];
				real target = *(real *)&target_bits;
				real current = *(real *)&current_bits;
				/* the same local holds this step's fraction */
				time_left = delta / time_left;
				real result = (target - current) * time_left + current;
				((dword *)class_fade_time)[-1] = *(dword *)&result;
				time_left = *class_fade_time - delta;
				*class_fade_time = time_left;
			}
			else
			{
				((dword *)class_fade_time)[-1] = ((dword *)class_fade_time)[-2];
				*class_fade_time = 0.0f;
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
	byte field_0[0xc];
	word field_c : 1;
	word field_d : 15;
};

PRIVATE inline void set_fade_flag(
	s_221a20 &arg_0,
	bool arg_1)
{
	if (arg_1)
		arg_0.field_c = true;
	else
		arg_0.field_c = false;
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
			{
			s_221a20 *local_0 = (s_221a20 *)&g_502118[i];
			set_fade_flag(*local_0, !set);
		}
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
	bool result = false;
	s_sound_driver_voice *voice = &SOUND_DRIVER_GLOBALS->voices[voice_index];
	LPDIRECTSOUND local_0 = SOUND_DRIVER_GLOBALS->direct_sound;
	DSBUFFERDESC description;
	memset(&description, 0, sizeof(description));

	description.dwSize = sizeof(description);
	description.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_MIXIN;
	if (SUCCEEDED(IDirectSound_CreateSoundBuffer(local_0, &description, &voice->buffer, NULL)))
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
	voice->submix = NULL;
	return result;
}

/* claims the next voice: a submix buffer reading input_mixbin, plus a 3D buffer when bit 0
   of flags is set (playing at once when bit 1 is); the voice index, or NONE when none is left */
// @retail 0x220b90
short sound_driver_voice_new(
	dword flags,
	dword input_mixbin)
{
	short result = NONE;
	s_sound_driver_globals *globals = SOUND_DRIVER_GLOBALS;
	short voice_index = globals->voice_count;

	if (voice_index < 0x40)
	{
		globals->voice_count = voice_index + 1;
		bool three_d = flags & 1;
		bool play = (flags & 2) != 0;
		s_sound_driver_voice &voice = globals->voices[voice_index];
		DSMIXBINVOLUMEPAIR pairs[6];
		DSMIXBINS mixbins;

		{
			DSBUFFERDESC description;
			memset(&description, 0, sizeof(description));

			description.dwSize = sizeof(description);
			description.dwFlags = DSBCAPS_FXIN2;
			description.dwInputMixBin = input_mixbin;
			LPDIRECTSOUND direct_sound = globals->direct_sound;
			if (SUCCEEDED(IDirectSound_CreateSoundBuffer(direct_sound, &description, &voice.submix, NULL)))
			{
				IDirectSoundBuffer_SetVolume(voice.submix, 0);
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
				mixbins.lpMixBinVolumePairs = pairs;
				mixbins.dwMixBinCount = 6;
				IDirectSoundBuffer_SetMixBins(voice.submix, &mixbins);
				IDirectSoundBuffer_Play(voice.submix, 0, 0, 0);
			}
		}
		result = voice_index;
		if (three_d)
		{
			DSMIXBINVOLUMEPAIR buffer_pairs[5];
			DSBUFFERDESC buffer_description;
			memset(&buffer_description, 0, sizeof(buffer_description));

			buffer_description.dwSize = sizeof(buffer_description);
			buffer_description.dwFlags = (play ? DSBCAPS_FXIN2 : DSBCAPS_FXIN) | DSBCAPS_CTRL3D;
			buffer_description.dwInputMixBin = input_mixbin;
			LPDIRECTSOUND direct_sound = SOUND_DRIVER_GLOBALS->direct_sound;
			if (SUCCEEDED(IDirectSound_CreateSoundBuffer(direct_sound, &buffer_description, &voice.buffer, NULL)))
			{
				buffer_pairs[0].dwMixBin = 6;
				buffer_pairs[0].lVolume = 0;
				buffer_pairs[1].dwMixBin = 8;
				buffer_pairs[1].lVolume = 0;
				buffer_pairs[2].dwMixBin = 7;
				buffer_pairs[2].lVolume = 0;
				buffer_pairs[3].dwMixBin = 9;
				buffer_pairs[3].lVolume = 0;
				buffer_pairs[4].dwMixBin = 10;
				buffer_pairs[4].lVolume = 0;
				mixbins.lpMixBinVolumePairs = buffer_pairs;
				mixbins.dwMixBinCount = 5;
				IDirectSoundBuffer_SetMixBins(voice.buffer, &mixbins);
				voice.unknown00 = input_mixbin;
				IDirectSoundBuffer_SetMaxDistance(voice.buffer, FLT_MAX, DS3D_DEFERRED);
				IDirectSoundBuffer_SetMinDistance(voice.buffer, FLT_MAX, DS3D_DEFERRED);
				if (play)
					IDirectSoundBuffer_Play(voice.buffer, 0, 0, 0);
			}
			else
			{
				result = NONE;
				SOUND_DRIVER_GLOBALS->voice_count--;
			}
		}
		else
		{
			voice.buffer = NULL;
		}
	}
	return result;
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

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// score 21 (was 21): the truncated float is kept in ecx where retail keeps it in edx, and
// the flag byte follows it. All four places are one scratch rotation that starts at the
// decibels push (ours ecx, retail eax). Tried: the room clamp as if/else if 90 (it
// branches); attenuation initialised at its declaration 21; the first barrier after the
// three float stores 27; lDirectHF after the barrier and first zero 143; the buffer in a
// local for both calls 42.
// Round 12 (fast): the scratch registers are VC's EAX -> ECX -> EDX round-robin for temporaries
// (diagnostic: one extra integer temporary before the call moves decibels to EDX, 131), so retail
// has one temporary fewer (or two more) than ours between the memset's zero and the decibels load.
// The first barrier removed 27 (the lDirectHF store sinks, decibels still ECX); `= { 0 }` in
// place of the memset 175 (the first zero store goes, 672 bytes); the 0x12aff0 call inside the
// room expression 65 (667 bytes); the voice fields through globals->voices[voice_index] 191; room
// declared at the top 21 (same code).
// local for both calls 42. The existing decibel-volume helper 65 (667 bytes); direct global reads 21;
// direct indexed voice accesses 191 (686); an independent RoomHF PIN 112 (691); a real loss local 57;
// chained filter zeros 23; the filter at first use 21; a room-base local 99. Body restored.
// Round 116: the scratch registers follow one round-robin pointer over EAX/ECX/EDX for temporaries without a
// preferred register, and the memset's zero is the one allocation ahead of the decibels load; retail's
// pointer is one step behind ours from there on (diagnostic: the memset removed gives every retail register,
// 173 at 638 bytes). The nine fields zeroed one by one 82 (the zero goes to EDI); the room as a sum then `-=`
// 99; `lRoom = lRoomHF = PIN(...)` 23; both barriers removed 28; memset of sizeof(DSI3DL2BUFFER) 21.
// Compile-only (same registers as the full build): `= { }`, memset first, the voice pointer after the memset,
// a forceinline memset wrapper and the zero-memory macro all keep the zero's allocation.
// Round 32 (lane 18, fast): the shared volume helper's clamp as `volume = PIN(volume, -6400, 0)` 184 (699 bytes;
// 0x220fd0 216); the rolloff and obstruction LF zeros chained 23; the room difference stored into lRoom and
// pinned in place 21 (same code); the 0x12aff0 call ahead of the three float stores 49 (672 bytes); the second
// barrier removed 22.
// Round 61: `__assume` on the voice after its pointer, the memset, either of the first two volume stores or before the
// 0x12aff0 call 21 each, on the globals 21, on the decibels 21, on the buffer 21; the memset's value in a named local 21;
// a voice-array local 21; the voice pointer as a sum 21; the voice pointer assigned after the memset 25.
// Compile-only (decibels stay in ecx, the truncated value in edx): a decibels local, with a pointer `__assume` before the
// call's block; locals for either or both of the first two volumes; the barrier ahead of the second store; `__assume` after
// each of the first two stores; the attenuation through a local.
// Round 105: the memset result as a multi-use environment pointer 182 (667); clearing from lDirect 21;
// both voice HF conversions before attenuation 127 (679), obstruction only / occlusion only 96/96 (672);
// nine explicit aggregate zeros 96 (662); const attenuation / const room 21/21. Original body restored.
// Round 75 (lane 1, fast): diagnostic: two dead loads anywhere between the memset and the 0x12aff0 call with both
// barriers removed differ from retail only by those two loads, so retail has no barriers and two more scratch steps (or
// a decibels load that takes no step); one dead load gives decibels EDX but every later register right. Forms that
// supply no step: the decibels forwarding helper 21 (without the first barrier 27, the second 22, both 28); forced-inline
// direct-volume or room helpers with parameters 21/27/22/28 each; a defaults helper 21; decibels declared first,
// definition and caller, 21/27/22/28. Without barriers: named gains 171 (681 bytes), gain fields named 172 (682);
// named, const or struct-held bounds, an array of bounds, decibels through a local or a struct, a four-parameter
// forwarder, decibels by const reference, a pointer local for the first stores, const references or an array pointer for
// the two volumes, the macro at the two volumes, dead zero or floor stores to the room, HF or direct fields, memset
// forwarders and a zero helper, a volume-store helper for two or four stores: 28 each. `= { 0 }` plus a barrier 192
// (667 bytes), `= { }` or memset plus a barrier 35 (without the first barrier 41); the gain helper by const reference 183.
// Round 94 (lane 16, fast): without both barriers (control 28): the globals read at each use 28; the voice pointer assigned
// after the memset 29; one volume local for the first two volumes and the pinned room 28. The volume helper with early
// returns 184 (699 bytes; 0x220fd0 204); the voice as a reference 21.
// Round 79 (fast; compile-only reproduces our registers): with no memset, or a memset of one dword (an immediate store,
// no zero register), every scratch register is retail's; a load-and-store temporary added anywhere, unused locals, and
// the zero register's placement change nothing, so the memset's zero is the one free register choice ahead of the
// decibels load. Without barriers: the room clamp as an if/else-if stored to both fields 96 (672 bytes); a named loss
// 131 (669); named obstruction/occlusion gains 91 (677); the voice from the global, `globals` after it 28; one `volume`
// local for both direct values 28; `parameters = DSI3DL2BUFFER();` 195 (684); memset ahead of globals/voice 29.
// Compile-only: deleting any other block keeps decibels ECX; a zero in a callee-saved register (an explicit nine-value
// initializer, a nine-dword loop) gives retail's later registers; memset wrappers, RtlZeroMemory/RtlFillMemory keep ours.
// Real build: the explicit nine-value initializer 211 (638 bytes: LTCG drops every zero store).
// Round 175 (fast): the volume helper with `volume = -6400` first and the conversion under `gain != 0.0f` 137 (689
// bytes, 0x220fd0 builds 199 in the reduced image), the same without both barriers 134; a named float product in the
// helper (`real decibels = log10(gain) * 2000.0f; volume = (long)decibels;`) 21 (same code), without both barriers 28;
// `real const decibels` as the parameter 21 (same code); a 600 s permuter run (66 tries) none below 21.
// @retail 0x2211e0
void sound_driver_voice_environment_set(
	long voice_index,
	real decibels)
{
	s_sound_driver_globals *globals = SOUND_DRIVER_GLOBALS;
	s_sound_driver_voice *voice = &globals->voices[voice_index];
	DSI3DL2BUFFER parameters;
	DSFILTERDESC filter;
	real attenuation;

	memset(&parameters, 0, sizeof(parameters));

	parameters.lDirect = sound_gain_to_volume(globals->volume_a);
	parameters.lDirectHF = sound_gain_to_volume(globals->volume_b);
	_ReadWriteBarrier();
	parameters.flRoomRolloffFactor = 0.0f;
	parameters.Obstruction.flLFRatio = 0.0f;
	parameters.Occlusion.flLFRatio = 0.25f;
	attenuation = function_12aff0(-64.0f, 0.0f, decibels, true);
	long room = sound_gain_to_volume(globals->volume_c) - (long)(6400.0f - attenuation * 6400.0f);
	parameters.lRoom = PIN(room, -10000, 0);
	parameters.lRoomHF = parameters.lRoom;
	parameters.Obstruction.lHFLevel = sound_gain_to_volume(1.0f - voice->unknown30);
	parameters.Occlusion.lHFLevel = sound_gain_to_volume(1.0f - voice->unknown2c);
	_ReadWriteBarrier();
	IDirectSoundBuffer_SetI3DL2Source(voice->buffer, &parameters, DS3D_DEFERRED);
	filter.dwMode = DSFILTER_MODE_DLS2;
	filter.dwQCoefficient = 0;
	filter.adwCoefficients[0] = sound_filter_frequency_coefficient((real)g_47005c.unknown84);
	filter.adwCoefficients[1] = sound_filter_gain_coefficient(g_47005c.unknown88);
	filter.adwCoefficients[2] = 0;
	filter.adwCoefficients[3] = 0;
	IDirectSoundBuffer_SetFilter(voice->buffer, &filter);
}
#pragma function(_ReadWriteBarrier)

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

/* the voice's flag byte as 0x220e00 writes it */
struct s_voice_flags_220e00
{
	byte bit0 : 1;
	byte unknown_1 : 7;
};

/* a change limited to plus or minus a rate (0x220e00) */
static __forceinline real pin_rate_220e00(real delta, real rate)
{
	return PIN(delta, -rate, rate);
}

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
		/* DirectSound's y axis is up */
		IDirectSoundBuffer_SetPosition(voice->buffer, parameters->position.x, parameters->position.z, parameters->position.y, DS3D_DEFERRED);
		voice->position = parameters->position;
	}
	if (!force && fabs(occlusion - voice->unknown2c) < 0.001f &&
		fabs(obstruction - voice->unknown30) < 0.001f &&
		(voice->flags & 1) == flag)
	{
		goto local_0;
	}
	if (!force && (voice->flags & 1) == flag && !(0.0f >= parameters->obstruction_rate))
	{
		real rate = parameters->obstruction_rate;
		real *value = &voice->unknown30;
		*value += PIN(obstruction - *value, -rate, rate);
	}
	else
	{
		voice->unknown30 = obstruction;
	}
	if (!force && (voice->flags & 1) == flag && !(0.0f >= parameters->occlusion_rate))
	{
		voice->unknown2c += pin_rate_220e00(occlusion - voice->unknown2c, parameters->occlusion_rate);
	}
	else
	{
		voice->unknown2c = occlusion;
	}
	((s_voice_flags_220e00 *)&voice->flags)->bit0 = flag;
	sound_driver_voice_environment_set(voice_index, parameters->decibels);
local_0:
	voice->flags |= 2;
}
