// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_2AE170.CPP: streamed sounds: the WMA and PCM stream codecs, the
   DirectSound stream wrappers that feed them cached sound chunks */

#include "unknown_11c920.h"
#include <xtl.h>
#include <string.h>
#include "globals.h"
#include "unknown_01e930.h"
#include "unknown_053310.h"
#include "unknown_218850.h"
#include "unknown_2ae170.h"
#include "unknown_21e230.h"

/* the sound globals (unknown_01e930.h), viewed for their DirectSound object */
struct s_sound_globals_view
{
	byte unknown0000[0x2ab0];
	IDirectSound *direct_sound;
};

#define WMA_PACKET_SIZE 0x10000
#define k_decibels_muted_bits 0xc2800000ul /* -64.0f */
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define TEST_FLAG(flags, bit) (((flags) >> (bit)) & 1)

class c_wma_codec : public c_sound_stream_codec
{
public:
	virtual void attach(s_sound_stream *stream);
	virtual void start(s_sound_stream *stream);
	virtual bool get_packet(s_sound_stream *stream, XMEDIAPACKET *packet);
	virtual void packet_submitted(s_sound_stream *stream, XMEDIAPACKET *packet);
	virtual void stop();
	virtual bool can_submit(s_sound_stream *stream, bool submitted);

	void release_decoder();
	dword read(dword offset, dword byte_count, void **data);

	s_sound_stream *m_stream;
	XMediaObject *m_decoder;
	dword m_position;
	dword m_packet_size;
	byte *m_buffer;
	dword m_packet_status[2];
};

class c_pcm_codec : public c_sound_stream_codec
{
public:
	virtual void mark_cache_usage(s_sound_stream *stream, s_sound_cache_usage *usage);
	virtual bool get_packet(s_sound_stream *stream, XMEDIAPACKET *packet);
	virtual void packet_submitted(s_sound_stream *stream, XMEDIAPACKET *packet);
	virtual void packet_failed(s_sound_stream *stream, XMEDIAPACKET *packet);
	virtual void chunk_finished(s_sound_stream *stream, s_sound_chunk *chunk, long unused);
	virtual bool can_submit(s_sound_stream *stream, bool submitted);
};

PRIVATE DWORD CALLBACK wma_codec_read_callback(LPVOID context, DWORD offset, DWORD byte_count, LPVOID *data);

// @retail 0x2ae170
void c_wma_codec::release_decoder()
{
	if (m_decoder)
	{
		m_decoder->Release();
		m_decoder = NULL;
		m_stream->started_count = m_stream->chunk_count;
		while (m_stream->chunk_count > 0)
		{
			sound_stream_release_chunk(m_stream, m_stream->chunks[0]);
		}
	}
}

struct s_47f0d0;

// @retail 0x2ae1d0
void function_2ae1d0(s_47f0d0 *codec_view)
{
	c_wma_codec *codec = (c_wma_codec *)codec_view;
	long buffer;

	PHYSICAL_MEMORY_ALLOCATE(buffer, 2 * WMA_PACKET_SIZE);
	codec->m_buffer = (byte *)buffer;
	long packet = 0;
	do
	{
		codec->m_packet_status[packet] = 0;
		++packet;
	} while (packet < 2);
}

// @retail 0x2ae230
void c_wma_codec::attach(s_sound_stream *stream)
{
	m_stream = stream;
}

// @retail 0x2ae240
void c_wma_codec::start(s_sound_stream *stream)
{
	WAVEFORMATEX format;

	release_decoder();
	stream->started_count++;
	WmaCreateInMemoryDecoder(wma_codec_read_callback, this, 1, &format, &m_decoder);
}

// @retail 0x2ae290
bool c_wma_codec::get_packet(s_sound_stream *stream, XMEDIAPACKET *packet)
{
	bool result = false;
	long packet_index = NONE;
	long index;

	for (index = 0; index < 2; index++)
	{
		if (m_packet_status[index] != XMEDIAPACKET_STATUS_PENDING)
		{
			packet_index = index;
			break;
		}
	}

	if (packet_index != NONE)
	{
		dword total = 0;
		byte *buffer = m_buffer + (packet_index << 16);

		while (total < m_packet_size)
		{
			WAVEFORMATEX format;
			XMEDIAPACKET output;
			DWORD completed;

			output.pvBuffer = buffer + total;
			output.dwMaxSize = m_packet_size - total;
			output.pdwCompletedSize = &completed;
			output.pdwStatus = NULL;
			output.hCompletionEvent = NULL;
			output.prtTimestamp = NULL;
			if (FAILED(m_decoder->Process(NULL, &output)))
			{
				break;
			}

			m_position += completed;
			total += completed;
			if (completed < output.dwMaxSize)
			{
				release_decoder();
				m_position = 0;
			}

			if (stream->started_count >= stream->chunk_count || stream->started_count != 0)
			{
				break;
			}

			stream->started_count++;
			WmaCreateInMemoryDecoder(wma_codec_read_callback, this, 1, &format, &m_decoder);
		}

		result = total > 0;
		if (result)
		{
			packet->pvBuffer = buffer;
			packet->dwMaxSize = total;
			packet->pdwCompletedSize = NULL;
			packet->pdwStatus = &m_packet_status[packet_index];
			packet->hCompletionEvent = NULL;
			packet->prtTimestamp = NULL;
		}
	}

	return result;
}

