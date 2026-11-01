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

/* the same for a buffer */
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

// score 71 (was 71): Frame 0x80, 618-byte length, the prologue (only ESI saved before the count test)
// and the failure exit agree. Still differs: the 3D bool sits in a stack byte and the play bool in BL where
// retail keeps the 3D bool in BL and the play bool in the stack byte; with that, the EAX/ECX roles for the
// voice offset, the descriptor zero and the input mixbin are swapped (the failure exit now agrees).
// Earlier: Frame 0x80 and 618-byte length agree; early register saves and descriptor/count/flag scheduling remain.
// Earlier: the second buffer's flags are now retail's play ? DSBCAPS_FXIN2 : DSBCAPS_FXIN
// (0x100010 when play is set; the old body set 0x180010, a behaviour difference, fixed at review in lane 15
// round 13). Every 98-105 form below carried the wrong flags; with the correct select see the 155-197 list.
// Old notes: frame/length agree; early saves and descriptor zero/input scheduling differ.
// Descriptor input first 105; flags before clear 105; spatial byte 105; unsigned result 105;
// pair clear plus direct loop 185; input mixbin snapshot 105; real voice reference 98 (plain);
// pair clear plus descriptor input first 183.
// Early saves and descriptor zero/input scheduling still differ.
// Spatial-table/both plain loops 98/98; first/second/both complete pair copies
// 230/186/264; native first/second/both descriptors 136/112/136.
// First/second/both pair aggregates 125/132/160; scoped mixbin aggregates 120;
// complete descriptor aggregates 229; reused descriptor 171; direct count increment 98;
// success-path result assignments 110 (caller differs).
// early saves and descriptor zero/input scheduling remain;
// voice before modes/result after count/next count word/spatial signed byte 98 each;
// spatial word 151; play signed byte 98; spatial word with play byte 151; play long 101.
// the old body set the second buffer's flags to 0x180010 when play was set;
// retail computes play ? DSBCAPS_FXIN2 : DSBCAPS_FXIN (0x100010). The 98 allocation needs
// 0x100000 used twice (first buffer and the select): the correct select 183, or
// (play ? FXIN : 0) + FXIN 183. With the correct select: index returned directly 155 (pushes
// sink as retail's, mov ax return, but flags arrive in eax), no result 155, long result 155,
// result set after the count 159, voice_index = NONE in an else 169, failure through the
// final return 173, the plain-locals body 197, select then |= CTRL3D 175, if/else select 189,
// parameters reordered (flags last) no change.
// Round 263: first/second/both pair-volume zero chains after identifiers 109/107/118,
// before identifiers 110/107/119; shared creation status 98, with both late chains 118. All restored.
// Round 208: disjoint native descriptor phases, shared/per-phase mixbins,
// pair arrays at success/phase entry and both descriptor name orders: 98 in all eight.
// Original storage and body restored; conditional saves and zero/input residency remain.
// Round 13: retail saves only esi before the count test and gives that exit its own epilogue
// (ours shares the full one, so every save stays at entry); it also stores the new count before
// the descriptor clear. The count stored before the clear or before the voice reference 98/98;
// the count test as a guard clause returning result or NONE 164/164 (every save sinks, but flags
// then arrive in eax and the exit is placed first); the failure path returning NONE 98.
// Round 219: Native descriptor phases with direct success short/long/no sentinel result 178/178/178;
// test-assigned result 181; common short/long exit 149/180; direct success and early count,
// with/without sentinel 179/179. All restored; early saves and zero/input scheduling remain.
// Native buffer-flag ownership, separate spatial predicate at original/late producer with original/plain first-pair loop: 157/157/142/142.
// Combined actual spatial/buffer flags at original/late producer with original/plain pair loop: 187/187/181/181.
// Late-producer 142 and 181 forms lose the matched caller; restored. Keep only original-producer buffer flags used by both the descriptor and playback test, ordinary 157.
// Original inherited storage, first six-bin statements, corrected SDK values and every call tuple remain.
// Round 23: `bool play` declared before `three_d` with the select in the 3D branch and `if (play)` 100;
// result assigned before the 3D branch and reset to NONE on failure, mixbins set pointer-first 79 (kept);
// the voice reference before the two bools 78; three_d declared first 182; `voice_index = NONE` on failure
// with the result assigned after the branch 91; a voice pointer local 79; the cached globals for the second
// creation 127. LTCG harness screens: result on both success arms / `return voice_index` / long result move
// flags to EAX (100/156/154); the 3D test on `flags & 1` 166 (EAX); bools at function scope 162/163;
// the 3D test negated 127; bool spellings, byte types, names and count-store position no change.
// Round 96 (one-link harness of this file with 0x191420's, 88 there for 79): `play` copied into a block-local
// for the 3D buffer 79 (propagated away); play declared after the descriptor clear and three_d just before
// the first creation 78; both after the voice reference 78; three_d declared first at either position
// 181-182 (both bools in stack bytes, frame 0x7c); play declared just before the 3D test 173-174 (605 bytes);
// the count stored before the clear no change. No form gives three_d BL (retail's `mov bl, cl`).
// Round 52: on the 3D failure `result = NONE;` before the count decrement 72 (retail's exit: globals in
// ECX after the pops; a gain of 7, not kept), with the count stored before the descriptor clear 72, with
// a voice pointer 72; three_d defined just before the first creation 73; play defined after the first
// creation 176 (606 bytes). Screened: whenever three_d is assigned before play, voice_index takes EBP
// and both bools spill (205-218); a copy of play in the 3D branch or `(void)&play` change nothing;
// play/three_d as dword 155/75; tests on `flags & 1`/`flags & 2` 202-203; play stays in BL in every form.
// Round 35 (lane 10, fast): failure arm with result before the decrement 72 (kept with the next), plus the voice
// reference declared before the bools 71 (kept); with the 3D flag as dword/long/byte 137/137/72, dword alone 134;
// the 3D flag defined before the first creation 73, just before its test 182 (602), after the descriptor clear 80;
// count stored before the clear 72 (71 with the voice reference first); 3D flag declared before play 180 (599).
// On 71: the 3D flag spelled `(flags & 1) != 0` 71 (same code); the decrement through the `globals` local
// 146 (619). Play keeps BL in every placement of either definition.
// Round 41: `byte flags` 70 (the caller 0x191420 then differs, 1); `word flags` 162; const bools 71; a copy of
// play for the 3D buffer 71 (propagated away); the 3D buffer as a force-inline helper taking play 125
// (diagnostic: play still BL, frame grows); three_d defined first 180 (retail's `mov al, cl; shr ecx, 1;
// and cl, 1` shape, but voice_index takes EBP, the zero EBX and both bools spill); three_d first ahead of
// the voice reference 181. Retail computes play destructively in ECX, so flags' last use is play.
// Round 37 (lane 10): retail computes the 3D flag first (byte copy `mov bl, cl`, play in place in ECX),
// so the 3D flag is defined first in the source; then the voice index (with the result coalesced into
// it) outranks it for the last callee-saved register. 3D flag defined first 180 (599 bytes: the voice
// index takes EBP, the zero EBX, both bools spill), with the count stored before the clear 180; that with
// no result (two returns) 164 (flags arrive in EAX, caller 0x191420 loses); that with a long result
// 161 (618: the 3D flag takes BL and play the stack byte as retail, but the result becomes a separate
// sign-extended copy spilled after the range test); flags as a byte parameter 70 (caller loses: xor cl, cl).
// Compile-only screens (about 60 forms, 3D flag first): voice index or result read/assigned anywhere,
// globals local dropped, voice pointer, natural stack locals, bool/flags spellings and widths, a second
// test of the 3D flag: the voice index or input mixbin always outranks the 3D flag unless the result is
// a separate long. Play first keeps BL whatever else changes.
// Round 118 (lane 5, fast with 0x191420, plain check): the anonymous storage struct (with its union and
// reference aliases) replaced by plain locals: the six pairs and the mix bins at the block top, the
// first descriptor in its own block and the 3D pairs before the 3D descriptor 71 (retail's 0x80 frame
// and every slot offset; kept). Plain locals with both descriptors at block level 132 (frame 0x70); the
// first descriptor at block level after the pairs and mix bins 110 (frame 0x98); the count stored before
// the descriptor block 71 (scheduling only); the 3D buffer as a force-inline helper taking play, with its
// own pairs, mix bins and descriptor 104 (frame 0x88, play still BL).
// Round 101 (lane 8; exact LTCG harness with 0x191420's object, harness scale 65 = 71): retail reloads
// play into BL right after the 3D test (play's range split once the 3D bool dies). play built by
// `= false; if (flags & 2) play = true;` 156 (both bools memory-homed); the 3D bool built that way 142;
// both 163; the select as `play ? FXIN2 | CTRL3D : FXIN | CTRL3D` 141, as CTRL3D then `|=` 65 (same
// code), as a default then `if (play)` 142, with if-built play 152; `(flags >> 1) & 1` 65; both `!= 0` 65.
// Round 60 (m3 lane 9, fast with 0x191420): the 3D flag defined first with the count stored before the
// descriptor clear 181 (599 bytes); both bools in one declaration, play first 71 (same code), the 3D flag
// first 182 (599); the result declared after the voice index 71 (same code), with the 3D flag first 182;
// the 3D flag before the voice reference and play after it 183 (599).
// Round 42 (lane 11, fast): the 3D flag defined first 182 (599 bytes: the voice index takes EBP, the zero
// EBX, both bools spill), with a long result 158 (618: bools as retail, the result a sign-extended copy
// stored after the range test), plus the count stored before the clear 158; the bools before the voice
// reference with the count stored before the clear 72; the 3D flag first with the voice index returned
// in the block (NONE outside, NONE stored on the 3D failure) 154 (628 bytes; 0x191420 1). Compile-only
// (mixbin passed on the stack as here): 288 orders of the voice reference, bools, mix-bin locals, top
// declarations, count store and failure-arm order (floor 64 against 66, all play first); result as
// short/long/word/dword/char, placed at the start/middle/end, with both bool spellings (3D first): the
// voice index, the input mixbin or play always outranks the 3D flag for EBX; renaming the bools and the
// voice index changes nothing.
// Round 65 (lane 12, fast with 0x191420): round 37's 3D-first long-result form rebuilt 158 (618 bytes:
// retail's BL/stack-byte bool homes, but the result is a separate sign-extended copy where retail stores
// the voice index's own zero-extended register); that with the result taken as `(word)voice_index` 155
// (the voice index then takes EBP); with the short result: `voice_count++` for the count 182 (599
// bytes), the result declared after the voice index 182, play as `(flags >> 1) & 1` 182, both flags
// defined before the voice reference 183 (599): the voice index takes EBP and both flags spill.
// Round 58 (lane 16, fast): three_d declared first with the result declared after the voice index 182 (599
// bytes: the voice index takes EBP, both bools spill); the voice index as a long 161; the globals local
// dropped (the global at each use) with three_d first 182 (599), with play first 71 (same code); three_d
// first with the 3D test negated (NULL arm first) 182 (599).
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
		s_sound_driver_voice &voice = globals->voices[voice_index];
		bool play = (flags & 2) != 0;
		bool three_d = flags & 1;
		DSMIXBINVOLUMEPAIR pairs[6];
		DSMIXBINS mixbins;

		{
			DSBUFFERDESC description;
			memset(&description, 0, sizeof(description));

			globals->voice_count = voice_index + 1;
			description.dwSize = sizeof(description);
			description.dwFlags = DSBCAPS_FXIN2;
			description.dwInputMixBin = input_mixbin;
			if (SUCCEEDED(IDirectSound_CreateSoundBuffer(globals->direct_sound, &description, &voice.submix, NULL)))
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
			if (SUCCEEDED(IDirectSound_CreateSoundBuffer(SOUND_DRIVER_GLOBALS->direct_sound, &buffer_description, &voice.buffer, NULL)))
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
