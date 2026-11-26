// @flags /O2 /Ob1 /arch:SSE /Gr
/* NETWORK_VOICE.CPP: the voice chat (lane D): the XHV engine and its
   callbacks (0x476fc8), the voice globals (0x4c9878), the ports' processing
   modes and voice masks, the remote talkers and the voice mail */

#include "unknown_11c920.h"
#include <xtl.h>
#include <xonline.h>
#include <xhv.h>
#include <float.h>
#include <xmmintrin.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include "globals.h"
#include "unknown_059ad0.h"
#include "network_voice.h"
#include "unknown_067e10.h"
#include "unknown_058ee0.h"
#include "crc.h"
#include "bit_vector.h"

void function_caf90(long unit_index, point3f *position);
bool juggernaut_is(short player_index);

c_voice_xhv g_476fc8;
s_voice_globals g_4c9878;

XHV_PROCESSING_MODE g_52731c[4];

/* the voice masks a port can use */
const XHV_VOICE_MASK g_43fe48[2] = { XHV_VOICE_MASK_NONE, XHV_VOICE_MASK_ANONYMOUS };

/* src/unknown_059670.cpp */
bool function_59670(c_class_58d20 **session);
bool function_596a0(c_class_58d20 **session);

static inline bool voice_available(void)
{
	return *(volatile bool *)&g_4c9878.initialized && *(volatile bool *)&g_476fc8.initialized;
}

/* the voice mode, 0 while voice is unavailable */
static inline long voice_get_mode(void)
{
	long result = 0;
	if (g_4c9878.initialized && g_476fc8.initialized)
		result = g_4c9878.mode;
	return result;
}

static inline long voice_port_next(long port)
{
	long next;
	switch (port)
	{
	case NONE:
		next = 0;
		break;
	case 0:
		next = 1;
		break;
	case 1:
		next = 2;
		break;
	case 2:
		next = 3;
		break;
	default:
		next = NONE;
		break;
	}
	return next;
}

static inline long voice_port_next_index(long port)
{
	long next = NONE;
	if (port >= 0 && port < 3)
		next = port + 1;
	return next;
}

static inline XUID voice_xuid(long id)
{
	XUID xuid;
	memset(&xuid, 0, sizeof(xuid));
	xuid.qwUserID = id;
	return xuid;
}

/* ---- the engine (members of the callback object) ---- */

// @retail 0x556a0
void voice_xhv_reset_masks(c_voice_xhv *xhv)
{
	for (long port = 0; port != NONE; port = voice_port_next(port))
	{
		xhv->masks[port].fRoboticValue = XHV_VOICE_MASK_PARAM_DISABLED;
		xhv->masks[port].fWhisperValue = XHV_VOICE_MASK_PARAM_DISABLED;
		xhv->masks[port].fPitchScale = XHV_VOICE_MASK_PARAM_DISABLED;
		xhv->masks[port].fSpecEnergyWeight = XHV_VOICE_MASK_PARAM_DISABLED;
	}
}

// @retail 0x55720
void voice_xhv_reset_port_modes(c_voice_xhv *xhv)
{
	long mode = 2;
	switch (xhv->mode)
	{
	case 1:
		mode = 3;
		break;
	}
	for (long port = 0; port != NONE; port = voice_port_next(port))
	{
		xhv->port_modes[port] = mode;
	}
}

// @retail 0x55780
bool voice_xhv_get_runtime_parameters(XHV_RUNTIME_PARAMS *parameters, c_voice_xhv *xhv)
{
	bool result = false;
	DSEFFECTIMAGEDESC *effects = g_510c90->description;
	if (effects)
	{
		DWORD remote_talkers = 0;
		DWORD compressed_buffers = 0;
		memset(parameters, 0, sizeof(*parameters));
		c_voice_xhv *const *reference = &xhv;
		if ((*reference)->mode == 2)
		{
			remote_talkers = 15;
			compressed_buffers = 5;
		}
		parameters->dwMaxRemoteTalkers = remote_talkers;
		parameters->dwMaxLocalTalkers = 4;
		parameters->dwMaxCompressedBuffers = compressed_buffers;
		parameters->dwFlags = 0;
		parameters->pEffectImageDesc = effects;
		parameters->dwEffectsStartIndex = 11;
		parameters->dwOutOfSyncThreshold = 32;
		parameters->bCustomVADProvided = FALSE;
		parameters->bHeadphoneAlwaysOn = FALSE;
		result = true;
	}
	return result;
}

// @retail 0x55810
bool voice_xhv_create(c_voice_xhv *xhv)
{
    XHV_RUNTIME_PARAMS parameters;
    bool result = voice_xhv_get_runtime_parameters(&parameters, xhv);
    if (result)
    {
        XHVEngine **engine = &xhv->engine;
        HRESULT error = XHVEngineCreate(&parameters, engine);
        if (!SUCCEEDED(error))
            return result;
        xhv->unknown0c = 0;
        switch (xhv->mode)
        {
        case 1:
            xhv->unknown0c = 0xb;
            break;
        default:
            xhv->unknown0c = 0xf;
            break;
        }
        for (DWORD mode = 0; mode < 4; mode++)
        {
            if (xhv->unknown0c & (1 << mode))
                error = (*engine)->EnableProcessingMode(g_52731c[mode]);
        }
        if (SUCCEEDED(error))
        {
            error = (*engine)->SetCallbackInterface(xhv);
            if (xhv->unknown0c & 4)
                (*engine)->SetMaxPlaybackStreamsCount(15);
            if (SUCCEEDED(error))
            {
                for (long port = 0; port != NONE; port = voice_port_next_index(port))
                {
                    XHV_LOCAL_TALKER_STATUS status;
                    (*engine)->RegisterLocalTalker(port);
                    (*engine)->SetProcessingMode(port, g_52731c[xhv->port_modes[port]]);
                    (*engine)->GetLocalTalkerStatus(port, &status);
                    xhv->communicator_present[port] = status.communicatorStatus == XHV_VOICE_COMMUNICATOR_STATUS_INSERTED;
                    xhv->chat_data_ready[port] = false;
                    xhv->voice_mail_active[port] = false;
                }
                result = true;
            }
        }
    }
    return result;
}

// @retail 0x55080
void voice_xhv_initialize(c_voice_xhv *xhv, long mode)
{
	if (!xhv->initialized)
	{
		xhv->engine = NULL;
		xhv->mode = mode;
		voice_xhv_reset_masks(xhv);
		voice_xhv_reset_port_modes(xhv);
		xhv->initialized = voice_xhv_create(xhv);
	}
}

// @retail 0x552a0
void voice_xhv_unregister_remote_talkers(c_voice_xhv *xhv)
{
	if (xhv->initialized && (xhv->unknown0c & 4))
	{
		DWORD count;
		XUID talkers[30];
		if (SUCCEEDED(xhv->engine->GetRemoteTalkers(&count, talkers)))
		{
			for (DWORD i = 0; i < count; i++)
			{
				if (xhv->initialized && (xhv->unknown0c & 4))
					xhv->engine->UnregisterRemoteTalker(voice_xuid((long)talkers[i].qwUserID));
			}
		}
	}
}

// @retail 0x550b0
void voice_xhv_dispose(c_voice_xhv *xhv)
{
	if (xhv->initialized)
	{
		for (long port = 0; port != NONE; port = voice_port_next(port))
		{
			xhv->engine->UnregisterLocalTalker(port);
		}
		if (xhv->unknown0c & 4)
			voice_xhv_unregister_remote_talkers(xhv);
		if (xhv->engine)
		{
			xhv->engine->Release();
			xhv->engine = NULL;
		}
	}
	xhv->initialized = false;
}

// @retail 0x55130
bool voice_xhv_has_remote_talker(c_voice_xhv *xhv, long id)
{
	bool found = false;
	if (xhv->initialized && (xhv->unknown0c & 4))
	{
		DWORD count;
		XUID talkers[30];
		if (SUCCEEDED(xhv->engine->GetRemoteTalkers(&count, talkers)))
		{
			XUID xuid = voice_xuid(id);
			for (DWORD i = 0; !found && i < count; i++)
			{
				found = XOnlineAreUsersIdentical(&xuid, &talkers[i]);
			}
		}
	}
	return found;
}

// @retail 0x551c0
bool voice_xhv_is_talking(c_voice_xhv *xhv, long id)
{
	bool result = false;
	if (xhv->initialized && (xhv->unknown0c & 4))
	{
		XUID xuid = voice_xuid(id);
		result = xhv->engine->IsTalking(xuid) != FALSE;
	}
	return result;
}

// @retail 0x55210
void voice_xhv_set_voice_mask(c_voice_xhv *xhv, long port, const XHV_VOICE_MASK *mask)
{
	if (xhv->initialized)
	{
		if (SUCCEEDED(xhv->engine->SetVoiceMask(port, mask)))
			xhv->masks[port] = *mask;
	}
}

// @retail 0x55250
void voice_xhv_set_mix_bins(c_voice_xhv *xhv, long id, DWORD port, const DSMIXBINS *bins)
{
	if (xhv->initialized)
	{
		XUID xuid = voice_xuid(id);
		xhv->engine->SetMixBinMapping(xuid, port, bins);
	}
}

// @retail 0x55330
bool voice_xhv_set_remote_talker(c_voice_xhv *xhv, long id, bool registered)
{
	bool result = false;
	if (xhv->initialized && (xhv->unknown0c & 4))
	{
		if (registered)
		{
			XUID xuid = voice_xuid(id);
			if (SUCCEEDED(xhv->engine->RegisterRemoteTalker(xuid)))
				result = true;
		}
		else
		{
			XUID xuid = voice_xuid(id);
			if (SUCCEEDED(xhv->engine->UnregisterRemoteTalker(xuid)))
				result = true;
		}
	}
	return result;
}

// @retail 0x554c0
void voice_xhv_play_voice_mail(DWORD port, c_voice_xhv *controller, const long *data, long size, bool force)
{
	c_voice_xhv *const *controller_reference = &controller;
	c_voice_xhv *xhv = *controller_reference;
	if (xhv->initialized && xhv->port_modes[port] == 3 && size > 16 && data[0] == 'FFIR' && data[2] == 'EVAW' && data[3] == ' tmf')
	{
		if (force || xhv->communicator_present[port])
		{
			if (SUCCEEDED(xhv->engine->VoiceMailPlay(port, size, (const BYTE *)data, force)))
				xhv->voice_mail_active[port] = true;
		}
	}
}

// @retail 0x55520
void voice_xhv_record_voice_mail(DWORD port, c_voice_xhv *xhv, DWORD maximum_time, BYTE *buffer, DWORD buffer_size, DWORD *size, DWORD *duration)
{
	*size = 0;
	*duration = 0;
	memset(buffer, 0, buffer_size);
	if (xhv->initialized && xhv->port_modes[port] == 3 && xhv->communicator_present[port])
	{
		xhv->voice_mail_sizes[port] = NULL;
		xhv->voice_mail_durations[port] = NULL;
		if (SUCCEEDED(xhv->engine->VoiceMailRecord(port, maximum_time, buffer_size, buffer)))
		{
			xhv->voice_mail_sizes[port] = size;
			xhv->voice_mail_durations[port] = duration;
			xhv->voice_mail_active[port] = true;
		}
	}
}

/* ---- the callbacks ---- */

void __stdcall function_53a20(DWORD port, VOID *data, DWORD size);

// @retail 0x555f0
HRESULT c_voice_xhv::LocalChatDataReady(DWORD port, DWORD size, VOID *data)
{
	if (size > 0)
	{
		chat_data_ready[port] = true;
		function_53a20(port, data, size);
	}
	return S_OK;
}

// @retail 0x555b0
HRESULT c_voice_xhv::CommunicatorStatusUpdate(DWORD port, XHV_VOICE_COMMUNICATOR_STATUS status)
{
	if (status == XHV_VOICE_COMMUNICATOR_STATUS_INSERTED)
		communicator_present[port] = true;
	else if (status == XHV_VOICE_COMMUNICATOR_STATUS_REMOVED)
		communicator_present[port] = false;
	return S_OK;
}

// @retail 0x55640
HRESULT c_voice_xhv::VoiceMailDataReady(DWORD port, DWORD duration, DWORD size)
{
	if (voice_mail_sizes[port])
		*voice_mail_sizes[port] = size;
	if (voice_mail_durations[port])
		*voice_mail_durations[port] = duration;
	voice_mail_active[port] = false;
	return S_OK;
}

// @retail 0x55680
HRESULT c_voice_xhv::VoiceMailStopped(DWORD port)
{
	voice_mail_active[port] = false;
	return S_OK;
}

/* xhv.h's default, which the game's vtable holds */
// @retail 0x55630
HRESULT c_voice_xhv::SpeechRecognized(DWORD port, XHV_SR_ITEM *items, DWORD item_count)
{
	return E_NOTIMPL;
}

/* xhv.h's default, which the game's vtable holds */
// @retail 0x55620
HRESULT c_voice_xhv::MicrophoneRawDataReady(DWORD port, DWORD size, VOID *data, BOOL *voice_detected)
{
	return E_NOTIMPL;
}

/* ---- the voice globals ---- */

// @retail 0x537f0
long voice_get_port_mode(long port)
{
	if (g_476fc8.initialized)
		return g_476fc8.port_modes[port];
	return 0;
}

// @retail 0x54f20
long voice_get_session_kind(void)
{
	long result = 0;
	if (voice_available())
		result = g_4c9878.session_kind;
	return result;
}

// @retail 0x54d00
long voice_get_unknownEE(void)
{
	long result = 0;
	if (voice_available())
		result = g_4c9878.unknownEE;
	return result;
}

// @retail 0x54fc0
void function_54fc0(long controller_index, long voice_through_tv)
{
	if (voice_available())
		g_4c9878.port_states[controller_index] = voice_through_tv;
}

// @retail 0x54f90
long voice_get_port_state(long port)
{
	long result = 0;
	if (voice_available())
		result = g_4c9878.port_states[port];
	return result;
}

// @retail 0x54ec0
inline bool voice_is_enabled(void)
{
	bool result = false;
	if (voice_available())
		result = g_4c9878.mode != 0;
	return result;
}

// @retail 0x54ef0
bool voice_mode_is_1(void)
{
	bool result = false;
	if (voice_available())
		result = g_4c9878.mode == 1;
	return result;
}

// @retail 0x53db0
bool voice_data_is_wave(const long *data, long size)
{
	bool result = false;
	if (size > 16 && data[0] == 'FFIR' && data[2] == 'EVAW' && data[3] == ' tmf')
		result = true;
	return result;
}

// @retail 0x53f50
void voice_fpu_enter(void)
{
	_control87(0x9001f, 0x8001f);
	_mm_setcsr(_mm_getcsr() | 0x1f80);
}

// @retail 0x53f80
void voice_fpu_leave(void)
{
	_mm_setcsr(_mm_getcsr() & ~0x3f);
	_clearfp();
	_control87(0x9001f, 0xfffff);
}

// @retail 0x53940
void voice_mail_stop(long port)
{
	if (voice_available())
	{
		if (g_476fc8.port_modes[port] == 3)
			g_476fc8.engine->VoiceMailStop(port);
		g_476fc8.voice_mail_active[port] = false;
	}
}

// @retail 0x539e0
void voice_mail_stop_if_present(long port)
{
	if (voice_available())
	{
		if (g_476fc8.port_modes[port] == 3 && g_476fc8.communicator_present[port])
			g_476fc8.engine->VoiceMailStop(port);
		g_476fc8.voice_mail_active[port] = false;
	}
}

// @retail 0x537b0
void voice_set_port_mode(long port, long mode)
{
	if (voice_available())
	{
		if (SUCCEEDED(g_476fc8.engine->SetProcessingMode(port, g_52731c[mode])))
			g_476fc8.port_modes[port] = mode;
	}
}

// @retail 0x53810
void function_53810(long voice_mask, long controller_index)
{
	if (voice_available())
		voice_xhv_set_voice_mask(&g_476fc8, controller_index, &g_43fe48[voice_mask]);
}

// @retail 0x53850
bool voice_test_unknownF0(long port, long bit)
{
	bool result = false;
	if (voice_available())
		result = (g_4c9878.unknownF0[port] & (1 << bit)) != 0;
	return result;
}

// @retail 0x53720
bool voice_has_remote_talker(long id)
{
	bool result = false;
	if (voice_available())
		result = voice_xhv_has_remote_talker(&g_476fc8, id);
	return result;
}

// @retail 0x53c30
bool voice_get_session(c_class_58d20 **session)
{
	bool result = false;
	if (voice_is_enabled())
	{
		switch (g_4c9878.session_kind)
		{
		case 1:
			result = function_59670(session);
			break;
		case 2:
			result = function_596a0(session);
			break;
		}
	}
	return result;
}

static inline void *voice_session_get_membership(c_class_58d20 *session)
{
	void *result = NULL;
	if (session->state && session->value4c != NONE)
		result = &session->value4c;
	return result;
}

static inline long voice_session_get_current_member(c_class_58d20 *session)
{
	long result = NONE;
	if (session->state && session->value4c != NONE)
		result = session->current_member;
	return result;
}

static inline long voice_session_get_member_index(c_class_58d20 *session)
{
	long result = NONE;
	if (session->state && session->value4c != NONE)
		result = session->member_index;
	return result;
}

// @retail 0x53de0
void *voice_get_membership(void)
{
	void *result = NULL;
	if (voice_available())
	{
		c_class_58d20 *session = NULL;
		if (voice_get_session(&session))
			result = voice_session_get_membership(session);
	}
	return result;
}

// @retail 0x53be0
dword voice_get_port_flags(long port)
{
	dword result = 0;
	if (voice_available())
	{
		dword mask = 0;
		byte *membership = (byte *)voice_get_membership();
		if (membership)
			mask = *(dword *)(membership + 0x10d0);
		if (mask & (1 << port))
			result = g_4c9878.unknown110[port];
	}
	return result;
}

// @retail 0x53b90
inline bool voice_port_flag1(long port)
{
	bool result = false;
	if (voice_available())
		result = (voice_get_port_flags(port) >> 1) & 1;
	return result;
}

// @retail 0x53bb0
bool voice_port_flag2(long port)
{
	bool result = false;
	if (voice_available())
		result = (voice_get_port_flags(port) >> 2) & 1;
	return result;
}

// @retail 0x53b40
bool voice_port_flag0_only(long port)
{
	bool result = false;
	if (voice_available())
	{
		if ((voice_get_port_flags(port) & 1) && !voice_port_flag1(port))
			result = true;
		else
			return false;
	}
	return result;
}

/* src/unknown_058ee0.cpp */
bool network_session_manager_get_session(c_class_58d20 **session);

// @retail 0x547e0
void voice_update_session_kind(void)
{
	c_class_58d20 *session;
	g_4c9878.session_kind = 0;
	if (network_session_manager_get_session(&session))
		g_4c9878.session_kind = (session->value14 == 2) + 1;
}

// @retail 0x54a20
long voice_get_current_member(void)
{
	long result = NONE;
	if (voice_available())
	{
		c_class_58d20 *session = NULL;
		if (voice_get_session(&session))
			result = voice_session_get_current_member(session);
	}
	return result;
}

// @retail 0x54a70
long voice_get_member_index(void)
{
	long result = NONE;
	if (voice_available())
	{
		c_class_58d20 *session = NULL;
		if (voice_get_session(&session))
			result = voice_session_get_member_index(session);
	}
	return result;
}

// @retail 0x549d0
bool voice_current_member_is_unknown00(void)
{
	bool result = false;
	if (voice_available())
	{
		long member = voice_get_current_member();
		if (member == NONE)
			return false;
		long unknown00 = 0;
		if (voice_available())
			unknown00 = g_4c9878.unknown00;
		if (member == unknown00)
			result = true;
		else
			return false;
	}
	return result;
}

// @retail 0x54ac0
long voice_get_current_member_value(void)
{
	long result = 0;
	if (voice_available())
	{
		byte *membership = (byte *)voice_get_membership();
		long member = voice_get_current_member();
		if (membership && member != NONE)
			result = *(long *)(membership + member * 0x10c + 0xa8);
	}
	return result;
}

// @retail 0x54b10
dword voice_get_player_mask(void)
{
	dword result = 0;
	if (voice_available())
	{
		byte *membership = (byte *)voice_get_membership();
		if (membership)
			result = *(dword *)(membership + 0x10d0);
	}
	return result;
}

// @retail 0x54b40
void *voice_get_players(void)
{
	void *result = NULL;
	if (voice_available())
	{
		byte *membership = (byte *)voice_get_membership();
		if (membership)
			result = membership + 0x10d4;
	}
	return result;
}

/* ---- the pools ---- */

/* src/loop_allocator.cpp */
bool loop_allocate(s_loop_allocator *loop, void **pointer, long size, char const *file, long line);
void loop_free(s_loop_allocator *loop, void **pointer);

/* the memory source of the second pool */
c_memory_source *g_46dd54;

// @retail 0x53290
void *voice_allocate(long size, long attributes)
{
	void *result = NULL;
	long const *unused = &attributes;
	if (!size)
		size = 0x200;
	if (g_4c9878.use_pool2)
		loop_allocate(g_4c9878.pool2, &result, size, NULL, 0);
	else
		loop_allocate(g_4c9878.pool, &result, size, NULL, 0);
	return result;
}

// @retail 0x532e0
void voice_free(void *pointer, long attributes)
{
	long const *unused = &attributes;
	if (pointer)
	{
		if (g_4c9878.use_pool2)
			loop_free(g_4c9878.pool2, &pointer);
		else
			loop_free(g_4c9878.pool, &pointer);
	}
}

// @retail 0x53510
void voice_start_engine(void)
{
	if (g_4c9878.initialized)
	{
		long mode = g_4c9878.pool_mode;
		if (mode == 2 || g_4e6948->state == 1 && mode == 1)
		{
			if (!g_476fc8.initialized)
				voice_xhv_initialize(&g_476fc8, mode);
		}
	}
}

// @retail 0x53550
inline void voice_stop_engine(void)
{
	if (voice_available())
	{
		voice_xhv_dispose(&g_476fc8);
		g_4c9878.unknownEE = 0;
	}
}

PRIVATE __forceinline s_loop_allocator *function_53421(c_memory_source *arg_0, long arg_1, const char *arg_2)
{
	if (!arg_0)
		arg_0 = (c_memory_source *)g_468758;
	s_loop_allocator *local_0 = (s_loop_allocator *)arg_0->allocate(arg_1 + 0x50);
	if (local_0)
		function_18e250(local_0, arg_1, arg_2, arg_0);
	return local_0;
}

// @retail 0x53420
void voice_initialize_menu_pool(void)
{
	if (g_4c9878.type == 1)
	{
		g_4c9878.use_pool2 = true;
		g_4c9878.pool2 = function_53421(g_46dd54, 0x61800, "voice y menu pool");
		if (g_4c9878.pool2)
		{
			g_4c9878.pool2->field3c = true;
			g_4c9878.pool2->field3d = true;
			g_4c9878.pool2->field3e = true;
			voice_start_engine();
		}
	}
}

PRIVATE __forceinline void function_534a1(s_loop_allocator *arg_0)
{
 c_memory_source *local_0 = arg_0->source;
 memset(arg_0, 0, sizeof(*arg_0));
 local_0->release(arg_0);
}

// @retail 0x534a0
void voice_dispose_menu_pool(void)
{
	if (g_4c9878.type == 1)
	{
		if (g_4c9878.pool2)
		{
			voice_stop_engine();
			function_534a1(g_4c9878.pool2);
			g_4c9878.pool2 = NULL;
		}
		g_4c9878.use_pool2 = false;
	}
}

// @retail 0x53580
void voice_do_work(void)
{
	if (voice_available())
	{
		_control87(0x9001f, 0x8001f);
		_mm_setcsr(_mm_getcsr() | 0x1f80);
		if (g_476fc8.initialized)
		{
			memset(g_476fc8.chat_data_ready, 0, sizeof(g_476fc8.chat_data_ready));
			g_476fc8.engine->DoWork();
		}
		_mm_setcsr(_mm_getcsr() & ~0x3f);
		_clearfp();
		_control87(0x9001f, 0xfffff);
	}
}

// @retail 0x53fb0
void voice_reset_talkers(void)
{
	g_4c9878.unknownEE = 0;
	g_4c9878.unknown20 = 0;
	memset(g_4c9878.unknown24, 0, sizeof(g_4c9878.unknown24));
	memset(g_4c9878.unknownF0, 0, 0x20);
	memset(g_4c9878.unknown110, 0, 0x20);
}

/* src/unknown_190001.cpp */
bool function_1906da(long index);

// @retail 0x536b0
bool voice_port_can_talk(long port)
{
	bool present = false;
	bool not_muted = false;
	bool allowed = true;
	if (voice_available())
	{
		present = g_476fc8.communicator_present[port];
		not_muted = g_4c9878.port_states[port] != 3;
		if (TEST_FIELD_BIT(g_54e8e0[port].flag5))
			allowed = function_1906da(port);
	}
	return present && not_muted && allowed;
}