// @retail 0x2ae3d0
void c_wma_codec::packet_submitted(s_sound_stream *stream, XMEDIAPACKET *packet)
{
	stream->offset = 0;
}

// @retail 0x2ae3e0
void c_wma_codec::stop()
{
	release_decoder();
}

// @retail 0x2ae3f0
bool c_wma_codec::can_submit(s_sound_stream *stream, bool submitted)
{
	return m_decoder && !submitted;
}

// @retail 0x2ae410
dword c_wma_codec::read(dword offset, dword byte_count, void **data)
{
	s_sound_chunk *chunk = m_stream->chunks[m_stream->started_count - 1];
	byte *chunk_data = sound_cache_chunk_get_data(chunk);
	*data = offset <= SOUND_CHUNK_SIZE(chunk) ? chunk_data + offset : NULL;

	s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(chunk->cache_index);
	if (entry->reference_count)
	{
		entry->reference_count--;
	}

	dword remaining = SOUND_CHUNK_SIZE(chunk) - offset;
	dword count = remaining > byte_count ? byte_count : remaining;
	return count > 0 ? (remaining > byte_count ? byte_count : remaining) : 0;
}

// @retail 0x2ae490
PRIVATE DWORD CALLBACK wma_codec_read_callback(LPVOID context, DWORD offset, DWORD byte_count, LPVOID *data)
{
	return ((c_wma_codec *)context)->read(offset, byte_count, data);
}

// @retail 0x2ae500
void sound_stream_add_chunk(s_sound_stream *stream, s_sound_chunk *chunk)
{
	stream->unknown03_3 = 0;
	switch (stream->state)
	{
	case 0:
	case 1:
		stream->state = 2;
		break;
	case 2:
		stream->state = 3;
		break;
	case 3:
		goto add_chunk;
	}

add_chunk:
	if (stream->chunk_count < k_maximum_stream_chunks)
	{
		function_218850(NONE, chunk, 4);
		stream->chunks[stream->chunk_count] = chunk;
		stream->chunk_count++;
	}

	if (stream->started_count == 0)
	{
		stream->codec->start(stream);
	}
}

// @retail 0x2ae590
void sound_stream_release_chunk(s_sound_stream *stream, s_sound_chunk *chunk)
{
	stream->started_count--;
	stream->chunk_count--;
	s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(chunk->cache_index);
	entry->lock_count--;
	memmove(&stream->chunks[0], &stream->chunks[1], stream->chunk_count * sizeof(s_sound_chunk *));
	memset(&stream->chunks[stream->chunk_count], 0, (k_maximum_stream_chunks - stream->chunk_count) * sizeof(s_sound_chunk *));

	switch (stream->chunk_count)
	{
	case 0:
		stream->state = 1;
		break;
	case 1:
		stream->state = 2;
		break;
	}
}

// @retail 0x2ae660
bool sound_stream_create(s_sound_stream *stream, c_sound_stream_codec *codec, DSSTREAMDESC *description, short channel_count)
{
	stream->codec = codec;
	codec->attach(stream);
	description->dwFlags |= 0x20000000;
	IDirectSound *direct_sound = ((s_sound_globals_view *)g_51ebe4)->direct_sound;
	bool success = false;
	if (SUCCEEDED(IDirectSound_CreateSoundStream(direct_sound, description, &stream->stream, NULL)))
	{
		success = true;
	}
	IDirectSoundStream_SetHeadroom(stream->stream, 0);
	stream->channel_count = channel_count == 2 ? 1 : channel_count;
	stream->chunk_count = 0;
	stream->started_count = 0;
	stream->unknown28 = NONE;
	memset(stream->chunks, 0, sizeof(stream->chunks));

	return success;
}

static inline void sound_stream_halt(s_sound_stream *stream)
{
	if (stream->state == 1)
	{
		DWORD status;
		stream->stream->GetStatus(&status);
		stream->stream->Flush();
		stream->codec->stop();
		stream->state = 0;
		sound_stream_release_chunks(stream);
		stream->flushing = 0;
	}
}

// @retail 0x2ae6f0
void sound_stream_stop(s_sound_stream *stream)
{
	sound_stream_halt(stream);
	if (stream->started_count != 0)
	{
		stream->started_count = 0;
	}
	if (stream->chunk_count != 0)
	{
		stream->chunk_count = 0;
	}
}

// @retail 0x2ae750
void sound_stream_reset(s_sound_stream *stream)
{
	sound_stream_set_envelope(stream, 0.0f, 0.25f);
	sound_stream_halt(stream);
	sound_stream_flush(stream);
}

// @retail 0x2ae7a0
void sound_stream_set_envelope(s_sound_stream *stream, real attack, real release)
{
	DSENVELOPEDESC envelope = { 0 };

	envelope.dwEG = DSEG_AMPLITUDE;
	envelope.dwMode = DSEG_MODE_ATTACK;
	envelope.dwAttack = (DWORD)(attack * 93.75f);
	envelope.dwRelease = (DWORD)(release * 93.75f);
	envelope.dwSustain = 0xff;
	IDirectSoundStream_SetEG(stream->stream, &envelope);
}

