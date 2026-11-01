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
#include "unknown_218ac0.h"

/* the sound globals (unknown_01e930.h), viewed for their DirectSound object */
struct s_sound_globals_view
{
	byte unknown0000[0x2ab0];
	IDirectSound *direct_sound;
};

#define WMA_PACKET_SIZE 0x10000
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
	real level00;
	real level04;
	real level08;
	real level0c;
	real level10;
	real gain;
	real unknown18;
	real level1c;
	real level20;
	real level24;
	real level28;
	real level2c;
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
	real level;
	real level_offset;
	real gain0c;
	real gain10;
	byte unknown14[4];
	real left_gain;
	real right_gain;
	real rear_left_gain;
	real rear_right_gain;
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
	virtual void add_send(long mixbin, long mode, real gain, s_mixbin_settings *mixbins);
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
__forceinline void sound_stream_append_mixbin(s_mixbin_settings *settings, long mixbin, real decibels)
{
	settings->pairs[settings->mixbins.dwMixBinCount].dwMixBin = mixbin;
	settings->pairs[settings->mixbins.dwMixBinCount].lVolume = sound_decibels_to_volume(decibels);
	settings->mixbins.dwMixBinCount++;
}

// score 281 (was 299): Workspace frame/prologue and repeated conversion coalescing remain.
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
// Round 239: On explicit third sends, single static table and first/second phase-owned descriptors,
// seven subsets 315/289/333/300/336/358/383 fast; all plus both reused levels 432.
// First phase alone 289 fast (control 297), 312 plain versus 281; frame 0xfc/0x94, original whole initializer restored.
// @retail 0x2aeb20
void c_sound_effects::initialize(s_sound_effect_settings *settings, s_sound_effect_buffers *buffers, bool rear)
{
	long const muted_mixbins[4] = {6, 8, 7, 9};
	s_mixbin_settings mixbins = s_mixbin_settings();
	s_mixbin_settings second = s_mixbin_settings();

	m_buffers = buffers;
	memmove(&m_settings, settings, sizeof(m_settings));

	sound_mixbins_initialize(&mixbins);
	c_sound_effects::add_send(6, 0, m_settings.level00, &mixbins);
	if (rear)
		c_sound_effects::add_send(8, 0, m_settings.level04, &mixbins);
	else
		c_sound_effects::add_send(8, 0, -64.0f, &mixbins);
	for (word first_pair_index = 2; first_pair_index < 4; ++first_pair_index)
		c_sound_effects::add_send(muted_mixbins[first_pair_index], 0, -64.0f, &mixbins);
	function_2216f0(NULL, &mixbins, buffers->buffer0c);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer0c, 0);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer18, -6400);

	sound_mixbins_initialize(&second);
	for (word second_pair_index = 0; second_pair_index < 2; ++second_pair_index)
		c_sound_effects::add_send(muted_mixbins[second_pair_index], 0, -64.0f, &second);
	c_sound_effects::add_send(7, 0, m_settings.level00, &second);
	c_sound_effects::add_send(9, 0, rear ? m_settings.level04 : -64.0f, &second);
	function_2216f0(NULL, &second, m_buffers->buffer10);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer10, 0);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer1c, -6400);

	{
		s_mixbin_settings third = s_mixbin_settings();
		sound_mixbins_initialize(&third);
		c_sound_effects::add_send(6, 0, -64.0f, &third);
		c_sound_effects::add_send(8, 0, -64.0f, &third);
		c_sound_effects::add_send(7, 0, -64.0f, &third);
		c_sound_effects::add_send(9, 0, -64.0f, &third);
		c_sound_effects::add_send(10, 0, 0.0f, &third);
		function_2216f0(NULL, &third, m_buffers->buffer20);
		IDirectSoundBuffer_SetVolume(m_buffers->buffer20, 0);
		IDirectSoundBuffer_SetVolume(m_buffers->buffer14, -6400);
	}
}

/* the level of the effect sends: none in mode 0 */
inline real sound_effects_send_level(c_sound_effects *effects, s_sound_send_parameters *parameters, long mode)
{
	if (mode != 1) return effects->m_settings.level10;
	if (TEST_FIELD_BIT(parameters->flag6)) return g_44f70c;
	return effects->m_settings.level2c;
}

