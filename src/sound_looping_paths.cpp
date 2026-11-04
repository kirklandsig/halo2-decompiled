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
	real decibels;
	if (value > 0.0f)
	{
		decibels = (real)(log10(pinned) * 20.0f);
		decibels = loop_path_pin(decibels, g_44a0b4, 0.0f);
	}
	else
		decibels = g_44a0b4;
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

// score 866 (was 866): frame 0x4e0 agrees; entry zero, target/primary/query-array homes and choice carriers remain.
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
// Round 47 (lane 10, fast): the four query arrays sit in other slots (retail gains +0xec, indices +0x1f0, portals
// +0x2f0, clusters +0x3f0; ours portals +0xf0, indices +0x1f0, clusters +0x2f0, gains +0x3f0) although retail reads
// them in our order (indices, clusters, gains, portals). The arrays renamed three ways 886 each; declared one per
// line in four orders 886 each; declared at function scope 886; the count declared before them 886 (all the same
// code: neither names, declaration order nor scope moves the slots).
// Round 470: unsigned raw decibel words in choice/target/datum records, all
// seven subsets 886 each; all with unsigned helper transport 886. Original
// records, helpers and body restored; entry zero and target/query/count homes remain.
// Round 494: short persistent target count with native long query/choice and
// bounded short existing-count subsets: 925/930/954/975/915/921/943/947. Two native-count forms
// recover 3327 bytes but regress; all original counter domains, helpers and body restored.
// Round 54 (m3 lane 1, fast): retail reads the query count straight from the out-parameter as a dword and keeps
// the choice count in ESI as a dword (the entry zero in ESI becomes the count; `cmp esi, 2`): the count read
// directly with a long choice count 927 (3297 bytes; aligned differing lines 700 -> 585), with the short choice
// count 919; on the long counts `s_loop_path_choice *choice = &choices[choice_count++]` with retail's store
// order (tag, source, decibels, cluster) 960 (our choice address becomes an induction variable; retail
// recomputes it from the count); the three 1.0f defaults as statements in retail's store order (inside, portal,
// outside; the chained assignment stores them reversed) 886, with the long counts 927. Array names (two full
// prefix orders) and declaration order leave the four query-array homes unchanged (retail: gains +0xec, indices
// +0x1f0, portals +0x2f0, clusters +0x3f0; ours portals, indices, clusters, gains).
// Round 446: native persistent/query/search scalar lexical homes, seven subsets
// and all with Boolean local roles 886 each. Original scalar names, body and helpers restored.
// Round 494: short persistent target count with native long query/choice and
// bounded short existing-count subsets: 925/930/954/975/915/921/943/947. Two native-count forms
// recover 3327 bytes but regress; all original counter domains, helpers and body restored.
// Round 153 (lane 4, fast): retail keeps the existing-record count in memory at 0xec right below the existing
// array (0xf0), one 0x104-byte object in the slot the query gains use earlier, and the target count at 0x88
// right below the targets (0x8c). Diagnostic (a grouping struct, not keepable): the existing list as
// {count, elements[64]} 875, every query array then in retail's slot and the count at 0xec; both lists 915
// (frame 0x4e4); targets only 918. Existing count declared before the array 886; used mask declared first
// 886; existing array and count declared at the top 886; append split into store and increment 886; target
// count declared before the targets 886. The same list as a named {count, indices[64]} type used directly
// (no references) 875 too; no existing type in src/ or include/ holds this list, so a new one would only
// move homes: lead only.
// AC477: Next-free choice/target/existing collection builders, subsets 1..7, then word
// completed choice count: 955/945/962/926/977/953/981/981. Original arrays, counts, scans,
// helpers and body restored; entry zero, target/query/count homes and choice carriers remain.
// DH round 290: sequential record-count, sound-index-list and outer-ordinal identities,
// seven nonempty subsets, then all with one inner-search ordinal: 887/886/887/886/887/886/887/887.
// Original separate workspaces, phase initializers, loop domains and complete body restored.
// AC485: intrinsic raw-decibel/linear bit transfers and exhaustive logarithmic producer,
// subsets 1..7, then common pin result: 884/916/916/866/864/903/903/936; exhaustive log result 866 kept.
// Bit captures/pin restored; entry zero and target/query/count homes remain.
// AC488: first-query positive index/tag/duplicate admission subsets 1/2/4/3/5/6/7,
// then capacity break in the bounded query: 866/866/866/924/866/866/924/924. Original graph and body restored;
// entry zero, target/primary/query-array homes and choice carriers remain.
// AC493: Dispatch-owned primary/target-count defaults and provider-filled query output,
// subsets 1/2/4/3/5/6/7 then conditional primary result: 963/937/915/924/923/951/915/915.
// All original defaults, counts, helpers and body restored; entry zero and query/count homes remain.
// Round 379: Actual audible admission/choice-muted default/exhaustive detail result, subsets
// 1/2/4/3/5/6/7, then complete audible index arms: 913/869/866/916/913/869/916/905. NOT kept: no
// exact match for added test-owned outputs. Original body and logarithmic producer retained; entry
// zero/query/count homes remain.
// N332: Actual integer/floating decibel views at choice transfer/attenuation/datum
// publication, subsets 1..7 then readonly completed target: 866/866/866/875/875/875/875/875.
// Native datum constants reach 3327 bytes
// but regress. All record types, helper stores and complete body restored.
// AC496: Native immutable choice/repair, fade-table and override receivers,
// subsets 1/2/4/3/5/6/7 then immutable BSP receiver: 866 each. All original
// types, helpers and body restored; entry zero and query/count homes remain.
// P285: Existing shared used-mask/runtime-word/fixed-word flag helpers, subsets 1..7,
// then complete helpers with CRT bit-vector initialization: 917/876/927/926/931/928/928/928.
// All original helpers, direct flag operations, counts and complete body restored.
// AC502: Shared pin macro at range/blend/active fade/active gain/inactive fade/inactive gain,
// then all body/all logarithmic clamps: 866/866/865/865/862/866/853/853. NOT kept:
// compound-only gain misses the native active-bound reload/lifetime. Original body/helpers restored.
// F351: Native 64-bit used-vector word/byte/wide storage 867/874/886;
// dword/word/byte/wide start after collection 880/866/871/886; word CRT zero 871.
// NOT kept: no gain with retail storage/frame evidence. Original mask, lifetimes and complete body restored.
// Lane 17 r37 (fast): the four query-array names prefixed to force an alphabetical order, and the target count renamed,
// leave every slot unchanged (diagnostic; names do not place these homes); query count read directly with a long choice
// count 942 (3297 bytes: the choice address becomes an induction pointer in ESI, retail recomputes it from the count in
// ESI); on that, `&choices[choice_count++]` 960, the pointer with the count incremented after the stores 952, the duplicate
// search as a loop with `break` 1001; the override target filled by index after `target_count++` 886 (3327 bytes).
// Retail keeps the target count in memory at +0x88 (re-read in the attenuation loop) and forms each new target and choice
// address from the incremented count. Body restored.
// M3 lane 4 r161 (fast): query count read directly with a long choice count and `&choices[choice_count++]`
// as a choice pointer 960 (3308 B; the pointer becomes an induction variable); with the count incremented
// first and the pointer from `choice_count - 1` 953; one function-scope fade-definition pointer for both
// loops 959 (retail reads the global again); the override's suppression gain hoisted out of its loop 962
// (retail calls it per target); the duplicate scan tested `!duplicate` first 917 (3327 B).
// P295: Late second-limiter bound with original/gain/fade/both/query/inactive/all macros,
// then the bound before listener publication: 866/866/865/865/866/866/870/870. NOT kept:
// one-point gain only; complete macro forms grow to 3333/3327. Original body/helpers restored.
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
void function_18ab70(s_sound_listener_rotation const *listener, s_sound_portal_bsp const *bsp, long portal_index, point3f const *point, real radius, bool reverse, s_sound_portal_angles *angles);
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

