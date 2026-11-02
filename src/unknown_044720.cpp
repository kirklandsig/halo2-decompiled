// @flags /O2 /arch:SSE /Gr
/* UNKNOWN_044720.CPP: marking the visible parts of the listed entries (built
   with SSE, apart from its neighbours in unknown_03f2b0.cpp) */

#include "cluster_mask_storage.h"
#include "material_records_storage.h"
#include "target_cache_storage.h"
#include "shader_storage.h"
#include "projection_storage.h"
#include "view_context_storage.h"
#include "unknown_11c920.h"
#include "visibility_storage.h"
#include "camera_storage.h"
#include "frame_storage.h"
#include "reset_storage.h"
#include "globals.h"
#include "index_cache_storage.h"
#include "physical_memory.h"
#include <string.h>
#include <xtl.h>
#include "unknown_030290.h"




struct s_word_bit_iterator
{
	short count;
	short index;
	dword remaining;
	dword const *words;
};

struct s_geometry_visibility_list
{
	dword *indices;
	long count;
};

PRIVATE __forceinline byte *visible_entry_geometry(s_44940_entry *entry)
{
	if (entry->tag != NONE)
	{
		byte *definition = g_4e3b44[entry->tag & 0xffff].bytes;
		byte *sections = *(byte **)(definition + 0x28);
		return *(byte **)(sections + ((entry->flags >> 9) & 0x1ff) * 0x5c + 0x34);
	}
	byte *structure = (byte *)g_4e0348;
	byte *data;
	if (!((byte)(entry->unknown00 >> 12) & 1))
	{
		byte *section = *(byte **)(structure + 0xa0) + ((entry->flags >> 9) & 0x1ff) * 0xb0;
		data = *(byte **)(section + 0x50);
	}
	else
	{
		byte *instances = *(byte **)(structure + 0x144);
		short index = *(short *)(instances + ((entry->flags >> 18) & 0x7ff) * 0x58 + 0x34);
		byte *section = *(byte **)(structure + 0x13c) + index * 0xc8;
		data = *(byte **)(section + 0x50);
	}
	return data;
}

struct s_render_part_context;
bool function_45ce0(short index, byte *out, short part, short transform_index);
void function_41040(short index, long part_index, long group, byte weight,
    s_render_part_context const *context, long material_override, bool force,
    dword and_mask, dword or_mask);

struct s_visibility_query_list
{
    dword *indices;
    long count;
    long capacity;
};
template<long N> struct s_visibility_query_storage
{
    s_visibility_query_list list;
    dword indices[N];
};

PRIVATE __forceinline void release_visibility_list(s_visibility_query_list *list)
{
    if (!(list->capacity & 0x80000000))
        g_480118->allocate((long)list->indices, (list->capacity & 0x7fffffff) * 4, 0x12);
}

transform4x3f *function_b8c00(long object_index, long *node_count);
point3f *function_142700(transform4x3f const *matrix, point3f const *point, point3f *out);
int __fastcall function_142a60(transform4x3f const *a, transform4x3f const *b, transform4x3f *result);
struct s_visibility_sphere_query
{
    point3f center;
    real radius;
};
void __cdecl function_2ddbd0(void *data, s_visibility_sphere_query const *query,
    s_visibility_query_list *second, s_visibility_query_list *first);

bool function_44690(s_word_bit_iterator *iterator, long *out);
void __stdcall function_44e90(short entry_index, short section_index);
void function_458d0(short index, bool ranges, s_geometry_visibility_list const *first, s_geometry_visibility_list const *second);
void function_45560(short entry_index, point3f const *center, real radius);

