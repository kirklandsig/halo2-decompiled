// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_10E480.CPP: start an object's vibration from a set of curves.
   Decompiled by lane F for 0x18ca20; its callee 0x10de40 is in
   unknown_10dc70.cpp, whose file this function probably shares. */

#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_11a4d0.h"

struct s_object_list;
extern s_object_list *g_4de2f4;

struct s_vibration_curve_set;
struct s_vibration_output;
void function_10de40(s_vibration_curve_set *set, s_vibration_output *output, real volatile time, real blend, bool skip_special);

struct s_vibration_object_view
{
	long definition_index;
	byte unknown04[0x33e - 4];
	short vibration_offset;
};

struct s_vibration_object_header
{
	byte unknown00[8];
	s_vibration_object_view *object;
};

struct s_vibration_view
{
	byte unknown00[0x30];
	byte output[6];
	short index;
	long tag_index;
};

struct s_vibration_definition_view
{
	byte unknown00[0x1e4];
	real blend;
};

struct s_vibration_globals_view
{
	byte unknown00[0x81];
	bool enabled;
};

// @retail 0x10e480
void function_10e480(long object_index, long tag_index, s_vibration_curve_set *curves, real time)
{
	s_vibration_object_view *object = ((s_vibration_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_vibration_view *vibration = (s_vibration_view *)((byte *)object + object->vibration_offset);
	s_vibration_definition_view *definition = (s_vibration_definition_view *)g_4e3b44[object->definition_index & 0xffff].bytes;
	real blend = 0.95f;

	if (definition->blend > 0.0f)
	{
		blend = definition->blend;
	}
	vibration->tag_index = tag_index;
	if (vibration->index != NONE)
	{
		bool skip_special = false;

		if (((s_vibration_globals_view *)g_4de2f4)->enabled && function_11b930(object_index))
		{
			skip_special = true;
		}
		function_10de40(curves, (s_vibration_output *)vibration->output, time, blend, skip_special);
	}
}

#include "unknown_1cafc0.h"

struct s_vibration_sample_view
{
	byte unknown00[2];
	byte countdown;
	byte unknown03[0x30 - 3];
	c_type_709360 base_animation;
	c_type_709360 weighted_animation;
	byte unknown38[8];
	real frame;
	char frames[12];
	char signed_weights[12];
	c_type_709360 animation_id;
	byte unknown60[0x6e - 0x60];
	byte weights[13];
};

extern real g_54764c;
struct s_anim_data;
void function_20adf0(s_anim_data *data, byte *dest);

// @retail 0x10ec00
void function_10ec00(long object_index, s_animation_state *state, long unused, dword const *node_mask,
	long node_count, real_quaternion_transform *transforms, byte *dest)
{
	long const &unused_reference = unused;
	(void)&unused_reference;
	s_vibration_object_view *object = ((s_vibration_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_vibration_sample_view *vibration = (s_vibration_sample_view *)((byte *)object + object->vibration_offset);
	c_type_709360 animation_id = vibration->animation_id;
	if (animation_id.index != NONE)
	{
		c_animation_channel channel;
		if (state->channel_play(&channel, animation_id, 0x40))
		{
			s_animation *animation = NULL;
			if (channel.animation_id.index != NONE)
				animation = function_1daea0(graph_tag_get(channel.graph_tag_index), channel.animation_id);
			bool sampled = false;
			for (long i = 0; i < 13; i++)
			{
				real frame = i * 2.0f + vibration->frame;
				if (frame >= 0.0f && frame < animation->frame_count && vibration->weights[i])
				{
					real weight = vibration->weights[i] * g_54764c;
					channel.set_frame_position(frame);
					channel.sample(weight, node_mask, node_count, transforms);
					sampled = true;
				}
			}
			if (sampled)
			{
				struct s_output_10ec00 : s_animation_data
				{
					__forceinline void write_10ec00(byte *out)
					{
						function_20adf0((s_anim_data *)this, out);
					}
				};
				s_output_10ec00 data;
				function_1ddb40(&data, graph_tag_get(channel.graph_tag_index), channel.animation_id);
				data.write_10ec00(dest);
			}
		}
	}
}

#include <math.h>
extern real g_547648;
void c_animation_channel_data_get(c_animation_channel const *channel, s_animation_data *data);

// score 207 (was 207): Frame 0x30 versus 0x2c, transforms/ordinal homes and getter expansion remain.
// Round 87: the cause is our inlining of 0x1c6440 in the weighted arm (retail calls it there and in the
// third arm). With the getter out of line (noinline in its header, diagnostic only) the entry and the first
// arm match retail (edi is the zero register, transforms is read from the stack): 174 at 940 bytes. Its
// inlining here did not move with an extra getter use in the first arm, a shrunken third arm or the third
// arm alone (which calls everything out of line even by itself). Tried: data write through a forceinline
// wrapper 207; early return when the weighted play fails 207; unused reference dropped 212 (ret 0x14);
// weight products global-first/grouped/frame-first 207/263/207; long sample frame used for the compare
// and the conversion 257 (aligned 99/91 against 109/110).
// Lane 8 r64: countdown weight with the `>= 3` case first (retail's fall-through) 215, kept; frame
// byte in a long local 260 (32-bit compare as retail, aligned 119, 969 bytes); char/word loop index
// 215 (identical: the loop stays a pointer plus spilled countdown); sampled flag declared before
// the getter result 285 (994 bytes); weight as `frame * (weight byte * scale)` 215. Diagnostic only: 0x1c6440
// declared noinline gives 940 bytes and 166 (with the long frame 215, aligned 94), but matched
// callers such as 0x1c6500, 0x1c6560 and 0x1ccb40 inline it, so the header was restored.
// and spilled induction remain; native-byte sample frame 221; short sample frame 261;
// native-byte sample frame 221; short sample frame 261;
// one tag frame count 255; input cursors 273; signed-byte sampled result 221;
// signed-byte ordinal 221; native frame/count 255; cursors/sampled result 273.
// Earlier: destination and induction homes plus getter expansion remain;
// successful-sample count 274; equality bound 221; frame cursor 283;
// weight cursor 284; end-controlled input cursors 265; frame guard before weight 266;
// weight-magnitude first 266; actual animation reference 221.
// Round 230: Read-only sample/tag/both views 221/221/221; reused weight scale 284;
// scale with either/both views 284/284/284; full input views with named conversion product 284.
// Round 164: Base identifier confined, separate base admission, both, separate
// weighted admission and its three combinations 221 each; complete countdown
// admission split 221. Original playback graph and all constructor points restored.
// Earlier at 221 (from 221): destination and induction homes plus getter expansion remain;
// Positive/countdown/touch order 289; touch/positive/countdown 276;
// touch/countdown/positive 304; countdown/positive/touch 341; countdown/touch/positive
// 351; positive/touch, touch/positive and countdown/positive guard exits 284/276/341.
// Negative no-samples bool/byte/short/long in weighted scope 221/221/222/221;
// across the positive playback phase 313/313/328/331. Original sampled flag and constructors restored.
// Round 150: Data producer/exporter inline contracts with direct/shared data acquisition
// 221/221/221/457/221/457; explicit out-of-line provider pair 221, with shared acquisition 221. Native
// long/dword/short/byte sample ordinal initialized before channel setup or at function entry 221 each.
// Complete original body and provider declarations restored; frame and getter expansion remain.
// Round 234: Last successful sample ordinal in long/short/signed-byte/dword
// at the original result lifetime 268/277/316/268; initialized before the getter
// 296/302/323/296. Frame and destination/ordinal allocation remain. Original
// Boolean result, all channel/data constructors and executable body restored.
// Scoped output names in seven combinations 221 each; with separate channel names 221.
// Common final publication bool/byte/short/long 301/301/300/299;
// with touch admission before construction 343/343/346/341. Original restored.
// Lane 8 r58: weighted id read into the animation id local / its own local 301/301; frame
// byte in a long local for the compare and the conversion 260; both 280; channel data getter
// call in the first two arms 221 (identical code, kept); with id local and long frame 280.
// The loop's pointer/countdown spill is the extra frame dword; retail indexes by i.
// Native unsigned-long/word induction with a full-width frame before/after weight 260 each;
// signed/unsigned 64-bit induction with that frame 256/249 at either boundary, recovering frame 0x2c.
// Signed 64-bit direct/char/short frames 295/295/256; unsigned 288/288/244; unsigned direct frame
// with byte/short sampled result 288/292. Extra loop length regresses; all restored.
// Round 60: ours inlines the weighted arm's animation getter call (retail calls it) and tail-merges
// the second arm's data/publish calls into the first arm's (retail keeps both). Weight product
// reordered, short and int index 221 each (identical); weighted id copied into the id local or its
// own local 301/301 (the tail un-merges as retail but the state pointer leaves EBP).
// Round 195: Native channel copy initialization and default-assigned identifier subsets [215, 215, 215,
// 215, 215, 215, 215]; joint construction with sample zero before getter 285. Original independent
// constructors restored; frame, destination/ordinal homes and weighted getter expansion remain.
// Round 201: A single frame/weight-array cursor with paired index and char frame gives 248 each
// with either end test; long frames give 227 each, recovering frame 0x2c but growing to 966 bytes.
// On that recovered-frame graph, shared/separate weighted identifiers, each with either countdown
// arm order, give 244/244/244/244. Original index loop, identifiers and constructors restored.
// Round 360: independent data constructors before base/weighted/countdown admission,
// pairs and all three 263/207/231/290/278/223/281; all with a long sample frame 279.
// Weighted output after admission/getter/result-zero/loop 213/213/261/215;
// before admission at its natural positive-phase scope 207 (950 bytes), selected.
// Frame 0x30, getter expansion and pointer/countdown induction remain.
// Round 239: fixed frames/weights/both array bases 250/250/287; channel lookup at each
// frame-count test 284, with frames/weights/both bases 262/262/319; both bases and
// lookup with full-width sample frame 300. Array bases recover frame 0x2c but regress;
// original body, weighted-output lifetime and all separate constructors restored.
// Round 225: bounded ordinal stepped at its input read with char/short/long sample frames
// 256/251/250; with actual sample count 264/260/259; full-width frame before weight 250,
// with count 259. Weighted output construction remains before admission; original loop restored.
// Round 364: independent data outputs at entry, base/weighted/countdown and
// pairs/all 295/304/304/294/283/304/290; all with long sample frame 312.
// Original separate construction points restored; frame and getter expansion remain.
// Native frame-array references, mutable/const with extent bounds, 250 each at 940 bytes; with
// twice-used full-width frames 222 each at 964 bytes. Frame 0x2c recovered, original loop and
// observations restored.
// Round 241: Sample occupancy dword/word at weighted scope 301/299; before getter 301/299; with a
// twice-used full-width frame 273/274; at positive entry 331/329. All masks and original body restored.
// Round 207: distinct weighted channel with native copy/default-assignment/member copy
// 308/308/380; each with twice-used long sample frame 332/332/375;
// copy/member copy with native sample-result enum 316/390. All exact-match-only
// storage forms restored; frame 0x30, getter expansion and ordinal spill remain.
// Round 222: Native datum accessor/direct object/both 207/207/207; record at each use, with accessor 411/411; direct object and record, with accessor 674/674; all with promoted sample frame 648. All restored; frame 0x30 versus 0x2c, weighted getter and ordinal spill remain.
// Round 212: complete native weight-array identity, mutable/const, with original/complete
// frame-array identity and literal/native bounds [250, 250, 287, 287, 250, 250, 287, 287].
// Original array observations restored; getter, destination and frame remain.
// Round 246: signed-char frame/weight/both fields 207/207/207; each with a twice-used long
// frame 257/257/257; both with short frame/immutable frame-count snapshot 258/254. Original
// field identities, observations and body restored.
// Native complete weighted input value, mutable/const before/after the getter,
// 346 each; each with a twice-used full-width sample frame 344. Original record
// observations, arithmetic, separate constructors and full body restored.
// Round 72: weight byte * scale named, then * frame 207 (retail's multiply order); frame byte in a
// long local for compare and conversion 257 (retail's 32-bit compare, 967 bytes); both 257;
// reversed frame-count compare 207; weight from the entry frame local 309.
// DH round 230: Post-getter countdown snapshot in byte/short/long/word domains 207/216/280/215;
// the same complete if/else weight producers 207/216/280/215. Original observations and body restored;
// frame 0x30/0x2c, destination/ordinal homes and weighted getter expansion remain.
// DH round 236: positive channel constructed at entry with independent countdown-channel,
// real ordinal and existing sample-result entry lifetimes, eight subsets
// 279/375/279/375/317/391/317/391. Original separate constructions, observations and body restored;
// frame 0x30/0x2c, destination/ordinal homes and weighted getter expansion remain.
// Round 398: actual eligibility retained for sampling and loop-result accumulation,
// bitwise bool/byte/short/long 306/306/303/304; logical
// accumulation with those carriers 324/324/314/310. Original result and body restored.
// Round 186: double storage for initial/weighted/both/countdown/initial-countdown/weighted-countdown/all
// results 285/326/352/290/314/305/330; weighted double with promoted sample frame 335.
// Original scalar storage and body restored; frame, getter expansion and ordinal spill remain.
// Round 297: On early weighted-output construction, frame rejection/nested
// frame-first/both rejections 207 each; magnitude rejection/nested magnitude-first/
// reverse rejections/reversed shared admission 262 each (frame 0x2c, 927 bytes);
// combined frame-or-negligible rejection 207. Original loop and full body restored.
// Round 225: fabsf with mutable/immutable weight, native real/double absolute
// values, mutable/immutable and with immutable weight: 207 each. Original
// absolute expression, frame tests and body restored; getter/ordinal/frame remain.
// DL202: Native external frame/weight table direct/pointer/const-pointer/per-iteration pointer/reference/
// const-reference 207/250/250/250/250/250; pointer/const-pointer with twice-used long sample
// frames 222/222. Input aliases recover frame 0x2c but regress; original table fields,
// observations, constructors and body restored. Destination/ordinal homes and getter expansion remain.
// Round 241: early weighted output with long/dword/short/word contribution counts
// 275/275/276/276; before getter 275/275/276/276. All result-storage diagnostics restored;
// original Boolean, separate constructors, frame 0x30/0x2c and getter/ordinal differences remain.
// Round 460: Positive admission bool/byte/short/long, then paired positive/nonzero admissions,
// 207/207/207/207/343/343/337/337. Original direct frame tests, constructors and body restored;
// frame 0x30/0x2c, weighted getter and ordinal homes remain.
// Round 266: existing sample success before frame positioning or between positioning/sampling,
// each in bool/byte/short/long storage, 276/276/279/278/276/276/279/278. Original result stores,
// constructors and complete body restored; frame, getter and ordinal differences remain.
// Round 270: Native countdown identifier copy/assignment, with direct base, weighted
// identifier and both input policies: 296/296/284/284/327/327/301/301. Original
// identifiers, separate constructors and body restored; frame, getter and ordinal homes remain.
// Round I (MiniMax lane I, fast): the getter called through a member-function pointer local (only to keep
// it out of line, which the noinline ruling rules out) with the twelve samples walked by a frames pointer
// that reads each weight as sample[12] 137 (frame 0x2c as retail's); the getter called directly with that
// walk 199 (the weight read past the frames array is a pun: not kept); a second pointer over signed_weights
// 269. On the 137 form: an index loop over the two arrays 174-216, an int frame compare 185, the data struct
// in the sampled block 189. Original body restored.
// Round 41 (ours ends `ret 0x18` as retail): the weighted getter's inlining is local to its call site: it stays
// inlined with arm 1 removed (421) or arm 3's getter removed (218), and goes out of line (as retail, which also
// un-merges the second publish tail) when its call sits under one more condition, even a redundant one the
// optimizer folds: a repeated `weighted.index != NONE` guard around it 227 (943 bytes, entry and first arm then
// retail's but for the 0x30 frame); with the data struct in the sampled block 210; plus a twice-used long frame
// 201 (940 bytes; left: the loop's pointer plus spilled countdown, frame-first product, `mov eax, edi` where
// retail recomputes `lea eax, [esp+0x14]` after 0x1ddb40/0x1c7380). Not kept (a repeated test, no match).
// Plain forms, getter still inlined: nested ifs for the weighted admission 207; `return` when it fails 207;
// separate weighted channel 360; weighted id in a twice-used local 306; animation declared at the positive
// block's top 207; sampled flag before the getter 276; data struct in the sampled block 215; the frame read
// from the field at both tests 260 (retail's reload at the second test, 955 bytes).
// Round 111 (lane 7, fast): one function-scope animation pointer for the weighted and countdown getters 207;
// the frame field read at both tests (no frame local) 260 (955 bytes); the base id read from the field (no id
// local) 256 (940 bytes, getter still inlined); the weighted play as a nested test returning on failure 207;
// the weight test as `fabs(weight) >= 0.0001f` 257. Retail calls 0x1c6440 out of line in both arms yet inlines
// the data getter after the weighted loop, so its refusal is not a plain size budget taken in source order.
// Round 227: Target inline/stdcall/both 207 each; channel getter, frame setter and sampler inline
// singly, getter/setter and getter/sampler pairs 207 each; complete playback inline declarations 207.
// All declarations and original bodies restored; weighted getter expansion and spilled induction remain.
// Round 275: Real two-identifier touch loop with four native channel initializations 268 each; all with long sample frame 202 each (five-point lead rejected); pointer traversal with bool/long flag/long count/short count 224/231/223/222.
// Original executable source restored; frame 0x30 versus 0x2c, getter expansion and ordinal homes remain.
// Round 277: Twice-used real/double sample frame, direct/named weight product 273/273/289/289; each
// with publication-owned weighted output 281/281/300/300. Original frame observations, constructors
// and body restored; frame, getter expansion and ordinal homes remain.
// Round 131 (m3 lane 6, fast; both builds now end `ret 0x18`): diagnostic, the first arm written twice 496
// (1120 bytes) and the weighted arm still inlines the getter and the data get, so the getter's inlining does
// not follow a budget used up by the first arm. The getter called in the loop condition 284 (992 bytes); a
// const animation pointer 207; the existing channel validity test 0xb6760 after the weighted play 239 (954
// bytes); in 0x1c6440 itself, its graph in a named local 207 (getter still matches), an early NULL return
// 207 (getter 15).
// Round 275: Real two-identifier touch loop with four native channel initializations 268 each; all with long sample frame 202 each (five-point lead rejected); pointer traversal with bool/long flag/long count/short count 224/231/223/222.
// Original executable source restored; frame 0x30 versus 0x2c, getter expansion and ordinal homes remain.
// Round 277: Twice-used real/double sample frame, direct/named weight product 273/273/289/289; each
// with publication-owned weighted output 281/281/300/300. Original frame observations, constructors
// and body restored; frame, getter expansion and ordinal homes remain.
// Round 294: Per-iteration native count with twice-used long/short sample frames,
// long/64-bit counts and before/after-weight count acquisition gives 257 each for
// long frames, 258 each for short. Exact-only bound copies and full body restored.
// @retail 0x10e530
void function_10e530(long object_index, s_animation_state *state, long unused, dword const *node_mask,
	long node_count, real_quaternion_transform *transforms, byte *dest)
{
	long const &unused_reference = unused;
	(void)&unused_reference;
	s_vibration_object_view *object = ((s_vibration_object_header *)g_4e0300->data)[object_index & 0xffff].object;
	s_vibration_sample_view *vibration = (s_vibration_sample_view *)((byte *)object + object->vibration_offset);
	real frame = vibration->frame;
	if (frame > 0.0001f)
	{
		c_animation_channel channel;
		c_type_709360 animation_id = vibration->base_animation;
		if (animation_id.index != NONE && state->channel_play(&channel, animation_id, 0))
		{
			channel.set_frame_position(0.0f);
			channel.sample(vibration->frame, node_mask, node_count, transforms);
			s_animation_data data;
			c_animation_channel_data_get(&channel, &data);
			function_20adf0((s_anim_data *)&data, dest);
		}
		s_animation_data data;
		if (vibration->weighted_animation.index != NONE && state->channel_play(&channel, vibration->weighted_animation, 0))
		{
			s_animation *animation = channel.function_1c6440();
			bool sampled = false;
			for (long i = 0; i < 12; i++)
			{
				real weight = vibration->signed_weights[i] * g_547648 * vibration->frame;
				if (vibration->frames[i] < animation->frame_count && !(fabs(weight) < 0.0001f))
				{
					channel.set_frame_position((real)vibration->frames[i]);
					channel.sample(weight, node_mask, node_count, transforms);
					sampled = true;
				}
			}
			if (sampled)
			{
				c_animation_channel_data_get(&channel, &data);
				function_20adf0((s_anim_data *)&data, dest);
			}
		}
	}
	else if (frame != 0.0f)
	{
		if (vibration->base_animation.index != NONE)
			state->animation_touch(vibration->base_animation);
		if (vibration->weighted_animation.index != NONE)
			state->animation_touch(vibration->weighted_animation);
	}
	else if (vibration->weighted_animation.index != NONE && vibration->countdown > 0 && vibration->countdown < 7)
	{
		c_animation_channel channel;
		if (state->channel_play(&channel, vibration->weighted_animation, 0) && channel.function_1c6440()->frame_count > 17)
		{
			real weight = vibration->countdown >= 3 ? (7 - vibration->countdown) * 0.25f : vibration->countdown * (1.0f / 3.0f);
			channel.set_frame_position(17.0f);
			channel.sample(weight, node_mask, node_count, transforms);
			s_animation_data data;
			c_animation_channel_data_get(&channel, &data);
			function_20adf0((s_anim_data *)&data, dest);
		}
	}
}
