// @flags /O2 /arch:SSE /Gr
/* STRUCTURES.CPP: queries of the structure bsp's render geometry: the
   lightmap triangle under a collision point */

#include "unknown_11c920.h"
#include "unknown_0259d0.h"
#include "globals.h"
#include "geometry_cache.h"

struct s_collision_result_1697c0;
bool __stdcall function_1697c0(long flags, point3f const *point, vector3f const *vector,
    long ignore_object_index, long ignore_unit_index, s_collision_result_1697c0 *result);
short function_10a0b0(long object_index);
bool function_bf9a0(long object_index, long *render_index, long *animation_index);
extern vector3f *g_4687bc;

/* a range of a section's triangles: the cluster (or instance definition)
   it is in, and its first triangle reference (8 bytes) */
struct s_structure_surface_range
{
	short cluster_index;
	short count;
	long first;
};

/* a triangle reference: its first index in the section's strip, its
   lightmap part, and its collision surface (8 bytes) */
struct s_structure_triangle_reference
{
	word first_index;
	word part_index;
	long surface_index;
};

/* a collision bsp's surfaces, as read here: the plane first (8 bytes) */
struct s_structure_collision_surface
{
	short plane_index;
	byte unknown02[6];
};

struct s_structure_collision_bsp
{
	long surface_count;
	s_structure_collision_surface *surfaces;
	byte unknown08[0x40 - 0x08];
};

/* a section's vertices: the count and the data, at an offset in the
   section's block (0x20 bytes) */
struct s_structure_vertex_block
{
	word unknown00;
	word count;
	byte unknown04[4];
	long offset;
	byte *data;
	byte unknown10[0x20 - 0x10];
};

/* a section of render geometry (0x44 bytes): its parts (the list
   function_16e1b0 searches), its strip indices and its vertices */
struct s_structure_section
{
	long part_count;
	void *parts;
	byte unknown08[0x20 - 0x08];
	long strip_index_count;
	word *strip_indices;
	byte unknown28[0x38 - 0x28];
	long vertex_block_count;
	s_structure_vertex_block *vertex_blocks;
	byte unknown40[4];
};

/* a cluster (0xb0 bytes) and an instanced geometry definition (0xc8 bytes)
   both start their geometry the same way */
struct s_structure_cluster_view
{
	byte unknown00[0x28];
	s_geometry_block_info field_28;
	byte unknown4c[4];
	s_structure_section *sections;
	byte unknown54[0xb0 - 0x54];
};

struct s_structure_instanced_geometry_definition
{
	byte unknown00[0x28];
	s_geometry_block_info field_28;
	byte unknown4c[4];
	s_structure_section *sections;
	byte unknown54[0x74 - 0x54];
	s_structure_collision_surface *surfaces;
	byte unknown78[0xb8 - 0x78];
	long surface_range_count;
	s_structure_surface_range *surface_ranges;
	byte unknownc0[4];
	s_structure_triangle_reference *triangle_references;
};

struct s_structure_instance
{
	transform4x3f matrix;
	short definition_index;
	byte unknown36[0x58 - 0x36];
};

struct s_structure_bsp_view
{
	byte unknown000[0x18];
	s_structure_collision_bsp *collision_bsps;
	byte unknown01c[0x30 - 0x1c];
	s_structure_surface_range *surface_ranges;
	byte unknown034[0x50 - 0x34];
	s_structure_triangle_reference *triangle_references;
	byte unknown054[0xa0 - 0x54];
	s_structure_cluster_view *clusters;
	byte unknown0a4[0x13c - 0xa4];
	s_structure_instanced_geometry_definition *instanced_geometry_definitions;
	byte unknown140[4];
	s_structure_instance *instances;
};

/* a collision result, as read here */
struct s_structure_collision_result
{
	long type;
	byte unknown04[4];
	point3f point;
	byte unknown14[0x3c - 0x14];
	long instance_index;
	byte unknown40[0x4c - 0x40];
	long surface_range_index;
	byte unknown50[4];
	long plane_index;
};

/* the lightmap triangle found */
struct s_structure_lightmap_triangle
{
	long cluster_index;
	long instance_index;
	byte unknown08[0x18 - 0x08];
	long unknown18;
	byte unknown1c[4];
	long part_index;
	long part_offset;
	long lightmap_part_index;
	byte unknown2c[4];
	real u;
	real v;
};

bool structure_get_lightmap_triangle(s_structure_collision_result const *collision,
    s_structure_lightmap_triangle *triangle);
bool __stdcall function_14a8e0(s_structure_collision_result const *collision,
    long model_index, s_structure_lightmap_triangle *triangle);

/* the collision a surface query starts from, passed by value */
struct s_surface_query_source
{
    void *collision;
};

/* whether the current structure bsp is loaded and its collision usable */
static __forceinline bool structure_bsp_collision_available()
{
    return g_4e0344 && g_4e0344->count > 0 && g_4e0348 &&
        *(long const *)((byte const *)g_4e0344->bsp + 0x1c) != NONE &&
        *(long const *)((byte const *)g_4e0344->bsp + 4) == *(long const *)((byte const *)g_4e0348 + 8);
}