// MiniMax R (10-31): no new form, no lower score.
// score 370 (was 370): Frame 0xc and input roles agree; selected-level/right-destination homes and integer/SSE
// staging remain. Dispatcher must stay exact.
// Earlier at 370 (was 370): Frame 0xc agrees; selected-level/right-destination homes and SSE staging remain.
// Frame 0xc agrees; selected-level/right-destination homes and SSE staging remain.
// Earlier: retail copies the input level into a dead argument slot through a
// frame 0xc agrees; retail copies the input level into a dead argument slot through a
// Frame 0xc agrees; selected-primary storage, destination construction and integer-bit/SSE staging differ.
// Earlier: frame 0xc agrees; retail copies the input level into a dead argument slot through a
// frame 0xc agrees; retail copies the input level into a dead argument slot through a
// general register before each SSE add (x87 or direct operands here) and pushes each sum from its slot.
// Round 75: inline two-operand sum helper 453 (identical code); level in a union with a dword 453;
// level by value 454; else-branch level local with gains read at use 491 (dispatcher lost); level
// selected before level2 457. Precision pragma diagnostic 453 (no change).
// Earlier 453 (was 447): Frame twelve agrees; incoming flags/receiver homes and SSE argument staging remain.
// Frame 0xc agrees; selected-level homes and SSE argument staging remain different.
// Earlier 453 from 460: frame twelve agrees; selected-level homes and SSE argument staging remain.
// Base snapshot 514 (caller lost); bin snapshots 511; both 484; direct
// completed-path return 532; offset reads 554; retail-order base 514;
// base/bin snapshots 484; base/primary snapshots 504 (caller losses).
// global optimization off 364 (caller lost); inline depth one 532.
// short destinations 531; boolean routing extraction 527.
// scalar home names 532; selector bitfield 527 (split 276).
// both named flag fields 524 (+3; split 276); routing bitfield 527.
// frame pointer 559 (dispatcher lost); ordinary route first 532; repeated-send loop 541;
// front setting snapshot 521; rear setting snapshot 520; both 515.
// per-use primary selection 467 (dispatcher lost); byte destinations 507;
// reversed mode cases 515; named special gains 515; conditional mode arms 532.
// ordinary gain snapshots 460; late secondary setting 517; native send copy 522 (dispatcher lost);
// alternate ordinary route first 507; special input snapshot 464; combined snapshots 455 (dispatcher lost).
// Primary level snapshot 457; secondary setting first 453 (plain); ordinary input snapshot 498
// (dispatcher lost); native destinations 469; scalar role names 460 (fast).
// Gain pair before selection 447 (plain); offset first 453; all ordinary inputs first 471; special
// settings record 453; ordinary settings record 474; input record 517; special record/early gains 455;
// ordinary record/early offset 476 (fast). Keep only paired gain declarations before the pure
// selector.
// Round 144: live ordinary gains with retained thresholds 500; selected-field reference 519;
// word destinations 510; both 528; primary value with those four forms 503/521/490/518.
// All restored; the 490 form loses the dispatcher. Frame/selected-level/SSE homes remain.
// Reused buffer/alternate/all channel subtotals 470/498/520; complete front/rear
// pair subtotals 484/458, both 494; repeated boosted offset 465; complete subtotal
// graph 521. All restored; selected-level homes and SSE argument staging remain.
// Live front/rear threshold references 489; selector field too 504; gain fields too
// 515; both groups 533; native destinations with those four forms 498/518/528/544.
// All expressions and opening body restored.
// Round 171: Shared scalar selector/forced expansion/readonly inputs 447 each; ordinary input copy
// 480; primary value 454; both copies 481; word destinations 461; primary value with native
// destinations 457. Input-copy and final native-destination forms lose the dispatcher; all restored.
// Frame 0xc agrees; selected-level homes, full-word destinations and SSE staging remain.
// Shared selector complete-result/default-first/scalar-early/reference-early 447/454/454/451;
// with native bit member 453/453/453/455. Complete scalar result rejected for one-use field copy.
// Native-bit scalar early returns retained: split 238 to 224, direct 447 to 453, net eight plain.
// Positive-mode original/native direct returns 447/447/453/453 (split 235 each, rejected);
// native routing bit two on retained selector graph 453 (tie, reverted).
// Round 234: primary/front consumption scopes and right-first destinations
// 453/453/453/449/449/449/449; special workspace end 453; complete target flag word
// 453 with right-first 449. Below-margin gains restored.
// Asymmetric first/second long destinations 446/462; first/second word 438/461;
// first/second signed short 438/461; first dword alone/right-first 446/448 (fast).
// First unsigned-word destination retained: 438 plain, dispatcher exact; other destination remains
// byte and both value sets are unchanged. Selected-level/SSE homes still differ in frame 0xc.
// Round 365: actual selected primary-field pointer with native destinations, special input
// snapshot and native routing domains: 453/461/441/455/447/461/438/454. The 438 plain
// form loses the dispatcher; input snapshots change the entry roles. All restored.
// Round 274: primary scalar on retained asymmetric destinations with original/long right and bool/word
// routing carriers 446/449/437/451; same forms with the secondary setting observed at its send
// 455/462/450/452. The 437 gain is below margin; 449/451 lose the dispatcher. All restored.
// Round 241: Paired selected-setting decision, rear/front first, with independent destinations, live
// routing field, and paired true/false-first destinations: 449/449/449/449/473/473/473/473. All
// selected-field observations and original body restored.
// Round 207: native two-gain field array, uniform at-use gain reads and plain paired
// front/rear/front+rear/tail/front+tail/rear+tail/all loops 466/474/475/497/490/492/507;
// all plus the repeated buffer send loop 523. Every loop uses a real channel/gain
// index. All restored; frame 0xc, selected-level and SSE staging differences remain.
// Round 164: ordinary append inline 438; right long original/ordinary inline 457/457,
// right word 456/456, signed byte 438/438; word route with ordinary inline 428,
// word route alone 428 plain (kept); long/dword/byte route 438 each. Only the native
// 0-or-1 route word remains; original append declaration and all gain trees retained.
// Round 154: the first branch's sums are decibels_add (unknown_218ac0.h) on levels selected through
// pointers, with a bool alternate and long bins: 393 (kept); levels as reals through an explicit
// pointer, passed as long bits 384 (kept); value-selected real levels 390 (convention changes);
// append gain by const reference 393 (0x2af920 worse); else-branch long level/offset with gains read
// at use 459; else-branch long level only 426; pointer-selecting send-level helper 406 (0x2af450
// worse); /Op diagnostic no change. Retail uses level/level2's stack homes directly as decibels_add's
// first operand and pushes each sum from a slot through a GPR; the else branch still differs.
// Else-branch offset as long bits from decibels_add 370 (kept); then front/rear levels read at use
// 415, gains read at use 403, send level and offset declared before the gains 401 (all three move
// the convention), named appended sums 370 (identical), offset operands swapped 370. Our !flag3
// sums are x87 (retail SSE, pushed from a slot through a GPR, as if the append took long bits).
// Round 168: Retained word route with right long/word/dword/signed-byte domains 451/450/451/428; primary scalar with inline/inline-noinline/cdecl/stdcall 437/437/449/437. Cdecl loses dispatcher; all restored.
// Round 247: native flag2 and primary pointer, original/native flag3 428/424; with shared
// ordinary input 471/463; right from left 461/461, with shared input 494/494. Four-point
// gain rejected; original field view, destinations and complete body restored.
// Round 224: Coupled left short/right word-short-long-dword 450/450/451/451;
// left long/right word-short-long-dword 451/451/452/452, all on the retained word route.
// Original word/byte destinations, selector, gains and append boundaries restored; dispatcher exact.
// Round 269: shared bit-domain sums for buffer pairs 348; front/rear pairs 380; both 442; mode tails 332;
// buffer/tail 310; front-rear/tail 443; all 507; all with the native alternate-routing field 507.
// Every lower-scoring form loses the dispatcher; original arithmetic and executable body restored.
// Round 294: Entry left/right gain and send-level storage, seven nonempty subsets, all 370;
// all with offset storage 370. All producers, helper boundaries and the complete
// body restored; frame 0xc agrees, selected-level and integer/SSE staging remain.
// Round 428: special selected-level names, repeated-sum name and both 370/370/370;
// separate three actual sum results 370; early special sum storage 370; entry special scalar storage 370;
// secondary-first acquisition on the copied-bit graph 374; one phase-disjoint sum workspace 370.
// All original scalar producers, names, scopes and arithmetic restored.
// DL199: Member stdcall/fastcall/inline with bool/word route give 370/371 each; cdecl gives
// 383/387 and loses the dispatcher. Inline trials recover it. Original body, bool route and both
// declarations restored; selected-level/right-destination homes and integer/SSE staging remain.
// Round 92: offset bin read before the two appends 404; bins through a buffers pointer, offset/right
// bins first 387 (both lose the dispatcher); first-branch level through a const pointer 398; second
// level inlined into its one sum 393; separate first/second sum locals 370; add_send sums through
// decibels_add 370; whole-file float-consistency pragma diagnostic 363 (all fast). Lead: retail's
// level/send-level kept in general registers and bounced through stack temps before each addss,
// every sum stored to a slot and pushed through a GPR, and no (send + level) shared between appends
// are /Op code generation; the sibling 0x2afb80 matches in a /O2 /Op /arch:SSE /Gr file (flags are
// the user's decision).
// Round 7 (lane 17, fast with 0x2af7b0/0x2af450/0x2aeb20): the else branch wholly through decibels_add
// (the input level once as long bits, every sum and mode tail as bits, gains and front/rear levels read at
// each use, as retail reads them) 463 at 1263 bytes and the dispatcher lost (parameters then stays on the
// stack and this comes in ECX); the same with the front/rear snapshots kept 470 at 1276 (dispatcher exact,
// so the flip follows the this/parameters balance; decibels_add's long operands load each gain as an
// integer and stage it, where retail loads the gains as floats and stages only register-held bits); the
// input level copied once into a real local for the else branch's sums 411 (dispatcher lost); the
// first branch's two selected levels as long bits 377 (aligned 161 -> 152, but level then sits in EBX and
// the right bin is spilled); the first level as a real/long union 370 (same code).
// Round 44 (lane 14, fast): else-branch gains read at each use 403; with the input level named once
// for every sum and the offset 420 (dispatcher 23); that level as a bits/value union reached through a
// reference 416 (dispatcher 23); offset as a plain float sum 384; send level and offset read before the
// gain snapshots 401 (dispatcher 23). Whole-file /Op diagnostic 363.
// J222: Special input bits 412; front subtotal 412, input/front 423; rear 412, input/rear 423; front/rear 422;
// all 432; all with boosted offset 416. Every form loses dispatcher 0x2af7b0 and is rejected; all original
// arithmetic, observations, helpers, declarations and executable body restored.
// Lane 18 r21 (fast unless noted): else-branch input level as a long read through `*(real *)&level`
// 410, in a local bits/value union 411 (both memory-homed; dispatcher lost). Lead, 296: the level as a
// long local never address-taken, every else-branch sum through a new inline two-operand bits-in,
// real-out sum (`return *(real *)&a + *(real *)&b;`) and the gains read at each use: 296 plain at
// 1116 B, dispatcher exact (retail's EBX level staged before each add, no shared send + level). Not
// kept: the helper's only caller is this function; 0x2af450 229, 0x2af920 205/208 and 0x2af800 79
// rise with it, and the same sum with a double body is 343. Without the gain reads 313 (1127 B;
// dispatcher lost). On the 296 form: front/rear levels at use 333, with gain locals 321; original
// mode-one tail 324; offset at the top of each flag3 branch 312; flag3 mode tails through the helper
// 307; long first-branch levels 309; send-first, inner-first, send-as-bits and first-branch helper
// forms 296. One-argument bits-to-real helper 342; every sum through decibels_add 470 (1276 B).
// Remaining there: our compiler makes all-memory sums x87 chains (regrouped); retail stays SSE.
// Round 240: Successive scalar sum statements in buffer/front/both/rear/buffer-rear/front-rear/all
// pairs, then all with a named repeated boosted result: 370/370/370/370/370/370/370/370.
// Original grouping, scalar observations and complete body restored; dispatcher checked.
// @retail 0x2aefe0
void c_sound_effects::add_effect_sends_direct(s_sound_send_parameters *parameters, bool rear, long mode, s_mixbin_settings *mixbins)
{
	if ((bool)((parameters->flags >> 2) & 1))
	{
		bool alternate = (bool)((parameters->flags >> 3) & 1);
		long left = alternate ? 0 : 6;
		long right = alternate ? 1 : 7;
		real level = *(rear ? &m_settings.level1c : &m_settings.level24);
		real level2 = *(rear ? &m_settings.level20 : &m_settings.level28);
		long sum = decibels_add(*(long *)&level, *(long *)&parameters->level);
		sound_stream_append_mixbin(mixbins, left, *(real *)&sum);
		sum = decibels_add(*(long *)&parameters->level, *(long *)&level);
		sound_stream_append_mixbin(mixbins, right, *(real *)&sum);
		sum = decibels_add(*(long *)&level2, *(long *)&parameters->level);
		add_send(2, mode, *(real *)&sum, mixbins);
		add_send(10, mode, parameters->level_offset, mixbins);
	}
	else
	{
		real left_gain = parameters->gain0c;
		real right_gain = parameters->gain10;
		real send_level = sound_effects_send_level(this, parameters, mode);
		long offset = decibels_add(*(long *)&parameters->level_offset, *(long *)&parameters->level);
		if (!TEST_FIELD_BIT(parameters->flag3))
		{
			sound_stream_append_mixbin(mixbins, m_buffers->mixbin00, left_gain + (send_level + parameters->level));
			sound_stream_append_mixbin(mixbins, m_buffers->mixbin04, right_gain + (parameters->level + send_level));
			long mixbin08 = m_buffers->mixbin08;
			switch (mode)
			{
			case 0:
				sound_mixbins_add(mixbins, mixbin08, *(real *)&offset);
				break;
			case 1:
				sound_mixbins_add(mixbins, mixbin08, *(real *)&offset + m_settings.gain);
				sound_mixbins_add(mixbins, mixbin08, *(real *)&offset + m_settings.gain);
				break;
			}
		}
		else
		{
			real front_level = m_settings.level00;
			if (front_level > -64.0f)
			{
				sound_mixbins_add(mixbins, 0, left_gain + (front_level + (parameters->level + send_level)));
				sound_mixbins_add(mixbins, 1, right_gain + (front_level + (parameters->level + send_level)));
			}
			if (rear)
			{
				real rear_level = m_settings.level04;
				if (rear_level > -64.0f)
				{
					sound_mixbins_add(mixbins, 4, left_gain + (parameters->level + send_level + rear_level));
					sound_mixbins_add(mixbins, 5, right_gain + (parameters->level + send_level + rear_level));
				}
			}
			switch (mode)
			{
			case 0:
				sound_mixbins_add(mixbins, 10, *(real *)&offset);
				break;
			case 1:
				sound_mixbins_add(mixbins, 10, left_gain + *(real *)&offset);
				sound_mixbins_add(mixbins, 10, right_gain + *(real *)&offset);
				break;
			}
		}
	}
}

