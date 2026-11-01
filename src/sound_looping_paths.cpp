// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "globals.h"
#include "unknown_0259d0.h"
#include <math.h>

struct s_portal_path_bsp;
struct s_14b240_owner;
struct s_bsp3d_disk;
void __stdcall function_18d730(s_portal_path_bsp *bsp, long cluster, point3f const *point,
	real range, long *indices, long *gains, long *portals, long *clusters, long *count);
real function_14b240(s_14b240_owner const *owner, s_bsp3d_disk const *disk, point3f const *point);
extern real g_44a0b4;
extern real g_4670fc;
extern long g_509420;
struct s_unknown_13bf00;
extern s_unknown_13bf00 *g_510c50;

struct s_loop_path_sound
{
	byte field_0[0x24];
	long tag;
	long field_28;
	long detail_tag;
	byte field_30[0x44 - 0x30];
	real range;
	dword flags;
	real inside;
	real portal;
	real outside;
	real fade_time;
	byte field_5c[8];
};

struct s_loop_path_bsp
{
	byte field_0[0x60];
	byte *portals;
	byte field_64[0xd8 - 0x64];
	s_loop_path_sound *sounds;
};

struct s_loop_path_choice
{
	real distance;
	real inside;
	real portal;
	real outside;
	long source;
	long tag;
	long cluster;
	real gain;
	long decibels;
	bool muted;
};

struct s_loop_path_target
{
	long tag;
	real gain;
	real step;
	long decibels;
	long priority;
	long cluster;
	long source;
	bool audible;
	bool muted;
};

struct s_loop_path_datum
{
	short salt;
	byte state;
	byte type;
	word flags;
	short field_6;
	real gain;
	long tag;
	long decibels;
	char source;
	char listener;
	short cluster;
};

struct s_loop_path_override
{
	byte field_0[0x3c];
	long tag;
	byte field_40[8];
	real suppression;
	real gain;
};

PRIVATE inline real loop_path_pin(real value, real lower, real upper)
{
	return lower > value ? lower : (value > upper ? upper : value);
}

__forceinline long loop_path_decibels(real value)
{
	if (value < g_44a0b4) return 0xc2800000;
	if (value > 0.0f) return 0;
	return *(long *)&value;
}

__forceinline real loop_path_linear(long value)
{
	long bits = loop_path_decibels(*(real *)&value);
	return (real)exp(*(real *)&bits * 0.05f * 2.3025851f);
}

__forceinline long loop_path_logarithmic(real value)
{
	real pinned = loop_path_pin(value, 0.0f, 1.0f);
	real decibels = g_44a0b4;
	if (value > 0.0f)
	{
		decibels = (real)(log10(pinned) * 20.0f);
		decibels = loop_path_pin(decibels, g_44a0b4, 0.0f);
	}
	return loop_path_decibels(decibels);
}

PRIVATE inline s_loop_path_datum *loop_path_get(s_record_pool *pool, long index)
{
	return &((s_loop_path_datum *)pool->data)[index & 0xffff];
}

__forceinline long loop_path_start(s_record_pool *pool, long tag, real gain)
{
	long index = NONE;
	if (tag != NONE)
	{
		index = record_pool_allocate(pool);
		if (index != NONE)
		{
			s_loop_path_datum *sound = loop_path_get(pool, index);
			sound->tag = tag;
			sound->state = 3;
			sound->flags = 0;
			sound->flags |= 0x100;
			sound->field_6 = NONE;
			sound->type = 0;
			sound->decibels = 0;
			sound->source = (char)0xff;
			sound->cluster = NONE;
		}
		if (index != NONE)
		{
			loop_path_get(pool, index)->flags |= 1;
			loop_path_get(pool, index)->gain = gain;
		}
	}
	return index;
}

