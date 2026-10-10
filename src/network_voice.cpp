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
		if (SUCCEEDED(XHVEngineCreate(&parameters, engine)))
		{
			HRESULT error;
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
void voice_xhv_play_voice_mail(c_voice_xhv *xhv, DWORD port, const long *data, long size, bool force)
{
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
void voice_xhv_record_voice_mail(c_voice_xhv *xhv, DWORD port, DWORD maximum_time, BYTE *buffer, DWORD buffer_size, DWORD *size, DWORD *duration)
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

// @retail 0x53420
void voice_initialize_menu_pool(void)
{
	if (g_4c9878.type == 1)
	{
		g_4c9878.use_pool2 = true;
		g_4c9878.pool2 = function_18e1f0(g_46dd54, 0x61800, "voice y menu pool");
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
			voice_xhv_play_voice_mail(&g_476fc8, port, data, size, force);
	}
}

// @retail 0x539a0
void voice_record_voice_mail(long port, DWORD maximum_time, DWORD buffer_size, BYTE *buffer, DWORD *size, DWORD *duration)
{
	if (voice_available())
		voice_xhv_record_voice_mail(&g_476fc8, port, maximum_time, buffer, buffer_size, size, duration);
}

/* ---- the players of the voice session ---- */

static inline long voice_get_unknown00(void)
{
	long result = 0;
	if (voice_available())
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
					return i;
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

// @retail 0x56220
long voice_channels_get_bandwidth(s_voice_channels *channels, long player)
{
	long total = 0;
	if (channels->initialized && voice_channels_have_player(channels, player))
	{
		long local_0 = voice_get_unknown00();
		bool is_unknown00 = local_0 == player;
		total = 2;
		long i = 0;
		do
		{
			s_voice_channel *channel = &channels->channels[i];
			if (voice_channel_has_player(channel, player))
				total += channel->values[player] * (is_unknown00 ? 13 : 11) + 2;
			i++;
		} while (i < 16);
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

// @retail 0x55c40
long function_55c40(s_voice_queue *queue, long player, byte *cursor, byte *output, long capacity, bool include_mask)
{
	long result = 0;
	if (*(bool *)&queue->unknown00)
	{
		dword player_bit = 1 << player;
		if (!(*(word *)queue->unknown08 & player_bit))
			return NONE;
		long clamped = player < 0 ? 0 : player > 15 ? 15 : player;
		if (clamped != player)
			return NONE;
		dword kept = (1 << queue->count) - 1;
		long written = 0;
		long entry_size = include_mask ? 13 : 11;
		capacity -= 2;
		if (capacity >= entry_size)
		{
			for (long i = 0; i < queue->count; i++)
			{
				byte *entry = (byte *)&queue->entries[i];
				if (*(word *)(entry + 6) & player_bit)
				{
					if (capacity < entry_size)
						break;
					*cursor = entry[4];
					if (include_mask)
					{
						*(word *)(cursor + 1) = *(word *)(entry + 2);
						if (!*(word *)(cursor + 1))
							*(word *)(cursor + 1) = (word)(1 << player);
						cursor += 3;
					}
					else
						cursor++;
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
			output[1] = (byte)written;
			output[0] = queue->unknown08[4];
			result = written * entry_size + 2;
			if ((word)kept != (1 << queue->count) - 1)
				voice_queue_compact(queue, (word)kept);
			*(word *)queue->unknown08 = 0;
			for (long i = 0; i < queue->count; i++)
				*(word *)queue->unknown08 |= *(word *)((byte *)&queue->entries[i] + 6);
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
		long current_player = 0;
		if (voice_available())
			current_player = g_4c9878.unknown00;
		bool include_mask = current_player == player;
		capacity -= 2;
		if (include_mask) output[0] |= 1;
		else output[0] &= ~1;
		byte *cursor = output + 2;
		byte *header = cursor;
		for (long i = 0; i < 16; i++)
		{
			s_voice_channel *channel = &channels->channels[i];
			if (channel->active && (channel->player_mask & (1 << player)) && capacity > 0)
			{
				cursor += 2;
				long written = function_55c40((s_voice_queue *)channel, player, cursor, header, capacity, include_mask);
				if (written <= 0)
					break;
				count++;
				cursor += written - 2;
				capacity -= written;
				header = cursor;
			}
		}
		if (count > 0)
		{
			output[1] = (byte)count;
			return original_capacity - capacity;
		}
		output[1] = 0;
		return 0;
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

// @retail 0x55990
void function_55990(s_voice_player_values *values)
{
	if (values->enabled)
	{
		dword players = 0;
		if (voice_available())
		{
			byte *membership = (byte *)voice_get_membership();
			if (membership)
				players = *(dword *)(membership + 0x10d0);
		}
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
				if (voice_available())
					muted = g_4c9878.unknownF0[i];
				dword mask = remote & ~muted;
				if (voice_available())
				{
					if (g_4c9878.mode == 1)
						mask &= voice_player_settings_get_active_value(&g_527104.settings, i);
					else if (g_4c9878.mode == 2 || g_4c9878.mode == 3)
						mask &= function_56c60(&g_527104.settings, i);
				}
				for (long j = 0; j < 16; j++)
				{
					dword target = 1 << j;
					if ((mask & target) &&
						(voice_test_unknownF0(j, i) || voice_port_flag1(j) || voice_port_flag2(j)))
						mask &= ~target;
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

/* src/unknown_138800_2.cpp, src/unknown_067e10.cpp */
bool function_138800();
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
		if (!function_138800() || !function_68250())
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
		if (function_138800() && function_68250())
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
	if (voice_available() && g_4c9878.mode == 3)
	{
		s_network_session_player *players = voice_get_players_inlined();
		dword mask = voice_get_player_mask_inlined();
		if ((mask & (1 << other)) && (mask & (1 << player)))
		{
			long other_team = (char)players[other].propertiesa8[0x7c];
			long team = (char)players[player].propertiesa8[0x7c];
			if (other_team != NONE && team != NONE && other_team == team)
				return true;
		}
		return false;
	}
	return true;
}

// @retail 0x56c60
dword function_56c60(s_voice_player_settings *settings, long player)
{
	dword result = 0;
	if (settings->initialized && voice_available())
	{
		if (g_4c9878.mode == 2 || g_4c9878.mode == 3)
		{
			long mode = g_4c9878.mode;
			dword mask = 0;
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
			long unit = *(long *)(datum + 0x2c);
			if (unit == NONE)
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
						result = (real)sqrt(z * z + x * x + y * y);
					}
				}
			}
		}
	}
	return result;
}

#include "unknown_0662e0.h"

// @retail 0x53e30
void function_53e30(void)
{
	long selected = NONE;
	long best = 0;
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
					long count = *(long *)(membership + 8);
					for (long i = 0; i < count; i++)
					{
						byte *member = membership + 0xc + i * 0x10c;
						long quality = *(long *)(member + 0x94);
						if (i != current && *(long *)(member + 0x98) != 3 &&
							*(long *)(member + 0x90) >= g_network_configuration.valuee4 &&
							quality >= g_network_configuration.valuee0 &&
							*(dword *)(member + 0x9c) == (1U << count) - 1 && quality > best)
						{
							selected = i;
							best = quality;
						}
					}
					if (selected == NONE && !voice_mode_is_1())
						selected = current;
					if (session->get_value_5e20() != selected)
						session->set_value_5e20(selected);
					g_4c9878.unknown00 = selected;
				}
				else
					g_4c9878.unknown00 = session->get_value_5e20();
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

static __forceinline bool voice_test_unknown110(long player, long other)
{
	bool result = false;
	if (voice_available())
		result = (g_4c9878.unknown110[player] & (1 << other)) != 0;
	return result;
}

// @retail 0x57d60
long function_57d60(long mode, long player, long other)
{
	long result = 0;
	if (!voice_test_unknown110(player, other) && !voice_test_unknown110(other, player))
	{
		switch (mode)
		{
		case 0: { result = 0; goto done; }
		case 1: result = (voice_get_unknownEE() & (1 << player)) ? 1 : 2; break;
		case 2: result = 2; break;
		case 3: result = 3; break;
		default: return result;
		}
		long controller = voice_get_player_unknown14(player);
		if (controller == NONE)
			{ result = 0; goto done; }
		switch (voice_get_port_state(controller))
		{
		case 1: if (result != 3) result = 2; break;
		case 2: { result = 1; goto done; }
		case 3: { result = 0; goto done; }
		}
		if (result == 2 || result == 3)
		{
			dword local_mask = voice_get_local_player_mask();
			for (long local = 0; local < 16; local++)
				if ((local_mask & (1 << local)) && voice_test_unknown110(local, other))
					{ result = 0; goto done; }
		}
	}
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
						bool allowed;
						if (!function_53d40())
							allowed = true;
						else if (function_53d90())
							allowed = !juggernaut_is((short)local) && !juggernaut_is((short)other);
						else
						{
							long team = (char)players[local].propertiesa8[0x7c];
							long other_team = (char)players[other].propertiesa8[0x7c];
							allowed = team != NONE && other_team != NONE && team == other_team;
						}
						if (allowed)
							settings->unknown84[local] |= 1 << other;
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
	dword talker_bit = 1 << talker;
	for (long player = 0; player < 16; player++)
	{
		long port = voice_get_player_unknown14(player);
		if (players & (1 << player))
		{
			settings->unknown108[player] |= talker_bit;
			if (port != NONE)
				ports |= 1 << port;
		}
		else
			settings->unknown108[player] &= ~talker_bit;
	}
	for (long port = 0; port != NONE; port = voice_port_next_index(port))
	{
		if (ports & (1 << port))
		{
			DSMIXBINVOLUMEPAIR pair;
			memset(&pair, 0, sizeof(pair));
			DSMIXBINS bins;
			memset(&bins, 0, sizeof(bins));
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
				function_caf90(object, &position);
				for (long j = 0; j < 16; j++)
				{
					dword bit = 1 << j;
					if ((players & bit) && !(local & bit) && function_54df0(j))
					{
						long other = voice_proximity_object(j);
						if (other != NONE)
						{
							point3f other_position;
							function_caf90(other, &other_position);
							real z = other_position.z - position.z;
							real x = other_position.x - position.x;
							real y = other_position.y - position.y;
							real squared = z * z + x * x + y * y;
							real range = 8.0f;
							if (g_510c94)
								range = g_510c94->motion_sensor_range;
							if (range + 1.5f >= sqrtf(squared))
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
void function_56790(long *capacity, dword *remaining, dword allowed, word *selected,
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
 byte *const *local_0 = &output;
 long result = 0;
 if (voice_is_enabled())
  result = function_56380(*local_0, &g_525a00, player, capacity);
 return result;
}

// Disabled pending a source-shape fix: enabling this removes a required
// caller-local initialization in the matched 0x54890.
#if 0
// Retail address: 0x565c0 (disabled).
void __stdcall function_565c0(s_voice_routing *routing, dword remaining, s_voice_route *output)
{
 output->unknown02 = 0;
 output->members = 0;
 if (!routing->enabled || !remaining)
  return;
 long capacity = voice_get_mode_value();
 if (capacity <= 0)
  return;
 bool preserve = voice_unknown00_valid();
 long preferred = 0;
 if (voice_available())
  preferred = g_4c9878.unknown00;
 long mode = 0;
 if (g_4c9878.initialized)
  mode = g_4c9878.session_kind;
 dword eligible = voice_get_current_member_value();
 dword preferred_bit = 1 << preferred;
 word selected = 0;
 word deferred = 0;
 bool blocked = false;
 if (!(eligible & preferred_bit))
  preserve = false;
 if (!(eligible & preferred_bit) || !preserve)
 {
  if (mode == 2)
   capacity = 1;
 }
 else if (mode == 1 || mode == 2)
 {
  blocked = true;
  goto finish;
 }
 {
  long kind = voice_get_session_kind();
  deferred = (word)(remaining & ~eligible);
  remaining &= eligible;
  if (deferred)
   blocked = true;
  if (kind == 2)
   function_56790(&capacity, &remaining, function_53c70(), &selected, preserve, preferred, &blocked);
  if (capacity != 1 || !preserve || !blocked)
   function_56790(&capacity, &remaining, (dword)-1, &selected, preserve, preferred, &blocked);
 }
finish:
 if (preserve)
 {
  if (blocked)
  {
   selected |= (word)preferred_bit;
   deferred |= (word)remaining;
  }
  else if (remaining & preferred_bit)
  {
   selected |= (word)preferred_bit;
   deferred |= (word)preferred_bit;
  }
 }
 else
  deferred = 0;
 output->members = selected;
 output->unknown02 = deferred;
}

#endif

// @retail 0x54590
void function_54590(void)
{
 s_network_session_player *players = voice_get_players_inlined();
 dword mask = voice_get_player_mask_inlined();
 char reason[0x80];
 reason[0] = 0;
 dword empty[3] = {0, 0, 0};
 if ((dword)g_4c9878.unknown20 != mask)
 {
  strncpy(reason, "session player mask change", sizeof(reason));
  reason[sizeof(reason) - 1] = 0;
 }
 else
 {
  long i;
  for (i = 0; i < 16; i++)
  {
   const void *identity = mask & (1 << i) ? (const void *)&players[i] : empty;
   if (memcmp(&g_4c9878.unknown24[i * 3], identity, sizeof(empty)))
    break;
  }
  if (i == 16)
   return;
  voice_sprintf(reason, "player identifier change");
 }
 g_4c9878.unknown20 = mask;
 for (long i = 0; i < 16; i++)
 {
  const void *identity = mask & (1 << i) ? (const void *)&players[i] : empty;
  memcpy(&g_4c9878.unknown24[i * 3], identity, sizeof(empty));
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
   long count = g_4c99c0.user_counts[port];
   if (count != NONE)
   {
    XONLINE_MUTELISTUSER *users = g_4c99c0.users[port];
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
  dword players = voice_get_player_mask();
  for (long i = 0; i < 16; i++)
  {
   if (players & (1 << i))
   {
    if (local & (1 << i))
    {
     if (voice_has_remote_talker(i) &&
      g_476fc8.initialized && (g_476fc8.unknown0c & 4))
      g_476fc8.engine->UnregisterRemoteTalker(voice_xuid(i));
     function_541a0(i);
    }
    else
     voice_update_remote_talker(i);
   }
   else
   {
    if (voice_has_remote_talker(i) &&
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
void function_58590(long mode, long talker, c_voice_observer *observer, long player)
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
				DSMIXBINVOLUMEPAIR pairs[4] = {0};
				DSMIXBINS bins = {0};
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
		long player = choices[i].player;
		long mode = choices[i].mode;
		if (mode == 1)
			headset |= 1 << player;
		else
			headset &= ~(1 << player);
		if (mode == 2 || mode == 3)
		{
			settings->unknownc8[player] |= 1 << talker;
			if (choices[i].mode == 2)
				radio = true;
			else if (!radio && choices[i].mode == 3 &&
				(nearest == NONE || choices[nearest].distance > choices[i].distance))
				nearest = i;
		}
		else
			settings->unknownc8[player] &= ~(1 << talker);
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
		function_58590(3, talker, (c_voice_observer *)settings, choices[nearest].player);
	else if (g_476fc8.initialized)
		g_476fc8.engine->SetPlaybackPriority(voice_xuid(talker), 4, (XHV_PLAYBACK_PRIORITY)-1);
}

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
			for (long player = 0; player < 16; player++)
			{
				long mode = 0;
				if (local & (1 << player))
				{
					mode = voice_players_share_team(player, talker);
					mode = function_57d60(mode, player, talker);
					if (mode) settings->unknownc4 |= bit;
				}
				choices[player].player = player;
				choices[player].mode = mode;
			}
			function_57eb0(settings, talker, choices, 16);
		}
	}
}

// @retail 0x56df0
void __stdcall function_56df0(s_voice_player_settings *settings, long talker, long mode)
{
	// The retail entry reads the talker from its stack argument slot.
	long const *talker_reference = &talker;
	bool active = false;
	if (settings->initialized && voice_available())
		active = g_4c9878.mode == 1;
	if (active && voice_xhv_has_remote_talker(&g_476fc8, *talker_reference))
	{
		dword local = voice_get_local_player_mask();
		bool valid = function_54df0(talker);
		s_voice_route_choice choices[16] = {0};
		for (long player = 0; player < 16; player++)
		{
			long route = 0;
			if ((local & (1 << player)) && function_54df0(player) && valid)
			{
				long requested = function_57c20(settings, player, talker, mode);
				route = function_57d60(requested, player, talker);
				if (requested == 1) settings->unknown148[talker] = g_510c54->game_time;
				if (route == 3) choices[player].distance = function_57a80(player, talker);
				if (route) settings->unknownc4 |= 1 << talker;
			}
			choices[player].player = player;
			choices[player].mode = route;
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
		bool active = false;
		if (g_4c9878.initialized && g_476fc8.initialized)
			active = g_4c9878.mode == 1;
		if (active)
		{
			function_56f60((c_voice_observer *)settings);
			function_57080((s_voice_player_values *)settings);
			function_57360((c_voice_observer *)settings);
			function_577d0(settings);
		}
		else
		{
			bool passive = false;
			if (g_4c9878.initialized && g_476fc8.initialized)
				passive = g_4c9878.mode == 2 || g_4c9878.mode == 3;
			if (passive) function_57960(settings);
		}
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

// @retail 0x55e10
long __stdcall function_55e10(s_voice_channel *channel, byte *data, long size, bool relay)
{
 if (voice_packet_clamp_count(data[1], 1, 16) != data[1])
  return NONE;
 byte *packet = data + 2;
 size -= 2;
 long consumed = 2;
 long packet_size = relay ? 13 : 11;
 for (long i = 0; i < data[1]; i++)
 {
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
   if (voice_available() && voice_xhv_has_remote_talker(&g_476fc8, channel->index))
   {
    function_56df0(&g_527104.settings, channel->index, packet[0]);
    voice_submit_incoming_packet(channel->index, packet + 1, 10);
   }
  }
  size -= packet_size;
  consumed += packet_size;
  packet += packet_size;
 }
 return consumed;
}

// @retail 0x564c0
void function_564c0(s_voice_channels *channels, byte *data, long size)
{
 if (channels->initialized)
 {
  byte *packet = data + 2;
  bool relay = (data[0] & 1) != 0;
  size -= 2;
  if (relay && !voice_current_member_is_unknown00()) return;
  if (voice_packet_clamp_count(data[1], 1, 15) != data[1]) return;
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