// MiniMax I (11-06): 45, not kept: the model query's result stored in `result` with a new `goto done` to the
// shared exit in place of the early return (retail's 286 bytes, same code otherwise); a goto main's body does
// not have is kept only with a byte match.
// score 53 (was 53): frame 0x60/prefix agree; exclusion BL and source-slot rewrite/reload remain absent; the byte result
// is still normalized with setne at both exits.
// Earlier at 53 (was 61): the 0xbf9a0 probe is inverted, so the 0x14a8e0 return is laid out after the requery as
// in retail. MiniMax E (10-31): with the type-6 test also setting a named exclusion flag 51 (a test-set flag,
// not kept without a match); VC still threads the flag on every edge (retail keeps it in BL) and the byte
// result is converted with setne.
// Earlier at 61 (was 61): VC threads the exclusion flag on every edge (retail keeps it in BL: `mov bl, 1;
// jmp` in the scenery arm, `xor bl, bl` before 0xbf9a0 and `test bl, bl` again after it, so
// &triangle->c takes EDI and the collision lives in its parameter slot: stored after the requery,
// reloaded after 0xbf9a0); the byte result is converted with setne. Retail 0x14af40 has the
// identical idiom. Tried: read-only object 68; unsigned object handle 68; word query buffer 68;
// native collision record 71; external triangle reference 68; source publication 68; byte result
// merge 68; query workspace names 68; native record retested with byte result storage; model
// inline/noinline 68/68; state inline 68; both contracts 68; probe result 68; model result 68;
// final found result 111; triangle inline 68; the collision re-read from the parameter at the join
// 68 (with a block-local flag 68); the flag as one && expression tested twice, with returns 68; the
// same with an else-if tail and a final else for the no-object case 71 (286 bytes); assigned with
// the && expression, tests unchanged 68; an inline helper for the scenery test stays out of line
// and is threaded 113/111; a switch on the object type with the scenery case falling into the
// general case 68, or the flag set in a one-case switch 68; the flag from the && expression with
// the model query nested under `if (!excluded)` 68; the requery in an inline helper that updates
// the collision through its address 90; found cleared in the scenery arm with no flag 68; bool
// result 78 (262 bytes: setne gone); the flag set in both arms with `if (excluded) goto done;` 68,
// and with a bool result 78; a pointer parameter passed by address to an inline probe helper 134;
// the by-value source passed by address to that helper 86/76; multi-consumer object policy,
// short/long and bool/byte found in object scope 117/117/116/116, at function scope 68/68/61/61
// (long policy reaches 286 bytes); `excluded = function_10a0b0(...) != 2` inside the type test
// with `if (excluded) goto done` or `if (!excluded)` guards 54/54 (286 bytes, setne and the
// threaded exit stay; a flag set inside a test, not kept); the object part in an inline helper
// returning `!excluded` 106 (keeps the flag in BL and the join test, adds a sete); the final test
// nested in the requery arm 77; `return function_14a8e0(...)` with an && flag 77 (263 bytes); a
// named `instance` bool from `!excluded && 0xbf9a0` 68; the exclusion clearing found with a
// found-only final test 68; the flag in two steps (type test, then `!= 2`) 92 (298 bytes: keeps
// the join test after 0xbf9a0, but as sete/setne); an inverted `lightmapped` flag from `||` 68; a
// single-exit `handled` flag also set after 0x14a8e0 68. A compile-only screen of ~190 flag,
// guard and tail forms threads the flag in every one; only a computed value in the join (a
// negation, a widening, a store, a setcc result) keeps retail's un-threaded flag test.
// retail keeps an exclusion flag in BL (`mov bl, 1` on the scenery arm, `xor bl,
// bl` before 0xbf9a0) and tests it at a join it does not thread, so &triangle->c takes EDI, the
// collision lives in its parameter slot (reloaded after the model query, stored after the requery)
// and the 0x14a8e0 return is laid out last; VC threads every flag form here, so the excluded arm now
// returns directly (no flag) and the model hit returns 0x14a8e0's result (286 bytes; the set-in-a-test
// flag and its goto are gone). Tried: read-only object 68; unsigned object handle 68; word query
// buffer 68; native collision record 71; external triangle reference 68; source publication 68; byte
// result merge 68; query workspace names 68; model inline/noinline 68/68; state inline 68; both
// contracts 68; probe result 68; model result 68; final found result 111; triangle inline 68; the
// collision re-read from the parameter at the join 68 (with a block-local flag 68); the flag as one &&
// expression tested twice, with returns 68; the same with an else-if tail and a final else for the
// no-object case 71 (286 bytes); assigned with the && expression, tests unchanged 68; an inline helper
// for the scenery test stays out of line and is threaded 113/111; a switch on the object type with the
// scenery case falling into the general case 68, or the flag set in a one-case switch 68; the flag
// from the && expression with the model query nested under `if (!excluded)` 68; the requery in an
// inline helper that updates the collision through its address 90; found cleared in the scenery arm
// with no flag 68; bool result 78 (262 bytes); the flag set in both arms with `if (excluded) goto
// done;` 68, with a bool result 78; a pointer parameter passed by address to an inline probe helper
// 134 (collision in EAX, 0xd2bb0 22); the by-value source passed by address to that helper 86/76
// (bool/byte result; 0xd2bb0 5); multi-consumer object policy, short/long and bool/byte found in
// object scope 117/117/116/116; function-scope policy 68/68/61/61 (long policy at 286 bytes, not
// kept); `excluded = function_10a0b0(...) != 2` inside the type test with `if (excluded) goto done`
// or `if (!excluded)` guards 54/54 (286 bytes, aligned worse: setne and the threaded exit stay); the
// object part in an inline helper returning `!excluded` 106 (keeps BL and the join test, adds a
// sete); the final test nested in the requery arm 77; `return function_14a8e0(...)` with an && flag
// 77 (263 bytes); 177 flag/guard/tail forms compile-only, all threaded; only a computed value in the
// join (a negation, a widening, a store) keeps retail's un-threaded flag test. Round 11 (relink
// screen, 66 = the old form there): the final test `found && !excluded` 113; the flag as long or
// byte 66; the collision read through the source at each use 66; on the kept form, a bool result 63
// (aligned 43 -> 41), `return false` on the excluded arm 58 (same code), `return` of the final query
// 128 (257 bytes). Lane 13 r14 (also: retail stores the requery's collision into the by-value
// source's slot, a dead store, and reads the source's collision from that slot after each call): a
// bool result 64; `return false` on the excluded arm 61; the flag from the && expression guarding the
// model query with `if (excluded) return result;` 65; the model result stored in `result` and the
// requery reading the source's collision 64; the flag set in the scenery arm, the model query in an
// else-if and the requery in `if (!excluded)` 82 (258 bytes).
// Round 44: existing availability producer result-first/guard returns/nested outcomes/
// structure once/settings once/both/both guarded/both with BSP receiver 61/98/98/61/78/78/98/61.
// Original helper/body restored; exclusion BL and source-slot join remain.
// Round 32 (lane 10, fast counts): `bool excluded = scenery && policy != 2;` with
// `if (!excluded && 0xbf9a0) { result = 0x14a8e0(...); return result; } else if (!excluded) requery; else
// return result;` 56 (286 bytes, threaded: no flag), with a bool result 71 (269 bytes); the requery as
// `if (!excluded) ... else found = false;` 72 (268 bytes), with a bool result 82 (258 bytes); the flag set
// in the scenery arm with the model query as its else-if and the collision re-read from the source at the
// join before `if (excluded) return result;` 55 (286 bytes, still threaded; a flag set in a test, not kept),
// with `if (!excluded) requery else found = false` 72; an inline exclusion test returning true/false
// (diagnostic) 95 (269 bytes).
// Round 11 (lane 2): ours threads the second flag test across the 0xbf9a0 call (the false edge knows
// the flag is clear); retail threads only the first. Real: the scenery test in a forceinline
// single-exit bool helper 112 (302 bytes: 0x10a0b0 inlined, flag threaded); the && flag tested
// twice (`!excluded && 0xbf9a0`, then `if (excluded) return result;`) 62; on it, `collision` reloaded
// from the source just before the second test 62 (dropped before threading; diagnostic); on it, a
// bool result 65; a function-scope flag cleared at entry, set in the scenery arm, 0xbf9a0 in an else-if,
// the requery under `if (!excluded)` with `found = false` otherwise 77 (263 bytes). The one-file LTCG
// harness passes the surface in EAX (the real build in ESI) even with 0xd2bb0's file and the other
// callers of 0x14ac60/0x14a8e0 linked, so it cannot screen this function.
// Round 97 (lane 8, fast with 0xd2bb0 and the callees): the flag as the out-parameter of an inline
// helper that also calls 0xbf9a0 (memory at threading time?) stays out of line 136 (251 bytes, ret 8,
// 0xd2bb0 lost); forced inline 112 (302 bytes: 0x10a0b0 inlined, flag still threaded), single-exit
// helper 112; a bool result with `return false` on the excluded arm 64; `result = 0x14a8e0(...);
// return result;` 54 (same code plus a setne on that path: the lengths line up, no structural gain),
// with a bool result 64, through `goto done` 56 (287 bytes); on it, `return false` on the excluded arm
// 54, the final call reading the source's collision 54, `found` set from the type test only when there
// is no object 92 (288 bytes, 0xd2bb0 5).
// Round 32 (lane 11, fast; 0x14af40 has the same BL flag and inlines 0x10a0b0 where this calls it, so
// a helper shared by the two was tried): the scenery test in a force-inline bool helper with
// `if (!excluded && 0xbf9a0)` storing the 0x14a8e0 result and `if (excluded) return` 118 (307 bytes),
// as plain inline 105 (245 bytes, out of line); the flag through a reference out-parameter 118; a helper
// returning the 0xbf9a0 result with the flag through a pointer 118; the flag cleared, then set from
// `!= 2` inside the type test 56 (288 bytes, setne and the direct exit stay; a flag set in a test).
// Compile-only (/O2 without /GL) all of these, plain/force-inline/single-exit helpers, thread the flag.
// Round 113 (lane 6, fast under the 0x14a8e0 reduced-image caveat): the flag as `type == 6 ? 0x10a0b0(...) != 2 : false` tested as `!excluded &&
// 0xbf9a0` then `if (excluded) return result;` 56 (288 bytes, setne in the type test, threaded); that with a
// bool result 59; plus the collision re-read from the source at the join 59; the 0x14af40 shape (`else if
// (!excluded)` requery with its own lightmap call, `else if (found)` for the no-object case) 86 (296 bytes);
// a function-scope flag cleared at entry and set in the type test, bool result 65. A scan of every matched
// retail function finds no other `mov r8, 1; jmp` to a `test r8, r8` past a call: only 0x14af40 shares it.
// N round 257: Complete native collision output on the inverted probe graph, byte/bool result and bool/byte found, raw/native triangle sentinel 53/53/53/53/63/63/63/63.
// Original record declarations, byte buffer and complete body restored.
// Round 108 (m3 lane 7, fast): a `modeled` flag set from 0xbf9a0 in the else arm of the scenery test, then
// `if (modeled) return 0x14a8e0(...)` and `if (!excluded)` requery 88 (found in BL, not the flag); `found = false`
// at the join before `if (!excluded)` 77 (263 bytes, threaded); a short policy defaulting to 2, tested twice
// 64; `excluded = false; if (type == 6) excluded = policy != 2;` with the model query and requery nested under
// `if (!excluded)`, else `found = false` 69 (268 bytes); a `lightmapped` flag tested around the model query
// and again around the requery 77 (263 bytes). Compile-only, all thread the join test. Compile-only probes:
// VC threads the flag through struct copies, inline accessors, address-taken references and cross-module
// (LTCG) inline helpers alike; only a side effect at the join (a store, even a redundant `result = false;`)
// leaves the test un-threaded, and that store stays in the code. Diagnostic (not kept): the flag set in both
// arms with `result = false;` before `if (excluded) return result;` 102 (302 bytes; flag in AL, not BL).
// Round 25 (m3 lane 2, fast; a compile-only /O2 screen of this file reproduces the threading): `found = false;`
// placed at the join before `if (!excluded)` around the requery, the && flag guarding 0xbf9a0, 77 (263 bytes:
// the store is sunk into the excluded arm and both tests thread); `result = false;` at the join with
// `if (excluded) return result;` 102 (302 bytes: the flag test stays un-threaded, `mov al, 1; jmp`, but the
// store stays, the flag is spilled and NONE takes EBP); `found = !excluded;` at the join with `if (found)`
// around the requery 86 (291 bytes: un-threaded, but sete and a spilled flag); that with the collision read
// through `source.collision` at every use 86 (same code); with the model result stored in `result` first
// 92 (296 bytes). The screen keeps the flag test only when the flag-test block's predecessor on the excluded
// edge is not the arm that sets the flag (a computed value or a memory test first); every such form that
// keeps the semantics adds code at the join, and the flag is then spilled where retail spills the collision.
// Round 61 (m3 lane 9, fast): the flag set in the scenery arm with 0xbf9a0 in its else-if and `point3f const
// *point = &collision->point;` named at the join before `if (excluded) return result;` 62 (the address sinks
// first; still threaded), with the requery under `if (!excluded)` and `else found = false;` 77 (263 bytes);
// the exclusion condition written out at both tests, no flag (looking for a CSE'd condition temp) 107 (309
// bytes: 0x10a0b0 called twice); a `done` flag set by the exclusion and by the model path (`result =
// 0x14a8e0(...); done = true;`) tested before the requery 68 (272 bytes); the positive test (`type != 6 ||
// 0x10a0b0 == 2`) holding the probe and requery with `else found = false;` 77 (263 bytes).
// Round 27 (m3 lane 2, fast): `found = false; if (!excluded) found = requery;` with the collision assigned after
// that if 79 (288 bytes: retail's `mov cl, 1; jmp` and an un-threaded `test cl, cl`, but `xor al, al` at the
// join, the flag spilled and NONE in EBP); with a bool result 81 (286 bytes); with the collision published
// through `source.collision` 79; `found = !excluded; if (found) found = requery;` with the assignment after the
// if 86 (291 bytes); an inline helper returning 0/1/2 (excluded/model/none) for the object part 160 (225 bytes,
// out of line); an inline `type == 6 && policy != 2` helper with `if (!excluded) requery else return result` 105
// (245 bytes, out of line). Compile-only: /O1, /Os, /G7, /Ob1, /Ob0, /Op and /Ox all thread the flag too, and so
// do helpers returning the exclusion (early returns, single-exit result, &&), `found = excluded ? ...`,
// `found = !excluded && requery` and an `__assume` at the join.
// Round 32 (m3 lane 2, fast): the && flag with the collision re-read from the source at the join 62 (the copy goes
// first; threaded); with a pointer to the query buffer named at the join 62; g_4687bc read into a local at
// the join 95 (296 bytes: the flag survives, `mov cl, 1; jmp` and the flag cleared just before 0xbf9a0 as in retail,
// but it is spilled, NONE takes EBX and the object index EDI); the source re-read through its address at the join
// 62 (folded); the join load with `if (!excluded) requery; else found = false;` 79 (286 bytes, same spill). A
// compile-only screen of 13 more flag forms (if-set, set after the probe, long, `== false`, struct-held, ternary
// requery, address-taken pointer parameter reassigned, parameter re-read at the join) threads every one; only a
// real load at the join keeps the flag, and it stays ahead of the test. The permuter's baseline link stalled.
// Round 39 (m3 lane 15, plain): the flag preset true and cleared in the not-excluded arm, `if (excluded) return
// result;` before the requery 62; the && flag with g_4687bc read into a local at the join and the requery and
// model reading the source's collision 95 (296 bytes: the flag survives but is spilled, &triangle->c takes EBX);
// that with every collision use through the source 95; with the model result stored in `result` and one `goto
// done` exit 89 (294 bytes, NONE in EBP); the preset-true flag with the join load 85 (293 bytes, still spilled).
// Compile-only, all threaded: preset-true in four placements, a positive requery flag, a negated or widened copy
// of the flag tested at the join, and the source as a class with a constructor or a destructor.
// Round 110 (m3 lane 7, fast): a compile-only screen keeps the join's flag test un-threaded (retail's `mov bl, 1;
// jmp`, `xor bl, bl` just before 0xbf9a0 and `test bl, bl` after it) when the flag reaches the test through a copy
// into a variable that has another live definition (as matched 0x1a0560's `result = loaded;`): a function-scope
// `excluded = false`, `excluded = scenery_excluded;` at the join and a final `found && !excluded`. Real builds of
// that: 77 (295 bytes: a second flag test, `found` spilled across the probe, the probe block laid out last); with
// the scenery test as if/else arms 77; with the probe nested under `if (!scenery_excluded)` 77; with `if
// (excluded) return result;` before the requery 62 (threaded again); `if (scenery_excluded) excluded = true; else
// { requery }` 75 (295 bytes).
// Round 113 (m3 lane 7, fast): the existing `object_get` for the object lookup 53 (same code); the object index
// read before `found` 97; the requery assigning `collision` only, 0x14a8e0 given `collision` 53 (same code); a failed
// requery returning `result` at once 108; the tail as `if (!found) return result; return lightmap(...)` 129 (257
// bytes); if/else exclusion arms (flag set true, or false with the model query returning 0x14a8e0) and `if (excluded)
// return result;` 62 (threaded).
// Round 286: Complete collision object-handle observations with cached/direct header,
// cached/per-use source and bool/byte found, eight forms 53 each. Original field
// snapshots and complete executable body restored; exclusion/source-slot join remains.
// Round 63 (m3 lane 16, fast): compile-only, the join test also threads as a switch on the flag, behind an
// `excluded || !0xbf9a0` guard, as a counter, as a parameter, and as `!!`, `== true`, `!= false`, `(char)`,
// `(long)`, `? true : false`, `| false`, `^ false`, `& 0xff`; the C front end and a constructed/destructed
// collision record thread it too. Only a bit-and join test keeps it: `if (excluded & 1)` 96 (297 bytes:
// retail's `mov r8, 1; jmp` and unthreaded join, but `test r8, 1`, the flag spilled into the collision's
// slot and NONE in EBX); that with the collision read through the source at every use 96; a plain
// non-inline exclusion helper 103 (240 bytes: LTCG keeps it out of line); the kept form with the collision
// read through the source at every use 53 (same code); a `short policy = 2` local set by 0x10a0b0 for
// scenery and tested `== 2` before 0xbf9a0 and `!= 2` before the requery 62.
// Round 221: coordinated byte return with native byte/bool result and direct/materialized model return
// 63/63/63/63; signed-byte return on those four graphs 63/63/63/63, all 269/286 bytes. The 0x60 frame
// and availability prefix remain correct; return normalization disappears but exclusion BL and
// source-slot publication do not appear. Every matched caller and file control survives. Original
// Boolean return, caller declaration, byte result, direct model return and complete body restored.
// Round 110 (m3 lane 7, fast): a compile-only screen keeps the join's flag test un-threaded (retail's `mov bl, 1;
// jmp`, `xor bl, bl` just before 0xbf9a0 and `test bl, bl` after it) when the flag reaches the test through a copy
// into a variable that has another live definition (as matched 0x1a0560's `result = loaded;`): a function-scope
// `excluded = false`, `excluded = scenery_excluded;` at the join and a final `found && !excluded`. Real builds of
// that: 77 (295 bytes: a second flag test, `found` spilled across the probe, the probe block laid out last); with
// the scenery test as if/else arms 77; with the probe nested under `if (!scenery_excluded)` 77; with `if
// (excluded) return result;` before the requery 62 (threaded again); `if (scenery_excluded) excluded = true; else
// { requery }` 75 (295 bytes).
// Round 294: Native query point assignment/memcpy/memmove 81/81/77; direction
// assignment/memcpy/memmove 80/80/75; both memcpy/memmove 113/137. Exact-only
// input snapshots restored; exclusion BL and source-slot publication/reload remain.
// Round 298: Finite acceptance/outcome/eligibility domains and pairs/all
// 52/56/53/74/52/56/74; all with complete finite model return 83.
// One-point leads rejected; original byte result and entire body restored.
// Exclusion BL and source-slot publication/reload remain.
// Round 300: Complete positive/rejection availability with common result exit and native
// short/long object policy, default/type-arm and conditional producers:
// 95/95/97/97/95/95/97/97. Original complete body restored; exclusion BL and source-slot join remain.
// Round 197 (lane 3, fast with the callees and 0xd2bb0): a plain collision pointer parameter in place of the by-value source
// 110 (frame 0x5c; 0xd2bb0 keeps its match); that with a `modeled` flag and nested tests 127 (262 bytes), with a bool result
// 137 (252 bytes); bool result alone 116 (271 bytes); requery split from the collision set with a second flag test 100
// (290 bytes; 0xd2bb0 21). Compile-only, the join test threads with a ternary, `&&`, do/while(false), `__assume` or `__noop`
// or an empty statement or goto label at the join, and a flag copied at the join into a variable with a second definition;
// only `found && !flag` (an extra test and an entry `xor`) and a store between the two tests keep it. Body restored.
// Round 519: Complete native collision-input assignment/memcpy/memmove after admission
// 156/156/154; after triangle resets 156/156/154; early assignment/memcpy with byte
// acceptance 156/156. Assignment/copy forms lose the matched caller; all restored.
// Exclusion BL and source-slot publication/reload remain.
// Round 343: Object-handle/header-producer/completed-classification last-use scope subsets
// 1..7 each 53; joint scopes with the model result at the shared exit 45 (286 bytes).
// Minimal model-result assignment + a new goto to a new label before the shared return 45 plain: NOT kept
// (goto, match only); assignment followed by its own return 46 NOT kept (gain seven, under the bar).
// Body restored at 53; exclusion/source-slot join remains.
// Round 138 (m3 lane 5): the model result stored in `result` with one `goto done` to the final return 45 plain: NOT kept
// (coordinator: the function keeps two other early returns, so the goto only moves this return, as round 343's; body
// restored at 53);
// on it, a bool result 56, the excluded arm also `goto done` 67 (272 bytes), the && flag guarding 0xbf9a0 with
// `if (excluded) return result;` 57 (287 bytes), a byte `found` 45 (same code), the 0xbf9a0 test positive with the
// requery after its block 56; before it, a byte flag `excluded |= 1` with the model query as its else-if 62 (threaded).
// Round 149 (m3 lane 5, fast with 0xd2bb0): 0x14af40's matching shape here (named `found`, requery in its then-arm, `!(excluded | found)`), the found arm
// storing the result 81 (291 bytes), `& 1` test 81, returning it 80 (286), store and return 79 (287): retail pushes EBX and EBP late here.
// Round 70 (m3 lane 14, fast with 0x14a8e0/0x14af40/0xd2bb0/0x14ac60/0x10a0b0/0xbf9a0): retail keeps source.collision in
// its stack slot (the requery's dead store to it stays, and only the model-query arm reloads it, before the un-threaded
// test), so forms that keep the parameter in memory: `(void)&source;` 53 (same code); the flag set in if/else arms with
// the collision re-read in the else arm and `if (excluded) return result;` 62, with `(void)&source;` 62, with the re-read
// at the join 62, the join re-read through `&source.collision` 62 (all threaded, 286 bytes).
// @retail 0x14b120
bool function_14b120(s_surface_query_source source, void *surface)
{
    byte result = false;
    byte local_collision[0x5c];
    *(short *)(local_collision + 0x24) = NONE;
    if (!structure_bsp_collision_available())
        return false;
    s_structure_collision_result const *collision = (s_structure_collision_result const *)source.collision;
    s_structure_lightmap_triangle *triangle = (s_structure_lightmap_triangle *)surface;
    triangle->cluster_index = NONE;
    triangle->instance_index = NONE;
    *(long *)((byte *)triangle + 8) = NONE;
    bool found = collision->type == 1 || collision->type == 3;
    long object_index = *(long const *)((byte const *)collision + 0x40);
    if (object_index != NONE)
    {
        byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
        if (object[0xaa] == 6 && function_10a0b0(object_index) != 2)
            return result;
        if (!function_bf9a0(object_index, (long *)((byte *)triangle + 0xc),
            (long *)((byte *)triangle + 0x10)))
        {
            found = function_1697c0(0x4800005, &collision->point, g_4687bc, NONE, NONE,
                (s_collision_result_1697c0 *)local_collision);
            source.collision = local_collision;
            collision = (s_structure_collision_result const *)source.collision;
        }
        else
        {
            return function_14a8e0((s_structure_collision_result const *)source.collision,
                *(long *)((byte *)triangle + 0xc), triangle);
        }
    }
    if (found)
        result = structure_get_lightmap_triangle(collision, triangle);
    return result;
}