// @retail 0x538e0
void voice_play_voice_mail(long port, const long *data, long size)
{
	if (voice_available())
	{
		long state = g_4c9878.port_states[port];
		bool force = false;
		if (state == 1 || !voice_port_can_talk(port))
			force = true;
		if (state != 3 && data && size > 0)
			voice_xhv_play_voice_mail(port, &g_476fc8, data, size, force);
	}
}

// @retail 0x539a0
void voice_record_voice_mail(long port, DWORD maximum_time, DWORD buffer_size, BYTE *buffer, DWORD *size, DWORD *duration)
{
	if (voice_available())
		voice_xhv_record_voice_mail(port, &g_476fc8, maximum_time, buffer, buffer_size, size, duration);
}

/* ---- the players of the voice session ---- */

static inline long voice_get_unknown00(void)
{
	long result = 0;
	if (g_4c9878.initialized && g_476fc8.initialized)
		result = g_4c9878.unknown00;
	return result;
}

// @retail 0x54d20
dword voice_get_local_player_mask(void)
{
	dword mask = 0;
	if (voice_available())
	{
		byte *membership = (byte *)voice_get_membership();
		if (membership)
		{
			s_network_session_player *players = (s_network_session_player *)(membership + 0x10d4);
			if (players)
			{
				dword player_mask = voice_get_player_mask();
				long member = voice_get_current_member();
				for (long i = 0; i < 16; i++)
				{
					if ((player_mask & (1 << i)) && players[i].member_index == member)
						mask |= 1 << i;
				}
			}
		}
	}
	return mask;
}

// @retail 0x54b70
long voice_find_player(long unknown14)
{
	long result = NONE;
	if (voice_available())
	{
		s_network_session_player *players = NULL;
		byte *membership = (byte *)voice_get_membership();
		if (membership)
			players = (s_network_session_player *)(membership + 0x10d4);
		long member = voice_get_current_member();
		dword mask = 0;
		if (voice_available())
		{
			membership = (byte *)voice_get_membership();
			if (membership)
				mask = *(dword *)(membership + 0x10d0);
		}
		if (member != NONE && players && mask)
		{
			for (long i = 0; i < 16; i++)
			{
				if ((mask & (1 << i)) && players[i].member_index == member && players[i].unknown14 == unknown14)
				{
					result = i;
					break;
				}
			}
		}
	}
	return result;
}

// @retail 0x54c20
long voice_get_player_unknown14(long index)
{
	long result = NONE;
	if (voice_available() && (voice_get_local_player_mask() & (1 << index)))
	{
		s_network_session_player *players = (s_network_session_player *)voice_get_players();
		if (players)
			result = players[index].unknown14;
	}
	return result;
}

// @retail 0x54c70
bool voice_unknown00_valid(void)
{
	bool result = false;
	if (voice_available() && !voice_current_member_is_unknown00())
		result = voice_get_unknown00() != NONE;
	return result;
}

// @retail 0x54cc0
long voice_get_mode_value(void)
{
	long result = 0;
	if (*(volatile bool *)&g_4c9878.initialized)
	{
		if (!*(volatile bool *)&g_476fc8.initialized)
			return 0;
		switch (g_4c9878.mode)
		{
		case 1:
			result = g_4c9878.unknown08;
			break;
		case 2:
		case 3:
			result = g_4c9878.unknown0c;
			break;
		default:
			return 0;
		}
	}
	return result;
}

// @retail 0x54f40
dword voice_get_player_flags(long unknown14)
{
	dword result = 0;
	if (voice_available())
	{
		long index = voice_find_player(unknown14);
		word high = 0;
		word low = 0;
		if (index != NONE)
		{
			low = g_4c9878.unknownF0[index];
			high = g_4c9878.unknown110[index];
		}
		result = (high << 16) | low;
	}
	return result;
}

// @retail 0x55060
char *voice_sprintf(char *buffer, const char *format, ...)
{
	va_list arguments;
	va_start(arguments, format);
	_vsnprintf(buffer, 0x7f, format, arguments);
	buffer[0x7f] = 0;
	return buffer;
}

// @retail 0x55920
char *voice_append(char *buffer, const char *format, ...)
{
	va_list arguments;
	va_start(arguments, format);
	dword length;
	const char *s = buffer;
	for (length = 0; *s++ && ++length < 0x7f;)
		;
	long size = 0x80 - length;
	char *end = buffer + length;
	_vsnprintf(end, size - 1, format, arguments);
	end[size - 1] = 0;
	return buffer;
}

// @retail 0x553b0
void voice_xhv_submit_packet(c_voice_xhv *xhv, long id, void *data, long size)
{
	if (xhv->initialized)
	{
		XUID xuid = voice_xuid(id);
		if (FAILED(xhv->engine->SubmitIncomingVoicePacket(xuid, data, size)))
		{
			dword crc;
			char description[128];
			description[0] = 0;
			crc = 0xffffffff;
			function_163ba0(&crc, data, size);
			strncpy(description, "voice_modes:", sizeof(description));
			description[127] = 0;
			long port = 0;
			do
			{
				long mode;
				if (xhv->initialized)
					mode = xhv->port_modes[port];
				else
					mode = 0;
				voice_append(description, "(c#%d %d)", port, mode);
				port = voice_port_next(port);
			} while (port != NONE);
		}
	}
}

// @retail 0x53890
void voice_submit_incoming_packet(long id, void *data, long size)
{
	if (voice_is_enabled() && voice_has_remote_talker(id))
	{
		voice_fpu_enter();
		voice_xhv_submit_packet(&g_476fc8, id, data, size);
		voice_fpu_leave();
	}
}

/* the per-player values the voice settings keep (0x5259b8) */
struct s_voice_player_values
{
	bool enabled;
	byte unknown01[3];
	long values[16];
};

// @retail 0x55960
long voice_player_values_get(s_voice_player_values *values, long index)
{
	long result = 0;
	if (values->enabled && (voice_get_local_player_mask() & (1 << index)))
		result = values->values[index];
	return result;
}

/* ---- the voice channels (0x525a00) ---- */

struct s_voice_channel
{
	bool active;
	byte unknown01[3];
	long unknown04;
	word player_mask;
	byte unknown0a[2];
	long index;
	dword data[0x48];
	long values[16];
};

struct s_voice_channels
{
	bool initialized;
	byte unknown01[3];
	s_voice_channel channels[16];
};

static inline bool voice_channel_has_player(const s_voice_channel *channel, long player)
{
	bool result = false;
	if (channel->active)
		result = (channel->player_mask & (1 << player)) != 0;
	return result;
}

// @retail 0x56080
void voice_channels_initialize(s_voice_channels *channels)
{
	for (long i = 0; i < 16; i++)
	{
		s_voice_channel *channel = &channels->channels[i];
		channel->index = i;
		channel->active = true;
		channel->unknown04 = 0;
		channel->player_mask = 0;
		memset(channel->data, 0, sizeof(channel->data));
		memset(channel->values, 0, sizeof(channel->values));
	}
	channels->initialized = true;
}

// @retail 0x560e0
void voice_channels_reset(s_voice_channels *channels)
{
	if (channels->initialized)
	{
		for (long i = 0; i < 16; i++)
		{
			s_voice_channel *channel = &channels->channels[i];
			channel->unknown04 = 0;
			channel->player_mask = 0;
			memset(channel->data, 0, sizeof(channel->data));
			memset(channel->values, 0, sizeof(channel->values));
		}
	}
}

// @retail 0x56160
bool voice_channels_have_player(s_voice_channels *channels, long player)
{
	bool result = false;
	if (channels->initialized)
	{
		for (long i = 0; i < 16; i++)
		{
			result = voice_channel_has_player(&channels->channels[i], player);
			if (result)
				break;
		}
	}
	return result;
}

// MiniMax I (11-17): no lower score (94): a permutation run over this function's source (4 tries in 375 s) left the
// score at 94; the total, the cursor and the flag in the orders total-cursor-flag, total-flag-cursor and
// cursor-flag-total measure 94, 108 and 99, the channels walked by position with the flag read before the total
// measure 120, and the channel value read into a named local compiles to the same code.
// MiniMax R (11-16): no new form, no lower score (94): the identity flag still lands in BL where retail keeps it in AL, and the flag-first order moves the channels pointer into EBP and still leaves the flag in BL (111), so the kept mask, the value load and the accumulate follow that allocation; the flag, pointer and total order still reaches only 93.
// MiniMax C (11-15): no new form, no lower score (94): the identity flag still lands in bl where retail keeps it in al, this build still sets the total to two and the channel pointer before the compare that retail does after it, and the inner channel blocks are still not unrolled.
// MiniMax G (11-14): no new form, no lower score (94): the identity flag still lands in bl where retail keeps it in
// al, retail sets the total to two and the channel pointer after the flag's compare where this build sets them
// before it, and the inner channel blocks are still not unrolled.
// MiniMax G (11-13): no new form, no lower score (94): the identity flag stays in bl where retail
// keeps it in al, so the loop temporaries take eax/al where retail uses ebx/cl, and the inner
// channel blocks are not unrolled.
// MiniMax A (11-13): no new form, no lower score (94): the identity flag still takes BL here while retail keeps it in AL, and retail sets the size to two only after the flag's compare where this build sets it before; no untried form in the list moves the flag out of BL.
// MiniMax S (11-15): no new form, no lower score (94): the identity flag still lands in bl where retail keeps it in al, so the loop temporaries take eax/al where retail uses ebx/cl, and the inner channel blocks still step the cursor by 0x170 where retail uses fixed offsets and one 0x5c0 step.
// score 94 (was 94): the flag stays in bl (retail al), so the loop temporaries take eax/al where retail uses
// ebx/cl; retail loads the count into ebp before the multiplier and adds 2 before the total (ours folds the load
// into imul and uses one lea). Frame, prologue, unrolling and cursor agree (347 bytes).
// MiniMax S (11-11): no new form, no lower score (94): the identity flag still takes BL here while retail keeps
// it in AL, and the inner channel blocks are still not unrolled; moving the flag ahead of the total and the
// channel pointer gives 111, and a nested group loop that would give the flag a longer live range also gives
// 111, so the flag's register does not change from this body.
// MiniMax S (11-10): no lower score (94): retail keeps the base of the four channels of a group and reaches
// them with positive offsets, advancing it by 0x5c0 once per group, and takes the unknown00 flag in AL after
// the total is set, while ours steps the cursor by 0x170 per channel (so the later channels get negative
// offsets), takes the flag in BL and sets the total before the availability bytes are read; the grouped
// index form reaches retail's fixed offsets but biases the base and loses the matched caller 0x54990.
// MiniMax I (11-10): no new form, no lower score (94).
// MiniMax R (11-09): no lower score (94): the order of the three statements in the block - the identity flag,
// the total and the channel pointer - was measured again: the flag, the pointer and the total (93), the total,
// the pointer and the flag (94), the pointer, the flag and the total (99), the pointer, the total and the flag
// (99), the total, the flag and the pointer (108) and the flag, the total and the pointer (111, which also
// loses the matched caller 0x54990). What still differs is the register split - the identity flag sits in bl
// where retail keeps it in al, so retail's loop takes the channel mask in ebx and the membership result in cl
// while ours takes the mask in eax and the result in al. The one point better order is inside the margin
// this lane keeps, so the original order stays.
// MiniMax R (11-09): no lower score (94): the arithmetic shapes - the value, the contribution, the multiplier and
// the constant in a named local, the two separate adds and the constant first - all compile to the same bytes; the
// four groups of four channels with the channel reached by index (109) or by a per-group cursor (107) and the flat
// masked index (194) move the group base into the shape retail has, but they drop the matched caller 0x54990 to 1.
// MiniMax R (11-09): no lower score (94): the forms tried - the identity flag at function scope, first or last
// among the declarations, typed char or const bool, the comparison with the operands swapped, the membership test
// written out instead of through the shared helper, the channel reached by index and the value and the contribution
// named - all leave the flag in bl where retail keeps it in al, and the forms that do move it put the channels
// pointer on the stack and lose the matched caller 0x54990; retail's group base with fixed displacements and a
// 0x5c0 stride stays out of reach because the pointer walk is what the callers pin.
// MiniMax G (11-12): no new form tried this round, no lower score (94): the identity flag still takes bl where
// retail keeps it in al, and the inner channel blocks are still not unrolled.
// MiniMax H (11-12): no new form, no lower score (94): the first difference is still at +0xd, where the exit out of the initialization test lands elsewhere than retail's, and the identity flag still lands in BL here where retail keeps it in AL.
// score 94 (was 94): the flag stays in bl (retail al), so the loop temporaries take eax/al where retail uses
// ebx/cl; retail loads the count into ebp before the multiplier and adds 2 before the total (ours folds the load
// into imul and uses one lea). Frame, prologue, unrolling and cursor agree (347 bytes).
// MiniMax E (11-07): no new form, no lower score; the named per-channel contribution and the value and
// contribution locals of the earlier rounds leave the flag in bl, and the flag's own declaration order
// only moves the score to 93, under the review threshold.
// MiniMax R (11-04): forms tried - the flag declared before the cursor and the total 93 (the channels argument leaves eax, and the gain is under the review threshold, so the original body is restored), the shared helper rewritten with an early return 94 here but 0x56160 falls to 25, the contribution written constant-first 94 (the same bytes) and a pointer-indexed walk 128, a nested four-by-four group loop 94 (it reproduces retail's unrolled four-channel block, its memory counter and its prologue, but the identity flag stays in bl), the same nest with a separate group cursor 112 and 118 and with the flag first 111 (the channels argument leaves eax), the declaration orders on that nest 93, 99 and 108, and the fast permuter 47 tries with no lower score.
// MiniMax D (11-01): no new form, no lower score; the declaration order may not change, and the
// flag's register follows the shared helper that the matched 0x56160 and 0x56380 pin.
// Earlier (94): this round's new forms - the flag declared ahead of the total and the
// cursor (108), between the two (111), the channel reached by index (109) and the value read
// into a named local (94) - all leave the flag in bl where retail keeps it in al.
// Earlier (94): this round found no gain - the flag stays in bl where retail keeps it in al, and the
// two availability globals are tested in the opposite order from retail.
// identity BL versus retail AL and membership AL versus CL;
// frame, entry convention and 347-byte extent agree; payload/add and cursor schedules follow.
// Earlier: this round's new forms - the flag declared ahead of the total and the
// Earlier: this round found no gain - the flag stays in bl where retail keeps it in al, and the
// Earlier: the flag stays in bl (retail al), so the loop temporaries take eax/al where retail uses
// Earlier at 94 (was 94): this round's new forms - the flag declared ahead of the total and the
// Earlier at 94 (was 94): this round found no gain - the flag stays in bl where retail keeps it in al, and the
// MiniMax R (11-04): forms tried - the flag declared before the cursor and the total 93 (the channels argument leaves eax, and the gain is under the review threshold, so the original body is restored), the shared helper rewritten with an early return 94 here but 0x56160 falls to 25, the contribution written constant-first 94 (the same bytes) and a pointer-indexed walk 128, a nested four-by-four group loop 94 (it reproduces retail's unrolled four-channel block, its memory counter and its prologue, but the identity flag stays in bl), the same nest with a separate group cursor 112 and 118 and with the flag first 111 (the channels argument leaves eax), the declaration orders on that nest 93, 99 and 108, and the fast permuter 47 tries with no lower score.
// MiniMax D (11-01): no new form, no lower score; the declaration order may not change, and the
// flag's register follows the shared helper that the matched 0x56160 and 0x56380 pin.
// Earlier at 225: scratch 0x10 versus retail 4; absolute-span inner traversal remained rolled, 225/347 bytes.
// scratch 0x10 versus retail 4; absolute-span inner traversal
// remains rolled, 225/347 bytes.
// Earlier: sixteen-step rolled loop versus four-by-four unrolling;
// expanded loop changes the entry convention and loses the matched caller.
// Direct getter comparison 281; split header/payload adds 281; arithmetic width 329;
// counted direct comparison 112; counted split adds 112; counted arithmetic width 161;
// direct indexed counted accesses 120, with arithmetic width 133. Original body retained.
// Early channel-array base with counted traversal 111; base after validation 111;
// early base with pointer traversal 301; signed-word ordinal 296;
// early base with validation returns 113. All lose the matched caller; original retained.
// Direct validation exits rolled/counted 303/120; counted exits with runtime width,
// full identity, inline declaration or int ordinal 181/145/120/120; success total
// before query rolled/counted 303/119. Expanded forms still lose caller 0x54990.
// Out-of-line rolled 281; counted with out-of-line/read-only element/both 112/112/112, each loses the
// checked caller; fixed equality bound alone/read-only/out-of-line/both 281 each. The equality bound
// prevents four-way expansion; all original code restored.
// Unsigned total 281; unsigned total with counted walk 112; direct-receiver word ordinal 281;
// word ordinal with unsigned total 281; unsigned counted ordinal 112. Expanded forms lose 0x54990.
// Native identity with original validation: rolled 316, counted 144; byte identity counted 112;
// other-player identity 144; native/byte identity with channel rejection 144/112;
// native/other-player identity with entry-width value 165/165. All lose the matched caller; original retained.
// Counted traversal with immutable channels/player/both 112 each; pointer traversal
// with the same forms 283 each; counted immutable identity/source 112/112.
// Counted forms lose 0x54990; pointer forms preserve it. Original body restored.
// Every unrolled form keeps the identity flag in bl where retail keeps it in al, so the channels
// pointer moves to ebp and its argument to the stack. Counted loop 112; named channel size 112;
// pointer walk 111, with the identity compared in its declaration 111; per-channel size helper 184.
// Do/while forms (pre-increment too) and short counters stay rolled.
// Round 188 separate successful total: rolled/counted/unsigned result/unsigned total/
// retained entry width/admission/split adds/pre-tested walk 281/112/112/112/165/112/112/112;
// expanded caller conventions checked separately; original accumulation restored.
// Round 152: Signed/unsigned-word identity, original/counting traversal 315/140/315/140;
// bounded byte/signed-byte ordinals, original/counting traversal 281/281/281/281. Word forms lose 0x54990; original restored.
// Round 76 (counted loop, fast with 0x54990/0x56160): bool from the getter compare without local_0 112;
// operands swapped 112; 2 + product 112; multiplier first 112; channel indexed at each use 120;
// identity compare inside the loop 324 (rolled). Each counted form pushes the channels pointer in 0x54990.
// Round 82: four channels per pass on `&channels->channels[i]`, i += 4: 111 (caller 0x54990 1);
// the same indexed in full 120 (caller 6); a channel pointer from channel 0 walked by four with
// four tests per pass and `i < 4`: fully unrolled to 1021 bytes (987), but channels stays in ebx,
// the flag in al, the mask in ebx and the active byte in cl as retail, and 0x54990 matches;
// that pointer walk with `i += 4` 113 (caller 13), as a do/while 113 (caller 13); pointer
// compare loop stays rolled 283. Original body restored.
// Round 196: shared nested groups for/for and for/while 111 each; for/do 255 (kept);
// while/for, do/for and descending-for/for 111 each; actual outer array ends/for 122 each.
// Post-tested inner with outer array ends 258 each, descending or post-tested outer 255 each.
// All pre-tested-inner forms lose exact caller 0x54990; post-tested-inner forms preserve it.
// Round 381: Grouped outer/inner short-long, byte-long, long-short, long-byte, short-short and byte-
// byte induction 255 each; actual group/step indexed elements with long/short domains 255 each.
// Original grouped traversal restored; scratch and expansion remain.
// Round 244: Shared membership guarded/success/failure/conditional results with post/pre-tested inner
// traversal 255/111, 265/111, 265/111, 292/94; positive return arms/two-arm result 111/111; direct/named
// conjunction, nested success and two failure exits 111 each; forced conditional 94, ordinary conditional
// 285. Every form loses 0x56160; 94 preserves 0x54990 but loses 0x56380. Original helper and grouped body
// restored.
// Round 396: named complete contribution/payload and payload-header/header-payload
// accumulation on the grouped graph: post-tested inner 255,255,255,255;
// pre-tested inner 111,111,111,111 at 347 bytes loses 0x54990. Original restored.
// Round 188: real group subtotal, direct/named contribution/group/function entry width,
// post/pre-tested inner: 233/233/241/267/128/128/104/130; original-site width bool/byte
// and direct subtotal byte/long identity: 233/233/233/257. All legal gains lose 0x54990;
// group-width 241 preserves it but hoists one-use computation, so restored. Original grouped total retained.
// Round 181: absolute spans pre/post ordered/equal 248/248/245/245;
// pointer spans 249 each (four forms); outer/inner counter domains 254/225/244/244;
// identity width/opposite/direct and unsigned span domains 225/245/244/225/225/225/225/225.
// Short group start with long absolute channel ordinal retained; every 0x54990 loss rejected.
// Round 86: the plain counted loop over &channels->channels[i] 112 (unrolled, but 0x54990 off by one: channels in
// ebp, passed on the stack); taking the cursor from channels->channels before the flag frees ebx for channels:
// total, cursor, flag 94 (kept); flag, cursor, total 93 (350 bytes); cursor, total, flag 99; flag, total, cursor
// 111 (0x54990 lost); nested entry tests 94; a short counter 287 (rolled); flag types, entry-size locals,
// operand orders and a by-value size helper give the same code; a 256-form search of the family 93+.
// Round 217: forward group-end ownership, outer/inner short-long, short-short, long-long,
// long-short with post/pre-tested inner spans 224/222, 258/237, 236/240, 260/260. Tiny gains
// rejected; other forms lose 0x54990 or regress. Original grouped traversal restored.
// Round 30 (lane 10, one-file harness exact): retail's unrolled copies address channel 0 at +4 with
// fixed offsets and one 0x5c0 step; indexed forms strength-reduce to a biased base (+0x17c or
// +0x2ec, as matched 0x56160 does) and pass channels on the stack (0x54990 1). Indexed with an
// element pointer 109, the same through a cursor `&channel[i]` 108, `channel + i` / `channel[i]`
// 128 (368 bytes), indexed with a separate result 112; cursor, flag, total order 99 (350 bytes);
// the cursor step in the for statement 94 (same code).
// Round 95: single cause, the identity flag's register (bl, retail al); every other difference follows from it.
// Flag before cursor and total 93 (350 bytes); flag, total, cursor 111 (0x54990 1); flag set by an if
// from false 93 (still bl); flag at function entry 108 (0x54990 1); getter value kept as a long and compared
// in the ternary 141 (0x54990 13); screened: const flag, `? true : false`, a per-channel stride local in
// either operand order, `2 + product`, two-statement and parenthesised adds 94 (same code); flag per channel
// 165 (390 bytes); the channels pointer kept live past the loop (diagnostic) leaves the flag in bl.
// Round 112: the materialised has_player bool takes EAX; written `active && (mask & bit) != 0` (no bool) the flag
// gets AL, the mask EBX and the active byte CL as retail (harness aligned 105 -> 49) but 0x56160/0x56380 need
// the result form: 118 (0x56160 69, 0x56380 130, 0x54990 5 lost). Count read before the has_player test 141
// (383 bytes); shared inline flag helper for 0x56220/0x56380 94 (same code); per-channel `result = 0; if (...)`
// helper 193 (411 bytes); values as dword 94 (same code). Screened, same code or worse: 27 order/traversal/helper
// forms, 30 product spellings (count/size locals, two adds), mixed direct/pointer reads 368-372 bytes.
// Round 8 (lane 17): BLOCKED-BY 0x56160 and 0x56380, whose matches pin the shared bool helper (retail materialises
// the per-channel test in CL after the shift, ours in AL). The helper returning byte/char/a bool through a byte
// return 94 each with 0x56160 68; returning long 123 (0x56160 103, 0x56380 123, 0x54990 5); `(!is_unknown00 ? 11 :
// 13)` 94 (same code); a per-channel multiplier set by an if 110 (0x54990 13); `player == voice_get_unknown00()` 94.
// Round 27 (lane 14, fast; harness positional/aligned 90/105 on the control): has-player as an early
// continue with the cursor step in the for header 94; identity compare written player == getter 94;
// multiplier before the count in the product 94; `total += 2` 94 (all four the same code); cursor, flag,
// total order 99 (350 bytes; channels moves to EDI, flag still BL); cursor declared at entry (harness) 113
// with 0x54990 off by 11. Retail's colours all follow from the flag taking EAX before the loop temps
// (mask then lands in EBX, has-player in CL); every form here lets the mask take EAX first.
// Round 20: the walk also differs in its steps: ours adds 0x170 after every unrolled channel, retail
// addresses the four channels at fixed offsets from one cursor and steps 0x5c0 once. `&channel[i]`
// through the local pointer 128 (368 bytes: a second IV for the values, biased +0x2ec base, 0x54990
// 6); flag first with the step in the for statement 111 (channels in EBP, 0x54990 1; flag still BL);
// a named per-channel size in the arm 94; nested initialized/have-player tests 94; `2 + (flag ? 13 :
// 11) * value` 94; a countdown counter `remaining = 16 .. > 0` 94 (all four the same code).
// Round 10 (lane 17): flag, total, cursor 111 (0x54990 1; channels moves to EBP but the flag still takes BL, so
// channels being live at the flag's definition is not what keeps retail's flag in AL); the flag declared at
// function entry and assigned in place 94; the getter result named before the compare 94; total, flag, cursor 108
// (0x54990 1); cursor, flag, total 99 (350 bytes); the cursor `s_voice_channel const *` 94 (same code).
// Round 436: complete conditional contribution 148 (loses 0x54990); selected total 94;
// whole-scan header bytes/channel count/payload bytes 143/143/143; header and payload totals 132;
// channel count and payload total 122; unsigned header/payload totals 132. Separate accumulators
// grow scratch to 8 versus 4 and spill the identity flag. Original restored.
// Round 61 (lane 1, fast): has-player helper as early return/if-first return 94/94 (0x56160 40/25), ternary 103 (356 bytes,
// aligned 66 -> 43; 0x56160 25, 0x56380 74, 0x55c40 140), if/else result 94 (0x56160 71), bit shifted down 133, named mask
// 94 (same code); the test written at the call site, `active && mask test` or nested, 118 (324 bytes; flag AL and mask EBX as
// retail, but player EDI and total ESI, 0x54990 5), as a ternary 95 (354 bytes, 0x54990 5). All restored.
// Round 198 (lane 3, fast with 0x54990): by-value size helper `count * (mask ? 13 : 11) + 2`, named entry size, multiplier
// first, staged add, named count, named count and size 94 each (same code; `imul ecx, [mem]` and `lea` remain).
// Round 204 (fast with 0x54990): a hoisted `long size = getter == player ? 13 : 11` 165 (294 bytes, 0x54990 13);
// early `return 0` for the failed entry test 109; a named per-channel contribution 94 (same code); the size as
// `11 + 2 * flag` 150 (309 bytes, 0x54990 13); the counter declared at the block top 94 (same code). Restored.
// Round 59 (m3 lane 11, fast with 0x54990/0x56160/0x56380): the 2 added after the loop (`total += 2`) 99; `total += 2` in
// place of `total = 2` 94 (same code); `11 + (is_unknown00 ? sizeof(word) : 0)` 130 (329 bytes, 0x54990 13); a named
// entry size from the getter compare in place of the flag 165 (294 bytes, 0x54990 13); the product written in each ternary
// arm 133 (325 bytes, 0x54990 13).
// Round 609: actual channel array/current-channel/values array/value lvalue bindings, mutable/const,
// 94/94/118/118/94/94/94/94. NOT kept: no exact result; current-channel references grow frame and length.
// Round 90 (m3 lane 9, fast with 0x54990/0x56160/0x56380): `__forceinline` on the shared has-player helper 94 (same
// code); a single-exit total set to 0 in the else of the inverted entry test 109; the per-channel product as an inline
// helper taking the include flag 94 (same code); the inverted flag `!= player` with `? 11 : 13` 94 (same code); the flag
// first, then the total and an indexed element pointer per iteration 112 (0x54990 1). Restored.
// M400: Shared membership seeded from its active byte, bool/byte/signed-char/short/word/
// long/dword/int storage: 94/94/94/146/146/133/133/133. NOT kept: every form
// raises shared-user scores; original false-seeded helper and target body restored.
// MiniMax D (11-10): no lower score (94): the group-of-four forms tried this round - a nested group loop over
// &channels->channels[group * 4] 111, a nested group loop walking a channel pointer 111, four channel tests
// written out per group 987 (1021 bytes, the outer loop unrolls), and the raw getter for both inner terms 122
// with the extracted bit in a byte temp read in the condition 117 - are all worse; retail's unrolled inner blocks
// with the identity flag in al still do not come out of a nested loop.
// MiniMax D (11-06): no new form, no lower score.
// Round 63 (m3 lane 15; one-file LTCG harness, code identical to the build; real = harness - 3 here): new: a
// pointer-bounded loop 286 (not unrolled); the flag typed long or dword 131 (326 bytes, 0x54990 15); the flag typed
// char 94 (same code). Re-measured: the element pointer taken per iteration from the index 106 (one IV with fixed
// offsets as retail, but biased +0x17c; 0x54990 1); the channel reached by index for both reads 120 (a second IV for
// the values, 0x54990 6); the pointer stepped in the for statement 94; flag first with the per-iteration element
// pointer 109 (0x54990 1); the element pointer with a named value 106 (0x54990 1).
// Round 71 (m3 lane 13, fast with 0x54990): the has-player result named before its test 94, total typed dword 94, the
// contribution parenthesised `total += (value * size + 2)` 94 (still one lea), unsigned 13u/11u 94 (all four the same
// code); the loop test `i != 16` 287 (154 bytes, not unrolled). Re-measured: the flag declared first 111 (channels moves
// to EBP and the stack, 0x54990 1; the flag still takes BL).
// MiniMax G (11-11): no new form, no lower score: the identity flag read first (before the total and the channel
// pointer) 111 (0x54990 1); the flag computed inside the loop 165 (390 bytes); the getter result in a named local
// 94 (the same code); the flag computed before the entry test 108 (0x54990 1). Retail keeps the channel pointer live
// past the flag, so the flag lands in al; our hoisted pointer frees bl and the flag lands in bl, which shifts the
// mask and the product registers with it.
// MiniMax R (11-11): no new form kept this round, no lower score (94): the first difference is at +0xd, where the exit out of the initialization test lands at a different place than retail's, and moving the identity flag ahead of the total gives 111, so the flag keeps BL here and AL in retail.
// Round 154 (m3 lane 7, one-file harness, exact; 0x54990 and 0x56160 matched unless noted). New: the shared
// membership helper with a named mask word 94 (same code; 0x55c40 and 0x56380 unchanged), as a ternary 103 (0x56160
// 25, 0x55c40 154, 0x56380 74), as one `&&` test setting the result 118 (0x54990 5, 0x56160 55); the identity test as
// an inline shared with 0x56380 (`return getter == player`, or through a named result) 94 (same code); the channels
// parameter and the cursor `const` 94 (same code). Re-measured: a named bit inside the membership helper 94 (MiniMax
// S in 0x55c40 note); the identity helper called inside the product 165 (the in-loop flag). Listing of the listed
// flag-first form (93): the channels argument becomes a stack argument loaded into EBP and the flag still takes BL.
// MiniMax S (11-14): no new form, no lower score (94): the identity flag still takes bl where retail takes al, and
// that moves the kept mask, the value load and the accumulate with it; every spelling of the flag (function scope,
// declared first, wide, word, char) compiles to the same code.
// MiniMax S (11-16): no new form, no lower score (94): the identity flag still lands in bl where retail keeps it in al, so the loop temporaries take eax/al where retail uses ebx/cl, and the inner channel blocks still step the cursor by 0x170 where retail uses fixed offsets; reading the identity flag before the total is assigned raises the score to 111 and indexing the channels instead of walking them with a cursor raises it to 123.
// MiniMax C (11-16): no new form, no lower score (94): the identity flag still lands in bl where retail keeps it in al, so the loop temporaries take eax and al where retail uses ebx and cl, and the inner channel blocks still step the cursor by 0x170 where retail uses fixed offsets; reading the identity flag before the total is assigned measures 111 and moves the channels pointer out of eax.
// MiniMax I (11-17): no lower score: the four groups of retail's loop were tried as an inner loop over four channels by index (110) and by cursor (94) and as four written-out copies (1020); the index form reaches retail's length but takes the channels on the stack and spills the identity flag, and the cursor form folds back into the single-channel loop.
// MiniMax D (11-17): no new form, no lower score (94): retail keeps the identity flag in al and its loop temporaries in ebx and
// cl where this build keeps the flag in bl and takes eax and al, and retail addresses four channels at fixed offsets from one
// cursor where this build steps the cursor by 0x170 per channel; the source order of the flag, the total and the cursor does not
// decide the registers (every order gives 94 or worse). This round: walking the channels by index (`&channels->channels[i]`) makes
// the compiler unroll the loop four times, which reaches retail's length (347) and its four blocks at 0, 0x170, 0x2e0 and 0x450
// with the 0x5c0 step, but the identity flag still takes bl and the channels pointer still takes ebp, so the score is 109; the
// flag read before the total measures 112, and four written-out blocks in a four-group loop are unrolled away into 1020 bytes.
// b16H (11-17) walk against retail, fast build on r852: every statement matches retail (initialized byte [ebx], call
// 0x56160 with ecx=player edx=channels, getter 0x4c99b8/0x476fcc/0x4c9878 then cmp eax,esi; sete, 2, 16 channels at +4
// step 0x170, mask word +8, values +0x130, 11 + 2 * flag, + 2): no behaviour difference; the 94 is all allocation.
// Retail's flag takes al (channels still in ebx when the flag is made, so A/B land in cl and the mask in ebx); ours
// puts the flag in bl because the lea of the cursor is scheduled before the compare. Retail also loads the value into
// ebp, then imul ecx,ebp; add ecx,2; add edi,ecx (ours: imul ecx,[mem]; lea edi,[edi+ecx+2]); three more spellings of
// the add (bandwidth local, total += 2 then product, value/size locals) 94 each. New, all worse: flag, cursor, total 93
// (350 bytes: channels in edi, the early-exit zero in ebx); total, flag, cursor 108 (channels in ebp from the stack,
// `ret 4`); indexed `&channels->channels[i]` 109 (IV biased to channel 1's mask at +0x17c, bl flag, stack argument);
// four `channel[k]` blocks per pass, i += 4 110 (second IV, player lands in ecx); a loop-level inline helper taking the
// flag is not inlined (182/287); do/while `++i < 16` 287. Outside piece voice_get_unknown00 (ternary / early-return
// body) 96 each, not kept. Lead: the unbiased IV and one 0x5c0 step in retail need the flag made while channels is
// still in ebx; every order that does that makes the allocator move channels to ebp or edi instead of the flag.
// @retail 0x56220
long voice_channels_get_bandwidth(s_voice_channels *channels, long player)
{
	long total = 0;
	if (channels->initialized && voice_channels_have_player(channels, player))
	{
		total = 2;
		s_voice_channel *channel = channels->channels;
		bool is_unknown00 = voice_get_unknown00() == player;
		for (long i = 0; i < 16; i++)
		{
			if (voice_channel_has_player(channel, player))
				total += channel->values[player] * (is_unknown00 ? 13 : 11) + 2;
			channel++;
		}
	}
	return total;
}

