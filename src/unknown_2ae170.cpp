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
	memset(codec->m_packet_status, 0, sizeof(codec->m_packet_status));
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
	long size = (long)SOUND_CHUNK_SIZE(chunk) - 0x48;
	long volatile offset = stream->offset;

	if (offset > MAX(size, 0)) offset = MAX(size, 0);

	packet->pvBuffer = data + offset;
	packet->hCompletionEvent = (HANDLE)chunk;
	packet->pdwCompletedSize = NULL;
	packet->pdwStatus = NULL;
	packet->prtTimestamp = NULL;
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
	dword flags;
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


/* Retail stream mixing expands the mix-bin append operation here. */
__forceinline void sound_stream_append_mixbin(s_mixbin_settings *settings, long mixbin, real decibels)
{
	settings->pairs[settings->mixbins.dwMixBinCount].dwMixBin = mixbin;
	settings->pairs[settings->mixbins.dwMixBinCount].lVolume = sound_decibels_to_volume(decibels);
	settings->mixbins.dwMixBinCount++;
}

// @retail 0x2aeb20
void c_sound_effects::initialize(s_sound_effect_settings *settings, s_sound_effect_buffers *buffers, bool rear)
{
	s_mixbin_settings mixbins;

	m_buffers = buffers;
	m_settings = *settings;

	sound_mixbins_initialize(&mixbins);
	sound_stream_append_mixbin(&mixbins, 6, m_settings.level00);
	sound_stream_append_mixbin(&mixbins, 8, rear ? m_settings.level04 : -64.0f);
	sound_stream_append_mixbin(&mixbins, 7, -64.0f);
	sound_stream_append_mixbin(&mixbins, 9, -64.0f);
	IDirectSoundBuffer_SetMixBinVolumes(m_buffers->buffer0c, &mixbins.mixbins);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer0c, 0);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer18, -6400);

	sound_mixbins_initialize(&mixbins);
	sound_stream_append_mixbin(&mixbins, 6, -64.0f);
	sound_stream_append_mixbin(&mixbins, 8, -64.0f);
	sound_stream_append_mixbin(&mixbins, 7, m_settings.level00);
	sound_stream_append_mixbin(&mixbins, 9, rear ? m_settings.level04 : -64.0f);
	IDirectSoundBuffer_SetMixBinVolumes(m_buffers->buffer10, &mixbins.mixbins);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer10, 0);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer1c, -6400);

	sound_mixbins_initialize(&mixbins);
	sound_stream_append_mixbin(&mixbins, 6, -64.0f);
	sound_stream_append_mixbin(&mixbins, 8, -64.0f);
	sound_stream_append_mixbin(&mixbins, 7, -64.0f);
	sound_stream_append_mixbin(&mixbins, 9, -64.0f);
	sound_stream_append_mixbin(&mixbins, 10, 0.0f);
	IDirectSoundBuffer_SetMixBinVolumes(m_buffers->buffer20, &mixbins.mixbins);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer20, 0);
	IDirectSoundBuffer_SetVolume(m_buffers->buffer14, -6400);
}

/* the level of the effect sends: none in mode 0 */
inline real const &sound_effects_send_level(c_sound_effects *effects, s_sound_send_parameters *parameters, long mode)
{
	return mode == 1 ? (TEST_FLAG(parameters->flags, 6) ? g_44f70c : effects->m_settings.level2c) : effects->m_settings.level10;
}

