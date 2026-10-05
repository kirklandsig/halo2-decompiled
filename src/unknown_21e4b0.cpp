// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "sound_driver.h"
#include "globals.h"
#include "crc.h"
#include <float.h>
#include <stddef.h>
#include <string.h>

struct s_sound_effect_buffers;
struct s_sound_i3dl2_parameters;
struct s_sound_send_parameters;
struct s_sound_mixbin_source;
struct s_mixbin_settings;

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

c_sound_effects g_47f088;

struct s_47f0d0
{
	void *vtable;
	long field04;
	long field08;
	long field0c;
	long field10;
	void *field14;
};

extern s_47f0d0 g_47f0d0;
extern dword g_510800_pool_base;
extern long g_510804_pool_size;
extern dword g_510808_pool_checksum;

struct s_sound_listener_environment;
struct s_sound_listener
{
	point3f position;
	vector3f forward;
	vector3f up;
	byte unknown24[0xc];
	long environment_count;
	s_sound_listener_environment const *environments;
};

struct s_sound_driver_counts
{
	short channels[3];
	byte unknown06[6];
	short voices;
};

typedef char check_effects_instance_size[sizeof(c_sound_effects) == 0x48 ? 1 : -1];
typedef char check_codec_view_size[sizeof(s_47f0d0) == 0x18 ? 1 : -1];
typedef char check_driver_voice_count_offset[offsetof(s_sound_driver_counts, voices) == 0xc ? 1 : -1];

bool function_191300(LPDIRECTSOUND direct_sound);
void function_191420(LPDIRECTSOUND direct_sound);
void function_12c040(void);
void function_21eae0(void);
void function_21e3b0(c_sound_driver_effects *effects);
bool function_21f4e0(long channel_index, long type);
void function_220790(s_sound_listener const *listener);
bool sound_driver_voice_create_buffer(long voice_index);
void function_221490(LPDIRECTSOUNDBUFFER *buffer_reference);