// @retail 0x2ae820
void sound_stream_update(s_sound_stream *stream)
{
	/* Retail keeps both flags in addressable stack slots. */
	bool submitted = false;
	bool const *submitted_reference = &submitted;
	bool success;
	bool ready;
	bool const *ready_reference = &ready;

	do
	{
		DWORD status;
		XMEDIAPACKET packet;

		success = false;
		if (((1 << stream->state) & ((1 << 2) | (1 << 3))) &&
			SUCCEEDED(stream->stream->GetStatus(&status)))
		{
			ready = (status & DSSTREAMSTATUS_READY) != 0;
			if (*ready_reference &&
				stream->codec->can_submit(stream, *submitted_reference) &&
				stream->codec->get_packet(stream, &packet))
			{
				success = SUCCEEDED(stream->stream->Process(&packet, NULL));
				if (success)
					stream->codec->packet_submitted(stream, &packet);
				else
					stream->codec->packet_failed(stream, &packet);
				if (success)
					submitted = true;
			}
		}
	}
	while (success);

	if (submitted)
		stream->stream->Discontinuity();
}

// @retail 0x2ae8e0
void sound_stream_release_chunks(s_sound_stream *stream)
{
	while (stream->chunk_count > 0)
	{
		s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(stream->chunks[stream->chunk_count - 1]->cache_index);
		entry->lock_count--;
		stream->chunk_count--;
	}
}

static inline void bit_vector_or_flag(dword *vector, long index)
{
	vector[index >> 5] |= 1 << (index & 0x1f);
}

// @retail 0x2ae940
void c_pcm_codec::mark_cache_usage(s_sound_stream *stream, s_sound_cache_usage *usage)
{
	for (long index = 0; index < stream->chunk_count; index++)
	{
		if (index < stream->started_count)
		{
			s_sound_chunk *chunk = stream->chunks[index];
			usage->counts[chunk->cache_index & 0xffff]++;
			bit_vector_or_flag(usage->flags, chunk->cache_index & 0xffff);
		}
	}
}

// @retail 0x2ae9d0
bool c_pcm_codec::get_packet(s_sound_stream *stream, XMEDIAPACKET *packet)
{
	s_sound_chunk *chunk = stream->chunks[stream->started_count];
	byte *data = sound_cache_chunk_get_data(chunk);

	long offset = MIN(stream->offset, MAX((long)SOUND_CHUNK_SIZE(chunk) - 0x48, 0));

	packet->pdwCompletedSize = NULL;
	packet->pdwStatus = NULL;
	packet->prtTimestamp = NULL;
	packet->pvBuffer = data + offset;
	packet->hCompletionEvent = (HANDLE)chunk;
	packet->dwMaxSize = SOUND_CHUNK_SIZE(chunk) - offset;

	return true;
}

// @retail 0x2aea50
void c_pcm_codec::packet_submitted(s_sound_stream *stream, XMEDIAPACKET *packet)
{
	stream->started_count++;
	stream->offset = 0;
}

// @retail 0x2aea80
void c_pcm_codec::packet_failed(s_sound_stream *stream, XMEDIAPACKET *packet)
{
	s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(stream->chunks[stream->started_count]->cache_index);
	if (entry->reference_count)
	{
		entry->reference_count--;
	}
}

// @retail 0x2aeac0
void c_pcm_codec::chunk_finished(s_sound_stream *stream, s_sound_chunk *chunk, long unused)
{
	sound_stream_release_chunk(stream, chunk);

	s_sound_cache_entry *entry = SOUND_CACHE_ENTRY(chunk->cache_index);
	if (entry->reference_count)
	{
		entry->reference_count--;
	}
}

// @retail 0x2aeb00
bool c_pcm_codec::can_submit(s_sound_stream *stream, bool submitted)
{
	return stream->chunk_count > stream->started_count;
}

/* ---- sound effects sends (vtable 0x457140, instance 0x47f088) ---- */

/* the effect send buffers, and the mix bins of the three sends that bypass
   them */
struct s_sound_effect_buffers
{
	long mixbin00;
	long mixbin04;
	long mixbin08;
	IDirectSoundBuffer *buffer0c;
	IDirectSoundBuffer *buffer10;
	IDirectSoundBuffer *buffer14;
	IDirectSoundBuffer *buffer18;
	IDirectSoundBuffer *buffer1c;
	IDirectSoundBuffer *buffer20;
};

/* send levels in decibels */
struct s_sound_effect_settings
{
	s_decibels level00;
	s_decibels level04;
	s_decibels level08;
	s_decibels level0c;
	s_decibels level10;
	s_decibels gain;
	real unknown18;
	s_decibels level1c;
	s_decibels level20;
	s_decibels level24;
	s_decibels level28;
	s_decibels level2c;
	byte unknown30[0x10];
};

struct s_sound_i3dl2_parameters
{
	DSI3DL2BUFFER i3dl2;
	long cutoff;
	real gain;
};

struct s_sound_send_parameters
{
	union
	{
		dword flags;
		struct
		{
			dword flag0 : 1;
			dword : 2;
			dword flag3 : 1;
			dword flag4 : 1;
			dword : 1;
			dword flag6 : 1;
			dword : 25;
		};
	};
	s_decibels level;
	s_decibels level_offset;
	s_decibels gain0c;
	s_decibels gain10;
	byte unknown14[4];
	s_decibels left_gain;
	s_decibels right_gain;
	s_decibels rear_left_gain;
	s_decibels rear_right_gain;
};

struct s_sound_mixbin_source
{
	byte unknown00[0x12];
	short mixbin;
	byte unknown14[0x1c];
	dword flags;
	word speaker_mode;
};

