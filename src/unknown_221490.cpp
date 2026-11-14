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
// MiniMax S (11-10): no new form, no lower score (71); the 3D bool still sits in a stack byte and the
// play bool in BL where retail keeps the 3D bool in BL and the play bool in the stack byte, and the
// EAX and ECX roles for the voice offset are still swapped. The count stored as the block's first
// statement measures 68 again, a gain of three that the keep rule drops on a function still above 30,
// so the original body stands.
// MiniMax R (11-08): no new form, no lower score (71); the two flag bytes still take the opposite roles of
// retail's, and the EAX and ECX roles for the voice offset are still swapped.
// MiniMax I (11-06): no new form, no lower score; the two flag bytes still take the opposite roles of retail's (retail copies the flags byte into BL and keeps the play bit in the stack byte, ours keeps the play bit in BL and the 3D bit in the stack byte), and the EAX and ECX roles for the voice offset are still swapped; this round the play bit written as (flags >> 1) & 1, the 3D bit through a byte cast and the two declarations in both orders score 71, 71, 71 and 182, and the 24 statement orders of the voice, count and flag block score 68 at best, a gain of three that the keep rule drops on a function still above 30, so the original body stands.
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
// Round 28 (m3 lane 2, fast with 0x191420): play defined inside the 3D branch (only the 3D flag before it) 158
// (flags then arrive in EAX and move to EBX; 0x191420 differs, 1); the 3D test inverted (`if (!three_d)` with the
// NULL store first) 132; `long flags` (definition and 0x191420's declaration) 71 (same code); the 3D flag first,
// result assigned after the 3D branch and the failure returning NONE 186 (597 bytes); that with play first 97.
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
// Round 17 (lane 17, fast with 0x191420): the 3D flag defined first with a long result 158 (618 bytes: the
// bools take retail's BL/stack byte and the tail is retail's one instruction late; the prologue differs: the
// result is a sign-extended copy stored after the range test where retail spills the zero-extended voice
// index before it, and the count store comes after the zeroing); with a long voice index 184 (597); the
// short result assigned at the block top with the 3D flag first 182 (599: both bools spill); the long result
// at the block top 158, at the end of each success path 159, short there 182 (599); the long result at the
// block top with `result = NONE` after the decrement 155.
// Round 286: native success-scope first/spatial/both pair-array initializers 71/71/71;
// with per-phase mixbin values 74. Spatial-first counterparts for the three array families
// 182/182/182. Original arrays, descriptors, predicates and all SDK calls restored.
// Round 226: actual result acquired before capacity admission, short/word/long/dword with
// play/spatial predicate first: 160/180, 164/176, 171/171, 171/171. All disturb the matched
// caller; original result lifetime, modes, storage and complete body restored at 71.
// Round 53 (lane 10, fast with 0x191420): the voice reference indexed by `globals->voice_count` 107 (608
// bytes); the voice index starting at NONE and taking the count inside the range test 107 (608); both
// descriptors zeroed by `= { 0 }` in place of memset 161 (606); the 3D flag first with direct returns (NONE
// on both failures, the voice index on success) 146 (619, 0x191420 1); the count stored by
// `globals->voice_count++` 71 (same code). Compile-only: both buffers in a force-inline helper taking both
// flags (count and result outside) 150 (frame 0x98); the voice index starting at NONE with the 3D flag first 228.
// Round 20 (lane 18, fast with 0x191420): no voice index local (the count field read at each use, the
// result assigned after the test) with the 3D flag first 194 (589 bytes: the count takes EBP, both bools
// spill); play declared first but the 3D flag assigned first 182 (599: the assignment order decides, not
// the declaration order); the 3D block as a force-inline helper taking play, 3D flag first 182 (diagnostic);
// `short voice_index = globals->voice_count++;` with the 3D flag first 191 (589), with play first 104;
// one voice-index variable defaulting to NONE and returned, 3D flag first 194 (589).
// Round 189 (m3 lane 3, fast with 0x191420): both bools declared uninitialised play first and assigned 3D
// first 182 (599 bytes: the definition order decides, not the declaration order); declared 3D first and
// assigned play first 71 (same code); the voice index read again inside the block after the bools, 3D
// first/play first 194/108; play at function scope before the range test 183, the 3D flag there 182, both
// there after the voice index 93, both before the result 117 (0x191420 1 in three of these).
// Round 112 (lane 8, fast with 0x191420): the 3D flag declared with play and assigned later in the first
// block (retail's `and bl, 1` sits among the first creation's pushes): after the zeroing and count store 80,
// with the count stored before the zeroing 80, after the input mixbin 73, before the zeroing 71 (same
// code), after the input mixbin with the count stored first 73. Play keeps BL in all five. Permuter
// (anneal, 600 s, 67 tries): best 71.
// Round 285: destructive spatial-first flag shift with short/word/long/dword result
// 182/178/158/158; playback-first flag mask 71/75/158/158. Matched caller retained;
// original flags, result and body restored.
// Round 216: Selected count owns returned result, short/long domain with play/spatial producer order and receiver before/after predicates:
// 160/167/180/182/160/167/180/182. Original executable source restored.
// Round 331: Complete final-value descriptor initializers first/spatial/both 176/157/211;
// spatial-first counterparts 200/197/211; both with count before construction 211, also spatial-first
// 211. Original memset boundaries and executable body restored; Boolean and zero/input homes remain.
// Round 285: bounded short/word/byte/signed-byte first-pair loops, then both
// pair arrays with the real spatial-bin table 234/234/234/234/234/234/234/234. Original
// complete body restored; spatial/play split and zero/input scheduling remain.
// Round 333: Complementary nonspatial bool/byte, optional play normal/complementary
// 154/156/154/156; nonspatial-first 152/155/152/155. Caller preserved; spatial/play and descriptor
// carriers remain.
// Round 336: descriptor-owned playback bit with play/spatial producer order,
// native conditional/default field construction and nonzero/full-bit tests
// 135/148/147/144/135/148/147/144. Original complete body restored; spatial/play and zero/input
// carriers remain. Default-field forms are exact-only post-test flag forms.
// Round 338: full-width six-bin loop on native block storage, spatial-first/per-phase
// mixbins/count-before-clear subsets 71/182/71/182/71/181/71/181. Original complete body
// restored; spatial/play and descriptor zero/input homes remain.
// Round 340: shared creation HRESULT on native descriptor scopes, spatial/play
// producer order, status-selected result and entry/success status declaration combinations
// 71/182/191/192/71/182/191/192. Original body restored; spatial/play and zero/input homes remain.
// Round 313: One actual SDK descriptor reused across both current native phases, separate/reused
// pair array and shared/per-phase mixbins 82/146/82/146; spatial-first counterparts
// 180/208/180/208. Original descriptors, arrays, predicates and complete body restored;
// spatial/play and descriptor zero/input homes remain.
// Round 466: Native submix/buffer output-slot pointers, each/both at admission/SDK
// phases, then both with indexed receivers: 82/158/170/82/100/111/176/109.
// All preserve the caller but move saves earlier (+0xc); original direct outputs,
// spatial/play carriers, descriptors and full body restored.
// Round 354: Real submix/spatial output-field references and count-first mixbin stores
// subsets 1..7, then spatial-first predicates: 82/158/170/74/85/156/168/166. Original actual output
// slots, descriptors, pairs, SDK flags, modes and calls restored.
// Round 237: Success-path library array clears first/spatial/both 167/177/174; direct volume-member
// loops on the original real arrays first/spatial/both 83/81/93; both loops with
// per-phase mixbins, then spatial-first predicates 93/182. Bin-ID stores and SDK
// calls are original; all zero producers, scopes, flags and full body restored. Spatial/play and zero/input homes remain.
// Round 357: Native-frame multi-use input-bin carrier, dword/long at admission/entry with playback-first then spatial-first predicates 71/71/71/71/182/182/182/182. Original parameter observations, descriptors and body restored; Boolean/zero/input allocation remains.
// Round 137 (m3 lane 6, fast with 0x191420, which stays matched): both bools declared after the pairs and mix
// bins, play first 71 (same code), the 3D flag first 182 (599 bytes); the submix creation as a force-inline
// helper taking (globals, voice, input mixbin) with its own pairs, mix bins and descriptor, play first 71 (same
// code, frame unchanged), the 3D flag first 181 (599); the 3D flag as `(byte)flags & 1` defined first 182.
// Round 377: Bare success-owned submix/spatial pair arrays and independent mixbins,
// seven subsets 71/71/71/71/71/71/71; all with spatial-first predicates 182.
// Original arrays, zero/ID stores, descriptor scopes and complete body restored;
// spatial/play and zero/input/voice allocation remain.
// AC485: existing SDK member calls at submix/spatial creation and buffer operations,
// subsets 1..7 then all with pre-clear count publication: 82/70/82/71/82/70/82/82;
// caller stays matched. One-point form rejected; original API calls/body restored.
// Round 80 (m3 lane 12, fast with 0x191420): retail keeps the 3D flag in BL and play in its byte home (BL
// reloaded from it after the 3D test); ours the reverse. Play as `(flags >> 1) & 1` 71; the 3D flag as
// `(flags & 1) != 0` 71; no 3D local, `if (flags & 1)` 182 (602 bytes, flags folded to the caller's 0,
// 0x191420 1); the 3D test inverted with the buffer clear first 132; the result declared after the voice
// index 71; the 3D flag declared before play 182 (599 bytes).
// Lane 17 r37 (fast with 0x191420): the 3D flag as `(flags & 1) == 1` 178 (626 bytes; 0x191420 then differs, 1);
// play as `flags & 2 ? true : false` 71, as `!!(flags & 2)` 71; the 3D flag as `!!(flags & 1)` 71 (same code each);
// both bools in one declaration, 3D flag first, 182 (599). Body restored.
// Round 81 (m3 lane 16, fast with 0x191420): the 3D select written CTRL3D first 71 (same code); both bools
// tested as `!= false` 71 (same code); the NULL store as its own `if (!three_d)` before the 3D block 132; a
// complemented 2D flag 154; the count stored at the block top before the voice reference 68 (retail's early
// `lea eax, [esi + 1]` count store; the bools stay swapped, a gain of 3, not kept).
// B389: Capacity-produced allocation outcome, bool/byte/short/long with playback-first
// then spatial-first predicates: 106/106/161/162/103/103/159/160. All regress; original
// signed result, SDK phases and body restored. Spatial/play and zero/input homes remain.
// B391: Split low-byte capture then spatial mask at entry/clear/count/input boundaries,
// canonical bool then byte playback: 71/80/80/73/71/80/80/73. Original complete
// body restored; spatial/play homes and descriptor zero/input scheduling remain.
// Round 52 (m3 lane 15, fast with 0x191420): a word result with the 3D flag first 178 (603 bytes), with play first 75;
// a long result with the 3D flag first re-measured 158 (bools as retail, the result a sign-extended copy after the
// range test); both bools const with the 3D flag first 182 (599); the 3D flag first with the result assigned at the
// end of each success arm 182 (599). Whichever bool is defined first is computed from a copy of flags and the other
// in place, so retail's source defines the 3D flag first; the open question is why its short result/voice index
// then loses the last callee-saved register to the 3D flag in retail and wins it here.
// B393: Both SDK creation outcomes bool/byte/word/long, success then failure polarity,
// with original descriptor phases and modes: 71/71/71/71/71/71/71/71. NOT kept: no exact match;
// original complete body restored; spatial/play carriers and descriptor zero/input homes remain.
// B422: Complementary playback with the original positive spatial predicate, bool/byte/short/long,
// playback-first then spatial-first: 154/154/150/125/153/153/153/154. NOT kept: no byte match; all regress.
// Original complete body restored; spatial/play carriers and descriptor zero/input homes remain.
// B426: Zero-extended word/dword count bits with signed capacity/index/publication
// uses, short/long result and playback/spatial-first producers: 71/182/158/158/159/182/158/158.
// NOT kept: no byte match; original signed index, result, SDK phases and body restored.
// B428: Complete routing-pair tables with memcpy first/spatial/both 212/181/242,
// memmove first/spatial/both 247/224/294, both with spatial-first modes 250/294.
// NOT kept: no exact match; tables shorten away from retail and regress. Original
// arrays, direct stores and full body restored; spatial/play and zero/input homes remain.
// Both playback-first complete-table forms also lose 0x191420 by one instruction.
// AC538: Independent capacity admission with the original signed result retained, positive/complementary
// bool/byte/short/long: 71/71/71/71/71/71/71/71. NOT kept: unmatched decision/storage forms; original
// complete body restored.
// Spatial/play carriers and descriptor zero/input homes remain.
// M3 lane 10 r71 (fast with 0x191420; one-file harness in brackets for screens): the flags tested as `(bool)((flags >> n) & 1)`, play first 71 (same code), 3D flag first 182 (599);
// a bitfield view of the flags word, play first 71 (same code); a byte copy of the flags for both tests, 3D flag first 173 (603, 0x191420 1); both flags as bytes of a local option record,
// 3D flag first 182 (599). Reading: in the play-first form it is the flags word kept for play's late shift (`mov ebx, ecx`) that outranks the voice index, so the voice index spills as
// retail; with the 3D flag first the word dies in ECX and the voice index takes EBP, so whatever retail's source is, it must keep a register busy until the first creation.
// Round 166 (m3 lane 4, one-file harness, 80 there for 71; confirmed --fast where noted): the count stored as the block's first
// statement, ahead of the voice reference, 68 (fast, a gain of 3, not kept; retail's `lea eax, [esi + 1]; mov [edx + 6], ax`
// ahead of the clear). Reading: the bool defined first gets the byte copy, the second is shifted in place in ECX and spills, so
// retail's `mov bl, cl; shr ecx, 1; and cl, 1` is the source order 3D flag, then play (185 here, 598 bytes, the index then wins
// EBP over the 3D flag and the zero takes EBX). With that order the index still takes EBP in every form: `voice_count++`, a named
// next index, `result = voice_index` at the block top or after the voice, the index through a long, the bools at function
// scope 178, a byte copy of the flags; dword flags 160 (618). The 3D flag read again (`if (!three_d)` as a second test, in the
// select, before the play test) 185 each: they fold before allocation and add no weight. `short result = voice_index` with
// `else result = NONE` 175 (flags arrive in EAX), play first 164. The 3D block as a forced inline helper taking play 113 (620),
// frame 0x88, play keeps BL; plain inline stays out of line. Random search of 400 forms (order, spellings, byte types, count and
// voice placement, declaration order): best 73 here, the early count above.
// MiniMax A (11-05): the two flag bools in the other order 181 (the play bit is then computed eagerly from ECX as retail, but the count argument takes EBP, the frame drops to 0x7c and the flags byte copy goes to AL); the same with the count declared before the result 181, with the play bit as an unsigned char 181, with the bools ahead of the count store 180 and with a dword count 182; a byte copy of the flags 68 and the declarations reordered 68 (the same code); the 3D test inlined as `flags & 1` 180; no lower score.
// MiniMax A (11-05): the 3D test spelled `flags & 1` on its own 180 (604 bytes, the flags word then reaches the callee as a stack argument and the voice index takes EAX) and the same with the play bit computed ahead of the count 180; the 3D flag local first 181, both bools at the top of the block 180; the bools between the count store and the voice reference 172 (the play bit then reaches the stack byte and the voice offset ECX as retail, but the 3D flag goes to AL and the voice index to EBP), the play bit spelled as a mask test 68 (the same code) and the 3D flag first with the play bit from the flags word 180; no lower score.
// MiniMax A (11-05): the count stored as the block's first statement, ahead of the voice reference, and the play
// bit spelled `(flags >> 1) & 1`, keeps retail's 618-byte length and the frame, and lowers the diff to 68 (not kept: under the keep bar).
// Still differs: the play bool sits in BL and the 3D bool in a stack byte where retail keeps the 3D bool in BL
// and the play bool in the stack byte; with that, the EAX/ECX roles for the voice offset, the descriptor zero
// and the input mixbin are swapped.
// M3 lane 15 r60 (fast with 0x191420): the voice index declared inside the range-test block (test on the count field),
// 3D flag first 193 (589 bytes), play first 107 (608); the range test inverted with `result = NONE` in its own arm, 3D flag
// first 177 (602, 0x191420 then 1); both flag bools at function scope after the voice index, 3D flag first 184 (600); 3D flag
// first with the flag locals renamed (sorting first, then last), under LTCG, 182/182 (599, same code). Diagnostics, reverted:
// 3D flag first with a constant result 197 (587: the input mix bin then takes EBP and the 3D flag still spills); 3D flag first
// with a volatile voice index 157 (618: the voice index gets retail's spilled home, but the input mix bin takes EBP and the zero
// EBX, so every dword candidate outranks the byte flag, where retail's 3D flag beats the input mix bin and the voice index).
// P351: Raw playback-bit byte/word/long/dword with playback-first/spatial-first production:
// 79/152/166/166/179/165/168/168. NOT kept: no byte match; spatial/play and descriptor zero/input homes remain.
// Original complete body and existing helpers restored.
// Round 135 (m3 lane 7, fast with 0x191420, which stays matched): the 3D flag first with a word (zero-extended) voice index
// and a long result 155 (the voice index takes EBP, both bools spill); `__assume(voice_index >= 0)` after the range test 71
// (same code), with the 3D flag first 182 (599 bytes); the 3D flag first with the voice reference indexed by the count field
// 193 (589); the 3D flag first with the 3D failure decrement through the `globals` local 141 (618 bytes: globals then holds
// ESI to the end and the voice index spills as retail, but the zero takes EBX and both bools still spill).
// MiniMax D (11-06): no lower score; this round the two flag bools' declaration order swapped (182), the 3D flag
// as a byte declared first (182) and second (71, the same code), the 3D test spelled `(byte)flags & 1` (71, the
// same code), the play bit shifted instead of masked (71, the same code), a byte copy of the flags word with the
// play bit shifted (71, the same code), and the 3D local dropped so the test reads `flags & 1` (that one breaks
// the caller 0x191420, so it was reverted). Still differs: retail computes the 3D descriptor flags arithmetically
// and copies the impulse parameter address through EAX where ours copies it through EDX.
// Round 85 (m3 lane 10, fast with 0x191420): the 3D flag first with the count stored as the block's first statement 181 (598 bytes); that with the
// result assigned right after the voice reference 182 (599); the 3D flag as a `long` defined first 125 (618 bytes, but the flags arrive in EAX, play keeps
// BL and the 3D flag a dword slot; 0x191420 1); `register` on the 3D flag, play first 71 (same code), 3D flag first 182 (599); play defined inside the
// 3D block with the count stored first 150 (620, 0x191420 1). Reading unchanged: retail's order is the 3D flag (byte copy in BL), then play (in place,
// spilled), and here the voice index and the zero outrank the 3D flag's copy in that order.
// M3 lane 8 r144 (fast with 0x191420; control 71): the play bit defined inside the 3D block 158 (0x191420 then 1); explicit
// returns in place of the result local, play first 162 (626 bytes), 3D flag first 146 (619); the result assigned at the end of
// each arm with no early assignment, 3D flag first 175 (608), play first 111 (628); the count published after the submix
// block 167 (626). All but the last also lose the caller 0x191420 by one instruction. Body restored.
// Round 86 (m3 lane 1, fast with 0x191420, which stays matched; aligned/masked in brackets, base 71 [52/14]): the matched
// sibling 0x220a70's locals. The device read into a local before the first clear and used for both creations 120
// [71/17]; with the 3D flag defined first 118 [67/15] (frame 0x84). LEAD: the device local inside the first creation
// block only (one use), 3D flag first: 78 [60/9], retail's frame and `mov bl, cl; shr ecx, 1; and cl, 1` (the 3D flag
// in BL, play in the stack byte), so a value live into the first creation is what lets the 3D flag win BL; with the
// count stored as the block's first statement 70 at 619 B [57/6] (left: the count store is scheduled ahead of play's
// mask and the voice offset takes EAX, the zero ECX, the input mix bin EAX, where retail has ECX/EAX/ECX); on 70: the
// count as `voice_count++` 70 (same code), the voice as a pointer local 70 (same code), the mix-bin count stored
// before the pointer 72 (619 B); the count stored after the voice reference 72 (a late `inc esi`), stored before the
// first block 78. None kept: the one-use device local and the moved count store need a match.
// Round 67 (m3 lane 15, fast with 0x191420; aligned/masked = difflib counts with targets / registers masked, base 71
// aligned 54 masked 14): retail's 3D bool dies at its test and play is reloaded into BL there (`mov bl, [esp + 0x13]`),
// so BL goes to the one-use 3D bool for the short first range and to play for the rest. The count stored before the
// descriptor clear 71 (one store moves); the result assigned at the block top 71; both 71; the 3D flag defined first 182
// (599 bytes, aligned 122, masked 67); that with a long result 158 (618 bytes, aligned 63, masked 23: both bools take
// retail's roles, the result is a sign-extended copy stored after the range test). After the matched 0x220a70's idiom:
// both mix-bin records stored count first 74; the 3D block's direct-sound object in a local read before its zeroing 92;
// the 3D pair array and its own mix-bin record inside the success arm, count first, with that local 117 (623 bytes).
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
// Round 78 (m3 lane 14, fast; compile-only /O2 /arch:SSE /Gr reproduces every register here, two probe loads at the
// top, at the memset or after either volume give all of retail's, after the 0x12aff0 call only the later ones; with
// a 4-byte memset (immediate stores, no zero register) all are retail's, and removing or constant-folding either
// volume changes nothing, so the zero is the only rotation step ahead of the decibels load): the two buffer calls
// as the C++ methods 21 (same code); the volume clamp as an inline helper taking the truncated long, without barriers
// 175 (677 bytes); the voice through an inline getter taking globals and the index 21; decibels as an s_decibels
// parameter and field (plain; 0x220e00 and 0x220fd0 still match) 21; re-measure between round 75's one- and
// four-parameter forwarders: the 0x12aff0 call through an inline (decibels, flag) forwarder 21;
// the filter declared ahead of the buffer parameters, without barriers 28 (same as that control). Compile-only, same
// registers: forwarders with one, two (with the index) or four parameters, const globals or voice, the buffer
// parameters declared first, a dword index, the voice from the global or ahead of `globals`, a zero local for the
// memset, the memset after the first volume.
// Round 157 (m3 lane 8, fast): the existing gain-to-volume helper `__forceinline` 21 (same code; 0x220fd0 194, 571 bytes).
// Round 78 (m3 lane 15, fast; a compile-only build of this file reproduces every scratch register): without both
// barriers (control 28, re-measured): `__assume(voice->buffer != NULL)` after the voice pointer 28; `__assume(voice_index
// >= 0 && voice_index < globals->voice_count)` there 28; a count local with `__assume(voice_index < count)` 28; an empty
// `if (voice->buffer != NULL && voice->submix != NULL)` there 28; the first two gains bound to const references ahead of
// their conversions 28 (all the control's code). Reading (the no-barrier listing): from the truncation on retail is a plain
// next-fit EAX/ECX/EDX rotation (truncation ECX, mask EDX, buffer set-up EAX/ECX, coefficient EDX, EAX/ECX again) and so is
// ours one step ahead, which then skips the busy EAX at the mask; the one step is the memset's zero. Compile-only, no
// form with retail's registers and no added code: 28 dead or code-free statements after the voice pointer or the memset
// (unused loads, empty tests, inline getters, eight assume shapes); a call ahead of the decibels load (the rotation is not
// reset by calls); forwarding, reference, out-parameter and forced-inline wrappers of the first two conversions; named
// volume locals; the helper returning `int`, `dword` or `LONG`, with an `int` local, or converting through `__int64`,
// `dword`, `double` or log10f. Forms that change the code: the helper returning from both arms 205 lines for 199, a
// pinned-volume or outer-ternary helper 205/208, early returns or a PIN statement 210 (decibels EAX, the rest ours), the
// helper returning `short` (a `movsx` per use, 204 lines) gives all three of retail's registers: a widening per conversion
// is the kind of step retail has two more of.
// Round 163 (m3 lane 8, fast; 0x220fd0 builds differently in the reduced image, so helper changes need the plain
// check): a dead `= 0` initialiser on the helper's volume 21 (same code); the helper's clamp as two separate ifs 32; the
// three aggregates declared ahead of the globals and voice pointers 21 (same code); the 0x12aff0 call made before the
// memset 104 (672 bytes; decibels then takes EAX, as the rotation predicts); the first barrier moved to right after the
// memset 41.
// Also round 163: the filter declared ahead of the I3DL2 parameters (barriers kept) 21; the helper's product as
// `2000.0f * log10(gain)` 21 (plain; 0x220fd0 and 0x220e00 stay matched).
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