// @retail 0x44720
void function_44720(void)
{
    s_2cb30_state *state = g_4c0b78.state;
    long end = g_4c0b78.previous;
    if (state->active) end = g_4c0b78.current;
    end = (word)end;
    word index = (word)(state->active ? g_4c0b78.previous : 0);
    while (index < end)
    {
        dword entry_flags = g_4ba138[(short)index].unknown00;
        if (!(entry_flags & 1) && (entry_flags & 0x40))
        {
            s_44940_entry *entry = &g_4ba138[(short)index];
            if (*(long *)((byte *)state + 0x2a7c) == 1)
            {
                point3f center = *(point3f *)((byte *)state + 0x2a68);
                transform4x3f const *transform = *(transform4x3f **)(entry->unknown10 + 0xc);
                if (transform)
                {
                    if (transform->scale != 0.0f)
                    {
                        point3f local;
                        local.x = center.x - transform->position.x;
                        local.y = center.y - transform->position.y;
                        local.z = center.z - transform->position.z;
                        if (transform->scale != 1.0f)
                        {
                            real inverse = 1.0f / transform->scale;
                            local.x *= inverse; local.y *= inverse; local.z *= inverse;
                        }
                        center.x = transform->forward.k * local.z + transform->forward.j * local.y + transform->forward.i * local.x;
                        center.y = transform->left.k * local.z + transform->left.j * local.y + transform->left.i * local.x;
                        center.z = transform->up.k * local.z + transform->up.j * local.y + transform->up.i * local.x;
                    }
                    else
                    {
                        center.x = 0.0f;
                        center.y = 0.0f;
                        center.z = 0.0f;
                    }
                }
                function_45560((short)index, &center, *(real *)((byte *)state + 0x2a74));
            }
            else if (entry_flags & 8)
            {
                long section = *(short *)(entry->unknown10 + 8);
                dword const *words = (dword const *)((byte *)g_4c0b78.state + (section + 0xa6) * 16);
                s_word_bit_iterator iterator = { 4, 0, words[0], words };
                long visible;
                while (function_44690(&iterator, &visible))
                    function_44e90((short)index, (short)visible);
            }
            else
                function_44e90((short)index, *(short *)(entry->unknown10 + 8));
        }
        ++index;
    }
}
// score 107 (was 107): the frame is still 0x44 against retail's 0x40, the entry is addressed
// through a pointer instead of retail's index-then-pointer form, and the geometry lookup keeps
// the flags word; this round's variants all scored worse, so the body is unchanged.
// Earlier: frame 0x44 versus retail 0x40; entry-offset construction
// and the geometry flag reloads differ.
// Round 125 (one-link LTCG harness with the callers' and callees' files, exact for this function):
// level != 31 at the call gives retail's frame (harness aligned 70 -> 50) but 135; on it the
// lookup taking the short index and addressing the entry itself gives retail's entry offset, the
// in-place transform and the flags re-read, but hoists the structure pointer and re-reads the
// first word 136 (0x45560 228); transform before weight 136; the lookup with one result variable
// 132; the parts under `if (geometry)` 135; split continue tests, material order and the selected
// ternary's parentheses compile the same.
// Earlier: level != 31 at the call (no cached byte) gives retail's 0x40 frame
// and call setup, 135 (487 bytes, aligned 42 against 59); retail also re-reads the flags
// word in the inlined geometry lookup where ours keeps it. On that form: short transform
// 141; material index read first 135; transform first 136; the lookup given
// &g_4ba138[(short)index] 136 (503 bytes, flags re-read, but the scenario structure
// pointer hoisted into ebx).
// Native context at part/entry/function scope with cached/live level 107/135 each;
// native context with byte/word level 133/138. Original storage restored; frame and geometry reloads remain.
// Round 420: global each-use 107; global each-use/live level 135; global each-use/indexed receiver 140;
// global each-use/live level/indexed receiver 136; global each-use/single result 132; global
// each-use/single result/live level 132; global each-use/single result/indexed receiver 143; global
// each-use/single result/live level/indexed receiver 128. Original body restored.
// Round 36 (one-file harness, this file plus both callees' files: identical to the full build): on the
// level-at-call form the structural difference is 23 instructions (entry pointer built before its
// first load, the flags kept in EAX through the lookup, no reload at the geometry join). On it every
// subset of: weight read at the call, short transform, ternary material, fallback masks swapped,
// material index by the long index, `geometry == NULL` (63 forms) none closer; a `dword` selector
// local read first (the 0x44720 form) or a `flags` local for the level and transform tests none
// closer; the lookup as an ordinary or forceinline function in another file stays a call.
// Round 33 (fast, agreeing with the full count on the unchanged source): with the level at the call
// and the lookup given `&g_4ba138[(short)index]` (136, 503 bytes) the entry address and the transform
// (`and eax, 0xf`, flags re-read in each lookup path) are retail's, but g_4e0348 is hoisted into EBX,
// the index and entry swap EDI/EBP, the level (not the weight) takes EBX in the inner loop and the
// non-tag path re-reads the first word. On that: the 0x44720 selector local (`dword entry_flags`) 136;
// the selector local alone with the level at the call 146; the expression argument on the cached-byte
// form 140; the lookup taking the entry index (both callers) 136 / 140 on the cached form, and
// 0x45560 190 -> 228.
// Round 44: lookup given `&g_4ba138[index]` (long index) 177, with level at the call 169; first test read through
// `g_4ba138[(short)index]` with the entry pointer after it and level at the call 139 (508 bytes), plus the indexed
// lookup 150; indexed lookup with the weight read at the call and level at the call 140 (503 bytes). Original kept.
// Round 453: Ordinary inline query/submission/both contracts 107/107/107;
// with live level 135 each. Both inline contracts with native context, cached/live level 107/135.
// Original contracts and context storage restored; frame, entry offset and geometry reloads remain.
// Round 45 (lane 10; one-link harness exact for this function, fast agreeing): 1..32 unused locals before the
// loop leave the current form and the level-at-call form unchanged (no numbering ties). On the level-at-call form
// (135): transform index before the weight 136, after the geometry lookup 150; weight after the material 161;
// transform index after the material 146; weight, transform and material after the lookup 145. Retail's lookup
// re-reads the flags word after `and eax, 0xf` and keeps tag/definition in EAX/EDX, so its geometry joins through
// the stack slot (store, then `mov eax, [esp + 0x10]`); ours keeps the flags in EAX and copies them for the mask.
// Round 47 (lane 10, fast with 0x45560 listed): on the level-at-call form (135, 487 bytes) the structure global
// read in each lookup branch 135 (0x45560 190 -> 246); the transform index written at the 0x45ce0 call 163; the
// material as a ternary 140; the selected material written at the 0x41040 call 135. On the current form: the
// structure global in each branch 107 (same code; 0x45560 246); the material as a ternary 127. Retail's 0x45560
// reads the structure global and the flags above the instance test, its 0x44c90 inside each branch.
// Round 32: 0x458d0's matched lookup loads the structure global once above its branch, so the per-branch
// reloads are this function's own; retail's loop callees reach none of the global's writers. Level test at
// the call 135 (frame, slots and EDI/EBP/EBX roles retail's); with the lookup given `&g_4ba138[(short)index]`
// 136 (global hoisted out of the loop into EBX); plus the global read in each lookup branch 136 (still
// hoisted; 0x45560 246); level test at the call with the transform after the lookup 150; `use_level` once
// before the outer loop 158.
// Round 467: Accepted entry level named at its original read in long/short/word/byte,
// used by admission and cached submission predicate 155/151/151/152; predicate at its
// submission use 152/151/151/149. Original level/Boolean ownership and full body restored.
// Round 19 (lane 17, fast with 0x44720/0x45560/0x458d0/0x44e90/0x44690): BLOCKED-BY 0x41040 and
// 0x45ce0 (todo, outside this machine's files): the inner loop's call setup and the hoisting around it
// follow those callees. Level at the call re-measured 135 (487 bytes); with the entry as `g_4ba138 +
// (short)index` 135 (same code). Diagnostics (mixed entry/global reads): level at the call with the
// transform read as `g_4ba138[(short)index].flags & 0xf` 136 (503 bytes); the cached byte with that
// transform read 147 (508); level at the call, that transform and the lookup given the indexed entry
// 157 (515).
// Round 326: material selector at each table use, cached/live level and pointer/indexed geometry
// inputs 107/135/140/136; the same four with conditional material production 127/140/150/119.
// Original selector, predicate and complete body restored; frame and geometry reloads remain.
// Round 52 (lane 1, fast with 0x44720): on the level-at-call form (re-measured 135, 487 bytes) the material
// index as a long 132; that with the lookup given `&g_4ba138[(short)index]` 130 (510 bytes); with the weight
// also a long 145; the entry as `g_4ba138 + (short)index` 135. On the current form the material index as a
// long 126. Retail's material index takes EAX after the in-place `and eax, 0xf` (the lookup re-reads the
// flags); ours loads it into ECX and keeps the flags in EAX through the lookup.
// Round 113 (lane 8, fast with 0x44720/0x45560): retail's helper flags reads do not share the level
// test's read (diagnostic only, a volatile flags read in the helper with the level at the call: 121, 494
// bytes, with retail's in-place `and eax, 0xf`, per-path flags reloads and per-arm g_4e0348 loads), while
// its first-word read does (the non-tag path tests the top's EDX). Forms: the helper reading g_4e0348 in
// each arm 107 (still loaded once above the branch; 0x45560 246); level at the call with the first word read
// as `g_4ba138[(short)index].unknown00` in both tests and the helper given `&g_4ba138[(short)index]` 136
// (retail's entry formation and transform, but g_4e0348 hoisted out of the loop into EBX), with the helper's
// per-arm global reads 136 (still hoisted; 0x45560 246), with the first test ahead of the entry pointer
// 136; the flags read through `g_4ba138[(short)index]` at the top 140 (506 bytes, entry folded away).
// BLOCKED-BY 0x45ce0 / 0x41040: both loop callees are unmatched in other owners' files, and our LTCG
// hoists g_4e0348 across them where retail reloads it per arm.
// Round 330: Native entry level/transform/both 107/107/107; complete geometry field
// views with live level 135; with native level/transform/both 107/107/107; all views
// with live level 135. Original entry type, selector and body restored; frame and reloads remain.
// Round 19 (lane 17, fast with 0x44720/0x45560/0x458d0/0x44e90/0x44690): BLOCKED-BY 0x41040 and
// 0x45ce0 (todo, outside this machine's files): the inner loop's call setup and the hoisting around it
// follow those callees. Level at the call re-measured 135 (487 bytes); with the entry as `g_4ba138 +
// (short)index` 135 (same code). Diagnostics (mixed entry/global reads): level at the call with the
// transform read as `g_4ba138[(short)index].flags & 0xf` 136 (503 bytes); the cached byte with that
// transform read 147 (508); level at the call, that transform and the lookup given the indexed entry
// 157 (515).
// @retail 0x44c90
void __stdcall function_44c90(long group, dword selection_mask, long level,
    dword and_mask, dword or_mask, dword fallback_and_mask, dword fallback_or_mask, bool force)
{
    long end = (word)(!group ? g_4c0b78.previous : g_4c0b78.current);
    long begin = !group ? 0 : (word)g_4c0b78.previous;
    for (long index = begin; index < end; ++index)
    {
        s_44940_entry *entry = &g_4ba138[(short)index];
        if (!((entry->unknown00 & selection_mask) & 0x1fffff) ||
            ((entry->flags >> 4) & 0x1f) != level || !(entry->unknown00 & 1) || entry->unknown10[0xa] <= 0) continue;
        bool use_level = level != 31;
        byte weight = g_4c0b78.weights[index];
        long transform_index = entry->flags & 0xf;
        byte material_index = g_4c0b78.material_indices[(short)index];
        long material = NONE;
        if (material_index != 0xff)
            material = g_4c0b78.materials[material_index];
        byte *geometry = visible_entry_geometry(entry);
        if (!geometry) continue;
        if ((*(byte **)(entry->unknown10))[0x76] == 0xff)
        {
            and_mask = fallback_and_mask;
            or_mask = fallback_or_mask;
        }
        for (long part = 0; part < *(long *)geometry; ++part)
        {
            byte context[0x2c];
            if (function_45ce0((short)index, context, (short)part, (short)transform_index))
            {
                long selected = force || (entry->unknown00 & 0x8000) ? material : NONE;
                function_41040((short)index, part, group, weight, (s_render_part_context *)context,
                    selected, use_level, and_mask, or_mask);
            }
        }
    }
}
// score 182 (was 182): Frame 0x1094 agrees; entry/index spill and center/query/bound homes remain.
// Earlier: Frame 0x1094 agrees; entry/index carriers, entry spill and sphere/position homes remain;
// retail also keeps a separate center point (+0x30) beside the sphere query (+0x20).
// Round 81 (fast): a `visible_word` pointer for the test and both marks 190 (same code); one marking
// site after the bound branch with `if (!(r * r >= d)) continue;` 210 (the marks become retail's single
// tail block, word pointer reloaded), with `<` 210; on that a function-scope center copy and the query
// local to the list branch 211, unaligned 209 (entry then in esi spilled at +0x44 as retail); plus a
// long index local for the table and call 216/209.
// frame 0x1094 agrees; entry/index registers, entry spill and sphere/position homes differ.
// Earlier 190 (was 196): frame 0x1094 agrees; entry/index registers, entry spill and sphere/position homes still differ.
// Transform inline/noinline declarations separately and together 196; chained counts
// or capacities 194/194; indexed geometry input 229; memory bound/center copies 196/196;
// radius before displacements 196; both list initialization chains 190.
// Frame 0x1094 agrees; entry/index registers, entry spill and sphere/position homes differ.
// external entry reference 196; read-only geometry 196.
// common sphere radius 227; bound center at use 203.
// bound radius before node query 204; unsigned node cache 196.
// radius plus geometry guard 227; list field initialization 194.
// Earlier 190 (was 190): entry/index spill and sphere/position homes still differ.
// Aligned split center, both points or bound alone 199/199/199; aligned bound with
// the original query 197; ordinary query with aligned center 199; complete native
// query with aligned center, bound or both 205/205/205.
// entry/index and sphere/position homes still differ.
// Small geometry first 230; visible guard 190; absent-bound guard 190; both 190;
// live bound count 190; read-only nodes 190; bound-index field 224; live visibility word 190.
// Entry/index and sphere/position homes remain; node-count branch/entry homes
// 190/190; both list entry lifetimes 191; bound reference 198; completed distance/radius/both
// squares 190/190/190; count/cache boundary 190.
// Earlier 190 (was 190): frame 0x1094 agrees; entry/index registers, entry spill and point homes remain.
// Both buffer pointers first 194; second pointer first 194; capacities/counts before pointers 188
// (two-point gain rejected); counts/pointers/capacities 192; large radius first 193; native query
// alignment 190; direct object-index field 190; read-only bound source 190. Opening body restored.
// Indexed mark/tag/link/read-only receivers 228/271/271/271; large center aggregate copy 202;
// signed-short node cache 216, with byte node 234, with selective indexed reads 285.
// Earlier 190 (was 190): entry/index spill and sphere/position homes remain.
// entry/index spill and sphere/position homes remain.
// Native bound sphere with branch/entry count homes 196/196; aligned bound 203/203.
// Native bound with split center/query 197 for both count homes and either bound alignment.
// All native sphere copies restored; no field observation or float grouping retained.
// Shared point setter at initial/query/bound producers 245/190/204; initial+query/initial+bound/query+bound 245/246/204.
// All three point setters 246; native bound-point initializer 204.
// Round 366: Native link/part/transform records and their pair/all combinations each 190;
// all records with uniform node-byte observations 219. Entry spill and point homes remain.
// Round 368: first/second list alignment 16/8, 8/16, 8/8, 16/4, 4/16,
// 8/4, 4/8, 4/4 with aligned query 190/190/190/190/190/190/190/190.
// Combined tag snapshot/live node fields, original/split center 219/221; with shared marking
// 231/232; branch-scoped count gives the same four 219/221/231/232. All restored.
// Selector read-only input 190; packed-flags value alone/with read-only input 236/236; selector
// tag value alone/with read-only input 190/190; both metadata values 236, with read-only input
// 236, with one final geometry result 236. Entry/index and point homes remain.
// Round 324: index/radius/center contract 192; radius/index/center 192; center/index/radius 190;
// radius/center/index 192; the same orders with a full absolute ordinal 193/193/191/193.
// Round 207: native list-storage construction in pointer/count/capacity, count/capacity/pointer,
// value-initialized-header and shared member forms 247 each; the same four with uniform
// at-use geometry selection 1000 each. All lose the matched caller; complete original restored.
// Round 214: complete native large sphere, point-copy/component initial center, after/before lists
// 207/235/205/234; component sphere initializer on those four graphs 199/247/201/247. All restored.
// Round 148: coherent inline point/node/dispatch, point+node, point+dispatch, node+dispatch and
// all three 190 each; coherent inline point with separate noinline 190.
// Round 232: first header split 220; second header split 216.
// both headers split 235; natural headers/aligned buffers 236.
// all natural split storage 236; all aligned split storage 235.
// first natural header/aligned buffer 220; second natural header/aligned buffer 216.
// Round 240: reversed ordered sphere comparison alone/with distance/radius/both squares 189/189/189/189;
// those forms with the existing point-difference helper 190/190/190/190. One-point gain rejected; original comparison restored.
// Round 179: link snapshot 190; bitmap field at each use 257; both 215;
// geometry at each use alone/with link/bitmap/both 1004/1004/1042/1057;
// all receiver forms with short node cache 1061. Original observations restored.
// Round 331: dispatch inline/noinline, node query inline/noinline, both inline,
// both noinline and either mixed pair: all eight 190f. Entry/index and point
// homes remain; both caller-local declarations and original body restored.
// Round 153: one complete query at entry/original capture 264/260; each with shared marking 254/220; removing only query alignment gives 263/259/253/219. Aligned forms grow frame to 0x10a4; best remains worse than 190. Original query, marking sites and alignments restored.
// Round 76: retail callers (here and 0x44e90) sign-extend the index before pushing it to 0x458d0, which
// fits a long parameter there: 0x458d0 long index stays matched but 191 here, 0x44e90 434 -> 449. On top of it:
// geometry from the element before the entry pointer 220f (index edi/entry esi as retail, frame 0x10a4);
// query after the entry/geometry 191f; entry as pointer sum 191f; entry scoped to the loop branch 220f; with a
// separate entry-time center point for the loop 221f, and the query in the list branch 221f. All restored.
// Round 155: Real part-table byte/record cursors, mutable/read-only, each with original
// or successful-query-local transform lifetime: 222/222/222/222/222/222/222/222. Original indexed walk and transform lifetime restored.
// Lane 4 r98: retail reads the node byte at each of its three uses (bound kept in ebx, geometry
// reloaded from its slot). Node byte at each use with long casts 219 (frame 0x10a4), without 202 (with a
// long index local, bound now in ebx as retail); a long index local alone 191 (retail's single movsx
// pushed for the 0x458d0 call); a function-scope plain center copy beside the branch-scoped aligned query
// 199 (frame 0x10a4: the query no longer shares the bound position's slot), with the long index 200.
// Round 224: Uniform node-byte observations with branch/broad count, initialized/filled output
// and original/success-local transform lifetimes 219/199/223/199/219/199/223/199. Original node snapshot and scopes restored.
// Round 230: native list count/capacity signedness and readonly release-input metadata,
// four header forms and their readonly-release pairs, all eight 190. Original types restored.
// Round 28 (m3 lane 12): retail's frame: query +0x20 shared with the loop's position, function-scope center
// copy +0x30, loop bit mask +0x3c; its marking block is one tail-merged copy of the two sites. Long index
// used for table and call 191 (index esi/entry edi, retail the other way); indexed entry at each use 271;
// function-scope center copy with the query local to the list branch 199 (frame 0x10a4), unaligned 192;
// one marking site with a `continue` 210 (comparison flips). All restored.
// Round 30 (m3 lane 12): query declared after entry/geometry 190 (identical); geometry helper taking the
// index (as the sibling file's) 228, 0x44c90 140; geometry helper with one result exit 188 but 0x44c90 107->132;
// bound position read at each use 198; transform at function scope 190.
// Lane 4 r106 (fast): a named visibility-word pointer for the test and both marks 190; with the
// function-scope center copy and the query in the list branch 199 (frame 0x10a4), with that query
// unaligned 192; the word pointer with a long index for table and call 191; the existing
// bit_vector_test/bit_vector_set 222 (890 bytes). Retail still shares one marking block.
// Round 63: aligned query/position union with separate center 202 (frame 0x10a4); original-name and
// reversed-declaration forms 202/202, with long index 203; center lexical rename 202; long index with
// const entry 191; entry/index declarations swapped 191; short index local 190. Original body restored.
// Round 180: Large query declared after both list headers with separate center; bound
// alignment/library center capture subsets 199/199/199/199. Complete native query copy on the same
// four graphs 207/207/207/207. Separate storage grows frame to 0x10a4; all original alignments,
// copies, calls and full body restored.
// Round 302: Nested node-cache guards NONE-first/cache-first with original/filled count
// and original/radius-first scheduling 190/190/191/191/190/190/191/191. Original guards,
// count initializer and complete executable body restored; entry/index and point homes remain.
// Round 437: One intersection result and a single marking block, crossed with read-only
// geometry/native bound sphere/uniform node reads, 223/223/223/223/234/234/242/242. All
// conditional-result forms restored; all matched controls survived.
// Round 446: Native center reference/own inline declaration/both 190/190/190;
// each with the full table/dispatch ordinal 191/191/191; explicit stdcall with
// original/reference center 190/190. Original definition, declaration and sole caller restored.
// Round 453: Library entry/query/both center copies 190/202/202; bound position alone,
// with entry/query/both 190/190/202/202; complete point-producer memmove 246 (caller 37).
// All original native and component copies restored; entry/index and point homes remain.
// Round 467: Displacement/combined-radius declaration orders ZYXR/ZYRX/ZRYX/RZYX,
// YZXR/YZRX/YRZX/RYZX, all 190 at 874 bytes. All expression trees, original declaration
// order, query/position storage and the complete body restored; entry spill remains.
// Round 324: positive node-count admission with byte-sentinel removal,
// provider-filled count and read-only node-array subsets 0-7,
// 190/182/191/192/190/182/191/192. Positive count and removal of the
// impossible byte sentinel retained; entry/index spill and point/query homes remain.
// Round 326: whole-traversal tag snapshot before/after geometry or at the small branch 218/218/218;
// the same inputs with separate center/query 219/219/219; branch count output on the first/third
// snapshot 218/218. Entry/index spill and point/query homes remain; complete original body restored.
// Round 113 (lane 8, fast with 0x44720/0x44c90): retail's frame is query +0x20 (shared with the loop's bound
// position), a function-scope center copy +0x30 that the node transform writes and the distances read, then
// the bit/visible/entry/word spills +0x3c..+0x48 and the transform +0x4c. That shape (`point3f point = *center`
// in place of `query.center = *center`, the transform into `&point`, the bound position copied into
// `query.center`, distances `point - query.center`) 198 (frame 0x10a4: our spills go ahead of the aligned
// query); with a long index local 199; `point` declared ahead of the query 198; the copy as the first
// statement 202 (875 bytes); the query without `__declspec(align(16))` 191 (frame 0x1094 again, but query
// +0x2c and point +0x3c).
// Round 328: Small-branch-only initial center, crossed with existing vector difference,
// provider-filled count and success-local transform homes, subsets 0-7:
// 219/221/229/233/219/221/229/233. Original center producer, helper boundary, count initialization and
// matrix lifetime restored; entry/index spill and point/query homes remain.
// Round 330: Native entry section/instance-ordinal/instance-bit views in seven nonempty
// subsets all 182; complete views with read-only selector input 182. Actual entry type,
// helper and complete body restored; entry/index spill and point/query homes remain.
// Round 332: Masks computed at each use with word cache, filled count and inner matrix scope, subsets
// 0-7: 215/226/202/217/215/226/202/217.
// Original executable source restored; no eligible gain.
// Round 499: All three squared-displacement products named, with completed distance/radius and
// reversed ordered comparison subsets: 182/184/182/184/182/184/182/184. Original products, frame
// and body restored; entry spill and point homes remain.
// Round 223: Existing node-cache publication before tag/after tag/after matrix
// 182/182/182; with provider-filled count 194/194/192; before-tag short/dword
// cache 209/182. Original cache timing, count storage and complete body restored.
// @retail 0x45560
void function_45560(short entry_index, point3f const *center, real radius)
{
    __declspec(align(16)) s_visibility_sphere_query query;
    s_44940_entry *entry = &g_4ba138[entry_index];
    byte *geometry = visible_entry_geometry(entry);
    query.center = *center;
    if (geometry)
    {
        if (*(long *)(geometry + 0x30) > 0)
        {
            __declspec(align(16)) s_visibility_query_storage<512> first, second;
            first.list.indices = first.indices;
            first.list.count = second.list.count = 0;
            first.list.capacity = second.list.capacity = (long)0x80000200;
            second.list.indices = second.indices;
            query.center.x = center->x;
            query.center.y = center->y;
            query.center.z = center->z;
            query.radius = radius;
            function_2ddbd0(*(void **)(geometry + 0x2c), &query, &first.list, &second.list);
            function_458d0(entry_index, true, (s_geometry_visibility_list *)&second.list,
                (s_geometry_visibility_list *)&first.list);
            release_visibility_list(&first.list);
            release_visibility_list(&second.list);
        }
        else
        {
            dword *visible = *(dword **)(entry->unknown10 + 4);
            long cached_node = NONE;
            transform4x3f transform;
            for (long i = 0; i < *(long *)(geometry + 8); ++i)
            {
                dword bit = 1 << (i & 31);
                if (!(visible[i >> 5] & bit))
                {
                    word bound_index = *(word *)(*(byte **)(geometry + 0xc) + i * 8 + 4);
                    if (bound_index == 0xffff)
                    {
                        entry->unknown00 |= 1;
                        visible[i >> 5] |= bit;
                    }
                    else
                    {
                        byte *bound = *(byte **)(geometry + 0x14) + bound_index * 20;
                        point3f position = *(point3f *)bound;
                        if (entry->tag != NONE)
                        {
                            long object_index = **(long **)entry->unknown10;
                            if (object_index != NONE)
                            {
                                long node = bound[0x10];
                                if (node != cached_node)
                                {
                                    long count = 0;
                                    transform4x3f *nodes = function_b8c00(object_index, &count);
                                    if (count > 0)
                                    {
                                        byte *tag = g_4e3b44[entry->tag & 0xffff].bytes;
                                        function_142a60(&nodes[node], (transform4x3f *)(*(byte **)(tag + 0x4c) + node * 0x60 + 0x28), &transform);
                                        function_142700(&transform, center, &query.center);
                                        cached_node = node;
                                    }
                                }
                            }
                        }
                        real dx = query.center.x - position.x;
                        real dy = query.center.y - position.y;
                        real dz = query.center.z - position.z;
                        real combined_radius = *(real *)(bound + 0xc) + radius;
                        if (combined_radius * combined_radius >= dz * dz + dy * dy + dx * dx)
                        {
                            entry->unknown00 |= 1;
                            visible[i >> 5] |= bit;
                        }
                    }
                }
            }
        }
    }
}