/* a list of ranges (0x48 bytes each), as unknown_16e290.cpp defines it */
struct s_16e1b0_range
{
	byte unknown00[6];
	word first;
	word count;
	byte unknown0a[0x48 - 0xa];
};

struct s_16e1b0_list
{
	long count;
	s_16e1b0_range *ranges;
};

void function_16e1b0(long value, s_16e1b0_list const *list, long *unknown, long *range_index, long *offset);
point3f *function_142700(transform4x3f const *matrix, point3f const *point, point3f *out);
bool function_11e800(point3f const *a, point3f const *b, point3f const *c, point3f const *p, real *u, real *v);

static inline bool structure_section_get_vertex(s_structure_section const *section, long index, point3f *point)
{
	s_structure_vertex_block const *block = &section->vertex_blocks[0];
	point3f const *vertices = (point3f const *)(block->data + block->offset);

	if (vertices && index >= 0 && index < block->count)
	{
		*point = vertices[index];
		return true;
	}
	return false;
}
// MiniMax H (11-13): no new form, no lower score (172): retail still saves ebx first and keeps the BSP global in ebx while this build keeps it in ebp and defers the ebp and esi saves, so the prologue's order and the frame's slot heights differ; the promoted 0x11e800 clone still hands its four pointers over in another register order (retail ecx, edx, eax, esi), and that callee lives in another file.
// MiniMax I (11-11): no new form, no lower score (172): retail pushes ebx first and keeps the BSP pointer in ebx, we keep it in ebp and defer the ebp and esi pushes, and our frame's store slots sit one word lower than retail's; the callee's four argument registers (0x11e800) are outside this file.