class c_sound_effects
{
public:
	virtual void initialize(s_sound_effect_settings *settings, s_sound_effect_buffers *buffers, bool rear);
	virtual void v1() {}
	virtual void set_i3dl2(s_sound_i3dl2_parameters *parameters, long unused);
	virtual void add_effect_sends(s_sound_send_parameters *parameters, bool rear, long mode, s_mixbin_settings *mixbins);
	virtual void add_send(long mixbin, long mode, s_decibels gain, s_mixbin_settings *mixbins);
	virtual void add_source_mixbins(s_sound_mixbin_source *source, bool rear, long mode, s_mixbin_settings *mixbins);
	virtual void add_sends(s_sound_send_parameters *parameters, long unused, long mode, s_mixbin_settings *mixbins);

	void add_effect_sends_direct(s_sound_send_parameters *parameters, bool rear, long mode, s_mixbin_settings *mixbins);
	void add_effect_sends_split(s_sound_send_parameters *parameters, bool rear, long mode, s_mixbin_settings *mixbins);

	s_sound_effect_settings m_settings;
	s_sound_effect_buffers *m_buffers;
};


struct s_mixbin_list;
void function_2216f0(s_mixbin_list *list, s_mixbin_settings *settings, LPDIRECTSOUNDBUFFER buffer);

/* Retail stream mixing expands the mix-bin append operation here. */
__forceinline void sound_stream_append_mixbin(s_mixbin_settings *settings, long mixbin, s_decibels decibels)
{
	settings->pairs[settings->mixbins.dwMixBinCount].dwMixBin = mixbin;
	settings->pairs[settings->mixbins.dwMixBinCount].lVolume = sound_decibels_to_volume(decibels);
	settings->mixbins.dwMixBinCount++;
}

