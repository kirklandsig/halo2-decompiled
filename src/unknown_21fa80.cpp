#include "unknown_11c920.h"
#include "sound_driver.h"
#include "unknown_21e230.h"
#include <math.h>
#include <stdlib.h>
#include <stddef.h>

// @flags /O2 /arch:SSE /Op /Gr

struct s_looping_channel_properties;
struct s_looping_effect_playback;
struct s_sound_channel_parameters;
struct s_mixbin_list { long count; DSMIXBINVOLUMEPAIR pairs[8]; };
struct s_channel_update_input
{
	dword field_0;
	long format;
	real pitch, gain;
	byte field_10[4];
	real field_14, field_18, field_1c, field_20;
	short voice_index;
	byte field_26[2];
	real field_28;
	long field_2c;
	dword field_30;
	byte field_34[4];
	real levels[4];
};
struct s_channel_send_state
{
	dword flags;
	real level, level_offset, gain0c, gain10, field_14;
	real left_gain, right_gain, rear_left_gain, rear_right_gain;
};
class c_channel_effects_view
{
public:
	virtual void initialize(void *, void *, bool)=0;
	virtual void dispose()=0;
	virtual void set_i3dl2(void *, long)=0;
	virtual void add_effect_sends(s_channel_send_state *, bool, long, s_mixbin_settings *)=0;
	virtual void add_send(long, long, real, s_mixbin_settings *)=0;
	virtual void add_source_mixbins(s_channel_update_input const *, bool, long, s_mixbin_settings *)=0;
	virtual void add_sends(s_channel_send_state *, long, long, s_mixbin_settings *)=0;
};

typedef char check_channel_update_levels[offsetof(s_channel_update_input, levels)==0x38?1:-1];
void function_2215d0(s_mixbin_list *, s_mixbin_settings *, LPDIRECTSOUNDSTREAM);
void function_2216f0(s_mixbin_list *, s_mixbin_settings *, LPDIRECTSOUNDBUFFER);
void function_2201f0(long, s_sound_channel_parameters const *, bool);
real const g_44a23c=-64.0f;

PRIVATE inline real channel_pin_gain(real value)
{
	return value < -64.0f ? -64.0f : value > 0.0f ? 0.0f : value;
}
PRIVATE inline long channel_pin_gain_bits(long bits)
{
	long result;
	if(*(real *)&bits < -64.0f) result=0xc2800000;
	else if(*(real *)&bits > 0.0f) result=0;
	else result=bits;
	return result;
}
PRIVATE inline real channel_linear_gain(real value)
{
	long pinned=channel_pin_gain_bits(*(long *)&value);
	real scaled=*(real *)&pinned*0.05f;
	return (real)exp(scaled*2.3025851f);
}
PRIVATE inline c_channel_effects_view *channel_effects()
{
	return (c_channel_effects_view *)SOUND_DRIVER_GLOBALS->effects;
}