// MiniMax G (11-13): no new form, no lower score (172): the BSP global sits in ebp where retail
// keeps it in ebx, the prologue's push order and the frame's slot heights differ, and the 0x11e800
// clone hands its four pointers over in another register order: BLOCKED-BY 0x11e800.
// MiniMax H (11-12): no new form, no lower score (172): our prologue still keeps the BSP global in ebp and defers the ebp and esi pushes where retail pushes ebx first, and our zero constant still takes ebx where retail gives it to edi.
// MiniMax H (11-11): no new form, no lower score (172); the BSP pointer still lands in EBP where retail keeps it in EBX, the frame's slots sit one word apart from retail's, and the four pointers of the 0x11e800 call are handed over in another register order, that callee being in another file.
// MiniMax E (11-10, run 73): no new form, no lower score (172); retail pushes ebx first and keeps the BSP global in ebx where we keep it in ebp, and the promoted clone of 0x11e800 hands its four pointers over in another register order (retail ecx, edx, eax, esi; ours ecx, edx, esi, eax); that callee lives in another file.
// MiniMax D (11-11): no new form, no lower score (172): the BSP pointer still lands in EBP where retail keeps it in EBX, and the body is still seven bytes longer than retail's 726.
// MiniMax G (11-11): no new form, no lower score (172): the first difference is at +0x16, in the prologue - ours stores the lightmap index into [esp+0x30] and loads the BSP base into ebp, retail pushes ebx first and keeps the BSP base in ebx; the frame's slots agree (0x58) and the 0x11e800 callee's registers are in another file, so this stays BLOCKED-BY 0x11e800.
// MiniMax D (11-06, run 49): no new form, no lower score (172); the promoted clone of function_11e800 still hands the four pointers to different registers (retail ecx, edx, eax, esi; ours ecx, edx, esi, eax); that callee lives in src/unknown_11cb00.cpp, not this lane's file.
// MiniMax A (11-04): no new form, no lower score (172). What still differs: the promoted clone of
// function_11e800 hands the four pointers to different registers. Retail calls it with
// ECX=arg1, EDX=arg2, EAX=arg3, ESI=arg4; our build calls it with ECX=arg1, EDX=arg2, ESI=arg3,
// EAX=arg4 (checked at our 0x407628 against retail's 0x14aebb). The assignment comes from the
// callee's own body in src/unknown_11cb00.cpp (126 off retail), which is not this machine's file,
// so the call site cannot be fixed from here: BLOCKED-BY 0x11e800.
// Also still different: the BSP/section carriers and the plane/instance stack homes.
// MiniMax E (11-07): no new form, no lower score; the BSP and section carriers and the range and
// plane homes are unchanged, and the vertex block still waits on 0x11e800's registers.
// MiniMax E (11-01): no new form, no lower score; the byte result set before the type branch gives
// 197 and the range declared before the section 172 with identical code. Left: the BSP in EBX with the
// section in its stack home, the rotated range/plane/instance homes and the 0x16e1b0 output homes.
// MiniMax D (11-06): no new form, no lower score (172); the prologue still keeps the BSP in EBP and
// the section in EBX where retail keeps the BSP in EBX and the section in its stack home, the range,
// plane and instance homes are rotated and the 0x16e1b0 output homes and the vertex joins still
// differ; BLOCKED-BY 0x11e800 (its v0/v1/v2 registers) and by 0x16e1b0, both in other files.
// score 172 (was 172): Frame 0x58 agrees; BSP/section carriers (ours keeps the section in EBX and
// the BSP in EBP, retail the BSP in EBX with the section in its stack home), range/plane homes and
// vertex joins remain. Retail never initialises its vertex flag (set only when a vertex exists,
// tested only after the first vertex); that uninitialised success-only flag gives 165 (163 with
// other declaration orders; 119 with an uninitialised definition), but it reads an uninitialised
// flag and drops the checks on the second and third vertices, so HEAD's body stays.
// Tried: output declarations reordered 185; outputs renamed 185; part index/unknown stores swapped
// 185; bool result 197; success-only flag 235, with bool result 221; instance index at each use
// 226/227/201; vertex loop with a break 287; inverted ternaries 165; surface range index at each use
// 191; flag initialised false 232; early return without a section 127 (an epilogue retail lacks;
// realignment only); BSP reads at use 217; conditional section selection 229; direct success returns
// 243; instance reads at use 213; relative triangle-range counter 206; reference table before
// traversal 221; instance case first 232; byte success result 185; result-controlled traversal 205;
// reference table and result traversal 216; index-first/output-first fetch contracts 185/185;
// filled-point return 185; native span inputs 238; actual validity publication 185; triangle query
// into result 217; validity-byte name 185; read-only section 185; shared residency 203; first-fetch
// validity 185; strip-input cursor 221; residency/first 203; residency/cursor 208; first/cursor 221;
// all three 208; both invariant geometry tables on the byte-result body 241; byte vertex validity 185;
// range index at use 199; cached range end 183 (below gate); later plane 211; native strip start 220;
// external section source 191; active section reference 185; surface table before walk 205; source
// and table 244; plane reads at use 225; collision record reference 185; long success/validity
// 192/185; native range 211; native reference 225; native section 216; native BSP 272; native
// collision 256; range and reference 218; section and range 227; collision and reference 248;
// BSP/range/block references 185/185/185; native block value 330; vertex count/memory copy 185/185;
// native strip-word input 257; range plus block references 185; failure accumulator bool/byte
// 185/185; signed byte/word fetch result 185/185; negative-index guard first 211; index test before
// data 208; failure plus early guard 211; failure accumulator plus signed-byte fetch 185; range/plane,
// range/instance, plane/instance and all metadata reads at every use 225/217/226/223, with BSP also
// read at every use 231/221/232/222; private point setter 230; dword index 258; both 256; dword fetch
// result 185; result with setter/index/both 230/258/256; complete input point captured before the
// shared setter 319; validity output pointer/reference/byte 185/185/185; predicate-produced bool/byte
// 185/185; predicate byte with native block 185; Boolean/byte outputs with native block 185/185;
// final failure bool/byte/short/long 182/182/192/194 (three-point best dropped); existing point setter
// 231; cluster reads at use 216; setter/cluster 245; instance receiver at use 188; setter/instance 228;
// both receivers 220; setter/both 251; native coordinate initializer 231; separate source/query points
// with entry/default initialization byte 233/233, bool 222/222, CRT byte 233/233, bool 222/222;
// eligible source-point return with section/block inputs and aggregate/CRT caller copies 266 each;
// validity assigned only on failure with those four forms 192 each; native-byte geometry,
// metadata/ordinal, query/triangle workspace and part-output lexical homes 185 each, all groups 185;
// two-use collision discriminator, reused reference surface ordinal and readonly BSP/instance inputs,
// all seven combinations 185 each; combined unsigned discriminator 185; part/residency/transform
// provider inline contracts, singles, pairs, all and all with separate noinline declarations 185 each
// (transform forms lose reduced controls 0x31590/0x168d60, also 0x23d030 without noinline); native/
// const fetch source capture with aggregate/library publication 185 each, each with query memmove
// 207; range declared before section 185; BSP declared last 186; range or BSP local renamed 185/185
// (spill slots unchanged); section local renamed 185 (names move neither slots nor registers);
// vertex fetches as one && chain with the triangle test 257 (677 bytes); vertex helper taking the
// block, read once per triangle 238; vertex getter returning the point or NULL 255; nested ifs in
// place of the continues 185 (same code), with the result in the loop test 221.
// Round 380: geometry-join byte/signed-byte result with source-owned definition and
// residency-owned section default subsets 208/172/216/217 each; original entry byte/bool
// result with source-owned definition 195/207. Branch-owned definition null default
// and geometry-join byte result 172 (733/726 bytes), plain confirmed; retained.
// Frame 0x58 agrees; BSP/section residence, range/plane homes and vertex joins remain.
// Round 384: strip receiver after readonly part lookup with original/readonly
// strip/reference/both 201/201/201/201; each with the two-use native first-index snapshot
// 230/230/230/230.
// Original strip acquisition, native byte result and complete defined-failure body restored;
// BSP/section and range/plane homes remain.
// Round 249: Complete pointer/lower/upper eligibility exits in all six orders
// 185/188/208/210/187/186; native bool/byte bounds predicates 254/254. Original
// defined-failure helper and complete query restored; BSP/section homes remain.
// Round 223: Signed/unsigned word masked-plane storage with original/reference-base/surface-base/both table selections 213/188/214/210/215/196/216/224. Original full-width plane, table selectors and defined vertex failures restored.
// Round 165: actual instance-table predicate shared at both selections, crossed with
// native byte fetch and the reused surface ordinal, 228 in all four forms; the same
// four with a canonical Boolean query result 232 each. Predicate shortens the frame;
// original long instance lifetime, helper, byte result and defined failures restored.
// Geometry-join Boolean result 182 (726 bytes); native byte fetch 172; both 182;
// initialized single-exit fetch 172, with Boolean result/byte fetch/both 182/172/182;
// complete result/fetch graph with readonly section 182. Original helper and result restored.
// Round 32 on branch-owned definition/result-at-join: all BSP observations 234;
// all collision metadata observations 228; CRT query snapshot 172; BSP/metadata 231;
// BSP/copy 234; metadata/copy 228; all 231, instance-first all 222. Original body restored.
// Round 251: bounded vertex byte/signed-byte/word/unsigned-long ordinals
// 259/259/259/172; each with a complete post-geometry range-end expression
// 269/269/269/196. Original ordinal and bounds restored; frame/BSP/section and
// defined vertex joins remain.
// Round 387: BSP/section carriers and range/plane homes remain. Native fetch output/section/both
// references 172 each; remaining index-output-section, output-index-section and section-output-
// index contracts 172 each; output reference with index-first and both references with output-
// first 172 each. Original query and complete defined-failure helper restored.
// Round 387: Three further native fetch reference contracts, index-output-section with section
// reference, output-index-section with point reference and section-output-index with both
// references: 172 each. Eight new reference contracts total; three pointer-only order observations
// are additional. Complete original query and defined-failure helper restored.
// Round 393: native external cluster/instance/triangle references in seven subsets
// 172 each; all with native surface reference 172. Original receivers and defined-failure body restored.
// Round 228 on the branch-owned geometry join: filled-output pointer success with native
// section/output references and index-first order, all eight contracts 172 each. Original
// Boolean fetch, defined failures and caller roles restored; BSP/section residence remains.
// Round 400: Existing vertex getter as a section member with bool/byte/word/long results
// 172/172/172/172; as a vertex-block member
// 172/172/172/172. Every defined failure and suppressed later fetch restored.
// Round 185: Direct collision-point transform input with aggregate/library/overlap-safe/shared-
// setter copies at entry 199/199/205/243; case-owned copies 251/251/257/296. Original in-place
// transform, point capture, defined vertex failures, complete body and include set restored.
// Round 187: Native byte-data/offset versus point-table fetch inputs, long/word count and
// early/result-first eligibility, all eight 235. Original fetch contract and body restored.
// Round 403: real triangle-reference cursor with readonly metadata/surface table/range
// bound/vertex cursor combinations 210/210/211/193/213/210/213; complete graph
// with Boolean result 224. Original traversal and defined-failure body restored.
// Round 207: native point-value getter with bool/byte/long/dword validity outputs,
// default-first result 284/284/278/278; eligibility-owned return 284/284/278/278.
// Frames 0x68/0x6c and shorter code regress; original Boolean fetch and defined failures restored.
// Round 426: captured instance key through publication with long/short/word/dword plane storage 232/180/197/232; Boolean results 242/187/194/242. Original collision observations and defined vertex failures restored.
// Round 229: failure-only in/out validity on the original point-output helper, bool/byte/short/long
// pointer outputs 172/172/172/172; references 172/172/172/172. Original helper, defined failures and query restored.
// Round 185 (lane 3, fast): the 0x16e1b0 outputs declared part index, unknown, part offset 172
// (same slots: retail's part offset/unknown/part index homes stay swapped); the BSP declared before
// the point copy 172; the vertex helper forced inline 172; the three fetches as one && expression
// 264 (681 bytes); `if (!section) return false;` 211 (726 bytes). Retail's skip after a missing
// first vertex tests a flag no path initialises, so no defined-failure form reproduces it.
// Round 61 (m3 lane 9, fast): retail's vertex flag (a byte set after each successful fetch, never cleared
// and never initialised, tested only when the first fetch fails; 0x11e800 is called whatever the second and
// third fetches gave) written as `if (get(v0)) found = true; else if (!found) continue; if (get(v1)) found =
// true; if (get(v2)) found = true;`: with `bool found = false;` 232 (756 bytes; aligned 51 against 99: the
// BSP takes EBX with retail's spill and the vertex block lines up), uninitialised 226 (aligned 50); with a
// bool result 207/212 (aligned 49/46); result and flag declared at the top 246/229, a byte result at the top
// 240/243; `definition = NULL` at its declaration 245, an uninitialised definition 257, with the
// uninitialised flag 280; `unknown` renamed `a_unknown`/`z_unknown` 226/226 (names move no homes). Left on
// the best flag form: the instance index in EBP (retail ECX, with the range in EBP), NONE held in EDX for
// three compares, the 0x16e1b0 unknown/part-index homes and BLOCKED-BY 0x11e800 (its v0/v1/v2/point
// registers). Any defined initial value adds a store retail lacks, and the flag is set in a test, so none of
// these is kept.
// Round 138 (m3 lane 5, fast; retail zeroes a byte at esp+0x13 and the definition in EDI at entry): `definition = NULL`
// at its declaration with the byte result declared among the locals 209 (727 bytes); with the result declared first
// 185 (726 bytes); with the BSP declared before the point 217; section before definition 209; a bool result 217.
// Round 123 (m3 lane 7, fast; plain agrees): retail's vertex byte at esp+0x12 is one slot written on each
// hit, tested only after the first miss and never initialised: the vertex getter's own result left unset on
// a miss (`bool result; if (ok) { *point = ...; result = true; } return result;`), inlined once in the
// `valid = valid && ...` loop and unrolled. With it the 66-instruction vertex block is exactly retail's:
// alone 226 (751 bytes, aligned 40 against 75); with a bool result 212; with `definition = NULL` at its
// declaration 219, bool 205; result, definition, section declared in that order after the instance index
// 208 at 726 bytes (aligned 20; rsd/drs 208, dsr/sdr 137, srd 220); then unknown18 stored before
// part_index 208 (aligned 16: the 0x16e1b0 output slots then as retail; the 22 other store orders worse).
// On that: `unknown` renamed (7 names) 208; output declaration orders 208; `section != NULL` 208; range
// declared first 208; declarations at the function top/after the BSP 220/220; result at the other entry
// positions 220-221; field locals moved after the group 216-224; `!count` 208; section NULL as a statement
// 208; permuter 198 (a volatile only). Left: VC keeps constant 0 in EBP (retail gives EBP to the range at the
// join and tests with `test`), which shifts the positional score, and BLOCKED-BY 0x11e800's registers.
// Round 216 (m3 lane 3, fast): a bool result declared first with definition and section NULL at their declarations
// (no branch NULL stores) 197 (713 bytes); the section declared first, before the point copy 172 (same code); the byte
// result cleared before the type branches 197 (742 bytes); the surface test written as one plane comparison (no
// surface local) 201; the 0x16e1b0 call moved after the vertex fetches 282 (774 bytes). Body restored.
// MiniMax E (11-08): no new form, no lower score: our frame's store slots are shifted (we store to esp+0x30 where retail stores to esp+0x34) and we do not push ebx first.
// MiniMax E (11-09): no new form, no lower score: our frame's store slots are shifted (we store to esp+0x30 where retail stores to esp+0x34) and we do not push ebx first.
// Round 82 (m3 lane 10, fast, control 172, aligned 99): the type tests as a switch with the instance case
// second and the NULL definition in a default 217 (728 bytes); the unknown18 clear before the byte result's
// declaration 172 (same code); the published instance index from the instance-index local 232 (720 bytes,
// and callers 0xd47d0/0x14af40 lose their match: the convention moves); both reference/surface selections
// tested on `definition == NULL` 228 (719 bytes); the strip indices indexed from the section at each fetch
// (no pointer local) 221 (747 bytes).
// MiniMax B (11-11): no new form, no lower score (172): our body is seven bytes longer than retail's 726;
// retail pushes ebx first and keeps the BSP base in EBX with the section in its stack home, we keep the BSP
// base in EBP and the section in EBX; retail's range, plane and instance homes sit one word higher than
// ours, its three vertex fetches are unrolled where ours are a loop, its two 0x16e1b0 output homes are
// swapped against ours, and the promoted clone of 0x11e800 takes its four pointers in another register order
// (retail ecx, edx, eax, esi; ours ecx, edx, esi, eax), that callee living in another file.
// MiniMax I (11-11): no new form, no lower score (172): the three 0x16e1b0 output homes follow the
// call's argument positions (arg3 to esp+0x34, arg4 to esp+0x38, arg5 to esp+0x3c) and not the
// declaration order (swapping the two output declarations is byte-identical); retail keeps the
// always-zero output at esp+0x38 and the range index at esp+0x34, so its two homes are swapped
// against ours, and its tail stores esp+0x34 into part_index and esp+0x38 into unknown18 where we
// store them the other way round. Swapping the two arguments (with or without the two field
// assignments) reproduces one side of the swap only, and the whole frame's other slots and the
// ebx/ebp split differ as well.
// MiniMax D (11-10): no new form, no lower score (172): the BSP pointer still lands in EBP where retail
// keeps it in EBX, so the cluster pointer takes EDX here and EBP there, and the section, BSP and instance
// slots sit one word apart; a const BSP pointer compiles the same code.
// MiniMax D (11-09): no new form, no lower score: the body is unchanged at 172 (726 bytes against retail's 726); the difference is the frame's shape (our slots sit one word lower than retail's and we do not push ebx first) and the 0x11e800 callee's registers, which are outside this file.
// MiniMax S (11-10): no new form, no lower score (172): retail pushes ebx first and keeps the BSP global in ebx, we keep it in ebp and defer the ebp and esi pushes, so the prologue order differs although the frame slots agree; our zero constant takes ebx where retail gives it to edi.
// MiniMax R (11-10): no new form, no lower score (172): the result as a bool (182), the result declared first (212/197) and four orders of the prologue reads (172/172/223/172) all leave retail's push-ebx-first prologue and its BSP in EBX unreproduced.
// MiniMax A (11-12): no new form, no lower score (172); retail pushes ebx first and keeps the BSP global in ebx while
// this build keeps it in ebp and defers the ebp and esi pushes, so the prologue order differs although the frame's
// slots agree, and 0x11e800's argument registers are decided by that callee's body in another file; the permuter
// (300 s, annealed) was stopped by the run's time limit without a result.
// MiniMax B (11-13): no new form, no lower score (172): retail pushes ebx first and keeps the BSP global in ebx with the section in a stack home, ours keeps the BSP in ebp and the section in a register; the promoted clone of 0x11e800 takes its four pointers in another register order (retail ecx, edx, eax, esi), that callee being in another file.
// MiniMax A (11-14): no new form, no lower score (172): retail still keeps the BSP global in ebx
// and pushes ebx first while this build keeps it in ebp and defers the ebp and esi pushes, and the
// zero flag's frame slot is exchanged with the global's; the only spelling left is a declaration
// order change (the bsp assignment before the point copy), which the rules ask to be reported
// rather than made.
// @retail 0x14ac60
bool structure_get_lightmap_triangle(s_structure_collision_result const *collision, s_structure_lightmap_triangle *triangle)
{
	point3f point = collision->point;
	s_structure_bsp_view *bsp = (s_structure_bsp_view *)g_4e0348;
	long surface_range_index = collision->surface_range_index;
	long plane_index = collision->plane_index & 0x7fff;
	long instance_index = collision->instance_index;
	s_structure_instanced_geometry_definition *definition;
	s_structure_section *section = NULL;
	s_structure_surface_range *range;

	if (collision->type == 1)
	{
		definition = NULL;
		range = &bsp->surface_ranges[surface_range_index];
		s_structure_cluster_view *cluster = &bsp->clusters[range->cluster_index];

		if (function_12de70(&cluster->field_28, 3))
		{
			section = &cluster->sections[0];
		}
	}
	else if (collision->type == 3)
	{
		s_structure_instance *instance = &bsp->instances[instance_index];

		definition = &bsp->instanced_geometry_definitions[instance->definition_index];
		if (definition->surface_range_count == 0)
		{
			return false;
		}
		range = &definition->surface_ranges[surface_range_index];
		function_142700(&instance->matrix, &point, &point);
		if (function_12de70(&definition->field_28, 3))
		{
			section = &definition->sections[0];
		}
	}

	else
	{
		definition = NULL;
	}

	byte result = false;
	triangle->unknown18 = 0;
	if (section)
	{
		for (long i = range->first; i < range->first + range->count; i++)
		{
			s_structure_triangle_reference *reference = instance_index == NONE ?
				&bsp->triangle_references[i] :
				&definition->triangle_references[i];

			if (reference->surface_index == NONE)
			{
				continue;
			}

			s_structure_collision_surface *surface = instance_index == NONE ?
				&bsp->collision_bsps[0].surfaces[reference->surface_index] :
				&definition->surfaces[reference->surface_index];

			if (surface->plane_index != plane_index)
			{
				continue;
			}

			word *indices = &section->strip_indices[reference->first_index];
			long unknown;
			long part_index;
			long part_offset;
			point3f vertices[3];
			bool valid = true;

			function_16e1b0(reference->first_index, (s_16e1b0_list const *)section, &unknown, &part_index, &part_offset);
			for (long vertex = 0; vertex < 3; vertex++)
			{
				valid = valid && structure_section_get_vertex(section, indices[vertex], &vertices[vertex]);
			}
			if (valid && function_11e800(&vertices[0], &vertices[1], &vertices[2], &point, &triangle->u, &triangle->v))
			{
				triangle->part_offset = part_offset;
				triangle->lightmap_part_index = reference->part_index;
				triangle->part_index = part_index;
				triangle->unknown18 = unknown;
				triangle->cluster_index = range->cluster_index;
				triangle->instance_index = collision->instance_index;
				result = true;
				break;
			}
		}
	}
	return result;
}