// score 886 (was 886): frame 0x4e0 agrees; entry zero, target/primary/query-array homes and choice carriers remain.
// Earlier 919 (was 919): count/array homes and choice registers differ in the 0x4e0 frame;
// count/array homes and choice registers differ in the 0x4e0 frame;
// Frame 0x4e0 agrees; entry zero, target/primary/query-array homes and choice registers remain.
// choice pointer/post-increment 931; byte choice count 922; duplicate break 959;
// unsigned target count 919; used-mask memset 919.
// frame 0x4e0 agrees; target/primary and query-array homes differ.
// Existing-count word 939; query-cursor word 927; completed append 929; early primary 940; primary word 925.
// Earlier 886 (was 919): Frame 0x4e0 agrees; entry zero, target/primary/query-array homes and
// Frame 0x4e0 agrees; entry zero, target/primary/query-array homes and
// choice register remain. byte target count 934; short query count 886; byte query count 914;
// byte query cursor 927; byte duplicate cursor 928; short query with byte target count 932;
// short query with byte query cursor 935; short query with byte duplicate cursor 922; short
// query with long choice count 957.
// Round 150: clamp result 952; decibel result 907; direct target gain 925; shared target step 889; live definition table 918; logarithm failed exit 1110; both clamp results 930; direct gain with shared step 920 (fast).
// First/second/both query pointer walks 944/941/951; duplicate/conversion choice pointer walks 947/1008; target pointer walk 899; existing-index pointer walk 919; unused local mask removed 904.
// With retained short query count: word first query ordinal 915; word second query ordinal 898;
// word choice-conversion ordinal 923; word duplicate ordinal 929 (retail length); all four word ordinals 894.
// Unsigned word query/choice/both counts 886 each; dword/byte/word inside result 921/886/910;
// unsigned first-query/duplicate ordinals 886/886. Original executable source restored.
// Choice/primary flag observations 886/886; shared neighboring gain 959; both flags 886;
// choice flags+gain 959; primary flags+gain 959; all 959; early choice flags with all 953. All restored.
// Round 126: Named squared blend term 886; repeated portal endpoint 909; existing pin/get forceinline
// 886/886; single successful allocation guard 905; choice-cap test first 932; accumulated target gain 886;
// default-first detail result 884 (fast, gain below threshold); all restored.
// Round 180 current gain active/inactive/outside state/both gains/active-outside/inactive-outside/all/byte
// outside 890/889/887/893/891/890/894/887 (fast); all restored. Frame 0x4e0 agrees; entry zero,
// query-array/count and primary homes remain.
// Native floating decibel fields/conversions 998 (frame 0x4dc); all seven ordinary-
// inline conversion-helper combinations 998 each. Original integer-bit fields,
// helpers, query boundary and complete body restored.
// Datum consumed-bit/selection bits/both 886/886/886; audible/muted bit assignments alone and with
// consumed/selection/both 911/911/911/911; all with allocator bits 911. Original word view and statements restored.
// Round 369: short-query graph choice pointer early/late count 962/922; all query-array names and gains/portal/cluster names individually 886 each; early/late pointers with all names 962/922. All restored.
// Query arrays, matching records and inactive-pass workspaces ended at their real phase
// boundaries, all seven subsets and producer-count ending after snapshot: 886 each.
// Round 224: shared inline iterator first/second/both 968/1007/1103; shared calling iterator
// first/second/both 905/915/899; real iterator-step first/second 953/919. Original scans restored.
// Round 385: Existing allocator sentinel/zero/final-flag initialization, all seven subsets 886 each;
// all with exhaustive allocation result 890. Original helper and body restored; entry zero,
// target/primary/query-array homes and choice carriers remain.
// DL167: Existing-record lookup from completed long/dword/short/word search ordinal
// 892/892/893/893; each with a repeated target-tag snapshot 893/893/906/906. All
// original scans, result publication, fields and helpers restored (fast/full control 886).
// Round 249: native choice-muted/target-audible/target-muted byte domains, seven
// subsets 915/890/886/898/886/890/890; all signed bytes 890. Original bool fields,
// record layouts, complete body and every gain expression restored.
// Round 225: target-step snapshot 889; target-tag snapshot 886; both 894 at retail length;
// live pool 886; step/pool 889; tag/pool 886; all 894; all with direct computed gain 894.
// Original target observations, pool snapshot and computed gain local restored.
// Round 233: Common override attenuation, readonly override metadata and native
// used-bit expression in seven subsets, then complete used-word expression: 982/886/982/886/982/886/982/982.
// Common-result forms require a match; all original producers and body restored.
// Round 407: completed choice-duplicate ordinal long/dword/short/word, each with
// original/repeated-tag snapshot on the short-query graph 948/948/948/948/947/947/934/934. All restored.
// Round 415: existing hoisted-bound first/second pool scans and shared salted-index
// producer, seven subsets 886/886/886/924/924/924/924; all with forced datum getter 924.
// Original scanners, identifier expressions, getter and complete body restored.
// Round 424: positive uniqueness bool/byte/short/long decisions 886 each;
// each with default-first detail completion 884 (two-point gain rejected).
// Original duplicate result, detail expression and complete body restored.
// Round 434: complete native choice/converted-target publication and applied-target
// value, seven subsets 937/947/925/945/938/953/926; all with CRT publication 926.
// Original records, field computations, query counts, gain trees and body restored.
// Round 238: reused active flag/active gain/inactive gain field lvalues, seven
// subsets 886 each; all with flag binding at its first update 886. Original
// field accesses, helpers, scans and complete body restored.
// Round 249: Complete sound/portal/default table snapshots, masks 1..7,
// 970/937/973/948/982/924/987; all with query metadata before its producer 988.
// Original observations and complete body restored; exact-length 924 has the wrong frame.
// Round 470: unsigned raw decibel words in choice/target/datum records, all
// seven subsets 886 each; all with unsigned helper transport 886. Original
// records, helpers and body restored; entry zero and target/query/count homes remain.
// Round 54 (m3 lane 1, fast): retail reads the query count straight from the out-parameter as a dword and keeps
// the choice count in ESI as a dword (the entry zero in ESI becomes the count; `cmp esi, 2`): the count read
// directly with a long choice count 927 (3297 bytes; aligned differing lines 700 -> 585), with the short choice
// count 919; on the long counts `s_loop_path_choice *choice = &choices[choice_count++]` with retail's store
// order (tag, source, decibels, cluster) 960 (our choice address becomes an induction variable; retail
// recomputes it from the count); the three 1.0f defaults as statements in retail's store order (inside, portal,
// outside; the chained assignment stores them reversed) 886, with the long counts 927. Array names (two full
// prefix orders) and declaration order leave the four query-array homes unchanged (retail: gains +0xec, indices
// +0x1f0, portals +0x2f0, clusters +0x3f0; ours portals, indices, clusters, gains).
// @retail 0x18ae80
void __stdcall function_18ae80(s_portal_path_bsp *bsp_data, long listener, point3f const *point,
	long cluster, long override_tag, real elapsed)
{
	s_loop_path_bsp *bsp = (s_loop_path_bsp *)bsp_data;
	s_loop_path_target targets[3];
	long target_count = 0;
	long primary = NONE;
	if (cluster != NONE)
	{
		long indices[64], gains[64], portals[64], clusters[64];
		long count = 0;
		function_18d730(bsp_data, cluster, point, 20.0f, indices, gains, portals, clusters, &count);
		short query_count = count;
		s_loop_path_choice choices[2];
		short choice_count = 0;
		for (long i = 0; i < query_count && choice_count != 2; i++)
		{
			long index = indices[i];
			if (index == NONE) continue;
			s_loop_path_sound *sound = &bsp->sounds[index];
			if (sound->tag == NONE) continue;
			bool duplicate = false;
			for (long j = 0; j < choice_count && !duplicate; j++)
				duplicate = choices[j].tag == sound->tag;
			if (duplicate) continue;
			choices[choice_count].source = index;
			choices[choice_count].tag = sound->tag;
			choices[choice_count].cluster = clusters[i];
			choices[choice_count].decibels = gains[i];
			if (sound->flags & 1)
			{
				choices[choice_count].inside = sound->inside;
				choices[choice_count].portal = sound->portal;
				choices[choice_count].outside = sound->outside;
			}
			else choices[choice_count].inside = choices[choice_count].portal = choices[choice_count].outside = 1.0f;
			if (sound->flags & 8)
			{
				choices[choice_count].inside *= g_4670fc;
				choices[choice_count].portal *= g_4670fc;
				choices[choice_count].outside *= g_4670fc;
				choices[choice_count].muted = g_509420 == 0;
			}
			else choices[choice_count].muted = false;
			choices[choice_count].distance = portals[i] != NONE ? function_14b240((s_14b240_owner *)bsp,
				(s_bsp3d_disk *)(bsp->portals + portals[i] * 0x24), point) : 0.0f;
			choice_count++;
		}
		bool inside = cluster == clusters[0];
		if (query_count > 1 && choice_count && inside)
		{
			s_loop_path_sound *sound = &bsp->sounds[choices[0].source];
			for (long i = 1; i < query_count; i++)
			{
				long index = indices[i];
				if (index != NONE && bsp->sounds[index].tag == choices[0].tag)
				{
					choices[0].distance = function_14b240((s_14b240_owner *)bsp,
						(s_bsp3d_disk *)(bsp->portals + portals[i] * 0x24), point);
					if (sound->flags & 4) choices[0].outside = bsp->sounds[index].flags & 1 ? bsp->sounds[index].inside : 1.0f;
					if (sound->flags & 2) choices[0].portal = bsp->sounds[index].flags & 1 ? bsp->sounds[index].inside : 1.0f;
					break;
				}
			}
		}
		s_loop_path_sound *definitions = *(s_loop_path_sound **)((byte *)g_4e0348 + 0xd8);
		for (long i = 0; i < choice_count; i++)
		{
			s_loop_path_choice *choice = &choices[i];
			real range = bsp->sounds[choice->source].range;
			real fraction = fabs(-range) < 0.0001f ? (choice->distance < 0.0f ? 0.0f : 1.0f) : loop_path_pin(choice->distance / range, 0.0f, 1.0f);
			real blend = loop_path_pin(fraction * fraction * 3.0f - fraction * fraction * fraction * 2.0f, 0.0f, 1.0f);
			choice->gain = ((inside ? choice->inside : choice->outside) - choice->portal) * blend + choice->portal;
			s_loop_path_target *target = &targets[target_count++];
			target->tag = choice->tag;
			target->gain = choice->gain;
			target->decibels = choice->decibels;
			target->audible = i == 0 && (!g_510c50 || !((byte *)g_510c50)[5] || g_4ed288->flag240);
			target->priority = i;
			target->cluster = choice->cluster;
			target->source = choice->source;
			target->step = elapsed / (fabs(definitions[choice->source].fade_time) < 0.0001f ? 2.0f : definitions[choice->source].fade_time);
			target->muted = choice->muted;
		}
		if (inside && choice_count) primary = choices[0].source;
	}
	if (override_tag != NONE)
	{
		s_loop_path_override *definition = (s_loop_path_override *)g_4e3b44[override_tag & 0xffff].data;
		for (long i = 0; i < target_count; i++)
		{
			long gain = loop_path_logarithmic(1.0f - definition->suppression);
			*(real *)&targets[i].decibels += *(real *)&gain;
		}
		if (definition->tag != NONE)
		{
			s_loop_path_target *target = &targets[target_count++];
			target->tag = definition->tag;
			target->gain = 1.0f;
			target->decibels = loop_path_logarithmic(definition->gain);
			target->audible = true;
			target->priority = 2;
		}
	}
	s_record_pool *pool = g_4ed28c;
	long existing[64];
	long existing_count = 0;
	dword used[2] = { 0, 0 };
	for (long i = data_next_absolute_index_inlined(pool, 0); i != NONE; i = data_next_absolute_index_inlined(pool, i + 1))
	{
		s_loop_path_datum *sound = (s_loop_path_datum *)(pool->data + pool->size * i);
		if (sound->type == 0 && sound->source != NONE)
		{
			sound->flags &= ~0x200;
			existing[existing_count++] = (sound->salt << 16) | i;
		}
	}
	for (long i = 0; i < target_count; i++)
	{
		s_loop_path_target *target = &targets[i];
		long index = NONE;
		for (long j = 0; j < existing_count; j++)
		{
			if (existing[j] != NONE && loop_path_get(pool, existing[j])->tag == target->tag)
			{
				index = existing[j];
				used[j >> 5] |= 1 << (j & 31);
				break;
			}
		}
		if (index == NONE)
		{
			index = loop_path_start(pool, target->tag, target->gain);
			if (index != NONE) loop_path_get(pool, index)->decibels = 0xc2800000;
		}
		if (index != NONE)
		{
			s_loop_path_datum *sound = loop_path_get(pool, index);
			real current = loop_path_linear(sound->decibels);
			real delta = loop_path_linear(target->decibels) - current;
			real gain = current + loop_path_pin(delta, -target->step, target->step);
			sound->decibels = loop_path_logarithmic(gain);
			sound->flags |= 0x200;
			if (target->audible) sound->flags &= ~0x80;
			else sound->flags |= 0x80;
			if (target->muted) sound->flags |= 0x10;
			else sound->flags &= ~0x10;
			sound->listener = (char)g_4686c4;
			sound->cluster = (short)target->cluster;
			sound->gain += loop_path_pin(target->gain - sound->gain, -target->step, target->step);
			sound->source = (char)target->source;
		}
	}
	s_loop_path_sound *definitions = *(s_loop_path_sound **)((byte *)g_4e0348 + 0xd8);
	long scan_index = NONE;
	while ((scan_index = data_next_absolute_index_inlined(pool, scan_index + 1)) != NONE)
	{
		s_loop_path_datum *sound = (s_loop_path_datum *)(pool->data + pool->size * scan_index);
		long index = (sound->salt << 16) | scan_index;
		if (sound->type == 0 && sound->source != NONE && !(bool)(((dword)sound->flags >> 9) & 1))
		{
			real step = elapsed / (fabs(definitions[sound->source].fade_time) < 0.0001f ? 2.0f : definitions[sound->source].fade_time);
			real gain = loop_path_linear(sound->decibels);
			gain += loop_path_pin(0.0f - gain, -step, step);
			sound->gain += loop_path_pin(0.0f - sound->gain, -step, step);
			if (fabs(gain) < 0.0001f) loop_path_get(pool, index)->flags |= 2;
			else sound->decibels = loop_path_logarithmic(gain);
		}
	}
	long detail = primary == NONE ? NONE : bsp->sounds[primary].detail_tag;
	long index = g_4ed288->indices[listener * 2];
	if (index != NONE && loop_path_get(pool, index)->tag != detail)
	{
		loop_path_get(pool, index)->flags |= 2;
		g_4ed288->indices[listener * 2] = NONE;
		index = NONE;
	}
	if (index == NONE && detail != NONE)
		g_4ed288->indices[listener * 2] = loop_path_start(pool, detail, 1.0f);
}