// score 224 (was 224): Frame 0x20 versus 0x18, mode/input-level homes and the second x87 tail sum remain.
// MiniMax R (10-31): no new form, no lower score.
// Retail loads level, offset and the send level with integer moves (/arch:SSE has no movd, so level
// in EBP is bounced through a temp before each add) and never shares send + level between appends.
// Round 152: send level from an out-parameter getter 224 (promoted, identical); gains at use, byte
// alternate and plain long bins 289 (this moves to the stack, dispatcher changes); front/rear sums
// through a by-value forceinline append 230 (still shared); level and offset read at each use 308;
// volatile send level 266 (diagnostic, reverted: mode keeps EBP).
// Earlier 238 from 254; complete scalar selector result 228 plain (one-use copy rejected); default-first/early
// scalar/reference 232/243/238 fast; native-bit forms 226/224/224/227 with direct-route regressions.
// Earlier: Frame 0x1c versus 0x18, mode/level homes and x87 tail sums remain; external
// Frame 0x1c versus 0x18, mode/level homes and x87 tail sums remain. Round 55: retail
// tests send flag 6 as a bitfield (`shr eax, 6; test al, 1`, also in 0x2aefe0): a flag6 bitfield in the
// helper gives 227 plain but 0x2aefe0 456 (447); with gain0c read at each use 261 (aligned closer:
// retail never snapshots it); flags 2/3 as bitfields too 227/456; bool alternate with plain long
// selectors 263 (dispatcher lost); send level and offset before the left gain 236. Retail keeps
// `level` in ebp and never CSEs (send_level + level) across the appends; ours CSEs it and gives
// ebp to mode. Earlier: external
// selected/input/offset references 294/291/272, pairs 311/311/307, three/four references 326/340.
// All restored.
// Earlier: frame 0x1c versus 0x18; mode occupies EBP.
// Front pair loop 308; rear pair loop 323; both 363; all short/word
// selectors 274; byte selectors 275; only spilled short/word selectors 280.
// Earlier: frame 0x1c versus 0x18 and input-level/mode homes remain.
// Shared append front 392, rear 394, tail 430 (dispatcher lost in all three);
// whole-function depth zero 673 (dispatcher lost), depth one 274.
// Earlier: frame and input-level/mode homes remain;
// front setting snapshot 270; rear 279; both 275; frame pointer 312;
// global optimization off 281 (dispatcher lost).
// Earlier: frame 0x1c versus 0x18 and input-level/mode homes remain;
// word routing predicate 264; named routing reads 270; native-word home names 270;
// front field reference 267 and both field references 267 (three-point gains rejected).
// Earlier: frame 0x1c versus 0x18; mode/level homes and x87 tail sums remain.
// frame 0x1c/0x18 and mode/input-level homes remain.
// Scalars before selectors 258; front/gain/both/all before selection 262/254/254/254;
// phase scope 264; inner/outer parts 258/263. Gain-first plain 254, 819/853 bytes.
// frame 0x1c versus 0x18; mode occupies EBP.
// frame 0x1c versus 0x18 and input-level/mode homes remain.
// frame and input-level/mode homes remain;
// frame 0x1c versus 0x18 and input-level/mode homes remain;
// frame 0x1c versus 0x18; mode/level homes and x87 tail sums remain.
// Left/right/both gain snapshots 258/262/266; input level/offset cursors 313/281;
// gains with level/offset/both cursors 274/261/291 (fast). Paired gains lose dispatcher.
// Kept only the left-gain snapshot, used three times; plain 258 at 809/853 bytes.
// frame 0x1c/0x18 and input-level/mode homes remain.
// Primary routing 272; signed word route 264; direct left 275; external output
// and live settings records 264/264; primary-output pair 272; full left 275; settings-direct pair 271.
// Earlier 258 (was 258). Dword right/rear-left/both 258/255/259; rear-right 255;
// right/rear-right 259, both rear 256, all spilled 260, all destinations 268 on gain-first source.
// Round 127: inline/noinline 254/254; native routing bit 251 (three-point gain rejected); complete
// input/settings/both copies 282/238/277 (dispatcher lost); Boolean/dword predicates with native
// endpoints 274/273; native bit plus native endpoints/Boolean predicate 273/260. Frame 0x1c and
// mode/level homes remain.
// Round 162: Complete settings snapshot plus ordinary right/rear-left/both/rear-right/
// right+rear-right/both-rear/all destination words 257/251/261/251/263/258/261;
// all ordinary words plus native routing 255. All lose the dispatcher; three-point
// gains also fail the minimum margin. Original scalar settings and qualifiers restored.
// Live front setting 248; rear snapshot 267; live front/rear snapshot 261; both gains before selection 260 (dispatcher lost);
// four coherent five-scalar home names 254 each; live front/native routing bit 238 plain (kept);
// live front/native widths and native-bit widths 256/256; input level/offset fields 322;
// selected-field reference with live front, without/with native bit 312/312 (dispatcher lost).
// Unqualified right/rear-left/both/rear-right/right+rear-right/both-rear/all words 267/261/266/262/267/261/267; all with native routing bit 265. Every right-word removal loses dispatcher; other subsets regress. Frame 0x1c/0x18, mode/level homes and tail sums remain.
// Uniform gain/setting/both/inner-level/gain-inner/setting-inner/all sum operand orders; paired tail
// operand order: 238/238/238/238/238/238/238/238 (fast); all restored.
// Append native destination 238; shared append cached index/stores/cursor 269/265/252;
// native destination with cached index/stores/cursor 269/265/252; cursor plus index 269.
// All output-residency changes lose the dispatcher; append helper and body restored.
// Negative-mode scalar selector with native bit six 224 plain; direct route 453, net eight.
// Positive-mode scalar direct returns 235 with original/native extraction (three-point gains rejected).
// Round 183: Eight actual native-word destination declaration permutations
// 227/228/228/225/228/224/227/228; starting with right ties, rear-first forms regress.
// Original declarations restored; frame 0x20, mode/input-level and x87 tail homes remain.
// Native-word destinations with bool/byte routing predicates and original/gain/level/both
// read-at-use forms 242/271/302/323 each. Original scalar observations restored.
// Round 190: Mode-first field-address selection with gain at entry/after selection/after
// offset/after input level 247/247/258/258; constant-first counterpart 232/231/245/245. Every form
// regresses; original helper and scalar lifetimes restored.
// Round 208: uniform shared input sums with paired route subtotals, repeated
// right gain and paired threshold observations 293/263/286/270/267/267/270/270.
// Without the right-gain snapshot, frame 0x18 shortens to 779-804 bytes; gain forms
// lose the dispatcher. Original sums, observations and local qualifications retained.
// Round 195: Input-level-first scalar schedules [227, 228, 239, 238]; offset-first and selector/gain
// schedules [239, 239, 245, 228]. Original observations and three local qualifications restored; frame,
// mode/input and tail homes remain.
// Round 201: Const send-input parameter, const receiver method, both, immutable scalar carriers,
// each contract with those carriers, both contracts/carriers, and coherent scalar names: 224 each.
// All eight tie; original signatures, selector inputs, scalar lifetimes and dispatcher retained.
// Round 234: destination/selected-send/input-level consumption scopes and all combinations
// 224/224/224/224/224/224/224; routing-predicate consumption scope 224. Original lifetimes restored.
// Round 197: Retained word-route graph, per-use gain/offset/both/level/gain-level/offset-level/all inputs 265/257/276/290/312/308/324; all inputs with native phase scope 324. Some forms recover frame 0x18 but regress; original observations restored.
// Round 204: Native signed/unsigned routing predicate with primary/spilled/all destination
// promotions 241/241/248/248/245/245/248/248. Independent selectors and scalar-return graph
// regress; original word route, scalar observations and three local qualifications restored.
// Round 365: joint native destination branch with all eight spilled-width subsets
// 262/263/263/264/263/264/264/265. All restored; frame/input-level and tail sum remain.
// Round 235: Existing qualified class raw-send path, seven front/rear/tail subsets and all-path if-else tail
// [224, 224, 247, 224, 247, 247, 248, 254] fast. Dispatcher preserved in every trial; all original helper
// calls, sums, fields and three local qualifications restored.
// Round 389: scalar-selector single-result chains, positive nesting, regular/alternate/constant
// defaults, negative/positive ternaries and mixed publication 224/226/219/231/228/224/235/224.
// Five-point default-result gain also loses dispatcher 38; original result graph/callers restored.
// Round 259: five complete shared inline-append formal orders and actual output
// pointer/reference result contracts 224 each (eight forms). Original helper and all calls restored.
// Round 279: natural mode tail, zero/one arm first 287/230; with nested rear guard
// 287/230; each with a shared right gain, alone or nested, 238 (dispatcher 27).
// Original mode switch, gain observations and three local qualifications restored.
// Real gain-array loops front/rear/tail/front-rear/front-tail/rear-tail/all-long/all-word
// 318/346/314/422/388/411/492/466; dispatcher 26 in every trial. Original fields, observations,
// three local qualifications and body restored.
// Round 301: Complementary primary-route word/bool/byte/long with conditional destinations
// 244/249/249/247; independent complete destination producers 240/256/256/253.
// Original routing, producers, observations and three local qualifications restored.
// Round 92: selectors without volatile as word/long/bool-route long 250/256/262 (each loses the
// dispatcher); offset as long bits 314; mode-one tail through decibels_add on those bits 293; input
// level declared first 227; whole-file float-consistency pragma diagnostic 224 (all fast). Lead:
// retail's level in EBP bounced through a stack temp before each addss, the offset and send level
// copied as integers into homes, and the store-reload pushes are /Op code generation (see 0x2aefe0).
// Round 216: Direct normalized route bit in word/short/byte/dword carriers 236/236/242/241; raw bit mask carriers 237/237/253/248. All restored; frame, mode/input-level homes and x87 tail remain.
// Round 44 (lane 14, fast; ours already returns with ret 8, as the matched dispatcher calls it):
// input level as a bits/value union reached through a reference 230; held as its bits (long) and read
// as a real at each use 230 (copied with integer moves, but mode keeps EBP); that with plain long
// selectors 261 (dispatcher 26); with both gains read at each use 265; and the send level read through
// the selected field's address 247. Whole-file /Op diagnostic 224.
// Lane 18 r21 (fast): the level as a long local with the inner sums through a two-operand bits-in,
// real-out inline sum (see 0x2aefe0) 229 at 832 B: level in EBP and mode on the stack as retail, but
// the all-memory sums become x87 chains; with gains at use 261; gains at use and offset bits 238;
// offset bits 232 (854 B); no volatile bins 240; long bins, no volatile 242; front-level local 228;
// offset bits, no volatile 237; bool flag with long bins, no volatile 247; bool flag 246; bool flag
// with long bins 256; gains at use, bool flag, long bins 254. 240/242/228/237/247 lose the dispatcher.
// Round 240: Successive scalar sums front/rear/both/both with tail results
// on real/bit-held input: 224/224/224/224/230/230/230/230. New own-address forms exact-only;
// all original scalar observations, grouping, qualifications and complete body restored.
// @retail 0x2af450
void c_sound_effects::add_effect_sends_split(s_sound_send_parameters *parameters, bool rear, long mode, s_mixbin_settings *mixbins)
{
	real left_gain = parameters->gain0c;
	real send_level = sound_effects_send_level(this, parameters, mode);
	real offset = parameters->level_offset;
	word alternate = TEST_FIELD_BIT(parameters->flag3);
	word left = alternate ? 0 : 6;
	volatile word right = alternate ? 1 : 7;
	volatile word rear_left = alternate ? 4 : 8;
	volatile word rear_right = alternate ? 5 : 9;
	real level = parameters->level;

	if (m_settings.level08 > -64.0f)
	{
		sound_stream_append_mixbin(mixbins, left, left_gain + (m_settings.level08 + (send_level + level)));
		sound_stream_append_mixbin(mixbins, right, parameters->gain10 + (m_settings.level08 + (level + send_level)));
	}
	if (rear && m_settings.level0c > -64.0f)
	{
		sound_stream_append_mixbin(mixbins, rear_left, left_gain + (m_settings.level0c + (level + send_level)));
		sound_stream_append_mixbin(mixbins, rear_right, parameters->gain10 + (m_settings.level0c + (level + send_level)));
	}
	switch (mode)
	{
	case 0:
		sound_stream_append_mixbin(mixbins, 10, offset);
		break;
	case 1:
		sound_stream_append_mixbin(mixbins, 10, left_gain + offset);
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

// score 55 (was 55): our body is 12 bytes shorter: retail adds the gain with
// SSE (the gain's bits copied through ebx into a stack temp) while our
// union pun makes MSVC use x87; five more rewrites of the pun scored 55 or
// worse. Settings gain copied into a local before each send (one-use copies,
// reverted in review) 44: retail's SSE sums and length; with operands swapped
// 44; the helper's level by reference 55; the sum written into the union 55;
// the bit copy before the switch 44.
// worse.
// Tried (MiniMax R run 12): retail reads the settings gain again for each mode-one send (movss xmm0,
// [edi+0x18] twice) and adds the GPR-held gain through its slot with SSE. m_settings.gain read into a named
// local per send 44 (retail's 279-byte length), not kept (one-use copies, as round 44 of lane 8); one copy
// for both sends 79, a copy for the first send only 52, the gain pun on the parameter 86, the sums assigned
// to the parameter 44, the pun hoisted above the switch 44.
// Earlier: retail inlines the existing out-of-line mix bin append (its
// count++ is reused by the second send), ours the local copy. The existing
// append called directly stays out of line 252, 238; plain sums 102; one
// adjusted level 167; the gain parameter adjusted in place 167.
// Round 92: mode-one sums through decibels_add 79 (SSE, both operands homed), without the bits local
// 79; plain sums 102; sums on `*(real *)&gain_bits` 86; one settings-gain local used twice 79; const
// reference to the settings gain 55; union result assigned back (`first.value = ... + first.value`,
// with or without a settings local) 55; union `+=` 55; append decibels by const reference with the
// original/plain/decibels_add bodies 55/102/79; whole-file float-consistency pragma diagnostic 55
// (the x87 add stays; all fast).
// @retail 0x2af800
__forceinline void c_sound_effects::add_send(long mixbin, long mode, real gain, s_mixbin_settings *mixbins)
{
	switch (mode)
	{
	case 0:
		sound_stream_append_mixbin(mixbins, mixbin, gain);
		break;
	case 1:
	{
		union s_send_level
		{
			real value;
			long bits;
		};
		long gain_bits = *(long *)&gain;
		s_send_level first;
		first.bits = gain_bits;
		sound_stream_append_mixbin(mixbins, mixbin, m_settings.gain + first.value);
		s_send_level second;
		second.bits = gain_bits;
		sound_stream_append_mixbin(mixbins, mixbin, m_settings.gain + second.value);
		break;
	}
	}
}

// MATCH (was 117): the remaining count, the zero gain and the four endpoint values are computed in
// retail's order, the front endpoints are full-width longs kept in ebp/ebx, and the run-time -6.0206
// attenuation sits in a bits/value union reached through a reference to its float member, so the
// compiler reloads it from the stack slot before each send as retail does.
// The zero passed as *(real *)&attenuation, the bits/value union and the store in case 0 (remaining
// is not read after the switch) are retail's code shape and are kept only with this byte match.
// score 117 (was 137): speaker-mode switch with explicit pair calls, a statement in the mode-0 case for
// retail's four-entry jump table, the zero gain passed as *(real *)&attenuation and the run-time
// -6.0206 attenuation through a local bits/value union; the length now matches retail's 396 bytes.
// score 137 (was 137): Frame eight agrees; source/bin homes, runtime zero attenuation and dispatch remain
// (on the decibel-struct interface, retail's four-entry jump table needs a statement in case 0).
// Earlier: frame eight agrees; source/bin ownership, runtime zero attenuation and mode dispatch remain.
// Earlier at 213 (from 213): frame 4 versus retail's 8; retail computes the
// attenuation at run time and dispatches the speaker modes through a jump
// table. Two-pass channel loops that pick the bin with a ternary 145 (not
// kept: they only lengthen the code toward retail's size); loops over
// left + channel 245; word mode 208.
// Earlier at 166 (from 213): frame four versus eight; zero gain folds and mode dispatch differs.
// Direct bin loops: mode one 172, mode two 174, attenuation 200, fixed pair 214, all 166;
// dynamic pairs only 166, modes one/two 171, mode one/attenuation 169.
// Earlier at 156 (from 166): frame eight agrees; zero gain folding and mode dispatch remain;
// frame pointer 166; word counters 157; common counter 166; exclusive end bins 165;
// pair/quad capacity predicates 156.
// Earlier at 137 (from 156): Frame eight agrees; runtime zero attenuation and mode dispatch remain. Shared modes 168; adjacent endpoints 193; all byte endpoints 142; byte capacity 156; unsigned counters 156; combined shared modes 161; rear bytes 170; front bytes 137 (plain, exact length).
// Earlier at 137 (from 137): source/bin allocation, runtime attenuation and dispatch remain.
// Word/long mode capture 137/148; direct capacity with word/long mode 163/165/168;
// range order/short front endpoints/flags at use 151/164/138.
// entry receivers, runtime zero attenuation and mode dispatch remain.
// All/front/rear byte bin counters; native source/destination/both records;
// mode field binding and promoted mode on the retained front-byte endpoints
// Fast scores 152/149/147/137/137/137/137/137; original body retained.
// Earlier at 137 (from 137): source/bin allocation, runtime attenuation and mode dispatch remain.
// Selected mode-two range with speaker cases 123: long/captured 159, long/direct 184,
// byte/captured 162, byte/direct 177; cases 321 give 159/184/162/177. Original loops retained.
// Earlier at 137 (from 137): the zero gain (retail keeps it in EDI and pushes EDI) and the run-time
// attenuation remain. Built with /Op (pragma diagnostic, or a /O2 /Op /arch:SSE /Gr file of its
// own) the attenuation becomes retail's run-time subss through the rear slot, 146 at 408 bytes;
// there the switch with explicit calls is 203 at 306 bytes (32-bit bin selections as retail). An
// empty case 0 switch 151; explicit calls 233; direct capacity tests 163.
// Long pair/quad/both predicates 153/141/140; both word predicates 143; long routing predicate 137; rear endpoints first 154; capacity before source flag 136; quad predicate first 137. Restored.
// Round 414: joint front/rear/both endpoint producers 137/151/141; routing-result arms
// alone/front/rear/both 137/137/152/142; routing default with both pairs 133 at 396/396
// (gain four, below eight; test-produced routing also exact-only). Original complete body restored.
// Round 81: zero-case switch 141; full-width front endpoints on that switch 137; gain after endpoints 137;
// explicit default 137; capacity first 141; separate gain assignment 137; integer-bit zero with
// full-width/byte endpoints 158/196 (exact-only, no match). Original body restored.
// Round 421: signed-byte routing/pair/quad truth carriers, all seven nonempty subsets
// 137 each; all three as signed short 144/404. Original Boolean carriers, real-bin
// loops, gains and body restored. Source/bin homes and runtime zero remain.
// Earlier at 137 (from 137): retail is a four-entry jump-table switch on the speaker mode with explicit call
// Round 92: speaker-mode switch with explicit per-bin sends and a long zero gain passed as
// `*(real *)&gain` 181 (326 B; attenuation x87, dec chain); with the attenuation through a bits/value union
// recomputed per bin 101 (365 B, prologue and send tail exact; not kept: value written at each use and a
// constant-only local); union computed once 153; named attenuation local 181; long zero gain on the
// original if-chain 155; switch + long gain on the original loops with union/pun attenuation 150/151
// (all fast). Retail's four-entry table needs a statement in a mode-0 case, and its store-reload
// attenuation push is /Op code generation.
// Round 48 (lane 12): MATCH (full check) as its own /O2 /Op /arch:SSE /Gr file with the zero gain as
// `long gain = 0` passed as *(real *)&gain and a speaker-mode switch whose mode-0 case holds a dead
// `remaining = 0;` (needed for retail's four-entry jump table; an empty case 0 drops the table, `gain = 0` 91).
// Held: the dead store is not kept even with a match, and a new /Op file is a build-flag decision.
// Earlier at 137: retail is a four-entry jump-table switch on the speaker mode with explicit call
// Also at 137: retail is a four-entry jump-table switch on the speaker mode with explicit call
// pairs (no loops), the zero gain held in EDI and pushed, and each -6.0206 attenuation computed at run
// time (`mov [slot], 0; movss; subss; movss [slot]; mov ecx, [slot]; push ecx`). Round 17 found the
// cause: retail passes decibels to 0x21e230 (and g_44f710) as a 4-byte struct by value; a copy of the
// zero gain adjusted in place (`level = gain; level.value -= 6.0206f;`) is not constant-folded and
// gives retail's sequence exactly (compile-only harness and fast build). On that interface (header
// struct, 0x21e230 taking it, g_44f710 of that type, the switch with explicit pairs and a memset zero
// gain) this function reaches 133 (aligned diff 21: prologue order and the level copy from EDI where
// retail stores an immediate 0) but only with a statement in `case 0` (any statement there, even an
// empty inline call, gives the jump table; without one it is a dec chain, 146); 0x2aefe0 370 -> 297
// and 0x21fa80 545 -> 532 on the same interface. Not kept: this function rises without the case-0
// statement. A long-bits parameter instead moves the decibels into EDX (0x21e230 8 -> 21).
// Earlier forms: frame 4 versus 8 with two-pass channel loops picking the bin with a ternary 145;
// loops over left + channel 245; word mode 208; direct bin loops 166-214; dynamic pairs 166-171;
// frame pointer 166; word counters 157; common counter 166; exclusive end bins 165; pair/quad
// capacity predicates 156; shared modes 168; adjacent endpoints 193; all byte endpoints 142; byte
// capacity 156; unsigned counters 156; rear bytes 170; front bytes 137 (kept); word/long mode capture
// 137/148; direct capacity 163-168; range order/short front endpoints/flags at use 151/164/138;
// byte bin counters, native records, mode field binding 137-152; selected mode-two range with speaker
// cases 159-184; /Op (pragma diagnostic or own file) 146 at 408 bytes, its switch with explicit calls
// 203 at 306 bytes; empty case 0 switch 151; explicit calls 233; long pair/quad/both predicates
// 153/141/140; both word predicates 143; rear endpoints first 154; capacity before source flag 136;
// joint endpoint producers 137/151/141; routing-result arms 137-152; routing default with both pairs
// 133 at 396/396; zero-case switch 141; gain after endpoints 137; explicit default 137; integer-bit
// zero 158/196; signed-byte/short truth carriers 137/144.
// Round 17: switch with explicit pairs 233 (276 bytes, gain folded); gain from a cross-module helper
// returning 0.0f 233 (LTCG inlines and folds it); with the decibel struct: memset zero gain and a
// statement in case 0 133, aggregate-initialised gain 123 (aligned 71), no case-0 statement 146, the
// loop body on the struct interface 161, the loop body passing the real gain through casts 176.
// Round 108: TEST_FIELD_BIT alternate 137; const gain 137; gain declared first 137; endpoints set in an
// if/else 177 (344 bytes); a switch on the speaker mode with the loops kept 151 (377 bytes). Restored.
// Round 48 (lane 12): MATCH (full check) as its own /O2 /Op /arch:SSE /Gr file with the zero gain as
// `long gain = 0` passed as *(real *)&gain and a speaker-mode switch whose mode-0 case holds a dead
// `remaining = 0;` (needed for retail's four-entry jump table; an empty case 0 drops the table, `gain = 0` 91).
// Held: the dead store is not kept even with a match, and a new /Op file is a build-flag decision.
// Round 94 (in place, long gain passed as *(real *)&gain, explicit pair calls, no case-0 statement): the
// union attenuation per bin 101 (365 bytes, dec chain instead of the table); one named `level` from
// `*(real *)&gain - 6.0206f` 181 (326 bytes, folded); real gain with that level 236 (278 bytes); the
// attenuation written at each call 181; the per-bin union with byte front bins 146. Lead: retail needs
// /Op code generation for the attenuation push and a mode-0 case statement for its four-entry table.
// Lane 17 round 5: quad as four explicit calls 139 (405 bytes); center pair as a 2..3 loop 138; named
// attenuation on the loops 137; right endpoints as left + 1 148 (374 bytes); remaining before the flag 137.
// The decibel-struct interface above is blocked only by the case-0 statement.
// Lane 14 r22: the switch with explicit pairs and long endpoints 233 (276 B; retail's setne/sbb/lea
// endpoints exactly, but a dec chain, the gain folded to `push 0` and a constant attenuation); with an
// empty case 0 233 (still a dec chain); with the quad arm as a loop 231; with byte front endpoints 223;
// the capacity bools renamed to flip their name order 137.
// Lane 16 r50 (fast): capacity bools declared after the endpoints 151 (376 B); the one-use quad bool
// dropped for `remaining >= 4` 138; mode 2's front loop first (`if (!rear)`) 139; the constant gain
// written as 0.0f at each use 137 (same code); mode 3's quad arm first 135; both front-first swaps 137.
// BLOCKED by the build-flag lead (/Op) and the case-0 statement above.
// Round 115 (lane 6, fast): the flag as `(flags & 8) != 0` 137 (same code); mode 3 as `rear && pair` /
// `!rear && quad` arms 137; the pair bool dropped for `remaining >= 2` at its three tests 138; modes 1 and 2's
// front loop merged under `mode == 1 || (mode == 2 && !rear)` 157 (364 bytes); the bools as `> 1` / `> 3` 137.
// @retail 0x2afb80
void c_sound_effects::add_source_mixbins(s_sound_mixbin_source *source, bool rear, long mode, s_mixbin_settings *mixbins)
{
	long remaining = 8 - mixbins->mixbins.dwMixBinCount;
	long attenuation = 0;
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
			sound_mixbins_add(mixbins, left, *(real *)&attenuation);
			sound_mixbins_add(mixbins, right, *(real *)&attenuation);
		}
		break;
	case 2:
		if (remaining >= 2)
		{
			if (rear)
			{
				sound_mixbins_add(mixbins, rear_left, *(real *)&attenuation);
				sound_mixbins_add(mixbins, rear_right, *(real *)&attenuation);
			}
			else
			{
				sound_mixbins_add(mixbins, left, *(real *)&attenuation);
				sound_mixbins_add(mixbins, right, *(real *)&attenuation);
			}
		}
		break;
	case 3:
		if (rear)
		{
			if (remaining >= 2)
			{
				sound_mixbins_add(mixbins, 2, *(real *)&attenuation);
				sound_mixbins_add(mixbins, 3, *(real *)&attenuation);
			}
		}
		else if (remaining >= 4)
		{
			union s_decibel { real value; long bits; } level;
			level.bits = attenuation;
			real &attenuated = level.value;
			attenuated -= 6.0206f;
			sound_mixbins_add(mixbins, left, attenuated);
			sound_mixbins_add(mixbins, left, g_44f710);
			level.bits = attenuation;
			attenuated -= 6.0206f;
			sound_mixbins_add(mixbins, right, attenuated);
			sound_mixbins_add(mixbins, right, g_44f710);
		}
		break;
	}

	if (source->mixbin != NONE)
	{
		add_send(source->mixbin, mode, *(real *)&attenuation, mixbins);
	}
}