/* a queue of 0x12-byte voice entries */
#pragma pack(push, 2)
struct s_voice_entry
{
	dword data[4];
	word unknown10;
};
#pragma pack(pop)

struct s_voice_queue
{
	long unknown00;
	long count;
	byte unknown08[8];
	s_voice_entry entries[16];
	long player_counts[16];
};

// @retail 0x56000
void voice_queue_compact(s_voice_queue *queue, word mask)
{
	long kept = 0;
	for (long i = 0; i < queue->count; i++)
	{
		if (mask & (1 << i))
		{
			if (i != kept)
				queue->entries[kept] = queue->entries[i];
			kept++;
		}
	}
	queue->count = kept;
}

// Matches retail byte for byte. What its bytes need: the head statements in retail's order (kept mask, written count,
// capacity debit, entry size); the entry size started at its base 11 ahead of the written count, with the mask's two
// bytes added after the debit (the compiler sorts a product's operands by the order their variables are first defined,
// the later one first, so the length's `written * entry_size` copies the count and multiplies the copy, retail's
// `mov edi, ecx; imul edi, ebx`, only when the size was defined before the count; defined after it, the product is
// formed in the size's register and the player loses its register); and the first cursor byte stored in each
// include_mask arm (retail loads and tests the flag, stores the byte, then branches).
// @retail 0x55c40
long function_55c40(s_voice_queue *queue, byte *cursor, long player, byte *output, long capacity, bool include_mask)
{
	long result = 0;
	if (*(bool *)&queue->unknown00)
	{
		dword player_bit = 1 << player;
		if (!voice_channel_has_player((s_voice_channel const *)queue, player))
			result = NONE;
		else
		{
			long clamped = player;
			if (player < 0)
				clamped = 0;
			else if (player > 15)
				clamped = 15;
			if (clamped != player)
				result = NONE;
			else
			{
				long entry_size = 11;
				dword kept = (1 << queue->count) - 1;
				long written = 0;
				capacity -= 2;
				entry_size += 2 * (include_mask != 0);
				if (capacity >= entry_size)
				{
					for (long i = 0; i < queue->count; i++)
					{
						byte *entry = (byte *)&queue->entries[i];
						if (*(word *)(entry + 6) & player_bit)
						{
							if (capacity < entry_size)
								break;
							if (include_mask)
							{
								*cursor = entry[4];
								*(word *)(cursor + 1) = *(word *)(entry + 2);
								if (!*(word *)(cursor + 1))
									*(word *)(cursor + 1) = (word)(1 << player);
								cursor += 3;
							}
							else
							{
								*cursor = entry[4];
								cursor++;
							}
							memcpy(cursor, entry + 8, 10);
							cursor += 10;
							*(word *)(entry + 6) &= ~(1 << player);
							queue->player_counts[player]--;
							if (!*(word *)(entry + 6))
								kept &= ~(1 << i);
							written++;
							capacity -= entry_size;
						}
					}
					result = written * entry_size + 2;
					output[1] = (byte)written;
					output[0] = queue->unknown08[4];
					if ((word)kept != (1 << queue->count) - 1)
						voice_queue_compact(queue, (word)kept);
					*(word *)queue->unknown08 = 0;
					for (long i = 0; i < queue->count; i++)
						*(word *)queue->unknown08 |= *(word *)((byte *)&queue->entries[i] + 6);
				}
			}
		}
	}
	return result;
}

// @retail 0x56380
long function_56380(byte *output, s_voice_channels *channels, long player, long capacity)
{
	long result = 0;
	long original_capacity = capacity;
	if (channels->initialized && voice_channels_have_player(channels, player) && capacity >= 15)
	{
		long count = 0;
		bool include_mask = voice_get_unknown00() == player;
		capacity -= 2;
		byte *packet = output;
		if (include_mask)
			packet[0] |= 1;
		else
			packet[0] &= ~1;
		output += 2;
		byte *header = output;
		for (long i = 0; i < 16; i++)
		{
			s_voice_channel *channel = &channels->channels[i];
			if (voice_channel_has_player(channel, player) && capacity > 0)
			{
				output += 2;
				long written = function_55c40((s_voice_queue *)channel, output, player, header, capacity, include_mask);
				if (written <= 0)
					break;
				count++;
				output += written - 2;
				capacity -= written;
				header = output;
			}
		}
		if (count > 0)
		{
			packet[1] = (byte)count;
			result = original_capacity - capacity;
		}
		else
			packet[1] = 0;
	}
	return result;
}

/* ---- the voice settings of the players (0x527108) ---- */

s_voice_player_values g_5259b8;
s_voice_channels g_525a00;
/* the routes of the voice packets (0x527104; s_voice_routing is in
   globals.h) */
s_voice_routing g_527104;

long voice_player_settings_get_active_value(s_voice_player_settings *settings, long player);
dword function_56c60(s_voice_player_settings *settings, long player);

static __forceinline bool function_57d63(long arg_0, long arg_1);

/* whether the voice mode is 1 (active) or 2 or 3 (passive) */
static inline bool voice_mode_is_active(void)
{
	bool result = false;
	if (g_4c9878.initialized && g_476fc8.initialized)
		result = voice_get_mode() == 1;
	return result;
}

static inline bool voice_mode_is_passive(void)
{
	bool result = false;
	if (g_4c9878.initialized && g_476fc8.initialized)
		result = g_4c9878.mode == 2 || g_4c9878.mode == 3;
	return result;
}

static inline dword voice_get_player_mask_inlined(void);