/* the sounds a listener's environment tag starts when it becomes or stops being
   the current one */
struct s_sound_environment_cues
{
	byte unknown00[0x54];
	long enter_cue;
	byte unknown58[4];
	long exit_cue;
};

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
	request.marker = NULL;
	request.source = NULL;
	request.variant = NULL;
	function_189fe0(&request, tag);
}

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
			long first_listener = sound_first_listener();
			s_local_camera *camera = local_camera_get(listener);
			if (listener == first_listener)
			{
				if (camera->active && camera->index != NONE)
				{
					s_sound_environment requests[2];
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
								s_listener_environment_definition *definition = &((s_listener_environment_bsp *)g_4e0348)->environments[index];
								if (!(fabs(definition->transition_time) < 0.0001f))
									transition = definition->transition_time;
							}
							requests[i].transition_time = transition;
							requests[i].index = tag;
							requests[i].fade = 1.0f;
							s_sound_portal_angles angles;
							if (index != NONE)
								function_18ab70((s_sound_listener_rotation *)camera, (s_sound_portal_bsp *)bsp, portals[i], &position, ((s_listener_environment_bsp *)bsp)->environments[index].radius, cluster == clusters[i], &angles);
							else
							{
								angles.lower = 0.0f;
								angles.upper = 6.2831855f;
								angles.horizontal_scale = 0.0f;
								if (tag != NONE)
									angles.horizontal_scale = 1.0f;
								angles.vertical_scale = 0.0f;
							}
							requests[i].angles = angles;
						}
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
					s_sound_environment_cues *definition = (s_sound_environment_cues *)g_4e3b44[*current & 0xffff].bytes;
					if (definition->exit_cue != NONE) sound_environment_play_cue(definition->exit_cue);
				}
				if (tag != NONE)
				{
					s_sound_environment_cues *definition = (s_sound_environment_cues *)g_4e3b44[tag & 0xffff].bytes;
					if (definition->enter_cue != NONE) sound_environment_play_cue(definition->enter_cue);
				}
				*current = tag;
			}
		}
	}
}
