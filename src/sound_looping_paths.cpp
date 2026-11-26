// @flags /O2 /arch:SSE /Gr
#include "unknown_11c920.h"
#include "unknown_124f90.h"
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

long looping_sound_new_attached(long tag_index, long object_index, short marker_index, long value);

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
		index = looping_sound_new_attached(tag, NONE, NONE, 0);
		if (index != NONE)
		{
			loop_path_get(pool, index)->flags |= 1;
			loop_path_get(pool, index)->gain = gain;
		}
	}
	return index;
}

// score 837 (was 846): The entry zero uses EBP instead of ESI,
// and the target count, primary source and query arrays receive different homes. The retail query keeps its
// full-width count through the walk and reloads a sound entry before testing its tag.
// M3 lane GB r3 (fast with 0x18bb80, 0x188f90, 0x18a5a0, 0x16b840, 0x2195f0, 0x2197f0, 0x2197b0, 0x219650, 0x18d730, 0x18dc90; control 837, 3327/3327 B, +0x2, all 8 companions MATCH; 43 trials, body unchanged): on main's 837 base: consistent 2-caller loop_path_fade_time (0x18bb80 idiom) + fade data_iterator_next_inlined 940 (3274 B, al 612, mask 499; keeping fade loop 936; direct g_4e0348 both loops 885; definition ptr both loops 940), + single-ptr loop_path_start 963 (alone 867), + 0x2195f0 linear shape 924 (alone 857 at 3327 B; scaled-only 851), all three 914 (mask 374), + approach_126660 961, + word* flags 961, + direct g_4ed28c 938, + reused iterator 937, + override order & scale local 961 (3242 B, mask 324); s_levels 3-real copy 923; s_decibels override += 838; flag_bits if-form 837 (same code), bitfield assigns 855. On counted-list + const-ref lead (+0x27, frame 0x4e0, 851 at 3302 B, al 332, mask 156): consistent 2-caller loop_path_fade_time 854 (al 326, mask 142: matches both fade-time address forms); separate fade iterator 833 (3327 B); s_levels copy 888; s_decibels 851; direct g_4ed28c 851; bg predicate / word_flags_set 851; no query flags local 855; !(fabs(-range) < 0.0001f) + distance local 856 (mask 139: removes fxch); existing slot pre-inc 835 (3327 B); flag_bits if-form 831 (3327 B, al 338, mask 151); flag_bits assigns 852 (al 300, mask 132: drops mov ebp, 0x200 hoist, moves pool out of EDX to EBP, puts NONE in EBX across target loop and hoists g_4e0348 into EBX for fade scan, matching 52 consecutive target-allocation/start instructions).
// M3 lane GB r12 (26 fast trials with 0x18bb80, 0x188f90, 0x18a5a0, 0x16b840, 0x2195f0, 0x2197f0, 0x2197b0, 0x219650, 0x18d730, 0x18dc90, 0x11c1b0; control 837 at 3327/3327 B, +0x2, all 9 matched peers MATCH; body unchanged): on main's 837 base: 2-caller loop_path_fade_time (ternary/if) + fade data_iterator_next_inlined 937/940 (3278/3274 B, mk 416/419; reused iterator 940), single-ptr detail tail alone 837 (same code), + single-ptr loop_path_start 867 (3299 B), 0x2197f0 linear/log helper shape 863 (3327 B, al 681, frame 0x4d4), + fade helper/iter 924 (mk 385), `(long)source != NONE` in both scans 837 (same code), query tag/flags/scale locals + combined flags 4|2 repair 876 (3304 B), combined clean main 921 (3329 B, mk 379), tag read before sound ptr 937 (3292 B), bsp->sounds fade time in build loop 933 (3285 B), all four main tweaks 964 (3269 B). On clean count-first stacks (`{ long count; T items[N]; }` for targets[3]/choices[2]/existing[64], frame 0x4e0, no const-ref pin): base 935 (3239 B, +0x27, al 448, mk 159), flags before duplicate check 926 (al 441), hoisted bsp->sounds 975 (3221 B), tag read before sound ptr (`bsp->sounds[index].tag` then `sound = &bsp->sounds[index]`) 849 (3239 B, +0x27, al 404, mk 143: matches prologue and first 86 insns through query/dup loop, but -88 B short of retail without the 14 g_44a0b4 reloads); on it, bsp->sounds fade + fade abs-index + outside=portal=inside + !audible + single-ptr start 903 (3209 B, al 843, mk 287; approach_126660 903, mk 279; PIN_126660 903; byte flags 912; fade data_iterator_next_inlined 892, mk 181; g_4e0348 fade time 869, al 511).
// MiniMax E (11-19): score 846 (was 846): no new form, no lower score. Read from the listing again: retail zeroes
// ESI at the entry and keeps the primary at frame+0x18 while our body zeroes EBP and keeps the primary at frame+0x24,
// retail's four query arrays start at frame+0xdc with the gains first and ours start at frame+0xe0 with the clusters
// and the indices in the other two slots, and retail reads the query count as a full word from frame+0x00 where our
// body narrows it. Reordering the four array declarations left every displacement unchanged, so the homes are not set
// by the declaration order. This round made the query count and the choice count full words: 919 with the query count
// alone, 964 with the choice count alone and 965 with both, all worse than 846, so retail's full-word count is not
// reached that way. Read off the listing, the four array homes run gains, indices, portals, clusters in retail and
// gains, clusters, portals, indices here, with the portals in the same slot in both.
// Earlier score 846 (was 846): Frame 0x4e0 agrees. The entry zero uses EBP and the target count, primary source and query arrays have different homes.
// entry zero, target-count, primary and query-array homes differ in the 0x4e0 frame.
// unchanged in m3 lane 9 r133 (at the end of this note). Earlier 846 (was 858): MiniMax S's PIN_126660 clamps (11-15, below) on lane 1 r109's form. Earlier 858 (was 866): m3 lane 1 r109 at the end of this note. Earlier 866 (was 866): s1F walk 11-06: walked against retail block by block, no behaviour difference. Retail accesses the query and choice
// frame layout and register homes remain (earlier starts 919 and 886 below).
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
// MiniMax S (11-15): 853 (was 866) on its base; 846 with lane 1 r109's forms (coordinator, fast): the six clamps of this function through the existing PIN_126660 macro
// instead of the local pin helper; the body now has retail's length (3327 bytes) and 0x18bb80 stays matched. Left:
// the prologue still zeroes ebp where retail zeroes esi, and the zeroed count's home differs (esp+0x24 here,
// esp+0x88 in retail), so the later frame offsets differ.
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
// Round 103 (m3 lane 10, fast with 0x18bb80 matched): the first loop with the count read directly, a long choice count, nested
// ifs and `choice_count++; choice = &choices[choice_count - 1];` storing tag, source, decibels, cluster through it 952 (3284 bytes:
// the choice address is still strength-reduced into a pointer stepped by 0x28 with the count in memory, where retail keeps the
// count in ESI and forms each address from it); the used bits cleared by memset 866 (same code); the range test as `fabs(range)`
// 907 (the negated form is retail's); the choice-to-target loop storing through `targets[target_count]` with the increment last
// 924 (3327 bytes); the primary source set ahead of the choice-to-target loop 940. All restored.
// T106: Strict open near-zero intervals at range/paired fade times/completion gain,
// seven families then continuing-output-first completion: 880/894/962/904/886/885/955/955.
// NOT kept: native absolute-value shape and byte equality unrecovered; every form
// lengthens the body. Original predicates, helpers and complete body restored.
// M3 lane 16 r118 (fast with 0x18bb80 and the 0x188f90/0x18a5a0 family, control 866): the override target stored tag, decibels,
// then the 1.0f gain 900 (3297 B); the decibel floor as the literal -64.0f in the three helpers 866 (identical code); the matched-sound
// overrides under `if (sound->flags & 4 || sound->flags & 2)` with one other-sound pointer 906 (retail's block at 0x18b18d in shape,
// `and eax, 4; jne; test cl, 2; je`, but a test repeated inside its guard: lead only); loop_path_start's allocation through the existing
// looping_sound_new_attached(tag, NONE, NONE, 0) 909 (3357 B: inlined, and both start sites become retail's exact `mov word [ecx+4], dx;
// or byte [ecx+5], 1` sequence, where the hand copy folds the two flag writes); with the start's second half through one datum pointer
// (0x18a5a0's own shape) 886 fast and plain (3337 B). Lead: loop_path_start is 0x18a5a0 over 0x188f90 (0x18a5a0 sits under
// auto_inline(off)); loop_path_logarithmic/linear are 0x2197f0/0x2195f0 (/Ob1, called out of line).
// P491: Float exponential/logarithmic/absolute-value API families, seven subsets then
// complete standard overloads: 866/866/866/866/866/866/866/866. NOT kept: no exact native math/entry/home repair;
// original APIs, logarithmic result producer, helpers and complete body restored.
// P493: Shared fade-step conditional/branch/divisor/result producers, ordinary
// then forced inline: 890/916/893/916/890/916/893/916. NOT kept: all regress; native
// duration reloads, entry zero and target/query/count homes remain. The new
// producer at both fade sites and all complete executable edits are restored.
// m3 lane 4 r199 (fast with 0x18bb80, 0x18d730; masked = register-masked aligned count, control 866, masked 549). Same three
// call targets as retail. The entry zero (ESI in retail) is the first pass's choice count, which retail keeps in ESI and
// stores back at each add (`inc esi; mov [F+0x24], esi; lea esi, [esi + esi*4]; lea esi, [esp + esi*8 + 0x10]`): lane 16
// r114's counted lists (listed 933-955). The 14 g_44a0b4 reloads need an address-taken global (listed 922/938; const alone
// keeps 5 reads), and a const g_44a0b4 would need its `extern` in src/sound_portal_paths.cpp (a held file) changed too,
// as MSVC mangles const into the name. New (the PIN macro, whose re-evaluated argument matched 0x1e9ce0 this round): the
// existing PIN_126660 at the six pin sites 853 (3327 B, masked 559: a length-term drop; at the target loop retail loads
// the step once into XMM3 where the macro re-reads it, so not kept), at the fraction and blend sites only 866 (other
// code), at the four step sites only 856 (masked 567); inside loop_path_logarithmic as well (same code), all eight 853
// (p1's code); the target loop's sound through loop_path_get at each use 909 (3361 B); the choice-to-target loop through
// `choices[i]` at each use 886 (3346 B); the pin helper written `value < lower` first (same code); lane 16 r91's nested-if
// first loop with the macro at every site 928 (3305 B, retail's ESI entry zero).
// Round 247 (m3 lane 3, fast with 0x18bb80): the fade scan as a `for` from index 0 like the first scan 933; the first scan as
// the fade scan's `while ((i = next(pool, i + 1)) != NONE)` 909; the listener's detail slot through a `long *slot` 866
// (same code); the existing list's store and increment as two statements 866; `fraction * fraction` named and used twice
// in the blend 866. Re-measured: the fade scan's silent flag through `sound` 876, `choice_count && inside` 866 (listed).
// M3 lane 1 r109 (fast with 0x18bb80 and 0x18d730, confirmed plain; control 866). Kept, 866 -> 858 (3328 B), three forms
// with retail evidence: the portal distance as an if/else with the store in each arm (retail stores xmm0 in both arms,
// 0x18b037/0x18b044); the detail tag defaulted to NONE and loaded under `if (primary != NONE)` (retail's `or ebp, -1`
// ahead of the primary test, 0x18b8b9); the target loop's existing entry in a named three-use local (retail loads
// existing[j] once into ESI, 0x18b550). Measured: the first two without the third 860; the detail form alone 864; the
// decibel helper as a single-exit result on the kept form 881 (3303 B; retail moves each constant into a register, then
// stores once). Left: the entry zero (ours EBP, retail ESI = the choice count), the query/choice counts (retail dword,
// count in ESI), the choice address (retail forms it from the count after the increment), the target loop's audible
// flag (retail two byte RMWs with the `or` arm first, ours one merged word RMW), and the pool scans' block layout.
// On the kept 858 form (fast): the choice count as a long 965 (3268 B); the audible update with the `or` arm first
// (`if (!target->audible)`) 858 (still one merged word RMW); the query count used directly at its three uses 913; the
// fade time named once at both step sites 884 (3330 B); the decibel helper as a single-exit result 881 (above).
// M3 lane 15 r92 (fast, control 846): the clusters array renamed (diagnostic: names do not move the array homes) 846
// (same code); the query count read directly with a long choice count 965 (3264 B: the prologue's ESI zero and the
// indices/clusters/portals homes become retail's +0x1f0/+0x3f0/+0x2f0, the count moves to +0x14 and the gains to
// +0xf0); that with each choice filled through `&choices[choice_count++]` 972 (3287 B); the same pointer with the short
// counts 967 (3275 B); the target count declared above the targets array 846 (same code).
// P557: Bounded accepted-choice cap <2/<=1 with short/long query and choice domains:
// 846/919/964/965/846/919/964/965. NOT kept: no equality or native entry/count/query-home repair;
// original counters, equality cap, helpers and complete body restored.
// P559: Declared sound/choice level vectors, paired storage and existing whole-copy/scale/default producers:
// 846/846/846/938/916/850/925/868. NOT kept: unmatched record/producer shape; native entry zero/query/count
// homes unresolved; original records, helpers and complete body restored.
// P561: Original negative duplicate-search flag in char/signed-char/byte/short/word/int/long/dword:
// 846/846/846/915/915/846/846/846 at 3327/3327 bytes. NOT kept: no native entry zero/count/query
// residence or exact bytes; original bool flag, helpers and complete executable body restored.
// P582: Original-site scalar memcpy at completed query count/paired query indices/existing
// candidate, seven subsets, then complete scalar memmove: 846/846/846/846/846/846/846/1144. NOT kept: no exact native entry-zero,
// count/query/primary-home recovery; original scalar producers, helpers and complete body restored.
// P585: Immutable existing range/fraction/blend/current/delta/active gain/inactive step,
// then the complete scalar graph: eight 846 at 3327/3327. NOT kept: no entry-zero,
// count/primary/query-home change; original scalar declarations and entire body restored.
// P588: Fixed bindings of original BSP/choice-sound/repair-sound/fade-table/
// choice/target/override/pool receivers: eight 846 at 3327/3327 bytes. NOT kept:
// no native ESI entry-zero or count/query/primary-home recovery; original body restored.
// P590: Original incoming BSP/listener/point/cluster/override/elapsed fixed bindings,
// complete input binding graph and six parameter role names: eight 846 at 3327/3327.
// NOT kept: no exact ESI-zero/count/query/primary-home recovery; original body restored.
// P592: Completed count before conversion/override/existing append, subsets 1/2/4/3/5/6/7,
// then full-width query/choice on the complete append graph: 926/871/860/929/914/880/917/947. NOT kept:
// no exact native entry-zero/count/query-home recovery; original body and helpers restored.
// M3 lane 3 r255 (fast, control 846; aligned/register-masked lines 640/527): the listener tail with an early return
// when the slot's sound already has the detail tag 846 (same code); the second loop's matched sound through a pointer
// formed after the distance call 929 (3305 B); both fade times through a by-reference inline fade-time helper 846 (same
// code); the definition table read inside the choice-to-target loop 927 (3309 B); on this base, the allocation through
// the existing looping_sound_new_attached(tag, NONE, NONE, 0) 896 (3274 B). Diagnostic, reverted at once: 0x14b240
// storing into its owner (its call then clobbers the bsp) 928, so retail's re-read of the sound table after that call
// is not a callee summary. Nothing in retail takes 0x44a0b4's address (no immediate, no data pointer); the matched
// 0x2195f0/0x2197f0 read it for a -64.0f literal, so its 14 reads are reloads of a literal, not an aliasing effect.
// M3 lane 1 r114 (fast with 0x18bb80): retail's matched-sound overrides read the matched sound's address once after the
// distance call (`mov ebp, [ebx+0xd8]` reloaded, one `imul`), because the choice count holds ESI and no callee-saved
// register is left for the sounds base; ours keeps the base in EBP across 0x14b240. One `other` pointer to the matched
// sound taken after the distance call and used by both overrides 929 (3305 B); a per-target pointer to the fade
// definition used for both fade-time reads 885; long counts with `&choices[choice_count++]` and that `other` pointer
// 981 (3255 B; long counts alone re-measured 965 at 3264 B); the inside/outside level named before the gain sum 846
// (same code); that with the portal named too 905 (3337 B). g_44a0b4 should be a const global in .rdata (retail reads it
// from memory 14 times), but its other declaration is in a held file.
// M3 lane 9 r133 (fast with 0x18bb80, which stays matched; control 846): redundant guards that repeat a loop's own test,
// meant to move callee-saved homes without code: `if (0 < query_count)` around the first loop 857, `if (0 < choice_count)`
// around the choice-to-target loop 909 (3328 B), `if (0 < target_count)` around the target loop 850, `if (0 < existing_count)`
// around the existing search 850; the first loop's new choice filled through `s_loop_path_choice &choice =
// choices[choice_count]` with the increment last 925 (3288 B: the reference becomes a strength-reduced pointer, as the listed
// pointer forms). Body unchanged.
// The 0x4e0 frame agrees. Entry zero and the query/choice/target stack homes still differ.
// count as for-loop ordinal 914; count as while-loop bound 913; per-target ordinal publication 930; completed
// choice count 951;
// target cursor 961; choice cursor 920; branch-produced ordinal 956; branch-produced completed count 939.
// NOT kept: all eight target-count forms regress against 846. The two retail-length variants retain the entry
// zero-register difference.
// Entry zero stays in EBP in the usual form, with the choice, target and query homes displaced.
// Complete source/default, default-first, initialized-default and common-predicate gain staging,
// published before scaling 939/939/936/932 and after scaling 984/984/984/933.
// NOT kept: all regress. The predicate with publication after scaling keeps 3327 bytes but changes the first load.
// F408: Shared query-gain and existing-index buffer 846; full-width query 919, choice 967, both 968.
// With the count also shared: 848/919/970/957 in the same order.
// NOT kept: full-width choices recover the entry ESI zero but shorten the body and raise the score.
// The target-count, primary and query-array homes remain.
// B460: Completed matching-sound search with reloaded index/carried index/while ordinal/pointer range
// 918/931/918/921, or with one first-choice tag for each scan 918/931/918/921.
// Sound table held through the scan and matched row reacquired after the distance call,
// with a reloaded or carried index 952/936. Entry zero and target/query/count homes remain.
// Original body restored.
// F417: Shared gain after tag 900; two-use computed gain 851; target gain readback 900;
// tag and decibels before shared gain 900; target entry before range 866; local gain with target-first
// publication 852; named endpoint before shared gain 900; allocation before choice gain 900.
// NOT kept: the entry zero and query array homes remain and the scores regress. Original body restored.
// AC571: Full producer count in the first scan or in repair 915/856. Capacity checked on acceptance,
// short/full-width count, 950/965. Count advanced before record stores and capacity checked afterward 964/984.
// Primary set during first inside conversion or first target append 940/956. NOT kept: all regress.
// The full-width acceptance check recovers ESI at entry but shortens to 3232 bytes and keeps the count-home differences.
// Entry zero chains for target/choice, target/query, all three, or target/existing counts
// 859/859/858/868. Choice and ordinal after the query or with its output before the call
// 847/846. Existing count and bitmap chained before the scan or with target count at
// entry 847/939. EBP remains the entry zero and the count and array homes differ.
// Round 623: accepted-range cursor throughout, ordinal duplicate scan, ordinal conversion and
// advance-before-fill 958/989/947/967 with the short query count, 956/958/948/966 with the full
// query count. NOT kept: the ordinal duplicate scans put zero in ESI but keep other count and query
// homes and lengthen the body.
// Round 625: first query cursor over clusters/gains/portals with short count 988/987/945
// (index cursor recheck 988); indices/clusters/gains/portals with long count 939/939/938/940.
// Attenuation sampled before the indexed walk with short/long count 847/919.
// NOT kept: portal cursors recover ESI zero at +0xf but grow the body and retain displaced
// count and array homes. Other cursor variants also move the frame away from 0x4e0.
// First source retained from acceptance, record completion, collection end or inside selection:
// 943/937/920/952. The same forms with full query count: 945/954/879/945. NOT kept: even the
// 3327-byte form raises the score, and the entry zero and stack homes still differ.
// Round 628: dword choice count through construction/repair, then short for later use 947/952.
// Short construction and long count at repair/conversion 912/893. Full query bound in construction
// with a short completed count 961, or in repair with a short conversion count 890.
// Typed portal indexing 846, with the first two choice-count splits 947/952.
// The dword construction forms recover the ESI zero but shorten the body and raise the score.
// shared sound lookup in first query and primary repair 846; shared sound lookup in query and neighbor
// repair 846; shared sound lookup through all BSP reads 846; shared sound reference through all BSP reads
// 846; ordinary inline sound lookup through all BSP reads 846; shared sound lookup with full query count
// 919; shared sound lookup with full choice count 964; shared sound reference with full query and choice
// counts 965. NOT kept: the shared sound lookups tie at the original width. With a full-width choice count,
// the entry zero and three query-array homes agree with retail, but the target-count and primary homes still
// differ and the extent falls to 3264 bytes. The opening executable source is restored.
// M3 lane 13 r110 (fast with 0x18bb80, which matched in each; control 846, aligned 1004): the literal -64.0f in place
// of the global in both decibel helpers 846 (same code); with the linear helper pinning the long bit pattern through
// an if/else result (0x2195f0's shape) 867 (3309 B, aligned 665); every helper in the shape of 0x2195f0/0x2197f0 873
// (3309 B); only the logarithmic helper in 0x2197f0's shape 848; the fraction's test as `fabs(0.0f - range)` 846 and
// its ternary as an if/else 846 (both the same code). Lead: retail treats 0x14b240 as a call that may write memory
// (0x18d730's note; retail reloads the sounds base after it here); with that callee form this body is 911, so a
// rework of this function belongs with build/r110/held-0x18dc90.patch.
// Definition bitfield gates: custom levels/scale/both 888/919/914,
// original-source repair/neighbor level/both 913/846/913. All definition bits
// 938; one initial flag snapshot with live repair bits 910. Entry zero,
// query/count homes and the accepted-choice register remain.
// Full-width choice count before target storage, before the cluster gate, at cluster-arm entry,
// or after the query output clear: 979/979/964/963. With the full query result: 979/979/965/965.
// NOT kept: ESI becomes the entry zero, but target count and primary remain displaced and the extent drops to 3264 bytes.
// M3 lane 5 r200 (fast with 0x18bb80, 0x188f90 and 0x18a5a0, all MATCH; control 846, frame-blind aligned 282 of 941/928
// lines with registers and stack offsets masked): long query and choice counts, `&choices[choice_count++]` in retail's store
// order with the 1.0f defaults as statements, and the first pool scan in the fade scan's while form with the salted index
// named before its test 1020 (3231 B, fb 277); with loop_path_start allocating through looping_sound_new_attached(tag, NONE,
// NONE, 0) and 0x18a5a0's second half 1000 (3238 B, fb 251: the four query arrays take retail's order, frame 0x4dc); plus
// the choice/target/existing lists as {count, items} records 971 (3250 B, fb 238: the choice cursor is no longer
// strength-reduced, but the zero is EDI and frame 0x4e8); without the existing record 984 (0x4e4), without the target
// record 978; the indexed choice stores on that base 1013. Lead: the override's `+=` through the shared decibel value type
// (include/unknown_21e230.h) gives retail's `movss xmm0, [eax]; mov [slot], edx; addss xmm0, [slot]` (984, fb 236, cast
// diagnostic): retail's integer decibel constants and slot-copied right operands are that type's signature, so the
// decibel fields here may be that type.
// M3 lane 12 r145 (fast with 0x18bb80 and 0x18ab70, both MATCH): retail's override loop re-reads the target count from its
// home each pass and the choice count is stored to its home after each increment with `&choices[count]` recomputed from it.
// The override add through the shared decibel value type 846 (same code), with the gain as such a local built from its bits
// 846 (same code); long query/choice counts with the query count read directly and each choice through
// `&choices[choice_count++]` 972 (3287 B: the pointer becomes an induction variable), that with the stores in retail's order
// (tag, source, decibels, cluster) and the three 1.0f defaults as statements 972; on main's body the four stores in retail's
// order 846 (code changes, score ties).
// M3 lane 7 r187 (fast with 0x18bb80 and 0x18ab70, both MATCH; control 846, difflib-aligned 1004). Reading: retail forms the
// choice as `&choices[count]` from the incremented count (`inc esi; mov [esp+0x34], esi; lea esi, [esi+esi*4]; lea esi,
// [esp+esi*8+0x10]`), stores tag, source, decibels, cluster through it, and reloads the count into ESI after the distance
// call; the count is never an induction pointer. Tried: long query and choice counts with `choice = &choices[choice_count++]`
// up front, stores in retail's order, the 1.0f defaults as three statements 972 (3287 B; the pointer is strength-reduced
// into [esp+0x18]); the same with the pointer formed from the count and `choice_count++` after the distance 966 (3264 B,
// ESI is then the entry zero as retail), that with the loops bounded by `count` directly 966 (same code); that with a
// short choice count 938 (3280 B, aligned 986); with a short query count 965 (3264 B).
// M3 lane 10 r126 (fast with 0x18bb80, MATCH throughout; control 846, aligned 1004): g_44a0b4 as `real const` at its
// definition and both extern lines 846 (same code) and it loses matched 0x2197b0 (the read folds to an immediate, 15)
// and raises 0x18d730/0x18dc90 287/315 -> 400/452; re-measured lane 15 r92's long counts 965, with
// `&choices[choice_count++]` 972, the pointer alone 967 (all as listed); new: the short count incremented first and
// each choice filled through `&choices[choice_count - 1]` 965 (3271 B, aligned 1047).
// Accepted ordinal captured at increment, after the header, or from the incremented count;
// post-increment choice reference; full-width choice count with captured ordinal; full-width counts
// with header increment; target count as conversion ordinal; target count published after conversion: 932/932/950/967/954/941/914/951.
// NOT kept: The entry zero, accepted count register and the separate count and buffer homes remain displaced.
// Inside from an accepted choice 927, or from a nonempty query 920;
// first-choice reference through repair 913; matching neighbor through both repair stores 948;
// shared neighbor gain 965; one post-distance repair-flags read 846;
// primary published during conversion 953; shared neighbor record and gain 961.
// NOT kept: The ESI count and the target, primary and query-array homes remain different.
// M3 lane 12 r152 (fast with 0x18bb80/0x18ab70; control 846): the detail tag as a ternary initializer 845 (one point, not
// kept); `bool inside` computed right after the query count 927; the override's logarithmic gain computed once ahead of
// its loop 959 (3380 B); the pool global read at the function top 966 (3355 B); `primary` declared ahead of the target
// count 846 (same code).
// M3 lane 8 r201 (fast with 0x18bb80, 0x188f90, 0x18a5a0, 0x2195f0, 0x2197b0, 0x2197f0, all MATCH each time; control 846,
// difflib-aligned 1004, register-masked 916, register- and frame-masked 473 = "fb" below). The kept body is a positional
// optimum at retail's length; every form with retail evidence changes the length, so the score rises while fb falls.
// New readings of retail's datum-flag code: the audible update is two byte operations (`or byte [eax+4], 0x80` under
// `!audible`, else `and byte [eax+4], 0x7f`) where ours merges one word load, select and store; the first scan clears
// 0x200 with a word AND against 0xfdff hoisted into EBP where ours narrows to `and byte [eax+5], 0xfd`; each start site
// stores a zero word and then ors byte +5 with 1. Tried on main's body: both masks cast to the field's width 846 (same
// code); the datum flags as a bit-field view of the word with the 0x200 clear and the audible update as bit writes 846
// (same code). Compile-only probes of the 0x200 clear, all `and byte [eax+5], 0xfd`: `(word)~0x200`, `& 0xfdff`,
// `(word)0xfdff`, `0xfdffu`, through a short view, `(word)(flags & ~(1 << 9))`, `~(word)0x200`, a mask local declared
// outside the loop, a short field. Probes of the audible update: an inline setter taking the datum and a bool, either
// sense, a ternary, two ifs, `== false` (all the merged word form); byte-wise access (a merged byte form); through
// `word *flags = &sound->flags;` as matched 0x18a720 writes its flag (`if (!target->audible) *flags |= 0x80; else *flags
// &= ~0x80;`) gives retail's two byte operations, 3 instructions shorter. The start's `flags = 0;` followed by an or of
// 1 into the word's second byte gives retail's `mov word [ecx+4], reg; or byte [ecx+5], 1` (as does the existing
// looping_sound_new_attached, lane 16 r118).
// Composite, built up one form at a time (score / bytes / fb): long counts with each new element addressed from the
// incremented count 969 / 3259 / 463; plus the first pool scan through the existing data_iterator_next_inlined 969 /
// 3235 / 397; plus the fade scan through it 995 / 3224 / 302; counted lists alone (lane 16 r114's template) 919 / 3287
// / 419; lists and both iterator scans 1013 / 3192 / 277; plus the pin helper's bounds as `real const &` (the -64.0
// global is then read 14 times as in retail) 935 / 3268 / 270; plus the audible update through the flags pointer 949 /
// 3254 / 265; plus the six clamps through the pin helper 942 / 3253 / 264; plus the start through
// looping_sound_new_attached(tag, NONE, NONE, 0) with 0x18a5a0's second half (no pin helper) 930 / 3275 / 208; plus the
// matched-sound overrides under one `flags & 4 || flags & 2` test with one other-sound pointer 932 / 3270 / 238; all of
// these 913 / 3290 / 196, without the helper clamps 908 / 3294 / 191; plus lane 16 r114's target loop (the count copied
// to the query count, definitions read inside, distance and portal named, the stores tag, decibels, gain, audible,
// cluster, priority, source) 908 / 3290 / 185. Lead: build/r201/lead-0x18ae80.patch (the 185 form; 37 bytes and 16
// instructions short of retail, frame 0x4e8 against 0x4e0, 130 aligned hunks left: the first loop's sound pointer and
// flag loads, the fraction clamp's order, the step loaded once per target, the 0xfdff word AND). Not kept: every step
// raises the score.
// Nest valid index, valid tag, unique choice, or tag and uniqueness 846/846/846/846. Full-width accepted
// count with these guards 964/964/964/964. NOT kept: the short forms tie. The full-width forms recover the
// ESI zero but shorten the body and raise the score. The target-count and query-buffer homes still differ.
// M3 lane 11 r133 (fast with 0x18bb80, 0x188f90 and 0x18a5a0, all MATCH; control 846, difflib-aligned 1004, frame-blind 473;
// body unchanged). On lane 8 r201's composite lead (counted lists, both scans through data_iterator_next_inlined, the pin
// with `real const &` bounds, the start through looping_sound_new_attached; re-measured 908 at 3290 B, aligned 895, frame-blind
// 185): the decibel helper as a single-exit result 924 (3265 B, aligned 382, frame-blind 168); that with the exponent written
// `(bits * 0.05f) * 2.3025851f` (retail's two fmul; without the parentheses the two constants fold into one) 899 (3291 B,
// frame-blind 157); that with the linear helper in matched 0x2195f0's own shape (a pin taking the long bits and returning a
// result local, then a named `scaled` product) 879 (3291 B, aligned 305, frame-blind 155): the frame is then retail's 0x4e0
// with every local on retail's home, because the pinned bits share one slot with the helper's argument. That is the lead
// saved as build/r133/lead-0x18ae80.patch. On it: a named tag local in the first loop 883, a named flags local 879 (one line
// worse aligned), both 879; the sound through an inline lookup helper 879 (same code); the duplicate flag set under an
// `if` 924; the existing list appended with `elements[count++]` (retail's `mov eax, edx; inc edx`) 880 (aligned 304); the
// allocation back through record_pool_allocate(pool) 909 (3262 B); the datum getter reading the pool global 885. On main's
// body: the single-exit decibel helper with the unfolded exponent 853 (3327 B), the unfolded exponent alone 856 (3338 B).
// Diagnostic, restored: the floor global defined in an existing file built without link-time code generation 846 (same code,
// 5 reads: that does not make stores kill it). Left on the lead (36 bytes short): the first loop gives the callee-saved
// registers to the query index and the tag where retail gives them to the index and the sound pointer (retail forms the
// pointer with `lea ebp, [ecx + eax]` and keeps the tag in ECX); retail holds the pool in EDI from the first scan to the end
// and copies it to EDX for each allocation, so EBP takes the target pointer and EBX a hoisted NONE for the whole target
// loop (`cmp esi, ebx`, `mov word ptr [ecx + 6], bx`), where ours keeps the pool in EDX across the allocation (retail's
// other callers of 0x16b840 do read EDX after it, so that is this function's allocation, not the callee's summary), and so
// has no register for NONE and hoists 0x100 instead; retail hoists 0xfdff into EBP for the first scan's word `and` and loads
// the type byte and the sign-extended source before testing them; retail reloads the sounds base after each 0x14b240 call
// (lane 13 r110's reading); retail's used-bit zeros come from one zeroed register with the existing count's zero as an
// immediate between them.
// MiniMax E (11-20): no new form, no lower score (846): retail's head keeps the entry zero in EBP and gives the target count the home at +0x24 and the primary the home at +0x34, while this build zeroes ESI and puts the count at +0x88 and the primary at +0x28, so the two locals' homes are exchanged; the decibel constant is a writable global in another file, so its fourteen loads carry a different address than retail's read-only constant.
// MiniMax B (11-20): no new form, no lower score (846): the entry zero is still EBP where retail uses ESI, the query
// count is still read as a word where retail reads a dword, and the target count and primary still have other homes;
// the permuter's best form is 842, under the 8-point bar. The decibel constant g_44a0b4 is a writable global in
// src/unknown_2197f0.cpp, so its 14 loads carry a different address than retail's .rdata constant.
// M3 lane 2 r122 (fast with 0x18bb80, 0x188f90 and 0x18a5a0, all MATCH; control 846; body unchanged). Lane 11 r133's lead
// (build/r133/lead-0x18ae80.patch in the m3-lane11 worktree) still applies: 879 at 3291 B, difflib-aligned 305 against the
// body's 1004, register- and frame-masked 157 against 476. On it, for the first loop's roles (retail forms the sound with `lea
// ebp, [ecx + eax]` and reads the tag through it into ECX; the lead loads the tag into EDI with the indexed address and leaves
// the sound in EAX): a named tag 883; the tag declared ahead of the sound 883; a named flags word read ahead of the count's
// increment 879 (masked 160); the duplicate scan through a walking element pointer 883; through a file-local inline search
// taking the list and the tag 879 (same code); the tag test ahead of the sound pointer 938; the choice's tag stored last 878;
// the duplicate scan re-deriving the element from the index 888 (3306 B, aligned 293: the tag then takes EBP); the sound
// declared at the loop top 879 (same code); the sound read through a const reference to a named element 885; the sounds base
// in a named local 879 (same code). Diagnostic (a behaviour change, reverted): one read of the sound after the 0x14b240 call
// 816 (3280 B) with the tag still in EDI, so a longer life alone does not give the sound EBP. None moves the tag out of EDI.
// Signed query ordinal with advancing gains/portals/clusters 929/918/929;
// gains and portals/gains and clusters/portals and clusters 853/931/853;
// all auxiliaries 904 or all four streams 910. Every form recovers the ESI zero,
// but the target and primary stores still differ and all scores rise.
// m3 lane 6 r221 (H2M fast with 0x18bb80/0x188f90/0x18a5a0, all MATCH; control 846): the target's step named once for
// both clamps 849; the used bit through bit_vector_set 912; the override target's decibels stored before its gain 880
// (3305 B: retail stores decibels, the 1.0f gain, audible and priority in one tail after the three decibel paths, ours
// duplicates them into each path), that with the single-exit decibel encoder 902 (3279 B), that with the targets and
// their count as one local aggregate (count first, as retail's +0x88 count ahead of the +0x8c elements suggests) 933
// (3305 B).
// M3 lane 5 r222 (H2M_FAST with 0x18bb80, 0x188f90 and 0x18a5a0, all MATCH; without the three the reduced image builds
// lane 11 r133's lead at 3216 B / 953 instead of 3291 B / 879, so screen with them). Retail evidence for that lead's counted
// lists: the choices' count sits at frame+0x24 right below the elements, is incremented ahead of the stores and saved
// (`inc esi; mov [esp + 0x34], esi`), and the element is addressed as `count - 1` after it. On the lead (879, aligned
// 305), the fraction: retail tests `0.0001 > |range|` with the long path falling through, zeroes a scratch XMM in each
// path and leaves the result in the quotient's XMM0; the lead hoists the zero into XMM2 and keeps the result there.
// Fraction as an if/else 879 (same code); the long path through the file's const-reference pin 885 (3286 B), also as an
// if/else 885, with `0.0f > distance` 885; the long path first under `>=` 861 (3302 B, aligned 343: retail's block order
// but an `fxch` and the zero still hoisted); `0.0001f <= fabs(-range)` as an if/else or a ternary 862 (3303 B, the same
// `fxch`). Main's body unchanged.
// Direct prior-tag test with short query and short/long choice count 1013/1036,
// long copied query 1030/1039, or provider count 1030/1039. Exactly one prior
// choice with short counts 1013 or provider count and long choices 1044.
// NOT kept: the forms replace the retail duplicate loop, shorten the body
// and leave the entry zero and target/query homes different.
// M3 lane 11 r146 (h2m fast with 0x18bb80, 0x188f90 and 0x18a5a0, all MATCH; control 846): the three 1.0f choice defaults
// as three statements in member order (retail stores inside, portal, outside upward; the chained assignment stores them
// downward) 846; that with the detail tag as a ternary initializer 845; plus the fade scan's datum index formed inside its
// `if` 848 (3332 B); the defaults and ternary with the four choice stores in retail's order (tag, source, decibels,
// cluster) 845; that with the audible update through a `word *` to the flags (0x18a720's spelling) 862 (3302 B).
// M3 lane 14 r138 (H2M_FAST with 0x18bb80, 0x188f90 and 0x18a5a0, all MATCH; control 846). Reading: retail's entry zero
// in ESI is the choice count's own register (`cmp esi, 2` in the query loop, `inc esi; mov [esp + 0x34], esi` before
// each choice is addressed), with the query count read as a dword; ours keeps both counts as shorts in memory and gives
// the zero to EBP (the query index). Tried: both counts long re-measured 965 (3264 B); the long choice count with the
// out count read directly (no query-count local) 965 (3264 B, same code); the choices alone as a counted list (count,
// then the two choices; the count incremented first and the choice addressed through it) 975 (3268 B, first difference
// +0x2); the query loop ending on `choice_count < 2` 846 (same code); the target count incremented after the target's
// stores 923 (3305 B).
// Long accepted count with short/word ordinals at selection or repair, short query bound
// 937/937/939/946, full query bound 967/967/952/952.
// NOT kept: all recover the ESI zero but shorten the body and leave the target and buffer homes displaced.
// Choice cap at scan-body entry with short/short, short/long, long/short and long/long query/choice counts
// 846/964/919/965. Cap after choice publication in the same width order 950/965/965/995. Widening the accepted
// count restores the ESI zero but shortens the body and leaves the array homes displaced.
// M3 lane 2 r135 (H2M_FAST with 0x18bb80, 0x188f90 and 0x18a5a0, all MATCH; control 846): the muted flag as `!g_509420`
// 846 (same code); the choice distance as one conditional expression 852; the query loop's tests as `choice_count != 2 &&
// i < query_count` 934 (3305 B); the override's suppression gain computed once ahead of its loop 959 (3380 B: retail
// evaluates it per target); the fade scan's flag test as `!(sound->flags & 0x200)` 845 (under the bar, not kept); on that,
// the fade scan as a head-tested for loop 899 (3369 B), the fade's flag set through the scanned datum 854 (3308 B: retail
// looks the datum up again), the duplicate test as `if (equal) duplicate = true;` 908, the query loop's continues as nested
// ifs 924 (3304 B), `clusters[0] == cluster` 845 (same code). Reading: retail forms the choice address from the incremented
// count in ESI (`lea esi, [esi + esi*4]; lea esi, [esp + esi*8 + 0x10]`), spills the count to +0x34 and reloads it after
// the store block, so its count is never strength-reduced. Body unchanged.
// M3 lane 3 r294 (h2m fast with 0x18bb80, 0x188f90 and 0x18a5a0, all MATCH; control 846): long counts with the query count
// read directly, the choice count incremented right after the duplicate test and every choice store addressed
// `choices[choice_count - 1]` 898 (3283 B: the entry zero is then ESI as retail and three query arrays take retail's
// homes, gains 4 bytes off, but the choice address is strength-reduced into a pointer stepped by 0x28 where retail
// recomputes it from the incremented count); that with one named `choice` pointer 960 (3267 B); with retail's store
// order (tag, source, decibels, cluster) and the three 1.0f defaults as statements 898 (same code); with a short
// choice count 930 (3304 B). The silence floor g_44a0b4 declared const where it is defined and in both files that read
// it 846 (same code here), but matched 0x2197b0 falls to 15 and 0x18d730/0x18dc90 rise, so not kept.
// M3 lane 6 r230 (H2M_FAST with 0x18bb80, 0x188f90, 0x18a5a0, all MATCH; control 846; difflib-aligned / register- and
// offset-masked 998 / 470): the existing-sound scan as a NONE-start while over data_next_absolute_index_inlined 893
// (3268 B, 977 / 453); through the existing data_iterator_next_inlined (iterator from NONE, the list storing the
// iterator's datum index) 860 (3296 B, 1022 / 415): the scan is then retail's exactly (`or esi, -1; mov ebp, 0xfdff;
// inc esi; js`, one bit-search expansion with the index advanced in place, the 0x200 clear as a word AND from the
// hoisted mask), retail evidence for the iterator, but the pool stays in EDX (retail EDI) and the score rises; that
// with long query and choice counts 993 (3225 B, 1082 / 394); with the targets and the existing list as count-first
// records (diagnostic: retail's homes put each count right ahead of its array) 905 (the frame differs); with `used` and
// the existing count declared ahead of the list 860 (same code). Body unchanged.
// M3 lane 11 r153 (H2M_FAST with 0x18bb80, 0x188f90 and 0x18a5a0, all MATCH; control 846, aligned 1004): lane 11 r133's
// lead re-measured 879 at 3291 B (aligned 305, registers and offsets masked 152). On main's body: the decibel pin helper as
// one conditional expression 905 (3337 B); `used` declared ahead of the existing count 846 (same code); the fade scan's
// near-zero test with its arms swapped under `>=` 838 (an arm swap, match-only; retail places both arms after the listener
// tail's `ret` and jumps back to the scan head, the record-pool iterator layout); the target level named ahead of the
// current level in the target loop 857 (3332 B); the first scan's datum index formed ahead of its type and source tests 846
// (same code). Body unchanged.
// M3 lane 14 r146 (H2M_FAST with 0x18bb80, 0x188f90 and 0x18a5a0, all MATCH; control 846, aligned 1004, masked 473): the
// target loop's search ending on `index == NONE` in its condition (no break) 857 (3338 B); the existing list appended as a
// store then `existing_count++` 846 (same code); the gain sum written portal first 846 (same code); the override loop
// through a named target pointer 850; the listener slot through a `long *` used for its three accesses 846 (same code).
// M3 lane 16 r179 (H2M_FAST with 0x18bb80, MATCH; control 846): the repair loop's first-choice flags read through the
// sounds table at both tests (no sound pointer) 873; the repair loop's matched sound as a named element pointer with a
// `continue` on NONE 948 (3289 B); the query loop's tag read once into a named local 846 (same code); the choice
// distance defaulted to zero in a local and stored once 924 (3304 B); the target's fade-time definition through a named
// element pointer 885. Body unchanged.
// M3 review of lane 2 r142: the 820 form below moves the definitions initialiser into the fade body, a placement kept
// only with a match, so the fade scan stays main's; kept: the first scan through the iterator and the new start (837).
// M3 lane 2 r142 (H2M_FAST with 0x18bb80, 0x188f90, 0x18a5a0, 0x16b840, 0x2195f0, 0x2197f0, 0x2197b0, 0x219650, 0x18d730 and
// 0x18dc90 in the image, all but the last two MATCH each time: without the file that defines g_44a0b4 in the image the
// screen builds this function differently). Kept, 846 -> 820 (3327 B, frame 0x4dc): both pool scans through the existing
// data_iterator_next_inlined (retail's layout: one bit search per scan, the scan bodies out of line, the fade body after
// the listener tail's `ret`, the datum index formed ahead of the type test), the fade scan's definitions read in its body
// (retail hoists only the g_4e0348 load and reads the +0xd8 field after the scan's tests), and loop_path_start allocating
// through the existing looping_sound_new_attached (inlined: retail's `mov word [ecx+4], dx; or byte [ecx+5], 1` start
// sequence at both sites). Each was listed above as a rise on its own (860, 909); measured again on main's body: the
// first scan alone 837 with the new start (863 with the helpers below), the fade scan alone 839 (848 with the new start).
// The fade scan's definitions spelled other ways on the kept form: read at each use with no local 808 (3310 B, not kept:
// the build loop still reads them through its own local, a mix), through a local ahead of the scan as main had it 861,
// one iterator for both scans 821. On the 808 spelling: the blend's square named 807; the fade's two pins through the
// existing approach_126660 820; the used bit through the existing bit_vector_set 898; the choice scale read once into a
// local 897; the build loop's definitions read at each use 885 (3298 B); the target loop's pins through approach_126660
// 850 to 855.
// Lead, not kept (build/r142/lead-18ae80.patch, 830 at retail's 3327 B and 0x4e0 frame; difflib-aligned 292 unmatched
// against 640 for main's body and 648 for the kept one, 107 with registers and offsets masked against 282 and 214): the
// form that reproduces retail's frame and statement shape. Its parts, each read off retail: (1) the choices, the targets
// and the existing list as count-first records (count, then the items; retail's homes F+0x24/F+0x28, F+0x78/F+0x7c and
// F+0xdc/F+0xe0, the existing record laid over the gains array), the count incremented first and the item addressed
// `items[count - 1]` for choices and targets (`inc esi; mov [count], esi; lea esi, [esi + esi*4]; lea esi, [esp + esi*8
// + 0x10]`: a count in memory is not strength-reduced), `items[count++]` for the existing list; (2) the query count read
// from its home at every use (no short copy) and set to the choice count ahead of the build loop (`mov [esp + 0x10], esi`,
// re-read at that loop's latch); (3) the matched sound's two overrides under `flags & 4 || flags & 2` with one pointer to
// the other sound (listed above, 906 alone); (4) loop_path_linear and loop_path_decibels in the shape of the matched
// 0x2195f0 and 0x2197f0 (a long-to-long pin with one result, so the pinned bits stay in a register and share the
// argument's home); (5) the silence floor's address taken (a const reference in the pin helper: retail reloads g_44a0b4
// after every call and store through a datum pointer, 14 loads; the reference gives them, 3240 -> 3312 B; 0x2197b0,
// 0x18d730 and 0x18dc90 unchanged; no code in retail takes the address, so the real taker is not found); (6) the target
// loop's and the fade's pins through approach_126660 and the used bit through bit_vector_set (retail's `0.0f - step` and
// the `lea` to the used word); (7) the range test as `fabs(-range) >= 0.0001f` with the division arm first; (8) the
// targets' count set ahead of `primary`. Steps on the way (fast, lead forms cumulative): records for choices and targets
// 927 (3277 B, frame 0x4e8), plus the existing record 902, plus the first scan's iterator 998 (3207 B), plus (3) 1000,
// plus the new start 953, or the helpers of (4) 985, both 960 (frame 0x4e0), plus (7), (8), the fade scan's iterator and
// the audible and muted flags as conditional expressions 931 (3240 B), plus (5) 853 (3312 B), plus (6) 842 (3327 B), the
// flags and the choice scale read into locals 830. On it: the flags through the existing word_flags_set 858 (3307 B);
// the fraction computed in place from a distance local 847 (3339 B); a tag local in the query loop 842 (same code).
// Left on the lead: the query loop's roles (retail: sound EBP, tag ECX, index i EDI; ours sound EAX, tag EDI, i EBP; a
// use of the sound after the distance call does not give them), the pool in EDX where retail has EDI and keeps NONE in
// EBX through the target loop, the `movsx` source test of the first scan, the fade-time address formed for its second read.
// M3 lane 2 r144 (H2M_FAST with the same eleven functions in the image; control 837). Retail's read-only data holds 0.0
// at 0x44a0b0, with no reference at all, and the -64.0 at 0x44a0b4 that 34 instructions in seven source files load: one
// extern object of two reals, not a literal and not a lone scalar. Written that way (a two-real table at 0x44a0b0 in
// place of g_44a0b4, its second element read at the five uses) this function reloads the floor after every call and
// every store through a datum pointer with no address taken anywhere: 907 at 3396 B on main's body (the 14 reads; 0x2197b0,
// 0x2195f0, 0x2197f0, 0x219650 still MATCH, 0x18d730 287 and 0x18dc90 295 unchanged), the same code as the pin helper
// taking its bounds by const reference 907 (3396 B). The floor defined `const` in another source (scalar or table) 837
// with 0x2197b0 lost (15) and 0x18d730/0x18dc90 at 400/398: the link folds the value across files, so retail's object
// is not const-qualified in any form this compiler sees. Lane 2 r142's lead rebuilt on current main (the first scan and
// the new start are already here): 830 with its const reference (aligned 292, masked 107), 832 with the table in its
// place (aligned 294, masked 108), so the lead no longer needs an address-taking form; build/r144/lead-18ae80-table.patch
// holds it. On that table lead: the duplicate scan as an early-exit loop over a named tag 930 (3291 B); the sounds
// array named ahead of the sound pointer 830 (aligned 292); the choice's source stored ahead of its tag 831; the
// duplicate scan stepping a named choice pointer 830 (aligned 291, masked 105). The query loop's roles (retail: i EDI,
// sound EBP formed by `lea` ahead of the tag read, tag ECX, scan counter and flags EAX; ours i EBP, tag EDI, sound EAX
// formed after the tag read, counter and flags ECX) are unchanged in all four. Diagnostic sweep of the local count (1
// to 8 unused initialised longs at the top): main's body 837 each, the table lead 830 each (aligned 292), the same code
// for every count, so none of these roles or homes is a symbol-count tie.
// Round 114: Frame 0x4dc versus 0x4e0, with the entry zero and query homes still displaced. Reusing the query count
// for conversion 837, incrementing the short choice count before its stores 915, and a duplicate break at the scan
// head 837. Primary tag held for repair 838, promoted source 837 with or without a named type byte, named fade
// records in both loops 888, and explicit range branch 837.
// The query/count homes and entry zero still differ. Const dword query 911, capacity break 837, dword query
// with tag snapshot 911, duplicate pointer walk 928, direct query bound and completed choice snapshot 931,
// dword query and duplicate ordinal 968, dword query and choice reference 927, dword construction and capacity
// break 936.
// M3 lane 11 r164 (H2M_FAST with 0x18bb80, 0x188f90, 0x18a5a0, 0x16b840, 0x2195f0, 0x2197f0, 0x2197b0, 0x219650,
// 0x18d730 and 0x18dc90; control 837). Reading of the frame: it is over 0x80 bytes, so its cells are sorted by
// reference density (densest lowest), and a separate 4-byte count cell always sorts below every array. Retail has each
// count right below its own array (the choices' count at frame+0x24 under the choices, the targets' at +0x78 under the
// targets, the existing list's at +0xdc in front of its 64 entries), and its frame is 4 bytes larger than ours because
// the existing list with its count is 0x104 bytes in the cell it shares with the gains array (ours 0x100). So retail's
// lists were count-first records (lane 2 r142's lead), not a spelling of these locals. Tried on main's body: the out
// count read at each use with no short query copy 911; the choice written through `s_loop_path_choice *choice =
// &choices[choice_count++];` 952 (3273 B); the four choice stores in retail's store order (tag, source, decibels,
// cluster) 837 and the three 1.0f defaults as three statements 837 (both the same code); the primary set inside the
// build loop as `if (i == 0 && inside) primary = choice->source;` 949 (3305 B).
// First choice reference 900; repair scan table 916; table through first
// choice lookup and tag scan 844; that table with a first-choice reference
// 903; guarded neighbor level read 925, also with a separate scan table 925;
// table and full-width repair bound 849; first tag compared first 844.
// NOT kept: the 0x4dc frame and EBP entry zero persist.
// Duplicate membership as bool/byte/long OR 954/954/948. Matching-choice tally 948. Full-width
// query and choice counts with bool OR or tally 1033/980. Retained candidate tag with bool OR
// or tally 954/948. NOT kept: the duplicate reductions shorten the body and raise the score.
// Existing approach helper for target decibels 857, target gain 859, fading decibels 840, fading gain 840,
// both target updates 857, both fade updates 839, both stored gains 864, or all four updates 857.
// NOT kept: the 0x4dc frame and displaced query homes persist, and every form raises the score.
// Active-index count-first record, increment then index or capture append slot: 877/900.
// With full-width query and choice counts: 937/934. Overlaid on the query gain buffer:
// 877/900 with short counts and 937/934 with full-width counts.
// NOT kept: the records recover the 0x4e0 frame, but the entry zero and target home still differ.
// M3 lane GA r1 (fast with 0x18bb80, 0x188f90, 0x18a5a0, 0x16b840, 0x2195f0, 0x2197f0, 0x2197b0, 0x219650, 0x18d730 and 0x18dc90;
// control 837, aligned 1015, masked 417; 0x18bb80 MATCH throughout; all restored): on main's 837 base, reading g_4ed28c directly
// (no pool local) 843; + CF decibel helpers and fade_time pointer 874 (aligned 693); real scale = g_4670fc in flags & 8 904;
// + long source + def pointer in second scan 912; single sound pointer in loop_path_start 867 (3299 B). On the count-first
// records lead (0x4e0 frame, ESI zero): base 914 (3253 B, aligned 432, masked 177); direct g_4ed28c 914; nested query ifs 914;
// fade_time pointer + def pointer + first-scan source local 926 (aligned 427, masked 172); used[2] before existing 926;
// bitfield union on flags 920 (aligned 408, pool moves to EDI); real scale = g_4670fc in flags & 8 918 (aligned 419, masked 166:
// matches retail's movaps xmm0, xmm7 scaling schedule); flags local + long source in second scan 918 (aligned 395, masked 147:
// second scan matches movsx + cmp -1); flags local before duplicate scan 910 (aligned 386, masked 147: query loop index i and
// duplicate scan pointer now take retail's EDI and spill at [esp+0x14]); build def pointer + word mask clear 919 (masked 142);
// adding a g_4686c4 writer (0x11c1b0) to --fast stops BL hoisting and puts NONE in EBX alongside pool in EDI (matches loop_path_start's
// bx stores); diagnostic with g_44a0b4 floor reloads via const-ref pin on flags-before-dup + 2nd-scan source: 818 at retail's 3327 B
// (aligned 273, masked 141; saved in build/ga1/18ae80_t20_cf_reloads_flagsdup_2ndsrc.log), and + single sound pointer in loop_path_start
// 856 (3290 B, aligned 260, masked 135: matches both loop_path_start flag/gain stores). Left on that lead: g_44a0b4's 14 reloads
// (extern two-real object in another file), query sound EBP / tag ECX swap, and first-scan movsx + word mask 0xfdff.
// Override attenuation as field or linear factor before the indexed walk 881/838,
// under the positive extent 875/887, with a live-count cursor 892/893, or with a
// fixed populated range 890/850. NOT kept: all raise the score and retain the
// 0x4dc frame; the existing-count, target-count and query-buffer homes still differ.
// The 0x4dc frame, EBP entry zero and target and query homes persist. Attenuation and flag snapshots
// keep retail length but raise the score. The conversion count and target cursor forms shorten the body.
// pre-walk attenuation 839; pre-walk attenuation with long query count 911; post-selection flags
// snapshot 838; shared flags and pre-walk attenuation 838; converted count published once 953; target
// end pointer 949; primary from both cluster outcomes 932; completed target append and primary result
// 930.
// M3 lane 2 r149 (H2M_FAST with 0x18bb80, 0x188f90, 0x18a5a0, 0x16b840, 0x2195f0, 0x2197f0, 0x2197b0, 0x219650, 0x18d730,
// 0x18dc90; control 837, aligned 646, registers and frame masked 259). Existing-helper composites on main's body (the
// helper header included): the target loop's two pins through approach_126660 with the used bit through bit_vector_set
// 909 (masked 256); plus the audible and muted updates through word_flags_set 914 (masked 257); plus the fade's two pins
// through approach_126660 914; the fade's pins with both flag setters 867 (3301 B); the used bit with the audible setter
// alone 926 (3304 B); that with the fade's pins 925 (3305 B); the audible setter alone 863 (3304 B, masked 253). The
// setters give retail's two byte operations but shorten the body (retail's word read for the audible flag stays).
// Active-list append cursor with a computed count 889/912, indexed end
// 845/912, cursor search 858/926, or counted append and cursor search
// 817/926 (short/full-width query). NOT kept: indexed forms reach 0x4e0 but
// regress. The 817 form loses gain-buffer reuse (frame 0x5dc) and shortens.
// Separate started/matched/both handles 839/860/851; continue on failed start/missing
// handle 837/837; separate match/start arms 861, with failed-start continue 838;
// separate started handle with continue 821. NOT kept: the 821 form removes retail's
// post-allocation handle test and leaves the 0x4dc frame. Its parts alone tie or regress.
// The 0x4dc frame and query-array homes still differ. Counted member append for choices, targets and
// existing indices individually 944/976/936, pairs 933/930/995, all 921, and all with a dword query count
// 930. NOT kept: the forms with an existing-index member append lose gain-buffer reuse and enlarge the
// frame to 0x5e0–0x5e8.
// Shared gain buffer reused as a count-prefixed active list, store/increment first, for short/short
// query and choice 876/877, short/long 936/946, full/short 899/901, full/long 937/937. NOT kept:
// the 0x4e0 frame returns, but the count and query homes differ and all scores rise.
// Query-only array scope, shared query ordinal, direct provider count, or both:
// 837/840/911/911. Early choice count or enclosing choice storage in that scope:
// 837/837. Early long choice count alone or with direct count/shared ordinal:
// 938/948. NOT kept: the 0x4dc frame persists and long choice counts shorten the body.
// Lane AO r3 (fast with 0x18bb80 and ten callees, control 837 at 3327 B): the targets array declared after the two
// counts 837, the primary declared ahead of the target count 837, the choices declared ahead of the query arrays 837,
// the query count declared ahead of the arrays 837 (same code each: declaration order moves no home); the duplicate scan
// as a plain loop with `break` 966 (3272 B); the fraction as an if/else 837 (same code); each target filled as
// `targets[target_count].field` with the count stepped last 940 (3289 B); the muted flag as `!g_509420` 837. Reading:
// retail walks the query with the long count read from its home, keeps the choice count in ESI with a copy at frame
// +0x24, and spills the loop index at frame +0x4 where ours puts the short query count. Body unchanged.
// Exact-end bounds for query, duplicate, repair, conversion, both target walks, active lookup,
// both query walks, or all choice and target walks: 837/905/837/837/837/837/837/905.
// The 0x4dc frame and count homes persist. Retail uses a relational duplicate bound.
// M3 lane 13 r158 (h2m fast with 0x18bb80, 0x188f90 and 0x18a5a0, all MATCH; control 837 at 3327 B; frame 0x4dc against
// retail's 0x4e0 on today's main): the fade time named once for its test and divisor 845; the sound-definition list read
// ahead of `inside` 924 (3289 B), at the block top 921; both counts long re-measured 1018 (3214 B: an opcode-aligned
// read puts retail's extra code in the second scan's inside/portal selections, the PIN clamps and the primary's detail
// lookup, not only the choice addressing); the scanned sound's tag named once for its test, the duplicate scan and the
// store 838; the range test as `fabs(range)` (no negation) 865.
// Active list with a terminal NONE slot, final or rolling marker for index/index, index/cursor,
// cursor/index and cursor/cursor append/search: 851/863/845/876/855/855/848/862. NOT kept: cursor append and indexed
// lookup restore the 0x4e0 frame but shorten the body. The other forms enlarge the frame, and
// all depart from retail's counted walk. The entry zero and count/query homes still differ.
// M3 lane 14 r157 (fast screens with 0x18bb80 and nine other callees, the eight matched ones MATCH; control 837). Frame
// reading from the back end's packer: our target count shares a 4-byte cell with two temporaries (11 references) and
// sorts low (+0x14), where retail's +0x78 cell, right below the targets, holds the target count alone (seven references
// at +0x88 in the listing), which only a count-first record (lane 2 r142's lead) places there. Forms on main's body: the
// detail tag as one conditional expression 837 (same code); the inside test written `clusters[0] == cluster` 837 (same
// code); the audible flag through an if/else 895 (3305 B); the portal distance as one conditional expression 838; the
// blend's square named once for both terms 838. Body unchanged.
// M3 lane 7 r231 (H2M_FAST with 0x18bb80, 0x188f90, 0x18a5a0, 0x16b840, 0x2195f0, 0x2197f0, 0x2197b0, 0x219650, 0x18d730 and
// 0x18dc90; control 837, 0x18bb80 MATCH each run): the three choice levels as `flags & 1 ? field : 1.0f` assignments 922
// (3289 B); the decibel pin as an if / else if / else chain 837 (same code); `0.0f - target->step` as both target pins'
// lower bound 837 (same code); the decibel pin as a single-exit result on today's base 869 (3297 B); a `char` choice count
// 903. Body unchanged.
// M3 lane 5 r250 (fast with 0x18bb80 and nine callees; control 837, frame 0x4dc): reading of the allocator's costs: every
// use of the short choice count is a widening load worth nothing to a register, so it never gets one (retail keeps a
// full-word count in ESI, stores it at each increment and reloads it after calls, as a memory member does). Tried: full-
// width query and choice counts with the out count read directly 951; count-first lists (count, then elements) for the
// choices, targets and existing indices with `list.count++` then `&list.elements[list.count - 1]` 977 at retail's 3327
// B (ESI entry zero, retail's first-loop count and index homes, instruction distance 998 against 1151, frame 0x4e8);
// that with retail's store order (tag, source, decibels, cluster) and the three 1.0f defaults as statements 977
// (distance 989); plus a scale local in the flags & 8 arm 969 (976); the linear conversion reusing its value parameter
// for the pinned bits 837 on main's body (same code) and 969 on the lists. The lists' frame is 8 bytes over retail's:
// in the target loop retail's conversions share two slots (value and bits in one slot per conversion), ours keep three
// separate address-taken temporaries alive across the loop.
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
			if (portals[i] != NONE)
				choices[choice_count].distance = function_14b240((s_14b240_owner *)bsp,
					(s_bsp3d_disk *)(bsp->portals + portals[i] * 0x24), point);
			else choices[choice_count].distance = 0.0f;
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
			real fraction = fabs(-range) < 0.0001f ? (choice->distance < 0.0f ? 0.0f : 1.0f) : PIN_126660(choice->distance / range, 0.0f, 1.0f);
			real blend = PIN_126660(fraction * fraction * 3.0f - fraction * fraction * fraction * 2.0f, 0.0f, 1.0f);
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
	s_record_pool_iterator iterator;
	s_loop_path_datum *existing_sound;
	iterator.data = pool;
	iterator.index = NONE;
	while ((existing_sound = (s_loop_path_datum *)data_iterator_next_inlined(&iterator)) != NULL)
	{
		if (existing_sound->type == 0 && existing_sound->source != NONE)
		{
			existing_sound->flags &= ~0x200;
			existing[existing_count++] = iterator.datum_index;
		}
	}
	for (long i = 0; i < target_count; i++)
	{
		s_loop_path_target *target = &targets[i];
		long index = NONE;
		for (long j = 0; j < existing_count; j++)
		{
			long candidate = existing[j];
			if (candidate != NONE && loop_path_get(pool, candidate)->tag == target->tag)
			{
				index = candidate;
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
			real gain = current + PIN_126660(delta, -target->step, target->step);
			sound->decibels = loop_path_logarithmic(gain);
			sound->flags |= 0x200;
			if (target->audible) sound->flags &= ~0x80;
			else sound->flags |= 0x80;
			if (target->muted) sound->flags |= 0x10;
			else sound->flags &= ~0x10;
			sound->listener = (char)g_4686c4;
			sound->cluster = (short)target->cluster;
			sound->gain += PIN_126660(target->gain - sound->gain, -target->step, target->step);
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
			gain += PIN_126660(0.0f - gain, -step, step);
			sound->gain += PIN_126660(0.0f - sound->gain, -step, step);
			if (fabs(gain) < 0.0001f) loop_path_get(pool, index)->flags |= 2;
			else sound->decibels = loop_path_logarithmic(gain);
		}
	}
	long detail = NONE;
	if (primary != NONE) detail = bsp->sounds[primary].detail_tag;
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