// MiniMax I (11-20): no new form kept, no lower score (64): with the muted word's test first in the inner loop the body is
// retail's 427 bytes but scores 85, because the outer body then loads the second ready byte after the first test and holds
// the remote mask in edx, and the second port-flag query is still called out of line where retail expands it; the
// flags-first order stands.
// MiniMax G (11-20): no new form, no lower score (64): the outer body matches retail instruction for instruction with the
// flags-first order (both ready bytes read up front, the remote mask in esi, the mode in ecx), and the muted-word-first order
// with plain ready-byte reads in the two port-flag queries reproduces retail's inner loop exactly up to the last query, which
// is still called out of line; that order moves the outer body to a reloaded second ready byte and the remote mask in edx,
// and the plain ready-byte reads alone (85 to 94) do not lower the score, so the flags-first body stands.
// score 64 (was 85): r209 unchanged. left: retail tests the muted-word bit first, then the first port flag reusing the second
// MiniMax B (11-19): no new form, no lower score (64): retail's inner loop is muted-word bit, first port flag, second
// port flag, with the second query expanded in place (two calls of 0x53be0); re-measuring retail's test order here gives
// 85, and `inline` on 0x53bb0 with either order gives 105 or 102 and takes the matched callers 0x54890 and 0x53a20 with
// it, so the flags-first body stands. The body is 415 bytes against retail's 427: the twelve missing bytes are the
// second port flag's expansion, and every form that expands it expands it in 0x54890 and 0x53a20 too, which the earlier
// per-site threshold screen showed no single threshold satisfies. BLOCKED-BY 0x53bb0 (its per-site inlining).
// MiniMax B (11-20): no new form, no lower score (64): the second port-flag query is still called out of line
// where retail expands it in place (two calls of 0x53be0); every expansion form measured before expands it in
// the matched callers 0x54890 and 0x53a20 as well, and `inline` on 0x53bb0 with either test order gives 105 or
// 102 and takes those two callers with it. The body stays 415 bytes against retail's 427, the missing twelve
// being that expansion. BLOCKED-BY 0x53bb0 (its per-site inlining) and 0x59670/0x596a0 (the session getters).
// Merged from a second score line (64, was 85): left: retail tests the muted-word bit first, then the first port flag reusing the second
// availability byte in CL, then the second port flag expanded in place (two calls of 0x53be0); this body tests the two
// port flags first and calls 0x53bb0 out of line, 12 bytes short. BLOCKED-BY 0x53bb0 (its per-site inlining).
// MiniMax I (11-17): the inner loop's condition now tests the two port flags before the muted lookup, which takes
// the function from 85 to 64; the three tests read only globals and have no side effects, so their order does not
// change the result, and 0x54890, 0x53a20 and 0x53bb0 stay matched. Still differs: retail keeps the second port
// flag query inline (the getter, the shift and the availability tests at the end of the loop body) where this build
// calls the out-of-line copy, so the body is 12 bytes shorter than retail's. Re-measured this round at 64 with the
// same three differences; the session getters 0x59670/0x596a0 live in another file and `inline` on 0x53bb0 costs
// the matched callers 0x54890 and 0x53a20, so the body stands: BLOCKED-BY 0x59670/0x596a0 and 0x53bb0.
// calls the out-of-line copy, so the body is 12 bytes shorter than retail's.
// MiniMax R (11-16): no new form, no lower score (85): retail loads both ready bytes before the first test and keeps the second in DL, while this build reloads it for the mode test and takes the mode in EAX; the remote mask rides in EDX here and in ESI there, and the flags held in locals are re-read from the globals at every use (96 to 104).
// MiniMax C (11-15): no new form, no lower score (85): the availability bytes are still read into locals for the muted test and the mode helpers are still parameterised on them, so the second byte still loads lazily inside the muted test where retail loads both before it and keeps the remote mask in esi.
// MiniMax G (11-14): no new form, no lower score (85): the availability bytes read into locals for the muted test
// and the mode helpers parameterised on them leave the second byte loading lazily inside the muted test, where
// retail loads both before it and keeps the remote mask in esi.
// MiniMax G (11-13): no new form, no lower score (85): the second availability byte loads lazily
// where retail loads both before the muted test, and the second port-flag query is called out of
// line: BLOCKED-BY 0x59670/0x596a0.
// MiniMax B (11-13): no new form, no lower score (85): the session getters 0x59670/0x596a0 (src/unknown_058ee0.cpp) are outside this lane's files, and the second port-flag query is still called out of line where retail inlines it here: BLOCKED-BY 0x59670/0x596a0.
// MiniMax S (11-15): no new form, no lower score (85): the session getters 0x59670/0x596a0 live in another file, so the second availability byte still loads lazily where retail loads both before the muted test and keeps the remote mask in esi.
// Earlier at 85 (was 85): the second availability byte loads lazily (retail loads both before the muted test and keeps
// remote in esi) and the second port-flag query is called out of line (retail inlines it here only); frame and bit
// spill agree. Earlier at 94: frame 0x14 vs 0x18, bit spill and flag-query boundary differed;
// word outer/target induction 118/106; word active 97; mask names 94; skip cleared inactive entries 110.
// caller depth 0/1/2 241/94/94; wide bit with depth 1/2 118/118;
// complete mask/policy/both assignments 108/104/110.
// Rejected set 100; rejected set with wide bit 168; indexed rejection bit 99;
// allowed result 107; signed allowed result 107; allowed result with wide bit 130;
// candidate snapshot 97; candidate snapshot with wide bit 119. Frame/spill and flag-query expansion remain.
// MiniMax S (11-11): no new form, no lower score (85): the second availability byte still loads lazily where retail
// loads both before the muted test, and the second port-flag query is still called out of line; expanding that query
// at this site costs the matched callers 0x54890 and 0x53a20, and the session getters 0x59670/0x596a0
// (src/unknown_059670.cpp) are outside this file: BLOCKED-BY 0x59670/0x596a0.
// MiniMax G (11-11): no new form, no lower score: the second availability byte still loads lazily where retail
// loads it beside the first and keeps it in dl for the mode test, and the remote mask still loads into edx
// where retail uses esi; that pair appears only when the session getters 0x59670/0x596a0 are opaque to LTCG,
// which is outside this file. `inline` on the second port-flag query scores 102 here and loses the matched
// callers 0x54890 (83) and 0x53a20 (84); an eager muted gate (the two bytes anded) scores 105. Both restored.
// MiniMax D (11-10): no lower score (85): the inner query forms tried this round - the raw getter for both
// port-flag terms 122, the raw getter's bit in a byte temp read in the condition 117, and the third term behind
// voice_available() with the raw getter 89 - are all worse; the per-site inlining of the second query still
// breaks the matched callers 0x54890 and 0x53a20.
// MiniMax S (11-10): no lower score (85): retail reads the two availability bytes up front and keeps them in
// CL and DL for the later tests, and expands both port-flag queries here (the first one reuses the byte
// read earlier, the second one reads both bytes again after the call), while ours reloads them at every use
// and calls 0x53bb0 out of line; the shared availability helper reads them through volatile casts that the
// matched functions in this file need, and reading them plainly loses 0x53b40, 0x54890 and 0x53a20.
// MiniMax I (11-10): no new form, no lower score (85).
// MiniMax R (11-09): no lower score (85): the second port-flag query marked `inline` was measured again and
// confirms the recorded result - 0x55990 goes to 102 while the matched callers 0x54890 and 0x53a20 are lost
// (83 and 84), and 0x53b90 and 0x53bb0 keep matching either way.
// MiniMax R (11-09): no new form measured this round (85): the second availability byte still loads lazily where
// retail loads both before the muted test, and the remote mask still reloads against retail's esi.
// MiniMax R (11-09): no lower score (85): the second availability byte still loads lazily where retail loads both
// before the muted test, and the second port-flag query is still called out of line; the form that inlines the
// first query and reaches retail's eager byte pair loses the matched callers 0x54890 and 0x53a20, and the session
// getters this needs live outside this file.
// MiniMax G (11-12): no new form tried this round, no lower score (85): what is left is the order of the two
// availability-byte loads and the register of the stack word, both decided by the schedule of the block.
// MiniMax H (11-12): no new form, no lower score (85): the first branch is still at +0xb and still leaves for a different place than retail's shared exit at +0x1a6, so only the uninitialized path's exit sits at the wrong address.
// Earlier at 85 (was 85): the second availability byte loads lazily (retail loads both before the muted test and keeps
// MiniMax G (11-18): no new form, no lower score (64): the loop body still runs the port-flag queries before the word mask
// test where retail does the word test first, and the second port-flag query is still called out of line where retail
// expands it at this site, so the body and its jump targets differ.
// MiniMax A (11-18): no new form tried this round, no lower score: retail still inlines both port-flag queries at this
// site where this build inlines one and calls the other out of line, and the second availability byte still loads
// lazily where retail loads both before the muted test and keeps the remote mask in esi. The second query's helper is
// in this file, but marking it inline takes the matches of 0x54890 and 0x53a20 with it, and the session getters
// 0x59670/0x596a0 live in another file: BLOCKED-BY 0x53bb0 and 0x59670/0x596a0.
// Earlier 85 (was 85): the second availability byte loads lazily (retail loads both before the muted test and keeps
// remote in esi) and the second port-flag query is called out of line (retail inlines it here only); frame and bit
// spill agree. Earlier at 94: frame 0x14 vs 0x18, bit spill and flag-query boundary differed;
// word outer/target induction 118/106; word active 97; mask names 94; skip cleared inactive entries 110.
// caller depth 0/1/2 241/94/94; wide bit with depth 1/2 118/118;
// complete mask/policy/both assignments 108/104/110.
// Rejected set 100; rejected set with wide bit 168; indexed rejection bit 99;
// allowed result 107; signed allowed result 107; allowed result with wide bit 130;
// candidate snapshot 97; candidate snapshot with wide bit 119. Frame/spill and flag-query expansion remain.
// MiniMax E (11-07): no new form, no lower score; the gap needs the two session getters and 0x53bb0,
// which live outside this file.
// MiniMax h (11-06): no new form, no lower score: the remaining gap needs the two session getters and 0x53bb0, which live outside this file.
// Retail shape (for-statement counter, dword bit and muted, store only for active players, plain
// availability tests with 0x56b70's active/passive mode flags, `1 << j` in the inner loop) 85, but
// the mode flags are set inside a test (kept only on a match); without them 104; with the counter
// declared first 117; volatile muted/mode/both tests 103/99/142; counter in the for statement alone
// 125. With the first port-flag query on the plain
// availability test and the second inlined, a one-file LTCG build is byte-identical to retail
// only when voice_get_membership's call chain may write arbitrary memory; ours is known to write
// only its own local, which leaves the second availability byte loaded lazily.
// Coordinated membership inline/local-mask metadata, native/wide bit 94/118/94/118; forced membership
// combinations 211/206/211/206 (21 checked matches lost). Restored.
// Round 82: retail inlines the second port-flag query only here (BLOCKED-BY 0x53bb0: `inline` on it also inlines it
// in 0x54890 and 0x53a20, which call it out of line) and loads both availability bytes before the muted test.
// Dword bit/muted, active-only store, `1 << j`, plain tests and the counter in the for statement 99; with a word
// bit/muted 96; with the shifting target 110; with the helper policy test 127; a named availability flag for both
// tests with the mode through voice_get_mode() 89; a once-used availability flag with voice_get_mode() 82 (the flag
// is materialised, which retail does not do; the tail only realigns; not kept); voice_get_mode() with the plain
// muted test 96.
// Round 86: retail's shape with the mode tests through voice_mode_is_active()/voice_mode_is_passive() (shared
// with 0x56b70) 85 (kept); with plain availability in the first port-flag query 94, an if/else muted word 98
// (remote in esi, the byte loaded in both arms), a word muted 88, a word bit 92, the muted test through
// voice_available() 103. One-file LTCG builds: 0x55990 in its own /Ob1 or /O2 file 133; the second query
// compiled /O2 or redeclared inline/__forceinline is inlined in all three callers; with the session getters
// 0x59670/0x596a0 opaque the eager pair and esi remote appear (60, 415 bytes), so the lazy byte is
// BLOCKED-BY 0x59670/0x596a0 being visible to LTCG, the call by 0x53bb0's per-site inlining.
// Round 95: the active mask through the existing getter 0x54d00 171 (called, 390 bytes); the inner removal
// test nested under the mask test, a named target bit, or a named removal flag 85 each (same code); the remote
// mask computed after the active mask 113. 0x53bb0 declared inline (diagnostic) inlines it in all three callers
// and its copy here runs 19 bytes long (102), so the per-site inlining is not the only gap.
// Round 112: harness with 0x59670/0x596a0 left out (opaque to LTCG) gives retail's eager byte pair and ESI
// remote from this source (aligned 19 -> 13); only the inlined second port-flag query is then left. Muted mask
// as one conditional expression 98; memset before the player-mask read 141; `~muted & remote` 85 (same code);
// the player mask through 0x54b10 182 (386 bytes); through the existing inline twin 85 (same code, kept).
// Round 436: eager muted gate 105; separate readiness snapshots with short-circuit/eager
// policy gates 104/132; common eager availability 123, with one mode observation 123;
// eager native-word muted 106; common eager/native-word 122; common eager/conditional muted 122.
// Eager gates combine the two tests; snapshots still defer or combine loads. Original restored.
// DH round 265: complementary eager unavailable bool/byte/short/long, separate/shared policy gates:
// 101/101/97/99/111/111/94/116. Original complete body restored.
// Round 61 (lane 1, fast): muted mask through an inline getter, as an expression/as a local 85/85 (same code); mode-allowed
// mask computed first, `remote & ~muted & allowed`, 95 (431 bytes); mode read once through voice_get_mode() 104 (406 bytes);
// 0x53bb0 `inline` (diagnostic) 102 at 446 bytes, aligned 20 -> 11, but 0x54890 83 and 0x53a20 84: restored.
// Round 198 (lane 3, fast with 0x54890 and 0x53a20): plain `g_4c9878.initialized && g_476fc8.initialized` in 0x53b90 with
// `inline` on 0x53bb0 43 (429 bytes, aligned 6), but 0x54890 83, 0x53a20 84. Plain in 0x53b90 alone 94 (404 bytes), plain in
// 0x53bb0 alone 85, both 94, both with `inline` 74 (427 bytes), plain in 0x53bb0 with `inline` 102. Named availability bools
// in the 43 form 43/43 (they fold); muted as a ternary 99, `&` of the bytes in a ternary/if 92/91 (eager pair and ESI remote,
// but `test cl, dl`); bools used by the mode tests 93, with an active flag 84, nested mode tests 93. One-file LTCG harness on
// the 43 form: the session getters left out of the link, or an unknown-pointer or volatile store added to them, to 0x53c30 or
// to 0x53de0, gives a byte-identical body at 427 bytes (diagnostics, reverted); added only to 0x53be0, 0x54d20 or 0x56c60 it
// stays 429. A caller passing a global's address or an unknown pointer to the getters or 0x53c30 does not change it; nor do a
// `long` cast or memcpy for the getter's store, or that local's address stored in a global. BLOCKED-BY retail's chain under
// 0x53de0 ending the availability bytes. A volatile store in the getters over this file (diagnostic): 0x55990 60 alone, but
// 0x56590 6 and 0x565c0 148 are lost, so it is not the whole cause. Per-site inlining: LTCG keeps 0x53bb0 out of line in
// 0x53a20 with one more test ahead of the call (64 at 229 bytes with a size test, 67 at 224 with `player < 16`; 2/3/5 tests
// 79/92/118); nested ifs, `!(a || b)` and a named first test stay inlined 84; a shared `a || b` inline 138 (386 bytes), 0x54890
// 88, 0x53a20 92 (neither query inlined inside it).
// Round 204 (fast): the active-mask test as plain availability reads 85 (same code); muted declared before the
// local/active test 118; the existing voice_test_unknownF0 for the inner F0 test 136 (387 bytes, called); an early
// return on !enabled 85 (same code); mask from remote, masked in the availability arm 130 (400 bytes). Restored.
// Round 59 (m3 lane 11, fast): active mask as a ternary on voice_available() 120; remote not named, the mask as
// `players & ~local & ~muted` 138; muted folded into the mask (`mask = remote; if (...) mask &= ~muted`) 130 (400
// bytes); the inner skip as `if (!(mask & bit)) continue;` 85 (same code); `if (!values->enabled) return;` 85 (same code).
// Round 402: Complete per-target rejected/allowed query-chain results in bool/byte/short/long after mask admission.
// Scores 85/85/85/85/85/85/85/85. NOT kept: unmatched outcome forms; original complete body restored.
// Round 146: definition below the matched callers 85; later inline second query 102 (both callers lost);
// inline/force-inline target 85/85; direct second/both getters 85/122; force first query/player-mask helper
// 85/85. Original declarations, position and body restored.
// Round 609: sequential rejected/allowed query updates, branch/conjunction stage graphs
// 85/85/85/85/103/100/80/85. NOT kept: partial flags require a match; 80 gains only five.
// MiniMax D (11-04): no lower score: the mode helpers parameterised on the availability bytes (92, 0x56b70 keeps its
// match) and a bitwise muted gate (105) leave the eager byte pair, the esi remote mask and the inlined second port-flag
// query out of reach.
// MiniMax A (11-04, run 41): no new form, no lower score (85); the second availability byte is still loaded
// lazily where retail loads both before the muted test and keeps the remote mask in esi.
// Round 90 (m3 lane 9, fast with 0x53610/0x54890/0x53a20): the values parameter `*const`, the inner counter at
// function scope, the target bit at function scope, the removal mask at function scope and the active mask declared
// ahead of the player masks, 85 each (same code). Restored.
// M400: Immutable original membership/local/remote bindings, all seven subsets, then
// all with immutable input/player-bit bindings: 85 each. Original bindings restored;
// no readiness/query boundary or remote residency change; all calibrated controls unchanged.
// MiniMax D (11-06): no new form, no lower score: retail reads the flag byte before the mask and passes
// the address of the settings member, an address the comparison cannot map, so the call setup differs
// in more than the register order.
// M3 lane 11 r77 (fast with 0x54890/0x53a20; aligned in brackets): `inline` on 0x53bb0 with its flags in a named local
// 102 [13] (0x54890 83, 0x53a20 84: same as plain `inline`); that with a compiler barrier before the muted test 103 [15];
// round 198's 43 form re-measured 43 [6] (only the second availability byte, loaded lazily, and the remote mask's EDX
// reload against retail's ESI differ); on it a barrier before the muted test 44 [8], muted assigned in an if/else 99
// [7], `~muted & remote` 43 [6] (same code). Every form needs 0x53bb0 inlined, which loses 0x54890 and 0x53a20.
// Round 63 (m3 lane 15; one-file LTCG harness with the session getter files linked, code identical to the build,
// real = harness - 2 here):
// new: memset between the remote and active masks 113; the active mask computed ahead of the player masks 125; the
// values written through a pointer stepped in the for statement 85 (same code); `(active & bit) != 0` 85 (same code);
// the inner removal as `mask ^= 1 << j` 87. Diagnostic, re-measured on the exact harness: plain availability in 0x53b90 with `inline` on 0x53bb0 43, but 0x53a20 84 (lost); retail
// inlines the first query and calls 0x53bb0 in both 0x53a20 and 0x54890, so the second query is BLOCKED-BY 0x53bb0.
// Round 71 (m3 lane 13, fast): `inline` on 0x53bb0 with a repeated `player != NONE` ahead of its call in 0x53a20 102
// (0x53a20 keeps the call and stays matched: a folding test ahead of the call stops the inlining there), but a repeated
// mask test or `i < 16` ahead of it in 0x54890 does not (83/97); `~local & players` 85 (same code); the active mask as a
// ternary on voice_available() 120; `mask = remote; mask &= ~muted;` 85 (same code).
// MiniMax R (11-11): no new form tried this round, no lower score (85): the first branch is at +0xb and leaves for a different place than retail's shared exit at +0x1a6, while the three later branches agree, so only the uninitialized path's exit sits at the wrong address.
// Round 154 (m3 lane 7, one-file harness with src/unknown_058ee0.cpp linked, exact 85 at 427). New, with `inline` on
// 0x53bb0: 0x53bb0 as `voice_available() ? bit != 0 : false` 93 (0x54890 76, 0x53a20 80); as `voice_available() &&
// bit` 89 (71/75). New, body only: `if (local & active & bit)` 82 (415 bytes; 3 lower at another length, not kept);
// after it the local mask read before the player mask 116, `j != 16` 85 (same code), the outer test as `if (!(local &
// bit) || !(active & bit)) continue;` 85 (same code), a named `local & active` mask before the loop 149 (411),
// `active &= local` before the loop 128 (415). Re-measured (same code as the listed `inline` on 0x53bb0, 102 with
// 0x54890 83 and 0x53a20 84): both flag queries through one shared `(port, bit)` inline, 0x53bb0 alone through it,
// through a two-level force-inlined chain, with a named flags local (lane 11 r77), as an early `return false`,
// through a three-bit bitfield view. Diagnostic: a repeated availability test inside 0x53bb0 (more IL; 0x53bb0 itself
// 31) keeps its call out of line in 0x53a20 (0) while 0x54890 still inlines it (119), so the per-site choice follows
// the callee cost, but no same-code form of 0x53bb0 reaches that threshold.
// MiniMax H (11-13), NOT kept (coordinator): 85 -> 74 came from two new __forceinline twins of voice_port_flag1/2
// (voice_port_flag1_inlined / voice_port_flag2_inlined) called at the inner loop's tests: new _inline twins of
// existing functions are not kept (noinline ruling, 10-30).
// MiniMax S (11-14): no new form, no lower score (85): reading the mode once into a local and comparing it with
// 1, 2 and 3 gives 104 (retail keeps the mode in ecx and materialises the active test), so the two mode helpers
// stay; what is left is retail's inner loop calling the port flag getter twice where this build calls the
// out-of-line copy, and retail's remote mask in esi.
// MiniMax S (11-16): no new form, no lower score (85): retail inlines both port-flag queries at this site where this build inlines one and calls the other out of line, and the second availability byte still loads lazily; making the second query inline measures 129 and takes two matched callers with it, so the helper stays as it is.
// MiniMax C (11-16): no new form, no lower score (85): retail still inlines both port-flag queries at this site where this build inlines one and calls the other out of line, and the second availability byte still loads lazily; making the second query inline measures 129 and takes two matched callers with it.
// MiniMax I (11-17): no new form, no lower score.
// MiniMax D (11-17): no new form, no lower score (85): retail still inlines both port-flag queries at this site where this
// build inlines one and calls the other out of line, and the second availability byte still loads lazily; the helper's
// spelling is shared with other files, so its inline decision cannot be changed from here.
// M3 lane 1 r118 (fast with 0x54890/0x53a20/0x53bb0/0x53b90/0x53b40/0x53610/0x56590/0x565c0/0x56b70; control 64, 415 B,
// aligned 17): retail's listing has the three inner tests in the order muted-word bit, first port flag, second port flag
// (0x55a3c-0x55b0a); that order here 85 (427 B, retail's length; aligned 29, of which 10 are padding), so the kept
// flags-first order is a positional gain only. Grid over the test order x `inline` kept or dropped on 0x53b90 x plain
// availability reads in 0x53b90 x `inline` added to 0x53bb0 x plain reads in 0x53bb0 (32 forms). Flags first: plain reads
// in 0x53bb0 64 (same code); `inline` on 0x53bb0 105 (446 B), with its plain reads 89 (433 B); plain reads in 0x53b90 89
// (427 B), with plain 0x53bb0 89, with `inline` 0x53bb0 105, with both 80 (427 B); no `inline` on 0x53b90 97 (391 B: it is
// then called too), with `inline` 0x53bb0 64 (416 B), with its plain reads 73 (407 B). Muted-word bit first: `inline`
// 0x53bb0 102 (446 B), plain 0x53b90 94 (404 B), plain 0x53b90 with `inline` 0x53bb0 43 (429 B, aligned 6: round 198's
// form), with both plain 74 (427 B); no `inline` on 0x53b90 122 (393 B), with `inline` 0x53bb0 84 (427 B). Every form
// with `inline` on 0x53bb0 loses 0x54890 and 0x53a20 (83/84, 72/72 with both plain, 21/28 without `inline` on 0x53b90),
// and every form without `inline` on 0x53b90 loses 0x53b40 (59) as well; no form expands 0x53bb0 here alone. Body unchanged.
// M3 lane 9 r144: this file is built /Ob1, so the non-inline 0x53bb0 is expanded nowhere; with `inline` on it, a
// one-object /GL screen of this file alone (retail's test order) gives the smallest inline threshold that expands
// each site: 0x53bb0 here about 22, 0x53bb0 in 0x54890 about 24, 0x53b90 in 0x53a20 about 27, 0x53bb0 in 0x53a20
// about 45. Retail expands 0x53b90 in 0x53a20 and 0x53bb0 here but calls 0x53bb0 in 0x54890, which no single
// threshold gives with these callers, so the site frequencies of the matched callers (or the calibration) block it.
// Fast trials (control 64): retail's test order muted word, first flag, second flag 85 (427 bytes); with a named
// target bit 85; with the flag tests nested under the mask test 85; with `local & active & bit` 82 (415 bytes); with
// an early return on !enabled 85.
// M3 lane 5 r209 (fast with 0x54890, 0x53a20, 0x53bb0, 0x53b90, 0x53b40; control 64, aligned 17, register-masked 16): on
// this order, an early return on !enabled 64, the inner mask test as a `continue` 64, the inner bit named once for the
// test and the clear 64 (all the same code); the muted test through the existing volatile-read inline twin of 0x53850,
// last 137 or first (retail's order) 137. Retail's first port flag reuses the second availability byte the muted test
// loaded (CL), so retail's reads there are plain; the per-site inlining of 0x53bb0 still blocks the rest.
// MiniMax H (11-19): no new form, no lower score (64): retail's tail tests the two availability bytes before it asks
// the mode query and then makes an extra call to 0x53be0 whose result it halves and masks to a bit, while this build
// asks the mode query first and keeps the two bytes' test after it; the earlier rounds' 85 point forms need the
// active and passive mode flags, which only a byte match would keep, so the control (64) stands.
// MiniMax R (11-20): re-checked, no new form and no lower score (64): the inner loop runs the two port flag queries before the muted word bit where retail runs the muted word bit first and expands both port flag queries in place, and this build calls the second one out of line, so the body is twelve bytes short: BLOCKED-BY 0x53bb0's per site inlining.
// M3 lane 14 r148 (fast with 0x54890, 0x53a20, 0x53bb0, all matched; control 64): diagnostic, the file built /Ob2
// 269 (539 B; 0x54890 113, 0x53a20 84), so retail's single-site expansion of 0x53bb0 is not a file-wide auto-inline;
// the second query written out in the muted-first order 89 (435 B: `test al, 4`, no result byte, both availability
// bytes re-read), in the flags-first order 93 (435 B); with plain availability reads 89 (435 B); both port-flag
// queries written out with plain reads 122 (393 B); a `short` inner index 136 (447 B).
// M3 lane 5 r240 (H2M_FAST with 0x54890, 0x53a20, 0x53bb0, 0x53b90, 0x53b40; on the held lead: `inline` on 0x53bb0,
// muted test first, plain availability reads in 0x53b90: 43 at 429 B, 0x54890 83, 0x53a20 84), 0x53bb0 bodies searched for
// one that keeps its 33 bytes and is expanded only here: the flag word in a local in the block or at the top 43 (same
// decisions); `return available ? bit : false;` 69; the bit tested by a nested `if` 73 (0x53a20 MATCH, 0x54890 71, 0x53bb0
// 44 B); `(flags & 4) >> 2`, `flags >> 1 >> 1` and `!= 0` 43 each (same code); `== 1` 67; an `else result = false;` arm 69;
// an early `return false` 43 (0x53bb0 differs); the flag word tested before the bit 78 (0x53a20 MATCH, 0x54890 96); a
// zero-default flag word 51; `!(f1 || f2)` in 0x53a20 and 0x54890 43, with 0x54890's test split in two `if`s 43. Only
// forms that change 0x53bb0's own code stop an expansion, and none stops 0x54890's. Body unchanged.
// M3 lane 4 r258 (H2M_FAST with 0x54890/0x53a20/0x53bb0/0x53b90 MATCH; control 64, 415 B, aligned 13): inner removal tests
// written second flag first 65; `(flag1 | flag2) || mask test` 55 at 427 B (a padding artefact: aligned 16, masked 21, and
// both queries always run where retail's flag1 exits early; not kept); mask test first then `flag1 | flag2` 80; mask test,
// second flag, first flag 85; first flag, mask test, second flag 89. Retail expands both flag queries here (the second
// only at this site), so the second needs `inline` on 0x53bb0, a held form.
// Candidate and all-allow guards 85, muted-test conditional fallback 85,
// selected rejection bit 83, admitted bit for test and clear 86, admitted
// bit XOR/subtraction 92/92 (muted first). Current order with admitted bit
// clear/XOR 65/67. The second port query remains out of line.
// M3 lane 2 r147 (H2M_FAST with 0x54890, 0x53a20, 0x53bb0, 0x53b90, 0x53b40, all MATCH; control 64): the inner removal
// tests in the two orders not yet listed, second flag, muted, first flag 65 and muted, second flag, first flag 85 (427 B);
// the active word as a select 127; the muted word as a select 105; `~muted & remote` 64 (same code). The second port
// query stays out of line in every form. Body unchanged.
// M3 lane 15 r145 (H2M_FAST with 0x53bb0, 0x53b90, 0x54890, 0x53a20 and 0x53be0, all MATCH; control 64): the inner
// player's bit named once for the test and the clear 64 (same code); that with retail's test order (muted bit, first
// flag, second flag) 85 at 427 B (aligned 40: the second flag is still a call and the first flag's availability bytes
// are read again where retail reuses CL); the mask test and the three queries as nested ifs 64 (same code); the outer
// test as `local & active & bit` 122 (405 B); the outer bit written out at both tests 64 (same code). Still BLOCKED-BY
// 0x53bb0. Body unchanged.
// @retail 0x55990
void function_55990(s_voice_player_values *values)
{
	if (values->enabled)
	{
		dword players = voice_get_player_mask_inlined();
		dword local = voice_get_local_player_mask();
		dword remote = players & ~local;
		dword active = 0;
		if (voice_available())
			active = g_4c9878.unknownEE;
		memset(values->values, 0, sizeof(values->values));
		for (long i = 0; i < 16; i++)
		{
			dword bit = 1 << i;
			if ((local & bit) && (active & bit))
			{
				dword muted = 0;
				if (g_4c9878.initialized && g_476fc8.initialized)
					muted = g_4c9878.unknownF0[i];
				dword mask = remote & ~muted;
				if (voice_mode_is_active())
					mask &= voice_player_settings_get_active_value(&g_527104.settings, i);
				else if (voice_mode_is_passive())
					mask &= function_56c60(&g_527104.settings, i);
				for (long j = 0; j < 16; j++)
				{
					if ((mask & (1 << j)) &&
						(voice_port_flag1(j) || voice_port_flag2(j) || function_57d63(j, i)))
						mask &= ~(1 << j);
				}
				values->values[i] = mask;
			}
		}
	}
}

// @retail 0x56ae0
void voice_player_settings_initialize(s_voice_player_settings *settings)
{
	memset(settings->unknown04, 0, sizeof(settings->unknown04));
	memset(settings->unknown44, 0, sizeof(settings->unknown44));
	memset(settings->unknown84, 0, sizeof(settings->unknown84));
	memset(settings->unknownc8, 0, sizeof(settings->unknownc8));
	memset(settings->unknown108, 0, sizeof(settings->unknown108));
	memset(settings->unknown194, 0, sizeof(settings->unknown194));
	memset(settings->unknown1d4, 0, sizeof(settings->unknown1d4));
	memset(settings->unknown148, 0, sizeof(settings->unknown148));
	settings->unknown18c = -1000;
	settings->unknown188 = -80;
	settings->unknown190 = true;
	settings->initialized = true;
}

// @retail 0x56dd0
long voice_player_settings_get_unknown84(s_voice_player_settings *settings, long index)
{
	long result = 0;
	if (settings->initialized)
		result = settings->unknown84[index];
	return result;
}

// @retail 0x53210
void function_53210(void)
{
	memset(&g_4c9878, 0, sizeof(g_4c9878));
	memset(g_5259b8.values, 0, sizeof(g_5259b8.values));
	g_4c9878.unknown00 = NONE;
	g_4c9878.unknown04 = 0;
	g_4c9878.mode = 0;
	g_4c9878.session_kind = 0;
	g_4c9878.pool_mode = 0;
	g_4c9878.type = 0;
	g_4c9878.pool = NULL;
	g_4c9878.pool2 = NULL;
	g_4c9878.use_pool2 = false;
	g_5259b8.enabled = true;
	g_527104.enabled = true;
	voice_channels_initialize(&g_525a00);
	voice_player_settings_initialize(&g_527104.settings);
	g_4c9878.initialized = true;
}

/* the header of a voice packet: the players it goes to */
struct s_voice_packet_header
{
	word player_mask;
	word unknown02;
};

#pragma pack(push, 2)
struct s_voice_packet
{
	s_voice_packet_header header;
	bool flag;
	byte unknown05;
	word player_mask;
	byte data[10];
};
#pragma pack(pop)

// @retail 0x55b40
void voice_channel_add_packet(s_voice_channel *channel, const s_voice_packet_header *header, const void *data, long size, const bool *flag)
{
	if (channel->active)
	{
		voice_get_current_member();
		if (channel->unknown04 + 1 >= 16)
		{
			channel->unknown04 = 0;
			channel->player_mask = 0;
			memset(channel->data, 0, sizeof(channel->data));
			memset(channel->values, 0, sizeof(channel->values));
		}
		s_voice_packet *packet = &((s_voice_packet *)channel->data)[channel->unknown04];
		packet->player_mask = header->player_mask;
		for (long i = 0; i < 16; i++)
		{
			if (header->player_mask & (1 << i))
				channel->values[i]++;
		}
		packet->header = *header;
		packet->flag = *flag;
		memcpy(packet->data, data, size);
		channel->player_mask |= header->player_mask;
		channel->unknown04++;
	}
}