#include "unknown_1efac0.h"

struct s_vertex_source_23ac30;
struct s_vertex_bounds_23ac30;
struct s_table_holder;
void *function_1efd80(s_table_holder *holder, dword position);
void function_ba690(long object_index, long *state_count, byte **states, long *a, long *b);
void function_23ac30(s_vertex_source_23ac30 const *source, long index,
	s_vertex_bounds_23ac30 const *bounds, point3f *point, long *node);

/* the collision record's object index, re-read at each use as retail does (no local copy) */
#define COLLISION_OBJECT_14A8E0 (*(long *)((byte const *)collision + 0x40))

// score 4 (was 4): the vertex-bounds read moved before the geometry-block call (retail reads it there
// and spills it) and the lookup's address held in a named pointer (retail keeps it in ESI for the
// second call); left: only 0x11e800's four argument registers (BLOCKED-BY 0x11e800, whose body decides
// them: ours v0/v1/v2/point in eax/esi/edx/ecx, retail in eax/edx/ecx/esi).
// Round 39 (lane 2): 0x11e800 declared (p, a, b, c, u, v), retail's setup order at this call read right to left, at
// its definition, this file's declaration and both calls: 0 (plain; 0x11e800 129 -> 126, 0x14ac60 stays 172), not
// kept because 0x11e800 is contributor code. Caller forms with 0x11e800 unchanged (fast): vertices + 1 and + 2 4; named
// second and third vertex pointers 4; a named point pointer 4; named u and v pointers 4; a named result 4.
// Earlier at 70: frame 0x58 and initial model/collision homes agree; bounds timing,
// lookup-address reuse and triangle-test registers differ.
// Earlier: with the part list read through its 0x48-byte range records and the part offset read
// at each use, the model/collision registers (EBX/EDI swapped), lookup address (retail keeps it
// in ESI for the second call) and BLOCKED-BY 0x11e800's argument registers differed.
// with the part list read through its 0x48-byte range records and the part
// offset read at each use, only the model/collision registers (EBX/EDI swapped), the lookup address
// (retail keeps it in ESI for the second call) and BLOCKED-BY 0x11e800's argument registers differ.
// Round 8 (lane 10): range-record list 95 (kept); part offset at each use 78 (kept); section groups
// after the node search 208; bounds read at its use in the vertex call 274; separate local for the
// second object lookup 78; lookup declared at the top 78; object lookup before the model 219;
// diagnostic: the bounds read before the geometry-block call gives 9 (registers as retail; left
// are the lookup address and 0x11e800's registers), held because it moves a one-use read.
// Round 10 (lane 10): model lookup before the triangle resets 83; model and object lookups there 88;
// point copied before the object re-read 78; section index local dropped 78 (same code).
// Earlier: region code and node search match; retail reads the model's bounds before
// the geometry-block call (that read moved there gives 143 with the section groups ahead of the node
// search, but it moves a definition read ahead of a call), keeps the vertex cursor in EBX and the
// mesh spilled, and BLOCKED-BY 0x11e800: its vertices/point registers (ours eax/esi/edx/ecx, retail
// eax/edx/ecx/esi) come from the contributor's body. Round 94: bool result 237 (886 bytes); signed
// count from sizeof (jle) 236; region pointer before the test with both 195; plus NONE-result node
// search 181 (kept); node count >= test 262/206; section groups after the surface lookup 254/214/177
// (swaps model and collision registers unless the bounds read moves too); for vertex loop 181; part
// range fields at their uses 180.
// Round 97: bounds before the geometry-block call with section groups before the node search 143
// (only BLOCKED-BY 0x11e800 registers, slots and the vertex cursor follow), same with a renamed
// section local 143 (names do not move slots), with a for vertex loop 143; bounds with the model
// lookup 221, plus early section groups 214.
// Frame 0x58 agrees; region scheduling and range/vertex homes remain. Split
// region/transform receivers 236; early section groups 254; both 254; byte return with
// split/groups/both 237/255/255; signed-byte result with split/groups 236/254.
// region scheduling and model/range/vertex homes remain.
// Region scheduling and model/range/vertex homes remain; staged region base
// 236; library point copy 236; both 236; false result at first failure 242; read-only
// reference/range 236/236; late false with reference 242; chained part resets 238.
// 0x58 frame; region scheduling/model spill remain.
// Retained mesh 258; unsigned node ordinal 240; masked plane snapshot 264;
// native surface-table view 240; relative range ordinal 252.
// Region scheduling and model/node storage still differ;
// direct surface-group expression 240; byte success result 244; word section ordinal 247;
// direct group and byte result 244; late bounds and byte result 236.
// cached reference end 250; native strip start 244; no search sentinel 248;
// shared point setter 238; positive parts scope 243; byte return 237.
// Region base at each use 268; surface lookup name 236; local point name 236; triangle vertex name
// 236; node/corner scalar names 236; node count at uses 236; native part-bound locals 236;
// section field at early uses 236.
// region scheduling and model/range/vertex homes remain. Fast trials: separate
// NONE result 269; separate count result 262; unsigned candidate and NONE result 269; unsigned
// candidate and count result 262; entry NONE-result lifetime 269; entry count-result lifetime 262;
// node pointer search 270; bounded mismatch scan 236. All restored.
// Model lookups at uses 298; refreshed model 284; read-only nodes 236; extraction-output lifetime 236;
// strip cursor 242; read-only strip 236; output lifetime plus strip 242; model uses plus strip 300.
// Cache inline/noinline, lookup inline/noinline, both inline/noinline, and
// mixed inline/noinline pairs 236 each; all declaration metadata restored.
// Safe signed transform count and direct region-record expression 236 each;
// section/range/reference/index/parts storage names and combined signed-count homes 236 each.
// Extraction-helper inline/noinline 236/236; unsigned/short/word vertex ordinal 236/248/248; initialized vertices/node/both 303/243/304. All helper declarations restored. Region schedule and model/range/vertex homes remain.
// Native surface-group/range/both roots through reused byte offsets 269/241/300;
// reference/group-reference/range-reference/all roots 247/294/265/329; all with bool result 320.
// Bounds/node/part table fields at every use 284/236/243; bounds-node/bounds-part/node-part
// graphs 258/286/243; all three/all three plus part-count field 239/239. All restored.
// Round 212: complete native range mutable/const 193/193; native reference mutable/const
// 194/194; both mutable/const 208/208; mesh field at every use 205; both const records with
// mesh field observations 214. Original records, observations and protected interfaces restored.
// DH round 189: Native lookup/extraction-array/node-output work-scope lifetimes, singles, pairs,
// all and reversed all 181 each; all original producer/read phases restored.
// Round 215: first transform in place 210; with model/section/both const views
// 210/210/210; const object/mesh views 181/181; in-place with const mesh/all
// four const views 210/210. Original input point, views and body restored.
// Round 62: section groups read right after the surface lookup, as retail does 177 (kept; model
// and collision swap EBX/EDI); after the node count/count test/nodes/sentinel 182/186/186/186;
// with the stored section index replaced by the local 206; object lookup before the model 202.
// Round 150: section test and geometry-block call in one condition 78; lookup pointer for the
// second call only 78; region index named for its four uses 254; the matched siblings' lookup
// pointer (`s_lookup *lookup_pointer = &lookup;` for both calls) 77 (retail's ESI reuse), and with
// the diagnostic bounds read 4: left are only 0x11e800's argument registers (ours second/third/point
// in esi/edx/ecx, retail edx/ecx/esi), so both forms wait on that contributor body.
// Round 202: Native mesh node/strip/parts fields, seven combinations, and all with const receiver 78/78/78/78/78/78/78/78 fast.
// Original executable source restored. Frame 0x58 agrees; model/collision roles, bounds acquisition, lookup-address reuse and triangle-test inputs remain.
// Round 76: the bounds read moved down to just before the lookup 70 (kept: model/collision
// take retail's EBX/EDI; left are the bounds read's place, the lookup-address reuse and
// BLOCKED-BY 0x11e800's registers); the bounds read after the object re-read 77, after the
// point copy/transform count 212, after the lookup initialize 105; const model/section/table
// views 78; bounds declared with the model, assigned in place 78; after the kept change: the
// surface lookup through an inline (lookup, position) helper 69, forced inline 262 (inlines
// 0x1efd80); lookup declared at the top 70; lookup declared before the bounds read 70;
// separate local for the transform object 70.
// Round 194: shared first/second/both object acquisitions, node-matrix accessor alone,
// either/both acquisitions with that accessor and shared headers with it 78 each.
// Model/collision homes, bounds timing, lookup reuse and triangle-call inputs remain.
// Round 209: Borrowed collision-point input with original/branch-owned point production, do/for vertex
// loop 70/70/70/70; corresponding bounded short vertex ordinals 102/102/102/102. Original body
// restored; bounds timing, lookup reuse and triangle-test registers remain.
// Round 78: the work after the lookup nested under `if (lookup.initialize(...))` 70; the lookup
// guard as `return result` 231; a const bounds pointer 70; the lookup and surface lookup in their
// own block 70; section groups from the section index local 157. Diagnostic: the bounds read
// before the geometry-block call still gives 9 (left: the lookup address, 0x11e800's registers).
// Round 81: the region permutations through the existing 0xba690 call, which LTCG inlines here as
// retail does (kept: identical code, its body no longer written out); with the bounds read before
// the geometry-block call 9, plus the lookup pointer 4 (only BLOCKED-BY 0x11e800's argument
// registers left, so neither form can be kept yet); lookup declared at the top 70; a region
// index local 262.
// Round 185 (lane 3, fast, diagnostic): the bounds read before the geometry-block call plus the
// lookup pointer still give 4, all four 0x11e800 argument registers (ours second/third/point in
// esi/edx/ecx, retail edx/ecx/esi), so BLOCKED-BY 0x11e800.
// Round 61 (m3 lane 9, fast): the existing bounds read moved before the region test 88, before `if
// (section_index == 0xff) goto done;` 15 (886 bytes; left: the read's home, the lookup-address reuse,
// 0x11e800's registers), before the section pointer 223, before the region-list declarations (ahead of
// 0xba690) 230, before the object lookup 78; the listed placement before the 0x12de70 test still gives 9.
// Each moves a read ahead of calls (kept only with a match); BLOCKED-BY 0x11e800.
// Earlier at 4 (was 70): the vertex-bounds read moved before the geometry-block call (retail reads it there
// and spills it) and the lookup's address held in a named pointer (retail keeps it in ESI for the
// second call); left: only 0x11e800's four argument registers (BLOCKED-BY 0x11e800, whose body decides
// them: ours v0/v1/v2/point in eax/esi/edx/ecx, retail in eax/edx/ecx/esi).
// R75: point address at call/before extraction 4/4; vertex base at call/through extraction 4/4; direct array
// arithmetic 4; both input pointers through extraction 4; vertices before the reference loop 4; output-address locals 4.
// Round 123 (m3 lane 7): listing re-read, still only 0x11e800's four argument registers; BLOCKED-BY 0x11e800.
// Round 116 (fast): failed-test continue 4; late success result 58 (879 bytes); early success result 4;
// vertex-node declaration before the array 4; index declaration after the vertex ordinal 6; counter
// preincrement in that context 6; exact-count exit 7; direct success return 335 (822). Original body restored;
// the four triangle-test argument registers remain BLOCKED-BY 0x11e800.
// Round 210 (fast, diagnostic, reverted): 0x11e800 declared (p, a, b, c, u, v) still gives 0 (0x11e800 126, 0x14ac60
// 172); all four differences are its argument registers. BLOCKED-BY 0x11e800 (contributor).
// Round 216 (m3 lane 3): baseline listing still differs only in 0x11e800's four argument registers; BLOCKED-BY 0x11e800.
// Round 96 (m3 lane 9): baseline listing still differs only in 0x11e800's four argument registers; BLOCKED-BY 0x11e800 (contributor).
// Round 162 (m3 lane 6): HELD (needs the owner's OK): 0x11e800 declared (p, a, b, c, u, v), as retail's argument setup at both
// calls implies (ecx, edx, eax, esi set in that order for c, b, a, p), with its definition, this declaration and both calls changed
// together: plain check MATCH here, 0x11e800 129 -> 126, 0x14ac60 172 -> 172. The definition is a contributor's, so the code is back
// to main and the full diff is in build/r162/held-14a8e0.patch.
// Round 109 (m3 lane 16): listing re-read, still only 0x11e800's four argument registers; the held r162 reorder re-measured on the
// current base (fast, restored at once): MATCH here, 0x11e800 129 -> 126, 0x14ac60 172 -> 172. Still HELD, BLOCKED-BY 0x11e800.
// Round 173 (m3 lane 6): listing re-read (fast 4), still only 0x11e800's four argument registers; BLOCKED-BY 0x11e800.
// Round 88 (m3 lane 13): baseline listing still differs only in 0x11e800's four argument registers (ours point/third/second/first
// in ecx/edx/esi/eax, retail third/second/first/point in ecx/edx/eax/esi). HELD (needs the owner's OK): the r162 reorder, 0x11e800
// declared (p, a, b, c, u, v) at its definition, this file's declaration and both calls, re-measured on the current base (fast,
// restored at once): MATCH here, 0x11e800 129 -> 126, 0x14ac60 172 -> 172; the diff is in build/r88/held-14a8e0.patch.
// BLOCKED-BY 0x11e800 (contributor).
// @retail 0x14a8e0
bool __stdcall function_14a8e0(s_structure_collision_result const *collision,
	long model_index, s_structure_lightmap_triangle *triangle)
{
	bool result = false;
	{
		triangle->unknown18 = NONE;
		*(long *)((byte *)triangle + 8) = NONE;
		*(long *)((byte *)triangle + 0x14) = NONE;
		triangle->part_index = NONE;
		triangle->part_offset = NONE;
		triangle->lightmap_part_index = NONE;
		*(long *)((byte *)triangle + 0xc) = NONE;
		*(long *)((byte *)triangle + 0x10) = NONE;
		byte *model = g_4e3b44[model_index & 0xffff].bytes;
		byte *regions;
		long region_count;
		function_ba690(COLLISION_OBJECT_14A8E0, &region_count, &regions, NULL, NULL);
		byte *region_data = *(byte **)(model + 0x20) + (*(short *)((byte const *)collision + 0x44)) * 16;
		if (regions[(*(short *)((byte const *)collision + 0x44))] != 0xff)
		{
			*(long *)((byte *)triangle + 0x14) =
				*(short *)(*(byte **)(region_data + 0xc) + (char)regions[(*(short *)((byte const *)collision + 0x44))] * 16 + 0xc);
		}
		else
			*(long *)((byte *)triangle + 0x14) = 0xff;
		long section_index = *(long *)((byte *)triangle + 0x14);
		if (section_index == 0xff) goto done;
		byte *section = *(byte **)(model + 0x28) + section_index * 0x5c;
		s_vertex_bounds_23ac30 const *bounds = *(s_vertex_bounds_23ac30 const **)(model + 0x18);
		if (!function_12de70((s_geometry_block_info *)(section + 0x38), 3)) goto done;
		byte *object = *(byte **)(g_4e0300->data + (COLLISION_OBJECT_14A8E0 & 0xffff) * 12 + 8);
		point3f point = collision->point;
		long transform_count = *(short *)(object + 0x114) / sizeof(transform4x3f);
		if (transform_count > 0)
		{
			transform4x3f *matrix = (transform4x3f *)(object + *(short *)(object + 0x116)) + (*(short *)((byte const *)collision + 0x46));
			function_142700(matrix, &collision->point, &point);
			matrix = (transform4x3f *)(*(byte **)(model + 0x4c) + (*(short *)((byte const *)collision + 0x46)) * 0x60 + 0x28);
			function_142700(matrix, &point, &point);
		}
		s_lookup lookup;
		s_lookup *lookup_pointer = &lookup;
		if (!lookup_pointer->initialize(COLLISION_OBJECT_14A8E0)) goto done;
		byte *surface_set = (byte *)function_1efd80((s_table_holder *)lookup_pointer,
			*(dword *)((byte const *)collision + 0x48));
		byte *section_groups = *(byte **)(model + 0x80) + *(long *)((byte *)triangle + 0x14) * 8;
		byte *section_mesh = *(byte **)(section + 0x34);
		long node_count = *(long *)(section_mesh + 0x64);
		if (node_count <= 0) goto done;
		byte *nodes = *(byte **)(section_mesh + 0x68);
		long node_index = NONE;
		for (long node = 0; node < node_count; ++node)
		{
			if (nodes[node] == (*(short *)((byte const *)collision + 0x46)))
			{
				node_index = node;
				break;
			}
		}
		if (node_index == NONE) goto done;
		byte *surface_group = *(byte **)(section_groups + 4) + node_index * 16;
		if (*(long *)surface_group != *(long *)(surface_set + 0x10)) goto done;
		s_structure_surface_range *range = (s_structure_surface_range *)*(byte **)(surface_group + 4)
			+ collision->surface_range_index;
		for (long reference_index = range->first; reference_index < range->first + range->count; ++reference_index)
		{
			if (reference_index < 0 || reference_index >= *(long *)(surface_group + 8)) continue;
			s_structure_triangle_reference *reference =
				(s_structure_triangle_reference *)*(byte **)(surface_group + 0xc) + reference_index;
			long surface = reference->surface_index;
			if (surface == NONE) goto done;
			if (surface >= *(long *)surface_set) continue;
			byte *surfaces = *(byte **)(surface_set + 4);
			if (*(word *)(surfaces + surface * 8) != (collision->plane_index & 0x7fff)) continue;
			section_mesh = *(byte **)(section + 0x34);
			word *indices = *(word **)(section_mesh + 0x24) + reference->first_index;
			point3f vertices[3];
			long vertex_node;
			long vertex = 0;
			do
			{
				function_23ac30((s_vertex_source_23ac30 const *)section_mesh, indices[vertex],
					bounds, &vertices[vertex], &vertex_node);
				++vertex;
			} while (vertex < 3);
			if (function_11e800(&vertices[0], &vertices[1], &vertices[2], &point, &triangle->u, &triangle->v))
			{
				triangle->unknown18 = NONE;
				*(long *)((byte *)triangle + 8) = COLLISION_OBJECT_14A8E0;
				triangle->part_index = 0;
				result = true;
				s_16e1b0_list const *parts = (s_16e1b0_list const *)section_mesh;
				for (long part = 0; part < parts->count; ++part)
				{
					s_16e1b0_range const *part_range = &parts->ranges[part];
					if (triangle->part_offset >= part_range->first &&
						triangle->part_offset < part_range->first + part_range->count)
					{
						triangle->part_index = part;
						break;
					}
				}
				triangle->part_offset = reference->first_index;
				triangle->lightmap_part_index = reference->part_index;
				*(long *)((byte *)triangle + 0xc) = model_index;
				*(long *)((byte *)triangle + 0x1c) = (*(short *)((byte const *)collision + 0x44));
				*(long *)((byte *)triangle + 0x2c) = vertex_node;
				goto done;
			}
		}
	}
done:
	return result;
}