#include "local_cameras.h"
#include "local_players.h"
#include "sound_sources.h"
#include <float.h>
#include <string.h>

struct s_sound_portal_bsp;
struct s_sound_listener_rotation;
struct s_sound_portal_angles
{
	real lower;
	real upper;
	real horizontal_scale;
	real vertical_scale;
};
void function_18ab70(s_sound_portal_bsp const *bsp, long portal_index, s_sound_listener_rotation const *listener,
	bool reverse, point3f const *point, real radius, s_sound_portal_angles *angles);
void __stdcall function_18dc90(s_portal_path_bsp *bsp, long cluster_index, point3f const *point,
	real maximum_range, long *indices, long *gains, long *portals, long *clusters, long *count);

struct s_sound_environment
{
	real transition_time;
	real fade;
	long index;
	s_sound_portal_angles angles;
};
void sound_environments_update(s_sound_environment const *requests);
long function_189fe0(s_sound_request const *request, long tag_index);

struct s_listener_environment_definition
{
	byte field_0[0x24];
	long tag;
	real radius;
	real transition_time;
	byte field_30[0x18];
};
struct s_listener_environment_bsp
{
	byte field_0[0xe0];
	s_listener_environment_definition *environments;
};

PRIVATE inline long sound_first_listener()
{
	long first = NONE;
	for (long i = 0; i < 4; ++i)
		if (g_4e8c20->entries[i] != NONE) { first = i; break; }
	return first;
}