// score 27 (was 281): one byte long; two store-before-push orders (mixbin 7 with the front level, mixbin 10) and a nop remain.
// Earlier 281 (was 299): Workspace frame/prologue and repeated conversion coalescing remain.
// Aligned 0x144 frame and zero-temporary/descriptor copies versus retail 0x94; repeated converter coalescing remains.
// Earlier at 299 (was 328): Workspace frame/prologue and repeated conversion coalescing remain.
// Workspace frame/prologue and repeated conversion coalescing remain.
// Earlier: frame 0x130 versus 0x94; our LTCG CSEs and hoists the identical 0x12aff0(-64, 0, -64)
// conversions (6 calls versus retail's 15; BLOCKED-BY 0x12aff0: retail's callers treat it as opaque).
// Round 75: SetMixBins (retail calls 0x37c5e4) for SetMixBinVolumes 449 (kept); the existing 0x2216f0 with a
// NULL list for the three submissions 393 (kept); explicit sends, one object 613, three objects 544; set-2 rear
// if/else 482; one reused object 483; level snapshots 416; set-1 rear ternary 391; static table 405;
// precision pragma diagnostic 624 (the conversions stay CSE'd).
// Earlier 449 (was 449): Frame 0xec versus 0x94, workspace homes and repeated-conversion residency remain.
// Earlier 449 from 463; Frame 0xdc versus 0x94; workspace overlap and repeated conversions remain.
// Frame 0xdc versus 0x94; workspace overlap and repeated conversions remain.
// Round 55: retail's 0x37c5e4 calls are SetMixBins inside an inlined 0x2216f0 with a NULL list (its dead
// count/pairs stores sit before each call), all three sets reuse one settings object, level00 is read
// into esi and level04 into a 4-byte snapshot after the copy, and there are no loops: frame 0x94 =
// settings 0x48 + 0x2216f0's pairs 0x40 and header 8 + snapshot 4. Explicit sends with one object and
// `function_2216f0(NULL, &mixbins, buffer)` 552 (frame 0x90, 0x2216f0 inlined as retail, but our LTCG
// CSEs the identical 0x12aff0(-64, 0, -64) calls: 7 calls versus retail's 15); with long-counter table
// loops 467 (call hoisted); with add_send not forceinline 549; sends through 0x21e230 826 (out of line);
// word loops with one object and 0x2216f0 447; append decibels by const reference 447.
// Frame 0xdc versus 0x94; workspace overlap and repeated conversions remain.
// Automatic routing storage and word ordinals retained; frame 0xec versus 0x94 and repeated conversions remain.
// Earlier: Frame 0xdc versus 0x94; workspace overlap and repeated conversions remain.
// Earlier 463 from 463: frame 0xdc versus 0x94 and repeated-conversion homes remain.
// First-buffer input 578; late workspaces 583; muted-send loop 576;
// native rear predicate 584; workspace role names 583; input plus loop 565
// (not kept: the loop's index formula only reproduces 6, 8, 7, 9).
// Native settings copy 574; last-two scopes 616; direct routing table 552; pair table 553;
// constant append helper 552; variable append helper 552; last workspace scope 552.
// j91: frame 0xd8 versus 0x94 and repeated constant conversions remain; shared routing storage reaches 0xa4 but loses.
// front level table 613; back level table 566; both level tables 635; shared level table 635; shared routing
// workspace 647
// First/second/both rear calls 579/532/583, first/second table pairs 542/509, second pair with call 491, both pairs 480, both pairs with call 463 (plain).
// first/third scopes reach frame 0x94 but score 490; pointer traversal 463;
// byte table 462; byte counters 458 (instruction sequence worsens); common counter 463;
// recovered-frame front/rear snapshots 546. Original loops and native workspaces kept.
// Word ordinals 464; word routing table 454; last-two scopes 498;
// front-only snapshot 502; word ordinals with first/third scopes 479 (fast).
// Fast control 453 versus full 463. Current-loop first/second scopes 497 (frame 0x94); both
// level snapshots 502; scopes with snapshots 545; unsigned ordinals 453; direct shared appends
// 463. Full frame 0xdc versus 0x94 and repeated-conversion homes remain.
// Fast control 453 versus full 463. All buffer arguments 460; input setting fields 453; both inputs
// 478; buffer record 459; reused buffer handles 487; plain table while loops 453; while/input fields
// 453; paired actual setting references 453. No preview beats its control; all restored.
// Round 361: first shared rear call on paired-table graph 488; with first/third, first/second,
// last-two and all scopes 515/514/535/572; with both repeated levels 535; levels plus first/third
// or all scopes 563/618 (fast control 453). Frame 0x94 in pair scopes but complete scores regress.
// Round 162: Actual first/second, first/third, last-two and all workspace reuse
// on current table loops 497/482/498/539; with both repeated level snapshots
// 545/530/551/598. Frame reuse does not recover conversion/state homes; all restored.
// Automatic routing table: fast 426, full 458 (five-point gain rejected alone); with first/second, first/third,
// last-two and all workspace scopes 454/460/475/520 fast; with word ordinals 436 fast, 449 plain (kept).
// Automatic table with first rear call or copied levels 456/466 fast. Table after settings copy or at first
// traversal 427/428 fast, 455/450 plain; with word ordinals 437/438 fast. Original table producer kept.
// Automatic routing word/byte/short tables with word ordinals; word/byte/short tables with long ordinals;
// long/word tables with byte ordinals: 435/427/435/424/419/424/436/435 (fast); all restored. Ordinary t05
// 455, t02 458, t04 456, t06 456; no qualifying gain.
// Automatic-table pointer walks first/second/first+second/third/first+third/second+third/all:
// 436/436/437/432/431/431/431 fast (control 436); shared table-end pointer 431.
// All pointer walks 446 plain, three-point gain rejected; original word ordinals restored.
// Round 208 reduced control 378/967, full 393/951: first/second, first/third,
// last-two and all descriptor scopes on the current submission graph
// 417/400/428/447; corresponding three pairwise object reuses 417/400/428;
// all scopes with both repeated levels 500. All scopes reach frame 0xa0;
// conversion reuse and the twelve-byte excess frame remain; original body restored.
// Round 197: Static routing table with all scopes/reuse 496/496; first-third scopes 447; both levels with reuse/scopes 531/531; rear level with reuse/scopes 470/470 (frame 0x94); first-third scopes and both levels 484 (fast control 378). Restored.
// Current submission graph: routing table after copy with original/first-second/first-third/all
// descriptor scopes 379/421/415/462 fast; first-use long/word/byte/short table 380/398/393/398
// fast (control 378, full 393). Original table producer and descriptors restored.
// Round 222: equality endpoints on all seven nonempty subsets of the three bounded word
// traversals 378 fast each; all equality endpoints with native-long ordinals 406 (fast control 378).
// Original ordinal comparisons restored; descriptor/conversion frame remains 0x130 versus 0x94.
// Round 203: descriptor lexical orders first-second-third, first-third-second, second-first-third,
// second-third-first, third-first-second and third-second-first 378 each (fast control 378/967);
// fourth order with both repeated levels 416; fourth order with existing raw append 390.
// Current inlined submission scratch still gives frame 0x130; every body form restored.
// Current submission graph: complete original buffers/settings/both observations 398/370/397 fast;
// both copied-level field references 378, with original buffers 398; native settings-copy producer
// 384, with original buffers 426; native buffer-record snapshot 390 (fast control 378).
// Original settings observations 396 plain, worse than full 393; all restored. Frame 0x130 remains.
// Round 244: early native header initialization before the member copy, first/second/both/third/
// first-third/last-two/all 381/378/375/371/368/374/374 fast; all before settings copy 374
// (control 378/967). First-third 399/952 plain versus 393/951; original phases restored.
// Round 235: Complete native descriptor initialization subsets and at-use clear [359, 348, 349, 349, 351,
// 333, 334, 338] fast; uniform all-three 336 plain kept (393 before). On that producer, pairwise/all
// descriptor scopes and one reused descriptor [370, 351, 381, 416, 426] fast; restored. Frame 0x130 and
// repeated-conversion reuse remain.
// Round 379: called shared append on first/second/third/first-second/first-third/last-two/all
// descriptors 468/469/413/609/548/541/679 fast; uniform existing raw inline append 335
// (control 334, full 336). All original calls and executable body restored.
// Round 168: Static routing table with complete-zero descriptors, original/first/second/first-second/third/first-third/last-two/all scopes 352/352/360/419/363/395/416/456 fast (control 334, full 336). All restored.
// Round 247: full CRT clears at native declaration, first/second/first-second/third/first-third/
// last-two/all 334/333/333/333/333/332/332 fast; all after member copy 336 (control 334,
// full 336). No qualifying gain; all aggregate declarations and shared initializers restored.
// Round 224: Complete native headers for first/second/first-second/third/first-third/last-two/all/
// at-use all 316/334/335/323/321/324/325/338 fast, restored as shared-initializer expansion.
// Existing initializer calls with first-use zero declarations in those seven subsets, then all after copy,
// 316/332/318/333/332/333/336/336 fast; first-only 335 plain, first-second 329 plain. On first-second,
// table after copy/first use/word/byte 317/330/347/341 fast; after-copy 329 plain. One/seven-point
// gains rejected below margin; original zero phases, table, shared helpers and all API inputs restored.
// Round 285: complete member-record memmove on complete-zero descriptors, original/first/second/first-second/third/first-third/last-two/all scopes
// 315/315/321/378/320/353/361/402 fast (control 334). Original-scope memmove 338 plain;
// third initialized scope with memmove 328 plain at 1015/1071, kept. Frame and conversion reuse remain.
// Round 191: Reduced control 320/1031 versus full 328/1015. Complete phase-zero descriptor reuse first-second/first-third/second-third/all 369/361/376/407 fast.
// Round 191: The same phase-zero reuse with the one routing table static 410/385/392/455. All reuse/static forms restored; the all-reuse static frame is 0x90 and the missing-conversion gap grows.
// J198: remove redundant inactive-pair aggregate zeros, seven native descriptor subsets
// 327/327/364/337/341/341/378; all with first-use declarations 378. Every shared initializer and active pair retained.
// Descriptor/conversion homes and repeated-conversion coalescing remain; all nonkeepers restored.
// Round 92: struct-assignment settings copy 333; front/rear levels read once after the copy into two
// locals 351; second-set rear send as if/else 329; long loop counters 337; first-set rear send as a
// ternary 342 (fast, control 320/1031). Whole-file float-consistency pragma diagnostic 328 (fast).
// Lead: retail calls 0x12aff0 with the plain /Gr fastcall convention (flag in CL, three floats
// pushed), which a non-LTCG object would also give; that would explain why retail never merges or
// hoists its 15 identical (-64, 0, -64) calls, which ours hoist out of the loops (BLOCKED-BY 0x12aff0).
// Round 248: initialized descriptor role-name permutations and two routing-table name positions
// 320 each (fast control 320, full 328); original complete body restored.
// Round 217: Complete descriptor value initialization at declaration and by assignment, first/second/
// third/all for each: 307/314/314/299/307/314/314/299. Unselected value-production forms restored.
// Selected native descriptor value production: 2aeb20-t04; original scopes and game-call inputs retained.
// Round 44 (lane 14, plain): the third set's four muted sends written out (6, 8, 7, 9) in place of the
// table loop 281 at 1071/1071 (kept; retail has no loop in any set). Plain explicit sends on one reused
// object 596 (752 bytes: 7 conversion calls against retail's 15), with front/rear levels named 627, with
// ternary rear sends 606; all three sets written out 408; on the kept form, the second set written out 323,
// the first set 300, third object declared with the others 308, third set reusing the first object 375.
// Diagnostics: 0x12aff0 alone in a /GL- file 308 (0x12aff0 itself 59: retail returns in xmm0, so it is
// LTCG code; five matched callers lost); the whole file with /Op 281. Neither is retail's cause.
// J221: On native value construction: first/second/both submission scopes 316/327/374; full-zero first/third restart 314; direct CRT zero first/second/both/all 299/299/307/348. Original value constructors, descriptors, routing and submission tuples restored.
// Lane 18 r21 (plain): memcpy settings copy 296 (retail's inline rep movsd; below margin); memcpy with
// undeclared-zero objects 393 (951 B); memcpy with the second rear send as if/else 317; memcpy with
// front/rear level locals 315; memcpy with explicit sends 412 (941 B). Without the zeroing our LTCG
// reuses the first set's (-64, 0, -64) conversion in sets 2 and 3 across the SetMixBins/SetVolume
// calls (2 and 1 calls there, retail 5 and 5): it treats 0x12aff0 as pure, retail as opaque.
// Round 50 (lane 13, fast): written from retail's three sets on the decibel struct (one settings object, explicit
// sends, no loops): named muted local 259 (1088 B); literal at each send 464 (1264 B, 13 calls as retail, the
// constant through movss); aggregate constants on a constructor-less struct 276; integer bits passed as the
// struct 27 (1072 B; retail's `mov eax, 0xc2800000`), kept through the bits constructor.
// On that form: sends through add_send with mode 0 27; rear level declared first 27; both levels declared
// apart 27; a named zero 27; the front level copied at its second send 27; that send on the settings field
// 293 (1071 B; EBX taken for the settings copy). The remaining byte is a nop before the third SetMixBins call.
// @retail 0x2aeb20
void c_sound_effects::initialize(s_sound_effect_settings *settings, s_sound_effect_buffers *buffers, bool rear)
{
	s_mixbin_settings mixbins;

	m_buffers = buffers;
	m_settings = *settings;
	s_decibels front_level = m_settings.level00;
	s_decibels rear_level = m_settings.level04;

	sound_mixbins_initialize(&mixbins);
	sound_stream_append_mixbin(&mixbins, 6, front_level);
	if (rear)
		sound_stream_append_mixbin(&mixbins, 8, rear_level);
	else
		sound_stream_append_mixbin(&mixbins, 8, s_decibels(k_decibels_muted_bits));
	sound_stream_append_mixbin(&mixbins, 7, s_decibels(k_decibels_muted_bits));
	sound_stream_append_mixbin(&mixbins, 9, s_decibels(k_decibels_muted_bits));
	function_2216f0(NULL, &mixbins, m_buffers->buffer0c);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer0c, 0);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer18, -6400);

	sound_mixbins_initialize(&mixbins);
	sound_stream_append_mixbin(&mixbins, 6, s_decibels(k_decibels_muted_bits));
	sound_stream_append_mixbin(&mixbins, 8, s_decibels(k_decibels_muted_bits));
	sound_stream_append_mixbin(&mixbins, 7, front_level);
	if (rear)
		sound_stream_append_mixbin(&mixbins, 9, rear_level);
	else
		sound_stream_append_mixbin(&mixbins, 9, s_decibels(k_decibels_muted_bits));
	function_2216f0(NULL, &mixbins, m_buffers->buffer10);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer10, 0);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer1c, -6400);

	sound_mixbins_initialize(&mixbins);
	sound_stream_append_mixbin(&mixbins, 6, s_decibels(k_decibels_muted_bits));
	sound_stream_append_mixbin(&mixbins, 8, s_decibels(k_decibels_muted_bits));
	sound_stream_append_mixbin(&mixbins, 7, s_decibels(k_decibels_muted_bits));
	sound_stream_append_mixbin(&mixbins, 9, s_decibels(k_decibels_muted_bits));
	sound_stream_append_mixbin(&mixbins, 10, s_decibels(0ul));
	function_2216f0(NULL, &mixbins, m_buffers->buffer20);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer20, 0);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer14, -6400);
}

