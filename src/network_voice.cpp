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

// score 2 (was 2): the engine pointer is loaded into ebp before the branch that
// skips the creation; retail loads it after the branch. Direct creation with
// the pointer bound after the call or in its argument 8; initialized error 2;
// error declared at creation 2; const engine pointer 6; plain result store 36;
// early false return 92; error before engine 2.
// MiniMax h (11-06): no new form, no lower score: the engine address bound after the guard (8), bound as a declaration initializer (8), read from the member at each use (8) and a volatile result (30) all sink the push of ebp into the guarded block, while a reference to the member keeps that push at the entry but loads the address before the branch (6).
// Round 19: unchanged; retail loads the engine address after the guard branch and ours loads it
// before it. Binding the address inside the guarded block lets MSVC sink `push ebp` into that
// block and re-encode the stack slots: null-initialised, redeclared, the result pointer moved
// into the block and the address taken at the call all score 8.
// MiniMax E (11-07): forms tried, none lower (2 unchanged; the engine pointer bound after the guard,
// in an else arm and in a nested block scope all score 8, because MSVC then sinks the save of ebp into
// the guarded block and the frame slots shift; the field address taken in the creation call is 2);
// a reference to the engine member keeps the save of ebp at the entry but loads the address before the
// branch (6), the same reference bound inside the guarded block sinks the save (8), and the engine
// pointer bound before the parameters call loads the address at the top (6); a permuter run reached
// its time limit without a measured variant.
// skips the creation; retail loads it after the branch. With the load after the
// branch ebp is pushed there instead of at entry, and without the volatile store
// the true result is folded into separate epilogues. Natural nested ifs 42;
// E_FAIL-initialised status tested after the guarded creation 39, with the field
// address in the creation call 42, with the stored result 26; plain store
// through the result pointer 36.
// MiniMax D (11-06): no new form, no lower score: binding the engine pointer after the guard still sinks
// the push of ebp into the guarded block, and the permuter run on this function reached its time limit
// without a measured variant.
// Round 78 (lane 10, fast): ours forwards AL to the result on the guard-fail edge (its `je` lands one
// instruction past retail's `mov al, [esp + 0xb]`; with the engine bound after the guard it lands on
// `pop ebx`, two past, and `push ebp` sinks: 8). The natural form (no volatile, goto or result pointer;
// nested SUCCEEDED ifs, `result = true` after the ports loop) already gives the result retail's spill
// slot, but copies a `mov al, 1` exit into the success path: 42 (284 bytes). On it: a flat chain of three
// SUCCEEDED blocks 42; `result = true` ahead of the ports loop 79; `result = SUCCEEDED(error)` 42;
// `result = port == NONE` 42; `__assume(xhv != NULL)` before the return 50.
// Round 63 (m3 lane 15; one-file LTCG harness, exact here: base 2, natural nested ifs 42, plain store 36): the
// creation folded into the guard, `if (!result || !SUCCEEDED(error = XHVEngineCreate(...)))`, 26; an E_FAIL-initialised
// error with the engine pointer and creation inside `if (result)` 8 (push ebp sinks); the natural body as a
// `do { ... } while (false)` with breaks 42; the natural nested body with an S_OK-initialised error 42; plain
// `result = true` with the engine bound after the guard 42. Retail keeps push ebp at the entry and puts the lea after
// the je only when the guard edge reloads the stored result; every natural form forwards AL on that edge or copies
// a `mov al, 1` exit into the success path.
// Round 98 (m3 lane 16, fast; the je's target is a third difference the count masks: ours skips the reload of the
// result on the guard edge). On the natural nested body (42, 284 bytes; the result is spilled to retail's slot by
// itself): an empty `if (!result) { }` before the return 42; an empty else arm on all four tests 42; the result as
// the one member of a local aggregate 42; the creation joined to the guard with `&&` 42; a dead `&result` in an
// `if (false)` block 42 (all the same code); the ports loop in an inline helper that sets the result through a
// bool pointer 166 while it stays a call (195 bytes), 42 force-inlined. Every inlined or dead address is resolved,
// so the result is a plain value again, the true store folds into a `mov al, 1` exit and the epilogue is copied
// three times; retail's one epilogue needs the result's load to be opaque on every edge.
// Round 95 (m3 lane 10, fast; natural bodies, no volatile, goto or result pointer): the natural nested body without the
// final true store (the result is already true there) 26 (275 bytes: no `mov al, 1` exit, but push ebp still sinks past the
// guard and the esi region's exit copies the reload); that with the engine pointer bound before the guard 20 (275, lea
// before the je again), bound at the function's top 24; `if (!result) return result;` then the flat body 90 (270 bytes,
// a separate `xor al, al` exit); a false-initialised result set true inside the parameters test 108 (278).
// Round 154 (m3 lane 7, one-file LTCG harness, exact). New: the guard written as `if (result) { ... }` around the
// body with the engine bound before it 2 (same code, one goto fewer: kept); that with the three inner gotos as breaks
// of a `do { } while (false)` 2 (same code; not kept, the loop only stands in for the gotos); `FAILED(error)` for
// `!SUCCEEDED(error)` 2 (same code); the result pointer declared before the call, the result assigned after it 2
// (same code); nested SUCCEEDED ifs keeping the volatile true store 20, with the engine bound inside 26 (280 bytes).
// Re-measured (same code as listed forms): the engine bound inside the guard block 8 (MiniMax E nested block scope);
// the ports loop as a force-inlined bool helper returning true 39/42/42 (round 98 force-inlined helper; plain
// `inline` stays a call, 175-178); on the goto form with the engine after the guard: split result declaration, the
// test through `*local_0`, the call stored through `*local_0`, `result == false`, a false-initialised result, the
// call embedded in the guard and `return *local_0` 8 each (the listed engine-after-guard 8); permuter on the
// unchanged body (600 s, 66 tries) best 2. Diagnostic: `#pragma optimize("s", on)` on the natural nested body 115
// (frame pointer, 244 bytes).
// Round 192 (m3 lane 4, fast): the result pointer declared before the engine address 2; the runtime parameters declared
// after the engine pointer and the error 2; the mode switch as an if/else 74 (291 bytes); the result assigned from the
// parameters call in its own statement 2; the processing-mode counter as `long` 3.
// @retail 0x55810
bool voice_xhv_create(c_voice_xhv *xhv)
{
    XHV_RUNTIME_PARAMS parameters;
    XHVEngine **engine;
    HRESULT error;
    bool result = voice_xhv_get_runtime_parameters(&parameters, xhv);
    engine = &xhv->engine;
    bool *local_0 = &result;
    if (result)
    {
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
    }
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

// MiniMax G (11-14): no new form, no lower score (94): the identity flag still lands in bl where retail keeps it in
// al, retail sets the total to two and the channel pointer after the flag's compare where this build sets them
// before it, and the inner channel blocks are still not unrolled.
// MiniMax G (11-13): no new form, no lower score (94): the identity flag stays in bl where retail
// keeps it in al, so the loop temporaries take eax/al where retail uses ebx/cl, and the inner
// channel blocks are not unrolled.
// MiniMax A (11-13): no new form, no lower score (94): the identity flag still takes BL here while retail keeps it in AL, and retail sets the size to two only after the flag's compare where this build sets it before; no untried form in the list moves the flag out of BL.
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

// MiniMax A (11-14): no new form, no lower score (60): the player bit stays in ebx where retail keeps it in ebp, the
// entry size comes from a zero-extended mask byte here and from a compare and setne there, and the capacity-fail exit
// keeps its own epilogue (460 bytes against retail's 449).
// MiniMax A (11-13): no new form, no lower score (60): the membership helper's mask and shift operands swapped compile to the same code (all four network-voice scores unchanged), and the register split stands (the player bit in EBX and the entry size in EBP where retail has EBP and EBX).
// score 60 (was 60): the player bit and entry size in EBX/EBP (retail EBP/EBX), the kept mask in EAX, capacity through
// a register, the capacity-fail exit's own epilogue (460 against 449 bytes).
// MiniMax H (11-11): no new form, no lower score (60); the membership helper's mask test written with the shift first gives the same code, so the mask load still reaches EAX before the shift where retail keeps the zero result in EAX and loads the mask into ECX after it.
// MiniMax S (11-11): no lower score (60): the player bit as a named local inside the membership helper 60 (same
// code), the entry size as `include_mask ? 13 : 11` 147 (461 bytes), and the same size with the retail order
// kept/written/capacity/size 137 (445 bytes, 0x56380 lost its match at 13); the mask load is still hoisted into
// EAX above the shift where retail keeps the zero result in EAX and loads the mask into ECX after the shift.
// MiniMax I (11-11): no new form, no lower score (60); retail keeps the zero result in EAX from entry to the
// capacity-failure exit and gives the player bit EBP and the entry size EBX, and every order of the result,
// capacity, size and written locals compiles to the same code.
// MiniMax G (11-12): no new form, no lower score (60): the helper's mask and shift operands swapped compile to
// the same code, and the entry size as `11 + 2 * (include_mask != 0)` reaches retail's setne form but lays the
// block out differently (146), so the body stands.
// MiniMax R (11-09): no lower score (60): what still differs is the register split - the player bit sits
// in EBX and the entry size in EBP where retail keeps the bit in EBP and the size in EBX, the kept mask
// is computed in EAX where retail computes it in EBX, and retail loads the queue's mask word after the
// bit is built while ours hoists that load into EAX before the frame is set up. The forms measured this
// round give the same code or are worse: the entry size declared in the outer scope and assigned later
// (60), the kept mask declared in the outer scope (60), the player bit at signed width (60), the entry
// size at int width (60), the written count at int width (60) and the player bit variable removed with
// the entry test using the shift (60), while the entry size computed in the outer scope (158), the
// capacity decremented before the size and the kept mask (136), the membership test with the channel's
// mask field (154), the membership test with the queue's mask word (154) and the entry size at word
// width (145, which also loses 0x56380) are worse.
// MiniMax R (11-09): no lower score (60): the statement orders and the size forms measured again - the size
// after the kept mask (60), the size as a times expression (60), the capacity comparison swapped (60), the
// written count as a dword (60) and the bit moved past the membership test (60) give the same code, while the
// size in the capacity test (160), the kept mask as a word (81), the size after the capacity (143), the kept
// mask after the capacity (136), the size from a ternary (147) or from a bool expression (146) and the
// membership test written with the queue's mask (154) are all worse.
// MiniMax R (11-09): score 60 (was 134): the clamp is two bounds tests on the player (so retail's
// `test edi, edi` appears) and the pre-loop statements run entry size, kept, capacity, written, with
// the entry size as `11 + 2 * include_mask`. Still differs: the player bit is in EBX and the entry size
// in EBP where retail keeps them in EBP and EBX, the kept mask is computed in EAX instead of EBX, the
// capacity is decremented through a register instead of in place (so the capacity-fail exit keeps its
// own `xor eax, eax` epilogue) and the loop head has an extra jump; the code is 460 bytes against 449.
// Earlier, score 60 (was 134): the retained mask is a plain local and the pre-loop statements run entry size,
// kept, capacity, written: the clamp test now agrees with retail. Still differs: the player bit and
// the entry size sit in EBX and EBP where retail has EBP and EBX, the kept mask is computed in EAX,
// the capacity is decremented through a register and the capacity-fail exit gets its own
// `xor eax, eax` epilogue; the code is 460 bytes against 449.
// size, written, mask: the bit (EBP), size (EBX), count (ECX), the frame order i/written/kept/size/bit
// and the single mask read at the end now agree with retail. Still differs: EAX's zero is reused for
// `written = 0` and the PIN test (retail `test edi, edi`, immediate store), so the capacity-fail exit
// gets its own `xor eax, eax` epilogue and the code is 460 bytes against 449.
// MiniMax E (11-11): no new form, no lower score (60): retail normalises the include_mask byte with
// `cmp`/`setne` before the lea that forms the entry size while ours widens the bool with `movzx`;
// writing that normalisation out (`include_mask != 0`, or through a byte local) measures 146, and
// moving the statements (kept, written, capacity, entry size) measures 137 with a different call
// convention (`ret 0x10` against retail's `ret 0xc`), so both are worse and the body stands.
// MiniMax E (11-07): no new form, no lower score; the earlier round's 24 statement orders of the
// capacity, size, written and mask statements leave the result in EAX with its own epilogue.
// MiniMax h (11-06): no new form, no lower score: all 24 orders of the capacity, size, written and mask statements were measured and the current one is the best (134); every order with the mask first (135-171) moves the player onto the stack and breaks 0x56380, so retail's mask in EBX, its literal zero store and its single epilogue stay out of reach.
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
// Round 287: Actual membership/clamp return producers, expression/branch/both,
// 192/167/179/170/197/170; membership/both producers without entry zero 198/199.
// Independent result zero and capacity-failure epilogue remain; original body restored.
// Round 480: Complement low-bit mask at both count observations, with original/shared
// recipient-bit consumers and original/native recipient accumulation: dword 138/158/174/181,
// word 179/158/183/182. Original mask expressions, field accumulation and body restored;
// zero reuse, pre-loop scheduling and the separate capacity epilogue remain.
// Round 485: Serialized recipient-result word/dword, branch/conditional, original/per-mode
// first-byte publication 132/166/132/166/132/166/132/166. Two-point gains rejected;
// original serializer and capacity/result graph restored.
// Round 287: Actual membership/clamp return producers, expression/branch/both,
// 192/167/179/170/197/170; membership/both producers without entry zero 198/199.
// Independent result zero and capacity-failure epilogue remain; original body restored.
// Round 198 (lane 3, fast with 0x56380; this base is only 27 off when aligned: `test edi, edi`, the pre-loop order, the
// test before the first cursor store, the exit layout): first cursor byte stored in each include_mask arm 178 (475 bytes;
// it gives retail's test/load/store/je head merge but moves the bit to memory); pre-loop orders (c capacity, e size,
// w written, k kept), plain kwce/wkce 137 (retail's visible schedule; the mask or size takes a register, the player goes on
// the stack, 0x56380 13), ekcw 147, ekwc 147, ecwk 137, eckw 141, ewck 137, ewkc 139, ckew 154, ckwe 154, cwke 154, cwek 145,
// cekw 160, kcew 154, kcwe 137, kecw 171, kewc 171, kwec 135, wcek 145, wcke 154, weck 145, wekc 136, wkec 135; the same with
// the arm stores ewkc 179, wekc 146, wkce 137, wkec 133, kwce 134, kwec 134, cewk 178, ckew 196, ckwe 113, cwek 128 (kept-
// first orders lose 0x56380, 12). Clamp through a by-value inline 134, with kwce 137, with arms 178, both 134; `return NONE`
// 170, with arms 169, with kwce or wkce 149, with arms and either 149 (0x56380 73); result dword/int 134/134, written dword
// 134, both 134, result on its own line 134, named active flag 134; length in its own local, assigned last, 134, with arms
// 178, with kwce/wkce 137/137, with arms and those 134/137; one-read retained mask word/dword with kwce 137/137, wkce 137,
// ckwe 154, cwke 154, wcke 154, kcwe 137, arms and kwce 134; bare block over the rest with cewk/kwce/wkce 134/137/137, with
// arms 178/134/137; second loop in a block with kwce 137; first loop in a block with kwce/wkce/arms and kwce 137/137/134.
// Diagnostics: volatile mask with kwce/wkce, plain or arms, 151/151/162/162 (0x56380 137); no second loop 163, no mask clear
// there 127, no compact call 197 (retail's literal zero tests and EAX's zero kept to the capacity exit appear only here), no
// kept update 127, no count decrement 178, no entry clear 142, no header stores 183. In kept-first forms the stable registers
// are queue, cursor, size, entry pointer against retail's queue, player, cursor, entry pointer: player against size.
// Round 59 (m3 lane 11, fast with 0x56380): the queue typed as the voice channel with typed packet entries (header word,
// flag, mask, data; 0x56380's call without the cast) 134 (identical code); result left unset and assigned 0 in the
// inactive and capacity-fail arms 177 (472 bytes, 0x56380 73); round 98's retained mask declared inside the capacity test,
// re-measured on the typed body, 127 again (449 bytes; the bit goes to memory, EBX takes it); the named bit at the header
// and clear uses 153 (433 bytes, 0x56380 13); `11 + (include_mask ? sizeof(word) : 0)` 137; the loop's player test as an
// early `continue` 134 (same code). Original body restored.
// Round 207 (lane 3, fast with 0x56380): the clamp written inline in the test, no local, 135; a short
// clamped value 180 (0x56380 2); entry size as `11` plus `sizeof(word)` under include_mask 191 (486
// bytes); a long player bit 134 (same code); `entry_size * written + 2` 134 (same code).
// Round 145: direct membership mask 140 (446 bytes); staged player bit, split written initialization and staged
// retained decrement 134 each; direct membership with retained mask in the capacity scope, also with staged bit,
// with per-arm first-byte stores, and with both 142 each (449/449/446/449 bytes). Original body restored.
// Round 72 (m3 lane 1, one-file harness, exact: control 134 at 460 bytes, 0x56380 0; masked aligned in brackets, base [25]): the two
// `result = NONE; goto done;` as an else chain 134 [25] (same code, kept: no goto left). Retail keeps `result` as one web in EAX from the
// entry to the capacity-fail exit, which is why kept takes EBX, written and the clamp use literal zeros and the capacity-fail edge joins the main epilogue; ours
// duplicates the return and rematerialises the zero. Diagnostic only, `long result = <global read>`: no `cmp edi, eax`, literal zero, `mov eax, edi` before the pops
// and one epilogue, 145 [28], with written, kept, capacity, size first 126 [41] (player then goes on the stack, 0x56380 12). Tried on it: named total assigned last
// 134 [25]; return NONE at both tests 170 [55], with total 159 [52], and written-first 152 [58], kept-first 149 [59] (each 0x56380 73); written-first without them
// 137 [38], kept-first 137 [38] (0x56380 12); positive nesting 158 [38], `has && clamped == player` 172 [54], `!has || clamped != player` 175 [52], the clamp inside the
// test 158 [38]; `__assume(result >= NONE)` or an empty `do { } while (false)` before the return 134 [25]; `__assume(result == 0)`, `(result != NONE)`, `(result >= 0)`
// before the capacity test or in its else arm 134 [25]; empty else 134 [25], `do { } while (false)` in it 190 [51] (0x56380 11), `result = 0` in it 201 [47] (0x56380 73);
// a one-pass `do ... while (false)` over the body 190 [51] (0x56380 11), inside the init test 190 [51], with the capacity test as a break 134 [25]; arms' first-byte
// stores on the else form 178 [37]. 24 return-structure combinations all had 0x56380 at 73 (scores 191-219).
// T57: Existing removal/fallback word-flag helpers and retained-bit helper, subsets
// 1/2/3/4/5/6/7, then complete shared flags with positive capacity admission
// 134/134/134/184/184/184/184/184. NOT kept: no exact or qualifying structural gain; original complete body restored.
// Round 90 (m3 lane 9, fast with 0x56380): the length in a local before the header stores and `return length;` at the
// end of the capacity block 220 (496 bytes, 0x56380 73); the capacity test as `if (capacity < entry_size) return result;`
// ahead of an unconditional block 179 (466 bytes, 0x56380 73); the entry size declared first and the rest assigned in
// retail's schedule order (kept, written, capacity, entry size) 137 (445 bytes, player on the stack, 0x56380 13); the
// player bit declared at function scope ahead of the result 134 and entry size/written/kept declared at the top of the
// active block 134 (both same code). Restored.
// MiniMax D (11-06): no new form, no lower score.
// M3 lane 11 r77 (fast with 0x56380; aligned in brackets, base [27]): one `length` variable as the written counter and
// then the byte total 151 [234] (player on the stack); include_mask typed char 134 (same code); a walking element pointer
// beside the counter 134 (same code); the loop test as `queue->count > i` 134 [29]; the clamp as
// `player >= 0 ? (player <= 15 ? player : 15) : 0` 140 [32].
// Round 63 (m3 lane 15; one-file LTCG harness, exact here: base 134 at 460 bytes, both ret 0xc): re-measured, the
// retained mask declared inside the capacity test 127 (fast-confirmed, 449 bytes, 0x56380 and 0x53b00 still match; one
// exit as retail, but the bit moves to EBX and memory and the capacity takes EBP before the loop; 7 lower, under the keep
// threshold). New on it: the bit declared after the has-player test 127, the bit written at its use 127, the mask typed word 127,
// entry size ahead of `capacity -= 2` 128, `entry_size <= capacity` 127, written and mask both inside the capacity
// block 196 (482 bytes). Grid on that form (6 orders of capacity/size/written x per-arm first-byte stores x bit
// placement x direct NONE returns, 48 builds): best 127 (capacity, size, written); written-first orders 124 but
// 0x56380 lost (136); direct NONE returns 156 (0x56380 73).
// Round 71 (m3 lane 13, fast with 0x56380; our build is already `ret 0xc`): `result = NONE` set before the has-player
// test and `result = 0` after the clamp 129 (445 bytes; result a variable in EAX as retail, but the player moves to EDX
// and the cursor to the stack, 0x56380 13); direct NONE returns with `return length;` ending the capacity block 171, with
// the player bit declared first 170 (0x56380 73 each: result becomes a real variable in ESI or memory, queue takes EAX,
// the player EBP and goes on the stack); has-player test first with `else if (active) result = NONE` 147 (threads to
// the same code, block order differs); kept declared before the capacity test and assigned inside it 127 (same code as
// the mask declared inside).
// Round 61 (m3 lane 2, fast with 0x56380 and 0x53b00; baseline 134, already `ret 0xc`): the two failures only assigning NONE and
// the rest under a following `if (result == 0)` 134 (same code: the test is threaded away and the result is still a constant,
// so EAX's zero is shared and the capacity exit gets its own epilogue), as `!result` 134, as `result != NONE` 134; that form
// with the retained mask first (mask, written, capacity, size) 137 and written first 137 (445 B each, the cursor then arrives
// in EAX and the player on the stack, 0x56380 13); the clamp ahead of one `!has || clamped != player` assignment 162; the
// length built in place from the counter (assign, scale, add 2) 134 (same code). Retail's call in 0x56380 sets EAX (player)
// then EDX (cursor), which fits the declared order. Original body restored.
// P399: Actual queue-count/recipient-count/aggregate-mask field identities, subsets 1-7, then entry-recipient word identity:
// 133/134/133/133/133/133/133/133. NOT kept: no byte equality or native home repair; original complete body restored.
// Round 142 (m3 lane 7; one-file LTCG harness, exact: 60 at 460, 0x56380 matched): the 24 orders of entry size (e), kept
// (k), capacity (c), written (w) x the size as `11 + 2 * include_mask` / `include_mask ? 13 : 11` / `11 + (include_mask ? 2
// : 0)` x the result before the header stores or at the end of the capacity block (144 forms): best 60 (main); kept-first
// orders move the player to the stack (0x56380 12-13), the result at the end moves the convention (0x56380 137, 449-452 B).
// cewk with `include_mask ? 13 : 11` 134 but aligned 18 against 41: retail's EBP bit, EBX size and setne/lea; left is the
// zero result in EAX reused for the clamp (`cmp edi, eax`) and `written = 0`, and the capacity failure's own epilogue. On
// that: direct NONE returns 170, `else result = 0;` on the capacity test 201, the zero set only in the inactive and
// capacity-fail arms 177, direct returns plus that else 189 (0x56380 73 each). Diagnostic, result started from a global
// read 145 (still two epilogues). 300 random forms (orders x sizes x bit at top/pre-loop/at use x if/else-if or ternary
// clamp x failure arm first or second): best 60, lowest aligned with the caller kept 18-19 (130-134).
// Round 181 (m3 lane 4, fast with 0x56380), re-measured on the `include_mask ? 13 : 11` size: current order 147 (461
// bytes), capacity/size/written/kept 134 (head matches retail through the active test; EAX's zero then serves the
// clamp and `written = 0`, and kept takes EAX), with round 71's NONE-then-0 result 129 (445 bytes, 0x56380 13), with
// written inside the capacity block 197, with kept inside it 127 (449 bytes). New on the 134 form: include_mask typed
// char 134 (same code); the clamp compare written `player != clamped` 135; the clamp testing its copy (`clamped < 0`,
// `clamped > 15`) 134 (same code); written typed short 135 (461 bytes); the compaction test without the word cast,
// `kept != (dword)((1 << queue->count) - 1)`, 128 (457 bytes).
// M3 lane 16 r100 (fast with 0x56380; both builds already return with ret 0xc): the result computed at the end of the capacity
// block re-measured 155 (0x56380 137); include_mask typed char with the size as `include_mask ? 13 : 11` 147 (461 B); the size as
// `include_mask != 0 ? 13 : 11` 147 (461 B); kept computed before a ternary size 171 (0x56380 12); the player bit typed word 139
// (445 B, 0x56380 13); the clamp's tests on the clamped copy 60 (same code). Rebuilt for reference: round 142's cewk order with the
// ternary 134 (aligned 19: retail's EBP bit, EBX size and setne; the zero result in EAX is then reused for `cmp edi, eax` and the
// written store, and kept takes EAX), retail's visible kwce order with the ternary 137 (player passed on the stack, 0x56380 13).
// Body unchanged.
// Round 73 (m3 lane 15; one-file LTCG harness, exact: base 60 at 460 bytes, 0x56380/0x564c0 matched): `result` itself counting
// the written entries (no `written` local; `output[1] = (byte)result; result = result * entry_size + 2;`) 153 (458 bytes,
// 0x56380 55), the same on the ternary cewk base 151 (0x56380 13); `capacity -= 2` inside the capacity test 60 (same code);
// the writer after the active test as a forced-inline helper with early returns (NONE, NONE, `return 0` on capacity, the
// length) 166 (queue moves to EAX, 0x56380 133), with one result variable 187 (477 bytes, 0x56380 66); as plain `inline` it
// stays out of line (diagnostic, 553). Re-measured: the length local with `return length;` ending the capacity block 216 (494
// bytes, 0x56380 73; round 90's 220 form): the zero result then becomes a stack variable and queue/player arrive in EAX/ECX.
// Round 73, continued: the clamp test through the existing voice_packet_clamp_count (`voice_packet_clamp_count((byte)player,
// 0, 15) != player`, the same result for every player) 157 (0x56380 55).
// Round 73, continued: voice_channel_has_player as `static __forceinline` 60 (same code, 0x56380 and 0x564c0 still matched).
// M3 lane 8 r151 (fast, control 60; both builds return with ret 0xc): what still differs is register allocation (retail
// keeps the zero result in EAX as a variable from entry to the capacity-fail exit, as matched 0x56380 keeps its zero in
// ESI, and gives the bit EBP and the size EBX). New forms: the written count typed byte 140 (462 bytes), with the size as
// `include_mask ? 13 : 11` 149; written declared inside the capacity block with that size 197 (488 bytes); `capacity -=
// entry_size` before `written++` 64. Re-measured: the ternary size in the current order 147. No other untried natural
// form is left in the note's list (orders, size spellings, result, clamp, bit and mask forms).
// m3 lane 2 r75 (fast with 0x56380 and 0x564c0; both builds already return with ret 0xc): `result +=` for the success
// length 60 (same code: the add folds); re-measured round 142's cewk order with `include_mask ? 13 : 11` 134 (head through
// the clamp is retail's but for `cmp edi, eax`, the zero result merged with the literal zeros; kept takes EAX); on that
// form: one `player < 0 || player > 15` test for the clamped copy 133 (444 B), the entry size typed dword 134 (same code),
// the inactive queue as an early `return result;` 175 (0x56380 73); on this body the clamp as `__max(0, __min(player, 15))`
// 66. Retail never uses EAX's zero for a literal zero (`test edi, edi`, an immediate written store), so its result is no
// constant there; every source start of 0 measured so far is merged.
// Round 154 (m3 lane 7, one-file harness, exact; 0x56380 matched unless noted). New: `11 + 2 * (include_mask != 0)`
// (MiniMax G 146 in the current order) in the other 23 orders of size, kept, capacity and written: best 134 (cewk),
// the rest 135-171 (orders with kept or written ahead of the size lose 0x56380 at 12/13); the per-arm flag-byte store
// below with the three size spellings in the 24 orders: best 113 (ckwe with the ternary size; 0x56380 12); the header
// stores swapped (`output[0]` first) 59 (1 lower, statement order, not kept); after it the count decrement before the
// bit clear 183 (478 bytes, 0x56380 2), `kept ^= 1 << i` 99 (456), the player mask cleared before the compaction call
// 70 (462), `written++` right after the capacity test 99, `capacity -= entry_size` right after the capacity test 176
// (472). Re-measured on this base (listed on older bases 178/142): the flag byte stored in each include_mask arm 176
// (472 bytes; the loop core becomes retail test/store/branch and the entry size takes EBX as in retail, but the
// player bit goes to memory), with `*cursor++ =` or `cursor[0] =` 176.
// Round 178 (m3 lane 5, fast with 0x56380/0x564c0/0x53b00, control 60): the kept-first order with the ternary size (137,
// 0x56380 13) is the one family whose code keeps the zero result in EAX up to the capacity-fail edge with one epilogue, as
// retail does, but it reuses that zero for the clamp and `written = 0`, passes the player on the stack and gives the cursor
// EDI and the size EDX; retail's allocation (cursor EDX, the size and the kept temporary both EBX, the bit EBP, capacity
// decremented in place) needs the cursor to keep EDX there. New forms: the result left unset on the active path with
// `else result = 0;` for the inactive queue 175 (472 bytes, 0x56380 73, the result on the stack); the success length stored
// through the result's address 60 (same code); on the kept-first ternary form the length in a local assigned to the result
// after the mask loop 137 (same code as that form); kept in two steps (`kept = 1 << count; kept--;`) 60 (same code), on the
// kept-first ternary form 137; `capacity -= 2` as the first statement of the active block 172 (472 bytes, 0x56380 2), with
// kept first and the ternary 152 (456); written declared at function scope with kept first and the ternary 145; kept
// declared at function scope and assigned in place, kept first with the ternary, 137. Diagnostic only (the result started
// from a global read; the 24 statement orders x the two size spellings, 48 forms): no zero reuse and one epilogue in the
// kept-first orders, but the player still goes on the stack (best 126, 0x56380 13); with the size first 54-167 and the
// caller matched (54 for size/written/kept/capacity with `11 + 2 * include_mask`, which re-reads the global at the
// capacity-fail exit). An opaque zero alone does not give retail's allocation.
// P442: Actual active-byte seeded result, long/int/dword/unsigned-int, direct admission
// then shared active truth: 181/181/181/181/181/181/181/181. NOT kept: no exact result/home repair;
// frame 0x10/0x14, ret 0x10/0xc and caller 0x56380 at 13 reject all forms. Original body restored.
// P458: Entry active byte/short/long/dword and inactive bool/byte/short/long outcomes, original separate result:
// 60/60/60/60/60/60/60/60. NOT kept: native zero retention, bit/size allocation and capacity epilogue not recovered; original complete body restored.
// P469: Independent 64-bit result signed/unsigned, recipient bit, clamped player,
// entry extent, emitted count, retained mask and both real scan ordinals:
// 177/177/212/226/203/180/197/261. NOT kept: native ingress/frame or extent regress;
// original 32-bit scalarization, matched caller contract and complete body restored.
// T105: Serializer channel/recipient moves and payload-return subsets 1..7:
// 185/181/216/60/185/181/216; complete move pipeline 202. NOT kept: no
// native zero/bit/size equality; copy forms lose the caller; original body restored.
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
				long entry_size = 11 + 2 * include_mask;
				dword kept = (1 << queue->count) - 1;
				capacity -= 2;
				long written = 0;
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

// MiniMax G (11-14): no new form, no lower score (85): the availability bytes read into locals for the muted test
// and the mode helpers parameterised on them leave the second byte loading lazily inside the muted test, where
// retail loads both before it and keeps the remote mask in esi.
// MiniMax G (11-13): no new form, no lower score (85): the second availability byte loads lazily
// where retail loads both before the muted test, and the second port-flag query is called out of
// line: BLOCKED-BY 0x59670/0x596a0.
// MiniMax B (11-13): no new form, no lower score (85): the session getters 0x59670/0x596a0 (src/unknown_058ee0.cpp) are outside this lane's files, and the second port-flag query is still called out of line where retail inlines it here: BLOCKED-BY 0x59670/0x596a0.
// score 85 (was 85): the second availability byte loads lazily (retail loads both before the muted test and keeps
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
// score 85 (was 85): the second availability byte loads lazily (retail loads both before the muted test and keeps
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
// P464: Comparison-test bool/raw signed result producers 146/205; signed/unsigned short
// comparison ordinal 188/188; literal mismatch byte/word/long 2/2/2; success-continue 2.
// NOT kept: no exact post-format reload recovery; original comparison and whole body restored.
// P466: Empty identity long/byte/word storage with memset 2/2/2; aggregate-zero
// dword/byte/word and read-only dword/byte 2/153/142/2/153 on the plain-copy graph.
// NOT kept: no exact reload recovery; original empty input and complete body restored.
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

// MiniMax G (11-13): no new form, no lower score (139): the member takes ebp where retail keeps
// the channel from entry and spills the member, and the cursor and route homes differ.
// MiniMax B (11-13): no new form, no lower score (139): the channel in a local (139), the cursor advanced on its own (180) and both together (180) leave the difference - retail keeps the channel in ebp from the entry and the data pointer in edi, ours keeps the data pointer in ebp.
// MiniMax A (11-13): no new form, no lower score (139): a hoisted packet cursor (`byte *packet = data + 2`, advanced by
// the packet size) scores 164 and a local copy of the channel compiles to the same bytes as the baseline, so retail's
// split (the channel in ebp from the entry, the member spilled to the stack) stays out of reach here.
// score 139 (was 139): the member takes EBP (retail: the channel from entry, the member spilled), the cursor and route
// homes differ.
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