// @retail 0x2aefe0
void c_sound_effects::add_effect_sends_direct(s_sound_send_parameters *parameters, bool rear, long mode, s_mixbin_settings *mixbins)
{
	if (TEST_FLAG(parameters->flags, 2))
	{
		volatile bool alternate = TEST_FLAG(parameters->flags, 3) != 0;
		real level = rear ? m_settings.level1c : m_settings.level24;
		real level2 = rear ? m_settings.level20 : m_settings.level28;
		sound_stream_append_mixbin(mixbins, alternate ? 0 : 6, level + parameters->level);
		sound_stream_append_mixbin(mixbins, alternate ? 1 : 7, parameters->level + level);
		add_send(2, mode, level2 + parameters->level, mixbins);
		add_send(10, mode, parameters->level_offset, mixbins);
	}
	else
	{
		real send_level = sound_effects_send_level(this, parameters, mode);
		real offset = parameters->level_offset + parameters->level;
		if (!TEST_FLAG(parameters->flags, 3))
		{
			sound_mixbins_add(mixbins, m_buffers->mixbin00, parameters->gain0c + (send_level + parameters->level));
			sound_mixbins_add(mixbins, m_buffers->mixbin04, parameters->gain10 + (parameters->level + send_level));
			c_sound_effects::add_send(m_buffers->mixbin08, mode, offset, mixbins);
		}
		else
		{
			if (m_settings.level00 > -64.0f)
			{
				sound_mixbins_add(mixbins, 0, parameters->gain0c + (m_settings.level00 + (parameters->level + send_level)));
				sound_mixbins_add(mixbins, 1, parameters->gain10 + (m_settings.level00 + (parameters->level + send_level)));
			}
			if (rear && m_settings.level04 > -64.0f)
			{
				sound_mixbins_add(mixbins, 4, parameters->gain0c + (parameters->level + send_level + m_settings.level04));
				sound_mixbins_add(mixbins, 5, parameters->gain10 + (parameters->level + send_level + m_settings.level04));
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
	real send_level = sound_effects_send_level(this, parameters, mode);
	real offset = parameters->level_offset;
	real level = parameters->level;
	bool alternate = TEST_FLAG(parameters->flags, 3) != 0;
	long left = alternate ? 0 : 6;
	long right = alternate ? 1 : 7;
	long rear_left = alternate ? 4 : 8;
	long rear_right = alternate ? 5 : 9;

	if (m_settings.level08 > -64.0f)
	{
		sound_stream_append_mixbin(mixbins, left, parameters->gain0c + (m_settings.level08 + (send_level + level)));
		sound_stream_append_mixbin(mixbins, right, parameters->gain10 + (m_settings.level08 + (level + send_level)));
	}
	if (rear && m_settings.level0c > -64.0f)
	{
		sound_stream_append_mixbin(mixbins, rear_left, parameters->gain0c + (m_settings.level0c + (level + send_level)));
		sound_stream_append_mixbin(mixbins, rear_right, parameters->gain10 + (m_settings.level0c + (level + send_level)));
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
	if (TEST_FLAG(parameters->flags, 0))
	{
		if (!TEST_FLAG(parameters->flags, 4))
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
		volatile s_send_level first;
		first.bits = gain_bits;
		first.value = m_settings.gain + first.value;
		sound_stream_append_mixbin(mixbins, mixbin, first.value);
		volatile s_send_level second;
		second.bits = gain_bits;
		second.value = m_settings.gain + second.value;
		sound_stream_append_mixbin(mixbins, mixbin, second.value);
		break;
	}
	}
}

// @retail 0x2afb80
void c_sound_effects::add_source_mixbins(s_sound_mixbin_source *source, bool rear, long mode, s_mixbin_settings *mixbins)
{
	bool alternate = ((source->flags >> 3) & 1) != 0;
	long remaining = 8 - mixbins->mixbins.dwMixBinCount;
	real gain = 0.0f;
	long left = alternate ? 0 : 6;
	long right = alternate ? 1 : 7;
	long rear_left = alternate ? 4 : 8;
	long rear_right = alternate ? 5 : 9;

	switch (source->speaker_mode)
	{
	case 0:
		break;
	case 1:
		if (remaining >= 2)
		{
			sound_mixbins_add(mixbins, left, gain);
			sound_mixbins_add(mixbins, right, gain);
		}
		break;
	case 2:
		if (remaining >= 2)
		{
			if (rear)
			{
				sound_mixbins_add(mixbins, rear_left, gain);
				sound_mixbins_add(mixbins, rear_right, gain);
			}
			else
			{
				sound_mixbins_add(mixbins, left, gain);
				sound_mixbins_add(mixbins, right, gain);
			}
		}
		break;
	case 3:
		if (rear)
		{
			if (remaining >= 2)
			{
				sound_mixbins_add(mixbins, 2, gain);
				sound_mixbins_add(mixbins, 3, gain);
			}
		}
		else if (remaining >= 4)
		{
			sound_mixbins_add(mixbins, left, gain - 6.0206f);
			sound_mixbins_add(mixbins, left, g_44f710);
			sound_mixbins_add(mixbins, right, gain - 6.0206f);
			sound_mixbins_add(mixbins, right, g_44f710);
		}
		break;
	default:
		break;
	}

	if (source->mixbin != NONE)
	{
		add_send(source->mixbin, mode, gain, mixbins);
	}
}

// @retail 0x2af920
void c_sound_effects::add_sends(s_sound_send_parameters *parameters, long unused, long mode, s_mixbin_settings *mixbins)
{
	bool alternate = TEST_FLAG(parameters->flags, 3) != 0;
	real volatile level = parameters->level_offset + parameters->level;

	sound_stream_append_mixbin(mixbins, alternate ? 0 : 6, *(real const volatile *)&parameters->left_gain);
	sound_stream_append_mixbin(mixbins, alternate ? 1 : 7, *(real const volatile *)&parameters->right_gain);
	sound_stream_append_mixbin(mixbins, alternate ? 4 : 8, *(real const volatile *)&parameters->rear_left_gain);
	sound_stream_append_mixbin(mixbins, alternate ? 5 : 9, *(real const volatile *)&parameters->rear_right_gain);
	c_sound_effects::add_send(10, mode, level, mixbins);
}