// score 545 (was 545): frame 0x22c versus 0x224; stream/gain homes and mixed math remain.
// the level-energy block summed in double (the gain converter returning double) 528, but the
// shared converter costs the matched 0x21f8a0 (0 -> 67): restored (MiniMax E run 12).
// frame 0x22c versus 0x224; stream/gain homes and math expansion differ.
// native input direct bit 541; all native input bits 548; captured direct use 540;
// shared gain converter 629; inline depth one 541; native output bits with captured direct 544.
// frame 0x22c versus 0x224; stream and gain homes plus SSE and x87 scheduling differ; native-bit tests gain only three
// tried initial driver value 541; native send bit writes 567; native send bit tests 538; native pitch truncation 551; level pointer traversal 558; real absolute values 541; native flags real abs 538; native flags driver 538.
// Frame 0x22c versus 0x224 and stream/gain homes remain. Redundant level clamp removed: 575 to 545, 1901/1901 bytes. Voice output reuse 575; query output scope 572; complete flag projection 572; output stream observation 614 (plain).
// Voice selector snapshot 572 fast; level-offset snapshot 557 fast; entry status,
// initial voice output and initial settings 532 fast each (unchanged preview 532).
// Real gain value 545; native linear return 545; unsigned pin bits 545; exponential input 561;
// native return/input 561; logarithm/root results 551/549; combined math results 554, frame 0x230.
// Original body and helper contracts restored; frame 0x22c and stream/gain homes remain.
// Native gain read/routing cache/all reads 532/532/532 fast (control 532); native gain/all publication 540/540; common status/buffer volume/stream volume 549/543/566. Full baseline remains 545.
// Linear input pointer 544; native speaker array 545; array plus input pointer 544; known-clear
// send bits 547; send field order 545; routing flag publication last 545; cache from produced send
// fields 545; direct output level 545. One-point reductions dropped; all body/type/helper forms
// restored. Frame 0x22c versus 0x224 and stream/gain/math homes remain.
// Native sixteen-byte speaker copy 529 fast; distinct scalar math names 532; early selected gain
// 577; copy/names 529; copy/early gain 561; names/early gain 577; all three 561;
// send-scope zero energy 533 (reduced control 532). Three-point reductions rejected.
// Round 127: changed/both word predicates 556/574; complete input record 620 (sibling lost); pin
// default-input/early returns 534/537; default level/bounded amplitude 543/530; combined default
// producers 552 (fast control 532). Every source form restored.
// Round 171 reduced control 532: live direct field 555; live mode field 588; shared voice output 532; live
// direct/mode 579; live direct/shared output 555; live mode/shared output 588; live fields/shared output 579;
// shared voice cursor 532.
// Round 213 reduced control 532: playback state-transition gate with bool/byte/word/long
// request domains 549/549/550/550; each with status declaration at query 549/549/550/550.
// Original playback arms, query schedule and all math restored.
// Round 377: Complete cache-valid bool/byte/short/long result graphs
// 542/542/569/569; entry declaration forms 542/542/569/569 (fast control 532,
// full baseline 545). Byte forms recover checker length but regress; all three
// consumers, original status, mixed math and helper contracts restored.
// Force-inline gain pin, bit pin, linear gain and combinations, then all four
// existing helpers: eight forms all 532 on the reduced control. Helpers restored.
// Round 227: Double energy/sample, named complete square, their pairs/all,
// all plus rounded root input 530/532/538/576/531/560/561/566 (precision exact-only). Restored.
// Existing mixbin/final-update inline-noinline declaration contracts, all seven subsets,
// and the complete inline-only graph 532 each (reduced control 532; full baseline 545).
// Every declaration and source body restored; frame and math expansion remain.
// Round 367: Native real exponential/root/logarithm APIs, all seven subsets and the full graph with a
// native gain value: eight forms all 532 (fast control 532; full baseline 545). Original body and helpers
// restored.
// Round 237: one driver/at-use channel/one effects receiver 567/641/539;
// driver-channel/driver-effects/channel-effects/all 567/555/623/555; all with mode interval 556
// (fast control 532; full 545). All helpers and body restored; frame and stream/gain homes remain.
// Round 381: Native integer-bit linear inputs, signed/unsigned and ordinary/forced expansion, copied
// speakers 542 each; original input-array speakers 551 each (fast control 532, full 545). All
// input/helper forms restored; frame and gain homes remain.
// Round 242: Native double absolute-value API across gain/routing/voice gates,
// seven complete subsets and all with real gain value [547, 534, 532, 539, 543, 535, 537, 537].
// Reduced control 532, full 545; all width forms and original helpers restored.
// Round 280: Byte/char/word/long force inputs with signed/unsigned channel indices all 532 fast (control
// 532, full 545); matched sibling preserved. Original signature, both forward declarations, body and
// mixed-math helpers restored.
// Round 396: Pitch bound-test result with long/short offset and switch/if dispatch,
// late destinations 532/535/531/534; entry uninitialized destinations 532/535/531/534
// (fast control 532, full 545). One-point reduction dropped; all original body/helpers restored.
// Round 290: Existing linear/bit-pin/real-pin input reference subsets 542/532/536/532/542/532/536;
// all with native gain value 536 (fast control 532, full 545). Original executable source
// restored.
// Round 218: Complete speaker library copy with seven native voice-output entry-scope subsets, then
// all three role names, 529 each (reduced control 532, full 545). Three-count reductions rejected;
// all native output scopes, copy and body restored.
// Round 197: Complete computed real gain/routing/pitch subexpressions, masks 1..7,
// 542/547/554/545/542/547/554; all with native gain value 554.
// Native send word stores/tests, stores, stores/tests with routing, tests on gain graph
// 545/538/562/550; complete library speaker copy with store-only/both views, then native
// gain values, 566/557/566/557. Three- and seven-point gains rejected; original body restored.
// DL204: staged routing-result bool/byte/short/long 532 each; guarded interval and
// conditional-absolute delta forms 589/544/589/544 (reduced control 532, full 545). Restored.
// Lane 10 round 34: retail sends the voice mixbins through a two-case switch on the channel mode (`sub eax, 0;
// je; dec eax; jne`, case 1 with two calls whose second call is cross-jumped with case 0's one call), where our
// counted loop gives a countdown loop. Switch cases 0 then 1 (or 1 then 0) with their own calls: retail's block
// and branch shape, fast 532 (aligned difference 462 -> 434), plain 568 at 1879 bytes (aligned 481 -> 441;
// the positional score rises with the shorter body). Fallthrough case 1 into case 0 545 fast; a plain real gain
// selected through the pointer 566 fast, with the fallthrough switch 559.
// Round 217: Complete active mixing-workspace aggregate zeros, seven subsets, then all with the
// unused workspace zeroed: 605/597/640/585/619/619/651/665. Original native workspace initialization restored.
// Round 54 (m3 lane 1; fast control 532, full 545): retail rounds nothing in its fabs tests: the gain difference
// is an SSE subss stored once and loaded for an x87 fabs, the three field differences are x87 `fld; fld; fsub
// st(1); fabs` and the two single fields go through a movss copy, each fabs result compared unrounded; ours (the
// float fabs overload under /Op) stores and reloads after every subtraction and fabs. Lead for the user (flags not
// changed): retail's 0x21fa80 looks built without /Op; `#pragma optimize("p", off)` around the body leaves the
// command-line /Op in force (545, identical code, the one compiler-setting diagnostic). Forms: `fabs((double)...)`
// at all six tests 537 fast (/Op turns the double fabs into a CRT call); mode and direct read before the gain bits
// 565 (plain, 1920 bytes); the send bits and the stream's 0x20 bit through bit_vector.h's bit_vector_set and
// byte_flags_set 532 fast (identical code: the flags stay in a register, retail updates them in memory and stores
// each bit's bool to one byte slot); changed computed after the gain bits 577 fast; direct declared just before
// the send block 545 fast.
// Round 479: shared native active-mixbin workspace for first/submix, first/routing,
// submix/routing and all phases, declared before updates 535/542/532/542;
// at entry 535/542/532/542 (fast control 532). Original workspaces and body restored.
// Round 66 (lane 9, fast control 532, full 545): retail's x87 difference compares never round (no fstp/fld after
// the subtraction or after fabs) and load the stored value first (`fld stored; fld input; fsub st(1); fabs;
// fstp st(1)`); the two magnitude compares bounce the input through xmm0. Ours (float fabs under /Op) rounds twice.
// Tried: `fabs((double)(difference))` 537 (fabs becomes a CRT call under /Op); `fabs((double)a - (double)b)` on the
// four differences 534; `fabsf` on all six 532; an inline previous/current compare helper on all six 617, on the
// four differences 585 (one call stays out of line). Diagnostic only (flags are the user's call): the file without
// /Op 723 (1728 bytes: inline fabs without rounding, but `fld input; fsub [stored]` and no xmm bounce).
// Round 25 (lane 18): the voice sends as `switch (mode) { case 1: add; case 0: add; }` (case 1 falling into
// case 0) 542 plain (not kept, three under the bar); the gain compare with a `(real)` cast 545 (same code); with
// the double fabs called directly, a CRT call (547 fast). Retail writes every flag-bit test (shr/and r8, 1) into
// one stack byte (+0xe, shared with the direct flag) and keeps 0 in EBP for the voice branch's zero arguments;
// a shared function-scope bool 549 fast and a set helper with a bool parameter 545 fast both stay in registers.
// Round 277: standard selected-gain/real-pin/bit-pin extrema, subsets 1..7,
// 570/564/561/536/556/558/578; all with real gain value 578 (fast control 532,
// full 545). Original body and helpers restored; frame and stream/gain homes remain.
// Round N284: Energy-loop dword/short/word/byte indices with <4 and !=4 bounds
// 532/563/563/563 and 532/563/563/563 (reduced control 532, full 545). Exact sibling
// preserved; original four-sample loop, all real roundings and complete body restored.
// Round 479: shared native active-mixbin workspace for first/submix, first/routing,
// submix/routing and all phases, declared before updates 535/542/532/542;
// at entry 535/542/532/542 (fast control 532). Original workspaces and body restored.
// @retail 0x21fa80
void function_21fa80(long channel_index, s_looping_channel_properties const *properties,
	s_looping_effect_playback const *effects, bool force)
{
	s_channel_update_input const *input=(s_channel_update_input const *)properties;
	s_sound_stream *stream=&SOUND_DRIVER_GLOBALS->channels[channel_index];
	bool changed=!(stream->unknown03_0&1) || !SOUND_DRIVER_GLOBALS->unknown0000;
	long gain_bits=*(long const *)(input->gain>-64.0f ? &input->gain : &g_44a23c);
	real const &gain=*(real const *)&gain_bits;
	long mode=stream->channel_count;
	bool direct=(input->field_30&1)!=0;
	if(changed || !(fabs(gain-*(real *)&stream->unknown0c)<0.1f))
	{
		if(input->voice_index>=SOUND_DRIVER_GLOBALS->unknown0008)
		{
			DSVOICEPROPS voice_properties;
			stream->stream->GetVoiceProperties(&voice_properties);
			s_sound_driver_voice *voice=&SOUND_DRIVER_GLOBALS->voices[input->voice_index];
			s_mixbin_settings settings;
			sound_mixbins_initialize(&settings);
			if(mode==0 || mode==1)
				for(long send=0;send<=mode;++send)
					sound_mixbins_add(&settings,voice->unknown00,gain);
			function_2215d0((s_mixbin_list *)&voice_properties,&settings,stream->stream);
			stream->stream->SetVolume(sound_decibels_to_volume(0.0f));
		}
		else stream->stream->SetVolume(sound_decibels_to_volume(gain));
		stream->unknown0c=gain_bits;
	}
	if(changed || direct!=((*(byte *)((byte *)stream+3)>>5&1)!=0) ||
		(!direct && (!(fabs(input->field_18-*(real *)(stream->unknown10))<0.1f) ||
		 !(fabs(input->field_14-*(real *)(stream->unknown10+8))<0.1f) ||
		 !(fabs(input->field_1c)<0.1f) || !(fabs(input->field_20)<0.1f))) ||
		(input->voice_index!=NONE && !(fabs(input->field_28-*(real *)(stream->unknown10+4))<0.1f)))
	{
		s_channel_send_state sends;
		sends.flags=(input->field_30>>1&1)?4:0;
		if((bool)(input->field_30>>3&1)) sends.flags|=8; else sends.flags&=~8;
		if((bool)(input->field_30>>4&1)) sends.flags|=16; else sends.flags&=~16;
		if((bool)(input->field_30>>6&1)) sends.flags|=64; else sends.flags&=~64;
		if(direct) *((byte *)stream+3)|=0x20; else *((byte *)stream+3)&=~0x20;
		if((bool)(input->field_30>>5&1))
		{
			sends.left_gain=input->levels[0]; sends.right_gain=input->levels[1];
			sends.rear_left_gain=input->levels[2]; sends.rear_right_gain=input->levels[3];
			real const *levels=&sends.left_gain;
			real energy=0.0f;
			for(long i=0;i<4;++i)
			{
				real amplitude=channel_linear_gain(levels[i]);
				energy+=amplitude*amplitude;
			}
			real amplitude=(real)sqrt(energy);
			real bounded=amplitude<0.0f?0.0f:amplitude>1.0f?1.0f:amplitude;
			real level=amplitude>0.0f?channel_pin_gain((real)(log10(bounded)*20.0f)):-64.0f;
			sends.level=level;
			sends.level_offset=input->field_14;
			sends.flags|=0x20;
		}
		else
		{
			if(!(input->field_30&1))
			{
				sends.level=input->field_18; sends.level_offset=input->field_14;
				sends.gain0c=input->field_1c; sends.gain10=input->field_20;
				*(real *)(stream->unknown10)=input->field_18;
				sends.flags|=1;
				*(real *)(stream->unknown10+8)=input->field_14;
			}
			if(input->voice_index!=NONE)
			{
				sends.field_14=input->field_28; sends.flags|=2;
				*(real *)(stream->unknown10+4)=input->field_28;
			}
		}
		if(input->voice_index>=SOUND_DRIVER_GLOBALS->unknown0008 && !(sends.flags&0x20))
		{
			s_sound_driver_voice *voice=&SOUND_DRIVER_GLOBALS->voices[input->voice_index];
			s_mixbin_settings unused;
			sound_mixbins_initialize(&unused);
			DSVOICEPROPS buffer_properties, submix_properties;
			voice->buffer->GetVoiceProperties(&buffer_properties);
			voice->submix->GetVoiceProperties(&submix_properties);
			if(sends.flags&2) voice->buffer->SetVolume(sound_decibels_to_volume(sends.field_14));
			else voice->buffer->SetVolume(-10000);
			DWORD status;
			if(sends.flags&1)
			{
				s_mixbin_settings settings;
				sound_mixbins_initialize(&settings);
				channel_effects()->add_effect_sends(&sends,SOUND_DRIVER_GLOBALS->surround,0,&settings);
				function_2216f0((s_mixbin_list *)&submix_properties,&settings,voice->submix);
				voice->submix->GetStatus(&status);
				if(!(status&DSBSTATUS_PLAYING)) voice->submix->Play(0,0,0);
			}
			else
			{
				voice->submix->GetStatus(&status);
				if(status&DSBSTATUS_PLAYING) voice->submix->StopEx(0,0);
			}
		}
		else
		{
			s_mixbin_settings settings;
			sound_mixbins_initialize(&settings);
			if(sends.flags&1) channel_effects()->add_effect_sends(&sends,SOUND_DRIVER_GLOBALS->surround,mode,&settings);
			if(sends.flags&2) channel_effects()->add_send(31,mode,sends.field_14,&settings);
			if(sends.flags&0x20) channel_effects()->add_sends(&sends,SOUND_DRIVER_GLOBALS->surround,mode,&settings);
			channel_effects()->add_source_mixbins(input,SOUND_DRIVER_GLOBALS->surround,mode,&settings);
			stream->stream->SetMixBins(&settings.mixbins);
		}
	}
	if(!force)
	{
		long offset;
		switch(input->format) { case 0: offset=-4597; break; case 1: offset=-501; break; default: offset=-2396; break; }
		real pitch=input->pitch*3.4133334159851074f+(real)offset;
		pitch=pitch<-32767.0f?-32767.0f:pitch>8191.0f?8191.0f:pitch;
		long value=(long)pitch;
		if(changed || (real)abs(value-stream->unknown08)>=1.5f)
		{
			stream->stream->SetPitch(value);
			stream->unknown08=value;
		}
	}
	function_2201f0(channel_index,(s_sound_channel_parameters const *)effects,false);
}

// @retail 0x21f8a0
void function_21f8a0(long channel_index,s_looping_channel_properties const *properties,
	s_looping_effect_playback const *effects)
{
	s_channel_update_input const *input=(s_channel_update_input const *)properties;
	s_sound_stream *stream=&SOUND_DRIVER_GLOBALS->channels[channel_index];
	*((byte *)stream+3)|=0x10;
	stream->unknown28=input->field_2c;
	stream->offset=0;
	if(stream->unknown28!=NONE) stream->stream->Pause(1);
	else stream->stream->Pause(0);
	sound_stream_set_envelope(stream,0.0f,0.1f);
	if(input->voice_index!=NONE)
	{
		stream->stream->SetOutputBuffer(SOUND_DRIVER_GLOBALS->voices[input->voice_index].buffer);
		stream->unknown00=(byte)input->voice_index;
	}
	else
	{
		stream->stream->SetOutputBuffer(NULL);
		stream->unknown00=0xff;
	}
	function_2201f0(channel_index,(s_sound_channel_parameters const *)effects,true);
	*((byte *)stream+3)&=~1;
	function_21fa80(channel_index,properties,NULL,false);
	*((byte *)stream + 3) |= 1;
}