PRIVATE __forceinline void sound_environment_play_cue(long tag)
{
	s_sound_request request;
	request.location.unknown02 = 0;
	request.location.flags = 0;
	request.location.audible = 0;
	request.location.requested_audible = 0;
	request.location.scale = 1.0f;
	request.location.unknown08 = 0;
	request.object_index = NONE;
	request.platform_playback = NONE;
	request.variant = NULL;
	request.marker = NULL;
	request.source = NULL;
	function_189fe0(&request, tag);
}

// score 159 (was 159): frame 0x148 agrees; BSP/count homes +0x10/+0x14
// are exchanged, camera/query and request publication scheduling remain.
// Earlier 154 (was 223): the transition definition read through the local bsp and the missing-query fill
// order; the entry test and scan match retail's, but the position copy (retail loads x, the cluster, y and
// z before it stores), the BSP/count home slots, the cue request tail order and the 0x18ab70 convention remain.
// Earlier 156 (was 223): the transition definition read through the local bsp; the entry test and
// first-listener scan match retail's, but the BSP spill slot, the position copy and the cue requests remain.
// Earlier 159 (was 223): Frame 0x14c versus 0x148; BSP/camera/query homes and duration spill remain.
// MiniMax A (10-31): local_player_exists entry test, the first-listener test before the camera is formed, the
// fill loop storing indices/portals/clusters in that order, tag and transition as plain ifs and the definition
// read through `bsp` 154 with the first listener in a one-use local (not kept); the same as a nested if 159.
// Lane 8 r64 (fast; two callees build differently in the reduced image): existing 0x14ddc0 listener
// test 295 (aligned 150 against 159: retail's `cmp iv, -0x48; setne`); existing first-local-player
// helper alone 235; default-first transition re-read from the definition alone 267; all three 277;
// those with the camera taken after the first-player compare 289 (aligned 142; retail computes the
// camera after the scan). Retail keeps the BSP in esi from entry and listener/stride in ebx/edi.
// Transition result 235; unsigned listener 264; byte listener 319; angle lifetime 264; separate request indices 264.
// Request transition field 263; bounded listener result 223 plain; separate fill guard 255;
// depth one 235; size optimization 441; direct search returns 254.
// Default-first sampled duration 259; default-first field reload 267 (frame 0x148);
// bounded pointer search 225; bounded while search 223; equality exhaustion 255.
// Original body/helper retained; fast preview 223.
// Short helper-search index 289; first-listener result 223; missing-slot store order 223;
// fallback angle value 298; early local tag observation 314; radius value 223;
// portal-side result 223; complete request publication 272.
// native duration 226; read-only position 223.
// position memcpy 223; tag aggregate initializer 270.
// cue sentinel chain 222; cue nibble chain 223.
// both cue chains 222; prior tag on bounded search 223.
// Earlier: BSP/camera homes, transition spill and cue zero carrier remain;
// cue sentinel order 222; cue pointer order 223; cue byte-first zeros 223;
// cue integer-zero/scale order 223; direct transition conditional 253;
// exact listener bound 223; query-index cursor 248; Boolean fallback scale 237.
// Earlier: frame, BSP/camera/query homes and duration spill remain.
// Environment-table lifetime 273; direct duration source 223; cue location subobject 250;
// shared caller cue workspace 229; unsigned query index 223; table/direct duration 273;
// shared workspace/location 233; table/workspace 254 (fast). Shared cue workspace gives frame 0xf4, not retail 0x148.
// Earlier: BSP/camera/query homes, duration spill and cue zero carrier remain.
// BSP/camera/query homes, duration spill and cue zero carrier remain.
// Native BSP view 223; multi-use environment row 267; initialized query count 296;
// duration interval/sign arms 231/229 (frame 0x148); BSP/count 296;
// BSP/interval 231; BSP/count/interval 286.
// Native cluster word 287; readonly camera 223; angle-output name 223;
// early fade/tag stores 270; cluster/camera 287; cluster/angle 287;
// camera/angle 223; combined input/output and earlier stores 285.
// Count-as-fill-cursor for six store orders 264 each; while forms in two orders 264 each.
// Round 164: Paired tag/transition result arms, tag after magnitude, paired
// defaults and defaults/tag-after 223/275/233/276; outer duration workspace
// 223/275/233/276. Original separate metadata graph and all helpers restored.
// Transition after angles with original arms, deferred metadata/direct angles/both 271/290/282/330;
// same four forms with default-first duration 263/278/309/323. Original observation timing, output copies and helpers restored.
// global bsp 275; point setter 240; bsp point setter 296; current at use 225; bsp current 274; setter current
// 318; bsp setter current 305; point initializer 239.
// Round 234: Uniform live duration with environment/portal/cluster query-array
// home names, their three pairs, all three and reversed array declarations: 267
// each, 1020 bytes and frame 0x148. Original sampled duration, helper contracts,
// array names and executable body restored at 223.
// Lane 8 r58: listener test through the existing 0x14ddc0 instead of the camera NULL test
// (retail's `cmp iv,-0x48; setne`) 295; with the existing first-local-player helper 293
// (loop head then retail's); first player in a local before the camera 292; that with the
// default-first transition re-read from the definition 276 (frame 0x148, tags at retail's
// slots; bsp not kept in esi, cluster spilled low); without the local 277; transition alone
// 267; cluster local renamed three ways 276 each.
// Round 180: Cue control/full-prefix clears 256/253; with pointer-tail clear 263/234;
// the same four with chained index sentinels 257/253/263/234. Original cue helper restored.
// Round 356: Cue equality continuation, outer request workspace, actual cue-handle return,
// their three pairs and all three 223 each; combined graph with direct request walk 221 (only -2).
// Original body and cue helper restored; frame and BSP/camera/query homes remain.
// Round 253: Complete primary-listener result bool/byte/short/long 223 each;
// uniform cluster at use 272, with bool/short/long result 272 each. All restored.
// Round 210: Late transition/fade/tag and their three strict pairs give
// 242/244/266/253/278/291; early tag/fade alone 270/267. Original restored.
// Round 16: query declarations portals-indices-clusters/portals-clusters-indices 223 each;
// environment-index lexical name 223; request-field duration workspace 247; plain-inline cue helper 452;
// forced listener search 223; separate query arrays and count declared after them 223 each.
// Original body and both helper declarations restored.
// Lane 10 r11: retail's camera test is `listener * 0x48 == -0x48` (a NONE index test) with
// the camera address formed after the first-listener scan. NONE-guarded camera lookup 312;
// camera test dropped, lookup inside the entry test 261 (1033 bytes); first-listener helper as
// a NONE result set in the scan (retail's scan exactly) 235 (1033); that with the existing
// local-player test 0x14ddc0 and the lookup inside 293 (retail's cmp/setne tests); with the
// lookup at each use after the first-listener test 286. Original body and helper restored.
// Round 16: query declarations portals-indices-clusters/portals-clusters-indices 223 each;
// environment-index lexical name 223; request-field duration workspace 247; plain-inline cue helper 452;
// forced listener search 223; separate query arrays and count declared after them 223 each.
// Original body and both helper declarations restored.
// Round 259: Two independent caller-owned cue requests at entry/listener/active/branch
// scope, each with initializer output first/last, 223 each. Original helper-owned
// requests and complete body restored; frame, BSP/camera/query and duration homes remain.
// DL175: Native short/word listener ordinals [346, 346]; const aggregate sentinel tags 270; short then
// word ordinals with const/mutable tags [346, 346, 346, 346]; signed-byte listener 346. Mutable
// aggregate alone is an excluded archived repeat. Original listener domain and bulk fill restored.
// Round 205: updater/angle/environment-consumer inline versus noinline
// declarations, all eight coordinated combinations 223. Every provider
// and caller declaration restored; frame and BSP/camera/duration homes remain.
// Round 425: Three native missing-tail library fills in six orders, then a shared
// signed/unsigned byte extent, 284 each. Original merged fill loop and body restored.
// Round 434: existing player-slot helper with complete original camera guard;
// shared first-player search/native BSP/live duration in eight graphs 294/297/294/297/281/279/281/279.
// Original guard, helpers, duration observation, query and cue calls restored.
// Round 445: Separate producer/processing count long/short/word/byte 261/263/263/279;
// the same four with independent native short request initialization 261/263/263/279.
// All increase frame or extent; original complete body and helpers restored.
// Round 35 (lane 11, fast): the entry test as the existing local_player_exists(listener) with the camera
// taken inside it and no NULL test 295 (retail's `cmp edi, -0x48` / `setne al; test al, al` tests, aligned
// 159 -> 150); plus the existing local_player_exists-style scan local_player_first_index() 293 (retail's
// scan); plus that index in a local before the camera 292 (scan and camera formation now retail's, aligned
// 137: left are the BSP spilled to [esp+0x14] in place of ESI, a zero kept in ESI and frame 0x14c); plus
// the definition read through `bsp` 262 (BSP in EBX, listener EDI, stride ESI: retail ESI/EBX/EDI;
// aligned 181); plus listener declared before bsp 262 (same code).
// Round 466: Seven BSP/query-count/camera role-name subsets and all roles with the query
// count declared at active scope: 159 each. Seven position/environment/angle output
// workspace entry subsets and all three at active-camera scope: 159 each.
// All original identities, workspaces and producers restored; BSP/count homes remain.
// Round 26 (m3 lane 2, fast): the request stores in retail's order (transition, index, fade) 158 (retail walks the
// requests from `.index`, the init loop's base; aligned 61 -> 53: the lead for the next round); on it the
// transition definition through the global 278 (BSP takes retail's +0x14 home but loses ESI), the camera fetched
// before the first-listener test 201, `count` at function scope 158, the position copy before the cluster 229.
// @retail 0x18bb80
void __stdcall function_18bb80(real elapsed)
{
	long tags[4];
	memset(tags, 0xff, sizeof(tags));
	s_portal_path_bsp *bsp = (s_portal_path_bsp *)g_4e0348;
	for (long listener = 0; listener < 4; ++listener)
	{
		if (local_player_exists(listener))
		{
			if (listener == sound_first_listener())
			{
				s_local_camera *camera = local_camera_get(listener);
				if (camera->active && camera->index != NONE)
				{
					long cluster = camera->index;
					point3f position = camera->position;
					function_18ae80(bsp, listener, &position, cluster, NONE, elapsed);
					long count;
					long indices[2], portals[2], clusters[2];
					if (cluster != NONE)
					{
						function_18dc90(bsp, cluster, &position, FLT_MAX, indices, NULL, portals, clusters, &count);
						if (count <= 1)
						{
							for (long j = count; j < 2; ++j)
							{
								indices[j] = NONE;
								portals[j] = NONE;
								clusters[j] = NONE;
							}
							count = 2;
						}
						if (portals[0] == NONE) portals[0] = portals[1];
					}
					s_sound_environment requests[2];
					long i = 0;
					do { requests[i].index = NONE; ++i; } while (i < 2);
					for (i = 0; i < count; ++i)
					{
						long index = indices[i];
						long tag = NONE;
						if (index != NONE)
							tag = ((s_listener_environment_bsp *)bsp)->environments[index].tag;
						real transition = 2.0f;
						if (index != NONE)
						{
							s_listener_environment_definition *definition = &((s_listener_environment_bsp *)bsp)->environments[index];
							if (!(fabs(definition->transition_time) < 0.0001f))
								transition = definition->transition_time;
						}
						requests[i].transition_time = transition;
						requests[i].fade = 1.0f;
						requests[i].index = tag;
						s_sound_portal_angles angles;
						if (index != NONE)
							function_18ab70((s_sound_portal_bsp *)bsp, portals[i], (s_sound_listener_rotation *)camera,
								cluster == clusters[i], &position, ((s_listener_environment_bsp *)bsp)->environments[index].radius, &angles);
						else
						{
							angles.lower = 0.0f;
							angles.upper = 6.2831855f;
							angles.horizontal_scale = tag != NONE ? 1.0f : 0.0f;
							angles.vertical_scale = 0.0f;
						}
						requests[i].angles = angles;
					}
					sound_environments_update(requests);
				}
			}
			long tag = tags[listener];
			long *current = &g_4ed288->indices[listener * 2 + 1];
			if (*current != tag)
			{
				if (*current != NONE)
				{
					long cue = *(long *)(g_4e3b44[*current & 0xffff].bytes + 0x5c);
					if (cue != NONE) sound_environment_play_cue(cue);
				}
				if (tag != NONE)
				{
					long cue = *(long *)(g_4e3b44[tag & 0xffff].bytes + 0x54);
					if (cue != NONE) sound_environment_play_cue(cue);
				}
				*current = tag;
			}
		}
	}
}