// @retail 0x56130
void voice_channels_add_packet(s_voice_channels *channels, long channel_index, const s_voice_packet_header *header, const void *data, long size, const bool *flag)
{
	if (channels->initialized && size == 10)
		voice_channel_add_packet(&channels->channels[channel_index], header, data, 10, flag);
}

/* copies of voice_get_players and voice_get_player_mask for the callers
   that retail inlines them into (others call them) */
static inline s_network_session_player *voice_get_players_inlined(void)
{
	s_network_session_player *result = NULL;
	if (voice_available())
	{
		byte *membership = (byte *)voice_get_membership();
		if (membership)
			result = (s_network_session_player *)(membership + 0x10d4);
	}
	return result;
}

static inline dword voice_get_player_mask_inlined(void)
{
	dword result = 0;
	if (voice_available())
	{
		byte *membership = (byte *)voice_get_membership();
		if (membership)
			result = *(dword *)(membership + 0x10d0);
	}
	return result;
}

static inline bool voice_has_remote_talker_inlined(long id)
{
	bool result = false;
	if (voice_available())
		result = voice_xhv_has_remote_talker(&g_476fc8, id);
	return result;
}

// @retail 0x56990
dword voice_get_members_of_players(dword players_wanted)
{
	dword members = 0;
	s_network_session_player *players = voice_get_players_inlined();
	dword player_mask = voice_get_player_mask_inlined();
	long member = voice_get_current_member();
	if (players && player_mask && member != NONE)
	{
		for (long i = 0; i < 16; i++)
		{
			if ((players_wanted & (1 << i)) && (player_mask & (1 << i)))
			{
				long other = players[i].member_index;
				if (other != member && other != NONE)
					members |= 1 << other;
			}
		}
	}
	return members;
}

struct s_voice_route
{
	word members;
	word unknown02;
};

void __stdcall function_565c0(s_voice_routing *routing, dword members, s_voice_route *route);

// @retail 0x56590
void voice_routing_get_route(s_voice_routing *routing, dword players, s_voice_route *route)
{
	route->unknown02 = 0;
	route->members = 0;
	if (routing->enabled && players)
	{
		dword members = voice_get_members_of_players(players);
		if (members)
			function_565c0(routing, members, route);
	}
}
// @retail 0x54850
bool voice_player_has_channel(long player)
{
	bool result = false;
	if (voice_is_enabled())
		result = voice_channels_have_player(&g_525a00, player);
	return result;
}

// @retail 0x54890
bool voice_member_is_route_target(long member)
{
	bool result = false;
	if (voice_get_players_inlined())
	{
		if (voice_current_member_is_unknown00())
		{
			result = true;
			goto done;
		}
		dword local_9b462b = voice_get_local_player_mask();
		for (long i = 0; i < 16; i++)
		{
			if ((local_9b462b & (1 << i)) && !voice_port_flag1(i) && !voice_port_flag2(i))
			{
				dword players = voice_player_values_get(&g_5259b8, i);
				if (players)
				{
					s_voice_route route;
					route.members = 0;
					route.unknown02 = 0;
					voice_routing_get_route(&g_527104, players, &route);
					if (route.members & (1 << member))
					{
						result = true;
						goto done;
					}
				}
			}
		}
	}
done:
	return result;
}

// @retail 0x54410
void voice_update_remote_talker(long player)
{
	s_network_session_player *players = voice_get_players_inlined();
	g_4c9878.unknownF0[player] = *(word *)((byte *)&players[player] + 0x138);
	g_4c9878.unknown110[player] = *(word *)((byte *)&players[player] + 0x13a);
	bool desired = voice_port_flag0_only(player);
	bool const *desired_reference = &desired;
	bool registered = false;
	if (voice_available())
		registered = voice_xhv_has_remote_talker(&g_476fc8, player);
	if (registered != desired)
	{
		bool success = voice_xhv_set_remote_talker(&g_476fc8, player, *desired_reference);
		if (desired && success)
			g_4c9878.unknownEE |= 1 << player;
		else
			g_4c9878.unknownEE &= ~(1 << player);
	}
	else
	{
		if (desired)
			g_4c9878.unknownEE |= 1 << player;
		else
			g_4c9878.unknownEE &= ~(1 << player);
	}
}

// @retail 0x54990
long voice_player_get_bandwidth(long player)
{
	long result = 0;
	if (voice_is_enabled())
		result = voice_channels_get_bandwidth(&g_525a00, player);
	return result;
}

/* ---- talking and the outgoing chat data ---- */

static inline bool voice_xhv_chat_data_ready(c_voice_xhv *xhv, long port)
{
	bool result = false;
	if (xhv->initialized)
		result = xhv->chat_data_ready[port];
	return result;
}

/* whether a player is talking: a local player's port has chat data this
   frame, a remote one is talking in the engine */
// @retail 0x53750
bool function_53750(long player)
{
	bool result = false;
	if (voice_available())
	{
		if (voice_get_local_player_mask() & (1 << player))
		{
			long port = voice_get_player_unknown14(player);
			if (port != NONE)
				result = voice_xhv_chat_data_ready(&g_476fc8, port);
		}
		else
		{
			result = voice_xhv_is_talking(&g_476fc8, player);
		}
	}
	return result;
}

/* the engine's chat data for a local port: routed to the members its
   player talks to, then queued on the player's channel */
// @retail 0x53a20
void __stdcall function_53a20(DWORD port, VOID *data, DWORD size)
{
	if (voice_available())
	{
		long player = voice_find_player(port);
		if (player != NONE && !voice_port_flag1(player) && !voice_port_flag2(player))
		{
			dword players = voice_player_values_get(&g_5259b8, player);
			if (players)
			{
				s_voice_route route;
				bool flag;
				route.members = 0;
				route.unknown02 = 0;
				flag = false;
				voice_routing_get_route(&g_527104, players, &route);
				if (route.members)
				{
					long channel = voice_find_player(port);
					if (channel != NONE)
					{
						if (g_527104.settings.initialized && g_527104.settings.unknown44[channel])
							flag = true;
						voice_channels_add_packet(&g_525a00, channel, (s_voice_packet_header *)&route, data, size, &flag);
					}
				}
			}
		}
	}
}

/* the members of the session that the other session also has */
struct s_voice_membership
{
	long value4c;
	long value50;
	long member_count;
	s_session_member members[16];
};

/* src/unknown_059ad0.cpp */
long network_session_find_member(c_class_58d20 *session, const s_session_member_identity *identity);

// @retail 0x53c70
dword function_53c70(void)
{
	dword mask = 0;
	if (voice_available() && g_4c9878.session_kind == 2)
	{
		c_class_58d20 *session;
		c_class_58d20 *other;
		other = NULL;
		session = NULL;
		if (function_596a0(&session) && function_59670(&other))
		{
			s_voice_membership *membership = NULL;
			if (session->state && session->value4c != NONE)
				membership = (s_voice_membership *)&session->value4c;
			for (long i = 0; i < membership->member_count; i++)
			{
				if (network_session_find_member(other, (const s_session_member_identity *)membership->members[i].words) != NONE)
					mask |= 1 << i;
			}
		}
	}
	return mask;
}

/* src/unknown_054fe0.cpp */
byte *network_session_interface_get_data_4db0(void);

// @retail 0x53d90
inline bool function_53d90(void)
{
	byte *data = network_session_interface_get_data_4db0();
	if (data && *(long *)(data + 0x44) == 7)
		return true;
	return false;
}

// @retail 0x53d40
bool function_53d40(void)
{
	bool flag = false;
	if (g_55e4d0[g_4e9ae8->engine_index])
		flag = TEST_FIELD_BIT(g_4e6948->flags184.bit0);
	bool other = function_53d90();
	if (flag || other)
		return true;
	return false;
}

/* src/unknown_067e10.cpp */
bool function_696d0(c_class_6a600 *world, long player_index);

/* whether the simulation world has a player marked (its flag25) */
// @retail 0x54df0
bool function_54df0(long player_index)
{
	bool result = false;
	if (voice_available() && g_4e6948 && g_4e6948->flag1120)
	{
		c_class_6a600 *world = (c_class_6a600 *)g_4cf77c;
		if (world)
			result = world_player_get(world, player_index) && function_696d0(world, player_index);
	}
	return result;
}

// @retail 0x54e60
byte *voice_get_world_player(long player_index)
{
	byte *result = NULL;
	if (voice_available() && function_54df0(player_index))
	{
		long datum_index = NONE;
		if (player_index != NONE)
		{
			byte *datum = g_4e8c24->data + g_4e8c24->size * player_index;
			datum_index = (*(short *)datum << 16) | player_index;
		}
		result = g_4e8c24->data + (datum_index & 0xffff) * 0x21c;
	}
	return result;
}

// @retail 0x589e0
bool function_589e0(long player_index)
{
	bool result = false;
	if (function_54df0(player_index))
	{
		byte *player = voice_get_world_player(player_index);
		if (player)
		{
			if (*(long *)(player + 0x2c) != NONE || *(long *)(player + 0x30) == NONE)
				result = true;
			else
			{
				long previous_object = *(long *)(player + 0x30);
				byte *object = *(byte **)(g_4e0300->data + (previous_object & 0xffff) * 12 + 8);
				long elapsed = g_510c54->game_time - *(long *)(object + 0x2e0);
				real seconds = (real)elapsed * g_510c54->rate;
				if (1.0f >= seconds)
					result = true;
			}
		}
	}
	return result;
}

// @retail 0x56bd0
long voice_player_settings_get_active_value(s_voice_player_settings *settings, long player)
{
	long result = 0;
	if (settings->initialized)
	{
		bool active = false;
		if (voice_available())
			active = g_4c9878.mode == 1;
		if (active && function_54df0(player) && voice_get_world_player(player))
		{
			if (!function_589e0(player))
				result = voice_player_settings_get_unknown84(&g_527104.settings, player);
			else
			{
				long first = 0;
				if (g_527104.settings.initialized)
					first = g_527104.settings.unknown04[player];
				long second = 0;
				if (g_527104.settings.initialized)
					second = g_527104.settings.unknown44[player];
				result = first | second;
			}
		}
	}
	return result;
}

/* src/game_loop_status.cpp, src/unknown_067e10.cpp */
bool game_loop_running();
bool function_68250(void);

/* chooses the voice mode from the session manager's state */
// @retail 0x544e0
void voice_update_mode(void)
{
	long state = 0;
	if (g_527330.initialized)
		state = g_527330.state;
	switch (state)
	{
	case 3:
		if (!game_loop_running() || !function_68250())
		{
			g_4c9878.mode = 2;
			break;
		}
local_0:
		g_4c9878.mode = 1;
		break;
	case 1:
	case 2:
	case 4:
	case 5:
	case 6:
	case 9:
		g_4c9878.mode = 2;
		break;
	case 8:
		if (game_loop_running() && function_68250())
		{
			goto local_0;
		}
	case 7:
		{
			byte *data = network_session_interface_get_data_4db0();
			if (data && (data[0x48] & 1))
				g_4c9878.mode = 3;
			else
				g_4c9878.mode = 2;
		}
		break;
	default:
		g_4c9878.mode = 0;
		break;
	}
}

/* whether two players may hear each other in team voice: on the same team,
   or always outside it */
// @retail 0x57b60
long voice_players_share_team(long other, long player)
{
	bool early = !voice_available() || g_4c9878.mode != 3;
	if (!early)
	{
		s_network_session_player *players = NULL;
		byte *membership = (byte *)voice_get_membership();
		if (membership)
			players = (s_network_session_player *)(membership + 0x10d4);
		dword mask = voice_get_player_mask_inlined();
		if ((mask & (1 << other)) && (mask & (1 << player)))
		{
			long other_team = (char)players[other].propertiesa8[0x7c];
			long team = (char)players[player].propertiesa8[0x7c];
			if (other_team != NONE && team != NONE && other_team == team)
				return true;
		}
	}
	if (early)
		return true;
	return false;
}

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x56c60
dword function_56c60(s_voice_player_settings *settings, long player)
{
	dword result = 0;
	if (settings->initialized && g_4c9878.initialized && g_476fc8.initialized)
	{
		if (g_4c9878.mode == 2 || g_4c9878.mode == 3)
		{
			long mode = g_4c9878.mode;
			dword mask = 0;
			/* retail's link treats the session lookup below as writing the voice globals */
			_ReadWriteBarrier();
			byte *membership = (byte *)voice_get_membership();
			if (membership)
				mask = *(dword *)(membership + 0x10d0);
			if (mode == 3)
			{
				s_network_session_player *players = (s_network_session_player *)voice_get_players();
				if (mask & (1 << player))
				{
					long team = (char)players[player].propertiesa8[0x7c];
					for (long i = 0; i < 16; i++)
					{
						if (i != player && (mask & (1 << i)))
						{
							long other_team = (char)players[i].propertiesa8[0x7c];
							if (team != NONE && other_team != NONE && team == other_team)
								result |= 1 << i;
						}
					}
				}
			}
			else
				result = mask;
		}
	}
	return result;
}
#pragma function(_ReadWriteBarrier)

// @retail 0x57270
bool function_57270(long player, long other)
{
	bool result = false;
	s_network_session_player *players = voice_get_players_inlined();
	dword local_mask = voice_get_local_player_mask();
	dword mask = voice_get_player_mask_inlined();
	if ((mask & (1 << player)) && (mask & (1 << other)) &&
		(local_mask & (1 << player)) && !(local_mask & (1 << other)))
	{
		if (!function_53d40())
			result = true;
		else if (function_53d90())
		{
			if (!juggernaut_is((short)player) && !juggernaut_is((short)other))
				result = true;
		}
		else
		{
			long team = (char)players[player].propertiesa8[0x7c];
			long other_team = (char)players[other].propertiesa8[0x7c];
			if (team != NONE && other_team != NONE && team == other_team)
				result = true;
		}
	}
	return result;
}

// @retail 0x57a80
real function_57a80(long player, long other)
{
	point3f first = { 0 };
	point3f second = { 0 };
	real result = -1.0f;
	if (function_54df0(player))
	{
		byte *datum = voice_get_world_player(player);
		if (datum)
		{
			long unit = NONE;
			if (*(long *)(datum + 0x2c) != NONE)
				unit = *(long *)(datum + 0x2c);
			else if (*(long *)(datum + 0x30) != NONE)
				unit = *(long *)(datum + 0x30);
			if (unit != NONE)
			{
				function_caf90(unit, &first);
				if (function_54df0(other))
				{
					datum = voice_get_world_player(other);
					if (datum && *(long *)(datum + 0x2c) != NONE)
					{
						function_caf90(*(long *)(datum + 0x2c), &second);
						double x = (double)second.x - first.x;
						double y = (double)second.y - first.y;
						double z = (double)second.z - first.z;
						double sum = z * z + x * x;
						sum += y * y;
						double const &distance = sqrt(sum);
						result = (real)distance;
					}
				}
			}
		}
	}
	return result;
}

#include "unknown_0662e0.h"

#pragma inline_depth(0)
static __forceinline long function_53e31(c_class_58d20 *session)
{
 return session->get_value_5e20();
}
#pragma inline_depth(255)

// @retail 0x53e30
void function_53e30(void)
{
	g_4c9878.unknown00 = NONE;
	if (voice_available())
	{
		c_class_58d20 *session = 0;
		if (voice_get_session(&session))
		{
			byte *membership = (byte *)voice_get_membership();
			long current = voice_get_current_member();
			long host = voice_get_member_index();
			if (membership && current != NONE && host != NONE)
			{
				if (current == host)
				{
					long best = 0;
					long selected = NONE;
					long count = *(long *)(membership + 8);
					for (long i = 0; i < count; i++)
					{
						byte *member = membership + 0xc + i * 0x10c;
						if (current != i && *(long *)(member + 0x98) != 3 &&
							*(long *)(member + 0x90) >= g_network_configuration.valuee4 &&
							*(long *)(member + 0x94) >= g_network_configuration.valuee0 &&
							*(dword *)(member + 0x9c) == (1U << count) - 1 && *(long *)(member + 0x94) > best)
						{
							selected = i;
							best = *(long *)(member + 0x94);
						}
					}
					if (selected == NONE && !voice_mode_is_1())
						selected = current;
					if (function_53e31(session) != selected)
						session->set_value_5e20(selected);
					g_4c9878.unknown00 = selected;
				}
				else
					g_4c9878.unknown00 = function_53e31(session);
			}
		}
	}
}

// @retail 0x57c20
long function_57c20(s_voice_player_settings *settings, long player, long other, long mode)
{
	long result = 0;
	s_network_session_player *players = voice_get_players_inlined();
	if (players)
	{
		dword mask = 0;
		if (settings->initialized)
			mask = settings->unknown04[player];
		if (!function_589e0(player) && !function_589e0(other))
			result = 2;
		else if (function_589e0(other))
		{
			if (mode == 1)
			{
				if (!function_53d40())
					{ result = 1; goto done; }
				if (function_53d90())
				{
					if (!juggernaut_is((short)player) && !juggernaut_is((short)other))
						{ result = 1; goto done; }
				}
				else
				{
					long team = (char)players[player].propertiesa8[0x7c];
					long other_team = (char)players[other].propertiesa8[0x7c];
					if (team != NONE && other_team != NONE && team == other_team)
						{ result = 1; goto done; }
				}
			}
			if (mask & (1 << other))
				result = 3;
		}
	}
done:
	return result;
}

static __forceinline bool voice_test_unknownF0_inlined(long player, long other)
{
	bool result = false;
	if (voice_available())
		result = (g_4c9878.unknownF0[player] & (1 << other)) != 0;
	return result;
}

static __forceinline bool function_57d63(long arg_0, long arg_1)
{
    bool local_0 = false;
    if (g_4c9878.initialized && g_476fc8.initialized)
        local_0 = (g_4c9878.unknownF0[arg_0] & (1 << arg_1)) != 0;
    return local_0;
}

static __forceinline long function_57d64(long other, long result)
{
    if (result == 2 || result == 3)
    {
        dword local_mask = voice_get_local_player_mask();
        for (long local = 0; local < 16; local++)
            if ((local_mask & (1 << local)) && voice_test_unknownF0_inlined(local, other))
            {
                result = 0;
                return result;
            }
    }
    return result;
}

// @retail 0x57d60
long function_57d60(long mode, long player, long other)
{
	long result = 0;
	if (function_57d63(player, other) || function_57d63(other, player)) goto failed;
	{
		switch (mode)
		{
		case 0: { result = 0; goto done; }
		case 1: result = (voice_get_unknownEE() & (1 << player)) ? 1 : 2; break;
		case 2: result = 2; break;
		case 3: result = 3; break;
		default: goto done;
		}
		long controller = voice_get_player_unknown14(player);
		if (controller == NONE)
			{ result = 0; goto done; }
		switch (voice_get_port_state(controller))
		{
		case 1: if (result == 3) { __assume(result == 3); } else result = 2; break;
		case 2: { result = 1; goto done; }
		case 3: { result = 0; goto done; }
		}
		result = function_57d64(other, result);
	}
	goto done;
failed:
	result = 0;
done:
	return result;
}

// @retail 0x577d0
void __stdcall function_577d0(s_voice_player_settings *settings)
{
	dword local_mask = voice_get_local_player_mask();
	s_network_session_player *players = voice_get_players_inlined();
	dword mask = voice_get_player_mask_inlined();
	memset(settings->unknown84, 0, sizeof(settings->unknown84));
	if (players)
	{
		for (long local = 0; local < 16; local++)
		{
			if ((local_mask & (1 << local)) && function_54df0(local) && !function_589e0(local))
			{
				for (long other = 0; other < 16; other++)
				{
					if ((mask & (1 << other)) && !(local_mask & (1 << other)) &&
						function_54df0(other) && !function_589e0(other))
					{
						if (!function_53d40())
							settings->unknown84[local] |= 1 << other;
						else if (function_53d90())
						{
							if (!juggernaut_is((short)local) && !juggernaut_is((short)other))
								settings->unknown84[local] |= 1 << other;
						}
						else
						{
							long team = (char)players[local].propertiesa8[0x7c];
							long other_team = (char)players[other].propertiesa8[0x7c];
							if (team != NONE && other_team != NONE && team == other_team)
								settings->unknown84[local] |= 1 << other;
						}
					}
				}
			}
		}
	}
}

// @retail 0x583c0
void function_583c0(s_voice_player_settings *settings, dword players, long talker)
{
	dword ports = 0;
	for (long player = 0; player < 16; player++)
	{
		long port = NONE;
		if (voice_available() && (voice_get_local_player_mask() & (1 << player)))
		{
			s_network_session_player *session_players = voice_get_players_inlined();
			if (session_players)
				port = session_players[player].unknown14;
		}
		if (players & (1 << player))
		{
			settings->unknown108[player] |= 1 << talker;
			if (port != NONE)
				ports |= 1 << port;
		}
		else
			settings->unknown108[player] &= ~(1 << talker);
	}
	for (long port = 0; port != NONE; port = voice_port_next_index(port))
	{
		if (ports & (1 << port))
		{
			DSMIXBINVOLUMEPAIR pair = {0};
			DSMIXBINS bins = {0};
			bins.dwMixBinCount = 1;
			bins.lpMixBinVolumePairs = &pair;
			pair.dwMixBin = 2;
			pair.lVolume = 0;
			if (g_476fc8.initialized)
				g_476fc8.engine->SetMixBinMapping(voice_xuid(talker), 4, &bins);
			if (g_476fc8.initialized)
				g_476fc8.engine->SetPlaybackPriority(voice_xuid(talker), port, (XHV_PLAYBACK_PRIORITY)0);
		}
		else if (g_476fc8.initialized)
			g_476fc8.engine->SetPlaybackPriority(voice_xuid(talker), port, (XHV_PLAYBACK_PRIORITY)-1);
	}
}

static __forceinline long voice_proximity_object(long player_index)
{
	long object_index = NONE;
	if (function_54df0(player_index))
	{
		byte *player = voice_get_world_player(player_index);
		if (player)
		{
			if (*(long *)(player + 0x2c) != NONE)
				object_index = *(long *)(player + 0x2c);
			else if (*(long *)(player + 0x30) != NONE)
				object_index = *(long *)(player + 0x30);
		}
	}
	return object_index;
}

/* the players each local player hears by distance; retail calls
   function_caf90 at both positions, so those calls are kept out of line */
// @retail 0x57080
void __stdcall function_57080(s_voice_player_values *values)
{
	dword players = 0;
	if (voice_available())
	{
		byte *membership = (byte *)voice_get_membership();
		if (membership)
			players = *(dword *)(membership + 0x10d0);
	}
	dword local = voice_get_local_player_mask();
	memset(values->values, 0, sizeof(values->values));
	for (long i = 0; i < 16; i++)
	{
		if ((local & (1 << i)) && function_54df0(i))
		{
			long object = voice_proximity_object(i);
			if (object != NONE)
			{
				point3f position;
#pragma inline_depth(0)
				function_caf90(object, &position);
#pragma inline_depth()
				for (long j = 0; j < 16; j++)
				{
					dword bit = 1 << j;
					if ((players & bit) && !(local & bit) && function_54df0(j))
					{
						long other = voice_proximity_object(j);
						if (other != NONE)
						{
							point3f other_position;
#pragma inline_depth(0)
							function_caf90(other, &other_position);
#pragma inline_depth()
							real x = other_position.x - position.x;
							real y = other_position.y - position.y;
							real z = other_position.z - position.z;
							real squared = z * z;
							squared += x * x;
							squared += y * y;
							real range = 8.0f;
							if (g_510c94)
								range = g_510c94->motion_sensor_range;
							if (sqrtf(squared) <= range + 1.5f)
								values->values[i] |= bit;
						}
					}
				}
			}
		}
	}
}

static __forceinline bool function_56790(dword arg_0, long arg_1)
{
	dword local_0 = arg_0;
	dword local_1 = 1 << arg_1;
	return (bool)(local_0 & local_1);
}

// @retail 0x56790
void function_56790(dword *remaining, long *capacity, dword allowed, word *selected,
	bool preserve_one, long excluded, bool *blocked)
{
	for (long i = 0; i < 16; i++)
	{
		dword bit = 1 << i;
		if (function_56790(*remaining, i) && (allowed & bit) && *capacity > 0 &&
			(!preserve_one || excluded != i))
		{
			if (*capacity == 1 && preserve_one)
			{
				*blocked = *blocked || bit != *remaining;
				if (*blocked)
					break;
			}
			*selected |= (word)(1 << i);
			*remaining &= ~bit;
			(*capacity)--;
		}
	}
}

// @retail 0x53b00
long __stdcall function_53b00(long player, byte *output, long capacity)
{
 long result = 0;
 if (voice_is_enabled())
  result = function_56380(output, &g_525a00, player, capacity);
 return result;
}

