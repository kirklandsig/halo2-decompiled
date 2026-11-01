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

// Round 19: unchanged; retail loads the engine address after the guard branch and ours loads it
// before it. Binding the address inside the guarded block lets MSVC sink `push ebp` into that
// block and re-encode the stack slots: null-initialised, redeclared, the result pointer moved
// into the block and the address taken at the call all score 8.
// score 2 (was 2): the engine pointer is loaded into ebp before the branch that
// skips the creation; retail loads it after the branch. Direct creation with
// the pointer bound after the call or in its argument 8; initialized error 2;
// error declared at creation 2; const engine pointer 6; plain result store 36;
// early false return 92; error before engine 2.
// skips the creation; retail loads it after the branch. With the load after the
// branch ebp is pushed there instead of at entry, and without the volatile store
// the true result is folded into separate epilogues. Natural nested ifs 42;
// E_FAIL-initialised status tested after the guarded creation 39, with the field
// address in the creation call 42, with the stored result 26; plain store
// through the result pointer 36.
// @retail 0x55810
bool voice_xhv_create(c_voice_xhv *xhv)
{
    XHV_RUNTIME_PARAMS parameters;
    XHVEngine **engine;
    HRESULT error;
    bool result = voice_xhv_get_runtime_parameters(&parameters, xhv);
    engine = &xhv->engine;
    bool *local_0 = &result;
    if (!result)
        goto local_1;
    error = XHVEngineCreate(&parameters, engine);
    if (!SUCCEEDED(error))
        goto local_1;
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
    if (!SUCCEEDED(error))
        goto local_1;
    error = (*engine)->SetCallbackInterface(xhv);
    if (xhv->unknown0c & 4)
        (*engine)->SetMaxPlaybackStreamsCount(15);
    if (!SUCCEEDED(error))
        goto local_1;
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
    *(volatile bool *)local_0 = true;
local_1:
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

// score 94 (was 94): the flag stays in bl (retail al), so the loop temporaries take eax/al where retail uses
// ebx/cl; retail loads the count into ebp before the multiplier and adds 2 before the total (ours folds the load
// into imul and uses one lea). Frame, prologue, unrolling and cursor agree (347 bytes).
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

// score 134 (was 134): the retained mask is a plain local and the pre-loop statements run capacity,
// size, written, mask: the bit (EBP), size (EBX), count (ECX), the frame order i/written/kept/size/bit
// and the single mask read at the end now agree with retail. Still differs: EAX's zero is reused for
// `written = 0` and the PIN test (retail `test edi, edi`, immediate store), so the capacity-fail exit
// gets its own `xor eax, eax` epilogue and the code is 460 bytes against 449.
// Round 19: unchanged; retail keeps the player bit in EBP and the queue count in ECX, ours
// caches the count in EBP and moves the bit to EBX; no new form was found.
// Earlier, at 141: the convention now agrees with retail (queue ESI, player EAX copied to
// EDI, cursor EDX before the player as the declared order, the rest on the stack, `ret 0xc`): the
// result is computed before the header stores, so EDI holds it after the loop. Still differs:
// retail keeps the player bit in EBP and the queue count in ECX (ours caches the count in EBP and
// moves the bit to EBX), tests include_mask before the first cursor store, reads the retained
// mask once at the end (ours reads the volatile twice) and orders the frame i/written/kept/size/
// bit. Before: five coordinated register-input orders 134; shared invalid exit 169; entry size
// as unsigned word 188 or byte 170; staged length 134; consumed-capacity length 188;
// accumulated length 201; word/signed retained mask 134/134; unsigned written/result
// 134/134; explicit retained intersection 134; real entry/header/both field views 134;
// direct valid range 153; remaining-budget local 171; result length before header with
// raw/entry/all fields 134; entry/packet mode observations 134/134; entry size before
// written count 107; shared scan ordinal 134; entry cursor / with mode 133/133;
// capacity result join 134; written count after header reservation 160; written before
// mask/size 133; mask after size/count 173; retail initialization order 162; completed
// length after compaction 172; signed-word written count 141; recipient bit at use 107;
// entry-size stride 164; shared header stride 184; serialized-span length 144; invalid
// result first/with stride 180/182; early result on size-first frame 141; serialized span
// with stride 199; result before the header stores 141 (kept with 0x56380's matching form);
// with written declared before the size 151/134; with a plain mask 171; bit after the tests
// 141; size before mask 180; early NONE returns 162; the player reassigned to the clamped
// value (diagnostic) arrives in ecx, 183. Round 67 (with the kept form): mask not volatile
// 171; written/capacity/size in retail's instruction order 151; mask, capacity, size,
// written 142; the 24 orders of those four statements and the product operand order,
// written increments, entry size as if/else, a function-level counter, an entry pointer walk
// and the bit declared later screened in a one-file LTCG harness: none keeps both the count
// in ECX and the product in EDI (written declared before the size keeps the count but puts the
// product in EBX and the player back in EDI).
// Round 198: one final mask snapshot dword/word after header 142/142, before result 141/141,
// after result 141/141; snapshot with written before size 168/168.
// Round 98 (lane 8; LTCG env link of this file, 0x56380 built in the same link; env rows
// positional/aligned, volatile base 128/54): the 24 orders of mask/size/written/capacity with a
// volatile or plain mask, named or inline bit, single exit or direct returns: with a plain mask,
// every order that computes the mask before the size passes the player on the stack (0x56380 lost);
// capacity/size/written/mask 122/27 (kept, check 134), size/capacity/written/mask and
// size/written/capacity/mask 125/33; direct returns 154+. On the kept order: the first cursor byte
// stored in each include_mask arm 151/58 (475 bytes; retail's test-before-store order); the mask
// declared inside the capacity test 127/49 (449 bytes, bit in memory); both 127/45; written declared
// there 157/77; capacity test as an early `goto done` 122/27; long/dword types 122/27;
// `entry_size <= capacity` 122/29; bit-first loop test 122/27; mask and written before the
// has-player test 150/82; has-player in a named bool 122/27; written at function scope 141/134
// (0x56380 lost); `2 + written * entry_size` 122/27; a direct `return NONE` 162/242; the inactive
// queue as an early `goto done` 122/27; one `||` failure test 139/40.
// Round 20: the zero in EAX is a shared constant for the result, the PIN test and `written = 0`;
// with a non-constant result start (diagnostic) the PIN test becomes `test edi, edi` and written an
// immediate store (205, convention moves). Inactive queue as an early `return 0` 191 (0x56380 73);
// written declared inside the capacity block 197; nested else arms instead of the gotos 134 (same
// code); first cursor byte stored in each include_mask arm with `*cursor++` in the else 178 (475
// bytes); the player bit written at its use 134 (same code); clamp test removed (diagnostic) 171
// with `ret 0x10`.
// Upper-first ternary, sequential bounds, lower/upper-first conditional clamped-result construction:
// 179/134/134/140; same four with exhaustive result production 201/177/177/177. Zero sharing and pre-loop
// scheduling remain; original source restored.
// @retail 0x55c40
long function_55c40(s_voice_queue *queue, byte *cursor, long player, byte *output, long capacity, bool include_mask)
{
	long result = 0;
	if (*(bool *)&queue->unknown00)
	{
		dword player_bit = 1 << player;
		if (!voice_channel_has_player((s_voice_channel const *)queue, player))
		{
			result = NONE;
			goto done;
		}
		long clamped = player < 0 ? 0 : player > 15 ? 15 : player;
		if (clamped != player)
		{
			result = NONE;
			goto done;
		}
		capacity -= 2;
		long entry_size = include_mask ? 13 : 11;
		long written = 0;
		dword kept = (1 << queue->count) - 1;
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
done:
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

// score 85 (was 85): the second availability byte loads lazily (retail loads both before the muted test and keeps
// remote in esi) and the second port-flag query is called out of line (retail inlines it here only); frame and bit
// spill agree. Earlier at 94: frame 0x14 vs 0x18, bit spill and flag-query boundary differed;
// word outer/target induction 118/106; word active 97; mask names 94; skip cleared inactive entries 110.
// caller depth 0/1/2 241/94/94; wide bit with depth 1/2 118/118;
// complete mask/policy/both assignments 108/104/110.
// Rejected set 100; rejected set with wide bit 168; indexed rejection bit 99;
// allowed result 107; signed allowed result 107; allowed result with wide bit 130;
// candidate snapshot 97; candidate snapshot with wide bit 119. Frame/spill and flag-query expansion remain.
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
						(function_57d63(j, i) || voice_port_flag1(j) || voice_port_flag2(j)))
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

// score 25 (was 25): mode save boundary differs; the complete peer loop and exits agree.
// Earlier at 25 (was 34): retail tests the mode in eax before saving ebx/esi and copies it
// to esi afterwards; ours loads it straight into esi, so both saves come first. Exits agree.
// Mode/mask saves precede acceptance and cold exits differ; peer loop agrees.
// early mode/mask saves and cold exits remain; 368/368 bytes.
// Earlier 46 (was 55): early mode save and exit layout differ; 377/368 bytes.
// Complete mask arms 46; mask-first output arms 118; result before filter 53; mode guard 118;
// team guard 140; byte teams 76; continues 55; membership first 89; shared bit 103;
// bit addition 59; signed result 55; initial guard 121; conditional/late mask 46/46; direct outputs 141/144.
// Mode with complete mask 46; shared team/all-peer modes 148/148; team acceptance first 46;
// mask before mode 34; equality with other/selected validity 117/118; unsigned peer ordinal 130.
// Passive-mode bool 55; mode re-read in the block 55; unsigned mode 55; switch 121;
// mode test in the entry condition 55; zero-default mode 55; a 600 s permuter run, none below 34;
// mode getter after a plain flag test 25 (kept); with the mask in the block 56; with
// voice_available() 63; getter as the only test 49; getter re-read for the team test 25.
// early mode/mask saves and cold exits remain; 368/368 bytes.
// Earlier 46 (was 55): early mode save and exit layout differ; 377/368 bytes.
// Earlier 34 (was 46): early mode/mask saves and cold exit placement remain; peer loop agrees.
// Accepted/available/filter output arms 124/117/119; filter arms with live mode 119;
// accepted-only mode capture 34 (exact-only); entry players/team/both declarations 34/34/34. Restored.
// Deferred mask initialization 55; complete query arms/selection 46/46;
// deferred live/native mode 55/55; accepted team predicate 127;
// deferred default output 53; deferred initial exit 55.
// Mode/mask saves still precede acceptance; cold exits differ; peer loop agrees.
// Live mode 34; function-scope mask before/after result 54/54; signed mask 34;
// enclosing mask with live mode 54; signed live mask 34; rejection to sole exit 34; enclosing exit 54.
// Accepted-boundary mask zero with signed equalities/range 56/125; unsigned 56/125;
// complete-query mask with signed equalities/range 47/51; unsigned 47/51. Shared getter retained.
// Global test with the mode named inside before/after the mask 56/56 (exact in a one-file link where
// the session lookup may write memory, BLOCKED-BY 0x59670: our link proves it writes only the
// caller's local, so the mode is not split); getter test with the mode named inside 46;
// mode named before the test with the mask inside 56; mask declared after the membership lookup 55.
// Round 136: defining-unit inline membership/players/both 25,25,25;
// with separate noinline 25,25,25; mode force-inline alone/with both outlined queries 25,25.
// Lane 13 r20: team flag taken before the membership call 142; team test as `mode != 2` 25; ternary
// mask in the block 47; `mode == 3 || mode == 2` 25; mode read from the global field 25.
// Lane 12 r66 (fast): retail's EAX test and `mov esi, eax` copy are the global-test form with the
// mode and the mask named inside the block (`if (g.mode == 2 || g.mode == 3) { long mode = g.mode;
// dword mask = 0; ...`) when the 0x53de0 call ends the global's availability: with a compiler barrier
// after those two declarations (diagnostic only) it matches byte for byte; our link proves 0x53de0
// leaves the global alone, so the inner uses fold back into the test's load (56). Tried: early
// returns of the named result (init, then mode) 114; a force-inline helper taking the mode 123 (the
// convention moves to ECX); the getter named inside under a global test 56; the global read at both
// tests with the mask inside 56; the mask declared after the mode 25 (same code). Diagnostic: a write to
// an unrelated global added to 0x53c30 does not end the mode's availability (56): the link tracks
// which globals a callee writes.
// @retail 0x56c60
dword function_56c60(s_voice_player_settings *settings, long player)
{
	dword result = 0;
	if (settings->initialized && g_4c9878.initialized && g_476fc8.initialized)
	{
		dword mask = 0;
		long mode = voice_get_mode();
		if (mode == 2 || mode == 3)
		{
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
// Round 19: unchanged; the mask/source reloads after the logging call stay in our order (edi then
// ebp) against retail's (ebp then edi); the reduced image shows 2 even for the full-build match.
// score 2 (was 2): only mask/source reload order after logging differs; frame and copy graph agree.
// Round 196: query-carrier names singly/paired, comparison-input names singly/paired,
// comparison ordinal and copy ordinal names [2, 2, 2, 2, 2, 2, 2, 2]; original source restored.
// Round 198: read-only mask/source/both 2/2/2; unsigned/signed-word/unsigned-word copy ordinal 123/318/318; unsigned comparison/both ordinals 162/186. Opening body restored.
// Round 107 (plain checks: --fast shows 2 even for a full-build match): this body matched in the full
// build of lane 5 round 82 and network_voice.cpp has not changed since, so the reload order follows the
// rest of the image. Mask declared first, logging call inside the loop, empty declared first, reason
// declared first, `if (changed) log; else return;`: 2 each, same pair swapped.
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

// score 363 (was 363): frame 0x54 and scalar homes agree; the unrolled loop's cursor bias (retail
// &choices[i + 1].mode, here &choices[i].mode), entry registers (retail: talker in EBX, the
// talker bit in EBP, zero in EBP so the initialized tests are `mov al / test al` and the two
// final SetPlaybackPriority calls share one call) and the radio update (retail branches and
// stores 1 into the byte) differ. Round 137: a per-choice pointer local
// (`choice = &choices[i]`) gives retail's +0x10 cursor bias (compile-only and real) 395, and
// with `if (mode == 2) radio = true; else if (!radio && ...)` 410 at 1270 bytes, frame 0x50,
// though its aligned differences drop from 372 to 302: the branch form stops the headset mask
// spilling to its stack home (retail spills it and reloads it in the first unrolled copy);
// the if/else-if radio alone 411; a named talker bit 391, with the if/else-if radio 422; the
// headset update through the existing flags_set_bit inline 363 (compiles the same), with the
// pointer and radio forms 410.
// Earlier: direct mode reads 394; nearest pointer 513; function-scope mixer workspaces 412;
// byte radio 409; complete mixer aggregates 404; direct reads with wider workspace scope 407;
// scalar role names 394; rejected-mode first 394; signed while traversal 394; descriptor before
// pairs 394; existing mixer helper 461; byte radio with direct reads 411; combined headset
// clear/set 519; boolean radio with unsigned nearest index 411; three mixer pairs 394;
// conditional headset assignment 394; split initial/replacement nearest 412; repeated
// player-field reads 472; native Boolean/word radio with choice receiver 410/392; Boolean with
// three pairs 397; plain pair loop 394, with Boolean 411; native descriptor alone 394 (retail
// length), with native choice/Boolean 411; native choice/Boolean mixer-workspace names 410;
// headset rejection first 394, with native Boolean radio 411; selected nearest result 1101,
// with native radio 1103, with both 1103; late mixer descriptor 394, with native radio 411;
// all three with native radio 1103; per-choice pointer 393; signed-word radio 393; signed
// headset 394; function-scope descriptor only 402; function-scope pairs only 401; selected
// route value 459; byte/word radio with route value 491/459; route value with three pairs 469;
// native descriptor 464; word/three pairs 467; byte/descriptor 495; native mixer combination
// 503; nearest-distance 534; candidate-distance 526; nearest-distance-bool 572;
// nearest-distance-word 533; nearest-distance-descriptor 538; candidate-distance-bool 572;
// candidate-distance-word 525; candidate-distance-native-mixer 576; radio predicate
// accumulation, bitwise long with either comparison order 436/436; bitwise Boolean 363/363
// (kept, byte); logical long 1082/1082; logical Boolean 1087/1087; reused outgoing identity at
// branch/engine/entry/after-routing 376/375/425/452; branch identity with three pairs/native
// descriptor 382/376; engine identity with three pairs 381; post-routing identity with three
// pairs and native descriptor 464.
// Earlier findings: selected-distance storage shortens the unrolled body; selected results prevent four-way
// expansion; route-value forms shrink the frame to 0x40; byte-accumulation scalar declarations
// 021/102/120/201/210 and nearest/headset/radio home renames 363 each.
// Round 183, recovered bitwise-radio frame: advancing choice cursor 392; reused talker/player
// masks 384/391; cursor with talker/player 390/409; both masks 379; cursor with both 401;
// one mode observation 405 (callers 57960/56df0 lose to 22/18). Original loop restored;
// frame 0x54 and scalar homes agree, entry saves and unrolled cursor bias remain.
// Complementary radio decision alone/choice receiver/native descriptor/both, with
// four mixer pairs 364/395/374/378; three mixer pairs 375/390/374/386; all restored.
// Round 57: retail sets the radio byte in a branch (`if (mode == 2) radio = true; else if
// (!radio && mode == 3 ...)`): that form gives retail's radio blocks but frame 0x50, the headset
// kept in a register instead of spilled, 411; with each other order of the three scalar
// declarations 410/411; as two separate ifs 411; with a char radio 411. Retail's remainder loop
// indexes `[offset + choices]` with choices in edi (which forces the headset spill); ours walks
// a pointer. Ternary headset bit update 363 (same code), with the radio branch 411.
// Complete current route value/const value/element reference with bitwise radio
// 438/438/395; with conditional radio 459/459/410; native three-pair descriptor
// with current value/reference 431/386. All original observations and workspaces restored.
// Reused radio/spatial/both predicates and both with conditional radio, after headset
// update 430/399/404/466; before headset update 400/479/547/598.
// Shared priority call with a nearest-path return, original/choice receiver/three pairs/
// native descriptor 402/401/419/424; shared priority with a combined routing decision,
// corresponding forms 402/401/419/424. All original routing and engine calls restored.
// Round 379: coordinated formal orders 1023/1203/1230/0213/0132/0312/2013/3012
// 363/363/363/363/364/364/363/364; first three preserve both callers, the other five lose them.
// Original definition and every call restored; entry saves and cursor bias remain.
// Round 381: headset accumulation after routing 423; settings publication after decisions 435;
// both 494; with the actual choice receiver 423/435/494; both with conditional radio 416,
// with the receiver 416. All restored; original byte accumulation and input reads retained.
// Round 109: retail tail-merges the two SetPlaybackPriority calls (engine loaded last in both arms;
// ours loads it early in the radio arm). The loop test as `count > i` 1091 (not unrolled; callers
// 57960/56df0 lose to 22/18); the index declared before the loop 363; the player field read at each
// settings update 477; the NONE-nearest priority arm first 362; the radio flag declared last 363.
// Round 169: Actual mask-array entry/loop/reference and per-element receivers
// 363/363/363/399; each with conditional radio 411/411/411/411. Original routing restored;
// entry saves and unrolled cursor bias remain.
// Round 220: Selected route pointer on byte-bitwise radio graph: original/choice receiver/
// three pairs/descriptor/both/receiver-three/receiver-descriptor/all
// 533/533/541/536/543/541/536/543. Original index, observations and workspaces restored.
// Round 396: shared routed-admission predicate across settings/reduction, at both
// observation boundaries with choice receiver and bitwise/branch radio:
// 1085,1088,1085,1088,1088,1090,1088,1090. All lose both callers and suppress expansion; original restored.
// Round 250: signed range admission, original/conditional headset 367/367;
// with split nearest selection 433/433; complete mode observation 440/440,
// with split nearest 512/512. All restored; entry saves and cursor bias remain.
// Round 237: explicit pair/descriptor/both library clears 387/391/395; complete pair values
// 379; complete descriptor values 373; pair clear with descriptor values 370; pair values
// with descriptor clear 378; complete values for both 374. Original body restored;
// both callers and both exact callee controls survive; entry/cursor homes remain.
// Round 395: signed nearest-index validity 419; with choice receiver 412, branch radio 415, both 414;
// three mixer pairs on those four graphs 428/402/422/421. Original validity tests and full body restored.
// Round 82 (one-file LTCG build, exact): every order of the radio/nearest/headset declarations gives the same code;
// branch radio 411 (headset kept in EBX, never spilled; talker in EDI; frame 0x50); with a per-choice pointer 410,
// with a per-choice reference 410; the pointer alone 395; a named talker bit 391.
// Round 86: a choice cursor declared first and stepped at the end of the body 392 (frame 0x48); a reference to
// the listener mask 399; the headset test inverted 406; the distance comparison reversed 363 and the nearest
// test nested 363 (same code).
// Round 95: after the loops retail holds talker in EBX and a zero in EBP (byte tests then load al and test
// it, the priority-0 push is `push ebp`, and the two SetPlaybackPriority tails merge); ours swaps them (zero
// in EBX gives `cmp [mem], bl` and the separate tails). An extra per-iteration use of choices or talker
// (diagnostic) moves neither. Word headset 364; branch radio with a word headset 408 (1273 bytes, aligned
// differences 220 against 281); the nearest arm first (`if (!radio && nearest != NONE)`) 383, with branch radio
// 416, with a word headset 384.
// Round 446: Unsigned nearest/player field plus ordinal/both, signed headset alone/with nearest/
// with player/all three, and player field alone: all eight 363 on the byte-bitwise graph.
// Unsigned mode field did not compile; original mode, records and full body restored.
// Round 112: a named XUID used by both radio-arm engine calls 375 (1288 bytes); with branch radio 418; with a
// per-choice pointer 385; branch radio, pointer and XUID 417; the same with the NONE-nearest arm first 420
// (aligned best of a 16-form screen, frame 0x50). Entry and tail rotation (talker EBP, zero EBX) unchanged.
// Round 462: Consumed next-ordinal choice pointer/reference before headset, original/reused talker
// mask 1075/1084; after headset 1079/1088; reference forms 1075/1084/1079/1088. All suppress
// four-way expansion; original body restored.
// Round 267: cleared-low-bit routed-mode membership with choice-receiver/branch-radio/mode-observation
// masks 0..7: 385/384/429/456/377/386/418/457. Mode-observation plus branch-radio forms lose both callers;
// original observations, bitwise radio and full body restored; entry/cursor allocation remains.
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
		if (choices[i].mode == 1)
			headset |= 1 << player;
		else
			headset &= ~(1 << player);
		if (choices[i].mode == 2 || choices[i].mode == 3)
		{
			settings->unknownc8[player] |= 1 << talker;
			radio |= choices[i].mode == 2;
			if (!radio && choices[i].mode == 3 &&
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

// score 139 (was 139): channel/cursor/count registers and route homes differ;
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
// Round 64 (lane 9, fast; 0x564c0 matched each time): a packet cursor `data + 2` advanced at the loop's end
// 164 (478 bytes: retail's EDI cursor, the member in EBP); with the member and packet-kind tests split 164;
// the cursor advanced before the size and count updates 167; the member declared, then assigned 164; the
// member bit cleared before the talker test 161 (469 bytes).
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