/* the level of the effect sends: none in mode 0 */
inline s_decibels sound_effects_send_level(c_sound_effects *effects, s_sound_send_parameters *parameters, long mode)
{
	return mode == 1 ? (TEST_FIELD_BIT(parameters->flag6) ? g_44f70c : effects->m_settings.level2c) : effects->m_settings.level10;
}

// @retail 0x2aefe0
void c_sound_effects::add_effect_sends_direct(s_sound_send_parameters *parameters, bool rear, long mode, s_mixbin_settings *mixbins)
{
	if ((bool)((parameters->flags >> 2) & 1))
	{
		bool alternate = (bool)((parameters->flags >> 3) & 1);
		long left = alternate ? 0 : 6;
		long right = alternate ? 1 : 7;
		s_decibels level = rear ? m_settings.level1c : m_settings.level24;
		s_decibels level2 = rear ? m_settings.level20 : m_settings.level28;
		sound_stream_append_mixbin(mixbins, left, level + parameters->level);
		sound_stream_append_mixbin(mixbins, right, level + parameters->level);
		add_send(2, mode, level2 + parameters->level, mixbins);
		add_send(10, mode, parameters->level_offset, mixbins);
	}
	else
	{
		s_decibels send_level = sound_effects_send_level(this, parameters, mode);
		if (!TEST_FIELD_BIT(parameters->flag3))
		{
			s_decibels level = parameters->level;
			s_decibels offset = level;
			offset += parameters->level_offset;
			long mixbin00 = m_buffers->mixbin00;
			long mixbin04 = m_buffers->mixbin04;
			long mixbin08 = m_buffers->mixbin08;
			sound_stream_append_mixbin(mixbins, mixbin00, parameters->gain0c + (send_level + level));
			sound_stream_append_mixbin(mixbins, mixbin04, parameters->gain10 + (send_level + level));
			switch (mode)
			{
			case 0:
				sound_mixbins_add(mixbins, mixbin08, offset);
				break;
			case 1:
				sound_mixbins_add(mixbins, mixbin08, s_decibels(offset) += m_settings.gain);
				sound_mixbins_add(mixbins, mixbin08, s_decibels(offset) += m_settings.gain);
				break;
			}
		}
		else
		{
			s_decibels level = parameters->level;
			s_decibels offset = parameters->level_offset + level;
			if (m_settings.level00 > -64.0f)
			{
				sound_mixbins_add(mixbins, 0, parameters->gain0c + (m_settings.level00 + (send_level + level)));
				sound_mixbins_add(mixbins, 1, parameters->gain10 + (m_settings.level00 + (send_level + level)));
			}
			if (rear && m_settings.level04 > -64.0f)
			{
				sound_mixbins_add(mixbins, 4, parameters->gain0c + (m_settings.level04 + (send_level + level)));
				sound_mixbins_add(mixbins, 5, parameters->gain10 + (m_settings.level04 + (send_level + level)));
			}
			switch (mode)
			{
			case 0:
				sound_mixbins_add(mixbins, 10, offset);
				break;
			case 1:
				sound_mixbins_add(mixbins, 10, parameters->gain0c + offset);
				sound_mixbins_add(mixbins, 10, parameters->gain10 + offset);
				break;
			}
		}
	}
}