// @retail 0x21e4b0
bool function_21e4b0(s_sound_driver_counts const *counts)
{
	byte *top = (byte *)(g_510800_pool_base + g_510804_pool_size);
	byte *memory = (byte *)(((dword)top + 3) & ~3);
	long allocated_size = (memory - top) + sizeof(s_sound_driver_globals);
	bool result = false;
	g_510804_pool_size += allocated_size;
	dword checksum = g_510808_pool_checksum;
	function_163ba0(&checksum, &allocated_size, sizeof(allocated_size));
	g_510808_pool_checksum = checksum;
	memset(memory, 0, sizeof(s_sound_driver_globals));
	g_51ebe4 = (s_bink_sound_settings *)memory;
	SOUND_DRIVER_GLOBALS->unknown0000 = false;
	if (SUCCEEDED(DirectSoundCreate(NULL, &SOUND_DRIVER_GLOBALS->direct_sound, NULL)) &&
		SUCCEEDED(SOUND_DRIVER_GLOBALS->direct_sound->GetCaps(&SOUND_DRIVER_GLOBALS->caps)) &&
		SUCCEEDED(SOUND_DRIVER_GLOBALS->direct_sound->SetDistanceFactor(3.048f, DS3D_IMMEDIATE)) &&
		SUCCEEDED(SOUND_DRIVER_GLOBALS->direct_sound->SetDopplerFactor(0.0f, DS3D_IMMEDIATE)))
	{
		DWORD speaker_config;
		if (SUCCEEDED(SOUND_DRIVER_GLOBALS->direct_sound->GetSpeakerConfig(&speaker_config)))
		{
			SOUND_DRIVER_GLOBALS->surround = (speaker_config & 0xffff) == 2 && (speaker_config & 0x10000);
			for (long mixbin = 0; mixbin <= 31; mixbin++)
				SOUND_DRIVER_GLOBALS->direct_sound->SetMixBinHeadroom(mixbin, 0);
			SOUND_DRIVER_GLOBALS->direct_sound->SetRolloffFactor(0.0f, DS3D_DEFERRED);
			DirectSoundDoWork();
			if (function_191300(SOUND_DRIVER_GLOBALS->direct_sound))
			{
				s_sound_listener listener;
				memset(&listener, 0, sizeof(listener));
				listener.forward = *g_4687a8;
				listener.up = *g_4687b0;
				function_220790(&listener);
				result = true;
				function_12c040();
				g_47f0d0.field04 = 0;
				g_47f0d0.field14 = NULL;
				g_47f0d0.field10 = 0x10000;
				for (long type = 0; type < 3; type++)
				{
					long first = SOUND_DRIVER_GLOBALS->channel_count;
					SOUND_DRIVER_GLOBALS->unknown1a0c[type] = SOUND_DRIVER_GLOBALS->channel_count;
					SOUND_DRIVER_GLOBALS->channel_count += counts->channels[type];
					for (long i = 0; result && i < counts->channels[type]; i++)
						result = result && function_21f4e0(first + i, type);
					SOUND_DRIVER_GLOBALS->unknown1a12[type] = SOUND_DRIVER_GLOBALS->channel_count - 1;
				}
				SOUND_DRIVER_GLOBALS->voice_count = counts->voices;
				SOUND_DRIVER_GLOBALS->unknown0008 = counts->voices;
				for (long voice = 0; result && voice < counts->voices; voice++)
					result = result && sound_driver_voice_create_buffer(voice);
				function_191420(SOUND_DRIVER_GLOBALS->direct_sound);
				SOUND_DRIVER_GLOBALS->effect_levels_a[0] = 0;
				SOUND_DRIVER_GLOBALS->effect_levels_a[1] = 0xc0c0a8c1;
				SOUND_DRIVER_GLOBALS->effect_levels_b[0] = 0;
				SOUND_DRIVER_GLOBALS->effect_levels_b[1] = 0xc0c0a8c1;
				((dword *)SOUND_DRIVER_GLOBALS->effect_settings)[0] = 0xc0c0a8c1;
				((dword *)SOUND_DRIVER_GLOBALS->effect_settings)[1] = 0xc0400000;
				((dword *)SOUND_DRIVER_GLOBALS->effect_settings)[2] = 0xc0c0a8c1;
				((dword *)SOUND_DRIVER_GLOBALS->effect_settings)[7] = 0xc0c0a8c1;
				SOUND_DRIVER_GLOBALS->effect_mixbins[0] = 23;
				SOUND_DRIVER_GLOBALS->effect_mixbins[1] = 24;
				SOUND_DRIVER_GLOBALS->effect_mixbins[2] = 25;
				for (long send = 0; send < 3; send++)
				{
					long mixbin = SOUND_DRIVER_GLOBALS->effect_mixbins[send];
					LPDIRECTSOUNDBUFFER *buffer = &SOUND_DRIVER_GLOBALS->effect_buffers_a[send];
					LPDIRECTSOUNDBUFFER *spatial_buffer = &SOUND_DRIVER_GLOBALS->effect_buffers_b[send];
					DSBUFFERDESC description = {0};
					description.dwSize = sizeof(description);
					description.dwFlags = DSBCAPS_FXIN2;
					description.dwInputMixBin = mixbin;
					if (SUCCEEDED(SOUND_DRIVER_GLOBALS->direct_sound->CreateSoundBuffer(&description, buffer, NULL)))
						(*buffer)->Play(0, 0, 0);
					DSBUFFERDESC spatial_description = {0};
					spatial_description.dwSize = sizeof(spatial_description);
					spatial_description.dwFlags = DSBCAPS_FXIN2 | DSBCAPS_CTRL3D;
					spatial_description.dwInputMixBin = mixbin;
					if (SUCCEEDED(SOUND_DRIVER_GLOBALS->direct_sound->CreateSoundBuffer(&spatial_description, spatial_buffer, NULL)))
					{
						(*spatial_buffer)->SetMaxDistance(FLT_MAX, DS3D_DEFERRED);
						(*spatial_buffer)->SetMinDistance(FLT_MAX, DS3D_DEFERRED);
						(*spatial_buffer)->Play(0, 0, 0);
					}
				}
				function_21e3b0((c_sound_driver_effects *)&g_47f088);
				long input_mixbins[8] = {15, 16, 17, 18, 19, 20, 21, 22};
				long output_mixbins[4] = {6, 7, 8, 9};
				for (long group = 0; group < 2; group++)
				{
					for (long i = 0; i < 4; i++)
					{
						LPDIRECTSOUNDBUFFER *buffer = &SOUND_DRIVER_GLOBALS->submix_buffers[group * 4 + i];
						DSBUFFERDESC description = {0};
						description.dwSize = sizeof(description);
						description.dwFlags = DSBCAPS_FXIN2;
						description.dwInputMixBin = input_mixbins[group * 4 + i];
						if (SUCCEEDED(SOUND_DRIVER_GLOBALS->direct_sound->CreateSoundBuffer(&description, buffer, NULL)))
						{
							(*buffer)->SetVolume(0);
							DSMIXBINVOLUMEPAIR pair;
							DSMIXBINS bins;
							pair.dwMixBin = output_mixbins[i];
							pair.lVolume = 0;
							bins.dwMixBinCount = 1;
							bins.lpMixBinVolumePairs = &pair;
							(*buffer)->SetMixBins(&bins);
							(*buffer)->Play(0, 0, 0);
						}
					}
				}
				for (long impulse = 0; impulse < 2; impulse++)
				{
					function_221490(&SOUND_DRIVER_GLOBALS->impulse_buffers[impulse]);
					SOUND_DRIVER_GLOBALS->impulse_ids[impulse] = NONE;
				}
				SOUND_DRIVER_GLOBALS->volume_a = 1.0f;
				SOUND_DRIVER_GLOBALS->volume_b = 1.0f;
				SOUND_DRIVER_GLOBALS->volume_c = 1.0f;
				SOUND_DRIVER_GLOBALS->volume_d = 1.0f;
				SOUND_DRIVER_GLOBALS->unknown2acc = 8000;
				SOUND_DRIVER_GLOBALS->unknown2ad0 = 0.0f;
			}
		}
	}
	if (result)
		SOUND_DRIVER_GLOBALS->unknown0000 = true;
	else
		function_21eae0();
	return result;
}