#undef COLLISION_OBJECT_14A8E0

/* excluded holds whether the hit object is left out of the model query. Nothing reads its incoming
   value (the only caller passes true); retail keeps the flag in BL and leaves the argument's slot alone. */
// MiniMax G (11-04): the result as a local with the parameter kept alive by an unused pointer 125 and 127;
// the entry order none/probe/flags/result 127; the exclusion flag assigned in the type-6 arm with the
// requery after the join 149; the join test as !(excluded & 1) 138. Body restored to the original.
// score 125 (was 125): frame 0x64 agrees; exclusion BL, NONE/EBX and spilled result allocation still differ.
// Lane 15 r17: the pointers to the direction, ignored object and surface parameters removed (each
// identical code; only the triangle pointer holds the convention: without it 158 and matched caller
// 0xd2a50 lost). Object branch first (`object_index != NONE`, retail's fall-through with `je` to the
// shared lightmap call) 130; with the exclusion set by an if 130; result as a local with the fourth
// parameter unread 131 (0xd2a50 lost); every parameter used plainly 156 (0xd2a50 lost). Retail gives
// ESI to the direction (reloaded after the calls that reuse ESI) and leaves the result in its
// parameter slot; ours gives the result EBX and NONE ESI.
// Earlier: retail keeps the exclusion flag in BL (`mov bl, 1` / `xor bl, bl`, tested again
// after the failed lightmap query), NONE in EBX and the result in its parameter slot; VC threads our
// flag. Flag as a helper out-parameter 168 (kept in memory); early-outs through the availability
// helper 149 (440 bytes, aligned closer); scenery getter call 146 (out of line); function-scope flag
// 125 (identical); early-out helper with an if/else flag 149.
// Existing model and collision probe declarations ordinary/inline/noinline, independently and
// jointly, 125 in all eight forms. Both original declarations restored; their definitions,
// arguments and the entire body are unchanged.
// Round 207: actual skipped/failed/found model state, signed byte/word/long/dword
// in object scope 134/136/133/133; function-scope declarations 134/136/133/133.
// Byte state reaches retail length; prologue/result/NONE allocation remains. All restored.
// Round 426: native byte/signed-byte/short/long retry state 125 each; all four with the existing point setter at retry advance 125 each. Original body restored; exclusion/result/direction allocation remains.
// Round 229: collision object observed at each use 125; complete eligibility observations 216,
// both 216; query success in the actual result 158, with object/eligibility/both 158/233/233;
// all with admitted-geometry result lifetime 267. Full original body restored.
// Lane 15 r19: retail's caller pushes the constant 1 for the third stack parameter and retail never
// reads that slot; retail's result byte lives in the surface parameter's slot (dead once the surface
// pointer is in EBP). Result as a local with the parameter unread 126 (LTCG drops the parameter,
// `ret 0xc`, matched caller 0xd2a50 56), plus `(void)&parameter` 126 (0xd2a50 56); the parameter read in
// the retry test 124 (471 bytes, 0xd2a50 kept; LTCG keeps the read); the two exclusion tests as one
// nested if 125 (same code), with the object branch first 130.
// Round 34 (m3 lane 14, fast): the exclusion flag homed in the unread bool parameter (result a local) as an &&
// expression or if-set 126/126 (LTCG drops the parameter, `ret 0xc`, 0xd2a50 lost; flag still threaded); the
// policy through the existing `function_10a0b0(object_index) != 2` 162 (450 bytes, 0xd2a50 lost); `if
// (!excluded) { if (!0xbf9a0) requery; else 0x14a8e0; }` 130 at 470 bytes (0x14a8e0 laid out last as retail,
// but the structure lookup call is duplicated where retail shares one between the no-object and requery
// paths); both 151; one shared structure lookup under a `found` flag, model query inverted or not 163/164 (448
// bytes, 0xd2a50 lost).
// Round 185 (lane 3, fast): the grouped state replaced by plain flags/attempts/collision locals 157
// (456 bytes, matched caller 0xd2a50 97); one 0x14ac60 call reached through a `found` flag 164 (0xd2a50
// 72); a compiled-out `do {} while (false);` at the requery join 125 (same code: VC drops it before
// threading); the existing 0x10a0b0 called, with `inline` on its definition, 162 (stays out of line,
// 0xd2a50 99); the flag and a `modeled` result set in if/else arms (false and the 0xbf9a0 result in
// the eligible arm) 125 (same code).
// Round 108 (m3 lane 7, fast): the flag as `type == 6 ? policy != 2 : false` 133 (470 bytes); `excluded = false;
// if (type == 6) { short policy = ...; excluded = policy != 2; }` 133 (470 bytes); the scenery test as an empty
// if-arm with the model query and requery as its else-ifs 125 (same code); the attempt limit as a `break` inside
// `if (!result && (bit & 1))` 125 (same code); a named `modeled` bool from `!excluded && 0xbf9a0` 125 (same
// code). Compile-only none of these keeps the flag in BL (VC threads it; a store at the join stops that, see
// 0x14b120's note).
// Round 25 (m3 lane 2, fast): the exclusion named in two parts (`bool scenery = ...; bool excluded = scenery &&
// ...`) 125 (same code); as `object[0xaa] == 6 ? policy != 2 : false` 133 (470 bytes, setne/test, still
// threaded); the retry test as `if (result || !(flags & 1) || ++attempts >= 3) break;` 125; the flag inverted
// (`bool lightmapped = type != 6 || policy == 2`) 125; no flag, the model query and requery nested under
// `if (type != 6 || policy == 2)` 125 (same code: the source's flag is threaded away completely). Diagnostic
// only (it requeries after a failed model lookup, so not keepable): `if (!result && !excluded)` in front of the
// requery keeps retail's `mov bl, 1; jmp` / `test bl, bl`, NONE in EBX with the back edge's `or ebx, -1` and
// the result in its parameter slot in a compile-only screen, but 157 (488 bytes) in the real build.
// Round 61 (m3 lane 9, fast): the object branch first with the probe inverted (`if (!excluded && !0xbf9a0)`
// requery, `else if (!excluded)` 0x14a8e0, the no-object lightmap in the else) 145 (456 bytes); the same
// inverted probe with the no-object case first 130 (470 bytes, retail's length; the 0x14a8e0 arm stays
// inside the loop where retail lays it out after it); the exclusion condition written out at both tests, no
// flag 216 (534 bytes: the policy loads repeat after 0xbf9a0; 0xd2a50 101); one lightmap call through `bool
// found = true;` (cleared for objects, set from the requery) with the model and requery under `if
// (!excluded)` 164 (448 bytes; 0xd2a50 72); the result parameter written through `bool *const
// result_reference = &result;` 125 (same code).
// Round 27 (m3 lane 2, fast): a `found` local (`object_index == NONE`, set by the requery) with one shared
// lightmap call after the object part 157 (455 bytes, matched caller 0xd2a50 67); that with the excluded arm
// clearing `found` 159 (453 bytes, 0xd2a50 72); the flag declared at function scope and cleared before the loop
// 125 (same code); the object part as an if/else-if chain with an empty excluded arm 125 (same code);
// `excluded = false; if (scenery) excluded = policy != 2;` 133 (470 bytes, setne, threaded).
// Round 32 (m3 lane 2, fast; 0xd2a50 in brackets): the surface parameter read once through its address in place
// of the pointer to the triangle local 125 (identical code [kept]); result as a local with the unread parameter
// read into it (a dead read) 126, or dead-written, 126 (LTCG drops the parameter either way, ret 0xc [56]); the
// result reset moved to just before the loop 136; the probe copy before the flags computation 128. Retail never
// reads that stack parameter (orig+0xc, the caller pushes 1) and spills the result into the surface pointer's
// dead slot; only 0xd2a50 calls it.
// Round 39 (m3 lane 15, plain): the flag preset true and cleared by the positive test 125 (same code); the flag set
// from the unread third stack parameter in the scenery arm with result a local 133 (471 bytes: the parameter is
// read and threaded on, no constant propagation); `direction` read once through its address at the join 125
// (folded); a long flag 125; the probe result in a `probed` flag tested before `else if (!excluded)` 125.
// Diagnostic (volatile read of `direction` at the join, reverted): 152 at 480 bytes, but the flag then takes
// retail's shape exactly (`mov bl, 1; jmp`, `xor bl, bl` before 0xbf9a0, &triangle->c in ESI with the direction
// reloaded, result in memory), so the threading of the flag is the one blocker.
// Round 110 (m3 lane 7, fast): the flag through a function-scope `excluded` copied from a `scenery_excluded` &&
// local inside the requery's else-if test (diagnostic: an assignment in a test) 125 (same code); `*probe = *point;`
// first at entry 128; `result = false;` first 130; the collision's NONE store first 130; the probe step through the
// existing `vector3f_add_scaled(probe, direction, 0.02f, probe)` 125 (same code).
// Round 113 (m3 lane 7, fast): the existing `object_get` for the object lookup 125 (same code); the probe copy
// first, then flags, the NONE short and `result = false` 128; the result parameter typed `byte` at the definition only
// 119 at 470 bytes (the caller's `bool` declaration no longer resolves, 0xd2a50 96; result still in BL: an artefact);
// a shared force-inline exclusion helper (object lookup + 0x10a0b0) for this and 0x14b120 125 (0x14b120 106); the
// requery's result as `result = query && lightmap(...)` 174 (505 bytes).
// Round 286: Native collision storage 125; unsigned flags 125; bounded short/byte attempts
// 124/129; native handle observations 125; separate flags/all scalar workspace 157/157
// (caller lost); direct triangle 158 (caller lost). Original storage and executable body restored.
// Round 63 (m3 lane 16, fast): a bit-and join test (`else if (!(excluded & 1))`, diagnostic) keeps the flag
// unthreaded and gives retail's allocation (flag in BL with `mov bl, 1; jmp` and `xor bl, bl` before 0xbf9a0,
// the direction reloaded into ESI, NONE in EBX with `or ebx, -1` at the back edge, the result in its
// parameter slot) but `test bl, 1` and a different layout: 149 (477 bytes); with one lightmap call reached
// through a `found` flag 139 (471 bytes); either with the existing availability helper 148 (470 bytes: the
// failure exit inline as retail's, but ESI pushed after it) / 150; the plain form with `*probe = *point`
// before the flags computation 128.
// Round 221: coordinated byte result parameter and caller declaration 119; independent retry count 126;
// existing initial point setter 142; both 143. Signed-byte parameter on those four graphs
// 119/126/142/143. Byte and signed-byte forms retain the caller at 119, but gain six and fail the
// eight-point gate; setter forms lose the matched caller. Original Boolean parameter, caller
// declaration, grouped storage, point copy and entire body restored. Frame 0x64 agrees; exclusion BL,
// result stack home, direction ESI and NONE ownership remain.
// DH round 264: Result-owned pretested retries with independent flags/long-short-word-byte attempts
// and native collision fields 157/156/156/160; with the existing initial point setter
// 144/145/145/157. All regress; original record layout, grouped state, triangle access and
// body restored; result/NONE/exclusion allocation and direction reloads remain.
// Round 294: Initial probe memmove/three-long triangle memset/two-byte material memset
// subsets 1/2/4/3/5/6/7 give 140/144/125/138/140/144/138; all with the availability
// producer 158. Original body restored; exclusion/result/NONE and direction homes remain.
// Round 298: Finite eligibility/retry/outcome domains and pairs/all
// 125/125/131/125/131/131/131; full finite graph with three-outcome model 164.
// Original Boolean parameter, grouped storage and complete body restored;
// exclusion/result/NONE ownership and direction residency remain.
// Round 300: Complete positive-admission/done-exit availability graphs with native short/long
// policy merges, default/type-arm and conditional producers:
// 136/136/147/147/136/136/147/147. Original complete body restored; exclusion/result/NONE and direction homes remain.
// Round 197 (lane 3, fast with 0xd2a50): a `modeled` flag set in the scenery test's else arm 125; a false probe arm with the
// flag set after 0x14a8e0 125 (same code), as an else-if chain 125; `requery` preset true and cleared in both arms 125, or
// from `!excluded` with the 0x14a8e0 arm clearing it 161 (485 bytes); the excluded result cleared in a nested else 137 (471
// bytes). The join test stays threaded in all of them. Body restored.
// Round 519: Complete direction value shared by both probes and retry advance,
// admitted/per-pass samples with original eligibility 177/165; positive eligibility
// 177/165; existing availability producer 142/147; both changes 142/147.
// All lose the matched caller; original direction observations and complete body restored.
// Round 343: BSP/header/object-handle last-consumer scope subsets 1..7 each 125;
// joint object-phase scopes with the existing availability helper 149 (440 bytes).
// Original availability, workspace, exclusions and body restored; NONE/result/exclusion homes remain.
// Round 125 (lane 8, fast with 0xd2a50): the guards as one positive test around the body with `return false`
// after it 125; `retry = true` before the probe advance 125; `bool retry = true; while (retry)` 125; the
// direction copied to a local for every use (diagnostic) 125; the exclusion preset false and set by an if 125;
// an empty excluded arm with the model query and requery as its else-if/else 125 (same code each: the flag is
// still threaded, the result keeps EBX and the direction no register). Body unchanged.
// Round 138 (m3 lane 5, fast): availability through the existing inline helper 149 (440 bytes), with `result = false`
// after it 169; `result = false` after the inline availability tests 136; the requery flags assigned in the call
// argument 125 (same code); diagnostic: the requery's lightmap call sharing the no-object call through a label 165
// (0xd2a50 lost).
// Round 123 (m3 lane 7, fast with 0xd2a50): both availability guards through the existing inline
// structure_bsp_collision_available() 149 (440 bytes, a repeat re-measured); the flag declared beside `retry` and
// assigned in the object arm 125; both flag tests as `excluded == false` 125; the direction read once into a
// loop-top pointer for both queries and the step 125; the flag set in if/else arms 125; preset false and set
// true under a nested policy test 125 (all the same code: VC still threads the flag).
// DH315: One result exit on original exclusion/storage graph; shared sentinel,
// entry attempt initialization and availability producer masks 0..7 give
// 136/136/144/144/136/136/144/144, all 452/470 bytes. Original exits,
// reset phases, exclusion, grouped workspace and complete body restored.
// Round 81 (m3 lane 9, fast with 0xd2a50): retail holds NONE and then the exclusion flag in EBX (`mov bl, 1` /
// `xor bl, bl`, re-set by `or ebx, -1` at the back edge), the direction in ESI and the result only in its parameter
// slot; ours threads the flag, keeps NONE in ESI and the result in BL. The triangle used directly without the
// reference pointer 158 (454 bytes, 0xd2a50 72); that with the state struct split into three locals 157 (456,
// 0xd2a50 97); the split alone 157 (0xd2a50 97); a named cluster pointer for the 0xbf9a0 output and its read 138
// (473); the exclusion preset false and assigned the definition test inside `if (type == 6)` 133 (470 bytes, setne).
// DL255: Fixed-three forward attempt counters, long/dword/short/word: 122/122/118/118;
// reverse remaining counters: 123/140/119/139. Extents 470/470/470/470/470/460/470/461.
// NOT kept: gains at most seven; narrow counter/layout forms require equality. Frames
// stay 0x64, first +3 and exclusion/result/NONE ownership remain. Original complete
// body, grouped workspace, triangle reference, retry state and query operands restored.
// Zero/one exclusion in signed-byte/short/word compound producers 125/125/125;
// complete branch producers 133/133/133; short/word conditional producers 133/133.
// Original Boolean exclusion, result parameter, grouped storage and complete body restored.
// DH335: collision-selection input char/signed-char/byte 125 each; short/word 126,
// int/long/dword 130. Wider inputs lose the matched caller. Original Boolean
// contract and whole body restored; exclusion/result/NONE and direction homes remain.
// N351: Native output long fields +08/+0c/+10, subsets 1/2/4/3/5/6/7, then model-output pointer:
// 125/125/125/125/125/125/125/124. NOT kept: no exact match or qualifying gain; complete original body restored.
// Remaining: exclusion/result/NONE homes and direction reload.
// Round 162 (lane 4, fast with 0xd2a50). LEAD, not kept (diagnostic forms): one lightmap call reached through a
// `hit` flag (true, cleared in the object arm, set from the requery, `if (hit) result = lightmap`) gives retail's shared
// `test al, al; mov [slot], al` tail and the result in memory, 164 at 448 bytes while the flag threads (0xd2a50 lost,
// probe in ESI). With the join test as `!(excluded & 1)` (or `excluded ^ 1`) 139 at 471 bytes and retail's allocation;
// plus the existing structure_bsp_collision_available() 150 at 461, entry order result, NONE short, probe copy, flags 112
// (n,p,f,r 112; p,n,r,f 113; p,f,r,n 130; f,p,r,n 150); plus result a local and the unread parameter kept by an unused
// `bool const *flag_reference = &parameter` 59 at 470 bytes, aligned 28, 0xd2a50 matching (a plain address cast, or an
// empty inline taking it, drops the parameter: ret 0xc, 114). Only three things then differ: the NONE short's store
// before the result's, `test bl, 1` for `test bl, bl`, and the found arm, inline in ours and after the step in retail.
// So what is left is the join test staying unthreaded with a plain `test bl, bl`, and the found arm's place.
// Threaded on that 59 form (167, 432 bytes; aligned 155): join test as `&` or `|` of itself, `&& itself`, `(long)` == 0, > 0,
// a reference, a pointer or a copy of the flag at the join, `while`, `do/while(false)`, `for` or `switch` around
// the requery, an empty inline before it, a known-false `hit` or `retry` as the compound's first element or second;
// `-(long)excluded == 0` stays unthreaded 65 [24]. On the kept body: a statement in the else before the test 125; result
// local 126 (parameter dropped, 0xd2a50 lost); the flag in the unread parameter 126 (dropped); `found` set in arms or
// from the && test, then if/else-if, 125 each; the requery and found call in an inline taking the flag 155 at 486 bytes
// (first test stays, second threads); negative dispatch with the found arm in the else 125, with `hit` 164; 16-128
// unused locals 164 (no tracking limit); the found arm at a label after the step (goto, diagnostic) 59 (ours moves it
// back inline); `found` tested after the lightmap call with `!found && !excluded` 182 at 428 (found arm after the
// lightmap, jumped over; the flag still threads), that with found before lightmap 174, as else-if 182, the flag set by if 183;
// for(;;) with breaks on the 112 form 144 (505 bytes).
// N369: Pass-owned false-result initialization with independent availability,
// generation and retry scopes, masks 0..7: 146/146/146/171/146/146/146/171.
// NOT kept: all lose the matched caller (14), regress and miss native extent;
// original entry-result lifetime, guards and complete body restored.
// Lane 5 r149 (held, coordinator): MATCH with the parameter renamed `unused`, a fresh `bool result`, a dead
// `bool const *unused_reference = &unused;` (retail never reads it, but its slot is address-taken), surface read once
// through its address, the BSP test through structure_bsp_collision_available, and the object path as a `hit` flag
// with the requery under `!(excluded | found)`. Without the dead address line 109 (fast). Not kept: a parameter-address
// line that is never read is reverted even with a match (10-19 ruling). Patch build/held/lane5-r149-14af40-unused-address.patch.
// N378: Complete object completion chain with eligibility/model/fallback results in the actual return result:
// 181/133/181/133/200/147/200/147. NOT kept: no byte equality or qualifying native-shape gain;
// original executable source restored; exclusion/result/NONE and direction homes remain.
// Lane 12 r92: matches (0) with `bool const *unused_reference = &unused; __assume(*unused_reference);` plus
// `__assume(!found)` in the !found arm, hit/found flags and plain locals; held, not kept: the assume on the
// unread parameter states a value nobody checks (patch build/held/lane12-r92-14af40-assume-unused.patch).
// T60: Overwritten result-parameter char/short/word/int/long/dword/unsigned-int domains
// 119/115/115/118/118/118/118; independent long result 120 (caller lost). NOT kept:
// wider result operations lack native byte storage/equality; other gains below margin.
// Original Boolean contract/body restored; exclusion/result/NONE and direction homes remain.
// Round 216 (m3 lane 3, fast with 0xd2a50): the existing availability helper with the entry order NONE short, probe
// copy, flags, result 151 (440 bytes); that entry order alone 127; the flag in two steps (type test, then `excluded =
// policy != 2` under it) 155 (480 bytes, sete/setne); the first query as the `while` condition with one `break` retry
// test 155 (480 bytes, 0xd2a50 lost); the flag as a byte member of the state aggregate 152 (483 bytes). Body restored.
// @retail 0x14af40
bool function_14af40(bool include_objects, point3f const *point, vector3f const *direction,
    long ignore_object_index, bool excluded, void *surface, point3f *probe)
{
    s_structure_lightmap_triangle *triangle = (s_structure_lightmap_triangle *)surface;
    long flags;
    long attempts;
    byte collision[0x5c];
    *(short *)(collision + 0x24) = NONE;
    bool result = false;
    *probe = *point;
    flags = include_objects ? 0x4480000d : 0x4800c0d;
    if (!structure_bsp_collision_available()) return false;
    triangle->cluster_index = NONE;
    triangle->instance_index = NONE;
    *(long *)((byte *)triangle + 8) = NONE;
    attempts = 0;
    bool retry;
    do
    {
        retry = false;
        if (!function_1697c0(flags, probe, direction, ignore_object_index, NONE,
            (s_collision_result_1697c0 *)collision)) break;
        long object_index = *(long *)(collision + 0x40);
        bool hit = true;
        if (object_index != NONE)
        {
            byte *object = *(byte **)(g_4e0300->data + (object_index & 0xffff) * 12 + 8);
            excluded = object[0xaa] == 6 && *(short *)(g_4e3b44[*(long *)object & 0xffff].bytes + 0xc0) != 2;
            hit = false;
            bool found = !excluded && function_bf9a0(object_index, (long *)((byte *)triangle + 0xc),
                (long *)((byte *)triangle + 0x10));
            if (!found)
            {
                if (!(excluded | found))
                {
                    flags = 0x4800005;
                    hit = function_1697c0(flags, probe, direction, NONE, NONE,
                        (s_collision_result_1697c0 *)collision);
                }
            }
            else
                result = function_14a8e0((s_structure_collision_result *)collision,
                    *(long *)((byte *)triangle + 0xc), triangle);
        }
        if (hit)
            result = structure_get_lightmap_triangle((s_structure_collision_result *)collision, triangle);
        if (!result && (collision[0x58] & 1) && ++attempts < 3)
        {
            probe->x += direction->i * 0.02f;
            probe->y += direction->j * 0.02f;
            probe->z += direction->k * 0.02f;
            retry = true;
        }
    } while (retry);
    return result;
}