// @retail 0x2af450
void c_sound_effects::add_effect_sends_split(s_sound_send_parameters *parameters, bool rear, long mode, s_mixbin_settings *mixbins)
{
	s_decibels send_level = sound_effects_send_level(this, parameters, mode);
	s_decibels offset = parameters->level_offset;
	bool alternate = TEST_FIELD_BIT(parameters->flag3);
	long left = alternate ? 0 : 6;
	long right = alternate ? 1 : 7;
	long rear_left = alternate ? 4 : 8;
	long rear_right = alternate ? 5 : 9;
	s_decibels level = parameters->level;

	if (m_settings.level08 > -64.0f)
	{
		sound_stream_append_mixbin(mixbins, left, parameters->gain0c + (m_settings.level08 + (send_level + level)));
		sound_stream_append_mixbin(mixbins, right, parameters->gain10 + (m_settings.level08 + (send_level + level)));
	}
	if (rear && m_settings.level0c > -64.0f)
	{
		sound_stream_append_mixbin(mixbins, rear_left, parameters->gain0c + (m_settings.level0c + (send_level + level)));
		sound_stream_append_mixbin(mixbins, rear_right, parameters->gain10 + (m_settings.level0c + (send_level + level)));
	}
	switch (mode)
	{
	case 0:
		sound_stream_append_mixbin(mixbins, 10, offset);
		break;
	case 1:
		sound_stream_append_mixbin(mixbins, 10, parameters->gain0c + offset);
		sound_stream_append_mixbin(mixbins, 10, parameters->gain10 + offset);
		break;
	}
}