/* fills a route with the members the routing reaches now and the ones it defers */
// @retail 0x565c0
void __stdcall function_565c0(s_voice_routing *routing, dword remaining, s_voice_route *output)
{
 output->members = output->unknown02 = 0;
 if (routing->enabled == 0 || !remaining)
  return;
 long capacity = voice_get_mode_value();
 if (capacity <= 0)
  return;
 bool preserve = voice_unknown00_valid();
 long preferred = voice_get_unknown00();
 long mode = 0;
 if (g_4c9878.initialized)
  mode = g_4c9878.unknown04;
 dword eligible = voice_get_current_member_value();
 word selected = output->members;
 bool blocked = false;
 word deferred = output->unknown02;
 if (!function_56790(eligible, preferred))
  preserve = false;
 else if (preserve && (mode == 1 || mode == 2))
 {
  blocked = true;
  goto finish;
 }
 if (mode == 2)
  capacity = 1;
 {
  long kind = voice_get_session_kind();
  deferred = (word)(remaining & ~eligible);
  remaining &= eligible;
  if (deferred)
   blocked = true;
  dword allowed = kind == 2 ? function_53c70() : 0;
  if (kind == 2)
   function_56790(&remaining, &capacity, allowed, &selected, preserve, preferred, &blocked);
  if (capacity != 1 || !preserve || !blocked)
   function_56790(&remaining, &capacity, (dword)-1, &selected, preserve, preferred, &blocked);
 }
finish:
 if (preserve)
 {
  if (blocked)
  {
   word_flags_set(&selected, preferred, true);
   deferred |= (word)remaining;
  }
  else if (function_56790(remaining, preferred))
  {
   word_flags_set(&selected, preferred, true);
   word_flags_set(&deferred, preferred, true);
  }
 }
 else
  deferred = 0;
 output->members = selected;
 output->unknown02 = deferred;
}

/* records the session's player mask and identities, and resets the voice
   channels when either has changed since the last call */
// score 2 (was 2): only mask/source reload order after logging differs; frame and copy graph agree.
// Round 19: unchanged; the mask/source reloads after the logging call stay in our order (edi then
// ebp) against retail's (ebp then edi); the reduced image shows 2 even for the full-build match.
// Round 196: query-carrier names singly/paired, comparison-input names singly/paired,
// comparison ordinal and copy ordinal names [2, 2, 2, 2, 2, 2, 2, 2]; original source restored.
// Round 198: read-only mask/source/both 2/2/2; unsigned/signed-word/unsigned-word copy ordinal 123/318/318; unsigned comparison/both ordinals 162/186. Opening body restored.
// Round 107 (plain checks: --fast shows 2 even for a full-build match): this body matched in the full
// build of lane 5 round 82 and network_voice.cpp has not changed since, so the reload order follows the
// rest of the image. Mask declared first, logging call inside the loop, empty declared first, reason
// declared first, `if (changed) log; else return;`: 2 each, same pair swapped.
// P464: Comparison-test bool/raw signed result producers 146/205; signed/unsigned short
// comparison ordinal 188/188; literal mismatch byte/word/long 2/2/2; success-continue 2.
// NOT kept: no exact post-format reload recovery; original comparison and whole body restored.
// P466: Empty identity long/byte/word storage with memset 2/2/2; aggregate-zero
// dword/byte/word and read-only dword/byte 2/153/142/2/153 on the plain-copy graph.
// NOT kept: no exact reload recovery; original empty input and complete body restored.
// Round 89 (m3 lane 2): the existing out-of-line player and mask accessors in place of the inline
// copies 244 (fast, 518 bytes: both stay calls); the scan without the changed flag (`break`, then
// `if (i == 16) return;`) 116 (fast); both bounded copies of this file through a file-local
// terminating-copy helper (as src/font_loading.cpp has) 2 (plain; 0x41640 flipped to 4); the
// identity pointer set to the player then overridden by empty 80 (fast); the mask test written
// mask-first 3 (fast). All restored.
// b14B (11-17) walk against retail, fast build on r847: statement by statement the body agrees with retail; the only
// difference is the pair of reloads after the voice_sprintf call (retail 0x5468e ebp = players slot, then 0x54692
// edi = mask slot; ours loads edi first). It follows the image, not the C text: score 2 on the plain 4-object image
// and with 0x7f070, 0xb3de0, 0x8e240 or 0x6b4a0 added; MATCH with 0x74aa0, 0x755d0, 0x8df50 (805 objects) or 0x193560
// added (the shared input there is matchmaking_hopper.obj). The seeded full-build report (r847) has score 2 at +0xff,
// the same pair. 0x53610 0x53de0 0x55060 0x560e0 stay MATCH. players + i, a byte-offset source and const mask act
// like the original in all those images; a source or destination pointer local, mask/players declared first or late,
// long mask: 2 (plain image); mask or players evaluated first/late 163-201; one identity copy 394.
// @retail 0x54590
void function_54590(void)
{
 s_network_session_player *players = voice_get_players_inlined();
 dword mask = voice_get_player_mask_inlined();
 char reason[0x80];
 reason[0] = 0;
 dword empty[3];
 memset(empty, 0, sizeof(empty));
 if ((dword)g_4c9878.unknown20 != mask)
 {
  strncpy(reason, "session player mask change", sizeof(reason));
  reason[sizeof(reason) - 1] = 0;
 }
 else
 {
  bool changed = false;
  for (long i = 0; i < 16; i++)
  {
   const long *previous = &g_4c9878.unknown24[i * 3];
   const void *identity = mask & (1 << i) ? (const void *)&players[i] : empty;
   if (memcmp(previous, identity, sizeof(empty)))
   {
    changed = true;
    break;
   }
  }
  if (!changed)
   return;
  voice_sprintf(reason, "player identifier change");
 }
 g_4c9878.unknown20 = mask;
 for (long i = 0; i < 16; i++)
 {
  if (mask & (1 << i))
   memcpy(&g_4c9878.unknown24[i * 3], &players[i], sizeof(empty));
  else
   memcpy(&g_4c9878.unknown24[i * 3], empty, sizeof(empty));
 }
 voice_channels_reset(&g_525a00);
}

struct s_online_mutelist_globals
{
 long startup_task;
 long tasks[4];
 XONLINE_MUTELISTUSER users[4][MAX_MUTELISTUSERS];
 long user_counts[4];
};
extern s_online_mutelist_globals g_4c99c0;
void voice_update_local_properties(long controller_index);

// @retail 0x541a0
void __stdcall function_541a0(long player)
{
 long port = voice_get_player_unknown14(player);
 bool talking = false;
 if (port != NONE)
 {
  s_network_session_player *players = voice_get_players_inlined();
  bool disabled = g_4c9878.port_states[port] == 3;
  bool allowed = true;
  long mode = g_476fc8.initialized ? g_476fc8.port_modes[port] : 0;
  if (TEST_FIELD_BIT(g_54e8e0[port].flag5))
   allowed = function_1906da(port);
  talking = voice_port_can_talk(port) && !disabled && allowed;
  if (!disabled && allowed)
  {
   if (!mode)
    voice_set_port_mode(port, 2);
  }
  else if (mode == 2)
   voice_set_port_mode(port, 0);
  if (disabled)
   g_4c9878.unknown110[player] |= 2;
  else
   g_4c9878.unknown110[player] &= ~2;
  if (talking)
   g_4c9878.unknown110[player] |= 1;
  else
   g_4c9878.unknown110[player] &= ~1;
  if (!allowed)
   g_4c9878.unknown110[player] |= 4;
  else
   g_4c9878.unknown110[player] &= ~4;
  if (TEST_FIELD_BIT(g_54e8e0[port].flag5))
  {
   XONLINE_MUTELISTUSER *users = 0;
   long count = g_4c99c0.user_counts[port];
   if (count != NONE)
   {
    users = g_4c99c0.users[port];
    if (!users)
     goto publish_properties;
    dword player_mask = voice_get_player_mask();
    dword local_mask = voice_get_local_player_mask();
    for (long other = 0; other < 16; other++)
    {
     if ((player_mask & (1 << other)) && !(local_mask & (1 << other)))
     {
      bool muted = false;
      for (long i = 0; i < count; i++)
       if (!memcmp(&users[i], &players[other], 12))
       {
        muted = true;
        break;
       }
      if (muted)
       g_4c9878.unknownF0[player] |= (word)(1 << other);
      else
       g_4c9878.unknownF0[player] &= (word)~(1 << other);
     }
    }
   }
  }
publish_properties:
  voice_update_local_properties(port);
 }
 if (talking)
  g_4c9878.unknownEE |= (word)(1 << player);
 else
  g_4c9878.unknownEE &= (word)~(1 << player);
}

// @retail 0x54030
void function_54030(void)
{
 if (voice_get_players_inlined())
 {
  dword local = voice_get_local_player_mask();
  dword players = voice_get_player_mask_inlined();
  for (long i = 0; i < 16; i++)
  {
   if (players & (1 << i))
   {
    if (local & (1 << i))
    {
     if (voice_has_remote_talker_inlined(i) &&
      g_476fc8.initialized && (g_476fc8.unknown0c & 4))
      g_476fc8.engine->UnregisterRemoteTalker(voice_xuid(i));
     function_541a0(i);
    }
    else
     voice_update_remote_talker(i);
   }
   else
   {
    if (voice_has_remote_talker_inlined(i) &&
     g_476fc8.initialized && (g_476fc8.unknown0c & 4))
     g_476fc8.engine->UnregisterRemoteTalker(voice_xuid(i));
    g_4c9878.unknownEE &= ~(1 << i);
   }
  }
 }
}

class c_voice_observer;
void function_586f0(long player, c_voice_observer *observer, long *count,
	DSMIXBINVOLUMEPAIR *bins, const point3f *position);

// @retail 0x58590
void function_58590(long talker, long mode, c_voice_observer *observer, long player)
{
	if (mode == 0)
	{
		if (g_476fc8.initialized)
			g_476fc8.engine->SetPlaybackPriority(voice_xuid(talker), 4, (XHV_PLAYBACK_PRIORITY)-1);
		return;
	}
	if (mode == 2)
	{
		DSMIXBINVOLUMEPAIR pairs[4] = {0};
		DSMIXBINS bins = {0};
		bins.dwMixBinCount = 3;
		bins.lpMixBinVolumePairs = pairs;
		pairs[0].dwMixBin = 0;
		pairs[1].dwMixBin = 1;
		pairs[0].lVolume = 0;
		pairs[1].lVolume = 0;
		pairs[2].dwMixBin = 2;
		pairs[2].lVolume = 0;
		voice_xhv_set_mix_bins(&g_476fc8, talker, 4, &bins);
	}
	else if (mode == 3)
	{
		point3f position = {0};
		if (function_54df0(talker))
		{
			byte *world_player = voice_get_world_player(talker);
			if (world_player && *(long *)(world_player + 0x2c) != NONE)
			{
				function_caf90(*(long *)(world_player + 0x2c), &position);
				DSMIXBINS bins = {0};
				DSMIXBINVOLUMEPAIR pairs[4] = {0};
				bins.lpMixBinVolumePairs = pairs;
				function_586f0(player, observer, (long *)&bins.dwMixBinCount, pairs, &position);
				voice_xhv_set_mix_bins(&g_476fc8, talker, 4, &bins);
			}
		}
	}
	if (g_476fc8.initialized)
		g_476fc8.engine->SetPlaybackPriority(voice_xuid(talker), 4, (XHV_PLAYBACK_PRIORITY)0);
}

struct s_voice_route_choice
{
	long player;
	long mode;
	real distance;
};

// @retail 0x57eb0
void function_57eb0(s_voice_player_settings *settings, long talker,
	const s_voice_route_choice *choices, long count)
{
	bool radio = false;
	long nearest = NONE;
	dword headset = 0;
	for (long i = 0; i < count; i++)
	{
		const s_voice_route_choice *choice = &choices[i];
		if (choice->mode == 1)
			headset |= 1 << choice->player;
		else
			headset &= ~(1 << choice->player);
		if (choice->mode == 2 || choice->mode == 3)
		{
			settings->unknownc8[choice->player] |= 1 << talker;
			if (choice->mode == 2)
				radio = true;
			else if (!radio && choice->mode == 3 &&
				(nearest == NONE || choices[nearest].distance > choice->distance))
				nearest = i;
		}
		else
			settings->unknownc8[choice->player] &= ~(1 << talker);
	}
	function_583c0(settings, headset, talker);
	if (radio)
	{
		DSMIXBINVOLUMEPAIR pairs[4] = {0};
		DSMIXBINS bins = {0};
		bins.dwMixBinCount = 3;
		bins.lpMixBinVolumePairs = pairs;
		pairs[0].dwMixBin = 0;
		pairs[1].dwMixBin = 1;
		pairs[2].dwMixBin = 2;
		pairs[0].lVolume = 0;
		pairs[1].lVolume = 0;
		pairs[2].lVolume = 0;
		if (g_476fc8.initialized)
			g_476fc8.engine->SetMixBinMapping(voice_xuid(talker), 4, &bins);
		if (g_476fc8.initialized)
			g_476fc8.engine->SetPlaybackPriority(voice_xuid(talker), 4, (XHV_PLAYBACK_PRIORITY)0);
	}
	else if (nearest != NONE)
		function_58590(talker, 3, (c_voice_observer *)settings, choices[nearest].player);
	else if (g_476fc8.initialized)
		g_476fc8.engine->SetPlaybackPriority(voice_xuid(talker), 4, (XHV_PLAYBACK_PRIORITY)-1);
}

extern "C" void _ReadWriteBarrier(void);
#pragma intrinsic(_ReadWriteBarrier)
// @retail 0x57960
void __stdcall function_57960(s_voice_player_settings *settings)
{
	dword local = voice_get_local_player_mask();
	dword enabled = 0;
	if (voice_available())
		enabled = g_4c9878.unknownEE;
	settings->unknownc4 = 0;
	for (long talker = 0; talker < 16; talker++)
	{
		dword bit = 1 << talker;
		if (!(local & bit) && (enabled & bit) && voice_available() &&
			voice_xhv_has_remote_talker(&g_476fc8, talker))
		{
			s_voice_route_choice choices[16] = {0};
			long *local_0 = &choices[0].mode;
			for (long player = 0; player < 16;)
			{
				long mode = 0;
				if (local & (1 << player))
				{
					mode = voice_players_share_team(player, talker);
					_ReadWriteBarrier();
					mode = function_57d60(mode, player, talker);
					if (mode) settings->unknownc4 |= bit;
				}
				local_0[-1] = player;
				local_0[0] = mode;
				local_0 += 3;
				player++;
			}
			function_57eb0(settings, talker, choices, 16);
		}
	}
}
#pragma function(_ReadWriteBarrier)

// @retail 0x56df0
void __stdcall function_56df0(s_voice_player_settings *settings, long talker, long mode)
{
	bool active = false;
	if (settings->initialized && g_4c9878.initialized && g_476fc8.initialized)
		active = voice_get_mode() == 1;
	if (active && voice_xhv_has_remote_talker(&g_476fc8, talker))
	{
		dword local = voice_get_local_player_mask();
		bool valid = function_54df0(talker);
		s_voice_route_choice choices[16] = {0};
		s_voice_route_choice *choice = choices;
		for (long player = 0; player < 16; player++)
		{
			long route = 0;
			if ((local & (1 << player)) && function_54df0(player) && valid)
			{
				long requested = function_57c20(settings, player, talker, mode);
				long result = function_57d60(requested, player, talker);
				if (requested == 1) settings->unknown148[talker] = g_510c54->game_time;
				if (result == 3) choice->distance = function_57a80(player, talker);
				route = result;
				if (route) settings->unknownc4 |= 1 << talker;
			}
			choice->player = player;
			choice->mode = route;
			choice++;
		}
		function_57eb0(settings, talker, choices, 16);
	}
}

void __stdcall function_56f60(c_voice_observer *observer);
void __stdcall function_57360(c_voice_observer *observer);

// @retail 0x56b70
void function_56b70(s_voice_player_settings *settings)
{
	if (settings->initialized)
	{
		if (voice_mode_is_active())
		{
			function_56f60((c_voice_observer *)settings);
			function_57080((s_voice_player_values *)settings);
			function_57360((c_voice_observer *)settings);
			function_577d0(settings);
		}
		else if (voice_mode_is_passive())
			function_57960(settings);
	}
}

// @retail 0x53610
void function_53610(void)
{
	if (voice_available())
	{
		voice_do_work();
		if (g_4c9878.pool_mode == 2)
		{
			voice_update_session_kind();
			voice_update_mode();
			long kind = 0;
			if (voice_available()) kind = g_4c9878.session_kind;
			if (!kind) g_4c9878.mode = 0;
			function_54590();
			bool (*query_enabled)(void) = voice_is_enabled;
			if (!query_enabled())
			{
				voice_xhv_unregister_remote_talkers(&g_476fc8);
				voice_reset_talkers();
			}
			else
			{
				function_54030();
				function_53e30();
				function_56b70(&g_527104.settings);
				if (g_5259b8.enabled) function_55990(&g_5259b8);
			}
		}
	}
}


static inline long voice_packet_clamp_count(byte count, long minimum, long maximum)
{
 if (count < minimum) return minimum;
 if (count > maximum) return maximum;
 return count;
}

