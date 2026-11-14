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

// MiniMax H (11-12): no new form, no lower score (866): retail still reads the shared global g_44a0b4 from memory fourteen times and keeps it in xmm4 where ours stays in xmm5 for the whole function, the prologue still zeroes a different register than retail (ours ebp, retail esi), and the zeroed count's home still differs (esp+0x24 here, esp+0x88 in retail), so every frame offset after it differs.
// MiniMax E (11-11, run 80): no new form, no lower score (866): retail reads the shared global g_44a0b4 from
// memory fourteen times and keeps it in xmm4 again after the pointer stores, while ours (a plain real declared
// in another file) stays in xmm5 for the whole function; a reload would need a volatile qualifier or the
// address of the global, both ruled out, and the global's definition and second declaration are outside this
// file; the two pool scans' iterator walks, the gains array sharing a slot with the later existing list and
// the zeroed count's home remain the other differences.
// MiniMax H (11-11): no new form, no lower score (866); the prologue still zeroes a different register than retail (ours ebp, retail esi, which retail also pushes before the zeroing), and the zeroed count's home differs (esp+0x24 here, esp+0x88 in retail), so every frame offset after it differs.
// MiniMax S (11-11): no new form, no lower score (866); the prologue still zeroes a different register than
// retail (ours ebp, retail esi, which retail also pushes before the zeroing), and the zeroed count's home differs
// (esp+0x24 here, esp+0x88 in retail), so every frame offset after it differs.
// MiniMax G (11-12): no new form tried this round, no lower score (866): the prologue still zeroes ebp where retail
// zeroes esi, and the zeroed count still sits at esp+0x24 instead of esp+0x88, so the later frame offsets differ.
// score 866 (was 866): s1F walk 11-06: walked against retail block by block, no behaviour difference. Retail accesses the query and choice
// frame layout and register homes remain (earlier starts 919 and 886 below).
// MiniMax S (11-10): no new form, no lower score (866): retail reads the query count as a dword at the loop head and
// keeps the choice count and the two loop indices in ESI/EDI/EAX, while ours narrows the count to a word and holds the
// outer index in EBP; taking the long count directly instead moves the frame offsets and scores 919. Retail's four
// 64-entry arrays sit at F+0xdc (gains), F+0x1e0 (indices), F+0x2e0 (portals) and F+0x3e0 (clusters) with the choice
// and target records at F+0x28 and F+0x7c; ours place them at F+0xe0/F+0x1e0/F+0x2e0/F+0x3e0, and reordering the
// declarations does not move them.
// MiniMax E (11-05): no new form, no lower score; the entry zero, the target, primary and query-array homes
// and the choice carriers are as the earlier rounds left them.
// s1F walk (11-06), at 866: walked against retail block by block, no behaviour difference. Retail accesses the query and choice
// Earlier at 866 (s1F walk 11-06): walked against retail block by block, no behaviour difference. Retail accesses the query and choice
// counts as dwords (0x18aeef, 0x18af10, 0x18af84, 0x18af81); the `short` copies here narrow them (no observable difference:
// counts <= 64). As `long` it scores 942, so it is not kept without a match. Frame 0x4e0 agrees; on the long form the choice count
// and first-loop index stay in memory (retail: ESI/EDI), 3297 bytes (retail 3327). s1F also tried, on that base: grouping
// {count, items[]} structs for targets+existing 933 (3327 B, frame 0x4e4), existing only 938, plus choices 1024-1025 (3413 B);
// the three 1.0f defaults as separate statements (retail's store order) 942. Retail evidence for grouped lists: counts at
// 0x34/0x88/0xec directly below arrays at 0x38/0x8c/0xf0, and the query gains share the existing list's slot at 0xec.
// MiniMax E (11-05): no new form, no lower score; the entry zero, the target, primary and query-array homes
// and the choice carriers are as the earlier rounds left them.
// M3 lane 10 r86 (fast with 0x18bb80, 0x18ab70 MATCH; control 866, aligned 1116, masked 503): retail scales the three gains by a copy
// of the hoisted global (`movaps xmm0, xmm7`, then two field-first products and the last in place on the copy). The scale named once
// as a local in that arm 918 (masked 502); the existing in-place vector scale on the three gains, diagnostic only (needs a cast)
// 917; the 1.0f defaults chained the other way round (retail's store order) 866 (same code), with the named scale 918; that with
// long query/choice counts and the count read directly 941 (3297 bytes, masked 490), with the vector scale 953 (3281 bytes).
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
// AC529: Existing count-before-array storage, with native choice/query count and target-list subsets:
// 856/937/912/935/938/920/938/933. NOT kept: unmatched local grouping; original records, counters, helpers and body restored.
// P326: Named smoothstep contributions/active gain difference/paired inactive fade
// differences, subsets 1..7, then all plus the choice blend difference: 866 each.
// Original grouping and observation sites preserved. NOT kept: no gain; original body restored.
// Round 63 (m3 lane 14, fast): the pin helper in place on its value (`if (lower > value) value = lower; else if (value > upper)
// value = upper;`) 949 (3353 B, aligned 503 -> 827), written value < lower first 949, the upper test first 948 (3357 B), through a
// result local 949, as a ternary with the upper test first 936 (3353 B). This function's clamps are retail's ternary form.
// F384: Actual override admission bool/byte/short/long at its phase boundary
// 866/866/866/866; entry-owned 959/959/949/980. NOT kept: one-use results
// do not match; early lifetimes grow to 3345-3361/3327. Original body restored;
// entry zero, target/query/count homes and choice carriers remain.
// F389: Actual floating linear-converter input at all three raw-field consumers,
// ordinary/forced existing conversion-helper masks 0..7, 875 each (3337/3327 bytes).
// NOT kept: all regress; original raw scalar contract, helpers and complete body restored.
// B435: Failure-first later pool collection/active-target/inactive-record admissions,
// seven subsets, then all with missing-handle search continuation: 866 each.
// Original complete body restored; native 0x4e0 frame, entry zero, target/primary/query
// homes and choice carriers remain. Existing logarithmic result producer retained.
// B439: Full-width query/choice counts, captured/direct bounds and four early
// choice-zero phases: 942/956/956/956/942/956/956/956 (3297/3327 bytes).
// Native scaled exponent part 877 (3339); existing shared conversion 1056 (3120).
// NOT kept: no native frame/home recovery at retail extent; full body/helpers restored.
// P330: Final existing detail-tag obsolete/matching results in bool/byte/short/long,
// eight forms 866 at 3329/3327 bytes. NOT kept: one-use comparison ownership does
// not match; original detail tail, helpers and complete body restored.
// P332: Initial cluster-admission bool/byte/short/long producers at the
// branch and before entry workspaces: eight 866 at 3329 bytes. NOT kept:
// no equality or gain; original body restored, entry zero and count/query homes remain.
// M386: Complete native-order four-buffer query record, full-width query/choice
// and count-before-existing-list subsets 0..7: 873/926/961/949/866/922/945/946.
// NOT kept: unmatched local grouping/domain graph; original arrays, counters and helpers restored.
// DH370: Original raw converter contracts with ordinary expansion, decibel/linear/
// logarithmic subsets 1/2/4/3/5/6/7, then all with forced pin: eight at 866.
// NOT kept: no exact expansion form; original declarations, helpers and full body
// restored. Entry zero, target/primary/query homes and exponent staging remain.
// Round 130 (m3 lane 7, fast): retail's own shape: long query and choice counts (`mov eax, [count]`, `cmp esi, 2`), the choice as a
// pointer from the post-incremented count (`inc esi; lea esi, [esi+esi*4]`), flags and tag in registers across the stores, bsp
// left as a stack argument, sounds hoisted ahead of the second loop. First loop written that way 960 (3308 B, aligned 703 -> 583),
// with the second phase 969 (3276 B, aligned 779). Singly: existing 0x13cb40 getter for the flag read 866 (same code), flags
// local 866, tag local 866, both 866, hoisted sounds 924 (3327 B), choice pointer 962 (3284 B), long query count 919 (aligned
// 584), long choice count 956 (3281 B), both 942 (3297 B, masked 288), with the pointer 960. With hoisted sounds: long query 881
// (3327 B), long choice 976, both 963, with flags and tag 949/881.
// M3 lane 16 r91 (fast; aligned unmatched in brackets, control 866 [703]): the choice cap as `if (choice_count == 2) break;`
// at the loop top 866 (same code); the first loop's three `continue`s as nested ifs 924 (3327 B, retail's entry: ESI zero
// register, first difference moves to the target-count home +0x17); the sound fields read through `bsp->sounds[index]` at
// each use 1005 (3381 B); nested ifs with long choice count and the query bound read from `count` 942 (3297 B) [577]; that
// with `&choices[choice_count++]` stored in retail's order (tag, source, decibels, cluster) 960 (3308 B) [801].
// Round 66 (m3 lane 14, fast): retail reloads the address-taken count at every use (`mov eax, [esp+0x10]` at each loop test and
// for `> 1`, no copy): count used directly with no query copy 919 (same as the long copy); with a long choice count 942 (3297 B);
// with the choice taken as `&choices[choice_count++]` 960 (3308 B; the cursor is strength-reduced into a hoisted pointer), with a
// short choice count 970 (3284 B); targets/count/primary declared primary, targets, count and count, primary, targets 866/866
// (same code). Retail's target count homes just above the choices (frame +0x78) and primary at +0x18; ours +0x14 and +0x24.
// AC552: Existing allocator private linkage/failure-first admission/ordinary inline, subsets
// 1/2/4/3/5/6/7: 866/864/866/864/866/864/864; allocated-or-NONE initializer 870.
// NOT kept: two-point best only; original allocator and whole body restored. Entry zero and native homes remain.
// DL270: Existing datum getter readonly-pool/pool-reference/datum-reference
// contracts, seven subsets then forced complete graph: eight 866s. Natural
// first-pool while scan 909 (3265 bytes). NOT kept: ties or shorter nonnative
// extent; original getter, all callers, both scans and complete body restored.
// M398: Complete choice/existing-index output defaults, aggregate/CRT pairs
// at phase/entry lifetimes: 923/987/921/958/971/979/962/979.
// NOT kept: all regress and grow; original arrays, helpers and complete body restored.
// M402: Native dword query/choice counts and duplicate-failure-first search,
// direct count/repeated flags/repeated tag subsets 0..7: 966/966/966/966/966/966/965/965.
// NOT kept: earlier entry mismatch and 3301/3327 extent; original counters/search/body restored.
// Lane 3 r230 (fast, base 866): the four query arrays renamed with a common prefix, and renamed so their names sort in
// retail's address order (gains, indices, portals, clusters), 866 both, byte-identical: names do not order these slots.
// The choices as a {short count, items[2]} list 1072 (3448 bytes); `inside` tested right after the query call 933 (the
// matched caller 0x18bb80 then differs on the reduced image); the second pool scan as a `for` like the first 933 (3375
// bytes). Re-measured listed groupings: targets as a {count, items[3]} list 940 (frame 0x4e4); the existing sounds as a
// {count, items[64]} list 856 (3329 bytes; round 153's 875 form, a new type that only moves homes, so not kept), with
// long query and choice counts on it 935 (3297 bytes).
// Round 165 (m3 lane 6, fast): on the listed nested-if first loop (924 at 3327 bytes: retail's ESI zero register):
// `s_loop_path_choice *choice = &choices[choice_count++]` with every store through it 966 (3268 bytes); the query count
// used directly as the bound 928; the duplicate search as a `for` with `duplicate = true; break;` 953 (3287 bytes); the
// 1.0f defaults as three statements 924 (same code); diagnostic only, the existing list grouped with its count in one
// object 918 (masked 528). Names and declaration order of the four query arrays re-measured 866 (same code: listed).
// On this body: the choice distance's ternary as if/else 865 (3333 bytes; on the nested-if loop 919); with it, the
// detail tag as `long detail = NONE; if (primary != NONE) detail = ...;` 863 (3332 bytes; three points, under the
// keep bar, restored: a lead). On that 863 form: the second scan's ternaries as if/else 941 (3349 bytes); `muted` as one
// value expression 880; the gain choice through a named near gain 863 (same code); `audible` cleared then set by an
// `if` 904; the fraction's outer ternary as if/else 863 (same code); the override gain computed once above its loop 980
// (3396 bytes); the final listener test nested 863 (same code); the fade time named with an `if` in the fade scan 864
// (3334 bytes, aligned 1101 -> 897: the frame shifts by one slot), in the target loop 889 (the listed shared step),
// both 891.
// M3 lane 9 r105 (fast; "masked" = aligned count with registers and stack offsets ignored, control 866 masked 503):
// re-measured the query count short / long / read directly x the choice count short / long, 866-956 as listed
// (masked 495-503). New: `bool inside = clusters[0] == cluster;` 866 (same code); the fade table read from the
// global at each use in the third loop only 924 (3327 bytes, masked 549: a mix, diagnostic), in both loops 929
// (3305 B, 528); `inside` computed right after the query 933 (512); the four choice stores in retail's order (tag,
// source, decibels, cluster) with the indexed choice 866 (masked 502); the portal distance as an if/else 865 (3333 B,
// masked 493); on that: the second loop's match test through a named `other` sound pointer under nested ifs 947
// (3297 B, masked 536); the store order with the if/else distance 865 (492); `muted = !g_509420` 866 (same code);
// the 1.0f chain reversed so it stores inside, portal, outside 866 (same code); that chain with the store order and
// the if/else distance 865 (492); on the body below: the reversed chain with the store order 866 (502); the second
// loop's test as nested ifs 866 (same code); `muted` stored before the three products in its arm 865 (masked 501);
// `if (choice_count && inside)` for the primary 866 (505); the source range read at each use 908 (504); `target->audible
// = !i && ...` 866 (same code); the flag-2 portal override before the flag-4 outside one 866 (502); the duplicate
// test as `sound->tag == choices[j].tag` 866 (504); the tag tested through `bsp->sounds[index]` before the sound
// pointer 940 (3327 B, masked 580); `muted = false` stored ahead of the flag-8 test instead of an else 869 (509).
// Retail's gains array shares +0xec with the later existing list and its count (stack packing), so the
// four query-array homes follow lifetimes and use weights, not names.
// Lane 4 r186 (fast with 0x18bb80/0x18d730; aligned/masked lines in brackets, control 866 [703/537]): the portal read once into a two-use
// local 866 (three lines move); the fade scan's silent flag set through `sound` 876 (3311 B); the listener's detail sound through one
// `current` pointer 872 (3333 B); the target loop's `existing[j]` in a three-use local 870 at retail's length (3327 B) [710/404: the
// closest shape so far]; that with the first loop's stores through `&choices[choice_count++]` 970 (3272 B) [706/396]; the listener slot
// read at each use 874 (3335 B). Same code: the target loop's `delta` folded into the pin call.
// Round 155 (m3 lane 8, fast; masked = register-masked aligned count, 305 at 866): retail's two pool scans are record-pool
// iterator walks (the found path computes the datum, then `(salt << 16) | index`, then saves the index; the not-found path
// is threaded straight to the exit and each body is placed remotely). Re-measured on this base through the existing
// data_iterator_next_inlined (round 224's idea): the fade scan 877 (3309 B, masked 296: retail's threaded exit into the
// tail), the first scan 893 (3281 B, masked 293), both 910 (3264 B, masked 287). `loop_path_start` without
// `__forceinline`, plain or `inline`, 866 each (same code: LTCG still inlines it and still folds `flags = 0; flags |=
// 0x100`, where retail stores the word and then ors byte +5). Retail's g_44a0b4 sits in .rdata (a const global) and is
// read from memory 14 times, into xmm4 again after pointer stores; ours (a plain `real` in unknown_2197f0.cpp) stays in
// xmm5 for the whole function. Diagnostic: read through its address in this file 938 (3405 B, masked 318), reverted.
// Lead: the iterator walk (both scans, masked 287) is 63 bytes short, about retail's nine extra 8-byte xmm4 reloads of
// g_44a0b4 (5 reads here against 14). They need g_44a0b4 const, and its declaration in src/sound_portal_paths.cpp sits
// in the held sort_4byte change's files.
// m3 lane 15 r72 (fast with 0x18bb80): re-measured the query count read directly with a long choice count 942 (3297 bytes):
// the choice address is strength-reduced into a pointer stepped by 0x28 ahead of the loop (`lea esi, [esp + 0x44]`) with
// the counts in memory, where retail keeps the choice count in ESI, the query index in EDI, and forms each choice address
// from the incremented count. The stacked second score line is merged. No new form was run: each remaining axis is listed or
// ruled out. Counter widths and domains (query, choice, target, existing, ordinals: short/long/byte/word/unsigned) 866-1025;
// the query count copied or read directly 919/942; the choice address (indexed, post-increment pointer, count-first) is
// strength-reduced on any long count (953-969); loop shapes (continue or nested ifs, cap test placement, duplicate-search
// forms) 866-1001; store orders and the 1.0f defaults (same code); array names, order and scope (same code: homes follow
// lifetimes); helper routes for the pin/log/linear conversions and getters; grouping a count with its array would only move
// homes (an aggregate made up to steer), so it is not a candidate.
// MiniMax B (11-13): no new form, no lower score (866): retail keeps the entry zero in esi and the constant g_44a0b4 in xmm4 across the two scans, ours keeps the zero in ebp and the constant in xmm5; g_44a0b4 has to be a const global and its declaration is in src/sound_portal_paths.cpp, outside this file.
// MiniMax A (11-13): no new form, no lower score (866): the first difference is still the stack ingress and the capacity switch, which retail expands as a table and this link keeps as a call; the two pool scans and the register homes that follow them depend on that choice.
// M3 lane 16 r114 (fast with the file, 0x2197b0's file, 0x18d730 and 0x18dc90; control 866, aligned 1116 lines): retail's frame
// keeps each count directly ahead of its array (choice count +0x34, choices +0x38; target count +0x88, targets +0x8c; existing
// count +0xec, existing +0xf0), loads, increments and stores the count at each add, and forms the new element's address from the
// incremented count: three counted lists (count, then elements), not separate locals. Retail also reads 0x44a0b4 fourteen times
// (ten movss, four fld) and reloads it after stores through the pool's pointers: the read count of a global whose address is taken,
// which the pin helper gives naturally when its bounds are `real const &`. Measured, not kept (the positional score rises while the
// bodies are different lengths): the pin with const-reference bounds 922 (3401 bytes, aligned 1007), with a const-reference value
// only 866 (same code); a counted-list template for choices and targets with `&list.elements[list.count++]` 933 (3301 bytes); with
// `list.count++; &list.elements[list.count - 1]` (retail's add: inc, store, lea from the new count) and the target count stored
// ahead of `primary` 937 (3272 bytes, aligned 1076: retail's prologue and first-loop shape); that with the reference pin 907 (3344
// bytes, aligned 950); with the existing list counted too 942 (3376 bytes, aligned 993: retail's array order on the stack, every
// home 8 bytes high because two more conversion temporaries are live); then the 1.0f stores as three statements, `definitions`
// read inside the target loop, `distance` and `portal` in two-use locals, the target stores tag, decibels, gain, audible, cluster,
// priority, source 955 (3392 bytes, aligned 970; the target loop then agrees line for line but for homes); the query count variable
// reused as that loop's bound (retail stores the choice count to +0x10 and reads it back each pass) same code; an `add()` member
// 948 (kept out of line even when forced). Diagnostics: 0x44a0b4 as an `extern const real` under a scratch name 866 (same code, 5
// reads), a file-local constant 866, a scratch global with its address stored 938 (3405 bytes, 14 reads). build/r114/lead-18ae80.patch
// holds the 955 body; next: the two extra temporaries (frame 0x4e8 against 0x4e0), then the first loop's registers.
// T106: Strict open near-zero intervals at range/paired fade times/completion gain,
// seven families then continuing-output-first completion: 880/894/962/904/886/885/955/955.
// NOT kept: native absolute-value shape and byte equality unrecovered; every form
// lengthens the body. Original predicates, helpers and complete body restored.
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