// @retail 0x2af7b0
void c_sound_effects::add_effect_sends(s_sound_send_parameters *parameters, bool rear, long mode, s_mixbin_settings *mixbins)
{
	if (TEST_FIELD_BIT(parameters->flag0))
	{
		if (!TEST_FIELD_BIT(parameters->flag4))
			add_effect_sends_direct(parameters, rear, mode, mixbins);
		else
			add_effect_sends_split(parameters, rear, mode, mixbins);
	}
}

// @retail 0x2aef50
void c_sound_effects::set_i3dl2(s_sound_i3dl2_parameters *parameters, long unused)
{
	DSFILTERDESC filter;

	IDirectSoundBuffer_SetI3DL2Source(m_buffers->buffer20, &parameters->i3dl2, DS3D_DEFERRED);
	filter.dwMode = DSFILTER_MODE_DLS2;
	filter.dwQCoefficient = 0;
	filter.adwCoefficients[0] = sound_filter_frequency_coefficient((real)parameters->cutoff);
	filter.adwCoefficients[1] = sound_filter_gain_coefficient(parameters->gain);
	filter.adwCoefficients[2] = 0;
	filter.adwCoefficients[3] = 0;
	IDirectSoundBuffer_SetFilter(m_buffers->buffer0c, &filter);
	IDirectSoundBuffer_SetFilter(m_buffers->buffer10, &filter);
}

// @retail 0x2af800
__forceinline void c_sound_effects::add_send(long mixbin, long mode, s_decibels gain, s_mixbin_settings *mixbins)
{
	switch (mode)
	{
	case 0:
		sound_stream_append_mixbin(mixbins, mixbin, gain);
		break;
	case 1:
	{
		s_decibels const &settings_gain = m_settings.gain;
		sound_stream_append_mixbin(mixbins, mixbin, settings_gain + gain);
		sound_stream_append_mixbin(mixbins, mixbin, settings_gain + gain);
		break;
	}
	}
}

// @retail 0x2afb80
void c_sound_effects::add_source_mixbins(s_sound_mixbin_source *source, bool rear, long mode, s_mixbin_settings *mixbins)
{
	long remaining = 8 - mixbins->mixbins.dwMixBinCount;
	s_decibels attenuation(0ul);
	bool alternate = ((source->flags >> 3) & 1) != 0;
	long left = alternate ? 0 : 6;
	long right = alternate ? 1 : 7;
	long rear_left = alternate ? 4 : 8;
	long rear_right = alternate ? 5 : 9;

	switch (source->speaker_mode)
	{
	case 0:
		remaining = 0;
		break;
	case 1:
		if (remaining >= 2)
		{
			sound_mixbins_add(mixbins, left, attenuation);
			sound_mixbins_add(mixbins, right, attenuation);
		}
		break;
	case 2:
		if (remaining >= 2)
		{
			if (rear)
			{
				sound_mixbins_add(mixbins, rear_left, attenuation);
				sound_mixbins_add(mixbins, rear_right, attenuation);
			}
			else
			{
				sound_mixbins_add(mixbins, left, attenuation);
				sound_mixbins_add(mixbins, right, attenuation);
			}
		}
		break;
	case 3:
		if (rear)
		{
			if (remaining >= 2)
			{
				sound_mixbins_add(mixbins, 2, attenuation);
				sound_mixbins_add(mixbins, 3, attenuation);
			}
		}
		else if (remaining >= 4)
		{
			s_decibels level = attenuation;
			level.value -= 6.0206f;
			sound_mixbins_add(mixbins, left, level);
			sound_mixbins_add(mixbins, left, g_44f710);
			level = attenuation;
			level.value -= 6.0206f;
			sound_mixbins_add(mixbins, right, level);
			sound_mixbins_add(mixbins, right, g_44f710);
		}
		break;
	}

	if (source->mixbin != NONE)
	{
		add_send(source->mixbin, mode, attenuation, mixbins);
	}
}

// @retail 0x2af920
void c_sound_effects::add_sends(s_sound_send_parameters *parameters, long unused, long mode, s_mixbin_settings *mixbins)
{
	bool alternate = TEST_FLAG(parameters->flags, 3) != 0;
	s_decibels sum = parameters->level_offset + parameters->level;

	sound_stream_append_mixbin(mixbins, alternate ? 0 : 6, parameters->left_gain);
	sound_stream_append_mixbin(mixbins, alternate ? 1 : 7, parameters->right_gain);
	sound_stream_append_mixbin(mixbins, alternate ? 4 : 8, parameters->rear_left_gain);
	sound_stream_append_mixbin(mixbins, alternate ? 5 : 9, parameters->rear_right_gain);
	c_sound_effects::add_send(10, mode, sum, mixbins);
}