// MiniMax R (10-31): no lower score kept - a settings-gain local in add_send makes the mode-one sends SSE
// (177 at retail's 606 bytes) but raises 0x2af800 (55 -> 79), so the original body stands.
// score 200 (was 200): Frame 0x10 versus consumed incoming homes; inherited send x87 sums remain.
// Tried (MiniMax R run 12): add_send's settings gain read into a named local per send 177 (retail's
// 606-byte length; not kept, one-use copies); the frame 0x10 versus consumed incoming homes and the tail sums remain.
// Round 55: retail copies parameters->level into the dead parameters slot with an integer move and adds it
// to the offset at entry (xmm0 = offset + slot) before the appends; the inlined add_send then copies the sum
// through edi and adds in SSE. Gains read at use 208; early named send sum 212 (direct `addss [edi + 4]`,
// no copy); early sum with gains at use 222 (frame 4); with a one-use base level 222; base level declared
// before the offset 201.
// Earlier at 208 (was 208): eight-byte frame and inherited x87 send sums differ.
// Frame 0x10 and x87 tail sums remain; level/rear/all references 221/208/231,
// pointers 221/208/231; reference early sums 212/222, all restored.
// Earlier: eight-byte frame and inherited x87 send sums differ.
// frame 0x10 versus incoming-slot reuse; inherited x87 send sums remain.
// Levels before flag 208; scalar names 208; depth one 208;
// size preference 449; shared gain loop 384.
// With a per-function precision pragma (173, not kept: pragmas other than a file-scope inline_depth
// steer the compiler) the frame is four bytes and the late level sum remains.
// Native channel bit 208; const samples 208; immutable input 208; bit/input 208;
// scoped precision pragma 173; global optimization off 229; frame pointer 225; precision/early sum 224.
// Earlier at 200 (was 208): frame 0x10 versus incoming-slot reuse; inherited x87 send sums differ.
// Entry front/rear/all gain samples 210/200/203; individual left/right/rear-left/rear-right
// samples 208/212/206/202; all samples with early send sum 215.
// Earlier at 200 (was 200): frame 0x10 and inherited x87 send sums remain; early sum with rear pair 212;
// sum after rear pair 212; accumulated sum 212; read-only input contract 200; rear reads reversed
// 200; rear before levels 201; signed-byte selector 200; unsigned-word selector 197 (three-point
// gain dropped).
// Earlier at 200 (was 200): Frame 0x10 and x87 tail sums remain. Rear pair after first/front appends 207/205;
// left/right after first 206/206; early sum with first/front/staggered rear samples 217/222/225; staggered
// rear samples 208 (fast).
// Earlier at 200 (was 200): frame 0x10 and inherited x87 sums remain; native input
// assignment or memcpy, each with late/early level sums and rear samples/direct
// rear fields, all 240. Copies grow the frame to 0x28; original inputs restored.
// Native front/rear/both input pair loops 243/275/300f; each with final level result
// 243/275/300f; native channel bit with both loops, with/without named final level,
// 300/300f. All input array views, pair loops and changed result producers restored.
// Earlier: frame 0x10 versus incoming-slot reuse; inherited x87 send sums differ.
// Earlier: frame 0x10 and inherited x87 send sums remain; early sum with rear pair 212;
// Earlier: Frame 0x10 and x87 tail sums remain. Rear pair after first/front appends 207/205;
// Earlier: frame 0x10 and inherited x87 sums remain; native input
// Live offset/base/both levels 208/208/221; live channel flag 199 (one-point gain dropped);
// live flag with offset/base/both levels 208/208/223; live levels and native flag bit 205.
// Round 330: t01-shared-front-late 319; t02-shared-rear-late 228; t03-shared-all-late 346; t04-shared-front-
// early 335; t05-shared-rear-early 234; t06-shared-all-early 357; t07-forced-shared-late 200; t08-forced-
// shared-early 212; shared helper declaration restored.
// Round 241: Complementary channel bool/byte/short/long 202/202/194/203;
// the same with four scalar lexical homes 202/202/194/203. Short complement
// gains six, below retention threshold. Original frame 0x10 and x87 sums remain.
// Round 190: Live input levels with bool/byte/short/word primary channel and entry-rear/live-rear
// observations 221/229, 221/229, 213/231, 213/231. All regress; original scalar captures, channel
// predicate and send expansion restored.
// Lane 4 r98: the mode-one sends stay x87 in every add_send form (our VC picks x87 for a memory-homed
// operand plus a field; retail adds the GPR-held level in SSE through its slot). Append decibels by const
// reference 214; plain add_send sums 212 (0x2af800 102); a local copy of the gain in add_send 212 (copy
// propagated); `real level = parameters->level; level += parameters->level_offset;` before the appends
// with the gains read at use: retail's early offset-first sum, frame 4, 222; with the const reference 232.
// Round 194: Native byte/short/word/dword channel masks with retained rear samples
// 200/198/198/199; with entry level sum 209/209/209/212. Small gains rejected;
// original scalar captures, channel decision, helpers and sum grouping restored.
// Round 203: qualified raw sends front/rear/all 196/198/194; with early level sum 209/211/207;
// native channel bit with all raw sends, late/early sum 194/207. Short-complement interactions
// front/rear/all 188/192/188; front plain full 188/592, all matches preserved, but word tests
// and aligned instruction changes regress (110/120 to 121/128). All executable forms restored.
// Round 389: actual appended-pair pointer/reference returns, signed/unsigned destinations
// and forced/ordinary inline combinations 215 each, 640/606 bytes. Original append producer,
// every caller and the frame 0x10/x87 send graph restored.
// Round 259: native output-reference append with six complete formal orders and
// pointer/reference output results: eight forms 200 each. Original helper and all calls restored.
// Round 267: retained append count dword/long 172/172; early level 177/177;
// direct rear fields 154/154; early level/direct rear 177/177. Plain 154/606 loses
// matched 0x2af7b0 (27); all count-retention forms restored. Actual pair pointer/reference
// reservation before both stores 226/226, between bin and volume 218/218; each loses
// the same caller. Original append producer and complete body restored.
// Round 185: Actual shared append provider inline at definition/all declarations, front/rear/all 200 each;
// force-inline also 200 each but dispatcher loses its match at 26; early level with either provider 212. All
// provider metadata, shared header, private append graph, input samples and body restored; frame 0x10 and x87
// tail remain.
// Round 220: One native gain-array/routing-table traversal, long table with long/dword/short/word ordinal
// 433/433/433/433; byte table 431/431/436/436. All regress and shorten away from retail; original
// input record, four appends, scalar captures, helper and complete body restored.
// Round 92: early decibels_add(offset, level) sum with gains read at use 221, operands swapped 222,
// sum before the flag 223; rear gains read at use 208; add_send sums through decibels_add 181 (0x2af800
// 55 -> 79), without its bits local 178, with the early sum 195; add_send with one settings-gain local
// used twice 177 at 606 bytes (0x2af800 79); whole-file float-consistency pragma diagnostic 205 (all
// fast). Lead: retail's entry sum (level copied through EAX into the dead parameters slot, then addss)
// and its SSE mode-one sends are /Op code generation (see 0x2aefe0).
// Round 115 (lane 6, fast): the flag as `(flags & 8) != 0` 200 (same code), with the rear gains read at use
// 208 (573 bytes); the four bins named first as long locals 200; the send sum written `base_level +
// level_offset` 200; named bins with every gain and the sum read at use 231 (545 bytes). Retail's entry sum
// (level copied through EAX into the dead parameters slot, then addss from it) is also the shape a 4-byte
// decibel struct or long-bits copy gives (as 0x2afb80's attenuation), not only /Op.
// Round 231: native scalar memcpy level/rear/channel, paired groups and all inputs
// 209/209/200/213/209/209/213; complete memmove 219. Original scalar acquisitions,
// channel decision, providers, sum and full body restored.
// Round 193 (lane 3, fast; control 200, 0x2af800 55): the send sum through decibels_add at the top with
// the rear locals 208 (the sum sinks to the call); the level staged as long bits and the offset sum named at
// the top, rear locals kept 201; the same with every gain read at use 212 (565 B: retail's entry exactly, the
// level bits and the sum sharing the dead parameters slot, but one extra 4-byte slot and x87 mode-one sends);
// that with one union reused for both mode-one sends in add_send 212 (0x2af800 55). Diagnostic only: that
// entry with add_send's per-send settings-gain locals (a reverted form) 193 (0x2af800 44; the SSE adds load
// the union first, retail the gain). That entry with the four mix-bin numbers as named long locals 212.
// @retail 0x2af920
void c_sound_effects::add_sends(s_sound_send_parameters *parameters, long unused, long mode, s_mixbin_settings *mixbins)
{
	bool alternate = TEST_FLAG(parameters->flags, 3) != 0;
	real level_offset = parameters->level_offset;
	real base_level = parameters->level;
	real rear_left_gain = parameters->rear_left_gain;
	real rear_right_gain = parameters->rear_right_gain;

	sound_stream_append_mixbin(mixbins, alternate ? 0 : 6, parameters->left_gain);
	sound_stream_append_mixbin(mixbins, alternate ? 1 : 7, parameters->right_gain);
	sound_stream_append_mixbin(mixbins, alternate ? 4 : 8, rear_left_gain);
	sound_stream_append_mixbin(mixbins, alternate ? 5 : 9, rear_right_gain);
	c_sound_effects::add_send(10, mode, level_offset + base_level, mixbins);
}