// MiniMax A (11-20): no new form, no lower score (139): retail keeps the channel in EBP and walks a packet cursor in EDI, ours keeps the data base in EBP with base+offset addressing
// score 139 (was 139): the member takes EBP (retail: the channel from entry, the member spilled), the cursor and route
// homes differ.
// MiniMax B (11-18, round 2): no new form, no lower score (139): the rewrite that advances the packet cursor on its
// own measures 164 at 478 bytes against retail's 488 - the same form lane A measured - so the split stands: retail
// keeps the channel in ebp from the entry and the cursor in edi, this build keeps the data pointer in ebp and
// recomputes the cursor from the counter.
// MiniMax S (11-17): no new form, no lower score (139): retail still takes the channel in ebp from the entry and
// spills the member to a frame slot, while this build keeps the member in ebp and the data pointer in a register; the
// cursor's absolute start and the route home are as before, and the permuter over this body found nothing below 139.
// MiniMax C (11-15): no new form, no lower score (139): the member still takes ebp where retail keeps the channel from entry and spills the member, and the cursor and route homes still differ.
// MiniMax G (11-13): no new form, no lower score (139): the member takes ebp where retail keeps
// the channel from entry and spills the member, and the cursor and route homes differ.
// MiniMax B (11-13): no new form, no lower score (139): the channel in a local (139), the cursor advanced on its own (180) and both together (180) leave the difference - retail keeps the channel in ebp from the entry and the data pointer in edi, ours keeps the data pointer in ebp.
// MiniMax A (11-13): no new form, no lower score (139): a hoisted packet cursor (`byte *packet = data + 2`, advanced by
// the packet size) scores 164 and a local copy of the channel compiles to the same bytes as the baseline, so retail's
// split (the channel in ebp from the entry, the member spilled to the stack) stays out of reach here.
// MiniMax D (11-14): no new form, no lower score (139): retail builds the route in the channel argument's own
// slot, which only frees if the channel is copied to a register at entry, and its cursor is an absolute pointer
// while the count is re-read from the data argument each pass; capturing the count in a local scores 166 and
// recomputing the cursor from the counter at the loop's end scores 167, both worse than the 139 baseline.
// MiniMax S (11-16): no new form, no lower score (139): retail still keeps the channel in ebp from the entry and spills the member, while
// this build loads the data pointer and keeps the member in ebp; the cursor and route homes are as before.
// MiniMax B (11-18): no new form, no lower score (139): retail still takes the channel in EBP from the entry and spills the member to a frame slot, while this build keeps the member in EBP and the data pointer in a register, and the cursor's absolute start and the route home still differ; the recorded forms (the local channel 139, the advanced cursor 164/159/180) do not move the split.
// MiniMax D (11-11): no new form, no lower score (139).
// MiniMax G (11-12): no new form tried this round, no lower score (139): retail keeps the channel in ebp from the
// entry and spills the member, while ours loads the data pointer and keeps the member in ebp.
// MiniMax H (11-12): no new form, no lower score (139): our EBP still serves the member and the consumed update in turn, so the channel gets no register, and retail's cursor still starts with an immediate add that ours folds into the shared constant.
// Earlier, score 139 (was 139): the member takes EBP (retail: the channel from entry, the member spilled), the cursor and route
// homes differ.
// MiniMax R (11-09): no lower score (139): the base and the offset kept apart, with the packet pointer never
// formed, compiles to the same bytes as the indexed form (139), so the gap is not the address form: retail holds
// the channel in ebp from the entry and spills the member word, ours holds the member in ebp and reloads the
// channel; the cursor advanced on its own (164), without the size local (159), the member moved before the calls
// (161 and 170) and the member in the loop scope (164) are all worse.
// MiniMax R (11-09): no lower score (139): with the packet cursor advanced on its own the code reaches retail's
// cursor in edi but the member word takes ebp over the channel (164), and naming the channel, the member or the
// size does not change that; the indexed form stays the better of the two (139).
// Earlier, score 139 (was 139): channel/cursor/count registers and route homes differ;
// MiniMax E (11-07): no new form, no lower score; the channel and the cursor homes follow the earlier
// rounds' forms, none of which reaches retail's channel in ebp with the cursor in edi.
// MiniMax h (11-06): no new form, no lower score: with the packet cursor advanced on its own the member word takes ebp over the channel (164), and retail's channel in ebp, advancing cursor in edi and stride in esi stay out of reach together.
// Round 19: unchanged; retail keeps the channel in ebp from entry and the cursor in edi, ours
// gives the member index ebp and reloads the channel; no new form was found.
// frame pointer 188; short stride 163; short index 139; recipient-bit reuse 161; route name 139.
// Round 53: retail keeps the channel in ebp from entry (its home then holds the route) and spills
// the current member; ours gives the member ebp and reloads the channel. A packet cursor beside the
// consumed count (retail keeps both) declared first 141f, after the count 164f, before the count
// 139f; with an if-set stride 165f; member at loop scope 141f; 36 cursor/stride/member forms
// screened, none puts the channel in a register.
// frame and extent agree; channel/cursor/count registers and route homes differ.
// Independent packet cursor 164; byte packet index 172; enclosing member lifetime 139;
// entry route lifetime 139; ordinary delivery arm first 162.
// Round 198: remaining set before/after delivery, word/dword 172/182/154/179; with independent
// cursor 141/182/164/188; separate input/remaining sets with conditional clear 146/176, with
// cursor 158/184.
// Round 82: a separate cursor declared after the count gives retail's EDI cursor and BX mask, but the member index
// then takes EBP instead of the channel 164; cursor at entry 141; with the member at function scope 164; route at
// function scope 139 (same code); cursor, route scope and the index declared before the loop 164.
// Round 86: cursor with a channel-index local in the relay arm 158, with a dword member mask 190, with split
// NONE/kind tests 164; index local, late stride and early counter 158; index local in the other arm 162;
// a 245-form toggle search 139+.
// Round 95: the allocator ranks the member local just above the channel parameter: with the packet cursor,
// one extra per-iteration read of channel->index (diagnostic store to a global) flips EBP to the channel
// from entry, the route moves into the channel's parameter slot and, with the entry order size, stride,
// count, cursor, only that store, `add edi, ecx` for `add edi, 2` and retail's separate zero-trip exit
// (`mov eax, ecx`) differ (harness 61); the member forced to memory gives the same flip (136). Local and
// parameter names change nothing. Real trials: cursor 164; cursor with entry order size/stride/count/cursor
// 167; member fetched before a shared kind test 169; clear through word_flags_set 164; named member bit
// 179; screened: the existing inline bit test 164, arms swapped 156, member declared at loop top 167,
// a shared two-call receive helper 184.
// Round 446: Direct count bounds disjunction/lower-first/upper-first 185/179/180;
// byte admission with original clamp 139; byte/long direct bounds 185/182;
// direct/byte bounds with independent cursor 177/177. Original complete body restored.
// Round 112: with a packet cursor declared after the consumed count (cursor first merges it back into
// data + consumed) the member takes EBP; one extra channel->index read at the loop, relay-arm or plain-arm top
// (diagnostic store, reverted) flips EBP to the channel (harness aligned 84 -> 30), leaving only the entry
// order and retail's separate zero-trip exit. Real: cursor with consumed, size, cursor, size order 164; consumed,
// size, packet size, cursor 162; shared two-call receive helper 192 (457 bytes); plain arm first 153; member at
// function scope with split NONE/kind tests 164. Screened, no flip: 24 entry orders, 48 combinations of split
// tests, members before the call, route at loop scope, (word) clear, `> 1`, `packet_size > size`.
// Round 52: packet cursor beside the count (retail's EDI cursor, BX mask, ESI size; the member then
// takes EBP and the channel stays in its stack slot) 164; with that, the NONE and kind tests split 164,
// the mask tested as `(members >> member) & 1` 164, a per-iteration channel-index local 164; the
// cursor at function scope before the stride 141. Screened: a channel pointer copy and the mask
// declared before the member read change nothing.
// Round 20 (no cursor; data keeps EBP, consumed EDI): NONE and kind tests split 139; size reduced
// before the stride 139; consumed, size, stride order 158 (476 bytes); `*(word *)&packet[1]` with
// `== 0` 139; the plain arm through the out-of-line talker test 181 (450 bytes, not inlined); one
// shared kind test before the relay branch 180 (467 bytes).
// Round 188 (fast, matched caller 0x564c0 checked each time): packet cursor beside the count 164
// (member index in EBP, channel reloaded, no separate zero-trip exit); the shared clamp helper
// with a long count 156 (0x564c0 64), with a single result 138 (0x564c0 87), with short parameters
// 156 (0x564c0 139); each of those with the cursor 163/163/166. All restored.
// Word/dword exclusion mask with original/independent cursor and signed/unsigned native ordinals:
// 166/168/174/175/166/168/174/175. Channel/cursor/member ownership remains; original source restored.
// Round 485: Native wire header/relay/direct records, seven subsets 139 each;
// all records with independent cursor 164. Actual record types and full body restored;
// channel/cursor/member ownership and route homes remain.
// Round 64 (lane 9, fast; 0x564c0 matched each time): a packet cursor `data + 2` advanced at the loop's end
// 164 (478 bytes: retail's EDI cursor, the member in EBP); with the member and packet-kind tests split 164;
// the cursor advanced before the size and count updates 167; the member declared, then assigned 164; the
// member bit cleared before the talker test 161 (469 bytes).
// Round 236: Uniform delivery-index observations at iteration/delivery/entry/loop-storage scopes,
// each with original/independent packet cursor: 203/195/192/193/203/195/203/195.
// Original complete body restored; channel/cursor/member/route allocation remains.
// Round 36 (lane 15, fast): the packet cursor beside the count re-measured 164 (478 bytes; the member
// takes EBP, the channel is reloaded into ESI per relay arm); the packet pointer declared at function
// scope and set per iteration 139 (same code); on the cursor form, the member bit cleared before the
// delivery calls 161 (469 bytes); the route zeroed as `= { 0 }` 139 and as a chained assignment 139
// (same code).
// Round 485: Native wire header/relay/direct records, seven subsets 139 each;
// all records with independent cursor 164. Actual record types and full body restored;
// channel/cursor/member ownership and route homes remain.
// Round 285: uniform packet-kind byte/word/long/dword observations with original cursor
// 150/137/154/154, with independent cursor 143/140/152/152. Word form recovers entry
// channel/cursor homes but gains only two and is 17 bytes short; complete body restored.
// Round 64 (lane 9, fast; 0x564c0 matched each time): a packet cursor `data + 2` advanced at the loop's end
// 164 (478 bytes: retail's EDI cursor, the member in EBP); with the member and packet-kind tests split 164;
// the cursor advanced before the size and count updates 167; the member declared, then assigned 164; the
// member bit cleared before the talker test 161 (469 bytes).
// Round 502: retained full-length remaining/end-offset bounds, retained payload bound and
// admission-time budget debit 161/161/146/161; with independent packet cursor
// 167/155/148/173. Original byte budget and complete body restored; channel/cursor homes remain.
// Round 241: Route output initialized at relay entry/member result/member-set/after local delivery
// 163/163/163/139; the same boundaries with an independent packet cursor 165/167/168/168.
// Original route initialization, live count reads and complete body restored.
// Round 511: consumed result advances before dispatch, after/before size admission,
// with original/admission byte debit 163/172/176/176; unsigned ordinals
// 163/172/176/176. Original body restored; channel/cursor/member
// and route homes remain.
// DH272: Known-present recipient bit removal by XOR/subtraction with long/short current member and
// original/independent packet cursor gives 143/161/167/162/144/161/167/162. All regress; original AND-not
// removal, member domain, live header counts, cursor, calls and body restored. Channel/packet/member
// ownership and route homes remain.
// Round 61 (lane 1, fast; 0x564c0 matched each time): cursor with the order size, consumed, stride, cursor 162 (479
// bytes, aligned 59 against 70; the member still takes EBP); plain-arm talker test through a one-line inline taking the
// channel 162; no __stdcall 139, with the cursor 162; positive nested member/mask/size tests, each and all, 162; bit
// test and clear through by-value/by-pointer inlines and word_flags_set 162; relay/direct arm as __forceinline helpers
// 162/162 (plain inline helpers stay out of line 375/227); the relay helper without the cursor 139 (all same code).
// Round 520: native unsigned/short current member on original/word current-kind and
// original/independent cursor graphs: 139/166/137/169/164/159/140/150.
// Two-count word-kind lead remains below the keep floor; original body restored.
// AC484: Native byte/word/short/dword count-clamp returns 174/138/150/156;
// with independent cursor 159/166/150/159. Every form raises the matched parser's
// count; NOT kept: shared-user regressions, and the best target gain is only one.
// Original clamp, live count observations, cursor and full body restored.
// Round 540: One in-function local-delivery phase, conditional/test-produced query result
// and repeated/shared payload receiver: 193/191/193/191; independent packet cursor
// 194/192/194/192. All shorten to 455/456 bytes and regress; complete original dispatch,
// live count reads, removal timing, helpers and body restored.
// Round 198 (lane 3, fast with 0x564c0): cursor with consumed, size, stride, cursor 162 (479 bytes); stride, size, consumed,
// cursor 164 (478); consumed, `size -= consumed`, cursor, stride 164 (478). 0x53720 marked `inline` and called from both
// arms 139 (same code): the relay arm stays a call and the plain arm inlines, so retail's two forms need no separate copy;
// with the cursor forms 162/164. One-file LTCG harness: cursor forms with the session getters left out, or every other
// source left out, keep the member in EBP (aligned 79-85); 11 entry and tail orders of the cursor form the same (79-82).
// Round 126 (lane 8, fast with 0x564c0): cursor with an unsigned stride 164; cursor with a short consumed count
// 163 (479 bytes); consumed advanced right after the size test 163 (475 bytes); cursor with a const reference to
// the channel index for every talker read 164; cursor with a count local from data[1] 159 (0x564c0 13).
// Round 549: Loop failures assign NONE to consumed and break; long/int/short/dword result
// with original traversal 138/138/159/138, with independent packet cursor
// 153/153/154/153. NOT kept: best gain is one; original body restored.
// Y553: Live count-byte pointer with offset/cursor/walking-formal/difference result
// 139/164/144/180; field-reference forms 139/164/144/180. Both formal walks lose
// the matched parser to 13. NOT kept: no gain; original complete body restored.
// Y556: Payload-relative consumed result, long/int/dword/short, original/independent
// packet traversal: 169/169/169/164/154/154/154/153. NOT kept: exact-only induction/domain forms do not match;
// original header-inclusive result, live counts and complete body restored.
// M344: Relay formal char/signed-char/byte/short/word/int/long/dword
// 139/139/139/155/155/140/140/140, all 488/488 bytes. NOT kept: byte forms tie;
// wider forms regress and lose the parser. Original bool contract/body restored.
// Round 50 (m3 lane 15, fast; 0x564c0 matched each time): on the packet-cursor form (consumed, `size -= consumed`,
// cursor; 164) the member mask declared at function scope 164; a named word member bit used for the test and the
// clear 168 (471 bytes); the route zeroed as `= { 0 }` 164; the size debited right after the size test 169; on the
// original form the mask at function scope 139 (same code). Retail's EBP channel / spilled member still not reached.
// DL254: Matched current-member/incoming-packet/channel-enqueue inline subsets; then all three with an independent cursor:
// 139/139/139/139/139/139/139/164. Original complete body and helper contracts restored;
// channel/cursor/member and route homes remain; no qualifying provider change.
// Y556: Payload-relative consumed result, long/int/dword/short, original/independent
// packet traversal: 169/169/169/164/154/154/154/153. NOT kept: exact-only induction/domain forms do not match;
// original header-inclusive result, live counts and complete body restored.
// M344: Relay formal char/signed-char/byte/short/word/int/long/dword
// 139/139/139/155/155/140/140/140, all 488/488 bytes. NOT kept: byte forms tie;
// wider forms regress and lose the parser. Original bool contract/body restored.
// Round 69 (m3 lane 10, fast; 0x564c0 matched each time): cursor with size, consumed, cursor, stride, then size, consumed,
// cursor at the tail 164 (478 bytes, aligned 79, masked 32). Diagnostic, the member in a scratch global: 68 (aligned 45, masked
// 16): EBP then holds the channel from the entry and the arms, the route's home and the tail are retail's, so the member
// being a register candidate is the cause. Keeping a local member out of registers failed, all 164 with the same code: an
// unused pointer to it, a const reference to the call, a one-element array, an assignment inside its test (alone, split from the
// packet test, NONE-initialised), declared at function or loop scope; a local copy of the channel 164; a dword member mask 190 (459 bytes).
// Twice-used relay payload at entry/member/admission/mask producers: indexed
// packets 139/139/139/139; independent packet cursor 164/164/164/164 (478 bytes).
// Original payload expressions, live count reads, traversal and complete body restored.
// P302: Actual mutable channel-object reference with cursor/word-kind/dword-member
// interference graphs 0-7: 139/164/166/164/139/164/166/164. NOT kept: receiver/domain nonmatches;
// original pointer contract, caller and whole body restored; channel/cursor/member homes remain.
// P304: Native forwarding-route memset with independent cursor/short member/word kind,
// interference masks 0-7: 152/172/172/170/150/149/163/156. Original route stores, observations and full body restored;
// channel/packet/member ownership and route homes remain.
// DH335: stride char/signed-char/byte 160 each; word 163; int 139; 64-bit 164;
// finite two-value stride and immutable long 139 each. No channel-home recovery;
// non-native extents regress. Original stride and complete body restored.
// Y579: Actual stream-end bounds with offset/cursor/walking-formal/difference results,
// admission-time producer 161/165/172/174, entry producer 161/145/167/163.
// NOT kept: no gain or native channel-home recovery; original whole body restored.
// AC547: Actual kind-byte pointer/reference at packet/dispatch-arm scope,
// indexed traversal 139/139/139/139; independent packet cursor 164/164/164/164.
// NOT kept: alias-only forms do not match; original byte reads and full body restored.
// Y583: Separate original relay/direct remote-query return storage bool/byte/short/long,
// original/independent cursor 139/139/139/139/164/164/164/164. NOT kept: one-use
// return carriers do not match; complete original body restored; channel/cursor homes remain.
// Y587: Complete parse-validation state bool/byte/short/long, indexed then independent
// cursor: 207/207/215/214/221/221/230/230; caller stays matched. NOT kept:
// test-produced state misses and adds a frame word; original direct failures and full body restored.
// Round 72 (m3 lane 1, one-file harness: control 139 [48] at 483 bytes, 0x564c0 0; masked aligned in brackets): consumed, size, a packet cursor, stride, tail size, consumed, cursor 161 [33] (478 bytes); index form as
// written that way 155 [48]; channel index through an inline getter 160 [33], without the cursor 144 [44]; `__assume(channel != NULL)` at the entry, `data`, both, or `(channel != NULL)` ahead of the relay test or the route, or
// `__assume(member >= NONE)` 161 [33] each (same code). The member still takes EBP over the channel; not kept, body unchanged.
// Round 90 (m3 lane 9, fast with 0x564c0): the channel parameter `*const` 139 (same code), on the packet-cursor form 164;
// the relay flag copied into a local before the stride 139 (same code), on the packet-cursor form 164; the packet as a
// const pointer `&data[consumed]` 139 (same code). Restored.
// Y603: Independent remaining budget with original signed debits, initializer/separate
// subtraction and local/entry declaration, indexed/cursor traversal: 175/175/175/175/159/159/159/159.
// NOT kept: all regress; indexed forms also lose 0x564c0 to 12. Original body restored;
// channel/member/cursor and route homes remain.
// Y605: Complete positive header/size/relay-member admission scopes, seven subsets
// on indexed traversal then all with packet cursor: 163/139/139/163/163/139/163/160.
// NOT kept: header ownership regresses, other scopes tie; original full body restored.
// Y607: Current-member char/signed-char/64-bit signed/unsigned result carriers,
// indexed then independent packet traversal: 168/168/156/156/164/164/156/156. NOT kept:
// no byte match from non-native result carriers; original whole body and observations restored.
// DH385: Packed recipient-word memcpy/memmove on original/cursor and scalar/bulk-route
// graphs: 139/188/164/187/152/183/172/179. NOT kept: no gain or native channel-home recovery;
// original word read, live counts, packet induction, route stores and whole body restored.
// MiniMax D (11-06): no new form, no lower score.
// M3 lane 11 r77 (fast with 0x564c0; aligned in brackets, base [84]): on the packet-cursor form (164 [81], the member in
// EBP): the plain arm first (`if (!relay)`) 157 [188] (frame 0x14); the stride named entry_size 164 (same code); `size -= 2`,
// cursor, stride, consumed 141 [77] (the cursor folds into data + consumed in EBP and the member goes to memory as in
// retail, but the channel is still reloaded per use); size, consumed, cursor, stride 164 [79]; the 141 order with the route
// declared at the loop-body top 141 [77].
// Round 63 (m3 lane 15; one-file LTCG harness, code identical to the build except one call-setup line): new: consumed
// set first and `size -= consumed` 139 (same code); consumed, `size -= consumed`, stride, then a packet cursor beside
// the count 159 (479 bytes); `(members >> member) & 1` on the indexed form 162; the packet-kind test ahead of the
// NONE member test 138 (1 lower, retail tests the member first; not kept); consumed advanced before the size 139.
// Re-measured: consumed, size, stride 152 (476 bytes); route at function scope 139; NONE and kind tests split 139.
// Round 71 (m3 lane 13, fast with 0x564c0): on the packet-cursor form (164, member in EBP) the packet-kind test ahead of
// the member call 145, the plain arm as `else if (packet[0] < 2) {...} else return NONE;` 164, both 145; without the
// cursor the plain-arm form 139 (same code) and both 150. The member keeps EBP in every form.
// Round 61 (m3 lane 2, fast with 0x564c0; baseline 139): retail loads the channel into EBP at entry, ahead of the count test,
// keeps the member in memory ([esp+0x1c], stored right after its call) and puts the route in the channel argument's dead home;
// ours gives EBP to the member, so the route takes the fourth frame slot. On the separate-cursor form (164, 478 B, re-measured):
// a short member 159 (480 B), the route at function scope 164, the cursor advanced in the for's increment 166, the cursor
// declared ahead of the consumed count 139 (488 B, the cursor folds back into data + consumed: main's code), a named member bit
// for the test and the clear 179 (467 B), the stride chosen after the cursor 164. The member keeps EBP in every one.
// Round 142 (m3 lane 7; one-file LTCG harness, exact: 139 at 488, 0x564c0 matched): the packet cursor beside the count
// 164 (478 B, member in EBP); a channel->index read at the loop top (diagnostic store) 163 (no flip there). 96 random forms
// (no cursor / cursor advanced last / cursor advanced first x member test joined or split x four entry orders of the
// stride, `size -= 2` and consumed x `(members >> member) & 1` or the mask test x member declared at the loop top or
// in the relay arm): best 139 (main's code), the cursor forms 162-165 (aligned 58 against 71).
// Round 68 (m3 lane 2, fast; 0x564c0 matched; indexed form): the member bit cleared after the bit test's block 167 (477 bytes);
// a `byte count = data[1]` local for the clamp test 139 (aligned 87); the packet pointer formed after the size test 139 (same
// code); `(1 << member) & members` 139 (same code); the member, kind and empty-mask tests in one condition 139; re-measured on
// the indexed form: the packet size set by an if 155. All callees are matched, so no callee convention is involved.
// M3 lane 10 r88 (fast with 0x564c0, matched each time; base 139, aligned 84, masked 47): the member test as `member < 0` 139
// (other code: `test; jl` where retail has `cmp eax, -1`); on the packet-cursor form (re-measured 164, masked 36) the same test 165;
// the cursor set from the count, `data + consumed`, after `size -= 2` and `consumed = 2` with the stride last 164 (masked 32), with
// the stride before the cursor 162 (479 bytes). In every cursor form our EBP serves the member and the consumed update in turn and
// the channel gets no register; retail's cursor starts with `add edi, 2` (an immediate, outside the constant 2 that `size -= 2` and
// `consumed` share in ECX) and its zero-trip exit returns that ECX. No untried natural form left that adds a channel reference.
// Round 168 (m3 lane 6): no trial; every candidate form (member storage and scope, channel-index reads, cursor
// placement and order, arm order, bit helpers, count and size locals, record views) is already listed above, the
// cursor forms all give 164 with the member in EBP, and no untried natural form is left that adds a channel use or
// removes a member use.
// R151 (m3 lane 7): no new form. The relay arm already reads channel->index three times and passes the channel once,
// the plain arm reads it three times and the member has retail's four references (call result, NONE test, test shift,
// clear shift), all as retail; the cursor, member, channel-index, test-split, clamp, route and count spellings left are
// those listed above, so no natural form is left to try against the member's EBP.
// Round 154 (m3 lane 7, one-file harness, exact; 0x564c0 matched). New: the indexed form with the size debited right
// after the size test 159 (479 bytes), that with the plain arm first 159 (490); the packet-cursor form with the plain
// arm first, the early debit and the members word read before the member call 161; cursor with the plain arm first
// and the early debit 190 (517); on the members-first indexed form: the packet-kind test before the member call 151,
// all packet tests before the member call 151, `(members >> member) & 1` 163 (480). Re-measured: the members word
// read before the member call 138 (round 112 screened it; 1 lower, statement order, not kept); indexed plain arm
// first 162 (ordinary delivery arm first 162); cursor plain arm first 157 (lane 11 r77); cursor early debit 169
// (round 50); the route at the relay arm top (same code). The member keeps EBP in every cursor form.
// T101: Actual local-recipient result reused for delivery and conditional removal,
// bool/byte/short/long: indexed 138/138/160/163, independent cursor 158/158/158/159.
// NOT kept: one-count leads fail the gain gate and change the native frame; no
// predicate carrier matches. Original complete body, live counts and masks restored.
// T107: Ordinary inline call metadata on settings/forwarding-route/XHV-query providers,
// seven subsets 139 each, complete native packet cursor 164. NOT kept: no gain;
// original provider declarations, independent call boundaries and full body restored.
// M3 lane 14 r95 (fast with 0x564c0): retail's cursor starts with `add edi, 2` (an immediate beside the ECX = 2 that
// size and consumed share), the shape a strength-reduced index product gives. New: consumed typed dword 139 (same code);
// consumed typed short 161 (480 B, the packet rebuilt per iteration); the packet at `data + 2 + i * packet_size` 165 (478
// B: it is strength-reduced into an EDI cursor, but the start shares ECX in the preheader and the member keeps EBP); a
// `packets = data + 2` base ahead of the loop with `packets + i * packet_size` 166; the stride computed at the loop top
// 160 (476 B). Re-measured: the packet cursor beside the count 164.
// M3 lane 2 r86 (fast with 0x564c0, matched throughout): re-measured the packet cursor beside the count 164 (478 B: EDI
// cursor, ESI stride, EBX relay then members, all as retail; only EBP differs, ours the member, retail the channel with
// the member in a frame slot). On it: the member declared at function scope 164; the NONE and kind tests split 164; the
// member fetched after the kind and members tests 147 (EBP still the member; alignment only); the cursor started as
// `&data[2]` 164 (still `add edi, ecx`); the member assigned inside its NONE test 164.
// MiniMax R (11-16): no new form, no lower score (139): re-measured the packet cursor beside the count (164, 478 B) and the
// same cursor with the size decrement before the stride (167); a channel copy in a local of its own gives the cursor's bytes
// unchanged, so the only difference left is the register the channel and the member take - retail keeps the channel in ebp
// from the entry and the member in a frame slot, this build keeps the member in ebp and the channel in esi - and the frame
// slot is not reachable from the source. The permuter over this function (300 s, 5 tries) found nothing below 139.
// MiniMax D (11-15): no new form, no lower score (139): the packet cursor form (164) still keeps the member in ebp where retail keeps the channel and puts the member in a frame slot, and a channel copy in a local of its own gives the same bytes; the indexed form stays at 139.
// P574: Original per-packet byte-budget admission, positive/complementary
// bool/byte/short/long: 139/139/139/139/139/139/139/139. NOT kept: no exact native channel,
// cursor/member or route-home recovery; original budget test and full body restored.
// M3 lane 14 r103 (fast with 0x564c0, matched throughout; none kept): on the packet-cursor form (re-measured 164, 478 B):
// the member declared at the loop-body top 164, the loop as a while with `i++` last 164, the member at function scope
// initialised to NONE 164, the members word declared at the loop-body top 164 (all same code); probe, not retail's order:
// the member bit cleared before the talker calls 161 (469 B): the member then no longer lives across the calls, the
// channel takes EBP from entry as in retail and the frame shrinks to 0xc, so the member's life across the three calls is
// what wins it EBP in our build (retail keeps it in its frame slot); the permuter on the cursor form (613 s, 39 tries)
// best 148 (a volatile cursor and numbered constant locals, not a form).
// M3 lane 6 r190 (fast with 0x564c0, matched each time): on the packet-cursor form (re-measured 164, 478 B; the member
// still takes EBP and the channel no register): the member bit as a `word` local for the test and the clear 168 (471 B);
// the channel bound as `s_voice_channel *const &` to the parameter 164 (same code); the NONE and kind tests split with
// the channel bound as `s_voice_channel &` for every index read 164 (same code); an `if (0 < data[1])` guard around the
// loop 166 (494 B: the clamp's count takes EBP instead); the cursor, consumed and size advances in the order cursor,
// consumed, size 167. Body unchanged.
// MiniMax C (11-17): no new form, no lower score (139): retail still takes the channel in ebp from the entry and spills the member, where this build keeps the member in ebp and the channel in a register; the member's life across the three calls is what wins it ebp here, and clearing the bit before the talker call measures 161, so the body stands.
// M3 lane 3 r259 (fast with 0x564c0, matched each time; control 139): the packet cursor beside the count (re-measured
// 164, 478 B: member EBP, channel no register); on it: the cursor start written `data + 2u` 164 (still `add edi, ecx`, so
// the constant's type does not keep retail's immediate), consumed declared before the clamp 154 (479 B), the route at
// function scope 164 (same code), one `long result = NONE` exit with `goto done` for the failures and `result =
// consumed` after the loop 158 (488 B, retail's length; member still EBP).
// The build loads data into EBP. Retail keeps the channel there, walks the packet in EDI and spills the member.
// indexed entry-index name 139; cursor with local-member name 164; cursor with packet-index name 164;
// cursor with four named slot roles 164; forwarding mask comparison 181;
// forwarding mask comparison with cursor 168; packet-kind clamp 182; packet-kind clamp with cursor 188.
// NOT kept: the cursor with recipient-set comparison recovers EBP but drops the member slot and shortens the body.
// One-based packet number with consumed offset, ordinal offset, separate cursor or
// cursor-distance result: long 156/165/163/180; short 156/162/163/179. The channel
// still needs EBP at entry and the member needs its own spill.
// m3 lane 8 r186 (fast with the matched caller 0x564c0, matched throughout; control 139): on the packet-cursor form
// (`byte *packet = data + 2;` beside the count, advanced at the loop's end; re-measured 164 at 478 bytes, the member in
// EBP): the relay arm's talker result in a named bool 164; both arms' talker results named 164; the member's NONE test
// through a by-value inline predicate 164 (all three the same code as the cursor form); the packet-kind test ahead of
// the member fetch with the NONE test after it 145 (EBP still the member); the entry order size, consumed, cursor,
// stride 164; the cursor advanced ahead of the size and consumed updates 167. No form moved the channel into EBP.
// Body unchanged.
// Pre-dispatch budget debit, consumed increase after guard, both updates, and consumed
// increase before guard: indexed 159/163/176/172; independent cursor 169/166/173/168.
// Round 140 (m3 lane 9, fast with 0x564c0, matched each time): retail's ten extra bytes are a fourth exit on the loop's
// zero-trip edge, `mov eax, ecx` returning consumed's initial 2 from ECX (a tail copy of `return consumed;`). On the
// packet-cursor form (re-measured 164): the arms swapped with the test negated 157 (488 B, EBP then `packet + 3`); the
// cursor advanced in the for header 166; the counter declared before the loop 164; `members` declared before the loop
// 164; the advances ordered cursor, size, consumed 167. The compile-only harness keeps the cursor in ESI here (no help).
// M3 lane 5 r204 (fast with 0x564c0, matched each time; control 139): the stride as `11 + 2 * relay` (0x55c40's
// spelling) 178 (469 B); on the packet-cursor form (`data + 2` beside the count): the member's NONE test split ahead of
// the kind test 164, the member test as a nonzero `word` 168 (471 B), the stride as `11 + 2 * relay` 165, the tail
// advances ordered consumed, size, cursor 164. The member still takes EBP in every cursor form.
// m3 lane 15 r107 (fast with 0x564c0, matched each time; control 139): on the packet-cursor form (re-measured 164, 478 B:
// data in EDI from entry and `add edi, ecx`, the member in EBP, the channel in no register): the head ordered size,
// consumed, cursor, stride 164 (same code); the cursor declared before the clamp (`byte *packet = data + 2;`, or `byte
// *packet = data;` with `packet += 2;` after the stride, the clamp reading data[1] or packet[1]) 141 each (478 B: the
// member is then spilled to a frame slot as retail's, but the cursor is folded back into `data + consumed` with consumed
// in EBP, so the channel still has no register); consumed declared above the clamp 154 (479 B). 0-7 unused locals at the
// top 139 each on this body and 164 each on the cursor form (no symbol-count state).
// m3 lane 8 r198 (fast with 0x564c0, 0x55b40, 0x565c0, 0x56df0 and 0x53890, all matched each time; control 139; aligned =
// difflib count with addresses masked, 93 at the control). The packet-cursor form (`byte *packet = data + 2;` advanced at
// the loop's end, `consumed` kept beside it) re-measured 164 (478 B, aligned 88). Diagnostics on it, not keepable (a
// compare of `channel->index` with a constant, placed in turn): at the loop's end 127 (488 B, aligned 52); after the else
// arm's submit call 79 (488 B, aligned 41); after the relay arm's submit call 115 (488 B, aligned 41); at the top of the
// relay arm 161 and after the else arm's kind test 160 (no change of homes); the member's second shift removed 186 (438
// B). In the first three and the last the channel takes EBP from the entry and the member goes to a frame slot, the route
// moves into the channel argument's slot and the whole loop body is retail's: one more reload point of the channel after
// a submit call (whose index argument takes ESI) is the margin, or the member not living across the calls. With the
// diagnostic after the else arm's submit call, the 24 orders of the four pre-loop statements (s `size -= 2`, z the size
// ternary, c `consumed = 2`, p the cursor): s z c p 70 (488 B, aligned 24: the pre-loop block is retail's but for the
// cursor's `add edi, ecx` three instructions late where retail has `add edi, 2`, the consumed and packet-size slots
// swapped, and no separate zero-trip exit returning ECX); c p z s 79 (aligned 30); c z p s and c z s p 80 (33); z c p s,
// z c s p and z s c p 79 (35); c s z p and s c z p 64 (41); c p s z, c s p z and s c p z 79 (41); every order with p first
// 94 to 96 (65 to 66) and s z p c, z p c s, z p s c, z s p c 146 (493 B): in those the cursor is folded back into `data
// + consumed`. `size -= consumed` for `size -= 2` gives the same code in each of its twelve orders. On s z c p: the
// cursor's start as `data + 2u`, `&data[2]`, `data + sizeof(word)`, `data + consumed`, and the 2 of consumed or of both
// as `sizeof(word)` 70 each (same code: the shared 2 stays in ECX for the add). Main's indexed form with the diagnostic
// at either place 146/157 (data keeps EBP, no cursor). Natural forms on the cursor form with the s z c p head, no
// diagnostic, 167 (478 B, aligned 124) and unchanged by each of: the member and kind tests as two ifs; `members == 0`;
// a block-scoped copy of the channel pointer for the arms; the member's bit cleared outside its test; no explicit
// convention on the function (also 139 on main's form: the compiler picks the same stack convention). The kind test
// ahead of the member fetch 162. The member or the channel renamed (four names) and the member declared at function
// scope 164 each on the plain cursor form (same code). A shared inline delivery helper taking the channel for both arms'
// two calls 192 to 197 (457/451 B: the index is read once). Lead, saved as
// build/r198/lead-0x55e10-70-diagnostic.diff in the m3-lane8 worktree: the s z c p cursor form needs a natural reason
// for the channel to outweigh the member (a fifth reload point), then the immediate cursor add and the zero-trip exit.
// m3 lane 15 r108 (fast with 0x564c0, matched each time; control 139): on the packet-cursor form (re-measured 164, 478 B):
// the relay arm's three talker calls reading a `long channel_index = channel->index;` named inside the member-bit test
// 158 (476 B: the member takes EBP straight from the call and the channel no register, retail reads [ebp + 0xc] three
// times), that local in the plain arm too 193 (455 B); the member typed dword 164, the stride typed dword 164 (same code
// each). On this body: the relay arm's channel_index local 174 (471 B).
// M3 lane 1 r133 (fast with 0x564c0, matched each time; control 139; one-file compile-only screens where said). Retail's
// frame is consumed, stride, counter, member from the bottom, and slots follow declaration order: with the four locals
// declared bare at the function top in the order consumed, stride, cursor, counter and assigned later (size, stride,
// consumed, cursor), lane 8's diagnostic form has retail's slots, 69 (488 B; left there: the cursor's add, the diagnostic
// and the zero-count exit). Retail's separate zero-count exit (`mov eax, ecx` after the in-loop failure exit) comes out
// only when that path has a return statement of its own: `i = 0; if (i < data[1]) { for (; i < data[1]; i++) { ... }
// return consumed; } return consumed;` (the same as a do-while under the guard). On the diagnostic base that form is
// retail's listing but for `add edi, ecx` three rows late where retail has `add edi, 2` and the diagnostic's own two
// rows, 74 (493 B); without the diagnostic 160 (490 B, the member in EBP as before); with the guard as `0 < data[1]`
// around an ordinary for 182 (506 B), the guard with one shared return 165 (494 B, lane 6's form). A compile-only grid of
// the 120 orders of the five pre-loop statements on the guarded form: size, consumed, cursor, stride, counter gives
// retail's instruction order (the add right after the size subtraction), but every order with the cursor after consumed
// adds through the register holding 2 and every order with the cursor ahead of consumed folds the cursor into `data +
// consumed`, whatever the declaration order, the order of the three tail advances (89 to 91 on the diagnostic base), an
// unsigned consumed or the count read through the parameter's own address. Same code as the plain cursor form (167 with
// the size, stride, consumed, cursor head): the member read through `long const &` or `long &`, the member filled
// through an out-parameter of an inline reader (diagnostic), the in-loop failures as `valid = false; break;` with `if
// (!valid) return NONE;` or `return valid ? consumed : NONE;` after the loop, the flag declared first. The flag in the
// loop test (`valid && i < data[1]`) 153 (488 B, another loop layout); one `result` exit with gotos on the diagnostic
// base 165 (479 B, no separate zero-count exit); consumed initialised at its top declaration 78 on the diagnostic base
// (the 2 stored ahead of the clamp); the data parameter itself advanced with a `header` copy for the count 141/142 (the
// link then passes data in EAX, ret 0xc); every local (member, members, route too) declared at the top in four orders
// 160 each on the guarded form. Compile-only, same pre-loop code as `data + 2`: the cursor from a member address, a
// typed pointer step, two one-byte steps or sizeof. Left: a natural reason for the channel to outrank the member (the
// diagnostic's extra channel read after a submit call still does it) and the immediate in the cursor's add. Body unchanged.
// Later the same round, on the guarded two-return form without the diagnostic (160): the three tail advances written at
// the end of each arm 147 (488 B: the member goes to a frame slot as in retail, but the stride takes EBP and the channel
// still has no register); the function as a member of the channel type, `this` on the stack or in ECX, 160 (and 139 on
// the kept body: same code, so `this` earns no register either); same code as the guarded form: the relay arm's tests as
// nested positive ifs with the failures in else arms, each failure in its own if, the size test as a block around the
// body, the three relay failures in one condition, the relay arm's talker test through the inline copy, the plain arm
// as an else-if; the member typed short 159 (492 B); members typed dword 179 (469 B). Probes (not forms): without the
// member's NONE test the member still takes EBP (159); with either shift taking a second 0x54a20 call instead of the
// member (so the member does not live across the talker calls) the channel takes EBP from entry (69 / 121).
// M3 lane 11 r136 (fast with 0x564c0, matched each time; control 139): the route block taken out of the relay arm and
// run after both arms on a `word members = 0;` set at the loop top 154 (488 B; frame 0x14, the data pointer in ESI),
// on the packet-cursor form (re-measured 167, 478 B) 162 (476 B); the member chosen by `relay ? 0x54a20 : NONE` after the
// size test 139 (same code: the two relay tests merge), on the cursor form 167 (same code); the kind byte read once into a
// local used by both arms' tests and the two 0x56df0 calls 150 (480 B). Body unchanged.
// M3 lane 13 r127 (fast with 0x564c0, 0x55b40, 0x565c0, 0x56df0 and 0x53890, all matched each time; control 139). Re-measured
// as bases: the packet cursor beside the count 164 (478 B) and lane 1 r133's guarded two-return form with the four top
// declarations 160 (490 B: retail's layout and zero-count exit, the member in EBP, the channel reloaded into ESI per arm,
// `add edi, ecx`). Reading: retail's `add edi, 2` is an immediate although ECX holds 2 from the count's store, which fits
// the cursor's start being emitted ahead of `consumed = 2`; every such order folds the cursor back, measured here without
// the diagnostic on the guarded form: cursor, size, stride, consumed 130 (489 B); cursor, consumed, size, stride 130; that
// with the cursor advanced first in the tail 130; that with the cursor declared first 130 (same code each: the member goes
// to a frame slot as in retail, consumed takes EBP and the packet is re-formed from data + consumed in EDI per pass; the
// guard and its second return are match-only forms, so not kept). Both arms as one inline delivery function taking the
// channel, the packet and the relay flag, called from the indexed, cursor and guarded loops 139 / 164 / 160, with the
// size test joined to it by `||` 160 (same code each: the function's own locals rank as the loop's did). The size and
// count advanced through an inline function taking their addresses, at the tail or at the head too (the count started at
// zero), on the guarded, cursor and indexed loops 160, 160, 164, 164, 139, 139 (same code: neither stays in memory for it).
// Body unchanged.
// M3 lane 7 r202 (fast with 0x564c0, matched each time; control 139): the packet-cursor form with the head ordered
// size, stride, consumed, cursor re-measured 167 (478 B, the member in EBP, the channel in no register). Diagnostics on
// it (each one conditional store to a spare byte, removed at once): an extra conditional block inside the member's live
// range, after the members test 166 and ahead of the bit clear 155 (frame 0x14; the member keeps EBP, so the member's
// block span is not what ranks it); a `channel->index` compare in the entry block 159 (the channel is loaded into ESI for
// that test and dropped, so an entry-block reference does not earn it EBP here); lane 8's compare after the plain arm's
// submit call re-measured 73 (491 B: channel EBP from entry, member in its frame slot). So the pair flips only for a
// channel reference that follows a submit call; references ahead of the calls and block-count changes do not move it.
// Source forms on the cursor form: the plain arm as its own `if (!relay)` (no else) 167 (same code); the member's bit
// cleared ahead of the talker calls 152 (469 B, frame 0xc, channel EBP); the channel's index named once per pass at the
// loop top 195 (445 B); the data parameter itself advanced with the count read through a `header` copy 163 (494 B;
// 0x564c0 lost, 13: the data then arrives in EAX); the relay arm's three failure tests in one condition with the members
// word read first 167 (same code). Body unchanged.
// M3 lane 13 r134 (fast with 0x564c0 and 0x54810, both MATCH throughout; control 139): the cursor form re-measured
// with the stride first 164 (478 B, aligned 81) and with retail's pre-loop order (size, stride, consumed, cursor) 167
// (aligned 115). On the latter: the member and packet-kind checks as separate ifs with `members == 0`, the member
// declared at the top of the loop body, `members` declared ahead of the member call, the loop counter at function
// scope, `!= 0` spellings of the two membership tests, positive nesting of every check with `else return NONE` arms:
// 167 each (the same code); the real 0x53720 called from both arms with no `inline` (the file is /Ob1, so neither
// expands) 199 (445 B), on this body 181 (450 B); one failure exit through a result variable and a goto (diagnostic of
// the exit shape) 159 (489 B, frame 0x14); a grid of 56 pre-loop bodies (the 24 orders of the size, stride, consumed
// and cursor statements with `size -= 2` or `size -= consumed` and `data + 2` or `data + consumed`) best 141 (aligned
// 77), none with the channel in EBP. Diagnostics, removed: one extra read of `channel->index` after the plain arm's
// submit call 67 at 488 B (aligned 13: the channel loads into EBP at entry, the member is spilled at its definition,
// the route takes the channel's dead argument home and the loop is retail's; left: `add edi, ecx` for retail's `add
// edi, 2` and the zero-trip exit, where retail has its own epilogue returning ECX); the same read after the relay
// arm's submit 104 (aligned 33); the clear written with a constant bit (one fewer member use) 159 (462 B, frame 0xc)
// also puts the channel in EBP; a named member bit for the test and the clear 176 (467 B, the channel is not held).
// With the plain-arm read kept: `long consumed = 2; size -= consumed;` ahead of the stride and the cursor 62 (retail's
// immediate `add edi, 2`, but the flag loads into DL and the size goes through ESI), that with the cursor ahead of the
// stride 77, size, consumed, cursor, stride 77, size, cursor, stride, consumed 88, cursor, size, stride, consumed 88,
// `&data[2]` for the cursor 67. Reading: the channel (seven uses) and the member (three) are one use apart for the
// last callee-saved register; no natural statement was found that changes either count without code.
// The channel and count-byte address still share the packet-base allocation decision. Live count by pointer or
// reference, with or without an entry snapshot, all 139 indexed and 164 with an advancing packet. The advancing
// packet still gives EBP to the member and shortens the body to 478 bytes.
// Shared payload boundary with indexed or walking packet. Prefix and stride 172/167,
// end-minus-payload 168/158, stride-minus-payload 159/160, selected address 139/164.
// Using the queued payload extent for the prefix gives the same 172/167.
// The new boundary either grows the frame or leaves channel allocation unresolved.
// Remaining bytes from input size and consumed bytes, indexed/walking 161/167.
// Whole packet end versus input size 161/155. Last complete packet offset 175/169.
// Available payload bytes from header-excluded size 160/167.
// NOT kept: the derived budget drops the retail size update and the last-offset forms change the caller.
// Round 666. The channel still needs EBP from entry.
// Next cursor at admission 172, with early byte debit 171, at both validated prefixes 159,
// with debit and prefix advance 160. One count read for the clamp 139, with admission cursor
// 173, or with prefix advance 159. Immutable current packet and admission cursor 172.
// The prefix-advance forms reach 488 bytes with a larger 0x14 frame.
// M3 lane 4 r246 (H2M_FAST with 0x564c0, MATCH each time; control 139): on the packet-cursor form (re-measured 167, 478 B; every
// register role but EBP is retail's: cursor EDI, members BX, stride ESI split around the submit call, the member in EBP where
// retail keeps the channel): `long consumed = 2; size -= consumed;` then the cursor, then the stride 164 (478 B); then the stride,
// then the cursor 162 (479 B); on that, the route declared at function scope 162, the member declared at function scope 162,
// the members word read into a dword 188 (460 B). Reading: retail's zero-trip exit returns ECX, the register holding the
// constant 2 that also feeds `size -= 2` and the count's store, so that exit returns the constant while the loop exit reloads
// the count from its slot. Body unchanged.
// M3 lane 5 r235 (h2m fast with 0x564c0, MATCH each time; control 139): the plain arm calling the out-of-line talker test
// 181 (450 B: not expanded there); the relay arm through the inline talker test 138 (same code: only the call's target
// symbol differs, the out-of-line copy is identical); `long consumed = 2; size -= consumed;` 139 (same code); the packet size
// as `11 + 2 * relay` 178 (469 B); the member bit test as `(1 << member) & members` 139 (same code). Body unchanged.
// M3 lane 14 r148 (fast with 0x564c0, matched each time; control 139, the packet-cursor form re-measured 164 at 478 B,
// the member in EBP, the channel in no register): on the cursor form, the remaining mask `members & ~(1 << member)` formed
// before the talker calls and tested `!= members` 168 (460 B); the member bound through a named `long const &` 164 (same
// code); the members word bound through `word &` 164 (same code); a separate `remaining = size - 2` local in place of
// debiting the size parameter 159 (479 B); the cursor started as `(byte *)((word *)data + 1)` 164 (478 B).
// M3 lane 14 r149 (H2M_FAST with 0x564c0, MATCH each time; control 139; the packet-cursor form re-measured 167 at 478 B,
// the member in EBP): on the cursor form, the arms in the other order (`if (!relay)` first) 156 (488 B, data in EDI from
// entry, the channel still in no register); the member declared bare and assigned next 167; the members word read ahead
// of the member call 167; the size test as `packet_size > size` 167 (the 167s the same code). On main's form the tail's
// consumed update ahead of the size debit 139 (same code). Body unchanged.
// M3 lane 15 r141 (fast screen with 0x564c0, MATCH each time; control 139). The allocator's ranking, read block by block
// on the packet-cursor form (`long consumed = 2; size -= consumed;`, the stride, `byte *packet = data + 2;`, re-measured
// 162 at 479 B; stride first 164, size first 167): the member scores 20 (its definition and NONE test +16, the bit test
// and the clear +12 each, -20 for the four blocks it lives through unreferenced) and the channel 14 (+104 for its seven
// uses, -90 for the 19 blocks from the entry on where it is live and unreferenced); the member wins ties, so the
// channel needs 7 more. One more candidate referenced in either delivery block (the 0x56df0 and submit calls, two
// channel uses each) is worth +8 to the channel and -2 to the member in the relay arm, which is why an extra channel
// read after a submit call flips it and reads elsewhere do not. Main's indexed form has the channel at 42 and the
// member at 10, but data (102) and consumed (127) take both registers. Forms on the cursor form: the two delivery
// calls through a file-local inline taking the channel, the kind (long 191, byte 192, a pointer to it 192) and the
// payload, or the packet and the payload's offset 183 (457 to 461 B: the index is then read once per arm and the
// channel falls to -10 against the member's 24); the guarded two-return loop leaves both scores as they are (14 / 20);
// the plain arm first (`if (!relay)`) 156 at retail's 488 B: the channel then outranks the member (30 against 18), but
// a cursor-relative temporary spanning the loop (53) takes EBP ahead of both. Body unchanged.
// Channel and packet ownership still differ at entry and the current member and route use different homes.
// Typed relay, direct, both and size-derived wire entries with indexed packets: 139 each.
// The same four with a walking cursor: 164 each. NOT kept: indexed forms fold to the opening
// code, and walking forms shorten to 478 bytes and keep EBP away from the channel.
// The channel still loses EBP in the tied forms. Forwarding-header storage with field or aggregate init
// 139/139, separate forwarding header from fields or whole copy 159/146. Header declared before the loop
// with the same four producers 139/154/159/146. NOT kept: separate headers add stores and enlarge the frame
// or body, and outer value initialization shortens the body without restoring EBP.
// Channel index field reference or pointer, bound per iteration or per delivery arm: 139 each indexed,
// 164 each walking. The indexed forms keep data in EBP. The walking forms give EBP to the member and shorten
// the body to 478 bytes. Original 488-byte body restored.
// The channel still lacks EBP at entry and the member/cursor homes differ. NOT kept: moving both checks
// before dispatch retains the 0x10 frame but shortens the body, and early cursor advancement increases
// the frame. member gate then shared kind test 172; member gate then shared kind test with walking
// packet 164; member selection then shared kind test 187; member selection then shared kind test with
// walking packet 181; combined member and kind validation 172; combined member and kind validation with
// walking packet 164; separate relay and direct validation phase 153; separate relay and direct
// validation phase with walking packet 170.
// M3 lane 6 r244 (h2m fast, 0x564c0 MATCH each time): on the packet-cursor form (164) the allocator's first pass
// ranks the member local 20 above the channel parameter 14 (the channel loses about 54 in the entry blocks where it
// is live but unread), so the member takes EBP. Cursor form plus one `payload = packet + 3` pointer for the submit
// and add-packet calls 164 (folded); the member read through a `long const &` 164 on the cursor form, 139 on this
// body (same code each); size debited at the loop top right after its check 169 on the cursor form, 159 here.
// Channel EBP and the walking packet still disagree. Combined delivery predicate with a separate masked route
// 154/164, named recipient bit with a separate route 171/179, removed local intersection 154/164, and named bit
// with in-place route filtering 178/179 (indexed/walking). NOT kept: the masks raise the score and the
// walking forms shorten the function.
// Channel still lacks EBP, and member and route homes differ. Refresh the bound at the loop tail,
// long/byte, seeded by the live header 139/139 or validated clamp 156/156. With a walking packet
// 164/164 and 163/163. NOT kept: indexed forms tie or regress, and the walking forms shorten the body.
// Channel EBP and the member spill still differ. Budget in the loop test or as a body break,
// with malformed packets returning immediately or sharing the ordinal-based exit:
// 167 in all eight indexed and walking forms. NOT kept: each folds to the same indexed
// walk and shortens the body to 477 bytes.
// M3 lane GA r11 (fast with 0x564c0/0x54810/0x55b40/0x54a20/0x565c0/0x56df0/0x53890/0x53720 and 0x57360, all MATCH
// across 22 trials; control 139 at 488/488 B, mk=41; body unchanged): plain arm using `voice_available() &&
// voice_xhv_has_remote_talker` directly (indexed 150, cursor 170 at 477 B, nested `if` 170) or inline clamp (indexed
// 156, cursor 160, both 156) lets the ternary clobber ECX and naturally emits retail's immediate `add edi, 2`, though
// retail's `mov eax, [ebp+0xc]` ahead of `je` confirms the plain arm really inlines `voice_has_remote_talker`;
// `0x55c40`'s `packet_size = 11; packet_size += 2 * (relay != 0);` (indexed 139, cursor 164, guarded two-return 160 at
// 490 B, mk=30); `if (relay) packet_size += 2` cursor 158 (494 B); guarded two-return + direct avail 157 (488 B) or
// + `consumed = 2; size -= consumed; packet_size = ...; packet = data + 2;` 160 (490 B, mk=30); `memset(&route)` cursor
// 170; step-in-arms + direct avail 165 (494 B: spills member, stride takes EBP); `packet = data; packet += 2;` 164;
// plain-first + direct avail 156 (488 B: spills member, `lea ebp, [edi+3]` steals EBP); rewriting
// `voice_has_remote_talker_inlined` with `&&` / ternary / early-return `if` ties 139 indexed and 162 cursor (guarded +
// `&&` leaves it out of line, 187 at 456 B, mk=27, spilling member while stride takes EBP). Channel EBP vs member EBP
// remains unchanged.
// @retail 0x55e10
long __stdcall function_55e10(s_voice_channel *channel, byte *data, long size, bool relay)
{
 if (voice_packet_clamp_count(data[1], 1, 16) != data[1])
  return NONE;
 long packet_size = relay ? 13 : 11;
 size -= 2;
 long consumed = 2;
 for (long i = 0; i < data[1]; i++)
 {
  byte *packet = data + consumed;
  if (size < packet_size) return NONE;
  if (relay)
  {
   long member = voice_get_current_member();
   if (member == NONE || packet[0] >= 2) return NONE;
   word members = *(word *)(packet + 1);
   if (!members) return NONE;
   if (members & (1 << member))
   {
    if (voice_has_remote_talker(channel->index))
    {
     function_56df0(&g_527104.settings, channel->index, packet[0]);
     voice_submit_incoming_packet(channel->index, packet + 3, 10);
    }
    members &= ~(1 << member);
   }
   if (members)
   {
    s_voice_route route;
    route.members = 0;
    route.unknown02 = 0;
    function_565c0(&g_527104, members, &route);
    voice_channel_add_packet(channel, (const s_voice_packet_header *)&route,
     packet + 3, 10, (const bool *)packet);
   }
  }
  else
  {
   if (packet[0] >= 2) return NONE;
   if (voice_has_remote_talker_inlined(channel->index))
   {
    function_56df0(&g_527104.settings, channel->index, packet[0]);
    voice_submit_incoming_packet(channel->index, packet + 1, 10);
   }
  }
  size -= packet_size;
  consumed += packet_size;
 }
 return consumed;
}

// @retail 0x564c0
void function_564c0(s_voice_channels *channels, byte *data, long size)
{
 if (channels->initialized)
 {
  bool relay = (data[0] & 1) != 0;
  size -= 2;
  if (relay && !voice_current_member_is_unknown00()) return;
  if (voice_packet_clamp_count(data[1], 1, 15) != data[1]) return;
  byte *packet = data + 2;
  for (long i = 0; i < data[1]; i++)
  {
   if (voice_packet_clamp_count(packet[0], 0, 15) != packet[0]) return;
   if (voice_packet_clamp_count(packet[1], 1, 15) == packet[1])
   {
    long consumed = function_55e10(&channels->channels[packet[0]], packet, size, relay);
    if (consumed <= 0) return;
    size -= consumed;
    packet += consumed;
   }
  }
 }
}

// @retail 0x54810
void __stdcall function_054810(void const *data, long size)
{
 if (voice_is_enabled())
  function_564c0(&g_525a00, (byte *)data, size);
}
